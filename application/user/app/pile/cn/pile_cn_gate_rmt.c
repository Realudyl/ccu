/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_rmt.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 远程控制命令处理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-29
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate_rmt";




extern stPileCnCache_t *pPileCnCache;







void sPileCnGateRmtStart(ePileGunIndex_t eGunIndex);
void sPileCnGateRmtStop (ePileGunIndex_t eGunIndex);


void sPileCnGateRmtUnlock(ePileGunIndex_t eGunIndex);




















/***************************************************************************************************
* Description                           :   远程启动充电处理
* Author                                :   Hall
* Creat Date                            :   2024-05-29
* notice                                :   
****************************************************************************************************/
void sPileCnGateRmtStart(ePileGunIndex_t eGunIndex)
{
    u32  u32Meter;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    stPrivDrvCmd06_t stRmtAck;
    stPrivDrvCmdA6_t *pRmt      =  pData->pRmt[eGunIndex];
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    
    memset(&stRmtAck, 0, sizeof(stRmtAck));
    stRmtAck.u8GunId = u8GunId;
    stRmtAck.eCmd = ePrivDrvRmtCmdStart;
    memcpy(stRmtAck.u8OrderId,    pRmt->u8OrderId,    sizeof(pRmt->u8OrderId   ));
    memcpy(stRmtAck.u8SrvTradeId, pRmt->u8SrvTradeId, sizeof(pRmt->u8SrvTradeId));
    if((sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusPreparing) && (sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeNone))
    {
        //回复06报文
        stRmtAck.eRst = ePrivDrvRmtRstSuccess;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
        sPrivDrvSetRmtAck(u8GunId, &stRmtAck);
        
        //获取电表读数
        sMeterDrvOptGetEnergy(eGunIndex, &u32Meter, NULL);

        //本地管理员启动的话不需要启动计量也不需要鉴权否则会导致网关与平台的订单之间出现混乱
        if(pRmt->eType == ePrivDrvStartTransTypeAdmin)
        {
            //本地管理员充电不需要生成订单号
            sPrivDrvGetTransAck(u8GunId, &pTransAck);
            memset(pTransAck, 0, sizeof(stPrivDrvCmdA7_t));

            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypePsw);
            sPileCnGateNewRcdInitForRmt(eGunIndex, pRmt, pTransAck);
        }
        else
        {
            //先获取一次A7信号量防止残余信号量导致获取到错误数据
            sPrivDrvGetTransAck(u8GunId, &pTransAck);

            //发送07报文
            memset(&stTrans, 0, sizeof(stTrans));
            stTrans.u8GunId = u8GunId;
            stTrans.eCmd = ePrivDrvStartTransCmdStart;
            stTrans.eType = ePrivDrvStartTransTypeApp;
            stTrans.stTime = sGetTime();
            stTrans.u32Meter = u32Meter;
            memcpy(stTrans.u8OrderId,    pRmt->u8OrderId,    sizeof(pRmt->u8OrderId   ));
            memcpy(stTrans.u8SrvTradeId, pRmt->u8SrvTradeId, sizeof(pRmt->u8SrvTradeId));
            sPrivDrvSetTrans(u8GunId, &stTrans);
            
            if((sPrivDrvGetTransAck(u8GunId, &pTransAck) == true)
            && (pTransAck->eRst == ePrivDrvCmdRstSuccess))
            {
                EN_SLOGE(TAG, "获取A7报文成功");
                //启动充电
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeApp);
                sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStartBySrv);
                
                //初始化并创建订单
                sPileCnGateNewRcdInitForRmt(eGunIndex, pRmt, pTransAck);
                sEepromAppGetBlockOperatorRate(&pData->unRate);
                
                //启动计量
                sOrderStart(eGunIndex, sEepromAppNewBlockChgRcd(pRcd), pRcd, &pData->unRate, sPileCnGateGetSoc);
                
            }
        }       
    }
    else
    {
        EN_SLOGD(TAG, "枪%d远程启动失败,枪状态:%d,鉴权状态:%d", eGunIndex, sPileCnStateGet(eGunIndex), pPileCnCache->stChgInfo[eGunIndex].eAuthorize);
        
        //回复06报文
        stRmtAck.eRst = ePrivDrvRmtRstFailCharg;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
        
        sPrivDrvSetRmtAck(u8GunId, &stRmtAck);
    }
}








