/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_02a2.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-19
* notice                                :     本文件负责实现内网协议中 02/A2 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_02a2.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_02a2";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 A2报文
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     供 Master 端使用
*                         A2报文对 Master 来说是 Ack发送 要使用 Ack发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendA2(void)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA2_t    *pCmdA2 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pPrivDrvCache->hAckTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8AckTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA2  = &pPkt->stPkt.unPayload.stCmdA2;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmdA2_t);
    memcpy(pCmdA2, &pData->stChg.stCacheA2, u16Len);
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmdA2, pData->stChg.u16Addr, pData->stChg.u8AckSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd02, (u8 *)pPkt, u16Len, false);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 02报文
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv02(const u8 *pBuf, i32 i32Len)
{
    i32  i;
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd02_t    *pCmd02 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd02  = &pPkt->stPkt.unPayload.stCmd02;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:SN号校验
    bRst = false;
    if(!sSnIsValid(pCmd02->u8Sn, sizeof(pCmd02->u8Sn)))
    {
        EN_SLOGE(TAG, "桩端签到:SN号格式错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        pData->stChg.u8GunNum               = pCmd02->u8GunNum;
        memcpy(&pData->stChg.stCache02, pCmd02, sizeof(stPrivDrvCmd02_t));
        for(i = 0; i < cPrivDrvGunNumMax; i++)
        {
            pData->stGun[i].eWorkStatus     = pCmd02->stGunStatus.eWorkStatus[i];
            pData->stGun[i].eConnStatus     = pCmd02->stGunStatus.eConnStatus[i];
            pData->stGun[i].eLockStatus     = pCmd02->stGunStatus.eLockStatus[i];
        }
        
        //桩处于boot情况,需要把版本号修改
        if((pCmd02->u8SwVer[0] == 0) || (pCmd02->u8SwVer[0] == 0xFF))
        {
            memset(pData->stChg.stCache02.u8SwVer, 0, sizeof(pData->stChg.stCache02.u8SwVer));
            memcpy(pData->stChg.stCache02.u8SwVer, cChgSwVerBootloader, strlen(cChgSwVerBootloader));
        }
        else if(strstr((char *)pCmd02->u8SwVer, cChgSwVerBootDc) != NULL)
        {
            memset(pData->stChg.stCache02.u8SwVer, 0, sizeof(pData->stChg.stCache02.u8SwVer));
            memcpy(pData->stChg.stCache02.u8SwVer, cChgSwVerBootloader, strlen(cChgSwVerBootloader));
        }
        
        pData->stChg.bCache02Flag           = true; 
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    //4:回复
    pData->stChg.u16Addr                = pHead->u16Addr;
    pData->stChg.u8AckSeqno             = pHead->u8Seqno;
    pData->stChg.stCacheA2.eRst         = ((bRst) && (pData->stChg.eLoginAck == ePrivDrvCmdRstSuccess)) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    pData->stChg.stCacheA2.eReason      = ePrivDrvLoginFailReasonNull;
    pData->stChg.stCacheA2.u32Time      = cPrivDrvReloginGap;
    pData->stChg.stCacheA2.eHwType      = pData->stChg.eHwType;
    sPrivDrvPktSendA2();
    
#if(cPrivDrv02RecvPrintf == 1)
    EN_SLOGW(TAG,"SN:%s,Name:%s,Ver:%s,最大功率:%u,最大电流:%u,工作模式:%d,重启原因:%d", pData->stChg.stCache02.u8Sn,
    pData->stChg.stCache02.u8DevName,
    pData->stChg.stCache02.u8SwVer,
    pData->stChg.stCache02.i32PowerMax,
    pData->stChg.stCache02.i32CurrMax,
    pData->stChg.stCache02.eWorkMode,
    pData->stChg.stCache02.eReason);
#endif
    
    return(true);
}






#elif (cSdkPrivDevType == cSdkPrivDevTypeS)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 02报文
* Author            :     Hall
* modified Date     :     2023-10-20
* notice            :     供 Slave 端使用
*                         02报文对 Slave 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSend02(void)
{
    bool bRst;
    
    i32  i;
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd02_t    *pCmd02 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd02  = &pPkt->stPkt.unPayload.stCmd02;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    u16Len = sizeof(stPrivDrvCmd02_t);
    memcpy(pCmd02, &pData->stChg.stCache02, u16Len);
    pCmd02->u8GunNum = pData->stChg.u8GunNum;
    for(i = 0; i < cPrivDrvGunNumMax; i++)
    {
        pCmd02->stGunStatus.eWorkStatus[i] = pData->stGun[i].eWorkStatus;
        pCmd02->stGunStatus.eConnStatus[i] = pData->stGun[i].eConnStatus;
        pCmd02->stGunStatus.eLockStatus[i] = pData->stGun[i].eLockStatus;
    }
    
    //2.2:head 
    sPrivDrvSetHead(pHead, ePrivDrvCmd02, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmdA2, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 A2报文
* Author            :     Hall
* modified Date     :     2023-10-20
* notice            :     供 Slave 端使用
***********************************************************************************************/
bool sPrivDrvPktRecvA2(const u8 *pBuf, i32 i32Len)
{
    i32  i;
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdA2_t    *pCmdA2 = NULL;
    stPrivDrvDataSlave_t    *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdA2  = &pPkt->stPkt.unPayload.stCmdA2;
    pData   = &pPrivDrvCache->unData.stSlave;
    
    
    //2:数据解析及缓存
    memcpy(&pData->stChg.stCacheA2, pCmdA2, sizeof(stPrivDrvCmdA2_t));
    bRst = (pCmdA2->eRst == ePrivDrvCmdRstSuccess) ? true : false;
    EN_SLOGI(TAG, "桩端签到应答结果:%d (0---失败, 1---成功)", bRst);
    pData->stChg.bCacheA2Flag = bRst;
    for(i = cPrivDrvGunIdChg; i < (cPrivDrvGunIdBase + cPrivDrvGunNumMax); i++)
    {
        //鉴权收到master应答以后 主动上报一轮03报文 同步桩端状态
        pPrivDrvCache->u32Time03[i] = 0;
    }
    
    
    //3:处理结果填充及信号量释放
    sPrivDrvPktRecvRstSet(pHead->eCmd, bRst);
    
    
    return(true);
}






#endif




























