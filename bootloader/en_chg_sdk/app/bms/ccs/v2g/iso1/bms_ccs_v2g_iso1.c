/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1.c
 * @Version               :     V1.0
 * @Author                :     Hall
 * @Date Created          :     2022-07-04
 * @Attention             :     
 * @Brief                 :     ISO15118 协议实现
 * 
 * @History:
 * 
 * 1.@Date: 2022-07-04
 *   @Author: Hall
 *   @Modify: 
 * 
 * 2.@Date: 2024-03-12
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/

/***************************************************************************************************
ISO15118 消息列表：

1、公用消息
SessionSetup                                会话建立
ServiceDiscovery                            服务发现
ServiceDetail                               服务详情
PaymentServiceSelection                     支付和服务选项
CertificateUpdate                           证书更新
CertificateInstallation                     证书安装
PaymentDetails                              支付详情
Authorization                               授权
ChargeParameterDiscovery                    充电参数发现
PowerDelivery                               能量传输
SessionStop                                 会话停止
MeteringReceipt                             计量收据

2、AC Charger专用信息
ChargingStatus                              充电状态

3、DC Charger专用信息
CableCheck                                  电缆检测
PreCharge                                   预充电
CurrentDemand                               当前需求
WeldingDetection                            焊接检测
****************************************************************************************************/

#include "bms_ccs_v2g_iso1.h"
#include "bms_ccs_v2g_iso1_api.h"
#include "bms_ccs_v2g_iso1_basic.h"
#include "bms_ccs_v2g_din_err.h"
#include "bms_ccs_v2g_iso1_print.h"

//TEST
#include "bms_ccs_v2g_timing.h"         //sV2gSeccCommunicationSetupPerformanceTimerRst(i32DevIndex);
//#include "adc_app_sample.h"             //sAdcAppSampleGetCpState(i32DevIndex)

//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_iso1";





stV2gIso1Cache_t *pV2gIso1Cache[cMse102xDevNum];


//---------------------------------------------------------------------------------------------------------



bool  sV2gIso1Init(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex));
bool  sV2gIso1CacheInit(i32 i32DevIndex);

bool  sV2gIso1SetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
bool  sV2gIso1GetSessionStop(i32 i32DevIndex);
bool  sV2gIso1SetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);







//V2G ISO15118 消息处理---------------------------------------------------------
//V2G ISO15118 EXI编解码
i32   sV2gIso1EncodeExiDoc(bitstream_t *pStream, void *pExiDoc);
i32   sV2gIso1DecodeExiDoc(bitstream_t *pStream, void *pExiDoc);

//V2G ISO15118 消息处理
i32   sV2gIso1Proc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen);




//V2G ISO15118 MsgId搜索
i32   sV2gIso1MsgIdSearch(struct iso1EXIDocument* pDocIn);

//V2G ISO15118 dinEXIDocument文档初始化
bool  sV2gIso1DocInit(struct iso1EXIDocument *pDoc, eV2gIso1MsgId_t eMsgId);

//V2G ISO15118 接收消息数据解析及回复报文数据填充
i32   sV2gIso1Parse(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);


