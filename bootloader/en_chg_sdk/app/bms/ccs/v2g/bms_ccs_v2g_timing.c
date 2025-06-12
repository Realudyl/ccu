/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_timing.c
* Description                           :   DIN70121 协议实现 之时序部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-06
* notice                                :   
****************************************************************************************************/
#include "io_pwm_app.h"

#include "bms_ccs_v2g_opt.h"
#include "bms_ccs_v2g_timing.h"
#include "bms_ccs_v2g_din_api.h"
#include "bms_ccs_v2g_iso1_api.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_timing";






extern stV2gOptCache_t *pV2gOptCache[cMse102xDevNum];







void sV2gInitTiming(i32 i32DevIndex, ePileGunIndex_t eGunIndex);
void sV2gResetTimingLogic(i32 i32DevIndex);


//timer reset------定时器复位
bool sV2gSeccCommunicationSetupPerformanceTimerRst(i32 i32DevIndex);
void sV2gSeccSequenceTimerRst(i32 i32DevIndex);
void sV2gSeccCpStateDetectionTimerRst(i32 i32DevIndex, eChgCpState_t eCpState);


//timer stop------定时器停止
bool sV2gSeccSequenceTimerStop(i32 i32DevIndex);
bool sV2gSeccCpStateDetectionTimerStop(i32 i32DevIndex);

//timer start-----定时器启动
void sV2gSeccCommunicationSetupPerformanceTimerStart(i32 i32DevIndex);
void sV2gSeccSequenceTimerStart(i32 i32DevIndex);
void sV2gSeccCpStateDetectionTimerStart(i32 i32DevIndex, eChgCpState_t eCpState);

//timer task------定时器（计时）任务
void sV2gSeccCommunicationSetupPerformanceTimerTask(void *pParam);
void sV2gSeccSequenceTimerTask(void *pParam);
void sV2gSeccCpStateDetectionTimerTask(void *pParam);












/***************************************************************************************************
* Description                           :   v2g协议 时序部分初始化
* Author                                :   Hall
* Creat Date                            :   2024-01-26
* notice                                :   
****************************************************************************************************/
void sV2gInitTiming(i32 i32DevIndex, ePileGunIndex_t eGunIndex)
{
    i32 *pDevIndex = (i32 *)MALLOC(sizeof(i32));
    stV2gOptCache_t *pCache = pV2gOptCache[i32DevIndex];
    
    
    pCache->xEvtTiming = xEventGroupCreate();
    pCache->eGunIndex = eGunIndex;
    xEventGroupClearBits(pCache->xEvtTiming, cEvt_All);
    
    
    *pDevIndex = i32DevIndex;
    
    //TEST:新增定时器(2024-04-01) ————> V2G_SECC_CommunicationSetup_Performance_Time(18s) ====> [V2G2-714] [V2G2-716]
    xTaskCreate(sV2gSeccCommunicationSetupPerformanceTimerTask,     "sV2gSeccCommunicationSetupPerformanceTimerTask",   (512), pDevIndex, 21, &pCache->xTaskSeccCommunicationSetupPerformance);
    xTaskCreate(sV2gSeccSequenceTimerTask,                          "sV2gSeccSequenceTimerTask",                        (512), pDevIndex, 21, &pCache->xTaskSeccSequenceTimer);
    xTaskCreate(sV2gSeccCpStateDetectionTimerTask,                  "sV2gSeccCpStateDetectionTimerTask",                (512), pDevIndex, 21, &pCache->xTaskSeccCpStateDetection);
}







/***************************************************************************************************
* Description                           :   v2g协议 时序部分 复位时序逻辑
* Author                                :   Hall
* Creat Date                            :   2024-02-02
* notice                                :   状态机切换到需要使用时序逻辑的状态之前 应该复位时序逻辑的所有事件标志
****************************************************************************************************/
void sV2gResetTimingLogic(i32 i32DevIndex)
{
    xTaskNotify(pV2gOptCache[i32DevIndex]->xTaskSeccSequenceTimer, cNotify_ResetTimingLogic, eSetBits);
}







/*******************************************************************************
 * @FunctionName   :      sV2gSeccCommunicationSetupPerformanceTimerRst
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月1日  16:49:46
 * @Description    :      v2g协议 时序部分 复位 V2G_SECC_CommunicationSetup_Performance_Time 定时器
 * @Input          :      i32DevIndex 
 * @Return         :      
*******************************************************************************/
bool sV2gSeccCommunicationSetupPerformanceTimerRst(i32 i32DevIndex)
{
    xEventGroupSetBits(pV2gOptCache[i32DevIndex]->xEvtTiming, cEvt_V2G_SECC_CommunicationSetup_Performance_Time);
    
    return(true);
}






