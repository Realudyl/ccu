 /*
  * FreeModbus Libary: RT-Thread Port
  * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id: port.c,v 1.60 2015/02/01 9:18:05 Armink $
  */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "FreeRTOS.h"
#include "task.h"
#include "en_log.h"
#include "en_shell.h"

//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "PORT_PORT";
/* ----------------------- Variables ----------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
void EnterCriticalSection(void)
{
    //taskENTER_CRITICAL();
}

void ExitCriticalSection(void)
{
    //taskEXIT_CRITICAL();
}

void assert_failed(char *file, int line)
{
    // 输出错误信息，可以根据实际需求输出到日志、控制台或其他输出设备
    EN_SLOGI(TAG, "Assertion failed in file %s on line %d\n", file, line);

    // 根据系统需求决定是进入死循环、复位系统或其他处理
    // 在嵌入式系统中可能需要停机或其他安全措施
    while (1)
    {
        // 可以在这里添加LED闪烁或其他指示错误的代码
    }
}