//V2G ISO15118 分类消息处理
i32   sV2gIso1SessionSetup(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1ServiceDiscovery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1ServiceDetail(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1PaymentServiceSelection(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1CertificateInstallation(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1CertificateUpdate(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1PaymentDetails(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1Authorization(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1ChargeParameterDiscovery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1CableCheck(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1PreCharge(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1PowerDelivery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1ChargingStatus(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1CurrentDemand(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1MeteringReceipt(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1WeldingDetection(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);
i32   sV2gIso1SessionStop(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut);






//V2G ISO15118 消息映射map
stV2gIso1MsgIdMap_t stV2gIso1MsgIdMap[] =
{
    //确保 eId 的连续性  以便程序可以直接通过 eId 去调用 pProc 函数
    {eV2gIso1MsgIdSupportedAppProtocol,         NULL                                    },
    {eV2gIso1MsgIdSessionSetup,                 sV2gIso1SessionSetup                    },
    {eV2gIso1MsgIdServiceDiscovery,             sV2gIso1ServiceDiscovery                },
    {eV2gIso1MsgIdServiceDetail,                sV2gIso1ServiceDetail                   },
    {eV2gIso1MsgIdPaymentServiceSelection,      sV2gIso1PaymentServiceSelection         },
    {eV2gIso1MsgIdCertificateInstallation,      sV2gIso1CertificateInstallation         },
    {eV2gIso1MsgIdCertificateUpdate,            sV2gIso1CertificateUpdate               },
    {eV2gIso1MsgIdPaymentDetails,               sV2gIso1PaymentDetails                  },
    {eV2gIso1MsgIdAuthorization,                sV2gIso1Authorization                   },
    {eV2gIso1MsgIdChargeParameterDiscovery,     sV2gIso1ChargeParameterDiscovery        },
    {eV2gIso1MsgIdCableCheck,                   sV2gIso1CableCheck                      },
    {eV2gIso1MsgIdPreCharge,                    sV2gIso1PreCharge                       },
    {eV2gIso1MsgIdPowerDeliveryStart,           sV2gIso1PowerDelivery                   },
    {eV2gIso1MsgIdChargingStatus,               sV2gIso1ChargingStatus                  },
    {eV2gIso1MsgIdCurrentDemand,                sV2gIso1CurrentDemand                   },
    {eV2gIso1MsgIdMeteringReceipt,              sV2gIso1MeteringReceipt                 },
    {eV2gIso1MsgIdPowerDeliveryRenegotiate,     sV2gIso1PowerDelivery                   },
    {eV2gIso1MsgIdPowerDeliveryStop,            sV2gIso1PowerDelivery                   },
    {eV2gIso1MsgIdWeldingDetection,             sV2gIso1WeldingDetection                },
    {eV2gIso1MsgIdSessionStop,                  sV2gIso1SessionStop                     },
    
};
















/*******************************************************************************
 * @FunctionName   :      sV2gIso1Init
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月12日  14:34:15
 * @Description    :      ISO15118 协议初始化
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCb0              指向 回调函数:EVSE紧急停机的函数指针
 * @Input          :      pCb1              指向 回调函数:V2G_SECC_Sequence_Timer 复位的函数指针
 * @Input          :      pCb2              指向 回调函数:V2G_SECC_Sequence_Timer 启动的函数指针
 * @Input          :      pCb3              指向 回调函数:V2G_SECC_CPState_Detection_Timeout 启动的函数指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1Init(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex))
{
    stV2gIso1Cache_t *pCache = NULL;
    
    
    EN_SLOGI(TAG, "ISO15118(通道:%d) 协议初始化", i32DevIndex);
    
    if(pV2gIso1Cache[i32DevIndex] == NULL)                                       //首次握手才会执行以下逻辑
    {
        pV2gIso1Cache[i32DevIndex] = (stV2gIso1Cache_t *)MALLOC(sizeof(stV2gIso1Cache_t));
        pV2gIso1Cache[i32DevIndex]->xSemHandshake = xSemaphoreCreateBinary();
    }
    
    
    //缓存数据初始化
    pCache = pV2gIso1Cache[i32DevIndex];
    pCache->pCbShutdownEmg = pCb0;
    pCache->pCbSeqTimerRst = pCb1;
    pCache->pCbSeqTimerStart = pCb2;
    pCache->pCbCpStateDetectionTimerStart = pCb3;
    pCache->pCbSetTcpConnClose = pCb4;
    pCache->eId = eV2gIso1MsgIdSupportedAppProtocol;
    
    xSemaphoreGive(pCache->xSemHandshake);                                      //发出握手信号
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1CacheInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月26日  20:26:15
 * @Description    :      ISO15118 相关缓存数据初始化
 * @Input          :      i32DevIndex     外设号
 * @Return         :      
*******************************************************************************/
bool sV2gIso1CacheInit(i32 i32DevIndex)
{
    //缓存数据初始化
    memset( pV2gIso1Cache[i32DevIndex]->u8SessionId,  0, sizeof(pV2gIso1Cache[i32DevIndex]->u8SessionId ));
    memset(&pV2gIso1Cache[i32DevIndex]->stData,       0, sizeof(pV2gIso1Cache[i32DevIndex]->stData      ));
    memset(&pV2gIso1Cache[i32DevIndex]->stDataPublic, 0, sizeof(pV2gIso1Cache[i32DevIndex]->stDataPublic));
    pV2gIso1Cache[i32DevIndex]->eId = eV2gIso1MsgIdMax;
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1SetCpState
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  14:41:25
 * @Description    :      ISO15118 CP 状态设置
 * @Input          :      i32DevIndex       外设号
 * @Input          :      eCpState          要设置的CP状态
 * @Return         :      
*******************************************************************************/
bool sV2gIso1SetCpState(i32 i32DevIndex, eChgCpState_t eCpState)
{
    return sV2gIso1CpStateErrorCheck(i32DevIndex, eCpState);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1GetSessionStop
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  14:43:00
 * @Description    :      ISO15118 SessionStop 会话查询
 * @Input          :      i32DevIndex       外设号
 * @Return         :      
*******************************************************************************/
bool sV2gIso1GetSessionStop(i32 i32DevIndex)
{
    return (pV2gIso1Cache[i32DevIndex]->eId == eV2gIso1MsgIdSessionStop) ? true : false;
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1SetResponseCodeForCpStateDetectionTimeout
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月20日  20:31:06
 * @Description    :      ISO15118 协议 V2G_SECC_CPState_Detection_Timeout 超时结果填充
 * @Input          :      i32DevIndex       外设号
 * @Input          :      eCpState          检测CP状态
 * @Return         :      
*******************************************************************************/
bool sV2gIso1SetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState)
{
    if((eCpState == eChgCpStateC) || (eCpState == eChgCpStateD))
    {
        //状态C/D         如果是DC的话,是由 ChargeParameterDiscoveryRes  消息发起的CP状态检查,
        //                如果是AC的话,是由 PowerDelivery1Req            消息发起的CP状态检查
        //它的下一条消息
        //                如果是DC的话,就是 eV2gIso1MsgIdCableCheckReq
        //                如果是AC的话,就是 eV2gIso1MsgIdPowerDeliveryStartRes
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCableCheck, iso1responseCodeType_FAILED);
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPowerDeliveryStart, iso1responseCodeType_OK);
    }
    else if(eCpState == eChgCpStateB)
    {
        //状态B 是由 ReadyToChargeState=0 的 PowerDelivery 消息发起的CP状态检查
        //它的下一条消息可以是以下两种
        //这里先全部设置 FAILED 
        //如果实际的报文传输中有 eV2gIso1MsgIdWeldingDetection 消息，那就由该消息处理函数将 eV2gIso1MsgIdSessionStop 的 ResponseCode 清零
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionStop,      iso1responseCodeType_FAILED);
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_FAILED);
    }
    
    return(true);
}













/*******************************************************************************
 * @FunctionName   :      sV2gIso1EncodeExiDoc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月12日  14:20:51
 * @Description    :      ISO15118 编码API
 * @Input          :      pStream           指向数据流的指针
 * @Input          :      pExiDoc           指向要解析的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1EncodeExiDoc(bitstream_t *pStream, void *pExiDoc)
{
    struct iso1EXIDocument *pIso1ExiDoc = NULL;
    
    pIso1ExiDoc = (struct iso1EXIDocument *)pExiDoc;
    
    return encode_iso1ExiDocument(pStream, pIso1ExiDoc);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1DecodeExiDoc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月12日  14:20:53
 * @Description    :      ISO15118 解码API
 * @Input          :      pStream           指向数据流的指针
 * @Input          :      pExiDoc           指向要解析的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1DecodeExiDoc(bitstream_t *pStream, void *pExiDoc)
{
    struct iso1EXIDocument *pIso1ExiDoc = NULL;
    
    pIso1ExiDoc = (struct iso1EXIDocument *)pExiDoc;
    
    return decode_iso1ExiDocument(pStream, pIso1ExiDoc);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1Proc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  14:24:23
 * @Description    :      ISO15118 消息处理函数
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pData             输入的EXI数据序列首地址
 * @Input          :      i32DataLen        输入的EXI数据序列长度
 * @Input          :      pRxBuf            用于解码接收数据的buf----handshake处理有使用buf
 * @Input          :      pTxBuf            用于解码发送数据的buf----底层v2g处理也公用该buf节省ram
 * @Input          :      i32BufLen         pRxBuf & pTxBuf 的长度
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1Proc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen)
{
    bitstream_t stStreamRx;
    bitstream_t stStreamTx;
    
    size_t posRx = 0;
    size_t posTx = 0;
    
    i32 i32Err;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：先初始化数据
    stStreamRx.size = i32BufLen;
    stStreamRx.data = pRxBuf;
    stStreamRx.pos = &posRx;
    memset(pRxBuf, 0, i32BufLen);
    
    stStreamTx.size = i32BufLen;
    stStreamTx.data = pTxBuf;
    stStreamTx.pos = &posTx;
    memset(pTxBuf, 0, i32BufLen);
    
    //2：解码
    memcpy(stStreamRx.data, pData, i32DataLen);
    *(stStreamRx.pos) = 0;
    i32Err = sV2gIso1DecodeExiDoc(&stStreamRx, (void*)&pCache->stDocIn);
    if(i32Err != 0)
    {
        EN_SLOGE(TAG, "ISO15118 v2g Req消息解码错误, errn:%d", i32Err);
        return i32Err;
    }
    
    //3：V2G消息处理 得到 stExiDocTx
    pCache->pCbSeqTimerRst(i32DevIndex);
    sV2gIso1Parse(i32DevIndex, &pCache->stDocIn, &pCache->stDocOut);
    
    //4：编码 并发送
    if(i32Err == 0)
    {
        i32Err = sV2gIso1EncodeExiDoc(&stStreamTx, (void*)&pCache->stDocOut);
        if (i32Err == 0)
        {
            //[V2G-DC-364]
            pCache->pCbSeqTimerStart(i32DevIndex);
            sBmsCcsNetV2gtpSendViaTcp(i32DevIndex, stStreamTx.data, (*stStreamTx.pos), cV2gtpVer, cV2gtpPayloadTypeExi);
        }
        else
        {
            EN_SLOGE(TAG, "ISO15118 v2g Res消息编码错误, errn:%d", i32Err);
        }
    }
    
    return(i32Err);
}















/*******************************************************************************
 * @FunctionName   :      sV2gIso1MsgIdSearch
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  14:19:47
 * @Description    :      ISO15118 消息解析函数 ———— 负责以下各分类消息的映射执行和回复
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1MsgIdSearch(struct iso1EXIDocument* pDocIn)
{
    i32 i32Err = ERROR_UNEXPECTED_REQUEST_MESSAGE;
    
    eV2gIso1MsgId_t eMsgId = eV2gIso1MsgIdMax;
    
    if(pDocIn->V2G_Message_isUsed) 
    {
        init_iso1EXIDocument(pDocIn);
        if (pDocIn->V2G_Message.Body.SessionSetupReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdSessionSetup;
        }
        else if (pDocIn->V2G_Message.Body.ServiceDiscoveryReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdServiceDiscovery;
        }
        else if (pDocIn->V2G_Message.Body.ServiceDetailReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdServiceDetail;
        }
        else if (pDocIn->V2G_Message.Body.PaymentServiceSelectionReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdPaymentServiceSelection;
        }
        else if (pDocIn->V2G_Message.Body.CertificateInstallationReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdCertificateInstallation;
        }
        else if (pDocIn->V2G_Message.Body.CertificateUpdateReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdCertificateUpdate;
        }
        else if (pDocIn->V2G_Message.Body.PaymentDetailsReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdPaymentDetails;
        }
        else if (pDocIn->V2G_Message.Body.AuthorizationReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdAuthorization;
        }
        else if (pDocIn->V2G_Message.Body.ChargeParameterDiscoveryReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdChargeParameterDiscovery;
        }
        else if (pDocIn->V2G_Message.Body.CableCheckReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdCableCheck;
        }
        else if (pDocIn->V2G_Message.Body.PreChargeReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdPreCharge;
        }
        else if (pDocIn->V2G_Message.Body.PowerDeliveryReq_isUsed) 
        {
            switch (pDocIn->V2G_Message.Body.PowerDeliveryReq.ChargeProgress)
            {
                case iso1chargeProgressType_Start:
                    eMsgId = eV2gIso1MsgIdPowerDeliveryStart;
                    break;
                case iso1chargeProgressType_Renegotiate:
                    eMsgId = eV2gIso1MsgIdPowerDeliveryRenegotiate;
                    break;
                case iso1chargeProgressType_Stop:
                    eMsgId = eV2gIso1MsgIdPowerDeliveryStop;
                    break;
                default:
                    break;
            }
        }
        else if (pDocIn->V2G_Message.Body.ChargingStatusReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdChargingStatus;
        }
        else if (pDocIn->V2G_Message.Body.CurrentDemandReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdCurrentDemand;
        }
        else if (pDocIn->V2G_Message.Body.MeteringReceiptReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdMeteringReceipt;
        }
        else if (pDocIn->V2G_Message.Body.WeldingDetectionReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdWeldingDetection;
        }
        else if (pDocIn->V2G_Message.Body.SessionStopReq_isUsed) 
        {
            eMsgId = eV2gIso1MsgIdSessionStop;
        }
    }
    
    return eMsgId;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1DocInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  17:21:31
 * @Description    :      ISO15118 待发送 struct iso1EXIDocument 结构体的初始化
 *                        按照输入的 消息类型 去初始化 准备发送的 EXI 文档 减少重复代码
 * @Input          :      pDoc              指向要解析的EXI文档结构体指针
 * @Input          :      eMsgId            要初始化的消息类型
 * @Return         :      
*******************************************************************************/
bool sV2gIso1DocInit(struct iso1EXIDocument *pDoc, eV2gIso1MsgId_t eMsgId)
{
    bool bRst = true;
    
    init_iso1MessageHeaderType(&pDoc->V2G_Message.Header);
    init_iso1BodyType(&pDoc->V2G_Message.Body);
    pDoc->V2G_Message_isUsed = 1u;
    
    switch(eMsgId)
    {
        case eV2gIso1MsgIdSessionSetup:
            init_iso1SessionSetupResType(&pDoc->V2G_Message.Body.SessionSetupRes);
            pDoc->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdServiceDiscovery:
            init_iso1ServiceDiscoveryResType(&pDoc->V2G_Message.Body.ServiceDiscoveryRes);
            pDoc->V2G_Message.Body.ServiceDiscoveryRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdServiceDetail:
            init_iso1ServiceDetailResType(&pDoc->V2G_Message.Body.ServiceDetailRes);
            pDoc->V2G_Message.Body.ServiceDetailRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdPaymentServiceSelection:
            init_iso1PaymentServiceSelectionResType(&pDoc->V2G_Message.Body.PaymentServiceSelectionRes);
            pDoc->V2G_Message.Body.PaymentServiceSelectionRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdCertificateInstallation:
            init_iso1CertificateInstallationResType(&pDoc->V2G_Message.Body.CertificateInstallationRes);
            pDoc->V2G_Message.Body.CertificateInstallationRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdCertificateUpdate:
            init_iso1CertificateUpdateResType(&pDoc->V2G_Message.Body.CertificateUpdateRes);
            pDoc->V2G_Message.Body.CertificateUpdateRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdPaymentDetails:
            init_iso1PaymentDetailsResType(&pDoc->V2G_Message.Body.PaymentDetailsRes);
            pDoc->V2G_Message.Body.PaymentDetailsRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdAuthorization:
            init_iso1AuthorizationResType(&pDoc->V2G_Message.Body.AuthorizationRes);
            pDoc->V2G_Message.Body.AuthorizationRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdChargeParameterDiscovery:
            init_iso1ChargeParameterDiscoveryResType(&pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes);
            pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdCableCheck:
            init_iso1CableCheckResType(&pDoc->V2G_Message.Body.CableCheckRes);
            pDoc->V2G_Message.Body.CableCheckRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdPreCharge:
            init_iso1PreChargeResType(&pDoc->V2G_Message.Body.PreChargeRes);
            pDoc->V2G_Message.Body.PreChargeRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdPowerDeliveryStart:
        case eV2gIso1MsgIdPowerDeliveryRenegotiate:
        case eV2gIso1MsgIdPowerDeliveryStop:
            init_iso1PowerDeliveryResType(&pDoc->V2G_Message.Body.PowerDeliveryRes);
            pDoc->V2G_Message.Body.PowerDeliveryRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdChargingStatus:
            init_iso1ChargingStatusResType(&pDoc->V2G_Message.Body.ChargingStatusRes);
            pDoc->V2G_Message.Body.ChargingStatusRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdCurrentDemand:
            init_iso1CurrentDemandResType(&pDoc->V2G_Message.Body.CurrentDemandRes);
            pDoc->V2G_Message.Body.CurrentDemandRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdMeteringReceipt:
            init_iso1MeteringReceiptResType(&pDoc->V2G_Message.Body.MeteringReceiptRes);
            pDoc->V2G_Message.Body.MeteringReceiptRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdWeldingDetection:
            init_iso1WeldingDetectionResType(&pDoc->V2G_Message.Body.WeldingDetectionRes);
            pDoc->V2G_Message.Body.WeldingDetectionRes_isUsed = 1u;
            break;
        case eV2gIso1MsgIdSessionStop:
            init_iso1SessionStopResType(&pDoc->V2G_Message.Body.SessionStopRes);
            pDoc->V2G_Message.Body.SessionStopRes_isUsed = 1u;
            break;
        default:
            bRst = false;
            break;
    }
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1Parse
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  17:25:08
 * @Description    :      ISO15118 接收消息数据解析及回复报文数据填充
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1Parse(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    eV2gIso1MsgId_t eMsgId;
    
    eMsgId = sV2gIso1MsgIdSearch(pDocIn);
    
    if((eMsgId < eV2gIso1MsgIdMax)
    && (stV2gIso1MsgIdMap[eMsgId].pProc != NULL))
    {
        return stV2gIso1MsgIdMap[eMsgId].pProc(i32DevIndex, pDocIn, pDocOut);
    }
    
    return(-1);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1SessionSetup
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  20:22:00
 * @Description    :      ISO15118 SessionSetup 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1SessionSetup(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1SessionSetupReqType *pReq = NULL;
    struct iso1SessionSetupResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //<ISO15118-2:2014.pdf>@8.7.3.3 [V2G2-715]
    sV2gSeccCommunicationSetupPerformanceTimerRst(i32DevIndex);
    
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintSessionSetupReq(i32DevIndex, pDocIn);
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdSessionSetup);
    pReq = &pDocIn->V2G_Message.Body.SessionSetupReq;
    pRes = &pDocOut->V2G_Message.Body.SessionSetupRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stSessionSetupReq, pReq, sizeof(pCache->stData.stSessionSetupReq));
    
    
    //生成 session id
    sV2gIso1MakeSessionId(pCache->u8SessionId, sizeof(pCache->u8SessionId));
    vTaskDelay(500 / portTICK_RATE_MS);
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionSetup, iso1responseCodeType_OK);
    sV2gIso1CompSessionIdForSessionSetup(i32DevIndex, pCache->u8SessionId, pReq->EVCCID.bytes, iso1SessionSetupReqType_EVCCID_BYTES_SIZE);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdSessionSetup, &pCache->stData.stSessionSetupRes.ResponseCode);
    
    
    
    //4：填充XML文档结构体
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:EVSEID        字段  ———— 提供 EVSEID 字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEID
    
    //4.3:EVSETimeStamp 字段  ———— 比如时间戳这一类字段,是需要在这里就地赋值的 避免时差
    pCache->stData.stSessionSetupRes.EVSETimeStamp_isUsed = 1u;
    pCache->stData.stSessionSetupRes.EVSETimeStamp = sGetTimestamp();
    
    memcpy(pRes, &pCache->stData.stSessionSetupRes, sizeof(pCache->stData.stSessionSetupRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintSessionSetupRes(i32DevIndex, pDocOut);
    
    
    return 0;
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ServiceDiscovery
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  20:41:39
 * @Description    :      ISO15118 ServiceDiscovery 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1ServiceDiscovery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1ServiceDiscoveryReqType *pReq = NULL;
    struct iso1ServiceDiscoveryResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintServiceDiscoveryReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdServiceDiscovery);
    pReq = &pDocIn->V2G_Message.Body.ServiceDiscoveryReq;
    pRes = &pDocOut->V2G_Message.Body.ServiceDiscoveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stServiceDiscoveryReq, pReq, sizeof(pCache->stData.stServiceDiscoveryReq));
    vTaskDelay(500 / portTICK_RATE_MS);
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdServiceDiscovery, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdServiceDiscovery, &pCache->stData.stServiceDiscoveryRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stServiceDiscoveryRes.ResponseCode);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:PaymentOptionList 字段 ———— 提供 PaymentOptionList    字段Set接口,由用户层设置该字段
    //sV2gIso1Set_PaymentOptionList
    
    //4.3:ChargeService     字段 ———— 提供 ChargeService        字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ChargeService
    
    //4.4:ServiceList       字段 ———— 提供 ServiceList          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ServiceList
    
    
    memcpy(pRes, &pCache->stData.stServiceDiscoveryRes, sizeof(pCache->stData.stServiceDiscoveryRes));
    
    
    //若ResponseCode不为iso1responseCodeType_OK,则ServiceList_isUsed不可启用
    if(pRes->ResponseCode != iso1responseCodeType_OK)
    {
        pRes->ServiceList_isUsed = 0;
    }
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintServiceDiscoveryRes(i32DevIndex, pDocOut);
    
    
    return 0;
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1ServiceDetail
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  21:21:21
 * @Description    :      ISO15118 ServiceDetail 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1ServiceDetail(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1ServiceDetailReqType *pReq = NULL;
    struct iso1ServiceDetailResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintServiceDetailReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdServiceDetail);
    pReq = &pDocIn->V2G_Message.Body.ServiceDetailReq;
    pRes = &pDocOut->V2G_Message.Body.ServiceDetailRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stServiceDetailReq, pReq, sizeof(pCache->stData.stServiceDetailReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdServiceDetail, iso1responseCodeType_OK);
    
    //[V2G2-425] —— SECC 无法提供对应的服务 ID
    //sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdServiceDetail, iso1responseCodeType_FAILED_ServiceIDInvalid);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdServiceDetail, &pCache->stData.stServiceDetailRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stServiceDetailRes.ResponseCode);
    sV2gIso1ServiceIdCheck(i32DevIndex, pCache, &pCache->stData.stServiceDetailRes.ResponseCode, pReq->ServiceID);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:ServiceID 字段：
    pCache->stData.stServiceDetailRes.ServiceID = pReq->ServiceID;
    
    //4.3:ServiceParameterList 字段 ———— 提供 ServiceParameterList 字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ServiceParameterList
    
    memcpy(pRes, &pCache->stData.stServiceDetailRes, sizeof(pCache->stData.stServiceDetailRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintServiceDetailRes(i32DevIndex, pDocOut);
    
    
    return 0;
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1PaymentServiceSelection
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月13日  21:10:15
 * @Description    :      ISO15118 PaymentServiceSelection 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1PaymentServiceSelection(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1PaymentServiceSelectionReqType *pReq = NULL;
    struct iso1PaymentServiceSelectionResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintPaymentServiceSelectionReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdPaymentServiceSelection);
    pReq = &pDocIn->V2G_Message.Body.PaymentServiceSelectionReq;
    pRes = &pDocOut->V2G_Message.Body.PaymentServiceSelectionRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPaymentServiceSelectionReq, pReq, sizeof(pCache->stData.stPaymentServiceSelectionReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPaymentServiceSelection, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdPaymentServiceSelection, &pCache->stData.stPaymentServiceSelectionRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPaymentServiceSelectionRes.ResponseCode);
    sV2gIso1ProcSelectedPaymentOption(i32DevIndex, pCache, &pCache->stData.stPaymentServiceSelectionRes.ResponseCode, pReq->SelectedPaymentOption);
    sV2gIso1ProcSelectedServiceList(i32DevIndex, pCache, &pCache->stData.stPaymentServiceSelectionRes.ResponseCode, &pReq->SelectedServiceList);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    memcpy(pRes, &pCache->stData.stPaymentServiceSelectionRes, sizeof(pCache->stData.stPaymentServiceSelectionRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintPaymentServiceSelectionRes(i32DevIndex, pDocOut);
    
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1CertificateInstallation
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  10:16:58
 * @Description    :      ISO15118 CertificateInstallation 消息处理(PnC模式)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1CertificateInstallation(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1CertificateInstallationReqType *pReq = NULL;
    struct iso1CertificateInstallationResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintCertificateInstallationReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdCertificateInstallation);
    pReq = &pDocIn->V2G_Message.Body.CertificateInstallationReq;
    pRes = &pDocOut->V2G_Message.Body.CertificateInstallationRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCertificateInstallationReq, pReq, sizeof(pCache->stData.stCertificateInstallationReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCertificateInstallation, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdCertificateInstallation, &pCache->stData.stCertificateInstallationRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCertificateInstallationRes.ResponseCode);
    
    
    //有关[V2G2-468] FAILED_CertificateExpired的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-469] FAILED_NoCertificateAvailable的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-694] FAILED_CertificateRevoked的逻辑处理可能得由应用层处理赋值
    //可能得设计一逻辑等待应用层释放的信号量 —————— 等待应用层处理完证书填充完字段释放信号量
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:SAProvisioningCertificateChain        字段  ———— 提供 SAProvisioningCertificateChain          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_SAProvisioningCertificateChain
    
    //4.3:ContractSignatureCertChain            字段  ———— 提供 ContractSignatureCertChain              字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ContractSignatureCertChain
    
    //4.4:ContractSignatureEncryptedPrivateKey  字段  ———— 提供 ContractSignatureEncryptedPrivateKey    字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ContractSignatureEncryptedPrivateKey
    
    //4.5:DHpublickey                           字段  ———— 提供 DHpublickey                             字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DHpublickey
    
    //4.6:eMAID                                 字段  ———— 提供 eMAID                                   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_eMAID
    
    memcpy(pRes, &pCache->stData.stCertificateInstallationRes, sizeof(pCache->stData.stCertificateInstallationRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintCertificateInstallationRes(i32DevIndex, pDocOut);
    
    
    return 0;
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1CertificateUpdate
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  10:24:20
 * @Description    :      ISO15118 CertificateUpdate 消息处理(PnC模式)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1CertificateUpdate(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1CertificateUpdateReqType *pReq = NULL;
    struct iso1CertificateUpdateResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintCertificateUpdateReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdCertificateUpdate);
    pReq = &pDocIn->V2G_Message.Body.CertificateUpdateReq;
    pRes = &pDocOut->V2G_Message.Body.CertificateUpdateRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCertificateUpdateReq, pReq, sizeof(pCache->stData.stCertificateUpdateReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCertificateUpdate, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdCertificateUpdate, &pCache->stData.stCertificateUpdateRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCertificateUpdateRes.ResponseCode);
    
    
    //有关[V2G2-470] FAILED_CertChainError的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-471] FAILED_NoCertificateAvailable的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-472] FAILED_ContractCanceled的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-473] FAILED_CertificateExpired的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-695] FAILED_CertificateRevoked的逻辑处理可能得由应用层处理赋值
    //可能得设计一逻辑等待应用层释放的信号量 —————— 等待应用层处理完证书填充完字段释放信号量
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:SAProvisioningCertificateChain        字段  ———— 提供 SAProvisioningCertificateChain          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_SAProvisioningCertificateChain
    
    //4.3:ContractSignatureCertChain            字段  ———— 提供 ContractSignatureCertChain              字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ContractSignatureCertChain
    
    //4.4:ContractSignatureEncryptedPrivateKey  字段  ———— 提供 ContractSignatureEncryptedPrivateKey    字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ContractSignatureEncryptedPrivateKey
    
    //4.5:DHpublickey                           字段  ———— 提供 DHpublickey                             字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DHpublickey
    
    //4.6:eMAID                                 字段  ———— 提供 eMAID                                   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_eMAID
    
    //4.7:RetryCounter                          字段  ———— 提供 RetryCounter                            字段Set接口,由用户层设置该字段
    //sV2gIso1Set_RetryCounter
    
    memcpy(pRes, &pCache->stData.stCertificateUpdateRes, sizeof(pCache->stData.stCertificateUpdateRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintCertificateUpdateRes(i32DevIndex, pDocOut);
    
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1PaymentDetails
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  10:52:02
 * @Description    :      ISO15118 PaymentDetails 消息处理(PnC模式)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1PaymentDetails(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1PaymentDetailsReqType *pReq = NULL;
    struct iso1PaymentDetailsResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintPaymentDetailsReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdPaymentDetails);
    pReq = &pDocIn->V2G_Message.Body.PaymentDetailsReq;
    pRes = &pDocOut->V2G_Message.Body.PaymentDetailsRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPaymentDetailsReq, pReq, sizeof(pCache->stData.stPaymentDetailsReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPaymentDetails, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdPaymentDetails, &pCache->stData.stPaymentDetailsRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPaymentDetailsRes.ResponseCode);
    
    
    
    //有关[V2G2-474] FAILED_CertificateExpired的逻辑处理可能得由应用层处理赋值
    //有关[V2G2-824] FAILED_CertificateExpired的逻辑处理可能得由应用层处理赋值
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:GenChallenge  字段  ———— 提供 GenChallenge 字段Set接口,由用户层设置该字段
    //sV2gIso1Set_GenChallenge
    
    //4.3:EVSETimeStamp 字段  ———— 比如时间戳这一类字段,是需要在这里就地赋值的 避免时差
    pCache->stData.stPaymentDetailsRes.EVSETimeStamp = sGetTimestamp();
    
    memcpy(pRes, &pCache->stData.stPaymentDetailsRes, sizeof(pCache->stData.stPaymentDetailsRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintPaymentDetailsRes(i32DevIndex, pDocOut);
    
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1Authorization
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  13:49:51
 * @Description    :      ISO15118 Authorization 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1Authorization(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1AuthorizationReqType *pReq = NULL;
    struct iso1AuthorizationResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 10)
    || (pCache->stData.stAuthorizationRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        sV2gIso1PrintAuthorizationReq(i32DevIndex, pDocIn);
    }
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdAuthorization);
    pRes = &pDocOut->V2G_Message.Body.AuthorizationRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stAuthorizationReq, pReq, sizeof(pCache->stData.stAuthorizationReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdAuthorization, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdAuthorization, &pCache->stData.stAuthorizationRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stAuthorizationRes.ResponseCode);
    if(pReq->GenChallenge_isUsed == 1)
    {
        sV2gIso1ProcGenChallenge(i32DevIndex, pCache, &pCache->stData.stAuthorizationRes.ResponseCode, pReq->GenChallenge.bytesLen, pReq->GenChallenge.bytes);
    }
    
    //关于[V2G2-693] FAILED_CertificateRevoked的逻辑处理还不是很懂怎么去判断
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:EVSEProcessing 字段 ———— 提供 EVSEProcessing 字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEProcessing
    
    memcpy(pRes, &pCache->stData.stAuthorizationRes, sizeof(pCache->stData.stAuthorizationRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 10)
    || (pCache->stData.stAuthorizationRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintAuthorizationRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ChargeParameterDiscovery
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  15:32:28
 * @Description    :      ISO15118 ChargeParameterDiscovery 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1ChargeParameterDiscovery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1ChargeParameterDiscoveryReqType *pReq = NULL;
    struct iso1ChargeParameterDiscoveryResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        sV2gIso1PrintChargeParameterDiscoveryReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdChargeParameterDiscovery);
    pReq = &pDocIn->V2G_Message.Body.ChargeParameterDiscoveryReq;
    pRes = &pDocOut->V2G_Message.Body.ChargeParameterDiscoveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stChargeParamDiscoveryReq, pReq, sizeof(pCache->stData.stChargeParamDiscoveryReq));
    sV2gIso1ProcPublicDataChargeParameterDiscovery(pCache, pReq);
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdChargeParameterDiscovery, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdChargeParameterDiscovery, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode);
    
    sV2gIso1ProcRequestedEnergyTransferMode(i32DevIndex, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode, pReq->RequestedEnergyTransferMode);
    
    if((pReq->RequestedEnergyTransferMode <= iso1EnergyTransferModeType_AC_three_phase_core) && (pReq->AC_EVChargeParameter_isUsed == 1))
    {
        sV2gIso1ProcAcEvChargeParameter(i32DevIndex, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode, &pReq->AC_EVChargeParameter);
    }
    
    if((pReq->RequestedEnergyTransferMode >= iso1EnergyTransferModeType_DC_core) && (pReq->DC_EVChargeParameter_isUsed == 1))
    {
        sV2gIso1ProcDcEvChargeParameter(i32DevIndex, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode, &pReq->DC_EVChargeParameter);
    }
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:EVSEProcessing            字段  ———— 提供 EVSEProcessing          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEProcessing
    
    //4.3:SAScheduleList            字段  ———— 提供 SAScheduleList          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_SAScheduleList
    
    //4.4:AC_EVSEChargeParameter    字段  ———— 提供 AC_EVSEChargeParameter  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_AC_EVSEStatus
    //sV2gIso1Set_EVSENominalVoltage
    //sV2gIso1Set_EVSEMaxCurrent
    
    //4.4:DC_EVSEChargeParameter    字段  ———— 提供 DC_EVSEChargeParameter  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    //sV2gIso1Set_EVSEMaximumCurrentLimit
    //sV2gIso1Set_EVSEMaximumPowerLimit
    //sV2gIso1Set_EVSEMaximumVoltageLimit
    //sV2gIso1Set_EVSEMinimumCurrentLimit
    //sV2gIso1Set_EVSEMinimumVoltageLimit
    //sV2gIso1Set_EVSECurrentRegulationTolerance
    //sV2gIso1Set_EVSEPeakCurrentRipple
    //sV2gIso1Set_EVSEEnergyToBeDelivered
    
    memcpy(pRes, &pCache->stData.stChargeParamDiscoveryRes, sizeof(pCache->stData.stChargeParamDiscoveryRes));
    
    //若ResponseCode不为iso1responseCodeType_OK,则SAScheduleList_isUsed不可启用
    if(pRes->ResponseCode != iso1responseCodeType_OK)
    {
        pRes->SAScheduleList_isUsed = 0;
    }
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintChargeParameterDiscoveryRes(i32DevIndex, pDocOut);
    }
    
    
    //6：启动CP状态检测, 根据<DIN SPEC 70121 2014-12 212P.pdf>@[V2G-DC-547] 条款
    if((pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode >= iso1EnergyTransferModeType_DC_core)
    && (pRes->ResponseCode == iso1responseCodeType_OK)
    && (pRes->EVSEProcessing == iso1EVSEProcessingType_Finished)
    && (pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEStatusCode == iso1DC_EVSEStatusCodeType_EVSE_Ready))
    {
        //期望的cp状态填写 eChgCpStateC 就可以了
        //时序逻辑会同时将 eChgCpStateC/eChgCpStateD 设定为期望值的
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCableCheck, iso1responseCodeType_OK);
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateC);
    }
    
    
    return 0;
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1CableCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  16:17:01
 * @Description    :      ISO15118 CableCheck 消息处理(DC专用)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1CableCheck(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1CableCheckReqType *pReq = NULL;
    struct iso1CableCheckResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stCableCheckRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        sV2gIso1PrintCableCheckReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdCableCheck);
    pReq = &pDocIn->V2G_Message.Body.CableCheckReq;
    pRes = &pDocOut->V2G_Message.Body.CableCheckRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCableCheckReq, pReq, sizeof(pCache->stData.stCableCheckReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
    
    //ResponseCode赋初值
    //<ISO15118-2:2014.pdf> @8.7.4.4 [V2G2-918]
    /*
    vTaskDelay(100 / portTICK_RATE_MS);
    if((sAdcAppSampleGetCpState(i32DevIndex) == eChgCpStateC) || (sAdcAppSampleGetCpState(i32DevIndex) == eChgCpStateD))
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCableCheck, iso1responseCodeType_OK);
    }
    else
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCableCheck, iso1responseCodeType_FAILED);
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    */
    
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdCableCheck, &pCache->stData.stCableCheckRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCableCheckRes.ResponseCode);
    
    //DC_EVStatus           字段  ———— 提供 DC_EVStatus     字段Get接口,由用户层获取并检查该字段
    //sV2gIso1Get_DC_EVStatus
    
    vTaskDelay(1500 / portTICK_RATE_MS);
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:DC_EVSEStatus     字段  ———— 提供 DC_EVSEStatus   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    //4.3:EVSEProcessing    字段  ———— 提供 EVSEProcessing  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEProcessing
    
    memcpy(pRes, &pCache->stData.stCableCheckRes, sizeof(pCache->stData.stCableCheckRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stCableCheckRes.EVSEProcessing == iso1EVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintCableCheckRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1PreCharge
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  16:32:59
 * @Description    :      ISO15118 PreCharge 消息处理(DC专用)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1PreCharge(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1PreChargeReqType *pReq = NULL;
    struct iso1PreChargeResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        sV2gIso1PrintPreChargeReq(i32DevIndex, pDocIn);
    }
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdPreCharge);
    pReq = &pDocIn->V2G_Message.Body.PreChargeReq;
    pRes = &pDocOut->V2G_Message.Body.PreChargeRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPreChargeReq, pReq, sizeof(pCache->stData.stPreChargeReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
    memcpy(&pCache->stDataPublic.EVTargetVoltage, &pReq->EVTargetVoltage, sizeof(struct iso1PhysicalValueType));
    memcpy(&pCache->stDataPublic.EVTargetCurrent, &pReq->EVTargetCurrent, sizeof(struct iso1PhysicalValueType));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPreCharge, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdPreCharge, &pCache->stData.stPreChargeRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPreChargeRes.ResponseCode);
    
    //DC_EVStatus       字段  ———— 提供 DC_EVStatus     字段Get接口,由用户层获取并检查该字段
    //sV2gIso1Get_DC_EVStatus
    
    //EVTargetVoltage   字段  ———— 提供 EVTargetVoltage 字段Get接口,由用户层获取并检查该字段
    //sV2gIso1Get_EVTargetVoltage
    
    //EVTargetCurrent   字段  ———— 提供 EVTargetCurrent 字段Get接口,由用户层获取并检查该字段
    //sV2gIso1Get_EVTargetCurrent
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:DC_EVSEStatus         字段  ———— 提供 DC_EVSEStatus       字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    //4.3:EVSEPresentVoltage    字段  ———— 提供 EVSEPresentVoltage  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEPresentVoltage
    
    memcpy(pRes, &pCache->stData.stPreChargeRes, sizeof(pCache->stData.stPreChargeRes));
    
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintPreChargeRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1PowerDelivery
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  17:27:53
 * @Description    :      ISO15118 PowerDelivery 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1PowerDelivery(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    eV2gIso1MsgId_t eMsgId;
    struct iso1PowerDeliveryReqType *pReq = NULL;
    struct iso1PowerDeliveryResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPowerDeliveryStart, iso1responseCodeType_OK);
    
    // AC专用时序
    if((pCache->eId == eV2gIso1MsgIdChargeParameterDiscovery)
    && (pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode <= iso1EnergyTransferModeType_AC_three_phase_core))
    {
        //期望的cp状态填写 eChgCpStateC 就可以了
        //时序逻辑会同时将 eChgCpStateC/eChgCpStateD 设定为期望值的
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateC);
    }
    
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintPowerDeliveryReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdPowerDeliveryStart);
    pReq = &pDocIn->V2G_Message.Body.PowerDeliveryReq;
    pRes = &pDocOut->V2G_Message.Body.PowerDeliveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPowerDeliveryReq, pReq, sizeof(pCache->stData.stPowerDeliveryReq));
    sV2gIso1ProcPublicDataPowerDelivery(pCache, pReq);
    
    
    //ISO15118-2 —— [V2G2-459]
    switch(pReq->ChargeProgress)
    {
        case iso1chargeProgressType_Start:                          //启动
            eMsgId = eV2gIso1MsgIdPowerDeliveryStart;
            break;
            
        case iso1chargeProgressType_Stop:                           //停止
            eMsgId = eV2gIso1MsgIdPowerDeliveryStop;
            //ResponseCode赋初值
            sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPowerDeliveryStop, iso1responseCodeType_OK);
            break;
            
        case iso1chargeProgressType_Renegotiate:                    //重新协商
            eMsgId = eV2gIso1MsgIdPowerDeliveryRenegotiate;
            //ResponseCode赋初值
            sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPowerDeliveryRenegotiate, iso1responseCodeType_OK);
            
            //<ISO15118-2:2014.pdf> @8.8.4.3.1 [V2G2-812]
            if(((pCache->eId == eV2gIso1MsgIdPreCharge) && (pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode >= iso1EnergyTransferModeType_DC_core))
            || ((pCache->eId == eV2gIso1MsgIdChargeParameterDiscovery) && (pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode <= iso1EnergyTransferModeType_AC_three_phase_core)))
            {
                sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdPowerDeliveryRenegotiate, iso1responseCodeType_FAILED);
                pCache->pCbShutdownEmg(i32DevIndex);
            }
            break;
            
        default:
            break;
    }
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eMsgId, &pCache->stData.stPowerDeliveryRes.ResponseCode);
    
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode);
    sV2gIso1SAScheduleTupleIDCheck(i32DevIndex, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode, pReq->SAScheduleTupleID);
    sV2gIso1ChargingProfileCheck(i32DevIndex, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode, &pReq->ChargingProfile);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:AC_EVSEStatus     字段  ———— 提供 AC_EVSEStatus   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_AC_EVSEStatus
    
    //4.2:DC_EVSEStatus     字段  ———— 提供 DC_EVSEStatus   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    memcpy(pRes, &pCache->stData.stPowerDeliveryRes, sizeof(pCache->stData.stPowerDeliveryRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintPowerDeliveryRes(i32DevIndex, pDocOut);
    
    
    //6：启动CP状态检测, 根据<DIN SPEC 70121 2014-12 212P.pdf>@[V2G-DC-556] 条款
    if(pReq->ChargeProgress == iso1chargeProgressType_Stop)
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionStop,      iso1responseCodeType_OK);
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_OK);
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateB);
    }
    
    
    return 0;
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1ChargingStatus
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  20:24:47
 * @Description    :      ISO15118 ChargingStatus 消息处理(AC专用)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1ChargingStatus(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1ChargingStatusResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        sV2gIso1PrintChargingStatusReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdChargingStatus);
    pRes = &pDocOut->V2G_Message.Body.ChargingStatusRes;
    
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdChargingStatus, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdChargingStatus, &pCache->stData.stChargingStatusRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stChargingStatusRes.ResponseCode);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:EVSEID            字段  ———— 提供 EVSEID          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEID
    
    //4.3:EVSEMaxCurrent    字段  ———— 提供 EVSEMaxCurrent  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEMaxCurrent
    
    //4.4:MeterInfo         字段  ———— 提供 MeterInfo       字段Set接口,由用户层设置该字段
    //sV2gIso1Set_MeterInfo
    
    //4.5:ReceptRequired    字段  ———— 提供 ReceptRequired  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ReceptRequired
    
    //4.6:AC_EVSEStatus     字段  ———— 提供 AC_EVSEStatus   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_AC_EVSEStatus
    
    pCache->stData.stChargingStatusRes.SAScheduleTupleID = pCache->stData.stPowerDeliveryReq.SAScheduleTupleID;
    
    memcpy(pRes, &pCache->stData.stChargingStatusRes, sizeof(pCache->stData.stChargingStatusRes));
    
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintChargingStatusRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}















