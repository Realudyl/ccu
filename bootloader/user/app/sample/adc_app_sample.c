/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   adc_app_sample.c
* Description                           :   ADC采样计算
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-20
* notice                                :   
****************************************************************************************************/
#include "adc_app_sample.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "adc_app_sample";





stAdcAppData_t stAdcAppData;










bool sAdcAppSampleInit(void);
void sAdcAppSampleTask(void *pParam);
void sAdcAppSample(u32 u32Notify);
void sAdcAppSampleNtc(eAdcPin_t ePin, f32 f32Value);
void sAdcAppSamplePin(eAdcPin_t ePin);







//对外的api接口函数---------------------------------------------------------------
//获取指定采样管脚的采样值---根据 ePin 获取
bool sAdcAppSampleGetData(eAdcPin_t ePin, f32 *pValue);
eAdcAppFbState_t sAdcAppSampleGetFbState(eAdcPin_t ePin);


//获取指定采样管脚的采样值---根据 枪序号 获取
bool sAdcAppSampleGetUfbk(ePileGunIndex_t eGunIndex, f32 *pUfbk);
bool sAdcAppSampleGetGunDcOut(ePileGunIndex_t eGunIndex, f32 *pGunDcOut);
bool sAdcAppSampleGetGunTemp(ePileGunIndex_t eGunIndex, f32 *pTemp1, f32 *pTemp2);
bool sAdcAppSampleGetUiso(ePileGunIndex_t eGunIndex, f32 *pUiso);


//获取指定采样管脚(根据采样值换算后)的(xxx)值
eAdcAppFbState_t  sAdcAppSampleGetFbStateK(ePileGunIndex_t eGunIndex);
eAdcAppPpState_t  sAdcAppSampleGetPpState(ePileGunIndex_t eGunIndex);
eAdcAppPdState_t  sAdcAppSampleGetPdState(ePileGunIndex_t eGunIndex);
eAdcAppCc1State_t sAdcAppSampleGetCc1State(ePileGunIndex_t eGunIndex);
eChgCpState_t     sAdcAppSampleGetCpState(ePileGunIndex_t eGunIndex);
f32  sAdcAppSampleGetUpp(ePileGunIndex_t eGunIndex);
f32  sAdcAppSampleGetUpd(ePileGunIndex_t eGunIndex);
f32  sAdcAppSampleGetUcc1(ePileGunIndex_t eGunIndex);
f32  sAdcAppSampleGetUcp(ePileGunIndex_t eGunIndex);


//复用采样通道类型配置
bool sAdcAppSampleSetMuxType(ePileGunIndex_t eGunIndex, eAdcPinMuxType_t eType);

void sAdcAppGetGunTemp(void);
bool sAdcAppShellGunTemp(const stShellPkt_t *pkg);









stShellCmd_t stSellCmdGunTempCmd = 
{
    .pCmd       = "guntemp",
    .pFormat    = "格式: guntemp 0/1",
    .pFunction  = "功能: 打印枪温",
    .pRemarks   = "备注: 0--关闭所有枪温打印 1--打开所有枪温打印",
    .pFunc      = sAdcAppShellGunTemp,
};










/**********************************************************************************************
* Description                           :   adc采样初始化函数
* Author                                :   Hall
* modified Date                         :   2023-12-06
* notice                                :   
***********************************************************************************************/
bool sAdcAppSampleInit(void)
{
    bool bRst = false;
    i32  i32Cnt = 0;
    i32  i;
    
    if(eAdcPinMax != cAdcDrvPinNum)
    {
        //通道数组越界保护
        EN_SLOGE(TAG, "ADC采样通道配置错误, 驱动层通道数cAdcDrvPinNum:%d, 应用层通道数eAdcPinMax:%d", cAdcDrvPinNum, eAdcPinMax);
        return(false);
    }
    
    memset(&stAdcAppData, 0, sizeof(stAdcAppData));
    stAdcAppData.xTaskSample = NULL;
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        stAdcAppData.eMuxType[i] = eAdcPinMuxTypeMax;
    }
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdGunTempCmd);
    if(xTaskCreate(sAdcAppSampleTask, "sAdcAppSampleTask", (512), NULL, 20, &stAdcAppData.xTaskSample) == pdPASS)
    {
        bRst = true;
    }
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   ADC采样计算任务
* Author                                :   Hall
* modified Date                         :   2023-11-22
* notice                                :   
***********************************************************************************************/
void sAdcAppSampleTask(void *pPara)
{
    bool bRst;
    u32  u32Notify;
    
    
    //AD外设的初始化不能放在 sAdcAppSampleInit 函数内执行
    //因为该处执行初始化，AD中断会立即执行并在20ms内发送任务通知
    //此时本任务还没跑起来，将导致任务陷入异常 一直无法从阻塞中出来
    
    //要放在此处执行，要确保AD中断对本任务发送任务通知时，本任务已经跑起来了
    bRst = sAdcDrvInit(stAdcAppData.xTaskSample);
    EN_SLOGD(TAG, "任务建立:ADC采样计算任务, AD外设初始化结果:%d(0---失败, 1---成功)", bRst);
    
    while(1)
    {
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        sAdcAppSample(u32Notify);
        sAdcAppGetGunTemp();
    }
    
    vTaskDelete(NULL);
}






