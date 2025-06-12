/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_din_print.c
* Description                           :   DIN70121 协议消息内容打印功能实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-08-24
* notice                                :   负责实现所有DIN70121消息的格式化打印
****************************************************************************************************/
#include "bms_ccs_v2g_din_print.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_din_print";







//---------------------------------------------------------------------------------------------------------

//dinresponseCodeType 字段字符串定义
char u8DinResponseCodeTypeString[dinresponseCodeType_FAILED_WrongEnergyTransferType + 1][128] =
{
    "0 ->OK,成功",
    "1 ->OK_NewSessionEstablished,成功，建立新的会话",
    "2 ->OK_OldSessionJoined,成功，加入旧的会话",
    "3 ->OK_CertificateExpiresSoon,成功，证书到期后不久",
    "4 ->FAILED,失败",
    "5 ->FAILED_SequenceError,失败，序列错误",
    "6 ->FAILED_ServiceIDInvalid,失败，ServiceID无效",
    "7 ->FAILED_UnknownSession,失败，未知的会话",
    "8 ->FAILED_ServiceSelectionInvalid,失败，服务选择无效",
    "9 ->FAILED_PaymentSelectionInvalid,失败，支付选择无效",
    "10->FAILED_CertificateExpired,失败，证书过期",
    "11->FAILED_SignatureError,失败，签名错误",
    "12->FAILED_NoCertificateAvailable,失败，没有可用证书",
    "13->FAILED_CertChainError,失败，证书链错误",
    "14->FAILED_ChallengeInvalid,失败，(合约认证的)查问无效",
    "15->FAILED_ContractCanceled,失败，合约取消",
    "16->FAILED_WrongChargeParameter,失败，充电参数错误",
    "17->FAILED_PowerDeliveryNotApplied,失败，PowerDelivery未应用",
    "18->FAILED_TariffSelectionInvalid,失败，费率选择无效",
    "19->FAILED_ChargingProfileInvalid,失败，充电概要文件无效",
    "20->FAILED_EVSEPresentVoltageToLow,失败，EVSE当前电压过低",
    "21->FAILED_MeteringSignatureNotValid,失败，计量签名无效",
    "22->FAILED_WrongEnergyTransferType,失败，能量传输类型错误",
};




//dinserviceCategoryType 字段字符串定义
char u8DinServiceCategoryString[dinserviceCategoryType_OtherCustom + 1][128] =
{
    "0 ->EVCharging,电动汽车充电服务",
    "1 ->Internet,互联网服务",
    "2 ->ContractCertificate,合同证书服务",
    "3 ->OtherCustom,其他服务服务",
};



//dinEVSESupportedEnergyTransferType 字段字符串定义
char u8DinEVSESupportedEnergyTransferTypeString[dinEVSESupportedEnergyTransferType_AC_core3p_DC_extended + 1][128] =
{
    "0 ->AC_single_phase_core,DIN70121不支持",
    "1 ->AC_three_phase_core,DIN70121不支持",
    "2 ->DC_core,基于IEC62196-3的核心引脚的直流充电",
    "3 ->DC_extended,基于扩展引脚的直流充电",
    "4 ->DC_combo_core,DIN70121不支持",
    "5 ->DC_dual,DIN70121不支持",
    "6 ->AC_core1p_DC_extended,DIN70121不支持",
    "7 ->AC_single_DC_core,DIN70121不支持",
    "8 ->AC_single_phase_three_phase_core_DC_extended,DIN70121不支持",
    "9 ->AC_core3p_DC_extended,DIN70121不支持",
};





//dinpaymentOptionType 字段字符串定义
char u8DinPaymentOptionTypeString[dinpaymentOptionType_ExternalPayment + 1][128] =
{
    "0 ->Contract,合约支付",
    "1 ->ExternalPayment,外部支付",
};



//dinEVSEProcessingType 字段字符串定义
char u8DinEVSEProcessingTypeString[dinEVSEProcessingType_Ongoing + 1][128] =
{
    "0 ->Finished,已完成",
    "1 ->Ongoing,正在进行中",
};




