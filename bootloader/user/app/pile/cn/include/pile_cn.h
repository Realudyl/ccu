/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn.h
* Description                           :   国标充电桩实现
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-28
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_h_
#define _pile_cn_h_
#include "en_common.h"
#include "en_log.h"

#include "AutoGenVersion.h"

#include "card_app.h"
#if defined(DC480K_G2SF311_600)
#include "cooling_app.h"
#endif
#include "bms_gbt_15_state.h"

#include "pile_cn_check.h"
#include "pile_cn_gate.h"
#include "pile_cn_gate_card.h"
#include "pile_cn_gate_vin.h"
#include "pile_cn_gate_ota.h"
#include "pile_cn_gate_param.h"
#include "pile_cn_gate_rmt.h"
#include "pile_cn_gate_rcd_rpt.h"
#include "pile_cn_gbt15.h"
#include "pile_cn_state.h"
#include "led_app.h"
#include "eeprom_app.h"
#include "fault.h"
#include "fault_chk.h"
#include "pile_cn_fault_bms.h"
#include "pile_cn_fault_hw.h"






typedef enum
{
    eCheckRstNormal                     = 0,                                    //自检结果正常
    eCheckRstError                      = 1,                                    //自检结果错误
    
}ePileCnCheckRst_t;










typedef struct
{
    u32                                 u32LastTick;                            //上一次执行动作的时间
    ePileCnELockCtrl_t                  eCtrl;                                  //当前电子锁控制字
    ePileCnELockCtrl_t                  eOldCtrl;                               //上一次电子锁控制字，用于刷新执行动作时间
}stPileCnElock_t;










//用户充电基本信息，先集成在一块，方便和tcu交互
typedef struct
{
    bool                                bInit;                                  //用户数据初始化
    ePrivDrvGunWorkStatus_t             eState;                                 //用户层状态机
    ePileCnAuthorizeType_t              eAuthorize;                             //用户是否授权充电
    ePrivDrvGunLockStatus_t             eElockState;                            //当前电子锁状态
    stPileCnElock_t                     stElockCtrl;                            //电子锁控制
    ePileCnVinChgCheckState_t           eVinCheckState;                         //Vin码平台校验状态
    
    bool                                bIsPauseChg;                            //是否暂停了充电
    ePileCnCheckRst_t                   eCheckResult;                           //上电自检结果
    
    f32                                 f32CurrLimitByGun;                      //枪线最大允许电压
    u16                                 u16OutputVol;                           //当前输出电压，来源于电表采集，0.1V
    u16                                 u16OutputCur;                           //当前输出电流，来源于电表采集，0.1A
    u16                                 u16ChargeTime;                          //充电持续时间，min
    u16                                 u16ReaminTime;                          //充电剩余时间，min
    u8                                  u8CurSoc;                               //当前车端soc
    u16                                 u16SetVol;                              //设定输出电压，0.1V
    u16                                 u16SetCur;                              //设定输出电流，0.1A
    u32                                 u32ChgStartWh;                          //充电开始电量，wh
    u32                                 u32ChgStopWh;                           //充电停止电量，wh
    u32                                 u32ChgTotalWh;                          //本次充电累计电量，wh
    u32                                 u32StageTime;                           //执行时间，用于某些时序判断
    
}stPileCnChargeInfo_t;





typedef struct
{
    //用户程序资源
    stPileCnGateData_t                  stGateData;                             //网关用户程序相关数据
    
    //需要区分枪的信息
    stPileCnChargeInfo_t                stChgInfo[ePileGunIndexNum];            //充电基本信息
}stPileCnCache_t;






//初始化
extern bool sPileCnInit(bool bIsSuperCharger, f32 f32CurrLimitByGun);

//鉴权
extern bool sPileCnSet_Authorize(ePileGunIndex_t eGunIndex, ePileCnAuthorizeType_t eAuthorize);
extern ePileCnAuthorizeType_t sPileCnGet_Authorize(ePileGunIndex_t eGunIndex);

//Vin码校验状态
extern bool sPileCnSet_VinChgCheckState(ePileGunIndex_t eGunIndex, ePileCnVinChgCheckState_t eVinRst);
extern ePileCnVinChgCheckState_t sPileCnGet_VinChgCheckState(ePileGunIndex_t eGunIndex);

//电子锁和辅源控制
extern void sPileCnSetElockState(ePileGunIndex_t eGunIndex, ePileCnELockCtrl_t eElockCtrl);
extern bool sPileCnElockCtrl(ePileGunIndex_t eGunIndex);
extern bool sPileCnAuxPowerCtrl(ePileGunIndex_t eGunIndex, ePileCnRelayCtrl_t eRelayCtrl);
extern bool sPileCnGetElockState(ePileGunIndex_t eGunIndex, ePrivDrvGunLockStatus_t *eElockState);
extern bool sPileCnGetSelfTestResult(ePileGunIndex_t eGunIndex, ePileCnCheckRst_t *ePileCnCheckRst);
extern bool sPileCnSetDisCc1Link(ePileGunIndex_t eGunIndex, bool bFlag);


#endif


