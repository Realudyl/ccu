/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   adc_app_sample_spec.h
* Description                           :   ADC采样计算 之 部分特殊采样通道的数据处理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
****************************************************************************************************/
#include "adc_app_sample.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "adc_app_sample_spec";





//PP信号状态描述字符串
u8   u8UppStateString[eAdcAppPpStateMax + 1][48] = 
{
    "State:2.77V,未插枪,已按下开关",
    "State:0.00V,未插枪",
    "State:1.53V,已插枪",
    "未知状态"
};





//PD信号状态描述字符串
u8   u8UpdStateString[eAdcAppPdStateMax + 1][32] = 
{
    "State:0.0V,未插枪",
    "State:2.0V,已插枪",
    "未知状态"
};





//CC1信号状态描述字符串
u8   u8Ucc1StateString[eAdcAppCc1StateMax + 1][24] = 
{
    "State U1a:(+12V)",
    "State U1b:(+06V)",
    "State U1c:(+04V)",
    "未知状态"
};



//CP信号状态描述字符串
u8   u8UcpStateString[eChgCpStateMax + 1][16] = 
{
    "State A:(+12V)",
    "State B:(+09V)",
    "State C:(+06V)",
    "State D:(+03V)",
    "State E:(+00V)",
    "State F:(-12V)",
    "未知状态"
};





extern stAdcAppData_t stAdcAppData;








bool sAdcAppSampleUfbToState (f32 f32Ufb,  eAdcAppFbState_t  *pState);
bool sAdcAppSampleUppToState(f32 f32Upp,   eAdcAppPpState_t  *pState);
bool sAdcAppSampleUpdToState(f32 f32Upd,   eAdcAppPdState_t  *pState);
bool sAdcAppSampleUcc1ToState(f32 f32Ucc1, eAdcAppCc1State_t *pState);
bool sAdcAppSampleUcpToState (f32 f32Ucp,  eChgCpState_t     *pState);

void sAdcAppSampleCheckFbState (stAdcAppFbData_t  *pFbData,  f32 f32Ufb);
void sAdcAppSampleCheckGunDcOut(ePileGunIndex_t eGunIndex,   f32 f32UdcOut);
void sAdcAppSampleCheckPpState (ePileGunIndex_t eGunIndex,   f32 f32Upp);
void sAdcAppSampleCheckPdState (ePileGunIndex_t eGunIndex,   f32 f32Upd);
void sAdcAppSampleCheckCc1State(ePileGunIndex_t eGunIndex,   f32 f32Ucc1);
void sAdcAppSampleCheckCpState (ePileGunIndex_t eGunIndex,   f32 f32Ucp);









/***************************************************************************************************
* Description                           :   FB反馈电压转换到状态
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
*                                           f32Ufb: 反馈电压采样值，单位V
*                                           仅将单周期反馈电压转换为对应的反馈状态，并未滤波，应该由调用者执行滤波
****************************************************************************************************/
bool sAdcAppSampleUfbToState(f32 f32Ufb, eAdcAppFbState_t *pState)
{
    if(pState == NULL)
    {
        return(false);
    }
    if(f32Ufb >= cAdcAppVoltFbState00)
    {
        (*pState) = eAdcAppFbState00;
    }
    else if(f32Ufb >= cAdcAppVoltFbStateX0)
    {
        (*pState) = eAdcAppFbStateX0;
    }
    else if(f32Ufb >= cAdcAppVoltFbState0X)
    {
        (*pState) = eAdcAppFbState0X;
    }
    else if(f32Ufb <= cAdcAppVoltFbState11)
    {
        (*pState) = eAdcAppFbState11;
    }
    else
    {
        (*pState) = eAdcAppFbStateMax;
    }
    
    return(true);
}









