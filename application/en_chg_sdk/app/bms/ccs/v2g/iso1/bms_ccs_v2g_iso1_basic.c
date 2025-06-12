/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1_basic.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-3-13
 * @Attention             :     
 * @Brief                 :     ISO15118 协议实现 之 V2G消息内各个字段的处理函数实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-3-13
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "bms_ccs_v2g_iso1_api.h"
#include "bms_ccs_v2g_iso1_basic.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_iso1_basic";





extern stV2gIso1Cache_t *pV2gIso1Cache[];






//ISO15118协议标准的单位定义  iso1unitSymbolType
const char u8Iso1Unit[7][8] = 
{
    "h",
    "m",
    "s",
    "A",
    "V",
    "W",
    "Wh"
};






//<ISO 15118-2:2014.pdf> @8.8.4.3 SECC
//标准协议文本 8.8.4.3:SECC请求-响应消息序列需求
//本表根据以上章节的以下条款定义 每个 Res 消息发送后 都有预期的 Req 消息接收
//如果收到不符合预期的消息 则需要根据 [V2G2-538] [V2G2-034] 去操作
#define cV2gIso1MsgNumMax                6
eV2gIso1MsgId_t eV2gIso1MsgIdMap[eV2gIso1MsgIdMax][cV2gIso1MsgNumMax] =
{
    //-1是无效值
    
    
    //[V2G2-541]:发送 supportedAppProtocol Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,               eV2gIso1MsgIdSessionSetup,                                       -1,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-543]:发送 SessionSetup Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,           eV2gIso1MsgIdServiceDiscovery,                                       -1,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-545]:发送 ServiceDiscovery Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,              eV2gIso1MsgIdServiceDetail,     eV2gIso1MsgIdPaymentServiceSelection,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-548]:发送 ServiceDetail Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,              eV2gIso1MsgIdServiceDetail,     eV2gIso1MsgIdPaymentServiceSelection,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-551]:发送 PaymentServiceSelection Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdPaymentDetails,     eV2gIso1MsgIdCertificateInstallation,          eV2gIso1MsgIdCertificateUpdate,                eV2gIso1MsgIdAuthorization,                                      -1},
    
    
    //[V2G2-554]:发送 CertificateInstallation Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdPaymentDetails,                                       -1,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-557]:发送 CertificateUpdate Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdPaymentDetails,                                       -1,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-560]:发送 PaymentDetails Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,              eV2gIso1MsgIdAuthorization,                                       -1,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-563]:发送 Authorization Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,              eV2gIso1MsgIdAuthorization,    eV2gIso1MsgIdChargeParameterDiscovery,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-573/582/688]:发送 ChargeParameterDiscovery Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,   eV2gIso1MsgIdChargeParameterDiscovery,                  eV2gIso1MsgIdCableCheck,         eV2gIso1MsgIdPowerDeliveryStart,                                        -1,                                      -1},
    
    
    //[V2G2-584/621]:发送 CableCheck Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,                 eV2gIso1MsgIdCableCheck,                   eV2gIso1MsgIdPreCharge,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-587]:发送 PreCharge Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,                  eV2gIso1MsgIdPreCharge,          eV2gIso1MsgIdPowerDeliveryStart,                                      -1,                                        -1,                                      -1},
    
    
    //[V2G2-576/590]:发送 (ChargeProgress 等于 "iso1chargeProgressType_Start" 的)PowerDelivery Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdChargingStatus,               eV2gIso1MsgIdCurrentDemand,          eV2gIso1MsgIdPowerDeliveryStop,     eV2gIso1MsgIdPowerDeliveryRenegotiate,                                      -1},
    
    
    //[V2G2-575/577]:发送 ChargingStatus 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdChargingStatus,           eV2gIso1MsgIdPowerDeliveryStop,    eV2gIso1MsgIdPowerDeliveryRenegotiate,             eV2gIso1MsgIdMeteringReceipt,                                      -1},
    
    
    //[V2G2-593/795]:发送 CurrentDemand Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,              eV2gIso1MsgIdCurrentDemand,           eV2gIso1MsgIdPowerDeliveryStop,    eV2gIso1MsgIdPowerDeliveryRenegotiate,             eV2gIso1MsgIdMeteringReceipt,                                      -1},
    
    
    //[V2G2-580/797]:发送 MeteringReceipt Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,             eV2gIso1MsgIdChargingStatus,               eV2gIso1MsgIdCurrentDemand,           eV2gIso1MsgIdPowerDeliveryStop,    eV2gIso1MsgIdPowerDeliveryRenegotiate,   eV2gIso1MsgIdChargeParameterDiscovery},
    
    
    //[V2G2-568/601]:发送 (ChargeProgress 等于 "iso1chargeProgressType_Renegotiate" 的)PowerDelivery Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,   eV2gIso1MsgIdChargeParameterDiscovery,                                       -1,                                       -1,                                       -1,                                      -1},
    
    
    //[V2G2-568/601]:发送 (ChargeProgress 等于 "iso1chargeProgressType_Stop" 的)PowerDelivery Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,           eV2gIso1MsgIdWeldingDetection,    eV2gIso1MsgIdChargeParameterDiscovery,                                       -1,                                       -1,                                      -1},
    
    
    //[V2G2-597]:发送 WeldingDetection Res 消息后 下一步应该收到的报文
    {eV2gIso1MsgIdSessionStop,           eV2gIso1MsgIdWeldingDetection,                                       -1,                                       -1,                                       -1,                                      -1},
    
    
    //[V2G2-571]]:发送 SessionStop Res 消息后 下一步应该收到的报文
    {-1,                                                            -1,                                       -1,                                       -1,                                       -1,                                      -1},
    
    
};











