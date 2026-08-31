#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "Modbus.h"

void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  
  /* Infinite loop */
  for(;;)
  {
   //Modbus_Write_Coil(0x05, 2, 0xff);

    osDelay(1000); 
  }
  /* USER CODE END StartTask02 */
}