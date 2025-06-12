/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :     meter_drv_opt.c
* Description                           :     电表驱动
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2022-08-09
* notice                                :     
****************************************************************************************************/
#include "meter_drv_opt.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_opt";


//---------------------------------------------------------------------------------------------------------


//电表协议映射表
static stMeterOpt_t stMeterOptMap[] = 
{
    //安科瑞DJSF1352-RN电表
    {
        .eType                          = eMeterTypeAcrelDjsf1352rn,
        .pInit                          = sMeterDrvDjsf1352rnInit,
        
        //
        .pGetUdcIdcPdc                  = sMeterDrvDjsf1352rnGetUdcIdcPdc,
        .pGetBreaklineSts               = sMeterDrvDjsf1352rnGetBreaklineSts,
        .pGetTmeter                     = sMeterDrvDjsf1352rnGetTmeter,
        .pGetEnergy                     = sMeterDrvDjsf1352rnGetEnergy,
        .pGetUdcRatio                   = sMeterDrvDjsf1352rnGetUdcRatio,
        .pGetIdcRated                   = sMeterDrvDjsf1352rnGetIdcRated,
        .pGetDiDoSts                    = sMeterDrvDjsf1352rnGetDiDoSts,
        .pGetWarningSts                 = sMeterDrvDjsf1352rnGetWarningSts,
        .pGetTime                       = sMeterDrvDjsf1352rnGetTime,
        .pGetReadingDay                 = sMeterDrvDjsf1352rnGetReadingDay,
        .pGetRate                       = sMeterDrvDjsf1352rnGetRate,
        .pGetSwVer                      = sMeterDrvDjsf1352rnGetSwVer,
        .pGetFault_MCF                  = sMeterDrvDjsf1352rnGetFault_MCF,
        
        
        .pSetClearE                     = NULL,
    },
    
    //英利达DJSF2669D电表
    {
        .eType                          = eMeterTypeEnlytaDjsf2669d,
        .pInit                          = sMeterDrvDjsf2669dInit,
        
        //
        .pGetUdcIdcPdc                  = sMeterDrvDjsf2669dGetUdcIdcPdc,
        .pGetBreaklineSts               = NULL,
        .pGetTmeter                     = NULL,
        .pGetEnergy                     = sMeterDrvDjsf2669dGetEnergy,
        .pGetUdcRatio                   = NULL,
        .pGetIdcRated                   = NULL,
        .pGetDiDoSts                    = NULL,
        .pGetWarningSts                 = NULL,
        .pGetTime                       = sMeterDrvDjsf2669dGetTime,
        .pGetReadingDay                 = NULL,
        .pGetRate                       = NULL,
        .pGetSwVer                      = NULL,
        .pGetFault_MCF                  = sMeterDrvDjsf2669dGetFault_MCF,
        
        
        .pSetClearE                     = sMeterDrvDjsf2669dSetClearE,
    },
};



stMeterOpt_t *pMeterOpt = &stMeterOptMap[eMeterTypeAcrelDjsf1352rn];


//---------------------------------------------------------------------------------------------------------

bool sMeterDrvOptInit(eMeterType_t eType);


//电表数据字段的读写接口函数
bool sMeterDrvOptGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
bool sMeterDrvOptGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag);
bool sMeterDrvOptGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter);
bool sMeterDrvOptGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
bool sMeterDrvOptGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio);
bool sMeterDrvOptGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated);
bool sMeterDrvOptGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts);
bool sMeterDrvOptGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts);
bool sMeterDrvOptGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);
bool sMeterDrvOptGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay);
bool sMeterDrvOptGetRate(ePileGunIndex_t eGunIndex, u8 *pRate);
bool sMeterDrvOptGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer);

bool sMeterDrvOptSetClearEnergy(ePileGunIndex_t eGunIndex);

//---------------------------------------------------------------------------------------------------------














/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 通讯资源初始化
* Author                                :     Hall
* Creat Date                            :     2022-08-09
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptInit(eMeterType_t eType)
{
    if((eType < eMeterTypeAcrelDjsf1352rn) || (eType >= eMeterTypeMax))
    {
        return (false);
    }
    pMeterOpt = &stMeterOptMap[eType];
    
    return ((pMeterOpt->pInit == NULL) ? false : pMeterOpt->pInit());
}








/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 电压、电流、功率
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc)
{
    return ((pMeterOpt->pGetUdcIdcPdc == NULL) ? false : pMeterOpt->pGetUdcIdcPdc(eGunIndex, pUdc, pIdc, pPdc));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 断线标志
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag)
{
    return ((pMeterOpt->pGetBreaklineSts == NULL) ? false : pMeterOpt->pGetBreaklineSts(eGunIndex, pFlag));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 电表内部温度
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter)
{
    return ((pMeterOpt->pGetTmeter == NULL) ? false : pMeterOpt->pGetTmeter(eGunIndex, pTmeter));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 正向有功、负向有功
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg)
{
    return ((pMeterOpt->pGetEnergy == NULL) ? false : pMeterOpt->pGetEnergy(eGunIndex, pEpos, pEneg));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 电压变比
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio)
{
    return ((pMeterOpt->pGetUdcRatio == NULL) ? false : pMeterOpt->pGetUdcRatio(eGunIndex, pUdcRatio));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 额定一次电流值
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated)
{
    return ((pMeterOpt->pGetIdcRated == NULL) ? false : pMeterOpt->pGetIdcRated(eGunIndex, pIdcRated));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 开入开出状态
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts)
{
    return ((pMeterOpt->pGetDiDoSts == NULL) ? false : pMeterOpt->pGetDiDoSts(eGunIndex, pSts));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 告警状态
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts)
{
    return ((pMeterOpt->pGetWarningSts == NULL) ? false : pMeterOpt->pGetWarningSts(eGunIndex, pSts));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取 日期、时间
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime)
{
    return ((pMeterOpt->pGetTime == NULL) ? false : pMeterOpt->pGetTime(eGunIndex, pTime));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 当前抄表日
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay)
{
    return ((pMeterOpt->pGetReadingDay == NULL) ? false : pMeterOpt->pGetReadingDay(eGunIndex, pDay));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 当前费率
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetRate(ePileGunIndex_t eGunIndex, u8 *pRate)
{
    return ((pMeterOpt->pGetRate == NULL) ? false : pMeterOpt->pGetRate(eGunIndex, pRate));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 软件版本号
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer)
{
    return ((pMeterOpt->pGetSwVer == NULL) ? false : pMeterOpt->pGetSwVer(eGunIndex, pSwVer));
}






/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 获取电表通信状态
* Author                                :     Dai
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptGetFault_MCF(void)
{
    return ((pMeterOpt->pGetFault_MCF == NULL) ? false : pMeterOpt->pGetFault_MCF());
}







/***************************************************************************************************
* Description                           :     电表驱动 api接口函数 之 设置 清除电表读数
* Author                                :     Hall
* Creat Date                            :     2024-03-19
* notice                                :     
****************************************************************************************************/
bool sMeterDrvOptSetClearEnergy(ePileGunIndex_t eGunIndex)
{
    return ((pMeterOpt->pSetClearE == NULL) ? false : pMeterOpt->pSetClearE(eGunIndex));
}





