//V2G ISO15118 PhysicalValueType 复杂数据类型处理函数----公用
f32  sV2gIso1GetPhyValue(struct iso1PhysicalValueType *pValue, char *pUnit);
void sV2gIso1SetPhyValue(struct iso1PhysicalValueType *pValue, f32 f32Value, iso1unitSymbolType eUnit);
bool sV2gIso1CmpPhyValue(struct iso1PhysicalValueType *pBig, struct iso1PhysicalValueType *pSmall, bool bEqualFlag);

//V2G ISO15118 SessionId 处理----公用
void sV2gIso1MakeSessionId(u8 *pSessionId, u16 u16Len);
bool sV2gIso1FillSessionId(struct iso1EXIDocument *pDoc, const u8 *pId, i32 i32IdLen);
void sV2gIso1CompSessionId(i32 i32DevIndex, struct iso1EXIDocument *pDoc, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode);

//V2G ISO15118 FAILED_SequenceError 错误检查
bool sV2gIso1MsgSequenceErrorCheck(i32 i32DevIndex, eV2gIso1MsgId_t eMsgId, iso1responseCodeType *pCode);

//V2G ISO15118 cp状态错误 检查
bool sV2gIso1CpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState);




//for SessionSetup
void sV2gIso1CompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen);

//for ServiceDetail
bool sV2gIso1ServiceIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, u16 u16ServiceId);

//for PaymentServiceSelection
bool sV2gIso1ProcSelectedPaymentOption(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1paymentOptionType eType);
bool sV2gIso1ProcSelectedServiceList(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1SelectedServiceListType *pList);

//for Authorization
bool sV2gIso1ProcGenChallenge(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i32 i32Len, const u8 *pGenChallenge);

//for ChargeParameterDiscovery
bool sV2gIso1ProcPublicDataChargeParameterDiscovery(stV2gIso1Cache_t *pCache, struct iso1ChargeParameterDiscoveryReqType *pReq);
bool sV2gIso1ProcRequestedEnergyTransferMode(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1EnergyTransferModeType eType);
bool sV2gIso1ProcDcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1DC_EVChargeParameterType *pDcEvParam);
bool sV2gIso1ProcAcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1AC_EVChargeParameterType *pDcEvParam);

//for PowerDelivery
bool sV2gIso1ProcPublicDataPowerDelivery(stV2gIso1Cache_t *pCache, struct iso1PowerDeliveryReqType *pReq);
bool sV2gIso1SAScheduleTupleIDCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i16 SAScheduleTupleID);
bool sV2gIso1ChargingProfileCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1ChargingProfileType *pChargingProfile);

//for CurrentDemand
bool sV2gIso1ProcPublicDataCurrentDemand(stV2gIso1Cache_t *pCache, struct iso1CurrentDemandReqType *pReq);

//for MeterReceipt
bool sV2gIso1ContractIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeteringReceiptReqType *pReq);
bool sV2gIso1MeterInfoCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeterInfoType *pMeterInfo);
