/***************************************************************************************************
* Description                           :   v2g协议 时序部分 复位 V2G_SECC_Sequence_Timer 定时器
* Author                                :   Hall
* Creat Date                            :   2024-01-31
* notice                                :   
****************************************************************************************************/
void sV2gSeccSequenceTimerRst(i32 i32DevIndex)
{
    xEventGroupSetBits(pV2gOptCache[i32DevIndex]->xEvtTiming, cEvt_V2G_SECC_Sequence_Timer);
}







/***************************************************************************************************
* Description                           :   v2g协议 时序部分 复位 V2G_SECC_CPState_Detection_Timeout 定时器
* Author                                :   Hall
* Creat Date                            :   2024-03-21
* notice                                :   
****************************************************************************************************/
void sV2gSeccCpStateDetectionTimerRst(i32 i32DevIndex, eChgCpState_t eCpState)
{
    xEventGroupSetBits(pV2gOptCache[i32DevIndex]->xEvtTiming, (1 << (eCpState)));
}










/***************************************************************************************************
* Description                           :   v2g协议 时序部分 提前结束 V2G_SECC_Sequence_Timer 定时器计时
* Author                                :   Hall
* Creat Date                            :   2024-01-30
* notice                                :   见状态机转换逻辑图，
*                                           此函数是图中的一个切换动作
****************************************************************************************************/
bool sV2gSeccSequenceTimerStop(i32 i32DevIndex)
{
    xEventGroupSetBits(pV2gOptCache[i32DevIndex]->xEvtTiming, cEvt_V2G_SECC_Sequence_Timer_s);
    
    return(true);
}








/***************************************************************************************************
* Description                           :   v2g协议 时序部分 提前结束 V2G_SECC_Sequence_Timer 定时器计时
* Author                                :   Hall
* Creat Date                            :   2024-01-30
* notice                                :   见状态机转换逻辑图，
*                                           此函数是图中的一个切换动作
****************************************************************************************************/
bool sV2gSeccCpStateDetectionTimerStop(i32 i32DevIndex)
{
    xEventGroupSetBits(pV2gOptCache[i32DevIndex]->xEvtTiming, cEvt_V2G_SECC_CPState_Detection_Timeout_s);
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gSeccCommunicationSetupPerformanceTimerStart
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月1日  16:31:15
 * @Description    :      v2g协议 时序部分 启动 V2G_SECC_CommunicationSetup_Performance_Time 定时器
 * @Input          :      i32DevIndex 
*******************************************************************************/
void sV2gSeccCommunicationSetupPerformanceTimerStart(i32 i32DevIndex)
{
    xTaskNotify(pV2gOptCache[i32DevIndex]->xTaskSeccCommunicationSetupPerformance, cNotify_CommunicationSetup_Performance, eSetBits);
}









/***************************************************************************************************
* Description                           :   v2g协议 时序部分 启动 V2G_SECC_Sequence_Timer 定时器
* Author                                :   Hall
* Creat Date                            :   2024-01-26
* notice                                :   
****************************************************************************************************/
void sV2gSeccSequenceTimerStart(i32 i32DevIndex)
{
    xTaskNotify(pV2gOptCache[i32DevIndex]->xTaskSeccSequenceTimer, cNotify_Sequence, eSetBits);
}






/***************************************************************************************************
* Description                           :   v2g协议 时序部分 启动 V2G_SECC_CPState_Detection_Timeout 定时器
* Author                                :   Hall
* Creat Date                            :   2024-01-29
* notice                                :   
****************************************************************************************************/
void sV2gSeccCpStateDetectionTimerStart(i32 i32DevIndex, eChgCpState_t eCpState)
{
    xTaskNotify(pV2gOptCache[i32DevIndex]->xTaskSeccCpStateDetection, eCpState, eSetValueWithOverwrite);
}











/*******************************************************************************
 * @FunctionName   :      sV2gSeccCommunicationSetupPerformanceTimerTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月1日  16:45:28
 * @Description    :      v2g协议 时序部分 V2G_SECC_CommunicationSetup_Performance_Time 定时器监视任务
 * @Input          :      pParam      
*******************************************************************************/
void sV2gSeccCommunicationSetupPerformanceTimerTask(void *pParam)
{
    i32  i32Ret;
    u32  u32Notify;
    i32  *pDevIndex;
    EventBits_t xEvtBitWait;
    
    stV2gOptCache_t *pCache = NULL;
    
    pDevIndex = pParam;
    EN_SLOGI(TAG, "任务建立:SECC[%d], V2G_SECC_CommunicationSetup_Performance_Time 定时器监视任务", (*pDevIndex));
    
    pCache = pV2gOptCache[*pDevIndex];
    
    while(1)
    {
        //等待接收通知数据
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        
        xEventGroupWaitBits(pCache->xEvtTiming, cEvt_V2G_SECC_CommunicationSetup_Performance_Time, pdTRUE, pdFALSE, 0);
        if((u32Notify & cNotify_CommunicationSetup_Performance) != 0)
        {
            EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CommunicationSetup_Performance_Time 定时器监视任务开始", (*pDevIndex));
            xEvtBitWait = cEvt_V2G_SECC_CommunicationSetup_Performance_Time;
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, V2G_SECC_CommunicationSetup_Performance_Time / portTICK_RATE_MS);
            if((i32Ret & cEvt_V2G_SECC_CommunicationSetup_Performance_Time) != 0)
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CommunicationSetup_Performance_Time 定时器监视任务结束", (*pDevIndex));
                //本次定时器结束计时, 已经收到后续消息
            }
            else
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CommunicationSetup_Performance_Time 定时器超时:%ds", (*pDevIndex), V2G_SECC_CommunicationSetup_Performance_Time / 1000);
                
                //TEST ————> <ISO15118-2:2014.pdf>@8.7.3.3 [V2G2-716] 超时需终止TLS/TCP连接
                pCache->pCbSwitchV2gToShutdownEmg((*pDevIndex));
            }
        }
        
    }
    
    vTaskDelete(NULL);
}







