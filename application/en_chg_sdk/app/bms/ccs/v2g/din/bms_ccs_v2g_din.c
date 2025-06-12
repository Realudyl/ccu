/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_din.c
* Description                           :   DIN70121 协议实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_v2g_din.h"
#include "bms_ccs_v2g_din_api.h"
#include "bms_ccs_v2g_din_basic.h"
#include "bms_ccs_v2g_din_err.h"
#include "bms_ccs_v2g_din_print.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_din";









stV2gDinCache_t *pV2gDinCache[cMse102xDevNum];




//---------------------------------------------------------------------------------------------------------




bool  sV2gDinInit(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex));
bool  sV2gDinCacheInit(i32 i32DevIndex);

bool  sV2gDinSetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
bool  sV2gDinGetSessionStop(i32 i32DevIndex);
bool  sV2gDinSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);






//V2G DIN70121 消息处理---------------------------------------------------------
//V2G DIN70121 EXI 编解码
i32   sV2gDinEncodeExiDoc(bitstream_t *pStream, void *pExiDoc);
i32   sV2gDinDecodeExiDoc(bitstream_t *pStream, void *pExiDoc);

//V2G DIN70121 消息处理
i32   sV2gDinProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen);



//V2G DIN70121 MsgId 搜索
i32   sV2gDinMsgIdSearch(struct dinEXIDocument* pDocIn);

//V2G DIN70121 dinEXIDocument 文档初始化
bool  sV2gDinDocInit(struct dinEXIDocument *pDoc, eV2gDinMsgId_t eMsgId);

//V2G DIN70121 接收消息数据解析及回复报文数据填充
i32   sV2gDinParse(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);