/**********************************************************************************************
* Description                           :   ADC采样计算
* Author                                :   Hall
* modified Date                         :   2024-03-19
* notice                                :   几个ADC外设的注入组中断和规则组中断都会周期性的释放任务通知
*                                           触发本函数执行
*                                           本函数每次仅需要计算该次触发所涉及的通道
*                                           这个逻辑是通过将 i32SumCnt 字段置0来完成的
***********************************************************************************************/
void sAdcAppSample(u32 u32Notify)
{
    i32  i;
    f32  f32Value;
    
    stAdcDrvDataUnit_t  *pDataUnit  = NULL;
    stAdcDrvDataTotal_t *pDataTotal = NULL;
    
    
    pDataTotal = sAdcDrvGetData();
    for(i = 0; i < eAdcPinMax; i++)
    {
        pDataUnit = &pDataTotal->stDataPin[i];
        if((pDataUnit->eMode != u32Notify)                                      //过滤 注入采样或规则采样类型不符 的通道
        || (pDataUnit->i32SumCnt == 0))                                         //过滤 数据未就绪的通道或除数为零 的通道
        {
            continue;
        }
        
        
        switch(pDataUnit->eType)
        {
        case eAdcDrvSampleTypeAvg:
            stAdcAppData.f32Value[i] = (pDataUnit->f32Sum / pDataUnit->i32SumCnt) * pDataUnit->f32Coef;
            break;
        case eAdcDrvSampleTypeRms:
            //被开方数为负数保护
            stAdcAppData.f32Value[i] = (pDataUnit->f32Sum > 0) ? sqrt(pDataUnit->f32Sum / pDataUnit->i32SumCnt) : 0;
            break;
        case eAdcDrvSampleTypeNtc:
            f32Value = (pDataUnit->f32Sum / pDataUnit->i32SumCnt) * pDataUnit->f32Coef;
            sAdcAppSampleNtc(i, f32Value);
            break;
        default:
            break;
        }
        
        //特定采样通道的额外数据处理逻辑
        sAdcAppSamplePin(i);
        
        //该通道完成计算后 清零 i32SumCnt 字段---非0值表示数据就绪
        pDataUnit->i32SumCnt = 0;
        
    }
}







 /**
  * @brief 通道数据计算。根据adc采样值与不同的通道硬件属性计算出需要的值
  * 
 * @param ePin 通道
 * @param f32Value adc计数值
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-05-22
 */
void sAdcAppSampleNtc(eAdcPin_t ePin, f32 f32Value)
{
    f32  f32Ohm    = 0;
    f32  f32Temp   = 0;
    
    switch(ePin)
    {
    /*case xxxxx:
        //NTC通道温度计算
        if((f32Value - 1) != 0)
        {
            f32Ohm = (4096.0f / (f32Value - 1)) * 10.0f * 1000.0f;
            if(sGetNtcTemp(f32Ohm, &f32Temp))
            {
                stAdcAppData.f32Value[ePin] = f32Temp;
            }
            else
            {
                //若温度超出范围,则设置一个很大的值，表示异常
                stAdcAppData.f32Value[ePin] = 1000000.0f;
            }
        }
        break;*/
    case eAdcPin_A_GUN_TEMP1:
    case eAdcPin_A_GUN_TEMP2:
    case eAdcPin_B_GUN_TEMP1:
    case eAdcPin_B_GUN_TEMP2:
        //PT1000温度采样
        if(f32Value != 0)
        {
            f32Ohm = ((6330.0f / f32Value) - 1) * 1000.0f;
            f32Temp = (f32Ohm - 1000.0f) / 3.85f;
            stAdcAppData.f32Value[ePin] = f32Temp;
        }
        break;
    default:
        break;
    }
}








