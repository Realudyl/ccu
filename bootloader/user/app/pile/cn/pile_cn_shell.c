/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_shell.c
* Description                           :   国标充电桩shell命令
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-10
* notice                                :   
****************************************************************************************************/

#include "pile_cn_shell.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_shell";















bool sPileCnShellGunStopChg(const stShellPkt_t *pkg);
bool sPileCnShellDisCc1Link(const stShellPkt_t *pkg);
bool sPileCnShellResumeCc1Link(const stShellPkt_t *pkg);










/***************************************************************************************************
* Description                           :   shell停止枪充电
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-23
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnShellGunStopChg(const stShellPkt_t *pkg)
{
    ePileGunIndex_t eGunIndex = (ePileGunIndex_t)atoi(pkg->para[0]);
    
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    //停止授权充电
    sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
    
    return true;
}






/***************************************************************************************************
* Description                           :   shell命令模拟拔枪
* Author                                :   haisheng.dang
* Creat Date                            :   2024-07-22
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnShellDisCc1Link(const stShellPkt_t *pkg)
{
    ePileGunIndex_t eGunIndex = (ePileGunIndex_t)atoi(pkg->para[0]);
    
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    return sPileCnSetDisCc1Link(eGunIndex, true);
}







/***************************************************************************************************
* Description                           :   shell命令恢复Cc1信号状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-07-22
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnShellResumeCc1Link(const stShellPkt_t *pkg)
{
    ePileGunIndex_t eGunIndex = (ePileGunIndex_t)atoi(pkg->para[0]);
    
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    return sPileCnSetDisCc1Link(eGunIndex, false);
}







