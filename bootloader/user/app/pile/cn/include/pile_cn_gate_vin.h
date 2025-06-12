/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_vin.h
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 Vin码充电功能
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_gate_vin_h_
#define _pile_cn_gate_vin_h_

#include "private_drv_opt.h"
#include "card_app_en.h"
















typedef struct
{
    //Vin码充电触发
    bool                                bVinChargeStart;
    
    //Vin码数据
    u8                                  u8Vin[cPrioDrvLenVin];
    
    //附加数据
    u32                                 u32Balance;                             //余额--------------单位0.01元
    ePrivDrvCardType_t                  eType;                                  //卡类型
    ePileGunIndex_t                     eGunIndex;                              //本次刷卡对应的枪号
    ePrivDrvStartTransType_t            eStartType;                             //启动类型
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    stTime_t                            stTime;                                 //刷卡时间
}stVinData_t;





















extern void sPileCnGateVinAuth(ePileGunIndex_t eGunIndex);
extern void sPileCnGateVinAuthAck(ePileGunIndex_t eGunIndex);

extern void sVinAppStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param);










#endif








