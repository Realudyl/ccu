/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0eae.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-06
* notice                                :     本文件负责实现内网协议中 0E/AE 报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_0eae.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0eae";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 AE报文
* Author            :     XRG
* modified Date     :     2024-01-20
* notice            :     供 Master 端使用
*                         AE报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendAE(u8 u8GunId)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdAE_t    *pCmdAE = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "桩端遥信遥测数据上报响应:枪号错误!!!");
        return(false);
    }
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdAE  = &pPkt->stPkt.unPayload.stCmdAE;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdAE_t);
    pCmdAE->u8GunId = u8GunId;
    pCmdAE->eSts    = ePrivDrvCmdAeStsSuccess;
    memcpy(pCmdAE->u8Data, pData->stGun[u8GunId - cPrivDrvGunIdBase].u8RsvdDataYcAck, sizeof(pCmdAE->u8Data));
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdAE, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd0E, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 0E报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv0E(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0E_t    *pCmd0E = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0E  = &pPkt->stPkt.unPayload.stCmd0E;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd0E->u8GunId < cPrivDrvGunIdBase) || (pCmd0E->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "桩端遥信遥测数据上报:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd0E->u8GunId - cPrivDrvGunIdBase;
        
        memcpy(&pData->stChg.stChgTemp, &pCmd0E->stChgTemp, sizeof(pCmd0E->stChgTemp));
        memcpy(&pData->stGun[u8Index].stCache0E, pCmd0E, sizeof(stPrivDrvCmd0E_t));
        
        if(pData->stGun[u8Index].hCache0EMutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache0EMutex);
        }
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    sPrivDrvPktSendAE(pCmd0E->u8GunId);
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 0E报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Slave 端使用
*                         0E报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend0E(u8 u8GunId)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0E_t    *pCmd0E = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "桩端遥信遥测数据上报:枪号错误!!!");
        return(false);
    }
    if(pPrivDrvCache->bPkt0EEnableFlag[u8GunId] == false)
    {
        //还没到发送周期
        return(false);
    }
    pPrivDrvCache->bPkt0EEnableFlag[u8GunId] = false;
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0E  = &pPkt->stPkt.unPayload.stCmd0E;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd0E_t);
    memcpy( pCmd0E,            &pData->stGun[u8GunId - cPrivDrvGunIdBase].stCache0E, u16Len                   );
    memcpy(&pCmd0E->stChgTemp, &pData->stChg.stChgTemp,                              sizeof(pCmd0E->stChgTemp));
    pCmd0E->u8GunId = u8GunId;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd0E, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdAE, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 AE报文
* Author            :     Hall
* modified Date     :     2023-11-06
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvAE(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    //stPrivDrvCmdAE_t    *pCmdAE = NULL;
    //stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    //pCmdAE  = &pPkt->stPkt.unPayload.stCmdAE;
    //pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    return(true);
}






#endif




























