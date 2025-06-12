/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_07a7.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 07/A7 报文 
*
*                                             07、A7报文可能会涉及到网络端鉴权交互，报文需要考虑去重
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_07a7.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_07a7";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A7报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Master 端使用
*                         A7报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA7(u8 u8GunId, stPrivDrvCmdA7_t *pTransAck)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA7_t    *pCmdA7 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "开始/结束充电应答:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA7  = &pPkt->stPkt.unPayload.stCmdA7;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdA7_t);
    memcpy(pCmdA7, pTransAck, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA7, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd07, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 07报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv07(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd07_t    *pCmd07 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd07  = &pPkt->stPkt.unPayload.stCmd07;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:检查cmd06下发启动充电字段和当前的校验
    bRst = false;
    if((pCmd07->u8GunId < cPrivDrvGunIdBase) || (pCmd07->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "开始/结束充电请求:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        //报文的序列号与上一次的值不一样 才处理数据并对外发出信号
        u8Index = pCmd07->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stTrans,   pCmd07,   sizeof(stPrivDrvCmd07_t));
        
        if(pData->stGun[u8Index].hCache07Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache07Mutex);
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
* Description       :     串口数据 帧发送处理函数 之 07报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Slave 端使用
*                         07报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend07(u8 u8GunId, stPrivDrvCmd07_t *pTrans)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd07_t    *pCmd07 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "卡认证请求:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd07  = &pPkt->stPkt.unPayload.stCmd07;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd07_t);
    memcpy(pCmd07, pTrans, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd07, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA7, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A7报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA7(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA7_t    *pCmdA7 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA7  = &pPkt->stPkt.unPayload.stCmdA7;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmdA7->u8GunId < cPrivDrvGunIdBase) || (pCmdA7->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "开始/结束充电应答:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmdA7->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stTransAck, pCmdA7, sizeof(stPrivDrvCmdA7_t));
        
        if(pData->stGun[u8Index].hCacheA7Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCacheA7Mutex);
        }
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}






#endif




























