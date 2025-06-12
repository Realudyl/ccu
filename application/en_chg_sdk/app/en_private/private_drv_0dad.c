/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0dad.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-06
* notice                                :     本文件负责实现内网协议中 0D/AD 报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_0dad.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0dad";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 AD报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Master 端使用
*                         AD报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendAD(void)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    //stPrivDrvCmdAD_t    *pCmdAD = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:校验
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    //pCmdAD  = &pPkt->stPkt.unPayload.stCmdAD;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdAD_t);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdAD, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd0D, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 0D报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv0D(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0D_t    *pCmd0D = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0D  = &pPkt->stPkt.unPayload.stCmd0D;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    
    
    //2:数据解析及缓存
    EN_SLOGI(TAG, "桩端日志打印:%s", pCmd0D->u8Data);
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 0D报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Slave 端使用
*                         0D报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend0D(void)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0D_t    *pCmd0D = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    EN_SLOGI(TAG, "桩端打印日志上报!!!");
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0D  = &pPkt->stPkt.unPayload.stCmd0D;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd03_t);
    memcpy(pCmd0D, &pData->stChg.stCache0D, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd0D, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdAD, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 AD报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvAD(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    //stPrivDrvCmdAD_t    *pCmdAD = NULL;
    //stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    //pCmdAD  = &pPkt->stPkt.unPayload.stCmdAD;
    //pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    return(true);
}






#endif




