/*******************************************************************************
 * @FunctionName   :      sV2gIso1GetPhyValue
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  15:49:19
 * @Description    :      ISO15118 的 PhysicalValueType 型数据结构 值计算
 * @Input          :      pValue        指向要解析PhysicalValueType结构体的指针
 * @Input          :      pUnit         指向存储要获取的数据单位的指针
 * @Return         :      f32Value      返回解析出来的数值
*******************************************************************************/
f32 sV2gIso1GetPhyValue(struct iso1PhysicalValueType *pValue, char *pUnit)
{
    f32 f32Value;
    f64 f64Temp;
    
    if((pUnit != NULL) && (pValue->Unit < (iso1unitSymbolType_Wh + 1)))
    {
        memcpy(pUnit, u8Iso1Unit[pValue->Unit], strlen(u8Iso1Unit[pValue->Unit]));
    }
    
    f64Temp = pow(10, pValue->Multiplier);
    f64Temp = f64Temp * pValue->Value;
    f32Value = (f32)f64Temp;
    
    return(f32Value);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1SetPhyValue
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  15:57:47
 * @Description    :      ISO15118 的 PhysicalValueType 型数据结构 赋值
 * @Input          :      pValue        指向要封装PhysicalValueType结构体的指针
 * @Input          :      f32Value      要封装的数值
 * @Input          :      eUnit         要封装的数值单位
*******************************************************************************/
void sV2gIso1SetPhyValue(struct iso1PhysicalValueType *pValue, f32 f32Value, iso1unitSymbolType eUnit)
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
    
    if(eUnit <= iso1unitSymbolType_Wh)
    {
        pValue->Unit = eUnit;
    }
}









