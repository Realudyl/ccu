/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_opt.c
* Description                           :   GBT应用层消息 协议管理
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   用于bms通讯解析指向 27930-15 或 27930-23，供底层调用
****************************************************************************************************/
#include "bms_gbt_opt.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_opt";








//国标bms 应用协议管理map
stBmsGbtOptMap_t stBmsGbtOptMap[eBmsGbtTypeMax] = 
{
    {
        .eType                          = eBmsGbtType15,
        .pDescString                    = "GBT27930-2015",
        .pInit                          = sBmsGbt15Init,
        .pCacheInit                     = sBmsGbt15CacheInit,
    },
    {
        .eType                          = eBmsGbtType23,
        .pDescString                    = "GBT27930-2023",
        .pInit                          = NULL,
        .pCacheInit                     = NULL,
    },
};







