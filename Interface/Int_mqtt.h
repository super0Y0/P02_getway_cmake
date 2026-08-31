#ifndef __INT_MQTT__
#define __INT_MQTT__

#include "socket.h"
#include "wizchip_conf.h"
#include "MQTTClient.h"
#include "Com_debug.h"
#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief 初始化以太网
 * 
 */
void Int_ETH_Init(void);

/**
 * @brief 创建MQTT客户端
 * 
 */
void Int_MQTT_Init(void);

/**
 * @brief 处理消息
 * 
 */
void Int_MQTT_ReceiveMsg(void);


/**
 * @brief 发布消息 => 
 * 
 * @param data 
 * @param len 
 */
void Int_MQTT_SendMsg(uint8_t *data, uint16_t len);


/**
 * @brief 注册MQTT收到消息之后处理消息的回调函数
 * 
 * @param callback 
 */
void Int_Reg_MQTT_Callback(messageHandler callback);

#endif // __INT_MQTT__