/*******************************************************************************
 * @FunctionName   :      sV2gIso1CmpPhyValue
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  16:03:44
 * @Description    :      ISO15118 的 PhysicalValueType 型数据结构 值比较
 * @Input          :      pBig          期望的较大值
 * @Input          :      pSmall        期望的较小值
 * @Input          :      bEqualFlag    带= 比较标志
 * @Return         :      如果参与比较的两个值大小关系符合期望 就返回 true
*******************************************************************************/
bool sV2gIso1CmpPhyValue(struct iso1PhysicalValueType *pBig, struct iso1PhysicalValueType *pSmall, bool bEqualFlag)
{
    bool bRst;
    f32  f32Big;
    f32  f32Small;
    
    f32Big = sV2gIso1GetPhyValue(pBig, NULL);
    f32Small = sV2gIso1GetPhyValue(pSmall, NULL);
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











/*******************************************************************************
 * @FunctionName   :      sV2gIso1MakeSessionId
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  16:06:49
 * @Description    :      ISO15118 协议 随机 SessionID 生成
 * @Input          :      pSessionId        指向储存SessionID的指针
 * @Input          :      u16Len            要生成的SessionID数量
 * @Notice         :      收到 SessionSetupReq 时 生成一个新的随机 SessionID,
 *                        直到 SessionStop 后,SessionID 失效
*******************************************************************************/
void sV2gIso1MakeSessionId(u8 *pSessionId, u16 u16Len)
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










/*******************************************************************************
 * @FunctionName   :      sV2gIso1FillSessionId
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  16:10:56
 * @Description    :      ISO15118 的 SessionId 字段 填充
 * @Input          :      pDoc          指向要填充EXI文档结构体指针
 * @Input          :      pId           指向储存有SessionId字段的指针
 * @Input          :      i32IdLen      SessionId字段的长度
 * @Notice         :      几乎所有消息都要使用
*******************************************************************************/
bool sV2gIso1FillSessionId(struct iso1EXIDocument *pDoc, const u8 *pId, i32 i32IdLen)
{
    memcpy(pDoc->V2G_Message.Header.SessionID.bytes, pId, i32IdLen);
    pDoc->V2G_Message.Header.SessionID.bytesLen = i32IdLen;
    
    return(true);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1CompSessionId
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  16:20:26
 * @Description    :      ISO15118 协议 SessionID 检查函数
 * @Input          :      i32DevIndex   外设号
 * @Input          :      pDoc          指向要检查EXI文档结构体指针
 * @Input          :      pCache        指向缓存结构体指针
 * @Input          :      pCode         指向储存ResponseCode的结构体指针
*******************************************************************************/
void sV2gIso1CompSessionId(i32 i32DevIndex, struct iso1EXIDocument *pDoc, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode)
{
    if((pDoc->V2G_Message.Header.SessionID.bytesLen != sizeof(pCache->u8SessionId))
    || (memcmp(pDoc->V2G_Message.Header.SessionID.bytes, pCache->u8SessionId, sizeof(pCache->u8SessionId)) != 0))
    {
        //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
        if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_UnknownSession))
        {
            //不匹配的 SessionID 应该返回  FAILED_UnknownSession
            (*pCode) = iso1responseCodeType_FAILED_UnknownSession;
            
            //[V2G2-538] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
        }
    }
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1MsgSequenceErrorCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  16:35:59
 * @Description    :      ISO15118 FAILED_SequenceError 错误检查函数
 * @Input          :      i32DevIndex       外设号
 * @Input          :      eMsgId            需进行检查的消息ID
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1MsgSequenceErrorCheck(i32 i32DevIndex, eV2gIso1MsgId_t eMsgId, iso1responseCodeType *pCode)
{
    i32  i;
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_SequenceError))
    {
        if((eMsgId < eV2gIso1MsgIdSupportedAppProtocol) || (eMsgId >= eV2gIso1MsgIdMax))
        {
            return(false);
        }
        
        for(i = 0; i < cV2gIso1MsgNumMax; i++)
        {
            if(eMsgId == eV2gIso1MsgIdMap[pCache->eId][i])
            {
                //输入的 eMsgId 从映射表中匹配到 说明收到的消息符合预期
                pCache->eId = eMsgId;
                
                return(true);
            }
        }
        
        //未匹配到 说明收到的消息不符合预期
        //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
        //根据 [V2G2-459] FAILED_SequenceError 是优先级最高的错误码，可以直接赋值
        pCache->eId = eMsgId;
        (*pCode) = iso1responseCodeType_FAILED_SequenceError;
        
        //[V2G2-538] [V2G2-034]
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    
    return(true);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1CpStateErrorCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  17:08:15
 * @Description    :      ISO15118 协议 cp状态错误检查 函数
 * @Input          :      i32DevIndex       外设号
 * @Input          :      eCpState          需进行检查的CP状态
 * @Return         :      
*******************************************************************************/
bool sV2gIso1CpStateErrorCheck(i32 i32DevIndex, eChgCpState_t eCpState)
{
    stV2gIso1Cache_t *pCache = pV2gIso1Cache[i32DevIndex];
    
    if(pCache->eCpStateOld != eCpState)                                         //确保cp状态发生变化时才执行错误处理
    {
        if(eCpState == eChgCpStateA)
        {
            //[V2G-DC-667]--->状态A
            pCache->pCbShutdownEmg(i32DevIndex);
        }
        else if((eCpState == eChgCpStateB) && (pCache->eId >= eV2gIso1MsgIdCableCheck) && (pCache->eId < eV2gIso1MsgIdPowerDeliveryStop))
        {
            //[V2G-DC-667] / [V2G2-917]--->意外的状态B
            pCache->pCbShutdownEmg(i32DevIndex);
        }
        else if((eCpState == eChgCpStateE) || (eCpState == eChgCpStateF))
        {
            //[V2G-DC-669] & [V2G-DC-670]
        }
        
        pCache->eCpStateOld = eCpState;
    }
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1CompSessionIdForSessionSetup
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  17:18:23
 * @Description    :      ISO15118 协议 SessionID 检查函数
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pSessionIdEvse    EVSE SessionID
 * @Input          :      pSessionIdEv      EV SessionID
 * @Input          :      i32IdLen          SessionID 长度
*******************************************************************************/
void sV2gIso1CompSessionIdForSessionSetup(i32 i32DevIndex, u8 *pSessionIdEvse, u8 *pSessionIdEv, i32 i32IdLen)
{
    if(memcmp(pSessionIdEvse, pSessionIdEv, i32IdLen) != 0)
    {
        //[V2G2-462]
        sV2gIso1Set_ResponseCode(i32DevIndex, eV2gIso1MsgIdSessionSetup, iso1responseCodeType_OK_NewSessionEstablished);
    }
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1ServiceIdCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月21日  10:57:55
 * @Description    :      ISO15118 协议 ServiceID 检查函数
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      u16ServiceId      要检查的ServiceID
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ServiceIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, u16 u16ServiceId)
{
    i32 i;
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_ServiceIDInvalid))
    {
        //[V2G2-416] —— ServiceID范围需在1 ~ 4
        if((u16ServiceId >= 1) && (u16ServiceId <= 4))
        {
            //这里还不清楚 ServiceDetailReq的ServiceID   是和stServiceDiscoveryRes.ChargeService的ServiceID匹配,
            //                                           还是和stServiceDiscoveryRes.ServiceList的ServiceID匹配
            if(u16ServiceId == pCache->stData.stServiceDiscoveryRes.ChargeService.ServiceID)
            {
                pCache->stDataPublic.ServiceID = u16ServiceId;
                return(true);
            }
            
            if(pCache->stData.stServiceDiscoveryRes.ServiceList_isUsed == 1)
            {
                for(i = 0; i < pCache->stData.stServiceDiscoveryRes.ServiceList.Service.arrayLen; i++)
                {
                    if(u16ServiceId == pCache->stData.stServiceDiscoveryRes.ServiceList.Service.array[i].ServiceID)
                    {
                        pCache->stDataPublic.ServiceID = u16ServiceId;
                        return(true);
                    }
                }
            }
        }
        
        //[V2G2-425]
        (*pCode) = iso1responseCodeType_FAILED_ServiceIDInvalid;
        
        //[V2G2-539] [V2G2-034]
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcSelectedPaymentOption
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月16日  17:34:01
 * @Description    :      ISO15118 PaymentServiceSelection Req消息 SelectedPaymentOption 字段 处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      eType             要检查的支付方式
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcSelectedPaymentOption(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1paymentOptionType eType)
{
    i32 i;
    
    for(i = 0; i < pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.arrayLen; i++)
    {
        //只要匹配到一种支付方式 就 return
        if(eType == pCache->stData.stServiceDiscoveryRes.PaymentOptionList.PaymentOption.array[i])
        {
            return(true);
        }
    }
    
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_PaymentSelectionInvalid))
    {
        //[V2G2-465]
        (*pCode) = iso1responseCodeType_FAILED_PaymentSelectionInvalid;
    }
    
    //[V2G2-539] [V2G2-034]
    pCache->pCbShutdownEmg(i32DevIndex);
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcSelectedServiceList
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月18日  10:32:36
 * @Description    :      ISO15118 PaymentServiceSelection Req消息 SelectedServiceList 字段 处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      pList             指向要检查SelectedServiceList的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcSelectedServiceList(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1SelectedServiceListType *pList)
{
    i32 i, j;
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_ServiceSelectionInvalid))
    {
        //本函数应该遍历 ServiceDiscovery Res 消息的 ServiceList 字段
        //从其中搜索传入的 pList->SelectedService.array[i].ServiceID
        
        //1:iso1responseCodeType_FAILED_NoChargeServiceSelected
        for(i = 0; i < pList->SelectedService.arrayLen; i++)
        {
            if(pList->SelectedService.array[i].ServiceID == 1)
            {
                //[V2G2-804]
                break;
            }
        }
        if(i == pList->SelectedService.arrayLen)
        {
            (*pCode) = iso1responseCodeType_FAILED_NoChargeServiceSelected;
        }
        
        //2:iso1responseCodeType_FAILED_ServiceSelectionInvalid
        if(pCache->stData.stServiceDiscoveryRes.ServiceList_isUsed == 1)
        {
            for(j = 0; j < pList->SelectedService.arrayLen; j++)
            {
                for(i = 0; i < pCache->stData.stServiceDiscoveryRes.ServiceList.Service.arrayLen; i++)
                {
                    if(pList->SelectedService.array[j].ServiceID == pCache->stData.stServiceDiscoveryRes.ServiceList.Service.array[i].ServiceID)
                    {
                        //有一个匹配的ServiceID,则跳出循环
                        break;
                    }
                }
                if(i == pCache->stData.stServiceDiscoveryRes.ServiceList.Service.arrayLen)
                {
                    //有任何一个 ServiceID 不存在 都应该将 pCode 填充 iso1responseCodeType_FAILED_ServiceSelectionInvalid
                    //[V2G2-467]
                    (*pCode) = iso1responseCodeType_FAILED_ServiceSelectionInvalid;
                    break;
                }
            }
        }
        
        if((*pCode) != iso1responseCodeType_OK)
        {
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
            return(false);
        }
    }
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcGenChallenge
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  14:42:16
 * @Description    :      ISO15118 PaymentServiceSelection Req消息 GenChallenge 字段 处理
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      i32Len            要检查的GenChallenge长度
 * @Input          :      pGenChallenge     指向要检查的GenChallenge的u8指针
