/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   meter_drv_djsf2669_api.c
* Description                           :   青岛英利达电表驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   具体型号为:DJSF2669D 型双回路导轨式直流电能表
*                                           使用modbus-rtu协议,而不是DLT645规约
****************************************************************************************************/
#ifndef _meter_drv_djsf2669_api_h_
#define _meter_drv_djsf2669_api_h_
#include "meter_drv_djsf2669.h"















//定义电表地址的基数是1---多电表时，地址要按1,2,3,...这样依次设置并与枪序号对应
#define cMeterDrvDjsf2669dAddrBase      1
















//DJSF2669D电表数据字段的读写接口函数
extern bool sMeterDrvDjsf2669dGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
extern bool sMeterDrvDjsf2669dGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
extern bool sMeterDrvDjsf2669dGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);

extern bool sMeterDrvDjsf2669dSetClearE(ePileGunIndex_t eGunIndex);

extern bool sMeterDrvDjsf2669dGetFault_MCF(void);







#endif


































