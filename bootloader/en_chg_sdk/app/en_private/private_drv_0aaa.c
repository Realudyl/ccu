/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0aaa.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-01
* notice                                :     本文件负责实现内网协议中 0A/AA 报文 
*
*                                             3.6版内网协议规定允许 slave 端主动发送0A报文
*                                             网关端根据需求下发AA报文，桩端回复0A报文
*                                             当桩端有需求时，也可以主动上报0A报文，网关需要回复AA报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_0aaa.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0aaa";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 AA报文
* Author            :     Hall
* modified Date     :     2023-11-01
* notice            :     供 Master 端使用
*                         bRptFlag:主动发送标志，true 表示本次是主动发送，false表示本次发送是被动应答
***********************************************************************************************/
bool sPrivDrvPktSendAA(u8 u8GunId, bool bRptFlag, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam)
{
    bool bRst;
    
    u16  u16Len;
    u8   u8Seqno;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdAA_t    *pCmdAA = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    SemaphoreHandle_t   hMutex  = NULL;
    
    //0:枪号校验(0---整桩, 1,2...是GunId)
    if(u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "桩参数管理:枪号%d错误!!!", u8GunId);
        return(false);
    }
    
    
    //1:访问保护---
    hMutex  = (bRptFlag == true) ? pPrivDrvCache->hRptTxBufMutex :pPrivDrvCache->hAckTxBufMutex;
    xSemaphoreTake(hMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)((bRptFlag == true) ? pPrivDrvCache->u8RptTxBuf : pPrivDrvCache->u8AckTxBuf);
    pHead   = &pPkt->stPkt.stHead;
    pCmdAA  = &pPkt->stPkt.unPayload.stCmdAA;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    pCmdAA->u8GunId = u8GunId;
    pCmdAA->eType   = eType;
    pCmdAA->eRst    = eRst;
    pCmdAA->eAddr   = eAddr;
    pCmdAA->u16Len  = u16ParamLen;
    if((u16ParamLen > 0) && (pParam != NULL))
    {
        memcpy(pCmdAA->u8Data, pParam, u16ParamLen);
    }
    u16Len = sizeof(stPrivDrvCmdAA_t) + u16ParamLen - sizeof(pCmdAA->u8Data);
    
    //2.2:head
    u8Seqno =  (bRptFlag == true) ? pData->stChg.u8RptSeqno :pData->stChg.u8AckSeqno;
    sPrivDrvSetHead(pHead, ePrivDrvCmdAA, pData->stChg.u16Addr, u8Seqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd0A, (u8 *)pPkt, u16Len, bRptFlag);
    
    //4:访问保护解除
    xSemaphoreGive(hMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 0A报文
* Author            :     Hall
* modified Date     :     2023-11-01
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv0A(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0A_t    *pCmd0A = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0A  = &pPkt->stPkt.unPayload.stCmd0A;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    bRst = false;
    //1:枪号校验(0---整桩, 1,2...是GunId)
    if(pCmd0A->u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "桩参数管理应答:枪号%d错误!!!", pCmd0A->u8GunId);
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        if((pCmd0A->eType == ePrivDrvCfgCmdTypeChargGet) || (pCmd0A->eType == ePrivDrvCfgCmdTypeChargSet))
        {
            memcpy(&pData->stChg.stCache0ARpt, pCmd0A, sizeof(stPrivDrvCmd0A_t));
            if(pData->stChg.hCache0ARptMutex != NULL)
            {
                xSemaphoreGive(pData->stChg.hCache0ARptMutex);
            }
            
            //桩端主动上报0A时更新
            pData->stChg.u16Addr    = pHead->u16Addr;
            pData->stChg.u8AckSeqno = pHead->u8Seqno;
            
            return(true);
        }
        memcpy(&pData->stChg.stCache0AAck, pCmd0A, sizeof(stPrivDrvCmd0A_t));
        bRst = (pCmd0A->eRst == ePrivDrvCmdRstSuccess) ? true : false;
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 0A报文
* Author            :     Hall
* modified Date     :     2023-11-01
* notice            :     供 Slave 端使用
*                         bRptFlag:主动发送标志，true 表示本次是主动发送，false表示本次发送是被动应答
***********************************************************************************************/
bool sPrivDrvPktSend0A(bool bRptFlag, u8 u8GunId, ePrivDrvCfgCmdType_t eType, ePrivDrvCmdRst_t eRst, ePrivDrvParamAddr_t eAddr, u16 u16ParamLen, const u8 *pParam)
{
    bool bRst;
    
    u16  u16Len;
    u8   u8Seqno;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0A_t    *pCmd0A = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    SemaphoreHandle_t   hMutex  = NULL;
    
    //0:枪号校验
    if(u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "桩参数管理:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---
    hMutex  = (bRptFlag == true) ? pPrivDrvCache->hRptTxBufMutex :pPrivDrvCache->hAckTxBufMutex;
    xSemaphoreTake(hMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)((bRptFlag == true) ? pPrivDrvCache->u8RptTxBuf : pPrivDrvCache->u8AckTxBuf);
    pHead   = &pPkt->stPkt.stHead;
    pCmd0A  = &pPkt->stPkt.unPayload.stCmd0A;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    pCmd0A->u8GunId = u8GunId;
    pCmd0A->eType   = eType;
    pCmd0A->eRst    = eRst;
    pCmd0A->eAddr   = eAddr;
    pCmd0A->u16Len  = u16ParamLen;
    if((u16ParamLen > 0) && (pParam != NULL))
    {
        memcpy(pCmd0A->u8Data, pParam, u16ParamLen);
    }
    u16Len = sizeof(stPrivDrvCmdAA_t) + u16ParamLen - sizeof(pCmd0A->u8Data);
    
    //2.2:head
    u8Seqno =  (bRptFlag == true) ? pData->stChg.u8RptSeqno :pData->stChg.u8AckSeqno;
    sPrivDrvSetHead(pHead, ePrivDrvCmd0A, pData->stChg.u16Addr, u8Seqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdAA, (u8 *)pPkt, u16Len, bRptFlag);
    
    //4:访问保护解除
    xSemaphoreGive(hMutex);
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 AA报文
* Author            :     Hall
* modified Date     :     2023-11-01
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvAA(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdAA_t    *pCmdAA = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdAA  = &pPkt->stPkt.unPayload.stCmdAA;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if(pCmdAA->u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "桩参数管理:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        memcpy(&pData->stChg.stCacheAARpt, pCmdAA, sizeof(stPrivDrvCmdAA_t));
        if((pCmdAA->eType == ePrivDrvCfgCmdTypeSet) && (pCmdAA->eAddr == ePrivDrvParamAddr018) && (pCmdAA->u8Data[0] == ePrivDrvParamGateOpReLogin))
        {
            pPrivDrvCache->u32Time02 = 0;
            pData->stChg.bCacheA2Flag = false;
        }
        if(pData->stChg.hCacheAARptMutex != NULL)
        {
            xSemaphoreGive(pData->stChg.hCacheAARptMutex);
        }
        
        //作为Ack一方时要更新
        pData->stChg.u16Addr    = pHead->u16Addr;
        pData->stChg.u8AckSeqno = pHead->u8Seqno;
        
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    
    return(true);
}






#endif




