//V2G DIN70121 分类消息处理
i32   sV2gDinSessionSetup(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinServiceDiscovery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinServicePaymentSelection(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinContractAuthentication(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinChargeParameterDiscovery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinCableCheck(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinPreCharge(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinPowerDelivery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinCurrentDemand(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinWeldingDetection(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);
i32   sV2gDinSessionStop(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut);





//DIN70121消息 映射map
stV2gDinMsgIdMap_t stV2gDinMsgIdMap[] =
{
    //确保 eId 的连续性  以便程序可以直接通过 eId 去调用 pProc 函数
    {eV2gDinMsgIdSupportedAppProtocol,      NULL                                },
    {eV2gDinMsgIdSessionSetup,              sV2gDinSessionSetup                 },
    {eV2gDinMsgIdServiceDiscovery,          sV2gDinServiceDiscovery             },
    {eV2gDinMsgIdServicePaymentSelection,   sV2gDinServicePaymentSelection      },
    {eV2gDinMsgIdContractAuthorization,     sV2gDinContractAuthentication       },
    {eV2gDinMsgIdChargeParameterDiscovery,  sV2gDinChargeParameterDiscovery     },
    {eV2gDinMsgIdCableCheck,                sV2gDinCableCheck                   },
    {eV2gDinMsgIdPreCharge,                 sV2gDinPreCharge                    },
    {eV2gDinMsgIdPowerDelivery1,            sV2gDinPowerDelivery                },
    {eV2gDinMsgIdCurrentDemand,             sV2gDinCurrentDemand                },
    {eV2gDinMsgIdPowerDelivery2,            sV2gDinPowerDelivery                },
    {eV2gDinMsgIdWeldingDetection,          sV2gDinWeldingDetection             },
    {eV2gDinMsgIdSessionStop,               sV2gDinSessionStop                  },
    
};






















/***************************************************************************************************
* Description                           :   DIN70121 协议初始化
* Author                                :   Hall
* Creat Date                            :   2023-05-06
* notice                                :   每次 supportedAppProtocol 消息 协议协商OK后调用本函数
****************************************************************************************************/
bool sV2gDinInit(i32 i32DevIndex, void (*pCb0)(i32 i32DevIndex), void (*pCb1)(i32 i32DevIndex), void (*pCb2)(i32 i32DevIndex), void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState), bool (*pCb4)(i32 i32DevIndex))
{
    stV2gDinCache_t *pCache = NULL;
    
    
    EN_SLOGI(TAG, "DIN70121(通道:%d) 协议初始化", i32DevIndex);
    
    if(pV2gDinCache[i32DevIndex] == NULL)                                       //首次握手才会执行以下逻辑
    {
        pV2gDinCache[i32DevIndex] = (stV2gDinCache_t *)MALLOC(sizeof(stV2gDinCache_t));
        pV2gDinCache[i32DevIndex]->xSemHandshake = xSemaphoreCreateBinary();
    }
    
    
    //缓存数据初始化
    pCache = pV2gDinCache[i32DevIndex];
    pCache->pCbSwitchV2gToShutdownEmg = pCb0;
    pCache->pCbSeqTimerRst = pCb1;
    pCache->pCbSeqTimerStart = pCb2;
    pCache->pCbCpStateDetectionTimerStart = pCb3;
    pCache->pCbSetTcpConnClose = pCb4;
    
    xSemaphoreGive(pCache->xSemHandshake);                                      //发出握手信号
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sV2gDinCacheInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  15:14:38
 * @Description    :      DIN70121 相关缓存数据初始化
 * @Input          :      i32DevIndex     外设号
 * @Return         :      
*******************************************************************************/
bool sV2gDinCacheInit(i32 i32DevIndex)
{
    //缓存数据初始化
    memset( pV2gDinCache[i32DevIndex]->u8SessionId,  0, sizeof(pV2gDinCache[i32DevIndex]->u8SessionId ));
    memset(&pV2gDinCache[i32DevIndex]->stData,       0, sizeof(pV2gDinCache[i32DevIndex]->stData      ));
    memset(&pV2gDinCache[i32DevIndex]->stDataPublic, 0, sizeof(pV2gDinCache[i32DevIndex]->stDataPublic));
    pV2gDinCache[i32DevIndex]->eId = eV2gDinMsgIdSupportedAppProtocol;
    
    return(true);
}








/***************************************************************************************************
* Description                           :   DIN70121 CP 状态设置
* Author                                :   Hall
* Creat Date                            :   2024-02-03
* notice                                :   
****************************************************************************************************/
bool sV2gDinSetCpState(i32 i32DevIndex, eChgCpState_t eCpState)
{
    return sV2gDinCpStateErrorCheck(i32DevIndex, eCpState);
}







/***************************************************************************************************
* Description                           :   DIN70121 SessionStop 会话查询
* Author                                :   Hall
* Creat Date                            :   2023-06-21
* notice                                :   
****************************************************************************************************/
bool sV2gDinGetSessionStop(i32 i32DevIndex)
{
    return (pV2gDinCache[i32DevIndex]->eId == eV2gDinMsgIdSessionStop) ? true : false;
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 V2G_SECC_CPState_Detection_Timeout 超时结果填充
* Author                                    Hall
* Creat Date                            :   2024-02-02
* notice                                :   外层的时序逻辑中 cp状态超时后 将相关消息的错误码 填充为 FAILED
*                                           参考 [V2G-DC-547]--->第2条 & [V2G-DC-556]--->第2条
****************************************************************************************************/
bool sV2gDinSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState)
{
    if((eCpState == eChgCpStateC) || (eCpState == eChgCpStateD))
    {
        //状态C/D 是由 ChargeParameterDiscovery 消息发起的CP状态检查 它的下一条消息就是 eV2gDinMsgIdCableCheck
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_FAILED);
    }
    else if(eCpState == eChgCpStateB)
    {
        //状态B 是由 ReadyToChargeState=0 的 PowerDelivery 消息发起的CP状态检查
        //它的下一条消息可以是以下两种
        //这里先全部设置 FAILED 
        //如果实际的报文传输中有 eV2gDinMsgIdWeldingDetection 消息，那就由该消息处理函数将 eV2gDinMsgIdSessionStop 的 ResponseCode 清零
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionStop,      dinresponseCodeType_FAILED);
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdWeldingDetection, dinresponseCodeType_FAILED);
    }
    
    return(true);
}










/***************************************************************************************************
* Description                           :   DIN70121 编码API
* Author                                :   Hall
* Creat Date                            :   2022-07-05
* notice                                :   
****************************************************************************************************/
i32 sV2gDinEncodeExiDoc(bitstream_t *pStream, void *pExiDoc)
{
    i32 errn;
    struct dinEXIDocument *pDinExiDoc = NULL;
    
    pDinExiDoc = (struct dinEXIDocument *)pExiDoc;
    
    errn = encode_dinExiDocument(pStream, pDinExiDoc);
    
    return errn;
}




/***************************************************************************************************
* Description                           :   DIN70121 解码API
* Author                                :   Hall
* Creat Date                            :   2022-07-05
* notice                                :   
****************************************************************************************************/
i32 sV2gDinDecodeExiDoc(bitstream_t *pStream, void *pExiDoc)
{
    struct dinEXIDocument *pDinExiDoc = NULL;
    
    pDinExiDoc = (struct dinEXIDocument *)pExiDoc;
    
    return decode_dinExiDocument(pStream, pDinExiDoc);
}







/***************************************************************************************************
* Description                           :   DIN70121消息处理函数
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   负责编解码EXI流
*
*                                           pData       :输入的EXI数据序列首地址
*                                           i32DataLen  :输入的EXI数据序列长度
*
*                                           pRxBuf      :用于解码接收数据的buf----handshake处理有使用buf
*                                           pTxBuf      :用于解码发送数据的buf----底层v2g处理也公用该buf节省ram
*                                           i32BufLen   :pRxBuf & pTxBuf 的长度
****************************************************************************************************/
i32 sV2gDinProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen)
{
    bitstream_t stStreamRx;
    bitstream_t stStreamTx;
    
    size_t posRx = 0;
    size_t posTx = 0;
    
    i32 i32Err;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
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
    i32Err = sV2gDinDecodeExiDoc(&stStreamRx, (void*)&pCache->stDocIn);
    if(i32Err != 0)
    {
        EN_SLOGE(TAG, "DIN70121 v2g Req消息解码错误, errn:%d", i32Err);
        return i32Err;
    }
    
    //3：V2G消息处理 得到 stExiDocTx
    pCache->pCbSeqTimerRst(i32DevIndex);
    sV2gDinParse(i32DevIndex, &pCache->stDocIn, &pCache->stDocOut);
    
    //4：编码 并发送
    if(i32Err == 0)
    {
        i32Err = sV2gDinEncodeExiDoc(&stStreamTx, (void*)&pCache->stDocOut);
        if (i32Err == 0)
        {
            //[V2G-DC-364]
            pCache->pCbSeqTimerStart(i32DevIndex);
            sBmsCcsNetV2gtpSendViaTcp(i32DevIndex, stStreamTx.data, (*stStreamTx.pos), cV2gtpVer, cV2gtpPayloadTypeExi);
        }
        else
        {
            EN_SLOGE(TAG, "DIN70121 v2g Res消息编码错误, errn:%d", i32Err);
        }
    }
    
    return(i32Err);
}







/***************************************************************************************************
* Description                           :   DIN70121消息解析函数
* Author                                :   Hall
* Creat Date                            :   2024-01-22
* notice                                :   负责以下各分类消息的映射执行和回复
****************************************************************************************************/
i32 sV2gDinMsgIdSearch(struct dinEXIDocument* pDocIn)
{
    i32 i32Err = ERROR_UNEXPECTED_REQUEST_MESSAGE;
    
    eV2gDinMsgId_t eMsgId = eV2gDinMsgIdMax;
    
    if(pDocIn->V2G_Message_isUsed)
    {
        init_dinEXIDocument(pDocIn);
        if (pDocIn->V2G_Message.Body.SessionSetupReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdSessionSetup;
        }
        else if(pDocIn->V2G_Message.Body.ServiceDiscoveryReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdServiceDiscovery;
        }
        else if(pDocIn->V2G_Message.Body.ServicePaymentSelectionReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdServicePaymentSelection;
        }
        else if(pDocIn->V2G_Message.Body.ContractAuthenticationReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdContractAuthorization;
        }
        else if(pDocIn->V2G_Message.Body.ChargeParameterDiscoveryReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdChargeParameterDiscovery;
        }
        else if(pDocIn->V2G_Message.Body.CableCheckReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdCableCheck;
        }
        else if(pDocIn->V2G_Message.Body.PreChargeReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdPreCharge;
        }
        else if(pDocIn->V2G_Message.Body.PowerDeliveryReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdPowerDelivery1;
        }
        else if(pDocIn->V2G_Message.Body.CurrentDemandReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdCurrentDemand;
        }
        else if(pDocIn->V2G_Message.Body.WeldingDetectionReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdWeldingDetection;
        }
        else if(pDocIn->V2G_Message.Body.SessionStopReq_isUsed)
        {
            eMsgId = eV2gDinMsgIdSessionStop;
        }
    }
    
    return eMsgId;
}







/***************************************************************************************************
* Description                           :   DIN70121 待发送 struct dinEXIDocument 结构体的初始化
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   按照输入的 消息类型 去初始化 准备发送的 EXI 文档 减少重复代码
****************************************************************************************************/
bool sV2gDinDocInit(struct dinEXIDocument *pDoc, eV2gDinMsgId_t eMsgId)
{
    bool bRst;
    
    init_dinMessageHeaderType(&pDoc->V2G_Message.Header);
    init_dinBodyType(&pDoc->V2G_Message.Body);
    pDoc->V2G_Message_isUsed = 1u;
    
    bRst = true;
    switch(eMsgId)
    {
    case eV2gDinMsgIdSessionSetup:
        init_dinSessionSetupResType(&pDoc->V2G_Message.Body.SessionSetupRes);
        pDoc->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdServiceDiscovery:
        init_dinServiceDiscoveryResType(&pDoc->V2G_Message.Body.ServiceDiscoveryRes);
        pDoc->V2G_Message.Body.ServiceDiscoveryRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdServicePaymentSelection:
        init_dinServicePaymentSelectionResType(&pDoc->V2G_Message.Body.ServicePaymentSelectionRes);
        pDoc->V2G_Message.Body.ServicePaymentSelectionRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdContractAuthorization:
        init_dinContractAuthenticationResType(&pDoc->V2G_Message.Body.ContractAuthenticationRes);
        pDoc->V2G_Message.Body.ContractAuthenticationRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdChargeParameterDiscovery:
        init_dinChargeParameterDiscoveryResType(&pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes);
        pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdCableCheck:
        init_dinCableCheckResType(&pDoc->V2G_Message.Body.CableCheckRes);
        pDoc->V2G_Message.Body.CableCheckRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdPreCharge:
        init_dinPreChargeResType(&pDoc->V2G_Message.Body.PreChargeRes);
        pDoc->V2G_Message.Body.PreChargeRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdPowerDelivery1:
    case eV2gDinMsgIdPowerDelivery2:
        init_dinPowerDeliveryResType(&pDoc->V2G_Message.Body.PowerDeliveryRes);
        pDoc->V2G_Message.Body.PowerDeliveryRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdCurrentDemand:
        init_dinCurrentDemandResType(&pDoc->V2G_Message.Body.CurrentDemandRes);
        pDoc->V2G_Message.Body.CurrentDemandRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdWeldingDetection:
        init_dinWeldingDetectionResType(&pDoc->V2G_Message.Body.WeldingDetectionRes);
        pDoc->V2G_Message.Body.WeldingDetectionRes_isUsed = 1u;
        break;
    case eV2gDinMsgIdSessionStop:
        init_dinSessionStopResType(&pDoc->V2G_Message.Body.SessionStopRes);
        pDoc->V2G_Message.Body.SessionStopRes_isUsed = 1u;
        break;
    default:
        bRst = false;
        break;
    }
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   DIN70121 接收消息数据解析及回复报文数据填充
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
i32 sV2gDinParse(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    eV2gDinMsgId_t eMsgId;
    
    eMsgId = sV2gDinMsgIdSearch(pDocIn);
    
    if((eMsgId < eV2gDinMsgIdMax)
    && (stV2gDinMsgIdMap[eMsgId].pProc != NULL))
    {
        return stV2gDinMsgIdMap[eMsgId].pProc(i32DevIndex, pDocIn, pDocOut);
    }
    
    return(-1);
}













/***************************************************************************************************
* Description                           :   DIN70121 SessionSetup 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-07-06
* notice                                :   会话建立消息
*
*ResponseCode 字段的填充策略:---适用所有消息
*   <DIN SPEC 70121 2014-12 212P.pdf> @Table 78 中多个 FAILED 或 FAILED_xxx 都符合时 应该取第一个值
*   一部分消息需要由用户程序调用api接口函数去填充
*   一部分消息则由自己根据标准定义的规则去填充
****************************************************************************************************/
i32 sV2gDinSessionSetup(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    struct dinSessionSetupReqType *pReq = NULL;
    struct dinSessionSetupResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gDinPrintSessionSetupReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdSessionSetup);
    pReq = &pDocIn->V2G_Message.Body.SessionSetupReq;
    pRes = &pDocOut->V2G_Message.Body.SessionSetupRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stSessionSetupReq, pReq, sizeof(pCache->stData.stSessionSetupReq));
    
    
    //生成 session id
    sV2gDinMakeSessionId(pCache->u8SessionId, sizeof(pCache->u8SessionId));
    vTaskDelay(500 / portTICK_RATE_MS);
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionSetup, dinresponseCodeType_OK);
    sV2gDinCompSessionIdForSessionSetup(i32DevIndex, pCache->u8SessionId, pReq->EVCCID.bytes, dinSessionSetupReqType_EVCCID_BYTES_SIZE);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdSessionSetup);
    
    
    
    //4：填充XML文档结构体
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    
    //4.2:DateTimeNow 字段----比如时间戳这一类字段,是需要在这里就地赋值的 避免时差
    pCache->stData.stSessionSetupRes.DateTimeNow_isUsed = 1u;
    pCache->stData.stSessionSetupRes.DateTimeNow = 1663079785;
    memcpy(pRes, &pCache->stData.stSessionSetupRes, sizeof(pCache->stData.stSessionSetupRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gDinPrintSessionSetupRes(i32DevIndex, pDocOut);
    
    
    return 0;
}











/***************************************************************************************************
* Description                           :   DIN70121 ServiceDiscovery 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-07-06
* notice                                :   服务发现消息
****************************************************************************************************/
i32 sV2gDinServiceDiscovery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    struct dinServiceDiscoveryReqType *pReq = NULL;
    struct dinServiceDiscoveryResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gDinPrintServiceDiscoveryReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdServiceDiscovery);
    pReq = &pDocIn->V2G_Message.Body.ServiceDiscoveryReq;
    pRes = &pDocOut->V2G_Message.Body.ServiceDiscoveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stServiceDiscoveryReq, pReq, sizeof(pCache->stData.stServiceDiscoveryReq));
    vTaskDelay(500 / portTICK_RATE_MS);
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdServiceDiscovery, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdServiceDiscovery);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stServiceDiscoveryRes.ResponseCode);
    //sV2gDinProcServiceCategory(pReq->ServiceCategory, pCache, &pRes->ResponseCode);//忽略服务类别  TEST 11.30
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stServiceDiscoveryRes, sizeof(pCache->stData.stServiceDiscoveryRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gDinPrintServiceDiscoveryRes(i32DevIndex, pDocOut);
    
    
    return 0;
}











