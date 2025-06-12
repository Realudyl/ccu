/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gbt15.h
* Description                           :   国标27930-2015用户程序
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-28
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_gbt15_h_
#define _pile_cn_gbt15_h_

#include "en_common.h"
#include "en_log.h"

#include "iso.h"
#include "io_app.h"
#include "bms_gbt_15_api.h"
#include "adc_app_sample.h"
#include "mod_app_opt.h"





#define cCC1FullConnect                 (4.0)                                   //cc1插枪后的采样电压
#define cCC1FullDiffVol                 (0.8)                                   //Gbt18487-2015 表B.1规定：cc1采样电压范围差值±0.8V 


#define cJudgeSelfCheckVolt             (100.0)                                 //插枪自检的安全电压
#define cJudgeSafeVolt                  (60.0)                                  //60V，泄放后安全电压判断阈值
#define cJudgeSafeCurr                  (5.0)                                   //5A，停止充电继电器动作的电流阈值

#define cJudgeCurrDropMinTemper         (95.0)                                  //开始降载温度
#define cJudgeCurrDropMaxTemper         (105.0)                                 //停止充电温度



#define cELockDelayTime                 200                                     //200ms，电子锁控制间隔



#define cTimeoutELock                   5000                                    //5s未锁止或解锁，认为电子锁故障
#define cTimeoutIsoCheck                30000                                   //30s未完成绝缘检测，认为绝缘检测故障
#define cTimeoutPreChg                  60000                                   //60s未完成预充，认为预充失败
#define cTimeoutPauseChg                600000                                  //10min暂停充电则认为故障
#define cTimeoutResumeChg               15000                                   //15s暂停充电到重新充电未完成，则认为故障










//用户授权类型
typedef enum
{
    eAuthorizeTypeNone                  = 0,                                    //用户未授权
    eAuthorizeTypeApp                   = 1,                                    //App授权充电
    eAuthorizeTypeCard                  = 2,                                    //刷卡授权充电
    eAuthorizeTypeVin                   = 3,                                    //vin码授权充电
}ePileCnAuthorizeType_t;










//电子锁控制
typedef enum
{
    eELockCtrlReset                     = 0,                                    //复位
    eELockCtrlOff                       = 1,                                    //电子锁解锁
    eELockCtrlOn                        = 2,                                    //电子锁锁止
}ePileCnELockCtrl_t;





//继电器控制
typedef enum
{
    eRelayCtrlOff                       = 0,                                    //继电器断开
    eRelayCtrlOn                        = 1,                                    //继电器吸合
}ePileCnRelayCtrl_t;





//枪当前可用状态
typedef enum
{
    eGunStateIdle                       = 0,                                    //空闲
    eGunStateWork                       = 1,                                    //工作中
    eGunStateErr                        = 2,                                    //故障中
    eGunStateBooking                    = 3,                                    //占用中
}ePileCnGunState_t;






//Vin码充电当前状态
typedef enum
{
    eVinChgCheckIdle                    = 0,                                    //空闲
    eVinChgCheckWait                    = 1,                                    //等待平台校验
    eVinChgCheckSuccess                 = 2,                                    //平台校验通过
    eVinChgCheckFailed                  = 3,                                    //平台校验不通过
}ePileCnVinChgCheckState_t;








extern void sPileCnGbt15MainLoop(ePileGunIndex_t eGunIndex);
extern bool sPileCnGbt15PreChgParamCheck(ePileGunIndex_t eGunIndex);

#endif

