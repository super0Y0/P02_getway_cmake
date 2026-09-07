#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "Modbus.h"

osThreadId_t Modbus_Handle;   //接收任务句柄，供中断回调通知使用

//接收缓冲与长度需要中断回调与接收任务共用，故定义为文件级全局
static uint8_t Modbus_recv_buff[32] = {0};
static uint16_t Modbus_recv_len = 0;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 1. 判断是否为串口2
    if (huart->Instance == USART2)
    {
        Modbus_recv_len = Size;
        Modbus_recv_buff[Size] = '\0';
        // 2. 通知接收任务（osThreadFlagsSet 可安全用于中断回调）
        osThreadFlagsSet(Modbus_Handle, 0x01);
    }
}

void StartRecive02(void *argument)
{
  /* USER CODE BEGIN StartRecive02 */
  Modbus_Handle = osThreadGetId();   //记录本任务句柄,供中断回调通知
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)Modbus_recv_buff, sizeof(Modbus_recv_buff) - 1);
  /* Infinite loop */
  for(;;)
  {
    //等待中断通知：收到一帧Modbus响应
    osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

    //打印收到的原始数据（调试用）
    for (uint16_t i = 0; i < Modbus_recv_len; i++)
    {
        printf("%02x ", Modbus_recv_buff[i]);
    }
    printf("\n");
    //解析modbus的返回数据
    Modbus_Analysis_Response(Modbus_recv_buff, Modbus_recv_len);

    //解析完成后重新启动接收，等待下一帧
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)Modbus_recv_buff, sizeof(Modbus_recv_buff) - 1);
  }
  /* USER CODE END StartRecive02 */
}