/***************************************************************************************************
* Description                           :   DIN70121 ServicePaymentSelection 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-07-06
* notice                                :   支付方式选择消息
****************************************************************************************************/
i32 sV2gDinServicePaymentSelection(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    struct dinServicePaymentSelectionReqType *pReq = NULL;
    struct dinServicePaymentSelectionResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    sV2gDinPrintServicePaymentSelectionReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdServicePaymentSelection);
    pReq = &pDocIn->V2G_Message.Body.ServicePaymentSelectionReq;
    pRes = &pDocOut->V2G_Message.Body.ServicePaymentSelectionRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stServicePaymentSelectionReq, pReq, sizeof(pCache->stData.stServicePaymentSelectionReq));
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdServicePaymentSelection, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdServicePaymentSelection);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stServicePaymentSelectionRes.ResponseCode);
    sV2gDinProcSelectedPaymentOption(i32DevIndex, pCache, &pCache->stData.stServicePaymentSelectionRes.ResponseCode, pReq->SelectedPaymentOption);
    sV2gDinProcSelectedServiceList(i32DevIndex, pCache, &pCache->stData.stServicePaymentSelectionRes.ResponseCode, &pReq->SelectedServiceList);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stServicePaymentSelectionRes, sizeof(pCache->stData.stServicePaymentSelectionRes));
    
    //5：打印将要发出的V2G响应消息
    sV2gDinPrintServicePaymentSelectionRes(i32DevIndex, pDocOut);
    
    
    return 0;
}