*******************************************************************************/
bool sV2gIso1ProcGenChallenge(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i32 i32Len, const u8 *pGenChallenge)
{
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_ChallengeInvalid))
    {
        if(memcmp(pGenChallenge, pCache->stData.stPaymentDetailsRes.GenChallenge.bytes, i32Len) != 0)
        {
            //[V2G2-475]
            (*pCode) = iso1responseCodeType_FAILED_ChallengeInvalid;
            
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
            
            return(false);
        }
    }
    
    return(true);
}


















/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcPublicDataChargeParameterDiscovery
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月18日  11:14:03
 * @Description    :      ISO15118 ChargeParameterDiscoveryReq消息  公共字段 处理
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pReq              指向ChargeParameterDiscoveryReq的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcPublicDataChargeParameterDiscovery(stV2gIso1Cache_t *pCache, struct iso1ChargeParameterDiscoveryReqType *pReq)
{
    //AC充电参数————没有必要存入公共数据缓存
    
    //DC充电参数
    if(pReq->DC_EVChargeParameter_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumCurrentLimit_isUsed = 1;
        pCache->stDataPublic.EVMaximumVoltageLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.DC_EVStatus,               &pReq->DC_EVChargeParameter.DC_EVStatus,            sizeof(struct iso1DC_EVStatusType));
        memcpy(&pCache->stDataPublic.EVMaximumVoltageLimit,     &pReq->DC_EVChargeParameter.EVMaximumVoltageLimit,  sizeof(struct iso1PhysicalValueType));
        memcpy(&pCache->stDataPublic.EVMaximumCurrentLimit,     &pReq->DC_EVChargeParameter.EVMaximumCurrentLimit,  sizeof(struct iso1PhysicalValueType));
        
        if(pReq->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed == 1u)
        {
            pCache->stDataPublic.EVMaximumPowerLimit_isUsed = 1;
            memcpy(&pCache->stDataPublic.EVMaximumPowerLimit,   &pReq->DC_EVChargeParameter.EVMaximumPowerLimit,    sizeof(struct iso1PhysicalValueType));
        }
    }
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcRequestedEnergyTransferMode
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  11:45:18
 * @Description    :      ISO15118 ChargeParameterDiscoveryReq消息 RequestedEnergyTransferMode 字段 处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      eType             要检查的充电模式
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcRequestedEnergyTransferMode(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, iso1EnergyTransferModeType eType)
{
    i32 i;
    i32 i32TempCode;
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_WrongEnergyTransferMode))
    {
        i32TempCode = (*pCode);
        (*pCode) = iso1responseCodeType_FAILED_WrongEnergyTransferMode;
        
        for(i = 0; i < pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen; i++)
        {
            //[V2G2-476]
            if(eType == pCache->stData.stServiceDiscoveryRes.ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array[i])
            {
                (*pCode) = i32TempCode;
                break;
            }
        }
        
        if((*pCode) != iso1responseCodeType_OK)
        {
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
            
            return(false);
        }
    }
    
    return(true);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcDcEvChargeParameter
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  13:52:46
 * @Description    :      ISO15118 ChargeParameterDiscoveryReq消息 DC_EVChargeParameter 字段 处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      pDcEvParam        指向要检查DC_EVChargeParameter的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcDcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1DC_EVChargeParameterType *pDcEvParam)
{
    struct iso1PhysicalValueType EVSEMinimumCurrentLimit;
    struct iso1PhysicalValueType EVSEMinimumVoltageLimit;
    
    //代码严谨一点 是需要此条件的
    if(pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter_isUsed == 1u)
    {
        EVSEMinimumCurrentLimit = pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumCurrentLimit;
        EVSEMinimumVoltageLimit = pCache->stData.stChargeParamDiscoveryRes.DC_EVSEChargeParameter.EVSEMinimumVoltageLimit;
        
        //充电参数检查[V2G2-477]
        if(((pDcEvParam->FullSOC_isUsed == 1u) && (pDcEvParam->FullSOC > 100))                                      //EV端充电完成SOC大于100
        || ((pDcEvParam->BulkSOC_isUsed == 1u) && (pDcEvParam->BulkSOC > 100))                                      //EV端快充完成SOC大于100
        || (sV2gIso1CmpPhyValue(&pDcEvParam->EVMaximumCurrentLimit, &EVSEMinimumCurrentLimit, false) == false)      //EV端允许的最大电流 小于 EVSE端最小输出电流
        || (sV2gIso1CmpPhyValue(&pDcEvParam->EVMaximumVoltageLimit, &EVSEMinimumVoltageLimit, false) == false))     //EV端允许的最大电压 小于 EVSE端最小输出电压
        {
            //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
            if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_WrongChargeParameter))
            {
                (*pCode) = iso1responseCodeType_FAILED_WrongChargeParameter;
            }
            
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
            
            return(false);
        }
    }
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcAcEvChargeParameter
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  14:07:07
 * @Description    :      ISO15118 ChargeParameterDiscoveryReq消息 AC_EVChargeParameter 字段 处理
 * @Input          :      i32DevIndex       外设号
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pCode             指向储存ResponseCode的结构体指针
 * @Input          :      pAcEvParam        指向AC_EVChargeParameter的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcAcEvChargeParameter(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1AC_EVChargeParameterType *pAcEvParam)
{
    struct iso1PhysicalValueType EVSENominalVoltage;
    struct iso1PhysicalValueType EVSEMaxCurrent;
    
    //代码严谨一点 是需要此条件的
    if(pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter_isUsed == 1u)
    {
        EVSENominalVoltage  = pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.EVSENominalVoltage;
        EVSEMaxCurrent      = pCache->stData.stChargeParamDiscoveryRes.AC_EVSEChargeParameter.EVSEMaxCurrent;
        
        //充电参数检查[V2G2-477]
        if((sV2gIso1CmpPhyValue(&pAcEvParam->EVMaxCurrent, &EVSEMaxCurrent, false) == false)        //EV端允许的最大电流 小于 EVSE端最大输出电流
        || (sV2gIso1CmpPhyValue(&pAcEvParam->EVMaxVoltage, &EVSENominalVoltage, false) == false))   //EV端允许的最大电压 小于 EVSE端标称电压电压
        {
            //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
            if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_WrongChargeParameter))
            {
                (*pCode) = iso1responseCodeType_FAILED_WrongChargeParameter;
            }
            
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
            
            return(false);
        }
    }
    
    return(true);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcPublicDataPowerDelivery
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  14:20:40
 * @Description    :      ISO15118 协议 PowerDeliveryReq消息  公共字段 处理
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pReq              指向PowerDeliveryReq的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcPublicDataPowerDelivery(stV2gIso1Cache_t *pCache, struct iso1PowerDeliveryReqType *pReq)
{
    if(pReq->DC_EVPowerDeliveryParameter_isUsed == 1u)
    {
        memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVPowerDeliveryParameter.DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
        if(pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed == 1u)
        {
            pCache->stDataPublic.BulkChargingComplete        = pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete;
            pCache->stDataPublic.BulkChargingComplete_isUsed = 1u;
        }
        pCache->stDataPublic.ChargingComplete = pReq->DC_EVPowerDeliveryParameter.ChargingComplete;
    }
    
    return(true);
}











