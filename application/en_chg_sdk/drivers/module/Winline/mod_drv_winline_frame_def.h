/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_winline_frame_def.h
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#ifndef _mod_drv_winline_frame_def_h_
#define _mod_drv_winline_frame_def_h_
#include "mod_drv_winline_data_def.h"















//永联模块modbus协议 帧头结构 定义
typedef struct
{
    eModDrvWinlineFun_t                 eFun;                                   //功能码
    eModDrvWinlineErr_t                 eErr;                                   //错误码---应答报文时有效
    eModDrvWinlineReg_t                 eReg;                                   //寄存器地址
    
}__attribute__((packed)) stModDrvWinlineHead_t;






//永联模块modbus协议 payload 定义
typedef union
{
    unIeee754_t                         unValue;                                //所有浮点数
    u32                                 u32Value;                               //部分整型数
    u32                                 u32Group;                                //组号
    eModDrvWinlineAddrMode_t            eAddrMode;                              //模块地址分配方式
    eModDrvWinlineTurn_t                eTurn;                                  //开关机
    eModDrvWinlineRstByVoltHigh_t       eRstByVoltHigh;                         //模块过压复位
    eModDrvWinlineUdcOutHighAssoc_t     eUdcOutHighAssoc;                       //模块输出过压保护关联是否允许
    stModDrvWinlineWarningCode_t        stWarningCode;                          //告警/状态
    eModDrvWinlineRstByShort_t          eRstByShort;                            //模块短路复位
    eModDrvWinlineInputMode_t           eInputMode;                             //模块输入模式
    
    u8                                  u8Buf[cModDrvWinlinePayloadLen];
}__attribute__((packed)) unModDrvWinlinePayload_t;






//永联模块modbus协议 整个帧结构定义
typedef struct
{
    stModDrvWinlineHead_t               stHead;
    unModDrvWinlinePayload_t            unPayload;
}__attribute__((packed)) stModDrvWinlinePkt_t;





















#endif