/***************************************************************************************************
* Description                           :   DIN70121 ContractAuthentication 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-07-12
* notice                                :   合约认证消息
****************************************************************************************************/
i32 sV2gDinContractAuthentication(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinContractAuthenticationResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 10)
    || (pCache->stData.stContractAuthRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        sV2gDinPrintContractAuthenticationReq(i32DevIndex, pDocIn);
    }
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdContractAuthorization);
    pRes = &pDocOut->V2G_Message.Body.ContractAuthenticationRes;
    
    
    //3：缓存接收内容---ContractAuthentication 消息不需要
    //memcpy(&pCache->stData.stCurrentDemandReq, pReq, sizeof(pCache->stData.stCurrentDemandReq));
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdContractAuthorization, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdContractAuthorization);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stContractAuthRes.ResponseCode);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stContractAuthRes, sizeof(pCache->stData.stContractAuthRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 10)
    || (pCache->stData.stContractAuthRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintContractAuthenticationRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}













/***************************************************************************************************
* Description                           :   DIN70121 DinChargeParameterDiscovery 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-07-12
* notice                                :   充电参数发现消息
****************************************************************************************************/
i32 sV2gDinChargeParameterDiscovery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinChargeParameterDiscoveryReqType *pReq = NULL;
    struct dinChargeParameterDiscoveryResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        sV2gDinPrintChargeParameterDiscoveryReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdChargeParameterDiscovery);
    pReq = &pDocIn->V2G_Message.Body.ChargeParameterDiscoveryReq;
    pRes = &pDocOut->V2G_Message.Body.ChargeParameterDiscoveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stChargeParamDiscoveryReq, pReq, sizeof(pCache->stData.stChargeParamDiscoveryReq));
    sV2gDinProcPublicDataChargeParameterDiscovery(pCache, pReq);
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdChargeParameterDiscovery, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdChargeParameterDiscovery);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode);
    sV2gDinProcDcEvChargeParameter(i32DevIndex, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode, &pReq->DC_EVChargeParameter);
    sV2gDinProcEvRequestedEnergyTransferType(i32DevIndex, pCache, &pCache->stData.stChargeParamDiscoveryRes.ResponseCode, pReq->EVRequestedEnergyTransferType);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stChargeParamDiscoveryRes, sizeof(pCache->stData.stChargeParamDiscoveryRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stChargeParamDiscoveryRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintChargeParameterDiscoveryRes(i32DevIndex, pDocOut);
    }
    
    
    //6：启动CP状态检测, 根据<DIN SPEC 70121 2014-12 212P.pdf>@[V2G-DC-547] 条款
    if((pRes->ResponseCode == dinresponseCodeType_OK)
    && (pRes->EVSEProcessing == dinEVSEProcessingType_Finished)
    && (pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEStatusCode == dinDC_EVSEStatusCodeType_EVSE_Ready))
    {
        //期望的cp状态填写 eChgCpStateC 就可以了
        //时序逻辑会同时将 eChgCpStateC/eChgCpStateD 设定为期望值的
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_FAILED_WrongEnergyTransferType + 1);//ResponseCode 先设置无效值
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateC);
    }
    
    
    return 0;
}












