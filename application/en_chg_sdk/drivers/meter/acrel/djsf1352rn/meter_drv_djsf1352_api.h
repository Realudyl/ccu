/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   meter_drv_djsf1352_api.h
* Description                           :   安科瑞电表通讯 驱动 api接口函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   具体型号为:DJSF1352-RN 导轨式直流电能表
*                                           使用modbus协议,而不是DLT645规约
****************************************************************************************************/
#ifndef _meter_drv_djsf1352_api_h_
#define _meter_drv_djsf1352_api_h_
#include "meter_drv_djsf1352.h"












//定义电表地址的基数是1---多电表时，地址要按1,2,3,...这样依次设置并与枪序号对应
#define cMeterDrvDjsf1352rnAddrBase     1

















//电表数据字段的读写接口函数
extern bool sMeterDrvDjsf1352rnGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
extern bool sMeterDrvDjsf1352rnGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag);
extern bool sMeterDrvDjsf1352rnGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter);
extern bool sMeterDrvDjsf1352rnGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
extern bool sMeterDrvDjsf1352rnGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio);
extern bool sMeterDrvDjsf1352rnGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated);
extern bool sMeterDrvDjsf1352rnGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts);
extern bool sMeterDrvDjsf1352rnGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts);
extern bool sMeterDrvDjsf1352rnGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);
extern bool sMeterDrvDjsf1352rnGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay);
extern bool sMeterDrvDjsf1352rnGetRate(ePileGunIndex_t eGunIndex, u8 *pRate);
extern bool sMeterDrvDjsf1352rnGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer);

extern bool sMeterDrvDjsf1352rnGetFault_MCF(void);











#endif







