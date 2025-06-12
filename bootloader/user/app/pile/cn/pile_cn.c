/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn.c
* Description                           :   国标充电桩实现
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-28
* notice                                :   
****************************************************************************************************/
#include "iso.h"

#include "pile_cn.h"
#include "pile_cn_config.h"
#include "pile_cn_shell.h"
#include "pile_cn_check.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn";




stPileCnCache_t *pPileCnCache;

i32 evccCanPort[] = {cEvcc01CanPort, cEvcc02CanPort};
i32 evccCanBaudrate[] = {cEvcc01CanBaudrate, cEvcc02CanBaudrate};












bool sPileCnInit(bool bIsSuperCharger, f32 f32CurrLimitByGun);
void sPileCnTask(void *pParam);

bool sPileCnSet_Authorize(ePileGunIndex_t eGunIndex, ePileCnAuthorizeType_t eAuthorize);
ePileCnAuthorizeType_t sPileCnGet_Authorize(ePileGunIndex_t eGunIndex);

bool sPileCnSet_VinChgCheckState(ePileGunIndex_t eGunIndex, ePileCnVinChgCheckState_t eVinRst);
ePileCnVinChgCheckState_t sPileCnGet_VinChgCheckState(ePileGunIndex_t eGunIndex);

void sPileCnSetElockState(ePileGunIndex_t eGunIndex, ePileCnELockCtrl_t eElockCtrl);
bool sPileCnElockCtrl(ePileGunIndex_t eGunIndex);
bool sPileCnAuxPowerCtrl(ePileGunIndex_t eGunIndex, ePileCnRelayCtrl_t eRelayCtrl);
void sPileCnCloseGunOutput(ePileGunIndex_t eGunIndex);
bool sPileCnGetElockState(ePileGunIndex_t eGunIndex, ePrivDrvGunLockStatus_t         *eElockState);
bool sPileCnGetSelfTestResult(ePileGunIndex_t eGunIndex, ePileCnCheckRst_t *ePileCnCheckRst);
bool sPileCnSetDisCc1Link(ePileGunIndex_t eGunIndex, bool bFlag);









stShellCmd_t stSellCmdBmsChgCmd = 
{
    .pCmd       = "shutdown",
    .pFormat    = "格式: shutdown 0",
    .pFunction  = "功能: 桩端停充",
    .pRemarks   = "备注: 第0号枪 : 0  第1号枪 : 1",
    .pFunc      = sPileCnShellGunStopChg,
};




stShellCmd_t stDisCc1LinkCmd = 
{
    .pCmd       = "dislink",
    .pFormat    = "格式: dislink 0",
    .pFunction  = "功能: 模拟Cc1信号断开",
    .pRemarks   = "备注: 第0号枪 : 0  第1号枪 : 1",
    .pFunc      = sPileCnShellDisCc1Link,
};




stShellCmd_t stResumeCc1LinkCmd = 
{
    .pCmd       = "relink",
    .pFormat    = "格式: relink 0",
    .pFunction  = "功能: 恢复cc1信号",
    .pRemarks   = "备注: 第0号枪 : 0  第1号枪 : 1",
    .pFunc      = sPileCnShellResumeCc1Link,
};












