/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_basic.h
* Description                           :   实现协议的基础功能，例如充电桩参数与BMS参数是否匹配等
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-17
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_basic_h_
#define _bms_gbt_15_basic_h_

#include "en_common.h"
#include "bms_gbt_15_data_def.h"




extern u8  sBmsGbt15GetPgnCode(u32 u32CanId);
extern u32 sBmsGbt15GetMsgCanId(u8 u8Pgn);

extern eBmsGbt15EvParamCheck_t sBmsGbt15EvParamMatchCheck(ePileGunIndex_t eGunIndex);

extern bool sBmsGbt15CheckBsmFrameIsNormal(ePileGunIndex_t eGunIndex);
extern eBmsGbt15StopReasonType_t sBmsGbt15CheckBstFrameIsNormal(ePileGunIndex_t eGunIndex);



#endif

