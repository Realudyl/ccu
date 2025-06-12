/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_timing.h
* Description                           :   GBT27930-15 协议实现 之时序部分
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-19
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_timing_h_
#define _bms_gbt_timing_h_

#include "en_common.h"
#include "bms_gbt_15.h"


//GBT27930 时序相关定义


//车端报文超时定义
#define cTimeout_Gbt15_Bhm              (10000   + 200  )                       //5s，bhm不管超不超时流程都往下走
#define cTimeout_Gbt15_Brm              (5000   + 200  )                        //5s
#define cTimeout_Gbt15_Bcp              (5000   + 200  )                        //5s
#define cTimeout_Gbt15_Bro              (5000   + 200  )                        //5s
#define cTimeout_Gbt15_BroAA            (60000  + 100 )                         //60s
#define cTimeout_Gbt15_Bcs              (5000   + 200  )                        //5s
#define cTimeout_Gbt15_Bcl              (1000   + 50   )                        //1s
#define cTimeout_Gbt15_Bst              (5000   + 200  )                        //5s
#define cTimeout_Gbt15_Bsd              (10000  + 1000 )                        //10s

//物理检测时间
#define cTimeout_Gbt15_IsoCheck         (15000  + 1000 )                        //15s 绝缘检测超时
#define cTimeout_Gbt15_PreChg           (60000  + 1000 )                        //60s 预充超时
#define cTimeout_Gbt15_PauseChg         (600000 + 10000)                        //10min 暂停充电超时





//事件组事件定义
#define cGbtEvt_All                     (0x00ffffff)                            //所有事件
#define cGbtEvt_None                    (0)                                     //没有事件
#define cGbtEvt_Pkt_Timer1               (1 << 0)                                //报文接收定时器1
#define cGbtEvt_Pkt_Timer2               (1 << 1)                                //报文接收定时器2
#define cGbtEvt_Pkt_Timer1_s             (1 << 2)                                //报文接收定时器1 停止计时---发生一些事件时 会提前结束计时
#define cGbtEvt_Pkt_Timer2_s             (1 << 3)                                //报文接收定时器2 停止计时---发生一些事件时 会提前结束计时




//任务通知定义
#define cNotify_ResetTimer              (1 << 0)                                //复位时序逻辑
#define cNotify_TimeoutCheck            (1 << 1)                                //超时检查








//定时器初始化
extern void sBmsGbtInitTiming(ePileGunIndex_t eGunIndex);



//定时器清零
extern void sBmsGbtTimer1Rst(ePileGunIndex_t eGunIndex);
extern void sBmsGbtTimer2Rst(ePileGunIndex_t eGunIndex);


//定时器停止
extern void sBmsGbtTimer1Stop(ePileGunIndex_t eGunIndex);
extern void sBmsGbtTimer2Stop(ePileGunIndex_t eGunIndex);



//定时器启动
extern void sBmsGbtTimer1Start(ePileGunIndex_t eGunIndex);
extern void sBmsGbtTimer2Start(ePileGunIndex_t eGunIndex);





#endif