/***************************************************************************************************
* Description                           :   v2g协议 时序部分 V2G_SECC_Sequence_Timer 定时器监视任务
* Author                                :   Hall
* Creat Date                            :   2023-06-13
* notice                                :   
****************************************************************************************************/
void sV2gSeccSequenceTimerTask(void *pParam)
{
    i32  i32Ret;
    u32  u32Notify;
    i32  *pDevIndex;
    EventBits_t xEvtBitWait;
    
    stV2gOptCache_t *pCache = NULL;
    
    pDevIndex = pParam;
    EN_SLOGI(TAG, "任务建立:SECC[%d], V2G_SECC_Sequence_Timer 定时器监视任务", (*pDevIndex));
    
    pCache = pV2gOptCache[*pDevIndex];
    
    while(1)
    {
        //等待接收通知数据
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        
        //复位时序逻辑通知
        if((u32Notify & cNotify_ResetTimingLogic) != 0)
        {
            xEventGroupWaitBits(pCache->xEvtTiming, cEvt_All, pdTRUE, pdFALSE, 1 / portTICK_RATE_MS);
            EN_SLOGI(TAG, "SECC[%d], V2G_SECC_Sequence_Timer 定时器 清除所有事件标志", (*pDevIndex));
        }
        
        
        //消息序列超时检查通知
        if((u32Notify & cNotify_Sequence) != 0)
        {
            xEvtBitWait = (cEvt_CpAbnormal | cEvt_V2G_SECC_Sequence_Timer | cEvt_V2G_SECC_Sequence_Timer_s);
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, V2G_SECC_Sequence_Timeout / portTICK_RATE_MS);
            if((i32Ret & cEvt_CpAbnormal) != 0)
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_Sequence_Timer 定时器 提前结束计时, 原因是CP变化", (*pDevIndex));
            }
            else if((i32Ret & cEvt_V2G_SECC_Sequence_Timer_s) != 0)
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_Sequence_Timer 定时器 提前结束计时, 原因是收到停止计时事件", (*pDevIndex));
            }
            else if((i32Ret & cEvt_V2G_SECC_Sequence_Timer) != 0)
            {
                //本次定时器结束计时, 已经收到后续消息
            }
            else
            {
                pCache->pCbSwitchV2gToShutdownEmg((*pDevIndex));
                sV2gSeccCpStateDetectionTimerStart((*pDevIndex), eChgCpStateMax);
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_Sequence_Timer 定时器超时:%ds", (*pDevIndex), V2G_SECC_Sequence_Timeout / 1000);
            }
        }
        
    }
    
    vTaskDelete(NULL);
}