//dinEVRequestedEnergyTransferType 字段字符串定义
char u8DinEVRequestedEnergyTransferTypeString[dinEVRequestedEnergyTransferType_DC_unique + 1][128] =
{
    "0 ->AC_single_phase_core,DIN70121不支持",
    "1 ->AC_three_phase_core,DIN70121不支持",
    "2 ->DC_core,基于IEC62196-3的核心引脚的直流充电",
    "3 ->DC_extended,基于扩展引脚的直流充电",
    "4 ->DC_combo_core,DIN70121不支持",
    "5 ->DC_unique,DIN70121不支持",
};






//dinDC_EVErrorCodeType 字段字符串定义
char u8DinDC_EVErrorCodeTypeString[dinDC_EVErrorCodeType_NoData + 1][128] =
{
    "0 ->NO_ERROR,无错误",
    "1 ->FAILED_RESSTemperatureInhibit,电池温度抑制，太热或太冷而无法充电",
    "2 ->FAILED_EVShiftPosition,车辆档位不正确，不在停车挡",
    "3 ->FAILED_ChargerConnectorLockFault,充电桩连接口锁定故障",
    "4 ->FAILED_EVRESSMalfunction,车辆RESS（电池包）故障",
    "5 ->FAILED_ChargingCurrentdifferential,充电电流差",
    "6 ->FAILED_ChargingVoltageOutOfRange,充电电压超出范围",
    "7 ->Reserved_A,保留A",
    "8 ->Reserved_B,保留B",
    "9 ->Reserved_C,保留C",
    "10->FAILED_ChargingSystemIncompatibility,充电系统不兼容",
    "11->NoData,无数据",
};




//dinisolationLevelType 字段字符串定义
char u8DinIsolationLevelString[dinisolationLevelType_Fault + 1][128] =
{
    "0 ->Invalid,无效",
    "1 ->Valid,有效",
    "2 ->Warning,告警",
    "3 ->Fault,错误",
};






//dinDC_EVSEStatusCodeType 字段字符串定义
char u8DinDC_EVSEStatusCodeTypeString[dinDC_EVSEStatusCodeType_Reserved_C + 1][128] =
{
    "0 ->EVSE_NotReady,EVSE未就绪",
    "1 ->EVSE_Ready,EVSE就绪",
    "2 ->EVSE_Shutdown,EVSE停机",
    "3 ->EVSE_UtilityInterruptEvent,供电中断事件",
    "4 ->EVSE_IsolationMonitoringActive,绝缘监测触发",
    "5 ->EmergencyShutdown,紧急停机",
    "6 ->EVSE_Malfunction,发生故障",
    "7 ->Reserved_8,保留8",
    "8 ->Reserved_9,保留9",
    "9 ->Reserved_A,保留A",
    "10->Reserved_B,保留B",
    "11->Reserved_C,保留C",
};





//dinEVSENotificationType 字段字符串定义
char u8DinEVSENotificationTypeString[dinEVSENotificationType_ReNegotiation + 1][128] =
{
    "0 ->None,无动作",
    "1 ->StopCharging,停止充电",
    "2 ->ReNegotiation,重新协议协商",
};



//---------------------------------------------------------------------------------------------------------


void sV2gDinPrintSessionSetupReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintSessionSetupRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintServiceDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintServiceDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintServicePaymentSelectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintServicePaymentSelectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintContractAuthenticationReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintContractAuthenticationRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintPowerDeliveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintPowerDeliveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintSessionStopReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintSessionStopRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintCableCheckReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintCableCheckRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintPreChargeReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintPreChargeRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintCurrentDemandReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintCurrentDemandRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);

void sV2gDinPrintWeldingDetectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
void sV2gDinPrintWeldingDetectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);



//---------------------------------------------------------------------------------------------------------



