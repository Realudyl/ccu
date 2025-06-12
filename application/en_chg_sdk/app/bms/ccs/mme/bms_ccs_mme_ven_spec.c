/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_ven_spec.c
* Description                           :   MME协议实现 之 供应商特定的MME帧
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   
****************************************************************************************************/
#include "en_common.h"
#include "en_log.h"

#include "bms_ccs_mme.h"
#include "bms_ccs_mme_op.h"

#include "bms_ccs_mme_ven_spec.h"
#include "bms_ccs_mme_ven_spec_print.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme_ven_spec";





extern stMmeCmdTypeMap_t stMmeCmdTypeMap[];

extern stMmeCache_t *pMmeCache[];





//MME_VS 帧发送函数
bool sMmePacketSendVsGetVersionReq(struct pev_item *pPev, stMmeVsGetVersionCnf_t *pCnf);
bool sMmePacketSendVsResetReq(struct pev_item *pPev);
bool sMmePacketSendVsGetStatusReq(struct pev_item *pPev, stMmeVsGetStatusCnf_t *pCnf);
bool sMmePacketSendVsGetLinkStatsReq(struct pev_item *pPev, stMmeVsGetLinkStatsReq_t *pReq);
bool sMmePacketSendVsGetNwInfoReq(struct pev_item *pPev);
bool sMmePacketSendVsFileAccessReq(struct pev_item *pPev, stMmeVsFileAccessReq_t *pReq, stMmeVsFileAccessCnf_t *pCnf);



//MME_VS 帧接收处理函数
bool sMmePacketRecvVsGetVersionCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool sMmePacketRecvVsResetCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool sMmePacketRecvVsGetStatusCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool sMmePacketRecvVsGetLinkStatsCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool sMmePacketRecvVsGetNwInfoCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool sMmePacketRecvVsFileAccessCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);








/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_GET_VERSION.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketSendVsGetVersionReq(struct pev_item *pPev, stMmeVsGetVersionCnf_t *pCnf)
{
    bool bRst;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], Vender Spec 发送:VS_GET_VERSION.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_GET_VERSION_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_GET_VERSION_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:数据返回
    if(bRst == true)
    {
        if(pCnf != NULL)
        {
            memset(pCnf, 0, sizeof(stMmeVsGetVersionCnf_t));
            memcpy(pCnf, &pCache->stMmeVs.stVsGetVersionCnf, sizeof(stMmeVsGetVersionCnf_t));
        }
    }
    
    
    //4:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_RESET.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketSendVsResetReq(struct pev_item *pPev)
{
    bool bRst;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], Vender Spec 发送:VS_RESET.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_RESET_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_RESET_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:数据返回
    
    //4:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_GET_STATUS.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   这个消息需要持续查询，内部不要产生打印信息，避免刷屏
