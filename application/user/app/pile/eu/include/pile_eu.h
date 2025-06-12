/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_eu.h
* Description                           :   欧标充电桩实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
#ifndef _pile_eu_H_
#define _pile_eu_H_
#include "en_common.h"
#include "en_log.h"

#include "adc_app_sample.h"
#include "io_app.h"
#include "io_pwm_app.h"
#include "mod_app_avg.h"

#include "bms_ccs_mme.h"
#include "bms_ccs_v2g_opt.h"
#include "bms_ccs.h"
#include "bms_ccs_v2g_din_api.h"

#include "pile_eu_config.h"
#include "pile_eu_state.h"





















//pile 充电桩缓存数据结构
typedef struct
{
    ePileEuState_t                      eState[ePileGunIndexNum];
    
    //CCS枪序号基值---第一把CCS枪的枪序号,如果有多把CCS枪,应保持连续
    ePileGunIndex_t                     eGunIndexBaseCcs;
    
    //用于判断插枪信号稳定
    u8                                  u8CpCnt[ePileGunIndexNum];
    
}stPileEuCache_t;







extern bool sPileEuInit(void);







#endif