/***************************************************************************************************
* Description                           :   DIN70121 CableCheck 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   电缆检测(绝缘)消息
****************************************************************************************************/
i32 sV2gDinCableCheck(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinCableCheckReqType *pReq = NULL;
    struct dinCableCheckResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stCableCheckRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        sV2gDinPrintCableCheckReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdCableCheck);
    pReq = &pDocIn->V2G_Message.Body.CableCheckReq;
    pRes = &pDocOut->V2G_Message.Body.CableCheckRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCableCheckReq, pReq, sizeof(pCache->stData.stCableCheckReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct dinDC_EVStatusType));
    
    //ResponseCode赋初值---本消息不在这里赋初值
    //根据 [V2G-DC-547] 应该在 ChargeParameterDiscovery 消息中发起CP状态检查的地方赋初值
    //sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdCableCheck, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdCableCheck);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCableCheckRes.ResponseCode);
    
    
    while((pCache->stData.stCableCheckRes.ResponseCode != dinresponseCodeType_OK)
    && (pCache->stData.stCableCheckRes.ResponseCode != dinresponseCodeType_FAILED)
    && (pCache->stData.stCableCheckRes.ResponseCode != dinresponseCodeType_FAILED_SequenceError)
    && (pCache->stData.stCableCheckRes.ResponseCode != dinresponseCodeType_FAILED_UnknownSession))
    {
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stCableCheckRes, sizeof(pCache->stData.stCableCheckRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    || (pCache->stData.stCableCheckRes.EVSEProcessing == dinEVSEProcessingType_Finished))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintCableCheckRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}







