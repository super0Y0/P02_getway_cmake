#include "FreeRTOS.h"   
#include "cmsis_os2.h"
#include "Int_MQTT.h"
#include "cJSON.h"
#include "Modbus.h"
#include "com_debug.h"


void MQTT_msg_callback(MessageData *msg_data)
{
    // 解析json字符串
    cJSON *root = cJSON_ParseWithLength((char *)msg_data->message->payload, msg_data->message->payloadlen);
    
if (root == NULL) {
    debug_printf("json parse error\r\n");
    /*const char *err = cJSON_GetErrorPtr();
        if (err != NULL) {
            // 1. 打印错误位置的字符（就是逗号 0x2C）
            debug_printf("Char at error: 0x%02X\r\n", (unsigned char)*err);
            
            // 2. 【关键】打印逗号前面的 1 个字节
            if (err > (const char*)msg_data->message->payload) {
                debug_printf("Char before error: 0x%02X\r\n", (unsigned char)*(err - 1));
            }
            // 3. 【关键】打印逗号前面的 2 个字节
            if (err - 1 > (const char*)msg_data->message->payload) {
                debug_printf("Two chars before error: 0x%02X\r\n", (unsigned char)*(err - 2));
            }
            
            // 4. 打印错误位置前后 15 个字节的完整 Hex
            int offset = err - (const char*)msg_data->message->payload;
            int start = offset - 10;
            if (start < 0) start = 0;
            int end = offset + 5;
            if (end > msg_data->message->payloadlen) end = msg_data->message->payloadlen;
            
            debug_printf("Hex around error: ");
            for (int i = start; i < end; i++) {
                debug_printf("%02X ", ((unsigned char*)msg_data->message->payload)[i]);
            }
            debug_printf("\r\n");
        }*/
    return;
}

    // 2. 判断连接类型是否为rs485 并且从设备id是否为5
    cJSON *connection_type = cJSON_GetObjectItem(root, "type");
    cJSON *device_id = cJSON_GetObjectItem(root, "id");

    if (cJSON_IsString(connection_type) && cJSON_IsNumber(device_id))
    {
        // 判断类型是正确的
        if (strcmp(connection_type->valuestring, "rs485") == 0 && device_id->valueint == 5)
        {
            // 连接类型和从设备id正确
            debug_printf("connection_type and device_id is correct\r\n");
        }
        else
        {
            debug_printf("connection_type or device_id is not correct\r\n");
            cJSON_Delete(root);
            return;
        }
    }
    else
    {
        debug_printf("connection_type or device_id is not string or number\r\n");
        cJSON_Delete(root);
        return;
    }
    // 发送modbus485指令给电机
    cJSON *motor_status = cJSON_GetObjectItem(root, "status");
    if (cJSON_IsBool(motor_status) && motor_status->valueint == 1)
    {   debug_printf("需要开机\r\n");
        // 启动电机 => 判断设置的圈数
        cJSON *motor_set_num = cJSON_GetObjectItem(root, "st");
        if (cJSON_IsNumber(motor_set_num))
        {
            debug_printf("需要开机转 %d 圈\r\n", motor_set_num->valueint);
            // 发送指令给电机:
            // 注意顺序!从机在收到"开机线圈"的瞬间就会读取当前圈数寄存器去执行,
            // 所以必须先写圈数寄存器,再写开机线圈触发,否则电机用的还是上一条的圈数。
            // ① 先写圈数寄存器
            Modbus_Write_Single_Reg(0x05, 2, motor_set_num->valueint + 500);
            // ② 后写开机线圈 => 触发电机按刚写入的圈数转动 (写ON=0xFF00)
            Modbus_Write_Coil(0x05, 2, 0xff);
        }
    }
    else if (cJSON_IsBool(motor_status) && motor_status->valueint == 0)
    {
        debug_printf("需要停机\r\n");
        // 停止电机: 把开机线圈写OFF(0x0000), 从站收到下降沿(1->0)立即停止
        Modbus_Write_Coil(0x05, 2, 0x00);
    }

    //读取当前电机状态
    Modbus_Read_Coil(0x05, 2, 1);//停止/启动
    Modbus_Read_Discrete(0x05, 3, 1);//转向
    Modbus_Read_Single_Reg(0x05, 2, 1);//当前圈数

     // 释放内存
    // delete删除结构体
    cJSON_Delete(root);
    //  释放字符串
    // cJSON_free();
}

void StartMQTTTask(void *argument)
{
  /* USER CODE BEGIN StartMQTTTask */
  //注册MQTT收到消息之后处理消息的回调函数
  Int_Reg_MQTT_Callback(MQTT_msg_callback);
  Int_MQTT_Init();
  
  /* Infinite loop */
  for(;;)
  {
    // 联网  使用w5500官方库
    Int_MQTT_ReceiveMsg();
    osDelay(100);
  }
  /* USER CODE END StartMQTTTask */
}
