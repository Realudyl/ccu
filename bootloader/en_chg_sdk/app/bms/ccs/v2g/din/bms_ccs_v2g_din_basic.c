/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_din_basic.c
* Description                           :   DIN70121 协议实现 之 V2G消息内各个字段的处理函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-24
* notice                                :   
****************************************************************************************************/
#include "adc_drv_isr.h"

#include "bms_ccs_v2g_din_api.h"
#include "bms_ccs_v2g_din_basic.h"
#include "bms_ccs_v2g_timing.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_din_basic";





extern stV2gDinCache_t *pV2gDinCache[];






//DIN70121协议标准的单位定义  dinunitSymbolType
const char u8DinUnit[10][8] = 
{
    "h",
    "m",
    "s",
    "A",
    "Ah",
    "V",
    "VA",
    "W",
    "Ws",
    "Wh"
};






//<DIN SPEC 70121 2014-12 212P.pdf> @9.7.4.2.4 Normal SECC message sequence
//标准协议文本 9.7.4.2.4:SECC正常的消息序列
//本表根据以上章节的以下条款定义 每个 Res 消息发送后 都有预期的 Req 消息接收
//如果收到不符合预期的消息 则需要根据 [V2G-DC-666] 去操作
#define cV2gDinMsgNumMax                4
eV2gDinMsgId_t eV2gDinMsgIdMap[eV2gDinMsgIdMax][cV2gDinMsgNumMax] =
{
    //-1是无效值
    
    
    //[V2G-DC-437]:发送 supportedAppProtocol Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionSetup,  -1,                                     -1,                                 -1},
    
    
    //[V2G-DC-439]:发送 SessionSetup Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdServiceDiscovery,           -1,                                 -1},
    
    
    //[V2G-DC-441]:发送 ServiceDiscovery Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdServicePaymentSelection,    -1,                                 -1},
    
    
    //[V2G-DC-444]:发送 ServicePaymentSelection Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdContractAuthorization,      -1,                                 -1},
    
    
    //[V2G-DC-495/497]:发送 ContractAuthentication Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdChargeParameterDiscovery,   eV2gDinMsgIdContractAuthorization,  -1},
    
    
    //[V2G-DC-453/498]:发送 ChargeParameterDiscovery Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdChargeParameterDiscovery,   eV2gDinMsgIdCableCheck,             -1},
    
    
    //[V2G-DC-455/499]:发送 CableCheck Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdCableCheck,                 eV2gDinMsgIdPreCharge,              -1},
    
    
    //[V2G-DC-458]:发送 PreCharge Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdPreCharge,                  eV2gDinMsgIdPowerDelivery1,         -1},
    
    
    //[V2G-DC-462]:发送 (ReadyToChargeState 等于“TRUE” 的)PowerDelivery Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdCurrentDemand, -1,                                     -1,                                 -1},
    
    
    //[V2G-DC-465]:发送 CurrentDemand Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdCurrentDemand, eV2gDinMsgIdPowerDelivery2,             -1,                                 -1},
    
    
    //[V2G-DC-459]:发送 (ReadyToChargeState 等于“FALSE”的)PowerDelivery Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdWeldingDetection,           -1,                                 -1},
    
    
    //[V2G-DC-469]:发送 WeldingDetection Res 消息后 下一步应该收到的报文
    {eV2gDinMsgIdSessionStop,   eV2gDinMsgIdWeldingDetection,           -1,                                 -1},
    
    
    //[V2G-DC-469]:发送 SessionStop Res 消息后 下一步应该收到的报文
    {-1,                        -1,                                     -1,                                 -1},
    
    
};









//V2G DIN70121 PhysicalValueType 复杂数据类型处理函数----公用
f32  sV2gDinGetPhyValue(struct dinPhysicalValueType *pValue, char *pUnit);
void sV2gDinSetPhyValue(struct dinPhysicalValueType *pValue, f32 f32Value, dinunitSymbolType eUnit);
bool sV2gDinCmpPhyValue(struct dinPhysicalValueType *pBig, struct dinPhysicalValueType *pSmall, bool bEqualFlag);

//V2G DIN70121 SessionId 处理----公用
void sV2gDinMakeSessionId(u8 *pSessionId, u16 u16Len);
bool sV2gDinFillSessionId(struct dinEXIDocument *pDoc, const u8 *pId, i32 i32IdLen);
void sV2gDinCompSessionId(i32 i32DevIndex, struct dinEXIDocument *pDoc, stV2gDinCache_t *pCache, dinresponseCodeType *pCode);