/***************************************************************************************************
* Description                           :   DIN70121 PreCharge 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   预充电消息
****************************************************************************************************/
i32 sV2gDinPreCharge(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinPreChargeReqType *pReq = NULL;
    struct dinPreChargeResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        sV2gDinPrintPreChargeReq(i32DevIndex, pDocIn);
    }
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdPreCharge);
    pReq = &pDocIn->V2G_Message.Body.PreChargeReq;
    pRes = &pDocOut->V2G_Message.Body.PreChargeRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPreChargeReq, pReq, sizeof(pCache->stData.stPreChargeReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct dinDC_EVStatusType));
    memcpy(&pCache->stDataPublic.EVTargetVoltage, &pReq->EVTargetVoltage, sizeof(struct dinPhysicalValueType));
    memcpy(&pCache->stDataPublic.EVTargetCurrent, &pReq->EVTargetCurrent, sizeof(struct dinPhysicalValueType));
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdPreCharge, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdPreCharge);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPreChargeRes.ResponseCode);
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stPreChargeRes, sizeof(pCache->stData.stPreChargeRes));
    
    
    //5：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintPreChargeRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}












/***************************************************************************************************
* Description                           :   DIN70121 PowerDelivery 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   电力传输消息
****************************************************************************************************/
i32 sV2gDinPowerDelivery(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    eV2gDinMsgId_t eMsgId;
    struct dinPowerDeliveryReqType *pReq = NULL;
    struct dinPowerDeliveryResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    sV2gDinPrintPowerDeliveryReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdPowerDelivery1);
    pReq = &pDocIn->V2G_Message.Body.PowerDeliveryReq;
    pRes = &pDocOut->V2G_Message.Body.PowerDeliveryRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stPowerDeliveryReq, pReq, sizeof(pCache->stData.stPowerDeliveryReq));
    sV2gDinProcPublicDataPowerDelivery(pCache, pReq);
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdPowerDelivery1, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    eMsgId = (pReq->ReadyToChargeState == 1) ? eV2gDinMsgIdPowerDelivery1 : eV2gDinMsgIdPowerDelivery2;
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eMsgId);
    
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode);
    if(pReq->ChargingProfile_isUsed == 1)
    {
        sV2gDinSAScheduleTupleIDCheck(i32DevIndex, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode, pReq->ChargingProfile.SAScheduleTupleID);
        sV2gDinChargingProfileCheck(i32DevIndex, pCache, &pCache->stData.stPowerDeliveryRes.ResponseCode, &pReq->ChargingProfile);
    }
    
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stPowerDeliveryRes, sizeof(pCache->stData.stPowerDeliveryRes));
    pRes->EVSEStatus_isUsed = 0u;
    pRes->AC_EVSEStatus_isUsed = 0u;
    
    //5：打印将要发出的V2G响应消息
    sV2gDinPrintPowerDeliveryRes(i32DevIndex, pDocOut);
    
    
    //6：启动CP状态检测, 根据<DIN SPEC 70121 2014-12 212P.pdf>@[V2G-DC-556] 条款
    if((pReq->ReadyToChargeState == 0) && (pRes->ResponseCode == dinresponseCodeType_OK))
    {
        //ResponseCode 先设置无效值
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionStop,      dinresponseCodeType_FAILED_WrongEnergyTransferType + 1);
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdWeldingDetection, dinresponseCodeType_FAILED_WrongEnergyTransferType + 1);
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateB);
    }
    
    
    return 0;
}








