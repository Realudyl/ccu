/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_fault_hw.c
* Description                           :   pile_cn_fault_hw-----硬件相关故障告警检查注册
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-08-12
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_fault_hw";












void sPileCnFaultChkRegister_Hw(bool bIsSuperCharger);



void sPileCnFaultCheck_TS(void);
void sPileCnFaultCheck_CFB(void);
void sPileCnFaultCheck_IMD(void);
void sPileCnFaultCheck_CGHT(void);
void sPileCnFaultCheck_GHT(void);
void sPileCnFaultCheck_CEE(void);
void sPileCnFaultCheck_CCLE(void);
void sPileCnFaultCheck_PCO(void);
void sPileCnFaultCheck_CGVE(void);
void sPileCnFaultCheck_OOVP(void);
void sPileCnFaultCheck_OOCP(void);
void sPileCnFaultCheck_IMDW(void);
void sPileCnFaultCheck_COVE(void);
void sPileCnFaultCheck_OCE(void);
void sPileCnFaultCheck_ORV(void);
void sPileCnFaultCheck_ORI(void);
void sPileCnFaultCheck_CDE(void);
void sPileCnFaultCheck_CVH(void);



/**********************************************************************************************
* Description                           :   硬件故障码注册
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultChkRegister_Hw(bool bIsSuperCharger)
{
    sFaultChkRegister(sPileCnFaultCheck_TS);
//    sFaultChkRegister(sPileCnFaultCheck_CFB);                                  //直流熔断器故障暂时屏蔽
    sFaultChkRegister(sPileCnFaultCheck_IMD);
    sFaultChkRegister(sPileCnFaultCheck_CGHT);
    sFaultChkRegister(sPileCnFaultCheck_GHT);
    sFaultChkRegister(sPileCnFaultCheck_CEE);
    sFaultChkRegister(sPileCnFaultCheck_CCLE);
    sFaultChkRegister(sPileCnFaultCheck_CGVE);
    sFaultChkRegister(sPileCnFaultCheck_OOVP);
    sFaultChkRegister(sPileCnFaultCheck_OOCP);
    sFaultChkRegister(sPileCnFaultCheck_IMDW);
    sFaultChkRegister(sPileCnFaultCheck_COVE);
    //sFaultChkRegister(sPileCnFaultCheck_OCE);                                   //无输出电流故障为调试方便暂时屏蔽
    sFaultChkRegister(sPileCnFaultCheck_ORV);
    sFaultChkRegister(sPileCnFaultCheck_ORI);
    sFaultChkRegister(sPileCnFaultCheck_CDE);
    sFaultChkRegister(sPileCnFaultCheck_CVH);
    
    
    if(sEepromAppGetBlockFactoryResFlag() == false) 
    {
        sFaultChkRegister(sPileCnFaultCheck_PCO);
    }
}









/**********************************************************************************************
* Description                           :   自检故障---桩故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_TS(void)
{
    u8 i;
    ePileCnCheckRst_t ePileCnCheckRst;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sPileCnGetSelfTestResult(i, &ePileCnCheckRst) == true)
        {
            if(ePileCnCheckRst == eCheckRstError)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuSelfTestErr) == false)
                {
                    sFaultCodeAdd(i, ePrivDrvFaultCode_ccuSelfTestErr);
                }
            }
            else
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuSelfTestErr) == true)
                {
                    sFaultCodeSub(i, ePrivDrvFaultCode_ccuSelfTestErr);
                }
            }
        }
    }
}









/**********************************************************************************************
* Description                           :   直流熔断器故障---枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
//void sPileCnFaultCheck_CFB(void)
//{
//    u8  i;
//    f32 f32UdcMeter;
//    i32 i32UdcMod;
//    f32 f32GunDcOutputVolt;
//    static u32 u32TimeStamp[ePileGunIndexNum];
//    static u32 u32StopTimeStamp[ePileGunIndexNum];
//    
//    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
//    {
//        sMeterDrvOptGetUdcIdcPdc(i, &f32UdcMeter, NULL, NULL);
//        sModAppOptGetUdcNow(i, &i32UdcMod);
//        sAdcAppSampleGetGunDcOut(i, &f32GunDcOutputVolt);
//        
//        //PCU输出电压大于200V，电表电压与枪口电压小于20V认为故障
//        if((i32UdcMod >= 200) && (f32UdcMeter <= 20) && (f32GunDcOutputVolt <= 20))
//        {
//            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuFuseBlown) == false)
//            {
//                if(xTaskGetTickCount() - u32TimeStamp[i] >= 10000)
//                {
//                    u32StopTimeStamp[i] = xTaskGetTickCount();
//                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuFuseBlown);
//                }
//            }
//        }
//        else
//        {
//            u32TimeStamp[i] = xTaskGetTickCount();
//            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuFuseBlown) == true)
//            {
//                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
//                {
//                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuFuseBlown);
//                }
//            }
//        }
//        
//    }
//}









/**********************************************************************************************
* Description                           :   绝缘检测--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_IMD(void)
{
    u8 i;
    eBmsGbt15IsoResult_t eResult;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_IsoCheckResult(i, &eResult);
        
        if(eResult == eBmsGbt15IsoCheckFail)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdErr) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdErr);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdErr);
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   充电接口过温降载告警--枪告警
* Author                                :   Dai
* modified Date                         :   2024-10-29
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_CGHT(void)
{
    u8 i;
    f32 f32GunTemp1;
    f32 f32GunTemp2;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sAdcAppSampleGetGunTemp(i, &f32GunTemp1, &f32GunTemp2);
        
        if((f32GunTemp1 >= cGunTempWarn) || (f32GunTemp2 >= cGunTempWarn))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cGunHightTempDetectTime)
            {
                 if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuHighTempWarn) == false)
                 {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuHighTempWarn);
                 }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuHighTempWarn) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuHighTempWarn);
                }
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   充电接口过温保护--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_GHT(void)
{
    u8 i;
    f32 f32GunTemp1;
    f32 f32GunTemp2;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sAdcAppSampleGetGunTemp(i, &f32GunTemp1, &f32GunTemp2);
        
        if((f32GunTemp1 >= cGunTempFault) || (f32GunTemp2 >= cGunTempFault))
        {
            if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cGunHightTempDetectTime)
            {
                 if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_gunHighTempErr) == false)
                 {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_gunHighTempErr);
                 }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_gunHighTempErr) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_gunHighTempErr);
                }
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   电子锁故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_CEE(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    ePrivDrvGunLockStatus_t eElockState;
    
    
    if(sEepromAppGetBlockFactoryElockFlag() == true)
    {
        for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
        {
            sPileCnGetElockState(i, &eElockState);
            
            if((eElockState == ePrivDrvGunLockStatusLock) && (sIoDrvGetInput(sIoGetLockFb(i)) == SET))
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cElochFaultDetectTime)
                {
                    if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr) == false)
                    {
                        u32StopTimeStamp[i] = xTaskGetTickCount();
                        sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr);
                    }
                }
            }
            else if((eElockState == ePrivDrvGunLockStatusUnlock) && (sIoDrvGetInput(sIoGetLockFb(i)) == RESET))
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= cElochFaultDetectTime)
                {
                    if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr) == false)
                    {
                        u32StopTimeStamp[i] = xTaskGetTickCount();
                        sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr);
                    }
                }
            }
            else 
            {
                u32StartTimeStamp[i] = xTaskGetTickCount();
                
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr) == true)
                {
                    if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                    {
                        sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr);
                    }
                }
            }
        }
    }
    else
    {
        for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
        {
            sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuELockErr);
        }
    }
}













/**********************************************************************************************
* Description                           :   充电连接故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_CCLE(void)
{
    u8 i;
    static u32 u32TimeStamp[ePileGunIndexNum];
    eBmsGbt15ChgCc1State_t eResult;
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_Cc1StateInChg(i, &eResult);
        if(eResult == eCc1stateChgFail)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgConnectLossErr) == false)
            {
                u32TimeStamp[i] = xTaskGetTickCount();
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgConnectLossErr);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c) 
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgConnectLossErr) == true)
            {
                //增加5s延时再取消故障
                if(xTaskGetTickCount() - u32TimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgConnectLossErr);
                }
            }
        }
    }
}









/**********************************************************************************************
* Description                           :   预充超时--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_PCO(void)
{
    u8 i;
    eBmsGbt15PreChgRst_t eResult;
    f32 f32SampleVol;
    i32 i32ChgMaxOutVol;
    u16 u16BcpAllowMaxVol, u16BcpCurBatVol;
    
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        //获取外侧采样电压
        sAdcAppSampleGetGunDcOut(i, &f32SampleVol);
        
        //获取充电机最大输出电压
        sModAppOptGetUdcMax(i, &i32ChgMaxOutVol);
        
        //获取电池允许充电最高电压
        sBmsGbt15Get_BcpMaxChgVol(i, &u16BcpAllowMaxVol);
        
        u16BcpAllowMaxVol = u16BcpAllowMaxVol / 10;
        
        //获取当前电池组的电压
        sBmsGbt15Get_BcpCurBatVol(i, &u16BcpCurBatVol);
        
        u16BcpCurBatVol = u16BcpCurBatVol / 10;
        
        if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_preChgOvertime) == false)
        {
            if(sBmsGbt15Get_PreChgResult(i, &eResult) == true)
            {
                if(eResult == eBmsGbt15PreChgFail)
                {
                    if((f32SampleVol <= (f32)i32ChgMaxOutVol)
                    && (f32SampleVol >= 0)
                    && (f32SampleVol <= (f32)u16BcpAllowMaxVol)
                    && ((fabs(f32SampleVol - (f32)u16BcpCurBatVol) / fmin(f32SampleVol, (f32)u16BcpCurBatVol)) <= 0.05))
                    {
                        sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_preChgOvertime);
                    }
                }
            }
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_preChgOvertime) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_preChgOvertime);
            }
        }
    }
}









/**********************************************************************************************
* Description                           :   CC1电压异常--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_CGVE(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if((sAdcAppSampleGetCc1State(i) == eAdcAppCc1StateMax))
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgGunVoltErr) == false)
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= 5000)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase,ePrivDrvFaultCode_chgGunVoltErr);
                }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgGunVoltErr) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_chgGunVoltErr);
                }
            }
        }
    }
}








/**********************************************************************************************
* Description                           :   输出电压过压--枪故障（保护）
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_OOVP(void)
{
    u8  i;
    u16 u16BclReqChgVol;
    f32 f32GunDcOutputVolt;
    static u32 u32TimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_BclReqChgVol(i, &u16BclReqChgVol);
        sAdcAppSampleGetGunDcOut(i, &f32GunDcOutputVolt);
        if((abs(((u16)f32GunDcOutputVolt) - (u16BclReqChgVol/10)) >= (u16BclReqChgVol/100)) && (((u16)f32GunDcOutputVolt) > (u16BclReqChgVol/10)) && (sPileCnStateGet(i) == ePrivDrvGunWorkStatusCharging))
        {
            if(xTaskGetTickCount() - u32TimeStamp[i] >= cOverVoltDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverVoltProtect) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverVoltProtect);
                }
            }
        }
        else
        {
            u32TimeStamp[i] = xTaskGetTickCount();
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverVoltProtect) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverVoltProtect);
            }
        }
    }
}









/**********************************************************************************************
* Description                           :   输出电流过流--枪故障（保护）
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_OOCP(void)
{
    u8 i;
    f32 f32MeterIdc;
    u16 u16BclReqChgCur;
    static u32 u32TimeStamp[ePileGunIndexNum];
    static bool bOverCurr[ePileGunIndexNum] = {false,false};
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_BclReqChgCur(i, &u16BclReqChgCur);
        sMeterDrvOptGetUdcIdcPdc(i, NULL, &f32MeterIdc, NULL);
        
        if((fabs(f32MeterIdc - (f32)(u16BclReqChgCur/10)) >= (f32)(u16BclReqChgCur/100)) && (f32MeterIdc > (f32)(u16BclReqChgCur/10)) && (sPileCnStateGet(i) == ePrivDrvGunWorkStatusCharging))
        {
            bOverCurr[i] = true;
            if(xTaskGetTickCount() - u32TimeStamp[i] >= cOverCurDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverCurrentProtect) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverCurrentProtect);
                    EN_SLOGI(TAG, "枪%d发生过流故障，需求电流：%d，当前电流：%fA", i, u16BclReqChgCur, f32MeterIdc);
                }
            }
        }
        else
        {
            u32TimeStamp[i] = xTaskGetTickCount();
            
            if(bOverCurr[i])
            {
                bOverCurr[i] = false;
                EN_SLOGI(TAG, "枪%d过流故障消除，需求电流：%dA，当前电流：%fA", i, u16BclReqChgCur, f32MeterIdc);
            }
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            bOverCurr[i] = false;
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverCurrentProtect) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputOverCurrentProtect);
            }
        }
    }
}













/**********************************************************************************************
* Description                           :   绝缘告警--枪告警
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_IMDW(void)
{
    u8 i;
    eBmsGbt15IsoResult_t eResult;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_IsoCheckResult(i, &eResult);
        
        if(eResult == eBmsGbt15IsoCheckWarn)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdWarn) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdWarn);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdWarn) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuImdWarn);
            }
        }
    }
}







/**********************************************************************************************
* Description                           :   绝缘检测前枪口外侧电压--枪告警
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_COVE(void)
{
    u8 i;
    eBmsGbt15PreIsoResult_t eResult;
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sBmsGbt15Get_PreIsoCheckResult(i, &eResult) == true)
        {
            if(eResult == eBmsGbt15PreIsoCheckFail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuOutsideVoltHiErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuOutsideVoltHiErr);
                }
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuOutsideVoltHiErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuOutsideVoltHiErr);
            }
        }
    }
}








/**********************************************************************************************
* Description                           :   无输出电流--枪告警
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_OCE(void)
{
    u8 i;
    stBmsGbt15Evcc_t pCache;
    f32 f32MeterCur;
    static u32 u32TimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_EvccCacheData(i, &pCache);
        sMeterDrvOptGetUdcIdcPdc(i, NULL, &f32MeterCur, NULL);
        
        //接触器闭合但无输出电流
        if((sAdcAppSampleGetFbStateK(i) == eAdcAppFbState11) && (f32MeterCur == 0) && (sPileCnStateGet(i) == ePrivDrvGunWorkStatusCharging))
        {
            if(xTaskGetTickCount() - u32TimeStamp[i] >= cNoOutputCurDetectTime)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputCurrErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputCurrErr);
                }
            }
        }
        else
        {
            u32TimeStamp[i] = xTaskGetTickCount();
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputCurrErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputCurrErr);
            }
        }
    }

}



/**********************************************************************************************
* Description                           :   枪线反接---桩故障
* Author                                :   Dai
* modified Date                         :   2024-06-12
* notice                                :   
***********************************************************************************************/
void sPileCnFaultCheck_ORV(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    f32 f32MeterVolt;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sMeterDrvOptGetUdcIdcPdc(i, &f32MeterVolt, NULL, NULL);
        
        if(f32MeterVolt < 0)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputReverse) == false)
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= 5000)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputReverse);
                }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputReverse) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_outputReverse);
                }
            }
        }
    }
}