//V2G DIN70121 FAILED_SequenceError 错误检查
bool sV2gDinMsgSequenceErrorCheck(i32 i32DevIndex, eV2gDinMsgId_t eMsgId);

//V2G DIN70121 cp状态错误 检查
bool sV2gDinCpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState);




//for SessionSetup
void sV2gDinCompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen);

//for ServicePaymentSelection
bool sV2gDinProcSelectedPaymentOption(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinpaymentOptionType eType);
bool sV2gDinProcSelectedServiceList(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinSelectedServiceListType *pList);

//for DinChargeParameterDiscovery
bool sV2gDinProcPublicDataChargeParameterDiscovery(stV2gDinCache_t *pCache, struct dinChargeParameterDiscoveryReqType *pReq);
bool sV2gDinProcEvRequestedEnergyTransferType(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinEVRequestedEnergyTransferType eType);
bool sV2gDinProcDcEvChargeParameter(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinDC_EVChargeParameterType *pDcEvParam);

//for PowerDelivery
bool sV2gDinProcPublicDataPowerDelivery(stV2gDinCache_t *pCache, struct dinPowerDeliveryReqType *pReq);
bool sV2gDinSAScheduleTupleIDCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, i16 SAScheduleTupleID);
bool sV2gDinChargingProfileCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinChargingProfileType *pChargingProfile);

//for CurrentDemand
bool sV2gDinProcPublicDataCurrentDemand(stV2gDinCache_t *pCache, struct dinCurrentDemandReqType *pReq);















/***************************************************************************************************
* Description                           :   DIN70121 的 PhysicalValueType 型数据结构 值计算
* Author                                :   Hall
* Creat Date                            :   2022-08-17
* notice                                :   pValue    : 待解析的数据结构
*                                           pUnit     : 向调用者返回单位的字符串 可以为NULL
****************************************************************************************************/
f32 sV2gDinGetPhyValue(struct dinPhysicalValueType *pValue, char *pUnit)
{
    f32 f32Value;
    f64 f64Temp;
    
    if((pUnit != NULL) && (pValue->Unit_isUsed == 1u) && (pValue->Unit < (dinunitSymbolType_Wh + 1)))
    {
        memcpy(pUnit, u8DinUnit[pValue->Unit], strlen(u8DinUnit[pValue->Unit]));
    }
    
    f64Temp = pow(10, pValue->Multiplier);
    f64Temp = f64Temp * pValue->Value;
    f32Value = (f32)f64Temp;
    
    return(f32Value);
}





/***************************************************************************************************
* Description                           :   DIN70121 的 PhysicalValueType 型数据结构 赋值
* Author                                :   Hall
* Creat Date                            :   2022-08-26
* notice                                :   pValue    : 待赋值的数据结构
*                                           f32Value  : 数值 
*                                           eUnit     : 单位
****************************************************************************************************/
void sV2gDinSetPhyValue(struct dinPhysicalValueType *pValue, f32 f32Value, dinunitSymbolType eUnit)
{
    i16 i;
    f32 f32ValueTemp;
    
    if(pValue == NULL)
    {
        return;
    }
    
    i = 0;
    f32ValueTemp = fabs(f32Value);
    
    if(f32ValueTemp > 3276.7)                                   //2023.11.29，这里好像就只有功率会进来
    {
        //数值部分大于 (2^15-1)/10 (即3276.7)时 只能原始值设定
        //不能再扩大倍数，否则 ->Value, i16型成员会溢出
        f32ValueTemp = f32ValueTemp / 1000;                     //2023.11.29，此处将数值单位从w改为kw
        i = 3;                                                  //2023.11.29，10的3次方
    }
    else                                                        //2023.11.29，这里好像除了功率都会进来
    {
        //数值部分小于 3276.7 时  统一放大10倍设定 不至于溢出
        f32ValueTemp = f32ValueTemp * 10;
        i = (-1);
    }
    
    if(f32Value < 0)
    {
        f32ValueTemp = f32ValueTemp * (-1);
    }
    
    pValue->Multiplier = i;
    pValue->Value = (i16)f32ValueTemp;
    
    if(eUnit <= dinunitSymbolType_Wh)
    {
        pValue->Unit_isUsed = 1u;
        pValue->Unit = eUnit;
    }
    else
    {
        pValue->Unit_isUsed = 0u;
        pValue->Unit = 0;
    }
}