/**********************************************************************************************
* Description                           :   ADC采样计算---特定采样通道的额外数据处理逻辑
* Author                                :   Hall
* modified Date                         :   2024-05-13
* notice                                :   一些通道的采样值是与充电枪有关的
*                                           本函数将这些通道值与充电枪绑定
***********************************************************************************************/
void sAdcAppSamplePin(eAdcPin_t ePin)
{
    f32  f32Value = stAdcAppData.f32Value[ePin];
    
    switch(ePin)
    {
    case eAdcPin_A_FB_K:                                                        //以下枪1
        stAdcAppData.f32Ufbk[ePileGunIndex0] = f32Value;
        sAdcAppSampleCheckFbState(&stAdcAppData.stFbDataK[ePileGunIndex0], f32Value);
        break;
    case eAdcPin_A_GUN_DC_P:
        sAdcAppSampleCheckGunDcOut(ePileGunIndex0, f32Value);
        break;
    case eAdcPin_A_GUN_TEMP1:
        stAdcAppData.f32GunTemp1[ePileGunIndex0] = f32Value;
        break;
    case eAdcPin_A_GUN_TEMP2:
        stAdcAppData.f32GunTemp2[ePileGunIndex0] = f32Value;
        break;
    case eAdcPin_A_ISO_AD:
        stAdcAppData.f32Uiso[ePileGunIndex0] = f32Value;
        break;
    case eAdcPin_A_PP_Pd_AD:
        stAdcAppData.f32Upppd[ePileGunIndex0] = f32Value;
        if(stAdcAppData.eMuxType[ePileGunIndex0] == eAdcPinMuxTypeCcs)
        {
            sAdcAppSampleCheckPpState(ePileGunIndex0, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex0] == eAdcPinMuxTypeCha)
        {
            sAdcAppSampleCheckPdState(ePileGunIndex0, f32Value);
        }
        break;
    case eAdcPin_A_CC1_CP_J_AD:
        stAdcAppData.f32Ucpcc[ePileGunIndex0] = f32Value;
        if(stAdcAppData.eMuxType[ePileGunIndex0] == eAdcPinMuxTypeGbt)
        {
            sAdcAppSampleCheckCc1State(ePileGunIndex0, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex0] == eAdcPinMuxTypeCcs)
        {
            sAdcAppSampleCheckCpState(ePileGunIndex0, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex0] == eAdcPinMuxTypeCha)
        {
            
        }
        break;
        
        
        
    case eAdcPin_B_FB_K:                                                        //以下枪2
        stAdcAppData.f32Ufbk[ePileGunIndex1] = f32Value;
        sAdcAppSampleCheckFbState(&stAdcAppData.stFbDataK[ePileGunIndex1], f32Value);
        break;
    case eAdcPin_B_GUN_DC_P:
        sAdcAppSampleCheckGunDcOut(ePileGunIndex1, f32Value);
        break;
    case eAdcPin_B_GUN_TEMP1:
        stAdcAppData.f32GunTemp1[ePileGunIndex1] = f32Value;
        break;
    case eAdcPin_B_GUN_TEMP2:
        stAdcAppData.f32GunTemp2[ePileGunIndex1] = f32Value;
        break;
    case eAdcPin_B_ISO_AD:
        stAdcAppData.f32Uiso[ePileGunIndex1] = f32Value;
        break;
    case eAdcPin_B_PP_Pd_AD:
        stAdcAppData.f32Upppd[ePileGunIndex1] = f32Value;
        if(stAdcAppData.eMuxType[ePileGunIndex1] == eAdcPinMuxTypeCcs)
        {
            sAdcAppSampleCheckPpState(ePileGunIndex1, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex1] == eAdcPinMuxTypeCha)
        {
            sAdcAppSampleCheckPdState(ePileGunIndex1, f32Value);
        }
        break;
    case eAdcPin_B_CC1_CP_J_AD:
        stAdcAppData.f32Ucpcc[ePileGunIndex1] = f32Value;
        if(stAdcAppData.eMuxType[ePileGunIndex1] == eAdcPinMuxTypeGbt)
        {
            sAdcAppSampleCheckCc1State(ePileGunIndex1, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex1] == eAdcPinMuxTypeCcs)
        {
            sAdcAppSampleCheckCpState(ePileGunIndex1, f32Value);
        }
        else if(stAdcAppData.eMuxType[ePileGunIndex1] == eAdcPinMuxTypeCha)
        {
            
        }
        break;
        
        
    case eAdcPin_FB_X:                                                          //以下公用
        sAdcAppSampleCheckFbState(&stAdcAppData.stFbDataX, f32Value);
        break;
    case eAdcPin_FB_Y:
        sAdcAppSampleCheckFbState(&stAdcAppData.stFbDataY, f32Value);
        break;
    case eAdcPin_FB_Z:
        sAdcAppSampleCheckFbState(&stAdcAppData.stFbDataZ, f32Value);
        break;
        
    default:
        break;
    }
    
}











