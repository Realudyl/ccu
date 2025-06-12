/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   meter_drv_djsf1352_api.c
* Description                           :   安科瑞电表通讯 驱动 api接口函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   具体型号为:DJSF1352-RN 导轨式直流电能表
*                                           使用modbus协议,而不是DLT645规约
****************************************************************************************************/
#include "meter_drv_djsf1352_api.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_djsf1352_api";



extern stMeterDjsf1352rnCache_t *pDjsf1352rnCache;




//DJSF1352-RN电表数据字段的读写接口函数
bool sMeterDrvDjsf1352rnGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
bool sMeterDrvDjsf1352rnGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag);
bool sMeterDrvDjsf1352rnGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter);
bool sMeterDrvDjsf1352rnGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
bool sMeterDrvDjsf1352rnGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio);
bool sMeterDrvDjsf1352rnGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated);
bool sMeterDrvDjsf1352rnGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts);
bool sMeterDrvDjsf1352rnGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts);
bool sMeterDrvDjsf1352rnGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);
bool sMeterDrvDjsf1352rnGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay);
bool sMeterDrvDjsf1352rnGetRate(ePileGunIndex_t eGunIndex, u8 *pRate);
bool sMeterDrvDjsf1352rnGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer);

bool sMeterDrvDjsf1352rnGetFault_MCF(void);










/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 Udc、Idc、Pdc
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc)
{
    if(eGunIndex < cMeterNum)
    {
        if(pUdc != NULL)
        {
            (*pUdc) = pDjsf1352rnCache->stData[eGunIndex].f32Udc;
        }
        
        if(pIdc != NULL)
        {
            (*pIdc) = pDjsf1352rnCache->stData[eGunIndex].f32Idc;
        }
        
        if(pPdc != NULL)
        {
            (*pPdc) = pDjsf1352rnCache->stData[eGunIndex].f32Pdc;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 断线标志
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   pFlag：true---断线， false---未断线
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetBreaklineSts(ePileGunIndex_t eGunIndex, bool *pFlag)
{
    if((eGunIndex < cMeterNum) && (pFlag != NULL))
    {
        (*pFlag) = (pDjsf1352rnCache->stData[eGunIndex].eBreaklineSts == 1) ? true : false;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 电表内部温度
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetTmeter(ePileGunIndex_t eGunIndex, f32 *pTmeter)
{
    if((eGunIndex < cMeterNum) && (pTmeter != NULL))
    {
        (*pTmeter) = pDjsf1352rnCache->stData[eGunIndex].f32Tmeter;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 正向有功、负向有功
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg)
{
    if(eGunIndex < cMeterNum)
    {
        if(pEpos != NULL)
        {
            (*pEpos) = pDjsf1352rnCache->stData[eGunIndex].u32Epos;
        }
        
        if(pEneg != NULL)
        {
            (*pEneg) = pDjsf1352rnCache->stData[eGunIndex].u32Eneg;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 电压变比
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetUdcRatio(ePileGunIndex_t eGunIndex, u16 *pUdcRatio)
{
    if((eGunIndex < cMeterNum) && (pUdcRatio != NULL))
    {
        (*pUdcRatio) = pDjsf1352rnCache->stData[eGunIndex].u16UdcRatio;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 额定一次电流值
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetIdcRated(ePileGunIndex_t eGunIndex, u16 *pIdcRated)
{
    if((eGunIndex < cMeterNum) && (pIdcRated != NULL))
    {
        (*pIdcRated) = pDjsf1352rnCache->stData[eGunIndex].u16IdcRated1;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 开入开出状态
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetDiDoSts(ePileGunIndex_t eGunIndex, u16 *pSts)
{
    if((eGunIndex < cMeterNum) && (pSts != NULL))
    {
        (*pSts) = pDjsf1352rnCache->stData[eGunIndex].unDiDoSts.u16Value;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 告警状态
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetWarningSts(ePileGunIndex_t eGunIndex, u16 *pSts)
{
    if((eGunIndex < cMeterNum) && (pSts != NULL))
    {
        (*pSts) = pDjsf1352rnCache->stData[eGunIndex].unWarningSts.u16Value;
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取 日期、时间
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime)
{
    if((eGunIndex < cMeterNum) && (pTime != NULL))
    {
        memcpy(pTime, &pDjsf1352rnCache->stData[eGunIndex].stTime, sizeof(stTime_t));
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 当前抄表日
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetReadingDay(ePileGunIndex_t eGunIndex, u8 *pDay)
{
    if((eGunIndex < cMeterNum) && (pDay != NULL))
    {
        (*pDay) = pDjsf1352rnCache->stData[eGunIndex].u8ReadingDay;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 当前费率
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetRate(ePileGunIndex_t eGunIndex, u8 *pRate)
{
    if((eGunIndex < cMeterNum) && (pRate != NULL))
    {
        (*pRate) = pDjsf1352rnCache->stData[eGunIndex].eRate;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 软件版本号
* Author                                :   Hall
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetSwVer(ePileGunIndex_t eGunIndex, u16 *pSwVer)
{
    if((eGunIndex < cMeterNum) && (pSwVer != NULL))
    {
        (*pSwVer) = pDjsf1352rnCache->stData[eGunIndex].u16SwVer;
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取电表通讯故障状态
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnGetFault_MCF(void)
{
    return pDjsf1352rnCache->bMeterFault_MCF;
}









