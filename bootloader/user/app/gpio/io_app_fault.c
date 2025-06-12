/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     io_app_fault.c
* Description                           :     输入输出信号 之 故障逻辑
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-09-12
* notice                                :     
****************************************************************************************************/
#include "io_app.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_app_fault";











void sIoAppFaultCheck_CDO(void);
void sIoAppFaultCheck_ESBP(void);
void sIoAppFaultCheck_SPD(void);
void sIoAppFaultCheck_DCF(void);
void sIoAppFaultCheck_CAE(void);




void sIoAppFaultCheck_DCF1(void);
void sIoAppFaultCheck_DCF2(void);













/**********************************************************************************************
* Description                           :   门禁故障---桩故障
* Author                                :   Hall
* modified Date                         :   2024-06-13
* notice                                :   eIo_DOOR ----> ePrivDrvFaultCode_dispenserDoorErr
***********************************************************************************************/
void sIoAppFaultCheck_CDO(void)
{
    if(sEepromAppGetBlockFactoryDoorFlag())
    {
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_dispenserDoorErr) == false)
        {
            if(sIoDrvGetInput(eIo_DOOR) == SET)
            {
                sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_dispenserDoorErr);
            }
        }
        else
        {
            if(sIoDrvGetInput(eIo_DOOR) == RESET)
            {
                sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_dispenserDoorErr);
            }
        }
    }
    else
    {
        sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_dispenserDoorErr);
    }
}









/**********************************************************************************************
* Description                           :   紧急停机故障---桩故障
* Author                                :   Hall
* modified Date                         :   2024-06-12
* notice                                :   eIo_FA_STOP ----> ePrivDrvFaultCode_ccuEstopErr
***********************************************************************************************/
void sIoAppFaultCheck_ESBP(void)
{
    if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuEstopErr) == false)
    {
        if(sIoDrvGetInput(eIo_FA_STOP) == SET)
        {
            sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuEstopErr);
        }
    }
    else
    {
        if(sIoDrvGetInput(eIo_FA_STOP) == RESET)
        {
            sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuEstopErr);
        }
    }
}






/**********************************************************************************************
* Description                           :   防雷器---桩故障
* Author                                :   Hall
* modified Date                         :   2024-06-12
* notice                                :   eIo_FA_STOP ----> ePrivDrvFaultCode_ccuEstopErr
***********************************************************************************************/
void sIoAppFaultCheck_SPD(void)
{
    static u32 u32StartTimeStamp;
    static u32 u32StopTimeStamp;
    
    if(sIoDrvGetInput(eIo_SPD) == RESET)
    {
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_spdErr) == false)
        {
            if(xTaskGetTickCount() - u32StartTimeStamp >= 5000)
            {
                u32StopTimeStamp = xTaskGetTickCount();
                sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_spdErr);
            }
        }
    }
    else
    {
        u32StartTimeStamp = xTaskGetTickCount();
        if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_spdErr) == true)
        {
            if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
            {
                sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_spdErr);
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   直流接触器---枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sIoAppFaultCheck_DCF(void)
{
    sIoAppFaultCheck_DCF1();
    sIoAppFaultCheck_DCF2();
}








/**********************************************************************************************
* Description                           :   辅助电源故障---枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sIoAppFaultCheck_CAE(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if((sIoDrvGetOutput(sIoGetRelayD1(i)) == SET) && (sIoDrvGetOutput(sIoGetRelayD2(i)) == SET) && (sIoDrvGetInput(sIoGetGbBms(i)) == SET))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cPowerFaultDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuApsErr) == false)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuApsErr);
                }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuApsErr) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuApsErr);
                }
            }
        }
    }
}

















/**********************************************************************************************
* Description                           :   直流接触器---枪故障(K1)
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sIoAppFaultCheck_DCF1(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if((sIoDrvGetOutput(sIoGetDrK(i)) == SET) && ((sAdcAppSampleGetFbStateK(i) == eAdcAppFbStateX0) || (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState00)))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cDcContactFaultDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err) == false)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err);
                }
            }
        }
        else if((sIoDrvGetOutput(sIoGetDrK(i)) == RESET) && ((sAdcAppSampleGetFbStateK(i) == eAdcAppFbStateX0) ||(sAdcAppSampleGetFbStateK(i) == eAdcAppFbState11)))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cDcContactFaultDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err) == false)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err);
                }
            }
        }
        
        if(((sIoDrvGetOutput(sIoGetDrK(i)) == SET) && (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState11)) || ((sIoDrvGetOutput(sIoGetDrK(i)) == RESET) && (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState00)))
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor1Err);
                }
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   直流接触器---枪故障(K2)
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sIoAppFaultCheck_DCF2(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if((sIoDrvGetOutput(sIoGetDrK(i)) == SET) && ((sAdcAppSampleGetFbStateK(i) == eAdcAppFbState0X) || (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState00)))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cDcContactFaultDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err) == false)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err);
                }
            }
        }
        else if((sIoDrvGetOutput(sIoGetDrK(i)) == RESET) && ((sAdcAppSampleGetFbStateK(i) == eAdcAppFbState0X) ||(sAdcAppSampleGetFbStateK(i) == eAdcAppFbState11)))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cDcContactFaultDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err) == false)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err);
                }
            }
        }
        
        if(((sIoDrvGetOutput(sIoGetDrK(i)) == SET) && (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState11)) || ((sIoDrvGetOutput(sIoGetDrK(i)) == RESET) && (sAdcAppSampleGetFbStateK(i) == eAdcAppFbState00)))
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_dcContactor2Err);
                }
            }
        }
    }
}