/***************************************************************************************************
* Description                           :   PP电压转换到PP状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
*                                           
****************************************************************************************************/
bool sAdcAppSampleUppToState(f32 f32Upp, eAdcAppPpState_t *pState)
{
    if(pState == NULL)
    {
        return(false);
    }
    
    if(fabs(f32Upp - cAdcAppVoltPpStateU1a) <= cAdcAppVoltPpErr)
    {
        (*pState) = eAdcAppPpStateU1a;
    }
    else if(fabs(f32Upp - cAdcAppVoltPpStateU1b) <= cAdcAppVoltPpErr)
    {
        (*pState) = eAdcAppPpStateU1b;
    }
    else if(fabs(f32Upp - cAdcAppVoltPpStateU1c) <= cAdcAppVoltPpErr)
    {
        (*pState) = eAdcAppPpStateU1b;
    }
    else
    {
        (*pState) = eAdcAppPpStateMax;
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   PD电压转换到PD状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
*                                           
****************************************************************************************************/
bool sAdcAppSampleUpdToState(f32 f32Upd, eAdcAppPdState_t *pState)
{
    if(pState == NULL)
    {
        return(false);
    }
    
    if(fabs(f32Upd - eAdcAppPdStatePlugOut) <= cAdcAppVoltPdErr)
    {
        (*pState) = eAdcAppPdStatePlugOut;
    }
    else if(fabs(f32Upd - eAdcAppPdStatePlugIn) <= cAdcAppVoltPdErr)
    {
        (*pState) = eAdcAppPdStatePlugIn;
    }
    else
    {
        (*pState) = eAdcAppPdStateMax;
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   CC1电压转换到CC1状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
*                                           
****************************************************************************************************/
bool sAdcAppSampleUcc1ToState(f32 f32Ucc1, eAdcAppCc1State_t *pState)
{
    if(pState == NULL)
    {
        return(false);
    }
    
    if(fabs(f32Ucc1 - cAdcAppVoltCc1StateU1a) <= cAdcAppVoltCc1Err)
    {
        (*pState) = eAdcAppCc1StateU1a;
    }
    else if(fabs(f32Ucc1 - cAdcAppVoltCc1StateU1b) <= cAdcAppVoltCc1Err)
    {
        (*pState) = eAdcAppCc1StateU1b;
    }
    else if(fabs(f32Ucc1 - cAdcAppVoltCc1StateU1c) <= cAdcAppVoltCc1Err)
    {
        (*pState) = eAdcAppCc1StateU1c;
    }
    else
    {
        (*pState) = eAdcAppCc1StateMax;
    }
    
    return(true);
}







/***************************************************************************************************
* Description                           :   CP电压转换到CP状态
* Author                                :   Hall
* Creat Date                            :   2023-06-09
* notice                                :   
*                                           f32Ucp: cp电压，单位V
*                                           本函数是一个接口函数，供采样计算的组件调用，仅仅将单周期CP
*                                           电压转换为对应的CP状态，并未滤波，应该由调用者执行滤波
****************************************************************************************************/
bool sAdcAppSampleUcpToState(f32 f32Ucp, eChgCpState_t *pState)
{
    if(pState == NULL)
    {
        return(false);
    }
    
    if(fabs(f32Ucp - cChgUcpA) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateA;
    }
    else if(fabs(f32Ucp - cChgUcpB) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateB;
    }
    else if(fabs(f32Ucp - cChgUcpC) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateC;
    }
    else if(fabs(f32Ucp - cChgUcpD) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateD;
    }
    else if(fabs(f32Ucp - cChgUcpE) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateE;
    }
    else if(fabs(f32Ucp - cChgUcpF) <= cChgUcpErr)
    {
        (*pState) = eChgCpStateF;
    }
    else
    {
        (*pState) = eChgCpStateMax;
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   FB反馈状态值更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   增加滤波逻辑后 利用采样值值更新反馈状态
****************************************************************************************************/
void sAdcAppSampleCheckFbState(stAdcAppFbData_t *pFbData, f32 f32Ufb)
{
    eAdcAppFbState_t eState;
    
    
    sAdcAppSampleUfbToState(f32Ufb, &eState);
    if(pFbData->eState == eState)
    {
        return;
    }
    
    if(pFbData->eStateTemp == eState)
    {
        pFbData->i32Cnt++;
        if(pFbData->i32Cnt >= 3)
        {
            pFbData->eState = pFbData->eStateTemp;
            pFbData->i32Cnt = 0;
        }
    }
    else
    {
        pFbData->eStateTemp = eState;
        pFbData->i32Cnt = 0;
    }
}








/***************************************************************************************************
* Description                           :   枪外侧电压稳定标志判断
* Author                                :   Hall
* Creat Date                            :   2024-04-15
* notice                                :   
****************************************************************************************************/
void sAdcAppSampleCheckGunDcOut(ePileGunIndex_t eGunIndex, f32 f32UdcOut)
{
    stAdcAppData.stGunDcOutData[eGunIndex].f32Udc = f32UdcOut;
    if(xTaskGetTickCount() - stAdcAppData.stGunDcOutData[eGunIndex].u32Tick > 200)//200ms级别稳定即可满足需求
    {
        stAdcAppData.stGunDcOutData[eGunIndex].u32Tick = xTaskGetTickCount();
        stAdcAppData.stGunDcOutData[eGunIndex].bStableFlag = (fabs(stAdcAppData.stGunDcOutData[eGunIndex].f32UdcStable - f32UdcOut) < 10.0f) ? true : false;
        stAdcAppData.stGunDcOutData[eGunIndex].f32UdcStable = f32UdcOut;
    }
}









/***************************************************************************************************
* Description                           :   PP状态值更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
void sAdcAppSampleCheckPpState(ePileGunIndex_t eGunIndex, f32 f32Upp)
{
    eAdcAppPpState_t eState;
    stAdcAppPpData_t *pPpData = &stAdcAppData.stPpData[eGunIndex];
    
    sAdcAppSampleUppToState(f32Upp, &eState);
    pPpData->f32Value = f32Upp;
    if(pPpData->eState == eState)
    {
        return;
    }
    
    if(pPpData->eStateTemp == eState)
    {
        pPpData->i32Cnt++;
        if(pPpData->i32Cnt >= 3)
        {
            EN_SLOGD(TAG, "充电枪%d, 当前PP电压:%5.2fV, PP状态变化:%s--->%s", eGunIndex, f32Upp, u8UppStateString[pPpData->eState], u8UppStateString[pPpData->eStateTemp]);
            pPpData->eState = pPpData->eStateTemp;
            pPpData->i32Cnt = 0;
        }
    }
    else
    {
        pPpData->eStateTemp = eState;
        pPpData->i32Cnt = 0;
    }
}







/***************************************************************************************************
* Description                           :   PD状态值更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
void sAdcAppSampleCheckPdState(ePileGunIndex_t eGunIndex, f32 f32Upd)
{
    eAdcAppPdState_t eState;
    stAdcAppPdData_t *pPdData = &stAdcAppData.stPdData[eGunIndex];
    
    sAdcAppSampleUpdToState(f32Upd, &eState);
    pPdData->f32Value = f32Upd;
    if(pPdData->eState == eState)
    {
        return;
    }
    
    if(pPdData->eStateTemp == eState)
    {
        pPdData->i32Cnt++;
        if(pPdData->i32Cnt >= 3)
        {
            EN_SLOGD(TAG, "充电枪%d, 当前PD电压:%5.2fV, PD状态变化:%s--->%s", eGunIndex, f32Upd, u8UpdStateString[pPdData->eState], u8UpdStateString[pPdData->eStateTemp]);
            pPdData->eState = pPdData->eStateTemp;
            pPdData->i32Cnt = 0;
        }
    }
    else
    {
        pPdData->eStateTemp = eState;
        pPdData->i32Cnt = 0;
    }
}







/***************************************************************************************************
* Description                           :   CC1状态值更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
void sAdcAppSampleCheckCc1State(ePileGunIndex_t eGunIndex, f32 f32Ucc1)
{
    eAdcAppCc1State_t eState;
    stAdcAppCc1Data_t *pCc1Data = &stAdcAppData.stCc1Data[eGunIndex];
    
    sAdcAppSampleUcc1ToState(f32Ucc1, &eState);
    pCc1Data->f32Value = f32Ucc1;
    if(pCc1Data->eState == eState)
    {
        return;
    }
    
    if(pCc1Data->eStateTemp == eState)
    {
        pCc1Data->i32Cnt++;
        if(pCc1Data->i32Cnt >= 1)
        {
            EN_SLOGD(TAG, "充电枪%d, 当前CC1电压:%05.2fV, CC1状态变化:%s--->%s", eGunIndex, f32Ucc1, u8Ucc1StateString[pCc1Data->eState], u8Ucc1StateString[pCc1Data->eStateTemp]);
            pCc1Data->eState = pCc1Data->eStateTemp;
            pCc1Data->i32Cnt = 0;
        }
    }
    else
    {
        pCc1Data->eStateTemp = eState;
        pCc1Data->i32Cnt = 0;
    }
}










/***************************************************************************************************
* Description                           :   CP状态值更新逻辑
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   增加滤波逻辑后 利用cp电压值更新cp状态
****************************************************************************************************/
void sAdcAppSampleCheckCpState(ePileGunIndex_t eGunIndex, f32 f32Ucp)
{
    eChgCpState_t eState;
    stAdcAppCpData_t *pCpData = &stAdcAppData.stCpData[eGunIndex];
    
    sAdcAppSampleUcpToState(f32Ucp, &eState);
    pCpData->f32Value = f32Ucp;
    if(pCpData->eCpState == eState)
    {
        return;
    }
    
    if(pCpData->eCpStateTemp == eState)
    {
        pCpData->i32CpCnt++;
        if(pCpData->i32CpCnt >= 3)
        {
            EN_SLOGD(TAG, "充电枪%d, 当前CP电压:%5.2fV, CP状态变化:%s--->%s", eGunIndex, f32Ucp, u8UcpStateString[pCpData->eCpState], u8UcpStateString[pCpData->eCpStateTemp]);
            pCpData->eCpState = pCpData->eCpStateTemp;
            pCpData->i32CpCnt = 0;
        }
    }
    else
    {
        pCpData->eCpStateTemp = eState;
        pCpData->i32CpCnt = 0;
    }
}
