/***************************************************************************************************
* Description                           :   国标bms初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnInit(bool bIsSuperCharger, f32 f32CurrLimitByGun)
{
    ePileGunIndex_t eGunIndex;
    ePileGunIndex_t eRealGunNum;
    ePileGunIndex_t *pGunIndex = MALLOC(sizeof(ePileGunIndex_t));
    
    //shell命令初始化
    sShellCmdRegister(&stSellCmdBmsChgCmd);
    sShellCmdRegister(&stDisCc1LinkCmd);
    sShellCmdRegister(&stResumeCc1LinkCmd);
    
    //故障检测组件初始化
    sPileCnFaultChkRegister_Bms();
    sPileCnFaultChkRegister_Hw(bIsSuperCharger);
    
    if(pPileCnCache == NULL)
    {
        pPileCnCache = (stPileCnCache_t *)MALLOC(sizeof(stPileCnCache_t));
        memset(pPileCnCache, 0, sizeof(stPileCnCache_t));
    }
    
    
    eRealGunNum = (bIsSuperCharger == true) ? 1 : ePileGunIndexNum;
    for(eGunIndex = 0; eGunIndex < eRealGunNum; eGunIndex++)
    {
        sPileCnGateNewRcdInvalid(eGunIndex);
        pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun = f32CurrLimitByGun;
        
        //bms初始化
        sBmsGbt15Init(eGunIndex, evccCanPort[eGunIndex], evccCanBaudrate[eGunIndex], sPileCnCloseGunOutput);
        
        //创建任务
        (*pGunIndex) = eGunIndex;
        xTaskCreate(sPileCnTask,        "sPileCnTask",        (512),   pGunIndex, 26, NULL);
        xTaskCreate(sPileCnStateTask,   "sPileCnStateTask",   (512),   pGunIndex, 27, NULL);
    }
    
    
    return true;
}









/***************************************************************************************************
* Description                           :   国标bms用户程序任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnTask(void *pParam)
{
    //获取枪Id
    ePileGunIndex_t eGunIndex = (*((ePileGunIndex_t *)pParam));
    
    EN_SLOGI(TAG, "任务建立:国标充电桩 主任务, 对应枪序号:%d", eGunIndex);
    
    
    //自检过程中以500ms闪烁红色指示灯
    sLedAppSet(eGunIndex, eLedColorRed, eLedSignalTypeBlink, cLedUpdateTimeBlink);
    
    while(1)
    {
        sPileCnGbt15MainLoop(eGunIndex);
        
        //网关控制
        sPileCnGateLoop(eGunIndex);
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}







/***************************************************************************************************
* Description                           :   国标bms鉴权设置
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-21
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnSet_Authorize(ePileGunIndex_t eGunIndex, ePileCnAuthorizeType_t eAuthorize)
{
    if(pPileCnCache != NULL)
    {
        pPileCnCache->stChgInfo[eGunIndex].eAuthorize = eAuthorize;
        return true;
    }
    
    return false;
}











/***************************************************************************************************
* Description                           :   获取国标bms鉴权状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-21
* notice                                :   
*                                       
****************************************************************************************************/
ePileCnAuthorizeType_t sPileCnGet_Authorize(ePileGunIndex_t eGunIndex)
{
    if(pPileCnCache != NULL)
    {
        return pPileCnCache->stChgInfo[eGunIndex].eAuthorize;
    }
    
    return eAuthorizeTypeNone;
}










/***************************************************************************************************
* Description                           :   设置Vin码充电平台校验结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-21
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnSet_VinChgCheckState(ePileGunIndex_t eGunIndex, ePileCnVinChgCheckState_t eVinRst)
{
    if(pPileCnCache != NULL)
    {
        pPileCnCache->stChgInfo[eGunIndex].eVinCheckState = eVinRst;
        return true;
    }
    
    return false;
}











/***************************************************************************************************
* Description                           :   获取当前Vin码充电结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-21
* notice                                :   
*                                       
****************************************************************************************************/
ePileCnVinChgCheckState_t sPileCnGet_VinChgCheckState(ePileGunIndex_t eGunIndex)
{
    if(pPileCnCache != NULL)
    {
        return pPileCnCache->stChgInfo[eGunIndex].eVinCheckState;
    }
    
    return eVinChgCheckIdle;
}












