#ifndef __COM_DEBUG__
#define __COM_DEBUG__

#include "usart.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE

// 修改输出文件的全路径名称 => 只有文件名
#define __FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define __FILENAME__ (strrchr(__FILENAME, '\\') ? strrchr(__FILENAME, '\\') + 1 : __FILENAME)

// 调用宏定义打印的时候 => 需要打印出日志输出的文件和行号
#define debug_printf(format, ...) printf("[%s:%d] " format "\r\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

#else
#define debug_printf(format, ...)

#endif // DEBUG_ENABLE

/**
 * @brief 日志输出初始化
 *
 */
void Com_debug_init(void);

#endif // __COM_DEBUG__
