#include "Modbus.h"
#include "freertos.h"
#include "cmsis_os2.h"

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
