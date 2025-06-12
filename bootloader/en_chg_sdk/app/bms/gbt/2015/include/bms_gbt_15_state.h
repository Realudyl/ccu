/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_state.h
* Description                           :   国标2015通讯协议状态机管理
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-19
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_state_h_
#define _bms_gbt_15_state_h_

#include "en_log.h"
#include "en_common.h"

#include "bms_gbt_15.h"
#include "bms_gbt_timing.h"
#include "bms_gbt_15_api.h"



//通信失败后最大重新握手次数
#define cReShakehandMaxCount            3

//保持旧报文发送时间
#define cKeepSendTime                   (5000+200)



//周期tick定义
#define c10msTime                       10
#define c50msTime                       50
#define c250msTime                      250
#define c500msTime                      500





























extern eBmsGbt15ProtState_t sBmsGbt15StateGet(ePileGunIndex_t eGunIndex);



extern void   sBmsGbt15StateTask(void *pParam);


















#endif

















