/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv_trans.h
* Description                           :   串口驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-27
* notice                                :   
****************************************************************************************************/
#ifndef _usart_drv_trans_H_
#define _usart_drv_trans_H_
#include "usart_drv.h"

































extern bool sUsartDrvOpen(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg, i32 i32RxRingBufSize, i32 i32TxRingBufSize);
extern i32  sUsartDrvRecv(eUsartNum_t eUsartNum, i32 i32MaxLen, u8 *pBuf);
extern bool sUsartDrvSend(eUsartNum_t eUsartNum, i32 i32Len, const u8 *pBuf);
extern void sUsartDrvSendCmBackTrace(eUsartNum_t eUsartNum, u16 u16Data);




#endif




















