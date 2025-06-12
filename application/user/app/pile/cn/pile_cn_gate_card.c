/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_card.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 刷卡充电功能
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-08
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate_card";




extern stPileCnCache_t *pPileCnCache;






void sPileCnGateCardFind   (ePileGunIndex_t eGunIndex);
void sPileCnGateCardAuth   (ePileGunIndex_t eGunIndex);
void sPileCnGateCardAuthAck(ePileGunIndex_t eGunIndex);


void sPileCnGateCardStop   (ePileGunIndex_t eGunIndex);
void sPileCnGateCardStart  (ePileGunIndex_t eGunIndex);



void sPileCnGateCardMakeUserId(u32 u32CardMefareSn, i32 i32MaxLen, u8 *pBuf);
















/***************************************************************************************************
* Description                           :   寻卡控制
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   只要有枪在充电 就要持续寻卡
****************************************************************************************************/
void sPileCnGateCardFind(ePileGunIndex_t eGunIndex)
{
    i32 i;
    if(eGunIndex != ePileGunIndex0)
    {
        //只需要第一把枪的线程执行就可以了
        return;
    }
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(((sPileCnStateGet(i) == ePrivDrvGunWorkStatusCharging) || (sPileCnStateGet(i) == ePrivDrvGunWorkStatusSuspendedByEv)) && (sPileCnGet_Authorize(i) == eAuthorizeTypeCard))
        {
            sCardAppStartForStop();
            break;
        }
    }
}











/***************************************************************************************************
* Description                           :   刷卡操作
* Author                                :   Hall
* Creat Date                            :   2024-06-08
* notice                                :   
****************************************************************************************************/
void sPileCnGateCardAuth(ePileGunIndex_t eGunIndex)
{
    i32 i;
    stCardData_t *pCard = NULL;
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    
    if(eGunIndex != ePileGunIndex0)
    {
        //只需要第一把枪的线程执行就可以了
        return;
    }
    
    pCard = sCardAppGetData();
    
    if(pCard != NULL)
    {
        for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
        {
            memset(&pData->stCard[i], 0,     sizeof(stCardData_t));
            memcpy(&pData->stCard[i], pCard, sizeof(stCardData_t));
            
            if((sPileCnStateGet(i) == ePrivDrvGunWorkStatusPreparing) && (sPileCnGet_Authorize(i) == eAuthorizeTypeNone) && (pCard->eGunIndex == i))
            {
                sPileCnGateCardStart(i);
            }
            else
            {
                sPileCnGateCardStop(i);
            }
            
        }
        
        //用完以后清除卡数据
        sCardAppSetDataClear();
    }
}








/***************************************************************************************************
* Description                           :   卡认证应答处理
* Author                                :   Hall
* Creat Date                            :   2024-06-11
* notice                                :   
****************************************************************************************************/
void sPileCnGateCardAuthAck(ePileGunIndex_t eGunIndex)
{
    u32  u32Meter;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    stPrivDrvCmdA5_t *pCardAuthAck = NULL;
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    if(sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeVin)
    {
        //非VIN码充电情况下才执行后续流程
        //避免把VIN码充电的A5报文信号量破坏了
        return;
    }
    
    if(sPrivDrvGetCardAuthAck(u8GunId, &pCardAuthAck) == true)
    {
        if((pCardAuthAck->eFailReason == ePrivDrvCardAuthFailRsn00)
        && (pCardAuthAck->eAccountStatus == ePrivDrvCardAccountStatusValid))
        {
            //获取电表读数
            sMeterDrvOptGetEnergy(eGunIndex, &u32Meter, NULL);
            
            //发送07报文
            memset(&stTrans, 0, sizeof(stTrans));
            stTrans.u8GunId = u8GunId;
            stTrans.eCmd = ePrivDrvStartTransCmdStart;
            stTrans.eType = ePrivDrvStartTransTypeCard;
            stTrans.stTime = sGetTime();
            stTrans.u32Meter = u32Meter;
            sPrivDrvSetTrans(u8GunId, &stTrans);
            
            if((sPrivDrvGetTransAck(u8GunId, &pTransAck) == true)
            && (pTransAck->eRst == ePrivDrvCmdRstSuccess))
            {
                //启动充电
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeCard);
                sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStartByCard);
                
                //初始化并创建订单
                sPileCnGateNewRcdInitForCard(eGunIndex, pCardAuthAck, pTransAck);
                sEepromAppGetBlockOperatorRate(&pData->unRate);
                
                //启动计量
                sOrderStart(eGunIndex, sEepromAppNewBlockChgRcd(pRcd), pRcd, &pData->unRate, sPileCnGateGetSoc);
                
            }
        }
    }
}