//SessionSetupReq/Res
void sV2gDinPrintSessionSetupReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinSessionSetupReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.SessionSetupReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<SessionSetup Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->EVCCID                  :", pReq->EVCCID.bytes, pReq->EVCCID.bytesLen);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintSessionSetupRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinSessionSetupResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.SessionSetupRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>SessionSetup Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->EVSEID_Len              :%d", pRes->EVSEID.bytesLen);
    EN_HLOGD(TAG, "    ->EVSEID                  :", pRes->EVSEID.bytes, pRes->EVSEID.bytesLen);
    if(pRes->DateTimeNow_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DateTimeNow             :%lld", pRes->DateTimeNow);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//ServiceDiscoveryReq/Res
void sV2gDinPrintServiceDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinServiceDiscoveryReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ServiceDiscoveryReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ServiceDiscovery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->ServiceScope                                            :", (const u8 *)pReq->ServiceScope.characters, pReq->ServiceScope.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, "    ->ServiceCateg                                            :%s", u8DinServiceCategoryString[pReq->ServiceCategory]);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintServiceDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    u16 i;
    struct dinServiceDiscoveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ServiceDiscoveryRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ServiceDiscovery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    for(i=0; i<pRes->PaymentOptions.PaymentOption.arrayLen; i++)
    {
        EN_SLOGD(TAG, "    ->PaymentOptions#%d                                        :%s", i, u8DinPaymentOptionTypeString[pRes->PaymentOptions.PaymentOption.array[i]]);
    }
    EN_SLOGD(TAG, "    ->ChargeService->ServiceType->ServiceTag->ServiceID       :%d", pRes->ChargeService.ServiceTag.ServiceID);
    EN_SLOGD(TAG, "                                            ->ServiceName     :DIN70121不可用");
    EN_SLOGD(TAG, "                                            ->ServiceCateg    :%s", u8DinServiceCategoryString[pRes->ChargeService.ServiceTag.ServiceCategory]);
    EN_SLOGD(TAG, "                                            ->ServiceScope    :DIN70121不可用");
    EN_SLOGD(TAG, "                                ->FreeService                 :%d", pRes->ChargeService.FreeService);
    EN_SLOGD(TAG, "                   ->EnergyTransferType                       :%s", u8DinEVSESupportedEnergyTransferTypeString[pRes->ChargeService.EnergyTransferType]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//ServicePaymentSelectionReq/Res
void sV2gDinPrintServicePaymentSelectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    u16 i;
    struct dinServicePaymentSelectionReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ServicePaymentSelectionReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ServicePaymentSelection Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->SelectedPaymentOption                                   :%s", u8DinPaymentOptionTypeString[pReq->SelectedPaymentOption]);
    for(i=0; i<pReq->SelectedServiceList.SelectedService.arrayLen; i++)
    {
        EN_SLOGD(TAG, "    ->SelectedServiceList->SelectedService[%d]->ServiceID      :%d", i, pReq->SelectedServiceList.SelectedService.array[i].ServiceID);
        EN_SLOGD(TAG, "                                             ->ParameterSetID :DIN70121不可用");
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintServicePaymentSelectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinServicePaymentSelectionResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ServicePaymentSelectionRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ServicePaymentSelection Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//ContractAuthenticationReq/Res
void sV2gDinPrintContractAuthenticationReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ContractAuthentication Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintContractAuthenticationRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinContractAuthenticationResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ContractAuthenticationRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ContractAuthentication Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->EVSEProcessing          :%s", u8DinEVSEProcessingTypeString[pRes->EVSEProcessing]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//ChargeParameterDiscoveryReq/Res
void sV2gDinPrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    struct dinChargeParameterDiscoveryReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ChargeParameterDiscoveryReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ChargeParameterDiscovery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                    :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->EVRequestedEnergyTransferType                                :%s", u8DinEVRequestedEnergyTransferTypeString[pReq->EVRequestedEnergyTransferType]);
    EN_SLOGD(TAG, "    ->AC_EVChargeParameter                                         :DIN70121不可用");
    EN_SLOGD(TAG, "    ->DC_EVChargeParameter->DC_EVStatus->EVReady                   :%d", pReq->DC_EVChargeParameter.DC_EVStatus.EVReady);
    if(pReq->DC_EVChargeParameter.DC_EVStatus.EVCabinConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                                       ->EVCabinConditioning       :%d", pReq->DC_EVChargeParameter.DC_EVStatus.EVCabinConditioning);
    }
    if(pReq->DC_EVChargeParameter.DC_EVStatus.EVRESSConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                                       ->EVRESSConditioning        :%d", pReq->DC_EVChargeParameter.DC_EVStatus.EVRESSConditioning);
    }
    EN_SLOGD(TAG, "                                       ->EVErrorCode               :%s", u8DinDC_EVErrorCodeTypeString[pReq->DC_EVChargeParameter.DC_EVStatus.EVErrorCode]);
    EN_SLOGD(TAG, "                                       ->EVRESSSOC                 :%d%%", pReq->DC_EVChargeParameter.DC_EVStatus.EVRESSSOC);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumCurrentLimit, u8Unit);
    EN_SLOGD(TAG, "                          ->EVMaximumCurrentLimit                  :%0.3f %s", f32Value, u8Unit);
    
    if(pReq->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMaximumPowerLimit                    :%0.3f %s", f32Value, u8Unit);
    }
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumVoltageLimit, u8Unit);
    EN_SLOGD(TAG, "                          ->EVMaximumVoltageLimit                  :%0.3f %s", f32Value, u8Unit);
    
    if(pReq->DC_EVChargeParameter.EVEnergyCapacity_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->DC_EVChargeParameter.EVEnergyCapacity, u8Unit);
        EN_SLOGD(TAG, "                          ->EVEnergyCapacity                       :%0.3f %s", f32Value, u8Unit);
    }
    
    if(pReq->DC_EVChargeParameter.EVEnergyRequest_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->DC_EVChargeParameter.EVEnergyRequest, u8Unit);
        EN_SLOGD(TAG, "                          ->EVEnergyRequest                        :%0.3f %s", f32Value, u8Unit);
    }
    
    if(pReq->DC_EVChargeParameter.FullSOC_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                          ->FullSOC                                :%d%%", pReq->DC_EVChargeParameter.FullSOC);
    }
    
    if(pReq->DC_EVChargeParameter.BulkSOC_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                          ->BulkSOC                                :%d%%", pReq->DC_EVChargeParameter.BulkSOC);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    u8   i,j;
    f32  f32Value;
    char u8Unit[8];
    struct dinChargeParameterDiscoveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes;
    
    //此消息打印量较大 增加一些延时 否则日志会有丢失
    vTaskDelay(200 / portTICK_RATE_MS);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ChargeParameterDiscovery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                    :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                                 :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    vTaskDelay(20 / portTICK_RATE_MS);
    EN_SLOGD(TAG, "    ->EVSEProcessing                                               :%s", u8DinEVSEProcessingTypeString[pRes->EVSEProcessing]);
    for(i = 0; i< pRes->SAScheduleList.SAScheduleTuple.arrayLen; i++)
    {
        EN_SLOGD(TAG, "    ->SAScheduleList->SAScheduleTuple#%d", i + 1);
        EN_SLOGD(TAG, "                    ->SAScheduleTuple->SAScheduleTupleID           :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].SAScheduleTupleID);
        EN_SLOGD(TAG, "                                     ->PMaxSchedule->PMaxScheduleID:%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleID);
        for(j=0;j<pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.arrayLen; j++)
        {
            EN_SLOGD(TAG, "                                                   ->PMaxScheduleEntry#%d", j + 1);
            if(pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval_isUsed == 1u)
            {
                if(pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.duration_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                                   ->PMaxScheduleEntry->RelativeTimeInterval->duration :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.duration);
                }
                EN_SLOGD(TAG, "                                                                                            ->start    :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.start);
            }
            EN_SLOGD(TAG, "                                                                      ->PMax                           :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].PMax);
        }
    }
    vTaskDelay(20 / portTICK_RATE_MS);
    
    EN_SLOGD(TAG, "    ->AC_EVSEChargeParameter                                       :DIN70121不可用");
    if(pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEChargeParameter->DC_EVSEStatus->EVSEIsolationStatus   :%s", u8DinIsolationLevelString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEChargeParameter->DC_EVSEStatus->EVSEStatusCode        :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                                           ->NotificationMaxDelay  :%d", pRes->DC_EVSEChargeParameter.DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                                           ->EVSENotification      :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSENotification]);
    vTaskDelay(20 / portTICK_RATE_MS);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, u8Unit);
    EN_SLOGD(TAG, "                            ->EVSEMaximumCurrentLimit              :%0.3f %s", f32Value, u8Unit);
    
    if(pRes->DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMaximumPowerLimit                :%0.3f %s", f32Value, u8Unit);
    }
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit, u8Unit);
    EN_SLOGD(TAG, "                            ->EVSEMaximumVoltageLimit              :%0.3f %s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit, u8Unit);
    EN_SLOGD(TAG, "                            ->EVSEMinimumCurrentLimit              :%0.3f %s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit, u8Unit);
    EN_SLOGD(TAG, "                            ->EVSEMinimumVoltageLimit              :%0.3f %s", f32Value, u8Unit);
    
    if(pRes->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSECurrentRegulationTolerance       :%0.3f %s", f32Value, u8Unit);
    }
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEPeakCurrentRipple, u8Unit);
    EN_SLOGD(TAG, "                            ->EVSEPeakCurrentRipple                :%0.3f %s", f32Value, u8Unit);
    
    if(pRes->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEEnergyToBeDelivered              :%0.3f %s", f32Value, u8Unit);
    }
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//PowerDeliveryReq/Res
void sV2gDinPrintPowerDeliveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    u16 i;
    struct dinPowerDeliveryReqType *pReq = NULL;
    struct dinDC_EVStatusType *pDcEvStatus = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PowerDeliveryReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PowerDelivery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                   :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ReadyToChargeState                                          :%d", pReq->ReadyToChargeState);
    if(pReq->ChargingProfile_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ChargingProfile->SAScheduleTupleID                          :%d", pReq->ChargingProfile.SAScheduleTupleID);
        for(i=0; i<pReq->ChargingProfile.ProfileEntry.arrayLen; i++)
        {
            EN_SLOGD(TAG, "                     ->ProfileEntry#%d", i);
            EN_SLOGD(TAG, "                     ->ProfileEntry->ChargingProfileEntryStart    :%d", pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryStart);
            EN_SLOGD(TAG, "                                   ->ChargingProfileEntryMaxPower :%d", pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryMaxPower);
        }
    }
    
    if(pReq->DC_EVPowerDeliveryParameter_isUsed == 1u)
    {
        pDcEvStatus  = &pReq->DC_EVPowerDeliveryParameter.DC_EVStatus;
        EN_SLOGD(TAG, "    ->DC_EVChargeParameter->DC_EVStatus->EVReady                  :%d", pDcEvStatus->EVReady);
        if(pDcEvStatus->EVCabinConditioning_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                                       ->EVCabinConditioning      :%d", pDcEvStatus->EVCabinConditioning);
        }
        if(pDcEvStatus->EVRESSConditioning_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                                       ->EVRESSConditioning       :%d", pDcEvStatus->EVRESSConditioning);
        }
        EN_SLOGD(TAG, "                                       ->EVErrorCode              :%s", u8DinDC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
        EN_SLOGD(TAG, "                                       ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
        
        if(pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                          ->BulkChargingComplete(快充完成标识)    :%d", pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete);
        }
        EN_SLOGD(TAG, "                          ->ChargingComplete(充电完成标识)        :%d", pReq->DC_EVPowerDeliveryParameter.ChargingComplete);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintPowerDeliveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinPowerDeliveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PowerDeliveryRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PowerDelivery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                   :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                                :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    
    EN_SLOGD(TAG, "    ->AC_EVSEStatus                                               :DIN70121不可用");
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEIsolationStatus                          :%s", u8DinIsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEStatusCode                               :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                   ->NotificationMaxDelay                         :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification                             :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}






//SessionStopReq/Res
void sV2gDinPrintSessionStopReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<SessionStop Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintSessionStopRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinSessionStopResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.SessionStopRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>SessionStop Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//CableCheckReq/Res
void sV2gDinPrintCableCheckReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinDC_EVStatusType *pDcEvStatus = NULL;
    
    pDcEvStatus = &pDoc->V2G_Message.Body.CableCheckReq.DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CableCheck Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDoc->V2G_Message.Body.CableCheckReq.DC_EVStatus.EVReady);
    if(pDcEvStatus->EVCabinConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVCabinConditioning      :%d", pDcEvStatus->EVCabinConditioning);
    }
    if(pDcEvStatus->EVRESSConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVRESSConditioning       :%d", pDcEvStatus->EVRESSConditioning);
    }
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8DinDC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintCableCheckRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinCableCheckResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CableCheckRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CableCheck Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEIsolationStatus    :%s", u8DinIsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEStatusCode         :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                   ->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    
    EN_SLOGD(TAG, "    ->EVSEProcessing                        :%s", u8DinEVSEProcessingTypeString[pRes->EVSEProcessing]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//PreChargeReq/Res
void sV2gDinPrintPreChargeReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct dinPreChargeReqType *pReq = NULL;
    struct dinDC_EVStatusType *pDcEvStatus = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PreChargeReq;
    pDcEvStatus = &pReq->DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PreCharge Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDcEvStatus->EVReady);
    if(pDcEvStatus->EVCabinConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVCabinConditioning      :%d", pDcEvStatus->EVCabinConditioning);
    }
    if(pDcEvStatus->EVRESSConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVRESSConditioning       :%d", pDcEvStatus->EVRESSConditioning);
    }
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8DinDC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->EVTargetVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetVoltage                       :%0.3f%s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->EVTargetCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetCurrent                       :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintPreChargeRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct dinPreChargeResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PreChargeRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PreCharge Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEIsolationStatus    :%s", u8DinIsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEStatusCode         :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                   ->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//CurrentDemandReq/Res
void sV2gDinPrintCurrentDemandReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct dinCurrentDemandReqType *pReq = NULL;
    struct dinDC_EVStatusType *pDcEvStatus = NULL;
    
    pReq = &pDoc->V2G_Message.Body.CurrentDemandReq;
    pDcEvStatus = &pReq->DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CurrentDemand Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDcEvStatus->EVReady);
    if(pDcEvStatus->EVCabinConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVCabinConditioning      :%d", pDcEvStatus->EVCabinConditioning);
    }
    if(pDcEvStatus->EVRESSConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVRESSConditioning       :%d", pDcEvStatus->EVRESSConditioning);
    }
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8DinDC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->EVTargetCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetCurrent                       :%0.3f%s", f32Value, u8Unit);
    
    if(pReq->EVMaximumVoltageLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->EVMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVMaximumVoltageLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->EVMaximumCurrentLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->EVMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVMaximumCurrentLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->EVMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->EVMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVMaximumPowerLimit                   :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->BulkChargingComplete_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->BulkChargingComplete                  :%d", pReq->BulkChargingComplete);
    }
    EN_SLOGD(TAG, "    ->ChargingComplete                      :%d", pReq->ChargingComplete);
    
    if(pReq->RemainingTimeToFullSoC_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->RemainingTimeToFullSoC, u8Unit);
        EN_SLOGD(TAG, "    ->RemainingTimeToFullSoC                :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->RemainingTimeToBulkSoC_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pReq->RemainingTimeToBulkSoC, u8Unit);
        EN_SLOGD(TAG, "    ->RemainingTimeToBulkSoC                :%0.3f%s", f32Value, u8Unit);
    }
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pReq->EVTargetVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetVoltage                       :%0.3f%s", f32Value, u8Unit);
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintCurrentDemandRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct dinCurrentDemandResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CurrentDemandRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CurrentDemand Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEIsolationStatus    :%s", u8DinIsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEStatusCode         :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                   ->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->EVSEPresentCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentCurrent                    :%0.3f%s", f32Value, u8Unit);
    
    EN_SLOGD(TAG, "    ->EVSECurrentLimitAchieved              :%d", pRes->EVSECurrentLimitAchieved);
    EN_SLOGD(TAG, "    ->EVSEVoltageLimitAchieved              :%d", pRes->EVSEVoltageLimitAchieved);
    EN_SLOGD(TAG, "    ->EVSEPowerLimitAchieved                :%d", pRes->EVSEPowerLimitAchieved);
    
    if(pRes->EVSEMaximumVoltageLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->EVSEMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumVoltageLimit               :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pRes->EVSEMaximumCurrentLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->EVSEMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumCurrentLimit               :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pRes->EVSEMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gDinGetPhyValue(&pRes->EVSEMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumPowerLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}





//WeldingDetectionReq/Res
void sV2gDinPrintWeldingDetectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    struct dinDC_EVStatusType *pDcEvStatus = NULL;
    
    pDcEvStatus = &pDoc->V2G_Message.Body.WeldingDetectionReq.DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<WeldingDetection Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDcEvStatus->EVReady);
    if(pDcEvStatus->EVCabinConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVCabinConditioning      :%d", pDcEvStatus->EVCabinConditioning);
    }
    if(pDcEvStatus->EVRESSConditioning_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                 ->EVRESSConditioning       :%d", pDcEvStatus->EVRESSConditioning);
    }
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8DinDC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gDinPrintWeldingDetectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct dinWeldingDetectionResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.WeldingDetectionRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>WeldingDetection Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8DinResponseCodeTypeString[pRes->ResponseCode]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEIsolationStatus    :%s", u8DinIsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->EVSEStatusCode         :%s", u8DinDC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "                   ->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8DinEVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gDinGetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}







