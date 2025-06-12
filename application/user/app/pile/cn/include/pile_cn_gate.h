/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate.h
* Description                           :   国标充电桩实现 内网协议用户程序部分实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_gate_h_
#define _pile_cn_gate_h_
#include "adc_app_sample.h"
#include "private_drv_opt.h"
#include "order.h"
#include "pile_cn_gate_vin.h"














//国标充电桩用户程序 之网关协议驱动用户程序 相关数据结构体
typedef struct
{
    bool                                bInitFlag;                              //初始化标志--上电初始化一次就好了
    
    ePrivDrvLoginedFlag_t               eLoginedFlag;                           //登录服务器标志
    eAdcAppCc1State_t                   eCc1StateOld[ePileGunIndexNum];         //上一次的枪状态
    ePrivDrvGunWorkStsRsn_t             eGunStsReason[ePileGunIndexNum];        //枪工作状态变化原因
    ePrivDrvStopReason_t                eStopReason[ePileGunIndexNum];          //停充原因
    
    //05/A5--->刷卡数据
    stCardData_t                        stCard[ePileGunIndexNum];

    //05/A5--->Vin码数据
    stVinData_t                         stVin[ePileGunIndexNum];
    
    //06/A6--->远程命令
    stPrivDrvCmdA6_t                    *pRmt[ePileGunIndexNum];
    bool                                bccuPoStopWarn[ePileGunIndexNum];       //远程结束订单号不一致告警
    
    //计量计费资源
    unPrivDrvRate_t                     unRate;                                 //费率模型
    stEepromAppBlockChgRcd_t            stNewRcd[ePileGunIndexNum];             //新充电记录----充电过程中计量用,停充后立即上报
    
    //09/A9--->充电记录缓存
    stEepromAppBlockChgRcd_t            stOldRcd;                               //旧充电记录----未充电状态下循环查询区间内旧充电记录,执行上报逻辑
    u16                                 u16CheckIndex;                          //当前正在检查的旧充电记录序号
    u32                                 u32OldChkTime;                          //旧充电记录检查的时间戳
    u32                                 u32OldRptTime;                          //旧充电记录上报的时间戳---检查不满足上报条件 就不会更新此时间戳
    
    //0A/AA--->参数管理用缓存
    stPrivDrvCmdAA_t                    stCmdAa[ePileGunIndexNum];              //
    ePrivDrvParamOta_t                  eOtaStatus;                             //网关OTA状态
    
    //0B/AB--->桩OTA升级数据
    stPrivDrvCmdAB_t                    *pOta;
    
    
    //13/B3--->BMS数据缓存
    u16                                 u16Seq[ePileGunIndexNum];               //BMS数据序号
    stPrivDrvCmd13_t                    stBmsData[ePileGunIndexNum];            //用于网关上报的缓存
    stBmsGbt15Evcc_t                    stEvccData[ePileGunIndexNum];           //用于从车端取数据的缓存
    
}stPileCnGateData_t;
















extern void sPileCnGateLoop(ePileGunIndex_t eGunIndex);


extern bool sPileCnGateSetGunStsReason(ePileGunIndex_t eGunIndex);
extern bool sPileCnGateClrStopReason(ePileGunIndex_t eGunIndex);

extern bool sPileCnGateMakeStopReason(ePileGunIndex_t eGunIndex, ePrivDrvStopReason_t eReason);
extern bool sPileCnGateMakeGunStsReason(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStsRsn_t eReason);
extern ePrivDrvGunConnStatus_t sPileCnCc1StateToGunConnStatue(eAdcAppCc1State_t eState);



#endif




















