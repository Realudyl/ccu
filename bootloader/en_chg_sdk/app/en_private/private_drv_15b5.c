/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_15b5.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-08-09
* notice                                :     本文件负责实现内网协议中 15/b5 报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_15b5.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_15b5";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 B5报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Master 端使用
*                         B5报文对 Master 来说是 Ack发送 要使用 Ack发送 资源
***********************************************************************************************/
bool sPrivDrvPktSendB5(ePrivDrvCmdRst_t eRst)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB5_t    *pCmdB5 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if(0)
    {
        EN_SLOGE(TAG, "无效打印, 规避告警使用");
    }
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB5  = &pPkt->stPkt.unPayload.stCmdB5;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdB5_t);
    pCmdB5->eRst = eRst;
    
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdB5, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd15, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 15报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv15(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd15_t    *pCmd15 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd15  = &pPkt->stPkt.unPayload.stCmd15;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = true;
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        memcpy(&pData->stChg.stMpcData, pCmd15, sizeof(stPrivDrvCmd15_t));
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendB5(ePrivDrvCmdRstSuccess);
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 15报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Slave 端使用
*                         15报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 资源
***********************************************************************************************/
bool sPrivDrvPktSend15(stPrivDrvCmd15_t *pMpcData, u16 u16ActualDataLen)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd15_t    *pCmd15 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:参数限幅
    if(u16ActualDataLen > sizeof(stPrivDrvCmd15_t))
    {
        EN_SLOGE(TAG, "主柜信息上报, 数据长度超限");
        return false;
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd15  = &pPkt->stPkt.unPayload.stCmd15;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = u16ActualDataLen;
    memcpy(pCmd15, pMpcData, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd15, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdB5, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 B5报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvB5(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB5_t    *pCmdB5 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB5  = &pPkt->stPkt.unPayload.stCmdB5;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    bRst = (pCmdB5->eRst == ePrivDrvCmdRstSuccess) ? true : false;
    
    
    //2:数据解析及缓存
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}

















#endif




