/**********************************************************************************************
* Description                           :   电流反向---桩故障
* Author                                :   Dai
* modified Date                         :   2024-06-12
* notice                                :   
***********************************************************************************************/
void sPileCnFaultCheck_ORI(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    f32 f32MeterCur;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sMeterDrvOptGetUdcIdcPdc(i, NULL, &f32MeterCur, NULL);
        
        if(f32MeterCur < 0)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuMeterConnectErr) == false)
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= 5000)
                {
                    u32StopTimeStamp[i] = xTaskGetTickCount();
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuMeterConnectErr);
                }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuMeterConnectErr) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuMeterConnectErr);
                }
            }
        }
    }
}








/**********************************************************************************************
* Description                           :   泄放超时---桩故障
* Author                                :   Dai
* modified Date                         :   2024-11-08
* notice                                :   
***********************************************************************************************/
void sPileCnFaultCheck_CDE(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    f32 f32MeterCur, f32MeterVolt;
    u16 u16ReqVolt, u16ReqCurr;
    
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sMeterDrvOptGetUdcIdcPdc(i, &f32MeterVolt, &f32MeterCur, NULL);
        sModAppPcuGetReqVolt(i, &u16ReqVolt);
        sModAppPcuGetReqCurr(i, &u16ReqCurr);
        
        if((u16ReqVolt == 0) && (u16ReqCurr == 0) && ((f32MeterVolt >= cSafeVolt) || (f32MeterCur >= cSafeCur)) && (sBmsGbt15StateGet(i) != eBmsGbt15StateWait))
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuDischargeErr) == false)
            {
                if(xTaskGetTickCount() - u32StartTimeStamp[i] >= 30000)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuDischargeErr);
                }
            }
        }
        else
        {
            u32StartTimeStamp[i] = xTaskGetTickCount();
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuDischargeErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuDischargeErr);
            }
        }
    }
}