/***************************************************************************************************
* Description                           :   DIN70121 的 PhysicalValueType 型数据结构 值比较
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   pBig          :期望的较大值
*                                           pSmall        :期望的较小值
*                                           bEqualFlag    :带= 比较标志
*
*                                           如果参与比较的两个值大小关系符合期望 就返回 true
****************************************************************************************************/
bool sV2gDinCmpPhyValue(struct dinPhysicalValueType *pBig, struct dinPhysicalValueType *pSmall, bool bEqualFlag)
{
    bool bRst;
    f32  f32Big;
    f32  f32Small;
    
    f32Big = sV2gDinGetPhyValue(pBig, NULL);
    f32Small = sV2gDinGetPhyValue(pSmall, NULL);
    if(bEqualFlag == true)
    {
        bRst = (f32Big >= f32Small) ? true : false;
    }
    else
    {
        bRst = (f32Big > f32Small) ? true : false;
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DIN70121 协议 随机 SessionID 生成
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   收到 SessionSetupReq 时 生成一个新的随机 SessionID,
*                                           直到 SessionStop 后,SessionID 失效
****************************************************************************************************/
void sV2gDinMakeSessionId(u8 *pSessionId, u16 u16Len)
{
    i32 i;
    i32 i32RandValue = 0;
    struct timeval stTime;
    
    for(i=0; i<u16Len; i++)
    {
        i32RandValue = rand();
        
        *(pSessionId + i) = i32RandValue % 256;
    }
    
    EN_HLOGD(TAG, "生成 SessionID:", pSessionId, u16Len);
}








/***************************************************************************************************
* Description                           :   DIN70121 的 SessionId 字段 填充
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   几乎所有消息都要使用
****************************************************************************************************/
bool sV2gDinFillSessionId(struct dinEXIDocument *pDoc, const u8 *pId, i32 i32IdLen)
{
    memcpy(pDoc->V2G_Message.Header.SessionID.bytes, pId, i32IdLen);
    pDoc->V2G_Message.Header.SessionID.bytesLen = i32IdLen;
    
    return(true);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 SessionID 检查函数
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   [V2G-DC-391]
****************************************************************************************************/
void sV2gDinCompSessionId(i32 i32DevIndex, struct dinEXIDocument *pDoc, stV2gDinCache_t *pCache, dinresponseCodeType *pCode)
{
    if((pDoc->V2G_Message.Header.SessionID.bytesLen != sizeof(pCache->u8SessionId))
    || (memcmp(pDoc->V2G_Message.Header.SessionID.bytes, pCache->u8SessionId, sizeof(pCache->u8SessionId)) != 0))
    {
        //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
        if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_UnknownSession))
        {
            //不匹配的 SessionID 应该返回  FAILED_UnknownSession
            (*pCode) = dinresponseCodeType_FAILED_UnknownSession;
            
            //....
            pCache->pCbSetTcpConnClose(i32DevIndex);
            pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
        }
    }
}









/***************************************************************************************************
* Description                           :   DIN70121 FAILED_SequenceError 错误检查函数
* Author                                :   Hall
* Creat Date                            :   2023-06-28
* notice                                :   [V2G-DC-390]
*
*                                           eMsgIdNew   :接收报文的MsgId
****************************************************************************************************/
bool sV2gDinMsgSequenceErrorCheck(i32 i32DevIndex, eV2gDinMsgId_t eMsgId)
{
    i32  i;
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    
    if((eMsgId < eV2gDinMsgIdSupportedAppProtocol) || (eMsgId >= eV2gDinMsgIdMax))
    {
        return(false);
    }
    
    for(i = 0; i < cV2gDinMsgNumMax; i++)
    {
        if(eMsgId == eV2gDinMsgIdMap[pCache->eId][i])
        {
            //输入的 eMsgId 从映射表中匹配到 说明收到的消息符合预期
            pCache->eId = eMsgId;
            
            return(true);
        }
    }
    
    //未匹配到 说明收到的消息不符合预期 执行错误处理
    //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
    //根据 [V2G-DC-390] FAILED_SequenceError 是优先级最高的错误码，可以直接赋值
    pCache->eId = eMsgId;
    sV2gDinSet_ResponseCode(i32DevIndex, eMsgId, dinresponseCodeType_FAILED_SequenceError);
    
    //[V2G-DC-666]
    pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
    pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    
    return(true);
}






/***************************************************************************************************
* Description                           :     DIN70121 协议 cp状态错误检查 函数
* Author                                :     Hall
* Creat Date                            :     2024-02-03
* notice                                :     
****************************************************************************************************/
bool sV2gDinCpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState)
{
    stV2gDinCache_t *pCache = pV2gDinCache[i32DevIndex];
    
    if(pCache->eCpStateOld != eCpState)                                         //确保cp状态发生变化时才执行错误处理
    {
        if(eCpState == eChgCpStateA)
        {
            //[V2G-DC-667]--->状态A
            pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
            sV2gSeccCpStateDetectionTimerStart(i32DevIndex, (eChgCpStateMax + 1));
        }
        else if((eCpState == eChgCpStateB) && (pCache->eId >= eV2gDinMsgIdCableCheck) && (pCache->eId < eV2gDinMsgIdPowerDelivery2))
        {
            //[V2G-DC-667]--->意外的状态B
            pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
            sV2gSeccCpStateDetectionTimerStart(i32DevIndex, (eChgCpStateMax + 1));
        }
        else if((eCpState == eChgCpStateE) || (eCpState == eChgCpStateF))
        {
            //[V2G-DC-669] & [V2G-DC-670]
        }
        
        pCache->eCpStateOld = eCpState;
    }
    
    return(true);
}









