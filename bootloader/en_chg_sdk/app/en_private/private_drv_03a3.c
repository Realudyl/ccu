/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_03a3.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 03/A3 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_03a3.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_03a3";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A3报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Master 端使用
*                         A3报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA3(u8 u8GunId)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA3_t    *pCmdA3 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if(u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "状态上报应答:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA3  = &pPkt->stPkt.unPayload.stCmdA3;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdA3_t);
    pCmdA3->u8GunId    = u8GunId;
    pCmdA3->eNetStatus = pData->stChg.eNetStatus;
    pCmdA3->stTime     = sGetTime();
    if(u8GunId == cPrivDrvGunIdChg)
    {
    }
    else
    {
        memcpy(pCmdA3->u8Data, pData->stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck, sizeof(pCmdA3->u8Data));
    }
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA3, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd03, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 03报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv03(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd03_t    *pCmd03 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd03  = &pPkt->stPkt.unPayload.stCmd03;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if(pCmd03->u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "状态上报:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        if(pCmd03->u8GunId == cPrivDrvGunIdChg)
        {
            memcpy(&pData->stChg.stCache03, pCmd03, sizeof(stPrivDrvCmd03_t));
            if(pData->stChg.hCache03Mutex != NULL)
            {
                xSemaphoreGive(pData->stChg.hCache03Mutex);
            }
        }
        else
        {
            //将最新的桩端时间缓存起来 供用户程序读取
            pData->stChg.stCache03.stTime = pCmd03->stTime;
            
            u8Index = pCmd03->u8GunId - cPrivDrvGunIdBase;
            pData->stGun[u8Index].eEvent          = pCmd03->eGunevent;
            pData->stGun[u8Index].eConnStatus     = pCmd03->eGunConnStatus;
            pData->stGun[u8Index].eWorkStatus     = pCmd03->eGunWorkStatus;
            pData->stGun[u8Index].eWorkStatusOld  = pCmd03->eGunWorkStatusOld;
            pData->stGun[u8Index].eStsReason      = pCmd03->eReason;
            memcpy(&pData->stGun[u8Index].stFaultCode,   &pCmd03->stFaultCode, sizeof(pCmd03->stFaultCode));
            
            if(pData->stGun[u8Index].hCache03Mutex != NULL)
            {
                xSemaphoreGive(pData->stGun[u8Index].hCache03Mutex);
            }
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendA3(pCmd03->u8GunId);
    
    //5:打印重要参数
    EN_SLOGE(TAG, " ---------- PrivDrvCmd03----------");
    EN_SLOGE(TAG, "枪连接状态:%d, 枪工作状态当前值:%d, 枪工作状态上一次值:%d", pCmd03->eGunConnStatus, pCmd03->eGunWorkStatus, pCmd03->eGunWorkStatusOld);
    ESP_LOGHW(TAG, "故障编码:", pCmd03->unFaultByte.u8Buf, cPrivDrvFaultCodeLen);
    EN_SLOGE(TAG, " ---------------------------------");
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 03报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Slave 端使用
*                         03报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend03(u8 u8GunId)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd03_t    *pCmd03 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if(u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "状态上报:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd03  = &pPkt->stPkt.unPayload.stCmd03;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd03_t);
    if(u8GunId == cPrivDrvGunIdChg)
    {
        memcpy(pCmd03, &pData->stChg.stCache03, u16Len);
    }
    else
    {
        pCmd03->eGunevent              = pData->stGun[u8GunId - cPrivDrvGunIdBase].eEvent;
        pCmd03->eGunConnStatus         = pData->stGun[u8GunId - cPrivDrvGunIdBase].eConnStatus;
        pCmd03->eGunWorkStatus         = pData->stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus;
        pCmd03->eGunWorkStatusOld      = pData->stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatusOld;
        pCmd03->eReason                = pData->stGun[u8GunId - cPrivDrvGunIdBase].eStsReason;
        memcpy(&pCmd03->unFaultByte,    &pData->stChg.stCache03.unFaultByte,                    sizeof(pCmd03->unFaultByte));
        memcpy(&pCmd03->unWarningByte,  &pData->stChg.stCache03.unWarningByte,                  sizeof(pCmd03->unWarningByte));
        memcpy(&pCmd03->stFaultCode,    &pData->stGun[u8GunId - cPrivDrvGunIdBase].stFaultCode, sizeof(pCmd03->stFaultCode));
        
        //以下字段值发送之后就清除 等待外部重新赋值
        pData->stGun[u8GunId - cPrivDrvGunIdBase].eEvent = ePrivDrvGunEventNone;
        pData->stGun[u8GunId - cPrivDrvGunIdBase].eStsReason = ePrivDrvGunWorkStsRsnNone;
    }
    pCmd03->stTime  = sGetTime();
    pCmd03->u8GunId = u8GunId;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd03, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA3, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A3报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA3(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA3_t    *pCmdA3 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA3  = &pPkt->stPkt.unPayload.stCmdA3;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if(pCmdA3->u8GunId > cPrivDrvGunNumMax)
    {
        EN_SLOGE(TAG, "状态上报应答:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        if(pCmdA3->u8GunId == cPrivDrvGunIdChg)
        {
            memcpy(&pData->stChg.stCacheA3, pCmdA3, sizeof(stPrivDrvCmdA3_t));
        }
        else
        {
            memcpy(&pData->stGun[pCmdA3->u8GunId - cPrivDrvGunIdBase].u8RsvdDataStsAck, pCmdA3->u8Data, sizeof(pCmdA3->u8Data));
        }
        
        pData->stChg.eNetStatus   = pCmdA3->eNetStatus;
        pData->stChg.stMasterTime = pCmdA3->stTime;
        if((pData->stChg.hTimeMutex != NULL) && (pData->stChg.eNetStatus == ePrivDrvNetStatusConn))
        {
            //master已联网的情况下 才发出信号 允许外部对时
            xSemaphoreGive(pData->stChg.hTimeMutex);
        }
        
        pPrivDrvCache->bCmdA3Flag = true;
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}






#endif




























