/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_11b1.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-06-19
* notice                                :     本文件负责实现内网协议中 11/B1 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_11b1.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
//static const char *TAG = "private_drv_11b1";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 B3报文
* Author            :     XRG
* modified Date     :     2024-06-19
* notice            :     供 Master 端使用
*                         B1报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendB1(stPrivDrvCmdB1_t *pEvtNoticeAck)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB1_t    *pCmdB1 = NULL;
    stPrivDrvDataMaster_t     *pData  = NULL;
    
    
    //0:枪号校验
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB1  = &pPkt->stPkt.unPayload.stCmdB1;
    pData   = &pPrivDrvCache->unData.stMaster;
    u16Len  = sizeof(stPrivDrvCmdB1_t);
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    memcpy(pCmdB1->u8Rsv, pEvtNoticeAck, sizeof(stPrivDrvCmdB1_t));
    
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdB1, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd11, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 11报文
* Author            :     XRG
* modified Date     :     22024-06-19
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv11(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd11_t    *pCmd11 = NULL;
    stPrivDrvDataMaster_t     *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd11  = &pPkt->stPkt.unPayload.stCmd11;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    memcpy(&pData->stChg.stCache11, pCmd11, sizeof(stPrivDrvCmd11_t));
    if(pData->stChg.hCache11Mutex != NULL)
    {
        xSemaphoreGive(pData->stChg.hCache11Mutex);
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 11报文
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     供 Slave 端使用
*                         11报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend11(stPrivDrvCmd11_t *pEvtNotice)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd11_t    *pCmd11 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd11  = &pPkt->stPkt.unPayload.stCmd11;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd11_t);
    memcpy(pCmd11, pEvtNotice, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd11, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdB1, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 B1报文
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvB1(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    //stPrivDrvCmdB1_t    *pCmdB1 = NULL;
    //stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    //pCmdB1  = &pPkt->stPkt.unPayload.stCmdB1;
    //pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    return(true);
}










#endif





