/***************************************************************************************************
* Description                           :     DIN70121 协议 SessionID 检查函数
* Author                                :     Hall
* Creat Date                            :     2023-06-29
* notice                                :     本检查函数专供 SessionSetup 会话使用 
****************************************************************************************************/
void sV2gDinCompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen)
{
    if(memcmp(pSessionIdEvse, pSessionIdEv, i32IdLen) != 0)
    {
        //[V2G-DC-393]
        sV2gDinSet_ResponseCode(i32DevIndex, eV2gDinMsgIdSessionSetup, dinresponseCodeType_OK_NewSessionEstablished);
    }
}









/***************************************************************************************************
* Description                           :   DIN70121 ServicePaymentSelection Req消息 SelectedPaymentOption 字段 处理
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   [V2G-DC-395] & [V2G-DC-665]
****************************************************************************************************/
bool sV2gDinProcSelectedPaymentOption(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinpaymentOptionType eType)
{
    i32 i;
    
    for(i = 0; i < pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.arrayLen; i++)
    {
        //只要匹配到一种支付方式 就 return
        if(eType == pCache->stData.stServiceDiscoveryRes.PaymentOptions.PaymentOption.array[i])
        {
            return(true);
        }
    }
    
    //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
    if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_PaymentSelectionInvalid))
    {
        (*pCode) = dinresponseCodeType_FAILED_PaymentSelectionInvalid;
    }
    
    //[V2G-DC-665]
    pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
    pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    
    return(false);
}





/***************************************************************************************************
* Description                           :   DIN70121 ServicePaymentSelection Req消息 SelectedServiceList 字段 处理
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   [V2G-DC-396]
****************************************************************************************************/
bool sV2gDinProcSelectedServiceList(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinSelectedServiceListType *pList)
{
    i8 i;
    
    for(i = 0; i < pList->SelectedService.arrayLen; i++)
    {
        if(pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceTag.ServiceID == pList->SelectedService.array[i].ServiceID)
        {
            return(true);
        }
    }
    
    
    //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
    if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_ServiceSelectionInvalid))
    {
        (*pCode) = dinresponseCodeType_FAILED_ServiceSelectionInvalid;
    }
    
    pCache->pCbSetTcpConnClose(i32DevIndex);
    pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    
    return(false);
}








