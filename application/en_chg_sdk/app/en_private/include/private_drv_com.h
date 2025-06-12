/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_com.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-12
* notice                                :     
****************************************************************************************************/
#ifndef _private_drv_com_H_
#define _private_drv_com_H_

#if (cSdkPrivDevType == cSdkPrivDevTypeS)
#include "usart_drv_trans.h"
#endif

#include "en_common.h"
#include "private_drv_frame_def.h"
















//串口端口定义
#ifndef cSdkPrivDrvComUartNum
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
#define cPrivDrvComUartNum              (UART_NUM_1)
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
#define cPrivDrvComUartNum              (eUsartNum2)
#endif
#else
#define cPrivDrvComUartNum              (cSdkPrivDrvComUartNum)
#endif


//串口波特率定义
#ifndef cSdkPrivDrvComBaudrate
#define cPrivDrvComBaudrate             (115200)
#else
#define cPrivDrvComBaudrate             (cSdkPrivDrvComBaudrate)
#endif


//串口发送脚定义
#ifndef cSdkPrivDrvComTxPin
#define cPrivDrvComTxPin                (32)
#else
#define cPrivDrvComTxPin                (cSdkPrivDrvComTxPin)
#endif


//串口接收脚定义
#ifndef cSdkPrivDrvComRxPin
#define cPrivDrvComRxPin                (34)
#else
#define cPrivDrvComRxPin                (cSdkPrivDrvComRxPin)
#endif


//串口发送缓冲区大小
#ifndef cSdkPrivDrvComTxBuffSize
#define cPrivDrvComTxBuffSize           (1 * 1024)
#else
#define cPrivDrvComTxBuffSize           (cSdkPrivDrvComTxBuffSize)
#endif


//串口接收缓冲区大小
#ifndef cSdkPrivDrvComRxBuffSize
#define cPrivDrvComRxBuffSize           (2 * 1024)
#else
#define cPrivDrvComRxBuffSize           (cSdkPrivDrvComRxBuffSize)
#endif





//桩端通讯接收异常检查使能
#ifndef cSdkPrivDrvRecvErrCheckEn
#define cPrivDrvRecvErrCheckEn          (1)
#else
#define cPrivDrvRecvErrCheckEn          (cSdkPrivDrvRecvErrCheckEn)
#endif





//桩端通讯接收异常检查复位次数(接收异常检查使能才有效,0不复位)
#ifndef cSdkPrivDrvRecvErrRebootCnt
#define cPrivDrvRecvErrRebootCnt        (20)
#else
#define cPrivDrvRecvErrRebootCnt        (cSdkPrivDrvRecvErrRebootCnt)
#endif



//桩端通讯接收异常复位参数使能(接收异常检查使能才有效)
#ifndef cSdkPrivDrvRecvErrResetEn
#define cPrivDrvRecvErrResetEn          (1)
#else
#define cPrivDrvRecvErrResetEn          (cSdkPrivDrvRecvErrResetEn)
#endif



//桩端通讯接收延时时间(ms)
#ifndef cSdkPrivDrvRecvDelay
#define cPrivDrvRecvDelay               (0)
#else
#define cPrivDrvRecvDelay               (cSdkPrivDrvRecvDelay)
#endif



//桩端通讯打印使能
#ifndef cSdkPrivDrvPrintfEn
#define cPrivDrvComPrintfEn             (0)
#else
#define cPrivDrvComPrintfEn             (cSdkPrivDrvPrintfEn)
#endif




//串口数据接收任务参数
#ifndef cSdkPrivComRecvTaskStackSize
#define cPrivComRecvTaskStackSize       (3 * 1024)
#else
#define cPrivComRecvTaskStackSize       (cSdkPrivComRecvTaskStackSize)
#endif

#ifndef cSdkPrivComRecvTaskPrio
#define cPrivComRecvTaskPrio            (23)
#else
#define cPrivComRecvTaskPrio            (cSdkPrivComRecvTaskPrio)
#endif





//串口接收扫描使能
#ifndef cSdkPrivDrvRxScanEn
#define cPrivDrvRxScanEn                  (0)
#else
#define cPrivDrvRxScanEn                  (cSdkPrivDrvRxScanEn)
#endif






bool sPrivComInit(void);
bool sPrivComSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag);










#endif









