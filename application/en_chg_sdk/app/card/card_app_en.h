/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_app_en.h
* Description                           :   EN+卡数据格式定义
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :   
****************************************************************************************************/
#ifndef _card_app_en_h_
#define _card_app_en_h_
#include "card_drv_mt626.h"







//
#define cCardAppEnBlockLen              (16)                                    //block长度









//扇区0，块0：序列号块结构
typedef struct
{
    u8                                  u8CardSn[4];                            //卡序列号----------仅用于校验计算
    u8                                  u8Data[12];                             //
}__attribute__((packed)) stCardAppEnS0B0_t;




//扇区2，块0：序列号块结构
typedef struct
{
    u32                                 u32Balance;                             //余额--------------单位0.01元
    bool                                bOnlineFlag;                            //联网卡标志--------true:联网卡，false:离网卡
    bool                                bUnlockFlag;                            //未锁卡标志--------true:未锁卡，false:已锁卡
    u8                                  u8Data[10];                             //
}__attribute__((packed)) stCardAppEnS2B0_t;



//扇区6，块0：序列号块结构
typedef struct
{
    u8                                  u8Code[4];                              //校验码
    u8                                  u8Rand[4];                              //随机数
    u8                                  u8CardNo[8];                            //卡号--------------自定义卡号
}__attribute__((packed)) stCardAppEnS6B0_t;



//扇区7，块0：序列号块结构
typedef struct
{
    u8                                  u8Sn[cCardAppEnBlockLen];               //充电桩SN码
}__attribute__((packed)) stCardAppEnS7B0_t;





typedef union
{
    stCardAppEnS0B0_t                   stS0B0;
    stCardAppEnS2B0_t                   stS2B0;
    stCardAppEnS6B0_t                   stS6B0;
    stCardAppEnS7B0_t                   stS7B0;
    u8                                  u8Buf[cCardAppEnBlockLen];
}unCardAppEnData_t;






























extern bool sCardAppEnRead(void *pData);






#endif




