/**********************************************************************************************
* Description                           :   获取指定采样管脚的采样值
* Author                                :   Hall
* modified Date                         :   2024-02-23
* notice                                :   
***********************************************************************************************/
bool sAdcAppSampleGetData(eAdcPin_t ePin, f32 *pValue)
{
    if((ePin >= eAdcPinMax) || (pValue == NULL))
    {
        return(false);
    }
    
    (*pValue) = stAdcAppData.f32Value[ePin];
    
    return(true);
}








/***************************************************************************************************
* Description                           :   获取指定 ePin 反馈信号的状态值
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
eAdcAppFbState_t sAdcAppSampleGetFbState(eAdcPin_t ePin)
{
    eAdcAppFbState_t eState;
    
    switch(ePin)
    {
    case eAdcPin_A_FB_K:
        eState = stAdcAppData.stFbDataK[ePileGunIndex0].eState;
        break;
    case eAdcPin_B_FB_K:
        eState = stAdcAppData.stFbDataK[ePileGunIndex1].eState;
        break;
    case eAdcPin_FB_X:
        eState = stAdcAppData.stFbDataX.eState;
        break;
    case eAdcPin_FB_Y:
        eState = stAdcAppData.stFbDataY.eState;
        break;
    case eAdcPin_FB_Z:
        eState = stAdcAppData.stFbDataZ.eState;
        break;
    default:
        eState = eAdcAppFbStateMax;
        break;
    }
    
    return eState;
}







/***************************************************************************************************
* Description                           :   获取指定枪 接触器组反馈
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
****************************************************************************************************/
bool sAdcAppSampleGetUfbk(ePileGunIndex_t eGunIndex, f32 *pUfbk)
{
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pUfbk != NULL)
    {
        (*pUfbk) = stAdcAppData.f32Ufbk[eGunIndex];
    }
    
    return true;
}







/***************************************************************************************************
* Description                           :   获取指定枪 外侧电压及其稳定性标志
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
bool sAdcAppSampleGetGunDcOut(ePileGunIndex_t eGunIndex, f32 *pGunDcOut)
{
    if((eGunIndex < ePileGunIndexNum) && (pGunDcOut != NULL))
    {
        (*pGunDcOut) = stAdcAppData.stGunDcOutData[eGunIndex].f32Udc;
    }
    
    return ((eGunIndex >= ePileGunIndexNum) ? false : stAdcAppData.stGunDcOutData[eGunIndex].bStableFlag);
}







/***************************************************************************************************
* Description                           :   获取指定枪 枪头温度
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
****************************************************************************************************/
bool sAdcAppSampleGetGunTemp(ePileGunIndex_t eGunIndex, f32 *pTemp1, f32 *pTemp2)
{
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pTemp1 != NULL)
    {
        (*pTemp1) = stAdcAppData.f32GunTemp1[eGunIndex];
    }
    
    if(pTemp2 != NULL)
    {
        (*pTemp2) = stAdcAppData.f32GunTemp2[eGunIndex];
    }
    
    return true;
}









/***************************************************************************************************
* Description                           :   获取指定枪 ISO绝缘采样电压
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
bool sAdcAppSampleGetUiso(ePileGunIndex_t eGunIndex, f32 *pUiso)
{
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pUiso != NULL)
    {
        (*pUiso) = stAdcAppData.f32Uiso[eGunIndex];
    }
    
    return true;
}








/***************************************************************************************************
* Description                           :   获取指定枪 接触器组反馈 状态
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
****************************************************************************************************/
eAdcAppFbState_t sAdcAppSampleGetFbStateK(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eAdcAppFbStateMax : stAdcAppData.stFbDataK[eGunIndex].eState);
}










