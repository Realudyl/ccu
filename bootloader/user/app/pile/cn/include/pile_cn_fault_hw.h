/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_fault_hw.h
* Description                           :   pile_cn_fault_hw-----硬件相关故障告警检查注册
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-08-12
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_fault_hw_H_
#define _pile_cn_fault_hw_H_
#include "fault.h"

//电表数据故障判断周期
#define cMeterFaultCheckPeriod          (60000)


//枪过温降载的温度值
#define cGunTempWarn                   (95.0)


//枪过温故障的温度值
#define cGunTempFault                   (105.0)

//枪过压保护
#define cGunOutputOverVolt              (15)


//枪过流保护
#define cGunOutputOverCurLevel0         (3)
#define cGunOutputCurLevel0             (30)


//电子锁反馈异常检测时间(ms)
#define cElochFaultDetectTime           (5000)


//枪口高温检测时间(ms)
#define cGunHightTempDetectTime         (5000)

//过压保护检测时间(ms)
#define cOverVoltDetectTime             (5000)

//过流检测时间(ms)
#define cOverCurDetectTime              (5000)

//无输出电流异常检测时间(ms)
#define cNoOutputCurDetectTime          (5000)



//安全电压
#define cSafeVolt                       (60.0)

//安全电流
#define cSafeCur                        (5.0)



//检测结果
typedef enum
{
    eHWCheckOk                         = 0,
    eHWCheckfail                       = 1
}eHWCheckResult;


















extern void sPileCnFaultChkRegister_Hw(bool bIsSuperCharger);






#endif






























