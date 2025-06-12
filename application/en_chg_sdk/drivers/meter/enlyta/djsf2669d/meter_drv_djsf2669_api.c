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
#include "meter_drv_djsf2669_api.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_djsf2669_api";



extern stMeterDjsf2669dCache_t *pDjsf2669dCache;





//DJSF2669D电表数据字段的读写接口函数
bool sMeterDrvDjsf2669dGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc);
bool sMeterDrvDjsf2669dGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg);
bool sMeterDrvDjsf2669dGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime);

bool sMeterDrvDjsf2669dSetClearE(ePileGunIndex_t eGunIndex);

bool sMeterDrvDjsf2669dGetFault_MCF(void);











/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 api接口函数 之 获取 Udc、Idc、Pdc
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dGetUdcIdcPdc(ePileGunIndex_t eGunIndex, f32 *pUdc, f32 *pIdc, f32 *pPdc)
{
    if(eGunIndex < (cMeterNum * 2))
    {
        if(pUdc != NULL)
        {
            (*pUdc) = pDjsf2669dCache->stData[eGunIndex].f32Udc;
        }
        
        if(pIdc != NULL)
        {
            (*pIdc) = pDjsf2669dCache->stData[eGunIndex].f32Idc;
        }
        
        if(pPdc != NULL)
        {
            (*pPdc) = pDjsf2669dCache->stData[eGunIndex].f32Pdc;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 api接口函数 之 获取 正向有功、负向有功
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dGetEnergy(ePileGunIndex_t eGunIndex, u32 *pEpos, u32 *pEneg)
{
    if(eGunIndex < (cMeterNum * 2))
    {
        if(pEpos != NULL)
        {
            (*pEpos) = (u32)(pDjsf2669dCache->stData[eGunIndex].f32Ept * 1000);
        }
        
        if(pEneg != NULL)
        {
            (*pEneg) = (u32)(pDjsf2669dCache->stData[eGunIndex].f32Ent * 1000);
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 api接口函数 之 获取 日期、时间
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dGetTime(ePileGunIndex_t eGunIndex, stTime_t *pTime)
{
    if((eGunIndex < (cMeterNum * 2)) && (pTime != NULL))
    {
        memcpy(pTime, &pDjsf2669dCache->stData[eGunIndex].stTime, sizeof(stTime_t));
        
        return(true);
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 api接口函数 之 设置 清除电量
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dSetClearE(ePileGunIndex_t eGunIndex)
{
    if(eGunIndex < cMeterNum)
    {
        return sMeterDrvDjsf2669dWriteRegClear(eGunIndex + cMeterDrvDjsf2669dAddrBase);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 api接口函数 之 获取电表通讯故障状态
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dGetFault_MCF(void)
{
    return pDjsf2669dCache->bMeterFault_MCF;
}






