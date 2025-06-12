/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_fault_bms.h
* Description                           :   pile_cn_fault_bms-----bms相关故障告警检查注册
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-08-12
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_fault_bms_H_
#define _pile_cn_fault_bms_H_
#include "fault.h"











//最大超时次数
#define cBmsShankhandOverTime            (3)




//bms相关故障检测·结果
typedef enum
{
    eBmsCheckOk                         = 0,
    eBmsCheckfail                       = 1
}eBmsCheckResult;





//bms相关故障检测·结果
typedef enum
{
    eBsmCheckOk                         = 0,
    eBsmCheckHight                      = 1,
    eBsmCheckLow                        = 2
}eBsmErrCheckResult;















extern void sPileCnFaultChkRegister_Bms(void);









#endif






