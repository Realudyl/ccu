/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_ccs_api.c
* Description                           :   ccs对外提供的数据读写接口
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-01-10
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_api.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_api";



extern stBmsCcsCache_t *pBmsCcsCache[cMse102xDevNum];






bool sBmsCcsSetSlacEnable(i32 i32DevIndex, i32 i32SlacEnable);























/***************************************************************************************************
* Description                           :   SECC 可读可写数据的读写接口 之 i32SlacEnable 设置
* Author                                :   Hall
* Creat Date                            :   2023-05-16
* notice                                :   
****************************************************************************************************/
bool sBmsCcsSetSlacEnable(i32 i32DevIndex, i32 i32SlacEnable)
{
    if(pBmsCcsCache[i32DevIndex] == NULL)
    {
        return(false);
    }
    
    pBmsCcsCache[i32DevIndex]->i32SlacEnable = i32SlacEnable;
    
    return(true);
}






















