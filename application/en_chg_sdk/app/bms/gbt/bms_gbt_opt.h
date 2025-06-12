/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_opt.h
* Description                           :   GBT应用层消息 协议管理
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   用于bms通讯解析指向 27930-15 或 27930-23，供底层调用
****************************************************************************************************/

#ifndef _bms_gbt_opt_h_
#define _bms_gbt_opt_h_

#include "en_common.h"
#include "bms_gbt_15.h"







//国标bms通讯协议
typedef enum
{
    eBmsGbtType15                       = 0x00,                                 //gbt27930-2015
    eBmsGbtType23                       = 0x01,                                 //gbt27930-2023
    
    eBmsGbtTypeMax,
}eBmsGbtNetType_t;


















//国标bms协议管理
typedef struct
{
    //协议 类型
    eBmsGbtNetType_t                    eType;

    //协议 描述字符串
    char                                *pDescString;

    //协议 初始化函数
    bool                                (*pInit)(ePileGunIndex_t eGunIndex, i32 i32Port, i32 i32Baudrate, void (*pCbFunc)(ePileGunIndex_t eGunIndex));

    //协议 缓存初始化函数
    bool                                (*pCacheInit)(ePileGunIndex_t eGunIndex);

    
}stBmsGbtOptMap_t;








#endif

