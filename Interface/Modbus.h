#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "usart.h"
#include "com_debug.h"
#include "mbcrc.h"
#include "cJSON.h"
#include "Int_mqtt.h"

// 电机状态结构体：保存从站返回的运行状态，用于 MQTT 回传
typedef struct{
    uint8_t state;        //停止/启动状态 (来自线圈[2])
    uint8_t dir;          //转向 (来自离散量[3])
    int16_t set_nums;     //设置圈数 (来自保持寄存器[2], 含500偏移)
}Motor_State;

extern Motor_State motor_state;  //电机状态
extern uint8_t full_flag;        //状态收集完成标志

void Modbus_Write_Coil(uint8_t slave_addr, uint16_t start_addr, uint8_t data);
void Modbus_Write_Single_Reg(uint8_t slave_addr, uint16_t start_addr, uint16_t data);
void Modbus_Read_Single_Reg(uint8_t slave_addr, uint16_t start_addr, uint16_t num);
void Modbus_Read_Coil(uint8_t slave_addr, uint16_t start_addr, uint8_t num);
void Modbus_Read_Discrete(uint8_t slave_addr, uint16_t start_addr, uint16_t num);
void Modbus_Analysis_Response(uint8_t *data, uint16_t len);



#endif
