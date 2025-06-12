/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_vin.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 Vin码充电功能
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"


//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate_vin";




extern stPileCnCache_t *pPileCnCache;








void sPileCnGateVinAuth(ePileGunIndex_t eGunIndex);
void sPileCnGateVinAuthAck(ePileGunIndex_t eGunIndex);


void sPileCnGateVinStart(ePileGunIndex_t eGunIndex);
void sVinAppStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param);





/***************************************************************************************************
* Description                           :   Vin码鉴权
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
void sPileCnGateVinAuth(ePileGunIndex_t eGunIndex)
{
    stVinData_t *pVin = &pPileCnCache->stGateData.stVin[eGunIndex];
    
    //判断是否开始Vin码充电流程
    if(pVin->bVinChargeStart == true)
    {
        if((sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusPreparing) && (sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeNone))
        {
            EN_SLOGD(TAG, "开始VIn码鉴权");
            //通知鉴权获取Vin码
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeVin);
        }
        else if((sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusFinishing) || (sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusFaulted) || (sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusAvailable))
        {
            //清除Vin码启动标志防止多次鉴权
            pVin->bVinChargeStart = false;
        }
        
        //Vin码获取成功
        if((sPileCnGet_VinChgCheckState(eGunIndex) == eVinChgCheckWait) && (sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeVin))
        {
            EN_SLOGD(TAG, "开始VIn码启动");
            sPileCnGateVinStart(eGunIndex);
            
            //清除Vin码启动标志
            pVin->bVinChargeStart = false;
        }
    }
}






/***************************************************************************************************
* Description                           :   Vin码认证应答
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
void sPileCnGateVinAuthAck(ePileGunIndex_t eGunIndex)
{
    u32  u32Meter;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    stPrivDrvCmdA5_t *pVinAuthAck = NULL;
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    
    if(sPileCnGet_Authorize(eGunIndex) != eAuthorizeTypeVin)
    {
        //VIN码充电情况下才执行后续流程
        //避免把刷卡充电的A5报文信号量破坏了
        return;
    }
    
    
    //收到A5报文
    if(sPrivDrvGetCardAuthAck(u8GunId, &pVinAuthAck) == true)
    {
        if((pVinAuthAck->eFailReason == ePrivDrvCardAuthFailRsn00)
        && (pVinAuthAck->eAccountStatus == ePrivDrvCardAccountStatusValid))
        {
            //获取电表读数
            sMeterDrvOptGetEnergy(eGunIndex, &u32Meter, NULL);
            
            //发送07报文
            memset(&stTrans, 0, sizeof(stTrans));
            stTrans.u8GunId = u8GunId;
            stTrans.eCmd = ePrivDrvStartTransCmdStart;
            stTrans.eType = ePrivDrvStartTransTypeVin;
            stTrans.stTime = sGetTime();
            stTrans.u32Meter = u32Meter;
            sPrivDrvSetTrans(u8GunId, &stTrans);
            
            if((sPrivDrvGetTransAck(u8GunId, &pTransAck) == true)
            && (pTransAck->eRst == ePrivDrvCmdRstSuccess))
            {
                //启动充电
                sPileCnSet_VinChgCheckState( eGunIndex, eVinChgCheckSuccess);
                sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStartByCard);
                
                //初始化并创建订单
                sPileCnGateNewRcdInitForVin(eGunIndex, pVinAuthAck, pTransAck);
                sEepromAppGetBlockOperatorRate(&pData->unRate);
                
                //启动计量
                sOrderStart(eGunIndex, sEepromAppNewBlockChgRcd(pRcd), pRcd, &pData->unRate, sPileCnGateGetSoc);
                
            }
            else if((sPrivDrvGetTransAck(u8GunId, &pTransAck) == true)
            && (pTransAck->eRst == ePrivDrvCmdRstFail))
            {
                sPileCnSet_VinChgCheckState( eGunIndex, eVinChgCheckFailed);
            }
        }
        else 
        {
            sPileCnSet_VinChgCheckState( eGunIndex, eVinChgCheckFailed);
        }
    }
}









/***************************************************************************************************
* Description                           :   Vin码开始充电
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
void sPileCnGateVinStart(ePileGunIndex_t eGunIndex)
{
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    
    stPrivDrvCmd05_t stVinAuth;
    stVinData_t *pVin = &pData->stVin[eGunIndex];
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    //获取vin码
    sBmsGbt15Get_VinCode(eGunIndex,pVin->u8Vin);
    
    memset(&stVinAuth, 0, sizeof(stVinAuth));
    stVinAuth.u8GunId = u8GunId;
    stVinAuth.eType = pVin->eType;
    stVinAuth.eStatus = ePrivDrvCardLockStatusUnlock;
    stVinAuth.u32CardBalance = pVin->u32Balance;
    stVinAuth.stTime = pVin->stTime;
    stVinAuth.eMode = pVin->eMode;
    stVinAuth.unParam.u32Value = pVin->unParam.u32Value;
    memcpy(stVinAuth.u8UserId, pVin->u8Vin, sizeof(pVin->u8Vin));
    sPrivDrvSetCardAuth(u8GunId, &stVinAuth);
}





/***************************************************************************************************
* Description                           :   开启Vin码充电流程
* Author                                :   Dai
* Creat Date                            :   2024-06-12
* notice                                :     
****************************************************************************************************/
void sVinAppStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param)
{
    stVinData_t *pVin = &pPileCnCache->stGateData.stVin[eGunIndex];
    
    pVin->bVinChargeStart = true;
    pVin->eType = ePrivDrvCardTypeVin;
    pVin->stTime = sGetTime();
    pVin->eGunIndex = eGunIndex;
    pVin->eStartType = ePrivDrvStartTransTypeVin;
    pVin->eMode = eMode;
    pVin->unParam.u32Value = u32Param;
}










