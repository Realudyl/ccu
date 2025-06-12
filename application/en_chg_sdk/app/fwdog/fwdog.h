/**
 * @file fwdog.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 
 * @version 0.1
 * @date 2024-08-02
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */
#pragma once

#include "en_chg_sdk_cfg.h"
#include "en_common.h"

//看门狗喂狗任务优先级
#ifndef cSdkFwdogPriv
#define cFwdogPriv 10u
#else
#define cFwdogPriv cSdkFwdogPriv
#endif

//看门狗喂狗任务周期
#ifndef cSdkFwdogCycleMs
#define cFwdogCycleMs 2000u
#else
#define cFwdogCycleMs cSdkFwdogCycle
#endif

//看门狗计时时间，最大支持4096
#ifndef cSdkFwdogMs
#define cFwdogMs 4000u
#else
#define cFwdogMs cSdkFwdogMs
#endif

//看门狗任务初始化等待时间
#ifndef cSdkFwdogInitWaitMs
#define cFwdogInitWaitMs 1000u
#else
#define cFwdogInitWaitMs cSdkFwdogInitWaitMs
#endif


extern bool sFwdog_Init(void);