/*******************************************************************************
 * @FunctionName   :      sV2gIso1CurrentDemand
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  17:37:06
 * @Description    :      ISO15118 CurrentDemand 消息处理(DC专用)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1CurrentDemand(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1CurrentDemandReqType *pReq = NULL;
    struct iso1CurrentDemandResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        sV2gIso1PrintCurrentDemandReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdCurrentDemand);
    pReq = &pDocIn->V2G_Message.Body.CurrentDemandReq;
    pRes = &pDocOut->V2G_Message.Body.CurrentDemandRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCurrentDemandReq, pReq, sizeof(pCache->stData.stCurrentDemandReq));
    sV2gIso1ProcPublicDataCurrentDemand(pCache, pReq);
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdCurrentDemand, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdCurrentDemand, &pCache->stData.stCurrentDemandRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCurrentDemandRes.ResponseCode);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:DC_EVSEStatus                     字段  ———— 提供 DC_EVSEStatus                   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    //4.3:EVSEPresentVoltage                字段  ———— 提供 EVSEPresentVoltage              字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEPresentVoltage
    
    //4.4:EVSEPresentCurrent                字段  ———— 提供 EVSEPresentCurrent              字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEPresentCurrent
    
    //4.5:EVSEVoltageLimitAchieved          字段  ———— 提供 EVSEVoltageLimitAchieved        字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEVoltageLimitAchieved
    
    //4.6:EVSECurrentLimitAchieved          字段  ———— 提供 EVSECurrentLimitAchieved        字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSECurrentLimitAchieved
    
    //4.7:EVSEPowerLimitAchieved            字段  ———— 提供 EVSEPowerLimitAchieved          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEPowerLimitAchieved
    
    //4.8:EVSEMaximumVoltageLimit           字段  ———— 提供 EVSEMaximumVoltageLimit         字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEMaximumVoltageLimit
    
    //4.9:EVSEMaximumCurrentLimit           字段  ———— 提供 EVSEMaximumCurrentLimit         字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEMaximumCurrentLimit
    
    //4.10:EVSEMaximumPowerLimit            字段  ———— 提供 EVSEMaximumPowerLimit           字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEMaximumPowerLimit
    
    //4.11:EVSEID                           字段  ———— 提供 EVSEID                          字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEID
    
    //4.12:MeterInfo                        字段  ———— 提供 MeterInfo                       字段Set接口,由用户层设置该字段
    //sV2gIso1Set_MeterInfo
    
    //4.13:ReceptRequired                   字段  ———— 提供 ReceptRequired                  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_ReceptRequired
    
    pCache->stData.stCurrentDemandRes.SAScheduleTupleID = pCache->stData.stPowerDeliveryReq.SAScheduleTupleID;
    
    memcpy(pRes, &pCache->stData.stCurrentDemandRes, sizeof(pCache->stData.stCurrentDemandRes));
    
    //若ResponseCode不为iso1responseCodeType_OK,
    //则EVSEMaximumVoltageLimit_isUsed、EVSEMaximumCurrentLimit_isUsed、EVSEMaximumPowerLimit_isUsed不可启用
    if(pRes->ResponseCode != iso1responseCodeType_OK)
    {
        pRes->EVSEMaximumVoltageLimit_isUsed = 0;
        pRes->EVSEMaximumCurrentLimit_isUsed = 0;
        pRes->EVSEMaximumPowerLimit_isUsed = 0;
    }
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintCurrentDemandRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1MeteringReceipt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  20:32:05
 * @Description    :      ISO15118 MeteringReceipt 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1MeteringReceipt(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1MeteringReceiptReqType *pReq = NULL;
    struct iso1MeteringReceiptResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        sV2gIso1PrintMeteringReceiptReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdMeteringReceipt);
    pReq = &pDocIn->V2G_Message.Body.MeteringReceiptReq;
    pRes = &pDocOut->V2G_Message.Body.MeteringReceiptRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stMeteringReceiptReq, pReq, sizeof(pCache->stData.stMeteringReceiptReq));
    
    //ResponseCode赋初值
    sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdMeteringReceipt, iso1responseCodeType_OK);
    
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdMeteringReceipt, &pCache->stData.stMeteringReceiptRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stMeteringReceiptRes.ResponseCode);
    
    if(pReq->Id_isUsed == 1)
    {
        //[V2G2-481] —— “SECC 无法验证签名” 具体是得判断什么条件,还有待后续调试验证
        sV2gIso1ContractIdCheck(i32DevIndex, pCache, &pCache->stData.stMeteringReceiptRes.ResponseCode, pReq);
    }
    if(pCache->stData.stChargeParamDiscoveryReq.RequestedEnergyTransferMode <= iso1EnergyTransferModeType_AC_three_phase_core)
    {
        sV2gIso1MeterInfoCheck(i32DevIndex, pCache, &pCache->stData.stMeteringReceiptRes.ResponseCode, &pReq->MeterInfo);
    }
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:AC_EVSEStatus                     字段  ———— 提供 AC_EVSEStatus                   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_AC_EVSEStatus
    
    //4.2:DC_EVSEStatus                     字段  ———— 提供 DC_EVSEStatus                   字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    memcpy(pRes, &pCache->stData.stMeteringReceiptRes, sizeof(pCache->stData.stMeteringReceiptRes));
    
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintMeteringReceiptRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1WeldingDetection
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  20:01:07
 * @Description    :      ISO15118 WeldingDetection 消息处理(DC专用)
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1WeldingDetection(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct iso1WeldingDetectionReqType *pReq = NULL;
    struct iso1WeldingDetectionResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        sV2gIso1PrintWeldingDetectionReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdWeldingDetection);
    pReq = &pDocIn->V2G_Message.Body.WeldingDetectionReq;
    pRes = &pDocOut->V2G_Message.Body.WeldingDetectionRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stWeldingDetectionReq, pReq, sizeof(pCache->stData.stWeldingDetectionReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
    
    //ResponseCode赋初值
    //<ISO15118-2:2014.pdf> @8.7.4.4 [V2G2-922]
    /*
    vTaskDelay(100 / portTICK_RATE_MS);
    if(sAdcAppSampleGetCpState(i32DevIndex) == eChgCpStateB)
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_OK);
    }
    else
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdWeldingDetection, iso1responseCodeType_FAILED);
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    */
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdWeldingDetection, &pCache->stData.stWeldingDetectionRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stWeldingDetectionRes.ResponseCode);
    
    
    //由 PowerDelivery 停充消息触发的CP检测(超时)结果
    //如果由本消息负责回复该结果 那就要清零 eV2gDinMsgIdSessionStop 消息中被设置的 FAILED 值
    //sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionStop, iso1responseCodeType_OK);
    
    vTaskDelay(1500 / portTICK_RATE_MS);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:DC_EVSEStatus         字段  ———— 提供 DC_EVSEStatus       字段Set接口,由用户层设置该字段
    //sV2gIso1Set_DC_EVSEStatus
    
    //4.3:EVSEPresentVoltage    字段  ———— 提供 EVSEPresentVoltage  字段Set接口,由用户层设置该字段
    //sV2gIso1Set_EVSEPresentVoltage
    
    memcpy(pRes, &pCache->stData.stWeldingDetectionRes, sizeof(pCache->stData.stWeldingDetectionRes));
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gIso1PrintWeldingDetectionRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1SessionStop
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月14日  20:16:34
 * @Description    :      ISO15118 SessionStop 消息处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pDocIn            指向要解析的EXI文档结构体指针
 * @Input          :      pDocOut           指向储存解析完的EXI文档结构体指针
 * @Return         :      
