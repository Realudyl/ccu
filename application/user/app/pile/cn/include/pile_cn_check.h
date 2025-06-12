/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_check.h
* Description                           :   国标用户自检程序
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-23
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_check_h_
#define _pile_cn_check_h_

#include "en_common.h"
#include "en_log.h"
#include "pile_cn.h"
#include "adc_app_sample.h"






//判断枪线温度小于此值，自检不通过，单位：1°
#define cCheckSelfMinTemp               (-30.0f)

//判断枪线温度大于此值，自检不通过，单位：1°
#define cCheckSelfMaxTemp               (85.0f)

























extern bool sPileCnCheckELockSt(ePileGunIndex_t eGunIndex);






#endif




