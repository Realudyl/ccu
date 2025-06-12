/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     card_app_fault.c
* Description                           :     刷卡数据管理 之 故障逻辑
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-09-12
* notice                                :     
****************************************************************************************************/
#include "card_app.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "card_app_fault";





extern stCardAppCache_t stCardAppCache;





void sCardAppFaultCheck_CRF(void);



























/**********************************************************************************************
* Description                           :   读卡器异常---桩故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sCardAppFaultCheck_CRF(void)
{
    static u32 u32StopTimeStamp;
    sCardReaderBreaklineCheck();
    
    if(stCardAppCache.bCardReaderBreakline == true) 
    {
       if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_cardRderErr) == false)
       {
            u32StopTimeStamp = xTaskGetTickCount();
            sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_cardRderErr);
       }
    }
    else
    {
       if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_cardRderErr) == true)
       {
            if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
            {
                sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_cardRderErr);
            }
       }
    }
}



