/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     meter_drv_opt_fault.c
* Description                           :     电表驱动 之 故障逻辑
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-09-12
* notice                                :     
****************************************************************************************************/
#include "meter_drv_opt.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_opt_fault";






void sMeterDrvOptFaultCheck_MCF(void);




















/**********************************************************************************************
* Description                           :   电表通讯异常---桩故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sMeterDrvOptFaultCheck_MCF(void)
{
    static u32 u32StopTimeStamp;
    if(sMeterDrvOptGetFault_MCF() == true)
    {
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_meterCommErr) == false)
        {
            u32StopTimeStamp = xTaskGetTickCount();
            sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_meterCommErr);
        }
    }
    else
    {
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_meterCommErr) == true)
        {
            if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
            {
                sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_meterCommErr);
            }
        }
    }
}



