#include "Modbus.h"
#include "cmsis_os2.h"
#include <string.h>

Motor_State motor_state = {0xff, 0xff};

uint8_t full_flag = 0;

void Modbus_Send(uint8_t *data, uint16_t len){
HAL_UART_Transmit(&huart2,data, len,1000);
osDelay(1000);
}



/**
 * @brief 写线圈函数
 * 
 * @param slave_addr 从设备地址
 * @param start_addr 起始地址
 * @param num 设定数值————
 */

void Modbus_Write_Coil(uint8_t slave_addr, uint16_t start_addr, uint8_t data)
{
    uint8_t buf[8] = {0};
    uint8_t index = 0;

    // 健壮性判断
    if (slave_addr == 0 || slave_addr > 247)
    {
        debug_printf("slave_addr error");
        return;
    }

    if (start_addr > 9)
    {
        debug_printf("start_addr error");
        return;
    }

    // 写入命令
    // 写入从设备地址
    buf[index++] = slave_addr;
    // 写入功能码
    buf[index++] = 0x05;
    // 写入起始地址
    buf[index++] = (uint8_t)(start_addr >> 8);
    buf[index++] = (uint8_t)(start_addr & 0x00ff);
    // 写入数据
    buf[index++] = data;
    buf[index++] = 0;

    // 添加crc校验
    uint16_t crc = usMBCRC16(buf, index);

    buf[index++] = (uint8_t)(crc);
    buf[index++] = (uint8_t)(crc >> 8);
 
    //发送命令
    Modbus_Send(buf, index);
}
/**
 * @brief 读线圈函数
 * 
 * @param slave_addr 从设备地址
 * @param start_addr 起始地址
 * @param data 数据
 */
void Modbus_Read_Coil(uint8_t slave_addr, uint16_t start_addr, uint8_t num)
{
    uint8_t buf[8] = {0};
    uint8_t index = 0;

    // 健壮性判断
    if (slave_addr == 0 || slave_addr > 247)
    {
        debug_printf("slave_addr error");
        return;
    }

    if (start_addr > 9)
    {
        debug_printf("start_addr error");
        return;
    }

    // 写入命令
    // 写入从设备地址
    buf[index++] = slave_addr;
    // 写入功能码
    buf[index++] = 0x01;
    // 写入起始地址
    buf[index++] = (uint8_t)(start_addr >> 8);
    buf[index++] = (uint8_t)(start_addr & 0x00ff);
    // 写入数据
    buf[index++] = (uint8_t)(num >> 8);
    buf[index++] = (uint8_t)(num & 0x00ff);

    // 添加crc校验
    uint16_t crc = usMBCRC16(buf, index);

    buf[index++] = (uint8_t)(crc);
    buf[index++] = (uint8_t)(crc >> 8);
 
    //发送命令
    Modbus_Send(buf, index);
}


//写保存寄存器
void Modbus_Write_Single_Reg(uint8_t slave_addr, uint16_t start_addr, uint16_t data)
{
    uint8_t buf[8] = {0};
    uint8_t index = 0;
    //健壮性判断
    if (slave_addr == 0 || slave_addr > 247)
    {
        debug_printf("slave_addr error");
        return;
    }

    if (start_addr > 9)
    {
        debug_printf("start_addr error");
        return;
    }

    
    buf[index++] = slave_addr;
   
    buf[index++] = 0x06;
    
    buf[index++] = (uint8_t)(start_addr >> 8);
    buf[index++] = (uint8_t)(start_addr & 0x00ff);

    buf[index++] = (uint8_t)(data >> 8);
    buf[index++] = (uint8_t)(data & 0x00ff);
  
    uint16_t crc = usMBCRC16(buf, index);
    buf[index++] = (uint8_t)(crc);
    buf[index++] = (uint8_t)(crc >> 8);

   
    Modbus_Send(buf, index);
}

//读保存寄存器
void Modbus_Read_Single_Reg(uint8_t slave_addr, uint16_t start_addr, uint16_t num)
{
    uint8_t buf[8] = {0};
    uint8_t index = 0;
    //健壮性判断
    if (slave_addr == 0 || slave_addr > 247)
    {
        debug_printf("slave_addr error");
        return;
    }

    if (start_addr > 9)
    {
        debug_printf("start_addr error");
        return;
    }

    
    buf[index++] = slave_addr;
   
    buf[index++] = 0x03;
    
    buf[index++] = (uint8_t)(start_addr >> 8);
    buf[index++] = (uint8_t)(start_addr & 0x00ff);

    buf[index++] = (uint8_t)(num >> 8);
    buf[index++] = (uint8_t)(num & 0x00ff);
  
    uint16_t crc = usMBCRC16(buf, index);
    buf[index++] = (uint8_t)(crc);
    buf[index++] = (uint8_t)(crc >> 8);

   
    Modbus_Send(buf, index);
}

void Modbus_Read_Discrete(uint8_t slave_addr, uint16_t start_addr, uint16_t num){
    
    uint8_t buf[8] = {0};
    uint8_t index = 0;
    //健壮性判断
    if (slave_addr == 0 || slave_addr > 247)
    {
        debug_printf("slave_addr error");
        return;
    }

    if (start_addr > 9)
    {
        debug_printf("start_addr error");
        return;
    }

    
    buf[index++] = slave_addr;
   
    buf[index++] = 0x02;
    
    buf[index++] = (uint8_t)(start_addr >> 8);
    buf[index++] = (uint8_t)(start_addr & 0x00ff);

    buf[index++] = (uint8_t)(num >> 8);
    buf[index++] = (uint8_t)(num & 0x00ff);
  
    uint16_t crc = usMBCRC16(buf, index);
    buf[index++] = (uint8_t)(crc);
    buf[index++] = (uint8_t)(crc >> 8);

   
    Modbus_Send(buf, index);
}
void Modbus_Analysis_Response(uint8_t *data, uint16_t len)
{
    // 1. 使用CRC校验返回的消息
    uint16_t crc = usMBCRC16(data, len - 2);
    if (crc != ((uint16_t)data[len - 2] | data[len - 1] << 8))
    {
        debug_printf("crc error");
        return;
    }

    // 2. 判断从设备地址
    if (data[0] != 0x05)
    {
        debug_printf("slave_addr error");
        return;
    }

    // 3. 判断功能码
    if (data[1] == 0x01)
    {
        full_flag |= 1;
        // 读取的线圈返回了
        motor_state.state = data[3];
    }

    if (data[1] == 0x02)
    {
        full_flag |= 2;
        // 读取的离散量返回了
        motor_state.dir = data[3];
    }

    // 读取设置的圈数
    if (data[1] == 0x03)
    {
        full_flag |= 4;
        motor_state.set_nums = (data[3] << 8 | data[4]) - 500;
    }


    // 判断数据是否收集完成
    // 无编码器:只需收集 线圈(1)+离散量(2)+圈数(4) = 7 即可回传
    if (full_flag == 7)
    {
        full_flag = 0;
        debug_printf("电机的状态为:%d,电机的方向为%d,设置的圈数:%d,\n", motor_state.state, motor_state.dir, motor_state.set_nums);

        // 将消息发送给mqtt
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "state", motor_state.state);
        cJSON_AddNumberToObject(root, "dir", motor_state.dir);
        cJSON_AddNumberToObject(root, "set_nums", motor_state.set_nums);

        char * cjson_str = cJSON_Print(root);

        Int_MQTT_SendMsg((uint8_t *)cjson_str, strlen(cjson_str));

        cJSON_Delete(root);
        cJSON_free(cjson_str);
    }
}
