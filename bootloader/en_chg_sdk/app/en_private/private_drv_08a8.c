/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_08a8.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-31
* notice                                :     本文件负责实现内网协议中 08/A8 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_08a8.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_08a8";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A8报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Master 端使用
*                         A8报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA8(u8 u8GunId, ePrivDrvCmdRst_t eRst, u32 u32AccountBalance, i32 i32RsvdLen, const u8 *pRsvd)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA8_t    *pCmdA8 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "充电中实时数据上报应答:枪号错误!!!");
        return(false);
    }
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA8  = &pPkt->stPkt.unPayload.stCmdA8;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len                      = sizeof(stPrivDrvCmdA8_t);
    pCmdA8->u8GunId             = u8GunId;
    pCmdA8->eRst                = eRst;
    pCmdA8->eNetStatus          = pData->stChg.eNetStatus;
    pCmdA8->u32AccountBalance   = u32AccountBalance;
    if(pRsvd != NULL)
    {
        memcpy(pCmdA8->u8Data,  pRsvd, i32RsvdLen);
    }
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA8, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd08, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 08报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv08(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd08_t    *pCmd08 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd08  = &pPkt->stPkt.unPayload.stCmd08;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmd08->u8GunId < cPrivDrvGunIdBase) || (pCmd08->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "充电中实时数据上报:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index = pCmd08->u8GunId - cPrivDrvGunIdBase;
        
        pData->stGun[u8Index].eWorkStatus               = pCmd08->eWorkStatus;
        memcpy(&pData->stGun[u8Index].stRtData,           pCmd08,                       sizeof(stPrivDrvCmd08_t));
        
        if(pData->stGun[u8Index].hCache08Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCache08Mutex);
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    pData->stChg.u16Addr    = pHead->u16Addr;
    pData->stChg.u8AckSeqno = pHead->u8Seqno;
    
    
    return(true);
}








#elif (cSdkPrivDevType == cSdkPrivDevTypeS)





/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 08报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Slave 端使用
*                         08报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
*
*                         用户程序充电时维护的是充电记录, 上报充电实时数据时也是传入该充电记录
*                         所以形参是09报文，需要一一填充
***********************************************************************************************/
bool sPrivDrvPktSend08(u8 u8GunId, stPrivDrvCmd09_t *pRcdData)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd08_t    *pCmd08 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //0:枪号校验
    if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax) || (pRcdData == NULL) || (u8GunId != pRcdData->u8GunId))
    {
        EN_SLOGE(TAG, "充电中实时数据上报:枪号错误!!!");
        return(false);
    }
    if(pPrivDrvCache->bPkt08EnableFlag[u8GunId] == false)
    {
        //还没到发送周期
        return(false);
    }
    pPrivDrvCache->bPkt08EnableFlag[u8GunId] = false;
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd08  = &pPkt->stPkt.unPayload.stCmd08;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    pCmd08->u8GunId             = u8GunId;
    pCmd08->eRptType            = pData->stGun[u8GunId - cPrivDrvGunIdBase].eRptType;
    pCmd08->eType               = pRcdData->eType;
    pCmd08->eMode               = pRcdData->eMode;
    pCmd08->unParam.u32Value    = pRcdData->unParam.u32Value;
    pCmd08->eWorkStatus         = pData->stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus;
    pCmd08->u32Power            = pData->stGun[u8GunId - cPrivDrvGunIdBase].u32ChargingPower;
    pCmd08->u32ChargingTime     = pRcdData->u32ChargingTime;
    pCmd08->stTimeStart         = pRcdData->stTimeStart;
    pCmd08->u32MeterStart       = pRcdData->u32MeterStart;
    pCmd08->u32MeterNow         = pRcdData->u32MeterStop;
    pCmd08->u8SocStart          = (((i8)pRcdData->u8SocStart) < 0) ? 0 : pRcdData->u8SocStart;  //增加负数保护---充电记录从bms获取到真实soc之前可能是负数
    pCmd08->u8SocNow            = (((i8)pRcdData->u8SocStop ) < 0) ? 0 : pRcdData->u8SocStop ;  //增加负数保护
    pCmd08->u32UsedMoney        = pRcdData->u32UsedMoney;
    pCmd08->u16MoneyDataLen     = pRcdData->u16MoneyDataLen;
    memcpy(pCmd08->u8UserId,      pRcdData->u8UserId,        sizeof(pCmd08->u8UserId     ));
    memcpy(pCmd08->u8OrderId,     pRcdData->u8OrderId,       sizeof(pCmd08->u8OrderId    ));
    memcpy(pCmd08->u8SrvTradeId,  pRcdData->u8SrvTradeId,    sizeof(pCmd08->u8SrvTradeId ));
    memcpy(pCmd08->u8DevTradeId,  pRcdData->u8DevTradeId,    sizeof(pCmd08->u8DevTradeId ));
    memcpy(&pCmd08->unMoney,     &pRcdData->unMoney,         pCmd08->u16MoneyDataLen     );
    
    //此处计算 payload 的实际长度, 电量明细里尚未使用的分段不传输
    u16Len = sizeof(stPrivDrvCmd08_t) - sizeof(pCmd08->unMoney) + pCmd08->u16MoneyDataLen;
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd08, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA8, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A8报文
* Author            :     Hall
* modified Date     :     2023-10-31
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA8(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u8   u8Index;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA8_t    *pCmdA8 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA8  = &pPkt->stPkt.unPayload.stCmdA8;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //1:枪号校验
    bRst = false;
    if((pCmdA8->u8GunId < cPrivDrvGunIdBase) || (pCmdA8->u8GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "充电中实时数据上报应答:枪号错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        u8Index                                     = pCmdA8->u8GunId - cPrivDrvGunIdBase;
        pData->stChg.eNetStatus                     = pCmdA8->eNetStatus;
        memcpy(&pData->stGun[u8Index].stRtDataAck, pCmdA8, sizeof(stPrivDrvCmdA8_t));
        
        if(pData->stGun[u8Index].hCacheA8Mutex != NULL)
        {
            xSemaphoreGive(pData->stGun[u8Index].hCacheA8Mutex);
        }
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}







#endif




