/***************************************************************************************************
* Description                           :   DIN70121 ChargeParameterDiscoveryReq消息  公共字段 处理
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   将公共字段拷贝到指定的区域
****************************************************************************************************/
bool sV2gDinProcPublicDataChargeParameterDiscovery(stV2gDinCache_t *pCache, struct dinChargeParameterDiscoveryReqType *pReq)
{
    if(pReq->DC_EVChargeParameter_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumCurrentLimit_isUsed = 1;
        pCache->stDataPublic.EVMaximumVoltageLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.DC_EVStatus,           &pReq->DC_EVChargeParameter.DC_EVStatus,           sizeof(struct dinDC_EVStatusType));
        memcpy(&pCache->stDataPublic.EVMaximumCurrentLimit, &pReq->DC_EVChargeParameter.EVMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
        memcpy(&pCache->stDataPublic.EVMaximumVoltageLimit, &pReq->DC_EVChargeParameter.EVMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
        if(pReq->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed == 1u)
        {
            pCache->stDataPublic.EVMaximumPowerLimit_isUsed = 1;
            memcpy(&pCache->stDataPublic.EVMaximumPowerLimit, &pReq->DC_EVChargeParameter.EVMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
        }
    }
    
    return(true);
}






/***************************************************************************************************
* Description                           :   DIN70121 ChargeParameterDiscoveryReq消息 EVRequestedEnergyTransferType 字段 处理
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   比对收到的 字段值 是否符合本地缓存内的允许值
****************************************************************************************************/
bool sV2gDinProcEvRequestedEnergyTransferType(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, dinEVRequestedEnergyTransferType eType)
{
    u8  u8Type;
    
    u8Type = pCache->stData.stServiceDiscoveryRes.ChargeService.EnergyTransferType;
    if(u8Type != eType)
    {
        //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
        if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_WrongEnergyTransferType))
        {
            (*pCode) = dinresponseCodeType_FAILED_WrongEnergyTransferType;
        }
        
        
        //[V2G-DC-665]
        pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
        
        return(false);
    }
    
    return(true);
}






/***************************************************************************************************
* Description                           :   DIN70121 ChargeParameterDiscoveryReq消息 DC_EVChargeParameter 字段 处理
* Author                                :   Hall
* Creat Date                            :   2023-06-29
* notice                                :   
****************************************************************************************************/
bool sV2gDinProcDcEvChargeParameter(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinDC_EVChargeParameterType *pDcEvParam)
{
    struct dinPhysicalValueType EVSEMinimumCurrentLimit;
    struct dinPhysicalValueType EVSEMinimumVoltageLimit;
    
    
    if(pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed == 1u)//代码严禁一点 是需要此条件的
    {
        EVSEMinimumCurrentLimit = pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumCurrentLimit;
        EVSEMinimumVoltageLimit = pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumVoltageLimit;
        
        
        //充电参数检查
        if(((pDcEvParam->FullSOC_isUsed == 1u) && (pDcEvParam->FullSOC > 100))                                  //EV端充电完成SOC大于100
        || ((pDcEvParam->BulkSOC_isUsed == 1u) && (pDcEvParam->BulkSOC > 100))                                  //EV端快充完成SOC大于100
        || (sV2gDinCmpPhyValue(&pDcEvParam->EVMaximumCurrentLimit, &EVSEMinimumCurrentLimit, false) == false)   //EV端允许的最大电流 小于 EVSE端最小输出电流
        || (sV2gDinCmpPhyValue(&pDcEvParam->EVMaximumVoltageLimit, &EVSEMinimumVoltageLimit, false) == false))  //EV端允许的最大电压 小于 EVSE端最小输出电压
        {
            //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
            if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_WrongChargeParameter))
            {
                (*pCode) = dinresponseCodeType_FAILED_WrongChargeParameter;
            }
            
            
            //[V2G-DC-665]
            pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
            pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
            
            return(false);
        }
    }
    
    return(true);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 SAScheduleTupleID 检查函数
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-16
* notice                                :   不匹配的 SAScheduleTupleID 应该返回 FAILED_TariffSelectionInvalid
****************************************************************************************************/
bool sV2gDinProcPublicDataPowerDelivery(stV2gDinCache_t *pCache, struct dinPowerDeliveryReqType *pReq)
{
    if(pReq->DC_EVPowerDeliveryParameter_isUsed == 1u)
    {
        memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVPowerDeliveryParameter.DC_EVStatus, sizeof(struct dinDC_EVStatusType));
        if(pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed == 1u)
        {
            pCache->stDataPublic.BulkChargingComplete        = pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete;
            pCache->stDataPublic.BulkChargingComplete_isUsed = 1u;
        }
        pCache->stDataPublic.ChargingComplete = pReq->DC_EVPowerDeliveryParameter.ChargingComplete;
    }
    
    return(true);
}