/**********************************************************************************************
* Description                           :   输出电压大于车端最大允许电压---桩故障
* Author                                :   Dai
* modified Date                         :   2024-11-14
* notice                                :   
***********************************************************************************************/
void sPileCnFaultCheck_CVH(void)
{
    u8 i;
    static u32 u32StartTimeStamp[ePileGunIndexNum];
    u16 u16MaxChgVol,u16BcpAllowMaxVol;
    f32 f32GunDcOutputVolt;
    
    if(sEepromAppGetBlockFactoryProtocolFlag() == true)
    {
        for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
        {
            sBmsGbt15Get_EvccMaxChgVol(i, &u16MaxChgVol);
            sBmsGbt15Get_BcpMaxChgVol(i, &u16BcpAllowMaxVol);
            sAdcAppSampleGetGunDcOut(i, &f32GunDcOutputVolt);
            
            u16MaxChgVol = u16MaxChgVol/10;
            u16BcpAllowMaxVol = u16BcpAllowMaxVol/10;
            
            if(((((f32GunDcOutputVolt >= (u16MaxChgVol + 15)) && (u16MaxChgVol != 0)) || ((f32GunDcOutputVolt >= (u16BcpAllowMaxVol + 15)) && (u16BcpAllowMaxVol != 0)))
            && (sPileCnStateGet(i) > ePrivDrvGunWorkStatusPreparing)))
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuChgVoltHigher) == false)
                {
                    if(xTaskGetTickCount() - u32StartTimeStamp[i] >= 250)
                    {
                        sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuChgVoltHigher);
                    }
                }
            }
            else
            {
                u32StartTimeStamp[i] = xTaskGetTickCount();
            }
            
            if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuChgVoltHigher) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuChgVoltHigher);
                }
            }
        }
    }
    else
    {
        for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
        {
            sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccuChgVoltHigher);
        }
    }
}