****************************************************************************************************/
bool sMmePacketSendVsGetStatusReq(struct pev_item *pPev, stMmeVsGetStatusCnf_t *pCnf)
{
    bool bRst;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    //EN_SLOGD(TAG, "SECC[%d], Vender Spec 发送:VS_GET_STATUS.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_GET_STATUS_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_GET_STATUS_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:数据返回
    if(bRst == true)
    {
        if(pCnf != NULL)
        {
            memset(pCnf, 0, sizeof(stMmeVsGetStatusCnf_t));
            memcpy(pCnf, &pCache->stMmeVs.stVsGetStatusCnf, sizeof(stMmeVsGetStatusCnf_t));
        }
    }
    
    
    //4:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_GET_LINK_STATS.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
bool sMmePacketSendVsGetLinkStatsReq(struct pev_item *pPev, stMmeVsGetLinkStatsReq_t *pReq)
{
    bool bRst;
    u32  u32PayloadLen;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], Vender Spec 发送:VS_GET_LINK_STATS.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_GET_LINK_STATS_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    if(eError == MME_SUCCESS)
    {
        eError = mme_put(&pCache->stTxCtx, pReq, sizeof(stMmeVsGetLinkStatsReq_t), &u32PayloadLen);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_GET_LINK_STATS_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}










/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_GET_NW_INFO.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketSendVsGetNwInfoReq(struct pev_item *pPev)
{
    bool bRst;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], Vender Spec 发送:VS_GET_NW_INFO.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_GET_NW_INFO_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_GET_NW_INFO_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:数据返回
    
    
    //4:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧发送函数之 VS_FILE_ACCESS.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
bool sMmePacketSendVsFileAccessReq(struct pev_item *pPev, stMmeVsFileAccessReq_t *pReq, stMmeVsFileAccessCnf_t *pCnf)
{
    bool bRst;
    u32  u32PayloadLen;
    u32  u32PayloadLenTemp;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    
    
    //1.1:填充 payload
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, VS_FILE_ACCESS_REQ, pCache->u8TxBuf, sizeof(pCache->u8TxBuf));
    if(eError == MME_SUCCESS)
    {
        //计算实际的payload长度
        EN_SLOGD(TAG, "SECC[%d], vender Spec 发送:VS_FILE_ACCESS.req, OpCode:%d, FileType:%d, Param:%s, FragTotal:%d, FragSeq:%d, Offset:%d, Leng:%d", 
                pPev->stMmeDev.i32Index, pReq->eOpCode, pReq->eFileType, pReq->u8Param, pReq->u16FragTotal, pReq->u16FragSeqNum, pReq->u32Offset, pReq->u16Length);
        u32PayloadLen = sizeof(stMmeVsFileAccessReq_t);
        eError = mme_put(&pCache->stTxCtx, pReq, u32PayloadLen, &u32PayloadLenTemp);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(VS_FILE_ACCESS_CNF, true, pPev, &pCache->stTxCtx);
        if((bRst == true) && (pCnf != NULL))
        {
            memcpy(pCnf, &pCache->stMmeVs.stVsFileAccessCnf, sizeof(pCache->stMmeVs.stVsFileAccessCnf));
        }
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}







//---------------------------------------------------------------------------------------------------




/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_VERSION.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecvVsGetVersionCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeVsGetVersionCnf_t stCnf;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&pCache->stMmeVs.stVsGetVersionCnf, 0, sizeof(pCache->stMmeVs.stVsGetVersionCnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsGetVersionCnf, sizeof(pCache->stMmeVs.stVsGetVersionCnf), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = (pCache->stMmeVs.stVsGetVersionCnf.eRst == eMmeVsRstSuccess) ? true : false;
        sMmePrintVsGetVersionCnf(pMmeDev->i32Index, &pCache->stMmeVs.stVsGetVersionCnf);
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_RESET.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecvVsResetCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&pCache->stMmeVs.stMmeVsResetCnf, 0, sizeof(pCache->stMmeVs.stMmeVsResetCnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stMmeVsResetCnf, sizeof(pCache->stMmeVs.stMmeVsResetCnf), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = (pCache->stMmeVs.stMmeVsResetCnf.eRst == eMmeVsRstSuccess) ? true : false;
        sMmePrintVsResetCnf(pMmeDev->i32Index, &pCache->stMmeVs.stMmeVsResetCnf);
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_STATUS.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-05
* notice                                :   这个消息需要持续查询和接收，内部不要产生打印信息，避免刷屏
****************************************************************************************************/
bool sMmePacketRecvVsGetStatusCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&pCache->stMmeVs.stVsGetStatusCnf, 0, sizeof(pCache->stMmeVs.stVsGetStatusCnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsGetStatusCnf, sizeof(pCache->stMmeVs.stVsGetStatusCnf), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = (pCache->stMmeVs.stVsGetStatusCnf.eRst == eMmeVsRstSuccess) ? true : false;
        sMmePrintVsGetStatusCnf(pMmeDev->i32Index, &pCache->stMmeVs.stVsGetStatusCnf);
        if(bRst == true)
        {
            if(pCache->eLinkState != pCache->stMmeVs.stVsGetStatusCnf.eLinkState)
            {
                EN_SLOGI(TAG, "SECC[%d], PLC LinkState变化:%d--->%d", pMmeDev->i32Index, pCache->eLinkState, pCache->stMmeVs.stVsGetStatusCnf.eLinkState);
            }
            pCache->eLinkState = pCache->stMmeVs.stVsGetStatusCnf.eLinkState;
        }
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_LINK_STATS.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecvVsGetLinkStatsCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    if((pCache->stRxCtx.tail - pCache->stRxCtx.head) == sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForTx))
    {
        memset(&pCache->stMmeVs.stVsGetLinkStatsCnfForTx, 0, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForTx));
        mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
        mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
        if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsGetLinkStatsCnfForTx, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForTx), &u32PayloadLen) == MME_SUCCESS)
        {
            bRst = (pCache->stMmeVs.stVsGetLinkStatsCnfForTx.eRst == eMmeVsRstSuccess) ? true : false;
            sMmePrintVsGetLinkStatsCnf(pMmeDev->i32Index, &pCache->stMmeVs.stVsGetLinkStatsCnfForTx, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForTx));
        }
    }
    else
    {
        memset(&pCache->stMmeVs.stVsGetLinkStatsCnfForRx, 0, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForRx));
        mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
        mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
        if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsGetLinkStatsCnfForRx, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForRx), &u32PayloadLen) == MME_SUCCESS)
        {
            bRst = (pCache->stMmeVs.stVsGetLinkStatsCnfForRx.eRst == eMmeVsRstSuccess) ? true : false;
            sMmePrintVsGetLinkStatsCnf(pMmeDev->i32Index, &pCache->stMmeVs.stVsGetLinkStatsCnfForRx, sizeof(pCache->stMmeVs.stVsGetLinkStatsCnfForRx));
        }
    }
    
    
    //2:payload 内字段解析
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_NW_INFO.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecvVsGetNwInfoCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload  此消息应该根据实际的 STA 数量调整接收的字节数
    bRst = false;
    memset(&pCache->stMmeVs.stVsGetNwInfoCnf, 0, sizeof(pCache->stMmeVs.stVsGetNwInfoCnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsGetNwInfoCnf, (pCache->stRxCtx.tail - pCache->stRxCtx.head), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        sMmePrintVsGetNwInfoCnf(pMmeDev->i32Index, &pCache->stMmeVs.stVsGetNwInfoCnf);
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_NW_INFO.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecvVsFileAccessCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    stMmeCache_t *pCache = NULL;
    
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&pCache->stMmeVs.stVsFileAccessCnf, 0, sizeof(pCache->stMmeVs.stVsFileAccessCnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &pCache->stMmeVs.stVsFileAccessCnf, (pCache->stRxCtx.tail - pCache->stRxCtx.head), &u32PayloadLen) == MME_SUCCESS)
    {
        if(pCache->stMmeVs.stVsFileAccessCnf.eRst == eMmeVsRstFail)
        {
            EN_SLOGD(TAG, "SECC[%d], vender 接收:VS_FILE_ACCESS.cnf, eRst:%d, 错误信息:%s", pMmeDev->i32Index, pCache->stMmeVs.stVsFileAccessCnf.eRst, pCache->stMmeVs.stVsFileAccessCnf.u8Data);
        }
        EN_SLOGD(TAG, "SECC[%d], vender 接收:VS_FILE_ACCESS.cnf, OpCode:%d, FileType:%d, Param:%s, FragTotal:%d, FragSeq:%d, Offset:%d, Leng:%d, eRst:%d", 
                 pMmeDev->i32Index, 
                 pCache->stMmeVs.stVsFileAccessCnf.eOpCode, 
                 pCache->stMmeVs.stVsFileAccessCnf.eFileType, 
                 pCache->stMmeVs.stVsFileAccessCnf.u8Param, 
                 pCache->stMmeVs.stVsFileAccessCnf.u16FragTotal, 
                 pCache->stMmeVs.stVsFileAccessCnf.u16FragSeqNum, 
                 pCache->stMmeVs.stVsFileAccessCnf.u32Offset, 
                 pCache->stMmeVs.stVsFileAccessCnf.u16Length, 
                 pCache->stMmeVs.stVsFileAccessCnf.eRst);
        
        bRst = (pCache->stMmeVs.stVsFileAccessCnf.eRst == eMmeVsRstSuccess) ? true : false;
    }
    
    
    //2:payload 内字段解析
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}














