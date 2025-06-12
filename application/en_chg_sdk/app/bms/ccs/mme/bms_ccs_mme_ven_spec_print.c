/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_mme_ven_spec_print.c
* Description                           :   MME协议实现 之 供应商特定的MME帧 打印功能
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   基于联芯通文档 <MSE102x Vertexcom Vender-Specific MME Specification> 开发
****************************************************************************************************/
#include "en_log.h"
#include "en_common.h"
#include "bms_ccs_mme_ven_spec.h"
#include "bms_ccs_mme_ven_spec_data_def.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme_ven_spec_print";









const char u8MmeVsRstStr[eMmeVsRstMax][16] = 
{
    "0 ->Success",
    "1 ->Failure",
};



const char u8MmeVsLinkStateStr[eMmeVsGetStatusLinkStateMax][32] = 
{
    "0 ->Disconnected",
    "1 ->Connected",
};



const char u8MmeVsReadyStr[eMmeVsGetStatusReadyMax][16] = 
{
    "0 ->Not Ready",
    "1 ->Ready",
};




void sMmePrintVsGetVersionCnf(i32 i32MmeDevIndex, void *pParam);
void sMmePrintVsResetCnf(i32 i32MmeDevIndex, void *pParam);
void sMmePrintVsGetStatusCnf(i32 i32MmeDevIndex, void *pParam);
void sMmePrintVsGetLinkStatsCnf(i32 i32MmeDevIndex, void *pParam, i32 i32Size);
void sMmePrintVsGetNwInfoCnf(i32 i32MmeDevIndex, void *pParam);
