*******************************************************************************/
i32 sV2gIso1SessionStop(i32 i32DevIndex, struct iso1EXIDocument *pDocIn, struct iso1EXIDocument *pDocOut)
{
    struct iso1SessionStopReqType *pReq = NULL;
    struct iso1SessionStopResType *pRes = NULL;
    
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    sV2gIso1PrintSessionStopReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gIso1DocInit(pDocOut, eV2gIso1MsgIdSessionStop);
    pRes = &pDocOut->V2G_Message.Body.SessionStopRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stSessionStopReq, pReq, sizeof(pCache->stData.stSessionStopReq));
    
    //ResponseCode赋初值
    //<ISO15118-2:2014.pdf> @8.7.4.4 [V2G2-922]
    /*
    vTaskDelay(100 / portTICK_RATE_MS);
    if(sAdcAppSampleGetCpState(i32DevIndex) == eChgCpStateB)
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionStop, iso1responseCodeType_OK);
    }
    else
    {
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionStop, iso1responseCodeType_FAILED);
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    */
    //ISO15118-2 —— [V2G2-459]
    sV2gIso1MsgSequenceErrorCheck(i32DevIndex, eV2gIso1MsgIdSessionStop, &pCache->stData.stSessionStopRes.ResponseCode);
    
    //字段检查
    sV2gIso1CompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stSessionStopRes.ResponseCode);
    
    vTaskDelay(1500 / portTICK_RATE_MS);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gIso1FillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stSessionStopRes, sizeof(pCache->stData.stSessionStopRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gIso1PrintSessionStopRes(i32DevIndex, pDocOut);
    
    
    return 0;
}