/***************************************************************************************************
* Description                           :   刷卡停止充电
* Author                                :   Hall
* Creat Date                            :   2024-06-10
* notice                                :   
****************************************************************************************************/
void sPileCnGateCardStop(ePileGunIndex_t eGunIndex)
{
    u32  u32Value, u32Meter;
    i32  i32RcdIndex;
    u8   u8UserId[cPrivDrvUserIdLen];
    
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    stCardData_t *pCard = &pData->stCard[eGunIndex];
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    //序列号转换为字符串格式
    sPileCnGateCardMakeUserId(pCard->u32Mefare1Sn, sizeof(u8UserId), u8UserId);
    
    //比对卡号一致则执行停充流程
    if(memcmp(pRcd->stRcdData.u8UserId, u8UserId, sizeof(u8UserId)) == 0)
    {
        //获取电表读数
        sMeterDrvOptGetEnergy(eGunIndex, &u32Meter, NULL);
        
        //发送07报文
        memset(&stTrans, 0, sizeof(stTrans));
        stTrans.u8GunId = u8GunId;
        stTrans.eCmd = ePrivDrvStartTransCmdStop;
        stTrans.eType = ePrivDrvStartTransTypeCard;
        stTrans.stTime = sGetTime();
        stTrans.u32Meter = u32Meter;
        memcpy(stTrans.u8OrderId,    pRcd->stRcdData.u8OrderId,    sizeof(pRcd->stRcdData.u8OrderId   ));
        memcpy(stTrans.u8SrvTradeId, pRcd->stRcdData.u8SrvTradeId, sizeof(pRcd->stRcdData.u8SrvTradeId));
        sPrivDrvSetTrans(u8GunId, &stTrans);
        
        //收到A7答复
        if((sPrivDrvGetTransAck(u8GunId, &pTransAck) == true)
        && (pTransAck->eRst == ePrivDrvCmdRstSuccess))
        {
            //停止充电
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonCard);
            sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStopByCard);
       }
       
    }
    
}





/***************************************************************************************************
* Description                           :   刷卡启动充电
* Author                                :   Hall
* Creat Date                            :   2024-06-10
* notice                                :   
****************************************************************************************************/
void sPileCnGateCardStart(ePileGunIndex_t eGunIndex)
{
    u8   i;
    u8   u8UserId[cPrivDrvUserIdLen] = {0};
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    
    stPrivDrvCmd05_t stCardAuth;
    stCardData_t *pCard = &pData->stCard[eGunIndex];
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    stEepromAppBlockChgRcd_t *pRcdCheck;
    
    //与正在充电订单中卡号不一致才能启动充电
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        pRcdCheck = &pData->stNewRcd[i];
        
        memset(u8UserId, 0, sizeof(u8UserId));
        sPileCnGateCardMakeUserId(pCard->u32Mefare1Sn, sizeof(u8UserId), u8UserId);
        if(memcmp(u8UserId, pRcdCheck->stRcdData.u8UserId, sizeof(u8UserId)) == 0)
        {
            return;
        }
    }
    
    memset(&stCardAuth, 0, sizeof(stCardAuth));
    stCardAuth.u8GunId = u8GunId;
    stCardAuth.eType = pCard->eType;
    stCardAuth.eStatus = ePrivDrvCardLockStatusUnlock;
    stCardAuth.u32CardBalance = pCard->u32Balance;
    stCardAuth.stTime = pCard->stTime;
    stCardAuth.eMode = pCard->eMode;
    stCardAuth.unParam.u32Value = pCard->unParam.u32Value;
    sPileCnGateCardMakeUserId(pCard->u32Mefare1Sn, sizeof(stCardAuth.u8UserId), stCardAuth.u8UserId);
    sPrivDrvSetCardAuth(u8GunId, &stCardAuth);
    
}







/***************************************************************************************************
* Description                           :   从 Mefare1 卡序列号 生成 user ID
* Author                                :   Hall
* Creat Date                            :   2024-06-14
* notice                                :   
****************************************************************************************************/
void sPileCnGateCardMakeUserId(u32 u32CardMefareSn, i32 i32MaxLen, u8 *pBuf)
{
    memset(pBuf, 0, i32MaxLen);
    snprintf((char *)pBuf, i32MaxLen, "%08x", u32CardMefareSn);
}




