/***************************************************************************************************
* Description                           :   获取指定枪 PP 状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
eAdcAppPpState_t sAdcAppSampleGetPpState(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eAdcAppPpStateMax : stAdcAppData.stPpData[eGunIndex].eState);
}







/***************************************************************************************************
* Description                           :   获取指定枪 PD 状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
eAdcAppPdState_t sAdcAppSampleGetPdState(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eAdcAppPdStateMax : stAdcAppData.stPdData[eGunIndex].eState);
}







/***************************************************************************************************
* Description                           :   获取指定枪 CC1 状态
* Author                                :   Hall
* Creat Date                            :   2024-05-17
* notice                                :   
****************************************************************************************************/
eAdcAppCc1State_t sAdcAppSampleGetCc1State(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eAdcAppCc1StateMax : stAdcAppData.stCc1Data[eGunIndex].eState);
}










/***************************************************************************************************
* Description                           :   获取指定枪 CP状态值
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
eChgCpState_t sAdcAppSampleGetCpState(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eChgCpStateMax : stAdcAppData.stCpData[eGunIndex].eCpState);
}





/***************************************************************************************************
* Description                           :   获取指定枪 Upp
* Author                                :   Hall
* Creat Date                            :   2024-05-28
* notice                                :   
****************************************************************************************************/
f32 sAdcAppSampleGetUpp(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? 0 : stAdcAppData.stPpData[eGunIndex].f32Value);
}





/***************************************************************************************************
* Description                           :   获取指定枪 Upd
* Author                                :   Hall
* Creat Date                            :   2024-05-28
* notice                                :   
****************************************************************************************************/
f32 sAdcAppSampleGetUpd(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? 0 : stAdcAppData.stPdData[eGunIndex].f32Value);
}





/***************************************************************************************************
* Description                           :   获取指定枪 Ucc1
* Author                                :   Hall
* Creat Date                            :   2024-05-28
* notice                                :   
****************************************************************************************************/
f32 sAdcAppSampleGetUcc1(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? 0 : stAdcAppData.stCc1Data[eGunIndex].f32Value);
}





/***************************************************************************************************
* Description                           :   获取指定枪 Ucp
* Author                                :   Hall
* Creat Date                            :   2024-05-28
* notice                                :   
****************************************************************************************************/
f32 sAdcAppSampleGetUcp(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? 0 : stAdcAppData.stCpData[eGunIndex].f32Value);
}








/***************************************************************************************************
* Description                           :   设置指定枪 复用采样通道的类型
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sAdcAppSampleSetMuxType(ePileGunIndex_t eGunIndex, eAdcPinMuxType_t eType)
{
    if((eGunIndex >= ePileGunIndexNum) || (eType >= eAdcPinMuxTypeMax))
    {
        return false;
    }
    
    stAdcAppData.eMuxType[eGunIndex] = eType;
    
    return true;
}





/***************************************************************************************************
* Description                           :   枪温获取任务
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
void sAdcAppGetGunTemp(void)
{
    static u32 u32StartTimeStamp = 0; 
    f32 f32Value1, f32Value2;
    u8 i;
    
    if(stAdcAppData.bGunTempPrtFlag)
    {
        if(xTaskGetTickCount() - u32StartTimeStamp >= 5000)
        {
            u32StartTimeStamp = xTaskGetTickCount();
            for(i = 0; i < ePileGunIndexNum; i++)
            {
                sAdcAppSampleGetGunTemp((ePileGunIndex_t)i, &f32Value1, &f32Value2);
                EN_SLOGI(TAG, "枪 %d：Temp1 %05.1f, Temp2 %05.1f", i, f32Value1, f32Value2);
            }
        }
    }
}





/***************************************************************************************************
* Description                           :   枪温度打印控制
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   定时5s循环打印枪温
****************************************************************************************************/
bool sAdcAppShellGunTemp(const stShellPkt_t *pkg)
{
    bool bFlag;
    bool bRst = true;
    
    // 根据输入参数决定是否开启打印
    bFlag = (atoi(pkg->para[0]) == 1) ? true : false;
    stAdcAppData.bGunTempPrtFlag = bFlag;
    
    return(bRst);
}












