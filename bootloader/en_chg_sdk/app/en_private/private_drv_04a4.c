/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_04a4.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-22
* notice                                :     本文件负责实现内网协议中 04/A4 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_04a4.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_04a4";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A4报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Master 端使用
*                         A4报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA4(void)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA4_t    *pCmdA4 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA4  = &pPkt->stPkt.unPayload.stCmdA4;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdA4_t);
    pCmdA4->eNetStatus = pData->stChg.eNetStatus;
    pCmdA4->stTime     = sGetTime();
    pCmdA4->i8Rssi     = pData->stChg.i8Rssi;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA4, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd04, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 04报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv04(const u8 *pBuf, i32 i32Len)
{
    i32  i;
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd04_t    *pCmd04 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd04  = &pPkt->stPkt.unPayload.stCmd04;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = true;
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        memcpy(&pData->stChg.stCache04, pCmd04, sizeof(stPrivDrvCmd04_t));
        pData->stChg.u8GunNum               = pCmd04->u8GunNum;
        for(i = 0; i < cPrivDrvGunNumMax; i++)
        {
            pData->stGun[i].eWorkStatus     = pCmd04->stGunStatus.eWorkStatus[i];
            pData->stGun[i].eConnStatus     = pCmd04->stGunStatus.eConnStatus[i];
            pData->stGun[i].eLockStatus     = pCmd04->stGunStatus.eLockStatus[i];
        }
        
        if(pData->stChg.hCache04Mutex != NULL)
        {
            xSemaphoreGive(pData->stChg.hCache04Mutex);
        }
    }
    else
    {
        EN_SLOGE(TAG, "枪号异常!!!");
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendA4();
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 04报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Slave 端使用
*                         04报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend04(void)
{
    bool bRst;
    
    i32  i;
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd04_t    *pCmd04 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    EN_SLOGI(TAG, "心跳上报!!!");
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd04  = &pPkt->stPkt.unPayload.stCmd04;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd04_t);
    pCmd04->u8GunNum = pData->stChg.u8GunNum;
    for(i = 0; i < cPrivDrvGunNumMax; i++)
    {
        pCmd04->stGunStatus.eWorkStatus[i] = pData->stGun[i].eWorkStatus;
        pCmd04->stGunStatus.eConnStatus[i] = pData->stGun[i].eConnStatus;
        pCmd04->stGunStatus.eLockStatus[i] = pData->stGun[i].eLockStatus;
    }
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd04, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA4, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A4报文
* Author            :     Hall
* modified Date     :     2023-10-22
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA4(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA4_t    *pCmdA4 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA4  = &pPkt->stPkt.unPayload.stCmdA4;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:数据解析及缓存
    memcpy(&pData->stChg.stCacheA4, pCmdA4, sizeof(stPrivDrvCmdA4_t));
    pData->stChg.eNetStatus   = pCmdA4->eNetStatus;
    pData->stChg.stMasterTime = pCmdA4->stTime;
    if((pData->stChg.hTimeMutex != NULL) && (pData->stChg.eNetStatus == ePrivDrvNetStatusConn))
    {
        //master已联网的情况下 才发出信号 允许外部对时
        xSemaphoreGive(pData->stChg.hTimeMutex);
    }
    pPrivDrvCache->u32TimeA4 = sGetTimestamp();
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    return(true);
}






#endif




























