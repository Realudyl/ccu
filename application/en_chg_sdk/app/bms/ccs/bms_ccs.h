/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs.h
* Description                           :   ccs 通讯主程序
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-28
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_h_
#define _bms_ccs_h_
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "bms_ccs_mme.h"
#include "bms_ccs_net.h"
#include "bms_ccs_v2g_opt.h"

















//ccs 状态定义
typedef enum
{
    eBmsCcsStateWait                    = 0,                                    //等待车辆连接
    eBmsCcsStateMmeNet                  = 1,                                    //MME网络建立阶段:SLAC & SDP
    eBmsCcsStateV2g                     = 2,                                    //其他 V2G 消息传输阶段
    eBmsCcsStateShutdownNormal          = 3,                                    //预期中止 9.7.4.2.2 SECC requirements for premature intentional termination
    eBmsCcsStateShutdownEmg             = 4,                                    //紧急停机 触发"EVSE-initiated emergency shutdown---充电设施启动的紧急停机"动作
    eBmsCcsStateAbnormal                = 5,                                    //其他异常 例如TCP连接意外断开
    
    eBmsCcsStateMax
}eBmsCcsState_t;


























//SECC 缓存数据结构定义
typedef struct
{
    eBmsCcsState_t                      eState;                                 //SECC 主状态
    
    bool                                bSlacMatchFlagOld;                      //SLAC 连接状态
    
    i32                                 i32SlacEnable;                          //SLAC匹配使能开关
    
    //外部传入的参数
    ePileGunIndex_t                     eGunIndex;                              //对应的枪序号, 操作CP资源需要
    void                                (*pCbCloseGunOutput)(ePileGunIndex_t eGunIndex);//回调函数:关闭枪输出函数
    void                                (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn);//回调函数:CP PWM控制函数
    
    
}stBmsCcsCache_t;

























extern eBmsCcsState_t sBmsCcsStateGet(i32 i32DevIndex);

extern bool  sBmsCcsInit(stMmeDev_t *pDev, ePileGunIndex_t eGunIndex, void (*pCbCloseGunOutput)(ePileGunIndex_t eGunIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));

//状态切换动作函数
extern void  sBmsCcsStateSwitchOpV2gToShutdownEmg(i32 i32DevIndex);




#endif
















