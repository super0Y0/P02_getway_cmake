#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "usart.h"
#include "com_debug.h"
#include "mbcrc.h"

void Modbus_Write_Coil(uint8_t slave_addr, uint16_t start_addr, uint8_t data);
void Modbus_Write_Single_Reg(uint8_t slave_addr, uint16_t start_addr, uint16_t data);




#endif
