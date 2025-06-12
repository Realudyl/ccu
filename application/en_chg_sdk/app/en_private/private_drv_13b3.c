/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_13b3.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-05-20
* notice                                :     本文件负责实现内网协议中 13/B3 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_13b3.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_13b3";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 B3报文
* Author            :     XRG
* modified Date     :     2024-05-20
* notice            :     供 Master 端使用
*                         B3报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendB3(u8 u8GunId, u16 u16Seq)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB3_t    *pCmdB3 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB3  = &pPkt->stPkt.unPayload.stCmdB3;
    pData   = &pPrivDrvCache->unData.stMaster;
    u16Len  = sizeof(stPrivDrvCmdB3_t);
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    pCmdB3->u16Seq = u16Seq;
    pCmdB3->u8GunId = u8GunId;
    memcpy(pCmdB3->u8Sn, pData->stChg.stCache02.u8Sn, sizeof(pCmdB3->u8Sn));
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdB3, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd13, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 13报文
* Author            :     XRG
* modified Date     :     2024-05-20
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv13(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd13_t    *pCmd13 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd13  = &pPkt->stPkt.unPayload.stCmd13;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    //1:枪号校验
    bRst = false;
    if((pCmd13->u8GunId < cPrivDrvGunIdBase) || (pCmd13->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "枪号错误!!!");
    }
    else if(memcmp(pData->stChg.stCache02.u8Sn, pCmd13->u8Sn, sizeof(pCmd13->u8Sn)) != 0)
    {
        EN_SLOGE(TAG, "SN错误!!!");
        return(false);
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd13->u8GunId - cPrivDrvGunIdBase;
        pData->stGun[u8Index].eWorkStatus = pCmd13->eWorkStatus;
        pData->stGun[u8Index].eConnStatus = pCmd13->eConnStatus;
        memcpy(&pData->stGun[u8Index].stBmsData, pCmd13, sizeof(stPrivDrvCmd13_t));
        pData->stGun[u8Index].bBmsDataUpdata = true;
        if(pData->stGun[u8Index].hCache13Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache13Mutex);
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendB3(pCmd13->u8GunId, pCmd13->u16Seq);
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 13报文
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     供 Slave 端使用
*                         13报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend13(u8 u8GunId, stPrivDrvCmd13_t *pBmsData)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd13_t    *pCmd13 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "桩端BMS数据上报:枪号错误!!!");
        return(false);
    }
    if(pPrivDrvCache->bPkt13EnableFlag[u8GunId] == false)
    {
        //还没到发送周期
        return(false);
    }
    pPrivDrvCache->bPkt13EnableFlag[u8GunId] = false;
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd13  = &pPkt->stPkt.unPayload.stCmd13;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd13_t);
    memcpy(pCmd13, pBmsData, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd13, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdB3, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 B3报文
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvB3(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    //stPrivDrvCmdB3_t    *pCmdB3 = NULL;
    //stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    //pCmdB3  = &pPkt->stPkt.unPayload.stCmdB3;
    //pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    return(true);
}










#endif




























