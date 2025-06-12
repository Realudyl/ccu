/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_check.c
* Description                           :   国标27930-2015用户上电自检程序
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-23
* notice                                :   
****************************************************************************************************/
#include "pile_cn_check.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_check";

















extern stPileCnCache_t *pPileCnCache;















//待检查项

bool sPileCnCheckELockSt(ePileGunIndex_t eGunIndex);



















/***************************************************************************************************
* Description                           :   自检逻辑 之 枪电子锁处理
* Author                                :   Hall
* Creat Date                            :   2024-05-24
* notice                                :   上电强制解锁即可
****************************************************************************************************/
bool sPileCnCheckELockSt(ePileGunIndex_t eGunIndex)
{
    sPileCnSetElockState(eGunIndex, eELockCtrlOff);
    
    return true;
}













