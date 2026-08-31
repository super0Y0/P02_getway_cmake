#include "Int_MQTT.h"
#include "cmsis_os2.h"

uint8_t gar[4] = {192, 168, 1, 1};
uint8_t sub[4] = {255, 255, 255, 0};
uint8_t mac[6] = {110, 120, 13, 10, 15, 160};
uint8_t ip[4] = {192, 168, 1, 77};

uint8_t server_ip[4] = {192, 168, 1, 12};
uint16_t server_port = 1883;

MQTTClient mqttClient;
Network network;
uint8_t mqtt_send_buff[1024] = {0};
uint8_t mqtt_recv_buff[1024] = {0};

#define PULL_TOPIC "pull_yy"
#define PUSH_TOPIC "push_yy"

messageHandler mqtt_callback;

void Int_Reg_MQTT_Callback(messageHandler callback)
{
    mqtt_callback = callback;
}


void Int_ETH_Reset(void)
{
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
  osDelay(10);

    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    osDelay(10);
}

void Int_ETH_Init(void)
{
    // 1. 重启W5500芯片
    Int_ETH_Reset();

    // 2. 注册SPI函数
    user_reg_function();

    // 3. 配置W5500的网络参数
    // 3.1 配置IP地址、子网掩码、网关、MAC地址
    setGAR(gar);
    setSUBR(sub);
    setSHAR(mac);
    setSIPR(ip);
}

void Int_MQTT_Init(void)
{
    int result = 0xff;
    // 1. 对网络进行初始化
    Int_ETH_Init();

    osDelay(3000);

    // 2. socket编程
    // 2.1 创建一个socket
    NewNetwork(&network, 0);

    // 2.2 连接服务器
    result = ConnectNetwork(&network, server_ip, 1883);
    if (result != SOCK_OK)
    {
        debug_printf("ConnectNetwork failed\r\n");
        return;
    }
    debug_printf("ConnectNetwork success\r\n");
    // 2.3 使用连接完成的套接字创建MQTT客户端
    MQTTClientInit(&mqttClient, &network, 3000, mqtt_send_buff, 1024, mqtt_recv_buff, 1024);

    // 2.4 填写连接的参数
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    // 修改参数
    data.MQTTVersion = 3;
    data.willFlag = 0;
    data.keepAliveInterval = 60;
    data.clientID.cstring = "STM32_GW_001";
    data.cleansession = 1;
    data.username.cstring = NULL;
    data.password.cstring = NULL;
    // 2.5 连接MQTT服务器
    result = MQTTConnect(&mqttClient, &data);
    if (result != SUCCESSS)
    {
        printf("MQTTConnect failed\r\n");
        return;
    }
    printf("MQTTConnect success\r\n");

    // 3. 订阅主题
    // 订阅完成之后 如果有新的消息发布 服务端会自动推送给客户端
    // 推送的消息处理 => 回调函数
    result = MQTTSubscribe(&mqttClient, PULL_TOPIC, QOS0, mqtt_callback);
    if (result != SUCCESSS)
    {
        debug_printf("MQTTSubscribe failed, ret=%d\r\n", result);
        return;
    }
    debug_printf("MQTTSubscribe success, topic=%s\r\n", PULL_TOPIC);
}

void Int_MQTT_ReceiveMsg(void)
{
    // MQTT客户端接收消息
    MQTTYield(&mqttClient, 1000);
}

void Int_MQTT_SendMsg(uint8_t *data, uint16_t len)
{
    MQTTMessage message;
    message.qos = QOS0;
    message.retained = 0;
    message.dup = 0;
    message.payload = data;
    message.payloadlen = len;

    MQTTPublish(&mqttClient, PUSH_TOPIC, &message);
}
