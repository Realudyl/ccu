/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_com_drv_data_s.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     完整的内网协议包含两个方向，网关和桩端，本驱动需要实现
*                                             完整的内网协议驱动，
*
*                                             定义网关为master，缩写m，桩端为slave，缩写s
*
*                                             本文件实现slave侧的抽象数据接口api
****************************************************************************************************/
#include "private_drv_opt.h"




#if (cSdkPrivDevType == cSdkPrivDevTypeS)




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_com_drv_data_s";






extern stPrivDrvCache_t *pPrivDrvCache;









/**********************************************************************************************
* Description       :     Slave 端缓存数据的初始化
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvVarInitS(void)
{
    i32  i;
    
    EN_SLOGI(TAG, "Slave 端缓存数据的初始化");
    pPrivDrvCache->unData.stSlave.stChg.u16Addr    = cPrivDrvAddrDefault;
    pPrivDrvCache->unData.stSlave.stChg.eLoginAck  = ePrivDrvCmdRstFail;
    pPrivDrvCache->unData.stSlave.stChg.u16EleLoss = 1000;
    pPrivDrvCache->unData.stSlave.stChg.stCacheA2.eRst = ePrivDrvCmdRstFail;
    
    for(i = 0; i < cPrivDrvGunNumMax; i++)
    {
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheA5Mutex    = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheA6Mutex    = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheA7Mutex    = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheA8Mutex    = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheA9Mutex    = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stSlave.stGun[i].hCacheB4Mutex    = xSemaphoreCreateBinary();
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheA5Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheA6Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheA7Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheA8Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheA9Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[i].hCacheB4Mutex, 0);
    }
    
    pPrivDrvCache->unData.stSlave.stChg.hCacheAARptMutex        = xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stSlave.stChg.hCacheABMutex           = xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stSlave.stChg.hCacheACMutex           = xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stSlave.stChg.hTimeMutex              = xSemaphoreCreateBinary();
    xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheAARptMutex,    0);
    xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheABMutex,       0);
    xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheACMutex,       0);
    xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hTimeMutex,          0);
    
    return(true);
}








/**********************************************************************************************
* Description       :     可读可写数据 set:充电桩地址
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetChargerAddr(u16 u16Addr)
{
    pPrivDrvCache->unData.stSlave.stChg.u16Addr = u16Addr;
    return(true);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:充电桩类型
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetChargerType(ePrivDrvChargerType_t eType)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eType = eType;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:结算时机
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetPayAt(ePrivDrvPayAt_t ePayAt)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.ePayAt = ePayAt;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:计费端
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetMoneyCalcBy(ePrivDrvMoneyCalcBy_t eMoneyCalcBy)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eMoneyCalcBy = eMoneyCalcBy;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:桩禁用状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetDisableFlag(ePrivDrvChargerDisableFlag_t eFlag)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eDisableFlag = eFlag;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:功率分配启动模式
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetStartPowerMode(ePrivDrvStartPowerMode_t eMode)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.ePowerMode = eMode;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:桩工作模式
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetWorkMode(ePrivDrvChargerWorkMode_t eMode)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eWorkMode = eMode;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:三相平衡挂载相位
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetBalancePhase(ePrivDrvBalancePhase_t eBalancePhase)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.ePhase = eBalancePhase;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:最大功率
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetPowerMax(i32 i32Power)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.i32PowerMax = i32Power;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:最大电流
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCurrMax(i32 i32Curr)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.i32CurrMax = i32Curr;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:额定功率
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetPowerRated(i32 i32Power)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.i32PowerRated = i32Power;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:额定电流
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCurrRated(i32 i32Curr)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.i32CurrRated = i32Curr;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:是否支持蓝牙预约
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetBookingViaBle(ePrivDrvBookingViaBle_t eBookingViaBle)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eBookingViaBle = eBookingViaBle;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:登录服务器标志
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetLoginedFlag(ePrivDrvLoginedFlag_t eFlag)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eLoginedFlag = eFlag;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:重启原因
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRebootReason(ePrivDrvRebootReason_t eReason)
{
    pPrivDrvCache->unData.stSlave.stChg.stCache02.eReason = eReason;
    return(true);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:桩SN号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetSn(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8Sn);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8Sn, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:设备型号代码 
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetDevCode(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8DevCode);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8DevCode, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:设备名称
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetDevName(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8DevName);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8DevName, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:硬件版本号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetHwVer(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8HwVer);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8HwVer, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:软件版本号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetSwVer(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8SwVer);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8SwVer, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:枪口数量
* Author            :     Hall
* modified Date     :     2023-10-26
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetGunNum(u8 u8GunNum)
{
    pPrivDrvCache->unData.stSlave.stChg.u8GunNum = u8GunNum;
    return(true);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:枪工作状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
                          注意cPrivDrvWorkStsConvertEn是否使能
***********************************************************************************************/
bool sPrivDrvSetGunWorkStatus(u8 u8GunId, ePrivDrvGunWorkStatus_t eStatus)
{
    if(u8GunId == cPrivDrvGunIdChg)
    {
        if(pPrivDrvCache->unData.stSlave.stChg.stCache03.eGunWorkStatus != eStatus)
        {
            pPrivDrvCache->unData.stSlave.stChg.stCache03.eGunWorkStatusOld = pPrivDrvCache->unData.stSlave.stChg.stCache03.eGunWorkStatus;
            pPrivDrvCache->unData.stSlave.stChg.stCache03.eGunWorkStatus = eStatus;
            pPrivDrvCache->u32Time03[cPrivDrvGunIdChg] = 0;
        }
        
        return(true);
    }
    else if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus != eStatus)
        {
            pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatusOld = pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus;
            pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus = eStatus;
            pPrivDrvCache->u32Time03[u8GunId] = 0;
        }
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:枪连接状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvSetGunConnStatus(u8 u8GunId, ePrivDrvGunConnStatus_t eStatus)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eConnStatus != eStatus)
        {
            pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eConnStatus = eStatus;
            pPrivDrvCache->u32Time03[u8GunId] = 0;
        }
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:枪  锁状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvSetGunLockStatus(u8 u8GunId, ePrivDrvGunLockStatus_t eStatus)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eLockStatus = eStatus;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:（签到信息）预留字段
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRsvdDataLogin(i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8Data);
    
    if((pBuf != NULL) && (i32Len <= i32DataLen))
    {
        memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache02.u8Data, pBuf, i32Len);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:枪事件
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvSetGunEvent(u8 u8GunId, ePrivDrvGunEvent_t eEvent)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eEvent != eEvent)
        {
            pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eEvent = eEvent;
            pPrivDrvCache->u32Time03[u8GunId] = 0;
        }
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:状态变化原因
* Author            :     Hall
* modified Date     :     2025-05-30
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvSetGunStsReason(u8 u8GunId, ePrivDrvGunWorkStsRsn_t eReason)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eStsReason = eReason;
        
        return(true);
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可读可写数据 set:故障码---旧的故障位字段设置
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetFaultByte(unPrivDrvFaultByte_t *pFaultByte)
{
    if(pFaultByte != NULL)
    {
        if(memcmp(pPrivDrvCache->unData.stSlave.stChg.stCache03.unFaultByte.u8Buf, pFaultByte, sizeof(unPrivDrvFaultByte_t)) != 0)
        {
            memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache03.unFaultByte.u8Buf, pFaultByte, sizeof(unPrivDrvFaultByte_t));
            
            //故障码变化 触发桩上报03报文
            pPrivDrvCache->u32Time03[cPrivDrvGunIdChg] = 0;
        }
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:告警码---旧的告警位字段设置
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetWarningByte(unPrivDrvWarningByte_t *pWarningByte)
{
    if(pWarningByte != NULL)
    {
        if(memcmp(pPrivDrvCache->unData.stSlave.stChg.stCache03.unWarningByte.u8Buf, pWarningByte, sizeof(unPrivDrvWarningByte_t)) != 0)
        {
            memcpy(pPrivDrvCache->unData.stSlave.stChg.stCache03.unWarningByte.u8Buf, pWarningByte, sizeof(unPrivDrvWarningByte_t));
            
            //告警码变化 触发桩上报03报文
            pPrivDrvCache->u32Time03[cPrivDrvGunIdChg] = 0;
        }
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:故障码---新的故障告警码字段设置
* Author            :     Hall
* modified Date     :     2024-06-17
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetFaultCode(u8 u8GunId, stPrivDrvFaultCode_t *pFaultCode)
{
    if(pFaultCode != NULL)
    {
        if(u8GunId == cPrivDrvGunIdChg)
        {
            if(memcmp(&pPrivDrvCache->unData.stSlave.stChg.stCache03.stFaultCode, pFaultCode, sizeof(stPrivDrvFaultCode_t)) != 0)
            {
                memcpy(&pPrivDrvCache->unData.stSlave.stChg.stCache03.stFaultCode, pFaultCode, sizeof(stPrivDrvFaultCode_t));
                
                //故障码变化 触发枪1上报03报文
                pPrivDrvCache->u32Time03[u8GunId] = 0;
            }
        }
        else if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
        {
            if(memcmp(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stFaultCode, pFaultCode, sizeof(stPrivDrvFaultCode_t)) != 0)
            {
                memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stFaultCode, pFaultCode, sizeof(stPrivDrvFaultCode_t));
                
                //故障码变化 触发枪1上报03报文
                pPrivDrvCache->u32Time03[u8GunId] = 0;
            }
        }
        
        return(true);
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可读可写数据 set:卡认证报文发送
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCardAuth(u8 u8GunId, stPrivDrvCmd05_t *pCardAuth)
{
    return sPrivDrvPktSend05(u8GunId, pCardAuth);
}














/**********************************************************************************************
* Description       :     可读可写数据 set:远程操作应答报文发送
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRmtAck(u8 u8GunId, stPrivDrvCmd06_t *pRmtAck)
{
    return sPrivDrvPktSend06(u8GunId, pRmtAck);
}








/**********************************************************************************************
* Description       :     可读可写数据 set:开始/结束充电报文 发送
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetTrans(u8 u8GunId, stPrivDrvCmd07_t *pTrans)
{
    return sPrivDrvPktSend07(u8GunId, pTrans);
}
















/**********************************************************************************************
* Description       :     可读可写数据 set:报文上报时机
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRtRptType(u8 u8GunId, ePrivDrvRtDataRptType_t eRptType)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eRptType = eRptType;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:充电功率
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetChargingPower(u8 u8GunId, u32 u32Power)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].u32ChargingPower = u32Power;
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.u32Power = u32Power;
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:充电实时数据上报
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRtRpt(u8 u8GunId, stPrivDrvCmd09_t *pRcdData)
{
    return sPrivDrvPktSend08(u8GunId, pRcdData);
}




/**********************************************************************************************
* Description       :     可读可写数据 set:结算报文上报
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRcdRpt(stPrivDrvCmd09_t *pRcdData)
{
    return sPrivDrvPktSend09(pRcdData);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:CMD0A 报文上报
* Author            :     Hall
* modified Date     :     2024-06-12
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCmdParam(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam, u8 u8RepeatCnt)
{
    return sPrivDrvPktSend0A(bRptFlag, u8GunId, eType, eRst, eAddr, u16ParamLen, pParam);
}















/**********************************************************************************************
* Description       :     可读可写数据 set:三相输入电压
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUacIn(u8 u8GunId, stPrivDrvPhaseData_t *pUacIn)
{
    if((pUacIn != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacIn, pUacIn, sizeof(stPrivDrvPhaseData_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:三相输入电流
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetIacIn(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacIn)
{
    if((pIacIn != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacIn, pIacIn, sizeof(stPrivDrvPhaseDataI_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:市电频率
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetFac(u8 u8GunId, i16 i16Fac)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Fac = i16Fac;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:三相输出电压或Udc
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUacOut(u8 u8GunId, stPrivDrvPhaseData_t *pUacOut)
{
    if((pUacOut != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacOut, pUacOut, sizeof(stPrivDrvPhaseData_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:三相输出电流或Idc
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetIacOut(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacOut)
{
    if((pIacOut != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacOut, pIacOut, sizeof(stPrivDrvPhaseDataI_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:总功率因数
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetPfTotal(u8 u8GunId, i16 i16PfTotal)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16PfTotal = i16PfTotal;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:三相功率因数
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetPf(u8 u8GunId, stPrivDrvPhaseData_t *pPf)
{
    if((pPf != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stPf, pPf, sizeof(stPrivDrvPhaseData_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:占空比
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetDuty(u8 u8GunId, i16 i16Duty)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Duty = i16Duty;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:CP电压
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUcp(u8 u8GunId, i16 i16Ucp)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Ucp = i16Ucp;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:CC电压
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUcc(u8 u8GunId, i16 i16Ucc)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Ucc = i16Ucc;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:直流漏电流偏移量 Offset0/Offset1
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetOffset(u8 u8GunId, i16 i16Offset0, i16 i16Offset1)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Offset0 = i16Offset0;
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Offset1 = i16Offset1;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:CT电流值
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetIct(u8 u8GunId, i32 i32Ict)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i32Ict = i32Ict;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:输入端对PE电压
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUpeIn(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeIn)
{
    if((pUpeIn != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeIn, pUpeIn, sizeof(stPrivDrvPhaseDataN_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:输出端对PE电压
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetUpeOut(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeOut)
{
    if((pUpeOut != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeOut, pUpeOut, sizeof(stPrivDrvPhaseDataN_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:交流漏电流值/直流漏电流值
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetIleak(u8 u8GunId, i16 i16IleakAc, i16 i16IleakDc)
{
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16IleakAc = i16IleakAc;
        pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16IleakDc = i16IleakDc;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:充电桩温度系列
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetChgTemp(stPrivDrvChgTempData_t *pTemp)
{
    if(pTemp != NULL)
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stChg.stChgTemp, pTemp, sizeof(stPrivDrvChgTempData_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:充电枪温度系列
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetGunTemp(u8 u8GunId, stPrivDrvGunTempData_t *pTemp)
{
    if((pTemp != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        memcpy(&pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stGunTemp, pTemp, sizeof(stPrivDrvGunTempData_t));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:遥测报文预留字段
* Author            :     Hall
* modified Date     :     2024-05-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRsvdDataYc(u8 u8GunId, i32 i32Len, u8 *pBuf)
{
    i32  i32DataLen;
    
    if((pBuf != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.u8Data);
        if(i32Len <= i32DataLen)
        {
            memcpy(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.u8Data, pBuf, i32Len);
            
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:触发遥测数据上报
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetYcRpt(u8 u8GunId)
{
    return sPrivDrvPktSend0E(u8GunId);
}








/**********************************************************************************************
* Description       :     可读可写数据 set:bms数据上报
* Author            :     Hall
* modified Date     :     2024-09-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetEvtNotice(stPrivDrvCmd11_t *pEvtNotice)
{
    return sPrivDrvPktSend11(pEvtNotice);
}










/**********************************************************************************************
* Description       :     可读可写数据 set:bms数据上报
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetBmsRpt(u8 u8GunId, stPrivDrvCmd13_t *pBmsData)
{
    return sPrivDrvPktSend13(u8GunId, pBmsData);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:预约/定时充电应答
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetBookingDataAck(u8 u8GunId, stPrivDrvCmd14_t *pBookingDataAck)
{
    return sPrivDrvPktSend14(u8GunId, pBookingDataAck);
}







/**********************************************************************************************
* Description       :     可读可写数据 主柜状态信息上报
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetMpcData(stPrivDrvCmd15_t *pMpcData, u16 u16ActualDataLen)
{
    return sPrivDrvPktSend15(pMpcData, u16ActualDataLen);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:液冷机状态上报
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCoolingData(u8 u8GunId, stPrivDrvCmd16_t *pCoolingData)
{
    return sPrivDrvPktSend16(u8GunId, pCoolingData);
}

































































/**********************************************************************************************
* Description       :     可读可写数据 get:签到结果
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetLoginAck(ePrivDrvCmdRst_t *pRst)
{
    if(pRst != NULL)
    {
        (*pRst) = pPrivDrvCache->unData.stSlave.stChg.stCacheA2.eRst;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:联网类型
* Author            :     Hall
* modified Date     :     2024-05-27
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetNetType(ePrivDrvNetHwType_t *pType)
{
    if(pType != NULL)
    {
        (*pType) = pPrivDrvCache->unData.stSlave.stChg.stCacheA2.eHwType;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:网关软件版本号
* Author            :     Hall
* modified Date     :     2024-05-27
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetGwVersion(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (i32MaxLen >= strlen((char *)pPrivDrvCache->unData.stSlave.stChg.stCacheA2.u8Ver)))
    {
        memset(pBuf, 0, i32MaxLen);
        memcpy(pBuf, pPrivDrvCache->unData.stSlave.stChg.stCacheA2.u8Ver, strlen((char *)pPrivDrvCache->unData.stSlave.stChg.stCacheA2.u8Ver));
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:桩连网状态
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetNetStatus(ePrivDrvNetStatus_t *pStatus)
{
    if(pStatus != NULL)
    {
        (*pStatus) = pPrivDrvCache->unData.stSlave.stChg.eNetStatus;
        return(true);
    }
    
    return(false);
}





/**********************************************************************************************
* Description       :     可读可写数据 get:信号强度
* Author            :     HH
* modified Date     :     2025-03021
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetSignalStr(i8 *pRssi)
{
    if(pRssi != NULL)
    {
        (*pRssi) = pPrivDrvCache->unData.stSlave.stChg.i8Rssi;
        return(true);
    }
    
    return(false);
}







/*******************************************************************************
 * @FunctionName   :      sPrivDrvGetMatserTime
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月19日  14:19:36
 * @Description    :      可读可写数据 get:网络时间
 * @Input          :      pTime       
 * @Return         :      
*******************************************************************************/
bool sPrivDrvGetMatserTime(stTime_t *pTime)
{
    bool bRst = pdFALSE;
    
    if(pTime != NULL)
    {
        (*pTime) = pPrivDrvCache->unData.stSlave.stChg.stMasterTime;
    }
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hTimeMutex, 0);
    }
        
    return ((bRst == pdTRUE) ? true : false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:（状态应答报文）预留字段
* Author            :     Hall
* modified Date     :     2023-10-26
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRsvdDataStsAck(u8 u8GunId, i32 i32MaxLen, u8 *pBuf)
{
    i32  i32DataLen;
    
    if((pBuf != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32DataLen = sizeof(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck);
        if(i32MaxLen >= i32DataLen)
        {
            memcpy(pBuf, pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck, i32DataLen);
            return(true);
        }
    }
    
    return(false);
}










/**********************************************************************************************
* Description       :     可读可写数据 get:卡认证应答报文 信号量
* Author            :     Hall
* modified Date     :     2024-06-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCardAuthAck(u8 u8GunId, stPrivDrvCmdA5_t **pCardAuth)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pCardAuth != NULL)
        {
            (*pCardAuth) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stCardAuthAckData;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheA5Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}













/**********************************************************************************************
* Description       :     可读可写数据 get:远程操作报文 信号量
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRmt(u8 u8GunId, stPrivDrvCmdA6_t **pRmt)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pRmt != NULL)
        {
            (*pRmt) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stRmt;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheA6Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:启动充电应答 信号量
* Author            :     Hall
* modified Date     :     2024-06-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetTransAck(u8 u8GunId, stPrivDrvCmdA7_t **pTransAck)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pTransAck != NULL)
        {
            (*pTransAck) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stTransAck;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheA7Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}








/**********************************************************************************************
* Description       :     可读可写数据 get:充电中实时数据应答 报文信号
* Author            :     Hall
* modified Date     :     2024-06-01
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRtRptAck(u8 u8GunId, stPrivDrvCmdA8_t **pRtDataAck)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pRtDataAck != NULL)
        {
            (*pRtDataAck) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stRtDataAck;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheA8Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}





/**********************************************************************************************
* Description       :     可读可写数据 get:结算应答报文 信号
* Author            :     Hall
* modified Date     :     2024-06-01
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRcdRptAck(u8 u8GunId, stPrivDrvCmdA9_t **pRcdDataAck)
{
    bool bRst;
    
    bRst = pdFALSE;
    if(pRcdDataAck != NULL)
    {
        (*pRcdDataAck) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stRcdDataAck;
    }
    
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheA9Mutex, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:CMDAA报文信号
* Author            :     Hall
* modified Date     :     2024-06-12
* notice            :     用于桩端主动上报信号量
*                         bRptFlag:false:获取网关被动应答的AA数据,   true:获取网关主动下发的AA数据
***********************************************************************************************/
bool sPrivDrvGetCmdParam(bool bRptFlag, stPrivDrvCmdAA_t **pCmdAA)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((pPrivDrvCache != NULL) && (pCmdAA != NULL))
    {
        if(bRptFlag == true)
        {
            (*pCmdAA) = &pPrivDrvCache->unData.stSlave.stChg.stCacheAARpt;
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheAARptMutex, 0);
        }
        else
        {
            (*pCmdAA) = &pPrivDrvCache->unData.stSlave.stChg.stCacheAAAck;
            bRst = pdTRUE;
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}











/*******************************************************************************
 * @FunctionName   :      sPrivDrvGetChgOta
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  11:30:10
 * @Description    :      可读可写数据 get:桩端Ota升级应答报文 信号
 * @Input          :      pOta        
 * @Return         :      
*******************************************************************************/
bool sPrivDrvGetChgOta(stPrivDrvCmdAB_t **pOta)
{
    bool bRst;
    
    bRst = pdFALSE;
    if(pOta != NULL)
    {
        (*pOta) = &pPrivDrvCache->unData.stSlave.stChg.stCacheAB;
    }
    
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheABMutex, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}











/**********************************************************************************************
* Description       :     可读可写数据 get:CMDAC报文信号
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetModOta(stPrivDrvCmdAC_t **pOta)
{
    bool bRst;
    
    bRst = pdFALSE;
    if(pOta != NULL)
    {
        (*pOta) = &pPrivDrvCache->unData.stSlave.stChg.stCacheAC;
    }
    
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stChg.hCacheACMutex, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}








/**********************************************************************************************
* Description       :     可读可写数据 get:遥测应答报文预留字段
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRsvdDataYcAck(u8 u8GunId, i32 i32MaxLen, u8 *pBuf)
{
    i32  i32Size;
    
    if((pBuf != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck);
        if(i32MaxLen >= i32Size)
        {
            memcpy(pBuf, pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck, i32Size);
            return(true);
        }
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可读可写数据 get:安全事件通知应答 信号
* Author            :     Hall
* modified Date     :     2024-09-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetEvtNoticeAck(stPrivDrvCmdB1_t **pEvtNoticeAck)
{
    if(pEvtNoticeAck != NULL)
    {
        (*pEvtNoticeAck) = &pPrivDrvCache->unData.stSlave.stChg.stCacheB1;
        
        return true;
    }
    
    return false;
}









/**********************************************************************************************
* Description       :     可读可写数据 get:B4预约报文信号
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetBookingData(u8 u8GunId, stPrivDrvCmdB4_t **pBookingData)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pBookingData != NULL)
        {
            (*pBookingData) = &pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].stBookingData;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].hCacheB4Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}










#endif