/***************************************************************************************************
* Description                           :   DIN70121 CurrentDemand 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   当前需求消息
****************************************************************************************************/
i32 sV2gDinCurrentDemand(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinCurrentDemandReqType *pReq = NULL;
    struct dinCurrentDemandResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    || (pCache->stData.stCurrentDemandRes.DC_EVSEStatus.EVSEStatusCode != dinDC_EVSEStatusCodeType_EVSE_Ready))
    {
        sV2gDinPrintCurrentDemandReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdCurrentDemand);
    pReq = &pDocIn->V2G_Message.Body.CurrentDemandReq;
    pRes = &pDocOut->V2G_Message.Body.CurrentDemandRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stCurrentDemandReq, pReq, sizeof(pCache->stData.stCurrentDemandReq));
    sV2gDinProcPublicDataCurrentDemand(pCache, pReq);
    
    //ResponseCode赋初值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdCurrentDemand, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdCurrentDemand);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stCurrentDemandRes.ResponseCode);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stCurrentDemandRes, sizeof(pCache->stData.stCurrentDemandRes));
    
    
    //5：打印将要发出的V2G响应消息
    if((abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 30)
    || (pCache->stData.stCurrentDemandRes.DC_EVSEStatus.EVSEStatusCode != dinDC_EVSEStatusCodeType_EVSE_Ready))
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintCurrentDemandRes(i32DevIndex, pDocOut);
    }
    
    return 0;
}







