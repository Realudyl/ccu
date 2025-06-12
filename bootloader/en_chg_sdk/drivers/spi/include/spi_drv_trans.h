/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   spi_drv_trans.h
* Description                           :   SPI驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-07
* notice                                :   
****************************************************************************************************/
#ifndef _spi_drv_trans_H_
#define _spi_drv_trans_H_
#include "spi_drv.h"





//dummy send时 发送此字节
#define cSpiDrvDummySendData            (0xA5)


//传输单个数据的超时时间 当前实际传输时间约0.8us
#define cSpiDrvTransDataTimeoutCnt      (200)                                   //200 实测约1.7us





















extern bool sSpiDrvOpen(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg);
extern bool sSpiDrvRecv(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf);
extern bool sSpiDrvSend(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf);
extern void sSpiDrvSetCs(eSpiNum_t eSpiNum, bool bOnOff);




#endif