/*******************************************************************************
 * @FunctionName   :      sV2gIso1SAScheduleTupleIDCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  14:27:28
 * @Description    :      ISO15118 SAScheduleTupleID 检查函数
 * @Input          :      i32DevIndex           外设号
 * @Input          :      pCache                指向缓存结构体指针
 * @Input          :      pCode                 指向储存ResponseCode的结构体指针
 * @Input          :      SAScheduleTupleID     要检查的SAScheduleTupleID
 * @Return         :      
*******************************************************************************/
bool sV2gIso1SAScheduleTupleIDCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, i16 SAScheduleTupleID)
{
    i32 i;
    
    for(i = 0; i < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen; i++)
    {
        if(SAScheduleTupleID == pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[i].SAScheduleTupleID)
        {
            return(true);
        }
    }
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_TariffSelectionInvalid))
    {
        //[V2G2-479]
        (*pCode) = iso1responseCodeType_FAILED_TariffSelectionInvalid;
    }
    
    //[V2G2-539] [V2G2-034]
    pCache->pCbShutdownEmg(i32DevIndex);
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ChargingProfileCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月23日  18:03:25
 * @Description    :      ISO15118 ChargingProfile 字段 检查函数
 * @Input          :      i32DevIndex               外设号
 * @Input          :      pCache                    指向缓存结构体指针
 * @Input          :      pCode                     指向储存ResponseCode的结构体指针
 * @Input          :      pChargingProfile          指向要检查ChargingProfile的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ChargingProfileCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1ChargingProfileType *pChargingProfile)
{
    u16 i, j, k;
    f32 f32MaxPower, f32PMax;
    
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_ChargingProfileInvalid))
    {
        for(i = 0; i < pChargingProfile->ProfileEntry.arrayLen; i++)
        {
            for(j = 0; j < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.arrayLen; j++)
            {
                for(k = 0; k < pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[j].PMaxSchedule.PMaxScheduleEntry.arrayLen; k++)
                {
                    f32MaxPower = sV2gIso1GetPhyValue(&pChargingProfile->ProfileEntry.array[i].ChargingProfileEntryMaxPower, NULL);
                    f32PMax = sV2gIso1GetPhyValue(&pCache->stData.stChargeParamDiscoveryRes.SAScheduleList.SAScheduleTuple.array[j].PMaxSchedule.PMaxScheduleEntry.array[k].PMax, NULL);
                    if(f32MaxPower <= f32PMax)
                    {
                        //若ChargingProfileEntryMaxPower符合其中一个PMax
                        return(true);
                    }
                }
            }
        }
        
        //[V2G2-478]
        (*pCode) = iso1responseCodeType_FAILED_ChargingProfileInvalid;
        
        //[V2G2-539] [V2G2-034]
        pCache->pCbShutdownEmg(i32DevIndex);
    }
    
    return(false);
}












