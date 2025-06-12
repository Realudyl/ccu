/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0bab_30.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-01-24
* notice                                :     主要兼容桩BOOT内网3.0帧格式 02/A2 报文
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_0bab_30.h"
#include "private_drv_0bab_update.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0bab_30";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)






/**********************************************************************************************
* Description       :     串口数据 帧发送处理函数 之 AB报文
* Author            :     Hall
* modified Date     :     2023-11-03
* notice            :     供 Master 端使用
*                         AB报文对 Master 来说是 Rpt发送 要使用 Rpt发送 的资源
***********************************************************************************************/
bool sPrivDrvPktSendAB30(u16 u16AddrCpu, u16 u16AddrBase, u16 u16Offset, ePrivDrvUpdateCmd_t eCmd, const u16 *pBuf, u16 u16DataLen)
{
    bool bRst;
    
    u16  u16Len;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmdAB_t    *pCmdAB = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    //0:枪号校验
    if(u16DataLen > (sizeof(pCmdAB->u16Data) / sizeof(pCmdAB->u16Data[0])))
    {
        EN_SLOGE(TAG, "远程升级数据, 数据包长度超限:%d", u16DataLen);
        return 0;
    }
    
    
    //1:访问保护---使用 Rpt发送 资源
    xSemaphoreTake(pPrivDrvCache->hRptTxBufMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)pPrivDrvCache->u8RptTxBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmdAB  = &pPkt->stPkt.unPayload.stCmdAB;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //2:填充数据
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    
    //2.1:payload
    pCmdAB->u16AddrCpu  = u16AddrCpu;
    pCmdAB->u16AddrBase = u16AddrBase;
    pCmdAB->u16Offset   = u16Offset;
    pCmdAB->eCmd        = eCmd;
    pCmdAB->u16Len      = u16DataLen;
    memcpy((u8 *)pCmdAB->u16Data, (u8 *)pBuf, u16DataLen * sizeof(u16));//2个字节
    u16Len = sizeof(stPrivDrvCmdAB_t) + u16DataLen * sizeof(u16) - sizeof(pCmdAB->u16Data);
    
    //2.2:head
    sPrivDrvSetHead30(pHead, ePrivDrvCmdAB, pData->stChg.u16Addr, pData->stChg.u8RptSeqno, u16Len);
    u16Len = u16Len + cPrivDrvHeadSize;
    
    //2.3:check sum
    sPrivDrvSetCrc((u8 *)pPkt, u16Len);
    u16Len = u16Len + cPrivDrvCrcSize;
    
    
    //3:发送数据
    //由于本协议上行和下行报文的消息类型不一样
    //需要找到自己对应的接收报文 去检查其 bProcRst&hAckMutex
    bRst = sPrivDrvSend(ePrivDrvCmd0B, (u8 *)pPkt, u16Len, true);
    
    //4:访问保护解除
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 0B报文
* Author            :     Hall
* modified Date     :     2023-11-03
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv0B30(const u8 *pBuf, i32 i32Len)
{
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    stPrivDrvCmd0B_t    *pCmd0B = NULL;
    //stPrivDrvDataMaster_t   *pData  = NULL;
    
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pCmd0B  = &pPkt->stPkt.unPayload.stCmd0B;
    //pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:校验
    
    
    //2:数据解析及缓存
    chgFwUpdateChargerRecvAck(pCmd0B->eCmd, pCmd0B->u16Rst);
    
    
    //3:处理结果填充及信号量释放
    //这里有一点特殊, 始终设置 true 而不根据0B报文的 操作结果 字段去设置
    //因为需要将该字段的值 返回给烧录接口去处理 这边就不用过滤了
    sPrivDrvPktRecvRstSet(pHead->eCmd, true);
    
    
    //4:回复
    
    return(true);
}












#endif




