void sPileCnGateRmtStop(ePileGunIndex_t eGunIndex)
{
    u32  u32Value, u32Meter;
    i32  i32RcdIndex;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stPileCnGateData_t *pData   = &pPileCnCache->stGateData;
    stPrivDrvCmd06_t stRmtAck;
    stPrivDrvCmdA6_t *pRmt      =  pData->pRmt[eGunIndex];
    
    stPrivDrvCmd07_t stTrans;
    stPrivDrvCmdA7_t *pTransAck =  NULL;
    
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    memset(&stRmtAck, 0, sizeof(stRmtAck));
    stRmtAck.u8GunId = u8GunId;
    stRmtAck.eCmd = ePrivDrvRmtCmdStop;
    memcpy(stRmtAck.u8OrderId,    pRmt->u8OrderId,    sizeof(pRmt->u8OrderId   ));
    memcpy(stRmtAck.u8SrvTradeId, pRmt->u8SrvTradeId, sizeof(pRmt->u8SrvTradeId));
    if((sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusCharging)
    && (memcmp(pRcd->stRcdData.u8OrderId,    pRmt->u8OrderId,    sizeof(pRmt->u8OrderId   )) == 0)
    && (memcmp(pRcd->stRcdData.u8SrvTradeId, pRmt->u8SrvTradeId, sizeof(pRmt->u8SrvTradeId)) == 0))
    {
        //回复06报文
        stRmtAck.eRst = ePrivDrvRmtRstSuccess;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
        sPrivDrvSetRmtAck(u8GunId, &stRmtAck);

        //本地管理员启动的话不需要结束计量也不需要鉴权否则会导致网关与平台的订单之间出现混乱
        if(pRmt->eType == ePrivDrvStartTransTypeAdmin)
        {
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
        }
        else
        {
            //获取电表读数
            sMeterDrvOptGetEnergy(eGunIndex, &u32Meter, NULL);
        
            //发送07报文
            memset(&stTrans, 0, sizeof(stTrans));
            stTrans.u8GunId = u8GunId;
            stTrans.eCmd = ePrivDrvStartTransCmdStop;
            stTrans.eType = ePrivDrvStartTransTypeApp;
            stTrans.stTime = sGetTime();
            stTrans.u32Meter = u32Meter;
            memcpy(stTrans.u8OrderId,    pRmt->u8OrderId,    sizeof(pRmt->u8OrderId   ));
            memcpy(stTrans.u8SrvTradeId, pRmt->u8SrvTradeId, sizeof(pRmt->u8SrvTradeId));
            sPrivDrvSetTrans(u8GunId, &stTrans);

            //获取A7信号量防止残余信号量导致获取到错误数据
            sPrivDrvGetTransAck(u8GunId, &pTransAck);
            
            //停止充电
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
            sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonApp);
            sPileCnGateMakeGunStsReason(eGunIndex, ePrivDrvGunWorkStsRsnStopBySrv);        
        }        
    }
    else
    {
        //回复06报文
        stRmtAck.eRst = ePrivDrvRmtRstFailCharg;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
        sPrivDrvSetRmtAck(u8GunId, &stRmtAck);
        
        //订单号不一致告警
        pData->bccuPoStopWarn[eGunIndex] = true;
    }
}






/***************************************************************************************************
* Description                           :   远程解锁处理
* Author                                :   Dai
* Creat Date                            :   2024-08-30
* notice                                :   
****************************************************************************************************/
void sPileCnGateRmtUnlock(ePileGunIndex_t eGunIndex)
{
    stPrivDrvCmd06_t stRmtAck;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId


    memset(&stRmtAck, 0, sizeof(stRmtAck));
    stRmtAck.u8GunId = u8GunId;
    stRmtAck.eCmd = ePrivDrvRmtCmdUnlock;
    
    //执行解锁
    sPileCnSetElockState(eGunIndex, eELockCtrlOff);
    
    //确认当前电子锁状态
    if(sIoDrvGetInput(sIoGetLockFb(eGunIndex)) == SET)
    {
        stRmtAck.eRst = ePrivDrvRmtRstSuccess;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
    }
    else
    {
        stRmtAck.eRst = ePrivDrvRmtRstFailUnlockStop;
        stRmtAck.eReason = ePrivDrvRmtFailReasonNull;
    }
    
    //回复06报文
    sPrivDrvSetRmtAck(u8GunId, &stRmtAck);
}

