/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_api_m.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     完整的内网协议包含两个方向，网关和桩端，本驱动需要实现
*                                             完整的内网协议驱动，
*
*                                             定义网关为master，缩写m，桩端为slave，缩写s

*                                             本文件实现master侧的抽象数据接口api
****************************************************************************************************/
#include "private_drv_opt.h"




#if (cSdkPrivDevType == cSdkPrivDevTypeM)




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_api_m";





extern stPrivDrvCache_t *pPrivDrvCache;








/**********************************************************************************************
* Description       :     Master 端缓存数据的初始化
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvVarInitM(void)
{
    i32  i;
    
    EN_SLOGI(TAG, "Master 端缓存数据的初始化");
    pPrivDrvCache->unData.stMaster.stChg.u16Addr    = cPrivDrvAddrDefault;
    pPrivDrvCache->unData.stMaster.stChg.eLoginAck  = ePrivDrvCmdRstFail;
    pPrivDrvCache->unData.stMaster.stChg.u16EleLoss = 1000;
    
    for(i = 0; i < cPrivDrvGunNumMax; i++)
    {
        pPrivDrvCache->unData.stMaster.stGun[i].hCache03Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache05Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache06Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache07Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache08Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache09Mutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache0EMutex   = xSemaphoreCreateBinary();
        pPrivDrvCache->unData.stMaster.stGun[i].hCache13Mutex   = xSemaphoreCreateBinary();
        
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache03Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache05Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache06Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache07Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache08Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache09Mutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache0EMutex, 0);
        xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[i].hCache13Mutex, 0);
    }
    
    pPrivDrvCache->unData.stMaster.stChg.hCache03Mutex   = xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stMaster.stChg.hCache04Mutex   = xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stMaster.stChg.hCache0ARptMutex= xSemaphoreCreateBinary();
    pPrivDrvCache->unData.stMaster.stChg.hCache11Mutex   = xSemaphoreCreateBinary();
    xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache03Mutex, 0);
    xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache04Mutex, 0);
    xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache0ARptMutex, 0);
    xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache11Mutex, 0);
    
    return(true);
}








/**********************************************************************************************
* Description       :     可读数据 get:协议版本
* Author            :     XRG
* modified Date     :     2024-01-25
* notice            :     获取当前桩接收的协议版本(目前支持3.0、3.6内网协议)
***********************************************************************************************/
bool sPrivDrvGetProtocolVersion(ePrivDrvVersion_t *pVer)
{
    if((pVer != NULL) && (pPrivDrvCache != NULL))
    {
        (*pVer) = pPrivDrvCache->unData.stMaster.stChg.eVersion;
        return(true);
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可读可写数据 get:充电桩地址
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetChargerAddr(u16 *pAddr)
{
    if((pAddr != NULL) && (pPrivDrvCache != NULL))
    {
        (*pAddr) = pPrivDrvCache->unData.stMaster.stChg.u16Addr;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:ack发送 报文序列号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetAckSeqno(u8 *pSeqno)
{
    if((pSeqno != NULL) && (pPrivDrvCache != NULL))
    {
        (*pSeqno) = pPrivDrvCache->unData.stMaster.stChg.u8AckSeqno;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:rpt发送 报文序列号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRptSeqno(u8 *pSeqno)
{
    if((pSeqno != NULL) && (pPrivDrvCache != NULL))
    {
        (*pSeqno) = pPrivDrvCache->unData.stMaster.stChg.u8RptSeqno;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:充电桩类型
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetChargerType(ePrivDrvChargerType_t *pType)
{
    if((pType != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pType) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eType;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:结算时机
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetPayAt(ePrivDrvPayAt_t *pPayAt)
{
    if((pPayAt != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pPayAt) = pPrivDrvCache->unData.stMaster.stChg.stCache02.ePayAt;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:计费端
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetMoneyCalcBy(ePrivDrvMoneyCalcBy_t *pMoneyCalcBy)
{
    if((pMoneyCalcBy != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pMoneyCalcBy) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eMoneyCalcBy;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:桩禁用状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetDisableFlag(ePrivDrvChargerDisableFlag_t *pFlag)
{
    if((pFlag != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pFlag) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eDisableFlag;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:功率分配启动模式
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetStartPowerMode(ePrivDrvStartPowerMode_t *pMode)
{
    if((pMode != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pMode) = pPrivDrvCache->unData.stMaster.stChg.stCache02.ePowerMode;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:桩工作模式
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetWorkMode(ePrivDrvChargerWorkMode_t *pMode)
{
    if((pMode != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pMode) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eWorkMode;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:三相平衡挂载相位
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetBalancePhase(ePrivDrvBalancePhase_t *pBalancePhase)
{
    if((pBalancePhase != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pBalancePhase) = pPrivDrvCache->unData.stMaster.stChg.stCache02.ePhase;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:最大功率
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetPowerMax(i32 *pPower)
{
    if((pPower != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pPower) = pPrivDrvCache->unData.stMaster.stChg.stCache02.i32PowerMax;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:最大电流
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCurrMax(i32 *pCurr)
{
    if((pCurr != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pCurr) = pPrivDrvCache->unData.stMaster.stChg.stCache02.i32CurrMax;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:额定功率
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetPowerRated(i32 *pPower)
{
    if((pPower != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pPower) = pPrivDrvCache->unData.stMaster.stChg.stCache02.i32PowerRated;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:额定电流
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCurrRated(i32 *pCurr)
{
    if((pCurr != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pCurr) = pPrivDrvCache->unData.stMaster.stChg.stCache02.i32CurrRated;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:是否支持蓝牙预约
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetBookingViaBle(ePrivDrvBookingViaBle_t *pBookingViaBle)
{
    if((pBookingViaBle != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pBookingViaBle) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eBookingViaBle;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:登录服务器标志
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetLoginedFlag(ePrivDrvLoginedFlag_t *pFlag)
{
    if((pFlag != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pFlag) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eLoginedFlag;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:重启原因
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRebootReason(ePrivDrvRebootReason_t *pReason)
{
    if((pReason != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pReason) = pPrivDrvCache->unData.stMaster.stChg.stCache02.eReason;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:桩SN号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetSn(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8Sn);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8Sn, i32Size);
            return(true);
        }
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:设备型号代码 
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetDevCode(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8DevCode);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8DevCode, i32Size);
            return(true);
        }
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:设备名称
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetDevName(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8DevName);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8DevName, i32Size);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:硬件版本号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetHwVer(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8HwVer);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8HwVer, i32Size);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:软件版本号
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetSwVer(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8SwVer);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8SwVer, i32Size);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:枪口数量
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetGunNum(u8 *pGunNum)
{
    if((pGunNum != NULL) && (pPrivDrvCache != NULL) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
    {
        (*pGunNum) = pPrivDrvCache->unData.stMaster.stChg.u8GunNum;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:枪工作状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
                          注意cPrivDrvWorkStsConvertEn是否使能
***********************************************************************************************/
bool sPrivDrvGetGunWorkStatus(u8 u8GunId, ePrivDrvGunWorkStatus_t *pStatus)
{
    if((pStatus != NULL) && (pPrivDrvCache != NULL) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(u8GunId == cPrivDrvGunIdChg)
        {
            (*pStatus) = pPrivDrvCache->unData.stMaster.stChg.stCache03.eGunWorkStatus;
        }
        else if(u8GunId >= cPrivDrvGunIdBase)
        {
            (*pStatus) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus;
        }
        else
        {
            return(false);
        }
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:枪连接状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvGetGunConnStatus(u8 u8GunId, ePrivDrvGunConnStatus_t *pStatus)
{
    if((pStatus != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pStatus) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eConnStatus;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:枪  锁状态
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvGetGunLockStatus(u8 u8GunId, ePrivDrvGunLockStatus_t *pStatus)
{
    if((pStatus != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pStatus) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eLockStatus;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:（签到信息）预留字段
* Author            :     Hall
* modified Date     :     2023-10-21
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRsvdDataLogin(i32 i32MaxLen, u8 *pBuf)
{
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32  i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCache02.u8Data);
        if((i32MaxLen >= i32Size) && (pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == true))
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stChg.stCache02.u8Data, i32Size);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:枪事件
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvGetGunEvent(u8 u8GunId, ePrivDrvGunEvent_t *pEvent)
{
    if((pEvent != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pEvent) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eEvent;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:枪工作状态上一次值
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
                          注意cPrivDrvWorkStsConvertEn是否使能
***********************************************************************************************/
bool sPrivDrvGetGunWorkStatusOld(u8 u8GunId, ePrivDrvGunWorkStatus_t *pStatus)
{
    if((pStatus != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pStatus) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatusOld;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:枪状态变化原因
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     注意枪号 u8GunId:0---表示整桩,   1~cPrivDrvGunNumMax:表示枪号
***********************************************************************************************/
bool sPrivDrvGetGunStsReason(u8 u8GunId, ePrivDrvGunWorkStsRsn_t *pReason)
{
    if((pReason != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pReason) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].eStsReason;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:故障码---旧的故障位字段
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetFaultByte(unPrivDrvFaultByte_t *pFaultByte)
{
    if((pFaultByte != NULL) && (pPrivDrvCache != NULL))
    {
        memcpy(pFaultByte, pPrivDrvCache->unData.stMaster.stChg.stCache03.unFaultByte.u8Buf, sizeof(unPrivDrvFaultByte_t));
        return(true);
    }
    
    return(false);
}





/**********************************************************************************************
* Description       :     可读可写数据 get:告警码---旧的告警位字段
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetWarningByte(unPrivDrvWarningByte_t *pWarningByte)
{
    if((pWarningByte != NULL) && (pPrivDrvCache != NULL))
    {
        memcpy(pWarningByte, pPrivDrvCache->unData.stMaster.stChg.stCache03.unWarningByte.u8Buf, sizeof(unPrivDrvWarningByte_t));
        return(true);
    }
    
    return(false);
}





/**********************************************************************************************
* Description       :     可读可写数据 get:当前时间
* Author            :     XRG
* modified Date     :     2024-03-19
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetNewTime(stTime_t *pTime)
{
    if((pTime != NULL) && (pPrivDrvCache != NULL))
    {
        memcpy(pTime, &pPrivDrvCache->unData.stMaster.stChg.stCache03.stTime, sizeof(stTime_t));
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:故障码---新的故障告警码表字段
* Author            :     Hall
* modified Date     :     2024-06-17
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetFaultCode(u8 u8GunId, stPrivDrvFaultCode_t *pFaultCode)
{
    if((pFaultCode != NULL) && (pPrivDrvCache != NULL))
    {
        if(u8GunId == cPrivDrvGunIdChg)
        {
            memcpy(pFaultCode, &pPrivDrvCache->unData.stMaster.stChg.stCache03.stFaultCode, sizeof(stPrivDrvFaultCode_t));
        }
        else if(u8GunId >= cPrivDrvGunIdBase)
        {
            memcpy(pFaultCode, &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stFaultCode, sizeof(stPrivDrvFaultCode_t));
        }
        
        return(true);
    }
    
    return(false);
}





/**********************************************************************************************
* Description       :     可读数据 get:桩状态上报
* Author            :     XRG
* modified Date     :     2024-04-19
* notice            :     用于cmd03实时数据信号量,应用层调用后获取数据
***********************************************************************************************/
bool sPrivDrvGetStatusRep(u8 u8GunId, u32 u32WaitTime)
{
    bool bRst;
    
    bRst = pdFALSE;
    if(u8GunId == cPrivDrvGunIdChg)
    {
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache03Mutex, u32WaitTime / portTICK_RATE_MS);
        }
    }
    else if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache03Mutex, u32WaitTime / portTICK_RATE_MS);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}








/**********************************************************************************************
* Description       :     可读可写数据 get:04桩心跳
* Author            :     XRG
* modified Date     :     2024-01-09
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetHeartbeat(void)
{
    bool bRst = false;
    
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache04Mutex, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:卡认证请求信号
* Author            :     Hall
* modified Date     :     2023-10-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCardAuth(u8 u8GunId, stPrivDrvCmd05_t **pCardAuth)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pCardAuth != NULL)
        {
            (*pCardAuth) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCardAuthData;
        }
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache05Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:远程操作命令应答
* Author            :     Hall
* modified Date     :     2024-06-08
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRmtAck(u8 u8GunId, stPrivDrvCmd06_t **pRmtAck)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pRmtAck != NULL)
        {
            (*pRmtAck) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stRmtAck;
        }
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache06Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}














/**********************************************************************************************
* Description       :     可读可写数据 get:开始/结束充电信号
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetTrans(u8 u8GunId, stPrivDrvCmd07_t **pTrans)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pTrans != NULL)
        {
            (*pTrans) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stTrans;
        }
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache07Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:充电中实时数据上报 信号
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRtRpt(u8 u8GunId, stPrivDrvCmd08_t **pRtData)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pRtData != NULL)
        {
            (*pRtData) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stRtData;
        }
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache08Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:结算报文上报 信号
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRcdRpt(u8 u8GunId, stPrivDrvCmd09_t **pRcdData)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pRcdData != NULL)
        {
            (*pRcdData) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stRcdData;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache09Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:参数管理应答 信号
* Author            :     Hall
* modified Date     :     2023-11-02
* notice            :     用于桩端主动上报信号量
*                         bRptFlag:false:获取桩端被动应答0A数据,   true:获取桩端主动上报的0A数据
***********************************************************************************************/
bool sPrivDrvGetCmd0A(bool bRptFlag, stPrivDrvCmd0A_t **pCmd0A)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((pPrivDrvCache != NULL) && (pCmd0A != NULL))
    {
        if(bRptFlag == true)
        {
            (*pCmd0A) = &pPrivDrvCache->unData.stMaster.stChg.stCache0ARpt;
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache0ARptMutex, 0);
        }
        else
        {
            (*pCmd0A) = &pPrivDrvCache->unData.stMaster.stChg.stCache0AAck;
            bRst = pdTRUE;
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}







/**********************************************************************************************
* Description       :     可读可写数据 get:输入电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUacIn(u8 u8GunId, stPrivDrvPhaseData_t *pUacIn)
{
    i32  i32Size;
    
    if((pUacIn != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacIn);
        memcpy(pUacIn,  &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacIn, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:输入电流
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetIacIn(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacIn)
{
    i32  i32Size;
    
    if((pIacIn != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacIn);
        memcpy(pIacIn,  &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacIn, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:市电频率
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetFac(u8 u8GunId, i16 *pFac)
{
    if((pFac != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pFac) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Fac;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:输出电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUacOut(u8 u8GunId, stPrivDrvPhaseData_t *pUacOut)
{
    i32  i32Size;
    
    if((pUacOut != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacOut);
        memcpy(pUacOut, &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUacOut, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:输出电流
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetIacOut(u8 u8GunId, stPrivDrvPhaseDataI_t *pIacOut)
{
    i32  i32Size;
    
    if((pIacOut != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacOut);
        memcpy(pIacOut, &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stIacOut, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:功率因数---总
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetPfTotal(u8 u8GunId, i16 *pPfTotal)
{
    if((pPfTotal != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pPfTotal) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16PfTotal;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:功率因数---三相
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetPf(u8 u8GunId, stPrivDrvPhaseData_t *pPf)
{
    i32  i32Size;
    
    if((pPf != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stPf);
        memcpy(pPf,     &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stPf, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:duty
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetDuty(u8 u8GunId, i16 *pDuty)
{
    if((pDuty != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pDuty) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Duty;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:cp电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUcp(u8 u8GunId, i16 *pUcp)
{
    if((pUcp != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pUcp) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Ucp;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:cc电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUcc(u8 u8GunId, i16 *pUcc)
{
    if((pUcc != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pUcc) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Ucc;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:cc电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetOffset(u8 u8GunId, i16 *pOffset0, i16 *pOffset1)
{
    if((pOffset0 != NULL) && (pPrivDrvCache != NULL) && (pOffset1 != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pOffset0) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Offset0;
        (*pOffset1) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16Offset1;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:CT电流
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetIct(u8 u8GunId, i32 *pIct)
{
    if((pIct != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pIct) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i32Ict;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:输入端对地电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUpeIn(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeIn)
{
    i32  i32Size;
    
    if((pUpeIn != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeIn);
        memcpy(pUpeIn,  &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeIn, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:输出端对地电压
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetUpeOut(u8 u8GunId, stPrivDrvPhaseDataN_t *pUpeOut)
{
    i32  i32Size;
    
    if((pUpeOut != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeOut);
        memcpy(pUpeOut, &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stUpeOut, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:漏电流
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetIleak(u8 u8GunId, i16 *pIleakAc, i16 *pIleakDc)
{
    if((pIleakAc != NULL) && (pIleakDc != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        (*pIleakAc) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16IleakAc;
        (*pIleakDc) = pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.i16IleakDc;
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:充电桩温度数据
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetChgTemp(stPrivDrvChgTempData_t *pTemp)
{
    i32  i32Size;
    
    if((pTemp != NULL) && (pPrivDrvCache != NULL))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stChgTemp);
        memcpy(pTemp,   &pPrivDrvCache->unData.stMaster.stChg.stChgTemp, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:充电枪温度数据
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetGunTemp(u8 u8GunId, stPrivDrvGunTempData_t *pTemp)
{
    i32  i32Size;
    
    if((pTemp != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stGunTemp);
        memcpy(pTemp,   &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.stGunTemp, i32Size);
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可读可写数据 get:遥测帧预留数据
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetRsvdDataYc(u8 u8GunId, i32 i32MaxLen, u8 *pBuf)
{
    i32  i32Size;
    
    if((pBuf != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.u8Data);
        if(i32MaxLen >= i32Size)
        {
            memcpy(pBuf, pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCache0E.u8Data, i32Size);
            return(true);
        }
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可读数据 get:遥测数据 信号
* Author            :     Hall
* modified Date     :     2024-10-09
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetYc(u8 u8GunId)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache0EMutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}









/**********************************************************************************************
* Description       :     可读数据 get:安全事件通知 信号
* Author            :     Hall
* modified Date     :     2024-09-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetEvtNotice(stPrivDrvCmd11_t **pEvtNotice)
{
    bool bRst;
    
    bRst = pdFALSE;
    if(pEvtNotice != NULL)
    {
        (*pEvtNotice) = &pPrivDrvCache->unData.stMaster.stChg.stCache11;
    }
    
    if(pPrivDrvCache != NULL)
    {
        bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stChg.hCache11Mutex, 0);
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读数据 get:BMS信息上报 信号
* Author            :     XRG
* modified Date     :     2024-05-20
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetBmsRpt(u8 u8GunId, stPrivDrvCmd13_t **pBmsData)
{
    bool bRst;
    
    bRst = pdFALSE;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pBmsData != NULL)
        {
            (*pBmsData) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stBmsData;
        }
        
        if(pPrivDrvCache != NULL)
        {
            bRst = xSemaphoreTake(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].hCache13Mutex, 0);
        }
    }
    
    return((bRst == pdTRUE) ? true : false);
}






/**********************************************************************************************
* Description       :     可读数据 get:预约/定时充电应答 
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetBookingDataAck(u8 u8GunId, stPrivDrvCmd14_t **pBookingDataAck)
{
    bool bRst;
    
    bRst = false;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pBookingDataAck != NULL)
        {
            (*pBookingDataAck) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stBookingDataAck;
        }
        
        bRst = true;
    }
    
    return bRst;
}







/**********************************************************************************************
* Description       :     可读数据 get:主柜状态信息
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetMpcData(stPrivDrvCmd15_t **pMpcData)
{
    if(pMpcData != NULL)
    {
        (*pMpcData) = &pPrivDrvCache->unData.stMaster.stChg.stMpcData;
        
        return true;
    }
    
    return false;
}







/**********************************************************************************************
* Description       :     可读数据 get:液冷机状态
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCoolingData(u8 u8GunId, stPrivDrvCmd16_t **pCoolingData)
{
    bool bRst;
    
    bRst = false;
    if((u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        if(pCoolingData != NULL)
        {
            (*pCoolingData) = &pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].stCoolingData;
        }
        
        bRst = true;
    }
    
    return bRst;
}






























/**********************************************************************************************
* Description       :     可写数据 set:桩签到状态
* Author            :     XRG
* modified Date     :     2024-05-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetLoginAck(ePrivDrvCmdRst_t eLogin)
{
    if((eLogin < ePrivDrvCmdRstMax) && (pPrivDrvCache != NULL))
    {
        pPrivDrvCache->unData.stMaster.stChg.eLoginAck      = eLogin;
        pPrivDrvCache->unData.stMaster.stChg.stCacheA2.eRst = eLogin;
        return(sPrivDrvPktSendA2());
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可写数据 set:桩连网类型
* Author            :     XRG
* modified Date     :     2024-05-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetNetType(ePrivDrvNetHwType_t eHwType)
{
    if((eHwType < ePrivDrvNetHwTypeMax) && (pPrivDrvCache != NULL))
    {
        pPrivDrvCache->unData.stMaster.stChg.eHwType           = eHwType;
        pPrivDrvCache->unData.stMaster.stChg.stCacheA2.eHwType = eHwType;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可写数据 set:网关版本号
* Author            :     XRG
* modified Date     :     2024-05-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetGwVersion(i32 i32DatLen, u8 *pBuf)
{
    i32  i32Size;
    
    if((pBuf != NULL) && (pPrivDrvCache != NULL))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stChg.stCacheA2.u8Ver);
        if(i32DatLen <= i32Size)
        {
            memset(pPrivDrvCache->unData.stMaster.stChg.stCacheA2.u8Ver, 0,    i32Size);
            memcpy(pPrivDrvCache->unData.stMaster.stChg.stCacheA2.u8Ver, pBuf, i32DatLen);
            return(true);
        }
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可写数据 set:桩连网状态
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetNetStatus(ePrivDrvNetStatus_t eStatus)
{
    if((eStatus < ePrivDrvNetStatusMax) && (pPrivDrvCache != NULL))
    {
        pPrivDrvCache->unData.stMaster.stChg.eNetStatus = eStatus;
        return(true);
    }
    
    return(false);
}








/**********************************************************************************************
* Description       :     可写数据 set:桩连网信号质量
* Author            :     XRG
* modified Date     :     2024-05-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRssi(i8 i8Rssi)
{
    if(pPrivDrvCache != NULL)
    {
        pPrivDrvCache->unData.stMaster.stChg.i8Rssi = i8Rssi;
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:（状态应答报文）预留字段
* Author            :     Hall
* modified Date     :     2023-10-26
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRsvdDataStsAck(u8 u8GunId, i32 i32DatLen, u8 *pBuf)
{
    i32  i32Size;
    
    if((pBuf != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck);
        if(i32DatLen <= i32Size)
        {
            memset(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck, 0, i32Size);
            memcpy(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck, pBuf, i32DatLen);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可写数据 set:心跳应答
* Author            :     XRG
* modified Date     :     2024-05-13
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetHbAck(void)
{
    return sPrivDrvPktSendA4();
}






/**********************************************************************************************
* Description       :     可读可写数据 set:卡认证请求应答
* Author            :     Hall
* modified Date     :     2023-10-28
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCardAuthAck(u8 u8GunId, stPrivDrvCmdA5_t *pCardAuthAckData)
{
    return sPrivDrvPktSendA5(u8GunId, pCardAuthAckData);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:远程操作指令发送
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRmt(u8 u8GunId, stPrivDrvCmdA6_t *pRmt)
{
    return sPrivDrvPktSendA6(u8GunId, pRmt);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:开始/结束充电应答
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetTransAck(u8 u8GunId, stPrivDrvCmdA7_t *pTransAck)
{
    return sPrivDrvPktSendA7(u8GunId, pTransAck);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:充电中实时数据应答 发送
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRtRptAck(u8 u8GunId, ePrivDrvCmdRst_t eRst, u32 u32AccountBalance, i32 i32RsvdLen, const u8 *pRsvd)
{
    return sPrivDrvPktSendA8(u8GunId, eRst, u32AccountBalance, i32RsvdLen, pRsvd);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:结算报文应答 
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRcdRptAck(u8 u8GunId, ePrivDrvCmdRst_t eRst, i32 i32OrderIdLen, const u8 *pOrderId, i32 i32SrvTradeIdLen, const u8 *pSrvTradeId)
{
    return sPrivDrvPktSendA9(u8GunId, eRst, i32OrderIdLen, pOrderId, i32SrvTradeIdLen, pSrvTradeId);
}






/**********************************************************************************************
* Description       :     可写数据 set:复位充电中数据
* Author            :     XRG
* modified Date     :     2024-05-09
* notice            :     应根据平台要求充电结束后,是否需要清除充电中数据
***********************************************************************************************/
bool sPrivDrvSetResetChargingData(u8 u8GunId)
{
    u8   u8GunIndex;
    stPrivDrvDataMaster_t   *pData   = &pPrivDrvCache->unData.stMaster;
    
    if((pData != NULL) && ((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax)))
    {
        return(false);
    }
    
    //cmd08
    u8GunIndex = u8GunId - cPrivDrvGunIdBase;
    pData->stGun[u8GunIndex].u32ChargingPower = 0;
    memset(&pData->stGun[u8GunIndex].stRtData, 0, sizeof(pData->stGun[u8GunIndex].stRtData));
    
    //cmd09
    memset(&pData->stGun[u8GunIndex].stRcdData, 0, sizeof(pData->stGun[u8GunIndex].stRcdData));
    
    return true;
}







/**********************************************************************************************
* Description       :     可读可写数据 set:参数管理--->下发参数
* Author            :     Hall
* modified Date     :     2023-11-02
* notice            :     
* Author            :     XRG
* modified Date     :     2024-03-28
* notice            :     增加失败重发次数
***********************************************************************************************/
bool sPrivDrvSetCmdAA(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam, u8 u8RepeatCnt)
{
    bool bRst;
    u8   u8Cnt  = 0;
    
    do
    {
        bRst = sPrivDrvPktSendAA(u8GunId, bRptFlag, eType, eRst, eAddr, u16ParamLen, pParam);
        if(u8Cnt++ >= u8RepeatCnt)
        {
            break;
        }
    }while (bRst == false);
    return(bRst);
}






/**********************************************************************************************
* Description       :     可读可写数据 set:桩端固件安装
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetChgUpdateFileInstall(const char *pPath)
{
    return chgFwUpdateFileInstall(pPath);
}







/**********************************************************************************************
* Description       :     可读可写数据 set:模块电源固件安装
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetModUpdateFileInstall(const char *pPath)
{
    return modFwUpdateFileInstall(pPath);
}






/**********************************************************************************************
* Description       :     可写数据 set:遥测应答帧预留数据
* Author            :     Hall
* modified Date     :     2023-11-07
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetRsvdDataYcAck(u8 u8GunId, i32 i32DatLen, u8 *pBuf)
{
    i32  i32Size;
    
    if((pBuf != NULL) && (pPrivDrvCache != NULL) && (u8GunId >= cPrivDrvGunIdBase) && (u8GunId <= cPrivDrvGunNumMax))
    {
        i32Size = sizeof(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck);
        if(i32DatLen <= i32Size)
        {
            memset(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck, 0, i32Size);
            memcpy(pPrivDrvCache->unData.stMaster.stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck, pBuf, i32DatLen);
            return(true);
        }
    }
    
    return(false);
}






/**********************************************************************************************
* Description       :     可写数据 set:安全事件通知应答
* Author            :     Hall
* modified Date     :     2024-09-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetEvtNoticeAck(stPrivDrvCmdB1_t *pEvtNoticeAck)
{
    return sPrivDrvPktSendB1(pEvtNoticeAck);
}







/**********************************************************************************************
* Description       :     可写数据 set:预约/定时充电
* Author            :     Hall
* modified Date     :     2024-08-10
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetBookingData(u8 u8GunId, stPrivDrvCmdB4_t *pBookingData)
{
    return sPrivDrvPktSendB4(u8GunId, pBookingData);
}



























#endif