/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_VERSION.cnf 内容打印
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
void sMmePrintVsGetVersionCnf(i32 i32MmeDevIndex, void *pParam)
{
    stMmeVsGetVersionCnf_t *pCnf;
    
    pCnf = (stMmeVsGetVersionCnf_t *)pParam;
    EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_GET_VERSION.cnf", i32MmeDevIndex);
    EN_SLOGD(TAG, "VS_GET_VERSION.cnf->Result     :%s", u8MmeVsRstStr[pCnf->eRst]);
    EN_SLOGD(TAG, "                  ->Device ID  :%02X %02X", pCnf->u8DeviceId[0], pCnf->u8DeviceId[1]);
    EN_SLOGD(TAG, "                  ->Image Index:%d", pCnf->u8ImageIndex);
    EN_SLOGD(TAG, "                  ->Boot    Ver:%s", pCnf->u8BootVer);
    EN_SLOGD(TAG, "                  ->App     Ver:%s", pCnf->u8AppVer);
    EN_SLOGD(TAG, "                  ->AvStack Ver:%s", pCnf->u8AvStackVer);
    EN_SLOGD(TAG, "                  ->App    Name:%s", pCnf->u8AppAlternate);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_RESET.cnf 内容打印
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
void sMmePrintVsResetCnf(i32 i32MmeDevIndex, void *pParam)
{
    stMmeVsResetCnf_t *pCnf;
    
    pCnf = (stMmeVsResetCnf_t *)pParam;
    EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_RESET.cnf)", i32MmeDevIndex);
    EN_SLOGD(TAG, "VS_RESET.cnf->Result      :%s", u8MmeVsRstStr[pCnf->eRst]);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_STATUS.cnf 内容打印
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   由于部分时序下 查询本消息的周期加快到几十ms级，
*                                           打印改为60秒一次，多PLC场景下公用此计时器。
****************************************************************************************************/
void sMmePrintVsGetStatusCnf(i32 i32MmeDevIndex, void *pParam)
{
    static u32 u32TimeOld = 0;
    stMmeVsGetStatusCnf_t *pCnf;
    
    if((sGetTimestamp() - u32TimeOld) >= 60)
    {
        u32TimeOld = sGetTimestamp();
        pCnf = (stMmeVsGetStatusCnf_t *)pParam;
        EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_GET_STATUS.cnf", i32MmeDevIndex);
        EN_SLOGD(TAG, "VS_GET_STATUS.cnf->Result      :%s",          u8MmeVsRstStr[pCnf->eRst]);
        EN_SLOGD(TAG, "                 ->连接    状态:%s",          u8MmeVsLinkStateStr[pCnf->eLinkState]);
        EN_SLOGD(TAG, "                 ->PLC 准备状态:%s",          u8MmeVsReadyStr[pCnf->eReadySts]);
        EN_SLOGD(TAG, "                 ->残余    频率:%lld mppm",   pCnf->i64FreqErr);
        EN_SLOGD(TAG, "                 ->频率    偏移:%lld Hz",     pCnf->i64FreqOffset);
        EN_SLOGD(TAG, "                 ->系统运行时间:%llu 秒",     pCnf->u64Uptime);
        EN_SLOGD(TAG, "                 ->认证    时间:%llu 秒",     pCnf->u64AuthTime);
        EN_SLOGD(TAG, "                 ->认证站点数量:%d 个",       pCnf->u16AuthCount);
    }
    
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_LINK_STATS.cnf 内容打印
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
void sMmePrintVsGetLinkStatsCnf(i32 i32MmeDevIndex, void *pParam, i32 i32Size)
{
    stMmeVsGetLinkStatsCnfForTx_t *pTxCnf;
    stMmeVsGetLinkStatsCnfForRx_t *pRxCnf;
    
    pTxCnf = (stMmeVsGetLinkStatsCnfForTx_t *)pParam;
    pRxCnf = (stMmeVsGetLinkStatsCnfForRx_t *)pParam;
    
    if(i32Size == sizeof(stMmeVsGetLinkStatsCnfForTx_t))
    {
        //Link Statistics for Transmit Link
        EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_GET_LINK_STATS.cnf for Transmit Link", i32MmeDevIndex);
        EN_SLOGD(TAG, "VS_GET_LINK_STATS.cnf->Result          :%s",  u8MmeVsRstStr[pTxCnf->eRst]);
        EN_SLOGD(TAG, "                     ->Req Id          :%d",  pTxCnf->u8ReqId);
        EN_SLOGD(TAG, "                     ->MSDUs      Total:%d",  pTxCnf->stLinkStatsTx.u32TxMsduSegSuccess);
        EN_SLOGD(TAG, "                     ->MPDUs      Total:%d",  pTxCnf->stLinkStatsTx.u32TxMpdu);
        EN_SLOGD(TAG, "                     ->MPDUs      Burst:%d",  pTxCnf->stLinkStatsTx.u32TxMpduBurst);
        EN_SLOGD(TAG, "                     ->MPDUs      Acked:%d",  pTxCnf->stLinkStatsTx.u32TxMpduAcked);
        EN_SLOGD(TAG, "                     ->MPDUs  Collision:%d",  pTxCnf->stLinkStatsTx.u32TxMpduColl);
        EN_SLOGD(TAG, "                     ->MPDUs     Failed:%d",  pTxCnf->stLinkStatsTx.u32TxMpduFail);
        EN_SLOGD(TAG, "                     ->PB       Success:%d",  pTxCnf->stLinkStatsTx.u32TxPbSuccess);
        EN_SLOGD(TAG, "                     ->PB       Dropped:%d",  pTxCnf->stLinkStatsTx.u32TxPbDropped);
        EN_SLOGD(TAG, "                     ->PB      CRC Fail:%d",  pTxCnf->stLinkStatsTx.u32TxPbCrcFail);
        EN_SLOGD(TAG, "                     ->缓冲短缺事件数量:%d",  pTxCnf->stLinkStatsTx.u32TxBufShortageDrop);
    }
    else
    {
        //Link Statistics for Receive Link
        EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_GET_LINK_STATS.cnf for Receive Link", i32MmeDevIndex);
        EN_SLOGD(TAG, "VS_GET_LINK_STATS.cnf->Result          :%s",  u8MmeVsRstStr[pRxCnf->eRst]);
        EN_SLOGD(TAG, "                     ->Req Id          :%d",  pRxCnf->u8ReqId);
        EN_SLOGD(TAG, "                     ->MSDUs      Total:%d",  pRxCnf->stLinkStatsRx.u32RxMsduSuccess);
        EN_SLOGD(TAG, "                     ->MPDUs      Total:%d",  pRxCnf->stLinkStatsRx.u32RxMpdu);
        EN_SLOGD(TAG, "                     ->MPDUs      Burst:%d",  pRxCnf->stLinkStatsRx.u32RxMpduBurst);
        EN_SLOGD(TAG, "                     ->MPDUs      Acked:%d",  pRxCnf->stLinkStatsRx.u32RxMpduAcked);
        EN_SLOGD(TAG, "                     ->MPDUs     Failed:%d",  pRxCnf->stLinkStatsRx.u32RxMpduFail);
        EN_SLOGD(TAG, "                     ->MPDUs ICV Failed:%d",  pRxCnf->stLinkStatsRx.u32RxMpduIcvFail);
        EN_SLOGD(TAG, "                     ->PB         Total:%d",  pRxCnf->stLinkStatsRx.u32RxPb);
        EN_SLOGD(TAG, "                     ->PB       Success:%d",  pRxCnf->stLinkStatsRx.u32RxPbSuccess);
        EN_SLOGD(TAG, "                     ->PB    Duplicated:%d",  pRxCnf->stLinkStatsRx.u32RxPbDuplicated);
        EN_SLOGD(TAG, "                     ->PB      CRC Fail:%d",  pRxCnf->stLinkStatsRx.u32RxPbCrcFail);
        EN_SLOGD(TAG, "                     ->BER        Total:%lld",pRxCnf->stLinkStatsRx.u64RxSumBerInPbSuccess);
        EN_SLOGD(TAG, "                     ->SSN   Under  Min:%d",  pRxCnf->stLinkStatsRx.u32RxSsnUnderMin);
        EN_SLOGD(TAG, "                     ->SSN   Over   Max:%d",  pRxCnf->stLinkStatsRx.u32RxSsnOverMax);
        EN_SLOGD(TAG, "                     ->PB        Missed:%d",  pRxCnf->stLinkStatsRx.u32RxPbMissed);
    }
    
    

}








/***************************************************************************************************
* Description                           :   MME 帧处理函数之 VS_GET_LINK_STATS.cnf 内容打印
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   
****************************************************************************************************/
void sMmePrintVsGetNwInfoCnf(i32 i32MmeDevIndex, void *pParam)
{
    i32 i;
    stMmeVsGetNwInfoCnf_t *pCnf;
    
    pCnf = (stMmeVsGetNwInfoCnf_t *)pParam;
    
    EN_SLOGD(TAG, "SECC[%d], vender Spec 接收:VS_GET_NW_INFO.cnf", i32MmeDevIndex);
    
    EN_HLOGD(TAG, "VS_GET_NW_INFO.cnf->u8Nid          :",            pCnf->u8Nid, sizeof(pCnf->u8Nid));
    EN_SLOGD(TAG, "                  ->短网络   标识符:%d",          pCnf->u8Snid);
    EN_SLOGD(TAG, "                  ->CCO的    标识符:%d",          pCnf->u8CCcoTei);
    EN_HLOGD(TAG, "                  ->CCO的   MAC地址:",            pCnf->u8CCcoMac, sizeof(pCnf->u8CCcoMac));
    EN_SLOGD(TAG, "                  ->STA站点    数量:%d 个",       pCnf->u8StaNum);
    for(i = 0; i < pCnf->u8StaNum; i++)
    {
        EN_SLOGD(TAG, "                  ->STA%d           :",            i);
        EN_SLOGD(TAG, "                  ->终端设备 标识符:%d",          pCnf->stSta[i].u8StaTei);
        EN_HLOGD(TAG, "                  ->MAC        地址:",            pCnf->stSta[i].u8StaMac, sizeof(pCnf->stSta[i].u8StaMac));
        EN_SLOGD(TAG, "                  ->Phy编码传输速率:%dMbps",      pCnf->stSta[i].u16PhyTxCoded);
        EN_SLOGD(TAG, "                  ->Phy原始传输速率:%dMbps",      pCnf->stSta[i].u16PhyTxRaw);
        EN_SLOGD(TAG, "                  ->Phy编码接收速率:%dMbps",      pCnf->stSta[i].u16PhyRxCoded);
        EN_SLOGD(TAG, "                  ->Phy原始接收速率:%dMbps",      pCnf->stSta[i].u16PhyRxRaw);
        EN_SLOGD(TAG, "                  ->自 动 增 益控制:%d",          pCnf->stSta[i].u8AgcGain);
    }
}






