/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_14b4.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-01-18
* notice                                :     本文件负责实现内网协议中 14/B4 报文 
*
*                                             预约/蓝牙定时启动充电流程
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_14b4.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_14b4";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 B4报文
* Author            :     XRG
* modified Date     :     2024-01-18
* notice            :     供 Master 端使用
*                         B4报文对 Master 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendB4(u8 u8GunId, stPrivDrvCmdB4_t *pBookingData)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB4_t    *pCmdB4 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "预约/定时充电:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB4  = &pPkt->stPkt.unPayload.stCmdB4;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdB4_t);
    memcpy(pCmdB4, pBookingData, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdB4, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd14, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 14报文
* Author            :     XRG
* modified Date     :     2024-01-18
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv14(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd14_t    *pCmd14 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd14  = &pPkt->stPkt.unPayload.stCmd14;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd14->u8GunId < cPrivDrvGunIdBase) || (pCmd14->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "预约/定时:枪号错误!!!");
    }
    else if(pCmd14->eType != pData->stGun[pCmd14->u8GunId - cPrivDrvGunIdBase].stBookingData.eType)
    {
        EN_SLOGE(TAG, "预约/定时:操作指令不符!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd14->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stBookingDataAck, pCmd14, sizeof(stPrivDrvCmd14_t));
    
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    
    return(true);
}







#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 14报文
* Author            :     Hall
* modified Date     :     2024-08-08
* notice            :     供 Slave 端使用
*                         14报文对 Slave 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend14(u8 u8GunId, stPrivDrvCmd14_t *pCmd14Data)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd14_t    *pCmd14 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "预约/定时充电应答:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd14  = &pPkt->stPkt.unPayload.stCmd14;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd14_t);
    memcpy(pCmd14, pCmd14Data, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd14, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdB4, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 B4报文
* Author            :     Hall
* modified Date     :     2024-08-08
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvB4(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdB4_t    *pCmdB4 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdB4  = &pPkt->stPkt.unPayload.stCmdB4;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:校验
    bRst = false;
    if((pCmdB4->u8GunId < cPrivDrvGunIdBase) || (pCmdB4->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "预约/定时充电:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmdB4->u8GunId - cPrivDrvGunIdBase;
        memcpy(&pData->stGun[u8Index].stBookingData, pCmdB4, sizeof(stPrivDrvCmdB4_t));
    }
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    //4:回复
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    return(true);
}

















#endif




