/***************************************************************************************************
* Description                           :   DIN70121 协议 SAScheduleTupleID 检查函数
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-16
* notice                                :   不匹配的 SAScheduleTupleID 应该返回 FAILED_TariffSelectionInvalid
****************************************************************************************************/
bool sV2gDinSAScheduleTupleIDCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, i16 SAScheduleTupleID)
{
    i32 i;
    
    
    for(i = 0; i < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen; i++)
    {
        if(SAScheduleTupleID == pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[i].SAScheduleTupleID)
        {
            return(true);
        }
    }
    
    
    //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
    if(((*pCode) <= dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_TariffSelectionInvalid))
    {
        (*pCode) = dinresponseCodeType_FAILED_TariffSelectionInvalid;
    }
    
    //[V2G-DC-665]
    pCache->pCbSwitchV2gToShutdownEmg(i32DevIndex);
    pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    
    return(false);
}









/*******************************************************************************
 * @FunctionName   :      sV2gDinChargingProfileCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月23日  18:58:52
 * @Description    :      DIN70121 CurrentDemandReq消息  ChargingProfile字段 处理
 * @Input          :      i32DevIndex               外设号
 * @Input          :      pCache                    指向缓存结构体指针
 * @Input          :      pCode                     指向储存ResponseCode的结构体指针
 * @Input          :      pChargingProfile          指向要检查ChargingProfile的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gDinChargingProfileCheck(i32 i32DevIndex, stV2gDinCache_t *pCache, dinresponseCodeType *pCode, struct dinChargingProfileType *pChargingProfile)
{
    u16 i, j, k;
    i16 i16MaxPower, i16PMax;
    
    //注意响应码字段的优先级，根据<DIN SPEC 70121 2014-12 212P.pdf> @9.7.3 Response code usage by the SECC
    if(((*pCode) < dinresponseCodeType_FAILED) || ((*pCode) > dinresponseCodeType_FAILED_ChargingProfileInvalid))
    {
        for(i = 0; i < pChargingProfile->ProfileEntry.arrayLen; i++)
        {
            for(j = 0; j < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen; j++)
            {
                for(k = 0; k < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[j].PMaxSchedule.PMaxScheduleEntry.arrayLen; k++)
                {
                    i16MaxPower = pChargingProfile->ProfileEntry.array[i].ChargingProfileEntryMaxPower;
                    i16PMax = pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[j].PMaxSchedule.PMaxScheduleEntry.array[k].PMax;
                    if(i16MaxPower <= i16PMax)
                    {
                        //若ChargingProfileEntryMaxPower符合其中一个PMax
                        return(true);
                    }
                }
            }
        }
        
        //[V2G-DC-267]
        (*pCode) = dinresponseCodeType_FAILED_ChargingProfileInvalid;
        
        pCache->pCbSetTcpConnClose(i32DevIndex);
        pCache->pCbCpStateDetectionTimerStart(i32DevIndex, eChgCpStateMax);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   DIN70121 CurrentDemandReq消息  公共字段 处理
* Author                                :   Hall
* Creat Date                            :   2024-01-25
* notice                                :   将公共字段拷贝到指定的区域
****************************************************************************************************/
bool sV2gDinProcPublicDataCurrentDemand(stV2gDinCache_t *pCache, struct dinCurrentDemandReqType *pReq)
{
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct dinDC_EVStatusType));
    if(pReq->EVMaximumVoltageLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumVoltageLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumVoltageLimit, &pReq->EVMaximumVoltageLimit, sizeof(struct dinPhysicalValueType));
    }
    if(pReq->EVMaximumCurrentLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumCurrentLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumCurrentLimit, &pReq->EVMaximumCurrentLimit, sizeof(struct dinPhysicalValueType));
    }
    if(pReq->EVMaximumPowerLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumPowerLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumPowerLimit, &pReq->EVMaximumPowerLimit, sizeof(struct dinPhysicalValueType));
    }
    if(pReq->BulkChargingComplete_isUsed == 1u)
    {
        pCache->stDataPublic.BulkChargingComplete        = pReq->BulkChargingComplete;
        pCache->stDataPublic.BulkChargingComplete_isUsed = 1u;
    }
    pCache->stDataPublic.ChargingComplete = pReq->ChargingComplete;
    
    memcpy(&pCache->stDataPublic.EVTargetCurrent, &pReq->EVTargetCurrent, sizeof(struct dinPhysicalValueType));
    memcpy(&pCache->stDataPublic.EVTargetVoltage, &pReq->EVTargetVoltage, sizeof(struct dinPhysicalValueType));
    
    
    return(true);
}
















