/***************************************************************************************************
* Description                           :   v2g协议 时序部分 V2G_SECC_CPState_Detection_Timeout 定时器任务
* Author                                :   Hall
* Creat Date                            :   2023-06-25
*                                           该定时器需要持续检查 Ucp电压，应在限定时间内达到预期状态
****************************************************************************************************
* Modify Date                           :   2024-04-10
*                                           增加一个任务通知功能，SessionStop Res 会话发出以后会
*                                           发出本通知，执行关闭CP振荡器的逻辑
****************************************************************************************************/
void sV2gSeccCpStateDetectionTimerTask(void *pParam)
{
    i32  i32Ret;
    u32  u32Notify;
    i32  *pDevIndex;
    EventBits_t xEvtBitWait;
    
    stV2gOptCache_t *pCache = NULL;
    
    pDevIndex = pParam;
    EN_SLOGI(TAG, "任务建立:SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器监视任务", (*pDevIndex));
    
    pCache = pV2gOptCache[*pDevIndex];
    
    while(1)
    {
        //等待接收通知数据
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        
        //CP状态B超时检查通知
        if(u32Notify == eChgCpStateB)
        {
            EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态B超时检查 开启计时", (*pDevIndex));
            
            xEvtBitWait = cEvt_CpStateB | cEvt_V2G_SECC_Sequence_Timer_s;
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, V2G_SECC_CPState_Detection_Timeout / portTICK_RATE_MS);
            if((i32Ret & cEvt_CpStateB) != 0)
            {
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdWeldingDetection, dinresponseCodeType_OK);
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdSessionStop, dinresponseCodeType_OK);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_OK);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdSessionStop, iso1responseCodeType_OK);
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态B超时检查 结束计时", (*pDevIndex));
            }
            else if((i32Ret & cEvt_V2G_SECC_Sequence_Timer_s) != 0)
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态B超时检查 提前结束计时, 原因是收到停止计时事件", (*pDevIndex));
            }
            else
            {
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdWeldingDetection, dinresponseCodeType_FAILED);
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdSessionStop, dinresponseCodeType_FAILED);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_FAILED);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdSessionStop, iso1responseCodeType_FAILED);
                sV2gOptSetResponseCodeForCpStateDetectionTimeout(*pDevIndex, cEvt_CpStateB);
                pCache->pCbSwitchV2gToShutdownEmg((*pDevIndex));
                sV2gSeccCpStateDetectionTimerStart((*pDevIndex), eChgCpStateMax);
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态B超时检查 超时:%0.1fs", (*pDevIndex), ((f32)V2G_SECC_CPState_Detection_Timeout) / 1000);
            }
        }
        
        
        //CP状态C/D超时检查通知
        if((u32Notify == eChgCpStateC) || (u32Notify == eChgCpStateD))
        {
            EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态C/D超时检查 开启计时", (*pDevIndex));
            
            xEvtBitWait = cEvt_CpStateC | cEvt_CpStateD | cEvt_V2G_SECC_Sequence_Timer_s;
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, V2G_SECC_CPState_Detection_Timeout / portTICK_RATE_MS);
            if((i32Ret & (cEvt_CpStateC | cEvt_CpStateD)) != 0)
            {
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_OK);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdCableCheck, dinresponseCodeType_OK);
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态C/D超时检查 结束计时", (*pDevIndex));
            }
            else if((i32Ret & cEvt_V2G_SECC_Sequence_Timer_s) != 0)
            {
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态C/D超时检查 提前结束计时, 原因是收到停止计时事件", (*pDevIndex));
            }
            else
            {
                sV2gDinSet_ResponseCode(*pDevIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_FAILED);
                sV2gIso1Set_ResponseCode(*pDevIndex, eV2gIso1MsgIdCableCheck, iso1responseCodeType_FAILED);
                sV2gOptSetResponseCodeForCpStateDetectionTimeout(*pDevIndex, eChgCpStateC);
                pCache->pCbSwitchV2gToShutdownEmg((*pDevIndex));
                sV2gSeccCpStateDetectionTimerStart((*pDevIndex), eChgCpStateMax);
                EN_SLOGI(TAG, "SECC[%d], V2G_SECC_CPState_Detection_Timeout 定时器 CP状态C/D超时检查 超时:%0.1fs", (*pDevIndex), ((f32)V2G_SECC_CPState_Detection_Timeout) / 1000);
            }
        }
        
        
        //关闭CP振荡器通知
        if(u32Notify == eChgCpStateMax)
        {
            //eChgCpStateMax 值通知关闭CP振荡器，但关闭前需要延时保持
            vTaskDelay(V2G_SECC_CPOscillator_Retain_Time / portTICK_RATE_MS);
            pCache->pCpPwmControl(pCache->eGunIndex, false);
        }
        else if(u32Notify == (eChgCpStateMax + 1))
        {
            //(eChgCpStateMax + 1) 值通知关闭CP振荡器，关闭前无需延时保持
            pCache->pCpPwmControl(pCache->eGunIndex, false);
        }
    }
    
    vTaskDelete(NULL);
}