/*******************************************************************************
 * @FunctionName   :      sV2gIso1ProcPublicDataCurrentDemand
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  14:30:27
 * @Description    :      ISO15118 CurrentDemandReq消息  公共字段 处理
 * @Input          :      pCache            指向缓存结构体指针
 * @Input          :      pReq              指向要检查CurrentDemandReq的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ProcPublicDataCurrentDemand(stV2gIso1Cache_t *pCache, struct iso1CurrentDemandReqType *pReq)
{
    memcpy(&pCache->stDataPublic.DC_EVStatus, &pReq->DC_EVStatus, sizeof(struct iso1DC_EVStatusType));
    if(pReq->EVMaximumVoltageLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumVoltageLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumVoltageLimit, &pReq->EVMaximumVoltageLimit, sizeof(struct iso1PhysicalValueType));
    }
    if(pReq->EVMaximumCurrentLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumCurrentLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumCurrentLimit, &pReq->EVMaximumCurrentLimit, sizeof(struct iso1PhysicalValueType));
    }
    if(pReq->EVMaximumPowerLimit_isUsed == 1u)
    {
        pCache->stDataPublic.EVMaximumPowerLimit_isUsed = 1;
        memcpy(&pCache->stDataPublic.EVMaximumPowerLimit, &pReq->EVMaximumPowerLimit, sizeof(struct iso1PhysicalValueType));
    }
    if(pReq->BulkChargingComplete_isUsed == 1u)
    {
        pCache->stDataPublic.BulkChargingComplete        = pReq->BulkChargingComplete;
        pCache->stDataPublic.BulkChargingComplete_isUsed = 1u;
    }
    pCache->stDataPublic.ChargingComplete = pReq->ChargingComplete;
    
    memcpy(&pCache->stDataPublic.EVTargetCurrent, &pReq->EVTargetCurrent, sizeof(struct iso1PhysicalValueType));
    memcpy(&pCache->stDataPublic.EVTargetVoltage, &pReq->EVTargetVoltage, sizeof(struct iso1PhysicalValueType));
    
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sV2gIso1ContractIdCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月28日  15:58:47
 * @Description    :      ISO15118 ContractId 字段 检查函数
 * @Input          :      i32DevIndex               外设号
 * @Input          :      pCache                    指向缓存结构体指针
 * @Input          :      pCode                     指向储存ResponseCode的结构体指针
 * @Input          :      pReq                      指向要检查MeteringReceiptReq的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1ContractIdCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeteringReceiptReqType *pReq)
{
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_MeteringSignatureNotValid))
    {
        if(pCache->stData.stPaymentServiceSelectionReq.SelectedPaymentOption != iso1paymentOptionType_Contract)
        {
            //[V2G2-481]
            (*pCode) = iso1responseCodeType_FAILED_MeteringSignatureNotValid;
            
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
        }
        return(true);
    }
    
    return(true);
}














/*******************************************************************************
 * @FunctionName   :      sV2gIso1MeterInfoCheck
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月28日  15:25:25
 * @Description    :      ISO15118 MeterInfo 字段 检查函数
 * @Input          :      i32DevIndex           外设号
 * @Input          :      pCache                指向缓存结构体指针
 * @Input          :      pCode                 指向储存ResponseCode的结构体指针
 * @Input          :      pMeterInfo            指向要检查MeterInfo的结构体指针
 * @Return         :      
*******************************************************************************/
bool sV2gIso1MeterInfoCheck(i32 i32DevIndex, stV2gIso1Cache_t *pCache, iso1responseCodeType *pCode, struct iso1MeterInfoType *pMeterInfo)
{
    //注意响应码字段的优先级，根据<ISO15118-2:2014.pdf> @8.8.3 ResponseCode handling
    if(((*pCode) < iso1responseCodeType_FAILED) || ((*pCode) > iso1responseCodeType_FAILED_MeteringSignatureNotValid))
    {
        if(pCache->stData.stChargingStatusRes.MeterInfo_isUsed == 1)
        {
            if(memcmp(&pMeterInfo, &pCache->stData.stChargingStatusRes.MeterInfo, sizeof(struct iso1MeterInfoType)) == 0)
            {
                return(true);
            }
            
            //[V2G2-481]
            (*pCode) = iso1responseCodeType_FAILED_MeteringSignatureNotValid;
            
            //[V2G2-539] [V2G2-034]
            pCache->pCbShutdownEmg(i32DevIndex);
        }
    }
    
    return(false);
}
















