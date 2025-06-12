/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :     meter_drv_opt.h
* Description                           :     电表驱动
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2022-08-09
* notice                                :     
****************************************************************************************************/
#ifndef _meter_drv_opt_h_
#define _meter_drv_opt_h_
#include "meter_drv_djsf1352_api.h"
#include "meter_drv_djsf2669_api.h"
#include "meter_drv_opt_fault.h"














//电表类型定义
typedef enum 
{
    eMeterTypeAcrelDjsf1352rn           = 0,                                                           //安科瑞 Acrel
    eMeterTypeEnlytaDjsf2669d           = 1,                                                           //英利达 Enlyta
    
    eMeterTypeMax
}eMeterType_t;
















//电表通讯协议 功能映射
typedef struct
{
    //电表类型
    eMeterType_t                        eType;
    
    //电表初始化
    bool                                (*pInit)(void);
    
    
    //电表对外的api接口函数
    bool                                (*pGetUdcIdcPdc)(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
    bool                                (*pGetBreaklineSts)(ePileGunIndex_t eGunIndex, bool *pFlag);
    bool                                (*pGetTmeter)(ePileGunIndex_t eGunIndex, f32 *pTmeter);
    bool                                (*pGetEnergy)(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
    bool                                (*pGetUdcRatio)(ePileGunIndex_t eGunIndex, u16 *pRatio);
    bool                                (*pGetIdcRated)(ePileGunIndex_t eGunIndex, u16 *pRated);
    bool                                (*pGetDiDoSts)(ePileGunIndex_t eGunIndex, u16 *pSts);
    bool                                (*pGetWarningSts)(ePileGunIndex_t eGunIndex, u16 *pSts);
    bool                                (*pGetTime)(ePileGunIndex_t eGunIndex, stTime_t *pTime);
    bool                                (*pGetReadingDay)(ePileGunIndex_t eGunIndex, u8 *pDay);
    bool                                (*pGetRate)(ePileGunIndex_t eGunIndex, u8 *pRate);
    bool                                (*pGetSwVer)(ePileGunIndex_t eGunIndex, u16 *pSwVer);
    bool                                (*pGetFault_MCF)(void);
    
    bool                                (*pSetClearE)(ePileGunIndex_t eGunIndex);
    
    
    
}stMeterOpt_t;
































extern bool sMeterDrvOptInit(eMeterType_t eType);

//电表数据字段的读写接口函数
extern bool sMeterDrvOptGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
extern bool sMeterDrvOptGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag);
extern bool sMeterDrvOptGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter);
extern bool sMeterDrvOptGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
extern bool sMeterDrvOptGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio);
extern bool sMeterDrvOptGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated);
extern bool sMeterDrvOptGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts);
extern bool sMeterDrvOptGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts);
extern bool sMeterDrvOptGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);
extern bool sMeterDrvOptGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay);
extern bool sMeterDrvOptGetRate(ePileGunIndex_t eGunIndex, u8 *pRate);
extern bool sMeterDrvOptGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer);
extern bool sMeterDrvOptGetFault_MCF(void);

extern bool sMeterDrvOptSetClearEnergy(ePileGunIndex_t eGunIndex);



#endif







