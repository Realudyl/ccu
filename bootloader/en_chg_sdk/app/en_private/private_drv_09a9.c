/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_09a9.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-31
* notice                                :     本文件负责实现内网协议中 09/A9 报文 
*
*                                             09、A9报文，需要去重 避免重复执行上报流程
*                                             
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_09a9.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_09a9";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A9报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Master 端使用
*                         A9报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA9(u8 u8GunId, ePrivDrvCmdRst_t eRst, i32 i32OrderIdLen, const u8 *pOrderId, i32 i32SrvTradeIdLen, const u8 *pSrvTradeId)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA9_t    *pCmdA9 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "结算报文上报应答:枪号%d错误!!!", u8GunId);
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA9  = &pPkt->stPkt.unPayload.stCmdA9;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len                      = sizeof(stPrivDrvCmdA9_t);
    pCmdA9->u8GunId             = u8GunId;
    pCmdA9->eRst                = eRst;
    memcpy(pCmdA9->u8OrderId,     pOrderId,    i32OrderIdLen);
    memcpy(pCmdA9->u8SrvTradeId,  pSrvTradeId, i32SrvTradeIdLen);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA9, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd09, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 09报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv09(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd09_t    *pCmd09 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd09  = &pPkt->stPkt.unPayload.stCmd09;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd09->u8GunId < cPrivDrvGunIdBase) || (pCmd09->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "结算报文上报:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd09->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stRcdData, pCmd09, sizeof(stPrivDrvCmd09_t));
        
        if(pData->stGun[u8Index].hCache09Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache09Mutex);
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    return(true);
}








#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 09报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Slave 端使用
*                         09报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend09(stPrivDrvCmd09_t *pRcdData)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd09_t    *pCmd09 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((pRcdData == NULL) || (pRcdData->u8GunId < cPrivDrvGunIdBase) || (pRcdData->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "结算报文上报:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd09  = &pPkt->stPkt.unPayload.stCmd09;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    memcpy(pCmd09, pRcdData, sizeof(stPrivDrvCmd09_t));
    
    //此处计算 payload 的实际长度, 电量明细里尚未使用的分段不传输
    u16Len = sizeof(stPrivDrvCmd09_t) - sizeof(pCmd09->unMoney) + pCmd09->u16MoneyDataLen;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd09, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA9, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A9报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA9(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA9_t    *pCmdA9 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA9  = &pPkt->stPkt.unPayload.stCmdA9;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmdA9->u8GunId < cPrivDrvGunIdBase) || (pCmdA9->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "结算报文上报应答:枪号错误!!!");
    }
    else if(pCmdA9->eRst == ePrivDrvCmdRstSuccess)
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    u8Index = pCmdA9->u8GunId - cPrivDrvGunIdBase;
    if(bRst == true)
    {
        memcpy(&pData->stGun[u8Index].stRcdDataAck, pCmdA9, sizeof(stPrivDrvCmdA9_t));
        if(pData->stGun[u8Index].hCacheA9Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCacheA9Mutex);
        }
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}







#endif




