/***************************************************************************************************
* Description                           :   设置电子锁控制字
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-21
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnSetElockState(ePileGunIndex_t eGunIndex, ePileCnELockCtrl_t eElockCtrl)
{
    if(pPileCnCache != NULL)
    {
        pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eCtrl = eElockCtrl;
    }
}










/***************************************************************************************************
* Description                           :   电子锁控制
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-30
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnElockCtrl(ePileGunIndex_t eGunIndex)
{
    eEepromAppELockType_t eType;
    
    //此处类型要改成从eeprom获取配置
    eType = eELockType_Pulse;
    
    if(pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eCtrl != pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eOldCtrl)
    {
        pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eOldCtrl = pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eCtrl;
        pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.u32LastTick = xTaskGetTickCount();
    }
    
    switch(eType)
    {
    case eELockType_Pulse:
        
        if(pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eCtrl == eELockCtrlOn)
        {
            //延时200ms
            if((xTaskGetTickCount() - pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.u32LastTick) >= cELockDelayTime)
            {
                sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), RESET);
                sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), RESET);
                
            }
            else
            {
                //正脉冲锁枪：LOCK_ON_OFF置1，LOCK_RELAY_EN再置1，延时100~300ms后，LOCK_RELAY_EN置0
                sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), SET);
                sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), SET);
            }
            pPileCnCache->stChgInfo[eGunIndex].eElockState = ePrivDrvGunLockStatusLock;
        }
        else
        {
            //延时200ms
            if((xTaskGetTickCount() - pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.u32LastTick) >= cELockDelayTime)
            {
                sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), RESET);
            }
            else
            {
                //负脉冲解锁：LOCK_ON_OFF置0，LOCK_RELAY_EN再置1，延时100~300ms后，LOCK_RELAY_EN置0
                sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), RESET);
                sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), SET);
            }
            pPileCnCache->stChgInfo[eGunIndex].eElockState = ePrivDrvGunLockStatusUnlock;
        }
        break;
    case eELockType_Level:
        
        if(pPileCnCache->stChgInfo[eGunIndex].stElockCtrl.eCtrl == eELockCtrlOn)
        {
            //通电锁枪：LOCK_ON_OFF和LOCK_RELAY_EN都置1
            sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), SET);
            sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), SET);
            pPileCnCache->stChgInfo[eGunIndex].eElockState = ePrivDrvGunLockStatusLock;
        }
        else
        {
            //断电解锁：依据原理图只需要LOCK_RELAY_EN置0，保险起见都置0
            sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), RESET);
            sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), RESET);
            pPileCnCache->stChgInfo[eGunIndex].eElockState = ePrivDrvGunLockStatusUnlock;
        }
        
        break;
    default:
        sIoDrvSetOutput(sIoGetLockOnOff(eGunIndex), RESET);
        sIoDrvSetOutput(sIoGetLockRelayEn(eGunIndex), RESET);
        
        break;
    }
    
    return true;
}









/***************************************************************************************************
* Description                           :   辅助电源控制
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-22
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnAuxPowerCtrl(ePileGunIndex_t eGunIndex, ePileCnRelayCtrl_t eRelayCtrl)
{
    //1.获取辅助电源检测端口电平
    //2.高电平则默认不需要吸合辅助电源，低电平需要桩提供辅助电源供电
    
    if(eRelayCtrl == eRelayCtrlOn)
    {
        if(sIoDrvGetInput(sIoGetGbBms(eGunIndex)) == SET)
        {
            sIoDrvSetOutput(sIoGetRelayD1(eGunIndex), SET);
            sIoDrvSetOutput(sIoGetRelayD2(eGunIndex), SET);
        }
    }
    else
    {
        sIoDrvSetOutput(sIoGetRelayD1(eGunIndex), RESET);
        sIoDrvSetOutput(sIoGetRelayD2(eGunIndex), RESET);
    }
    
    return true;
}












/***************************************************************************************************
* Description                           :   给驱动层紧急停机调用的函数接口
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnCloseGunOutput(ePileGunIndex_t eGunIndex)
{
    //通知模块停止输出
    sModAppOptClose(eGunIndex);
    
    //立即断开直流接触器，避免分体桩超时
    sIoSetDrK(eGunIndex, false);
}










/***************************************************************************************************
* Description                           :   获取电子锁控制字
* Author                                :   Dai
* Creat Date                            :   2024-07-17
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnGetElockState(ePileGunIndex_t eGunIndex, ePrivDrvGunLockStatus_t         *eElockState)
{
    if(pPileCnCache != NULL)
    {
        (*eElockState) = pPileCnCache->stChgInfo[eGunIndex].eElockState;
        
        return true;
    }
    return false;
}





/***************************************************************************************************
* Description                           :   获取自检结果
* Author                                :   Dai
* Creat Date                            :   2024-07-17
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnGetSelfTestResult(ePileGunIndex_t eGunIndex, ePileCnCheckRst_t *ePileCnCheckRst)
{
    if(pPileCnCache != NULL)
    {
        (*ePileCnCheckRst) = pPileCnCache->stChgInfo[eGunIndex].eCheckResult;
        
        return true;
    }
    return false;
}







/***************************************************************************************************
* Description                           :   模拟断开Cc1信号
* Author                                :   haisheng.dang
* Creat Date                            :   2024-07-22
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnSetDisCc1Link(ePileGunIndex_t eGunIndex, bool bFlag)
{
    bool bRst = false;
    
    if(bFlag == true)
    {
        if(eGunIndex != 0)
        {
            bRst = sIoDrvSetOutput(eIo_SITE1, RESET);
        }
        else
        {
            bRst = sIoDrvSetOutput(eIo_SITE4, RESET);
        }
    }
    else
    {
        if(eGunIndex != 0)
        {
            bRst = sIoDrvSetOutput(eIo_SITE1, SET);
        }
        else
        {
            bRst = sIoDrvSetOutput(eIo_SITE4, SET);
        }
    }
    
    return bRst;
}







