/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_16b6.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-08-09
* notice                                :     本文件负责实现内网协议中 15/b5 报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_16b6.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_16b6";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 B6报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Master 端使用
*                         B6报文对 Master 来说是 Ack发送 要使用 Ack发送 资源
***********************************************************************************************/
bool sPrivDrvPktSendB6(ePrivDrvCmdRst_t eRst)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB6_t    *pCmdB6 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB6  = &pPkt->stPkt.unPayload.stCmdB6;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdB6_t);
    pCmdB6->eRst = eRst;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdB6, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd16, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 16报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv16(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd16_t    *pCmd16 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd16  = &pPkt->stPkt.unPayload.stCmd16;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd16->u8GunId < cPrivDrvGunIdBase) || (pCmd16->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "液冷机状态上报:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd16->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stCoolingData, pCmd16, sizeof(stPrivDrvCmd16_t));
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendB6(ePrivDrvCmdRstSuccess);
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 16报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Slave 端使用
*                         16报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 资源
***********************************************************************************************/
bool sPrivDrvPktSend16(u8 u8GunId, stPrivDrvCmd16_t *pCoolingData)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd16_t    *pCmd16 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "液冷机状态上报:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd16  = &pPkt->stPkt.unPayload.stCmd16;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd16_t);
    memcpy(pCmd16, pCoolingData, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd16, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdB6, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 B6报文
* Author            :     Hall
* modified Date     :     2024-08-09
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvB6(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB6_t    *pCmdB6 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB6  = &pPkt->stPkt.unPayload.stCmdB6;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    bRst = (pCmdB6->eRst == ePrivDrvCmdRstSuccess) ? true : false;
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}

















#endif




























