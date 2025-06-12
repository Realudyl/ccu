/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_timing.h
* Description                           :   DIN70121 协议实现 之时序部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-06
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_timing_h_
#define _bms_ccs_v2g_timing_h_
#include "en_common.h"
#include "adc_app_sample.h"

#include "mse102x_drv.h"







//DIN70121 时序相关定义



//根据<DIN SPEC 70121 2014-12 212P.pdf> @9.6@表75  定义
#define V2G_SECC_Sequence_Timeout       (60 * 1000)


//根据<DIN SPEC 70121 2014-12 212P.pdf> @9.6@表77  定义
#define V2G_SECC_CPState_Detection_Timeout  (1500)

//V2G_SECC_CPOscillator_Retain_Time 
//标准要求1.5 S，但实测经常提前中断，可以适当提高时间，测试要求是 4.5S，实测3s最佳
#define V2G_SECC_CPOscillator_Retain_Time   (3000)


//V2G_SECC_WeldingDetection_Timer 
//防止粘连检测进入无限循环 CharIN要求,实际的要求是20S超时，由于使用的是计数，时间不精准，每个回复都有1.5S的延时，因此采用12次
#define V2G_SECC_WeldingDetection_Timer (12)






//ISO15118 时序相关定义



//根据<ISO 15118-2:2014.pdf> @8.7.3.1@表111  定义
#define V2G_SECC_CommunicationSetup_Performance_Time                (18 * 1000)









//事件组事件定义
#define cEvt_All                                                (0x00ffffff)                            //所有事件
#define cEvt_None                                               (0)                                     //没有事件
#define cEvt_CpStateA                                           (1 << (eChgCpStateA))                   //State A (+12 V) 电动汽车没有连接到充电桩
#define cEvt_CpStateB                                           (1 << (eChgCpStateB))                   //State B (+9V)   电动汽车连接到充电桩，但没有开始充电
#define cEvt_CpStateC                                           (1 << (eChgCpStateC))                   //State C (+6V)   电动汽车连接到充电桩上，准备好开始充电 （不需要通风 ventilation）
#define cEvt_CpStateD                                           (1 << (eChgCpStateD))                   //State D (+3V)   电动汽车连接到充电桩上，准备好开始充电 （  需要通风 ventilation）
#define cEvt_CpStateE                                           (1 << (eChgCpStateE))                   //State E (+0V)   电网供电有问题或未连接到电网。
#define cEvt_CpStateF                                           (1 << (eChgCpStateF))                   //State F (-12V)  充电桩未准备好。
#define cEvt_V2G_SECC_CommunicationSetup_Performance_Time       (1 << (eChgCpStateF + 1))               //V2G_SECC_CommunicationSetup_Performance_Time 定时器
#define cEvt_V2G_SECC_Sequence_Timer                            (1 << (eChgCpStateF + 2))               //V2G_SECC_Sequence_Timer 定时器
#define cEvt_V2G_SECC_Sequence_Timer_s                          (1 << (eChgCpStateF + 3))               //V2G_SECC_Sequence_Timer 定时器停止计时---发生一些事件时 会提前结束计时
#define cEvt_V2G_SECC_CPState_Detection_Timeout_s               (1 << (eChgCpStateF + 4))               //V2G_SECC_CPState_Detection_Timeout 定时器停止计时---发生一些事件时 会提前结束计时

//CP断开或异常事件
#define cEvt_CpAbnormal                                         (cEvt_CpStateA | cEvt_CpStateE | cEvt_CpStateF)


//V2G_SECC_Sequence_Timeout 任务通知位定义
#define cNotify_CommunicationSetup_Performance                  (1 << 0)                                //TLS超时检查
#define cNotify_ResetTimingLogic                                (1 << 1)                                //复位时序逻辑
#define cNotify_Sequence                                        (1 << 2)                                //消息序列超时检查
















extern void sV2gInitTiming(i32 i32DevIndex, ePileGunIndex_t eGunIndex);
extern void sV2gResetTimingLogic(i32 i32DevIndex);
extern bool sV2gTimerTimeoutGet(i32 i32DevIndex);



//timer stop------定时器复位
extern bool sV2gSeccCommunicationSetupPerformanceTimerRst(i32 i32DevIndex);
extern void sV2gSeccSequenceTimerRst(i32 i32DevIndex);
extern void sV2gSeccCpStateDetectionTimerRst(i32 i32DevIndex, eChgCpState_t eCpState);

//timer stop------定时器停止
extern bool sV2gSeccSequenceTimerStop(i32 i32DevIndex);
extern bool sV2gSeccCpStateDetectionTimerStop(i32 i32DevIndex);

//timer start-----定时器启动
extern void sV2gSeccCommunicationSetupPerformanceTimerStart(i32 i32DevIndex);
extern void sV2gSeccSequenceTimerStart(i32 i32DevIndex);
extern void sV2gSeccCpStateDetectionTimerStart(i32 i32DevIndex, eChgCpState_t eCpState);






#endif