/***************************************************************************************************
* Description                           :   DIN70121 WeldingDetection 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   焊接检测消息
****************************************************************************************************/
i32 sV2gDinWeldingDetection(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    i32 i32TimeStamp = 0;
    static i32 i32TimeStampOld[cMse102xDevNum];
    
    struct dinWeldingDetectionReqType *pReq = NULL;
    struct dinWeldingDetectionResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    i32TimeStamp = sGetTimestamp();
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        sV2gDinPrintWeldingDetectionReq(i32DevIndex, pDocIn);
    }
    
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdWeldingDetection);
    pReq = &pDocIn->V2G_Message.Body.WeldingDetectionReq;
    pRes = &pDocOut->V2G_Message.Body.WeldingDetectionRes;
    
    
    //3：缓存接收内容
    memcpy(&pCache->stData.stWeldingDetectionReq, pReq, sizeof(pCache->stData.stWeldingDetectionReq));
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct dinDC_EVStatusType));
    
    //ResponseCode赋初值---本消息不在这里赋初值
    //根据 [V2G-DC-556] 应该在 ReadyToChargeState=0 的 PowerDelivery 消息中发起CP状态检查的地方赋初值
    //sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdWeldingDetection, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdWeldingDetection);
    
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stWeldingDetectionRes.ResponseCode);
    
    
    while((pCache->stData.stWeldingDetectionRes.ResponseCode != dinresponseCodeType_OK)
    && (pCache->stData.stWeldingDetectionRes.ResponseCode != dinresponseCodeType_FAILED)
    && (pCache->stData.stWeldingDetectionRes.ResponseCode != dinresponseCodeType_FAILED_SequenceError)
    && (pCache->stData.stWeldingDetectionRes.ResponseCode != dinresponseCodeType_FAILED_UnknownSession))
    {
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    
    //由 PowerDelivery 停充消息触发的CP检测(超时)结果
    //如果由本消息负责回复该结果 那就要清零 eV2gDinMsgIdSessionStop 消息中被设置的 FAILED 值
    sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionStop, dinresponseCodeType_OK);
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stWeldingDetectionRes, sizeof(pCache->stData.stWeldingDetectionRes));
    
    //6.2：打印将要发出的V2G响应消息
    if(abs(i32TimeStamp - i32TimeStampOld[i32DevIndex]) >= 1)
    {
        i32TimeStampOld[i32DevIndex] = i32TimeStamp;
        sV2gDinPrintWeldingDetectionRes(i32DevIndex, pDocOut);
    }
    
    
    return 0;
}







/***************************************************************************************************
* Description                           :   DIN70121 SessionStop 消息处理
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   会话关闭消息
****************************************************************************************************/
i32 sV2gDinSessionStop(i32 i32DevIndex, struct dinEXIDocument *pDocIn, struct dinEXIDocument *pDocOut)
{
    struct dinSessionStopResType *pRes = NULL;
    
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    //1：打印收到的V2G请求消息
    sV2gDinPrintSessionStopReq(i32DevIndex, pDocIn);
    
    //2：初始化回复XML结构体
    sV2gDinDocInit(pDocOut, eV2gDinMsgIdSessionStop);
    pRes = &pDocOut->V2G_Message.Body.SessionStopRes;
    
    
    //3：缓存接收内容
    //memcpy(&pCache->stData.stSessionStopReq, pReq, sizeof(pCache->stData.stSessionStopReq));
    
    //ResponseCode赋初值---本消息不在这里赋初值
    //根据 [V2G-DC-556] 应该在 ReadyToChargeState=0 的 PowerDelivery 消息中发起CP状态检查的地方赋初值
    //sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionStop, dinresponseCodeType_OK);
    
    //[V2G-DC-390]
    sV2gDinMsgSequenceErrorCheck(i32DevIndex, eV2gDinMsgIdSessionStop);
    
    //字段检查
    sV2gDinCompSessionId(i32DevIndex, pDocIn, pCache, &pCache->stData.stSessionStopRes.ResponseCode);
    
    
    while((pCache->stData.stSessionStopRes.ResponseCode != dinresponseCodeType_OK)
    && (pCache->stData.stSessionStopRes.ResponseCode != dinresponseCodeType_FAILED)
    && (pCache->stData.stSessionStopRes.ResponseCode != dinresponseCodeType_FAILED_SequenceError)
    && (pCache->stData.stSessionStopRes.ResponseCode != dinresponseCodeType_FAILED_UnknownSession))
    {
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    
    //4：填充XML文档结构体字段
    //4.1:SessionID 字段：
    sV2gDinFillSessionId(pDocOut, pCache->u8SessionId, sizeof(pCache->u8SessionId));
    memcpy(pRes, &pCache->stData.stSessionStopRes, sizeof(pCache->stData.stSessionStopRes));
    
    
    //5：打印将要发出的V2G响应消息
    sV2gDinPrintSessionStopRes(i32DevIndex, pDocOut);
    
    
    //6：关闭CP振荡器
    pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    
    
    return 0;
}















