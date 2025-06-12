/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_06a6.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-30
* notice                                :     本文件负责实现内网协议中 06/A6 报文 
*
*                                             06、A6报文，需要去重 避免重复执行启动充电流程
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_06a6.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_06a6";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A6报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Master 端使用
*                         A6报文对 Master 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA6(u8 u8GunId, stPrivDrvCmdA6_t *pRmt)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA6_t    *pCmdA6 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "远程操作:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA6  = &pPkt->stPkt.unPayload.stCmdA6;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdA6_t);
    memcpy(pCmdA6, pRmt, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA6, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd06, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 06报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv06(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd06_t    *pCmd06 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd06  = &pPkt->stPkt.unPayload.stCmd06;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd06->u8GunId < cPrivDrvGunIdBase) || (pCmd06->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "远程操作应答:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd06->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stRmtAck, pCmd06, sizeof(stPrivDrvCmd06_t));
        
        if(pData->stGun[u8Index].hCache06Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache06Mutex);
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 06报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Slave 端使用
*                         06报文对 Slave 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend06(u8 u8GunId, stPrivDrvCmd06_t *pRmtAck)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd06_t    *pCmd06 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "远程操作应答:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd06  = &pPkt->stPkt.unPayload.stCmd06;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd06_t);
    memcpy(pCmd06, pRmtAck, u16Len);
    
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd06, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA6, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A6报文
* Author            :     Hall
* modified Date     :     2023-10-30
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA6(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA6_t    *pCmdA6 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA6  = &pPkt->stPkt.unPayload.stCmdA6;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmdA6->u8GunId < cPrivDrvGunIdBase) || (pCmdA6->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "远程操作:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmdA6->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stRmt, pCmdA6, sizeof(stPrivDrvCmdA6_t));
        
        if(pData->stGun[u8Index].hCacheA6Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCacheA6Mutex);
        }
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    return(true);
}






#endif




























