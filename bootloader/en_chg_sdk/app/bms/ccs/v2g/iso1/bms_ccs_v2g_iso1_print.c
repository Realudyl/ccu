/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_iso1_print.c
* Description                           :   ISO15118v1 协议消息内容打印功能实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-08-24
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_v2g_iso1.h"
#include "bms_ccs_v2g_iso1_basic.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_iso1_print";








//---------------------------------------------------------------------------------------------------------
void sV2gIso1PrintServiceList(u8 u8Space, struct iso1ServiceListType *pList);

void sV2gIso1PrintSessionSetupReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintSessionSetupRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintServiceDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintServiceDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintServiceDetailReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintServiceDetailRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintPaymentServiceSelectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintPaymentServiceSelectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintCertificateInstallationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintCertificateInstallationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintCertificateUpdateReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintCertificateUpdateRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintPaymentDetailsReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintPaymentDetailsRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintAuthorizationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintAuthorizationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintCableCheckReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintCableCheckRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintPreChargeReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintPreChargeRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintPowerDeliveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintPowerDeliveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintChargingStatusReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintChargingStatusRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintCurrentDemandReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintCurrentDemandRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintMeteringReceiptReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintMeteringReceiptRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintWeldingDetectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintWeldingDetectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

void sV2gIso1PrintSessionStopReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
void sV2gIso1PrintSessionStopRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);

//---------------------------------------------------------------------------------------------------------



//iso1responseCodeType 字段字符串定义
char u8Iso1ResponseCodeTypeString[iso1responseCodeType_FAILED_CertificateRevoked + 1][128] =
{
    "0 ->OK,成功",
    "1 ->OK_NewSessionEstablished,成功，建立新的会话",
    "2 ->OK_OldSessionJoined,成功，加入旧的会话",
    "3 ->OK_CertificateExpiresSoon,成功，证书即将过期",
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

    "20->FAILED_MeteringSignatureNotValid,失败，计量签名无效",
    "21->FAILED_NoChargeServiceSelected,失败，充电服务未选择",
    "22->FAILED_WrongEnergyTransferType,失败，能量传输类型错误",
    "23->FAILED_ContactorError,失败，接触器错误",
    "24->FAILED_CertificateNotAllowedAtThisEVSE,失败，此EVSE不允许证书",
    "25->FAILED_CertificateRevoked,失败，证书撤销",
};





//iso1serviceCategoryType 字段字符串定义
char u8Iso1ServiceCategoryString[iso1serviceCategoryType_OtherCustom + 1][128] =
{
    "0 ->EVCharging,电动汽车充电服务",
    "1 ->Internet,互联网服务",
    "2 ->ContractCertificate,合同证书服务",
    "3 ->OtherCustom,其他服务服务",
};



//iso1paymentOptionType 字段字符串定义
char u8Iso1PaymentOptionTypeString[iso1paymentOptionType_ExternalPayment + 1][128] =
{
    "0 ->Contract,合约支付",
    "1 ->ExternalPayment,外部支付",
};






//iso1EnergyTransferModeType 字段字符串定义
char u8Iso1EVSESupportedEnergyTransferTypeString[iso1EnergyTransferModeType_DC_unique + 1][128] =
{
    "0 ->AC_single_phase_core,符合IEC 62196的交流单相充电",
    "1 ->AC_three_phase_core,符合IEC 62196的交流三相充电",
    "2 ->DC_core,基于IEC62196-3的核心引脚的直流充电",
    "3 ->DC_extended,基于扩展引脚的直流充电",
    "4 ->DC_combo_core,基于IEC 62196-3配置EE或配置FF连接器核心引脚的直流充电",
    "5 ->DC_unique,基于专用直流耦合器的直流充电。",
};






//iso1DC_EVErrorCodeType 字段字符串定义
char u8Iso1DC_EVErrorCodeTypeString[iso1DC_EVErrorCodeType_NoData + 1][128] =
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






//iso1isolationLevelType 字段字符串定义
char u8Iso1IsolationLevelString[iso1isolationLevelType_No_IMD + 1][128] =
{
    "0 ->Invalid,无效",
    "1 ->Valid,有效",
    "2 ->Warning,告警",
    "3 ->Fault,错误",
    "4 ->No_IMD,不支持IMD",
};






//iso1DC_EVSEStatusCodeType 字段字符串定义
char u8Iso1DC_EVSEStatusCodeTypeString[iso1DC_EVSEStatusCodeType_Reserved_C + 1][128] =
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






//iso1EVSEProcessingType 字段字符串定义
char u8Iso1EVSEProcessingTypeString[iso1EVSEProcessingType_Ongoing_WaitingForCustomerInteraction + 1][128] =
{
    "0 ->Finished,已完成",
    "1 ->Ongoing,正在进行中",
    "2 ->Ongoing_WaitingForCustomerInteraction,正在进行中_等待用户交互",
};






//iso1EVSENotificationType 字段字符串定义
char u8Iso1EVSENotificationTypeString[iso1EVSENotificationType_ReNegotiation + 1][128] =
{
    "0 ->None,无动作",
    "1 ->StopCharging,停止充电",
    "2 ->ReNegotiation,重新协商",
};





//iso1chargeProgressType 字段字符串定义
char u8Iso1ChargeProgressTypeString[iso1chargeProgressType_Renegotiate + 1][128] =
{
    "0 ->Start,开始充电",
    "1 ->Stop,停止充电",
    "2 ->Renegotiate,重新协商",
};





//iso1chargingSessionType 字段字符串定义
char u8Iso1ChargeSessionTypeString[iso1chargingSessionType_Pause + 1][128] =
{
    "0 ->Terminate,终止V2G会话",
    "1 ->Pause,暂停V2G会话",
};






//---------------------------------------------------------------------------------------------------------
void sV2gIso1PrintServiceList(u8 u8Space, struct iso1ServiceListType *pList)
{
    u8 i,j;
    char u8FmtString[256];
    char u8InfoString[512];
    char u8String[iso1ChargeServiceType_ServiceScope_CHARACTERS_SIZE];
    
    //1：先获取缩进格式
    memset(u8FmtString, 0, sizeof(u8FmtString));
    for(i=0; i<u8Space; i++)
    {
        u8FmtString[i] = ' ';
    }
    
    //2：打印总条目信息
    memset(u8InfoString, 0, sizeof(u8InfoString));
    snprintf(u8InfoString, sizeof(u8InfoString), "%s->ServiceList(支持的服务列表):", u8FmtString);
    EN_SLOGD(TAG, "%s", u8InfoString);
    
    //3：打印单个条目信息
    for(i=0; i<pList->Service.arrayLen; i++)
    {
        memset(u8InfoString, 0, sizeof(u8InfoString));
        snprintf(u8InfoString, sizeof(u8InfoString), "%s->Service#%d:", u8FmtString, i);
        EN_SLOGD(TAG, "%s", u8InfoString);
        
        memset(u8InfoString, 0, sizeof(u8InfoString));
        snprintf(u8InfoString, sizeof(u8InfoString), "%s         ->ServiceID:%d", u8FmtString, pList->Service.array[i].ServiceID);
        EN_SLOGD(TAG, "%s", u8InfoString);
        
        if(pList->Service.array[i].ServiceName_isUsed == 1u)
        {
            memset(u8String, 0, sizeof(u8String));
            for(j=0; j<pList->Service.array[i].ServiceName.charactersLen; j++)
            {
                u8String[j] = pList->Service.array[i].ServiceName.characters[j];
            }
            memset(u8InfoString, 0, sizeof(u8InfoString));
            snprintf(u8InfoString, sizeof(u8InfoString), "%s         ->ServiceName:%s", u8FmtString, u8String);
            EN_SLOGD(TAG, "%s", u8InfoString);
        }
        
        memset(u8InfoString, 0, sizeof(u8InfoString));
        snprintf(u8InfoString, sizeof(u8InfoString), "%s         ->ServiceCateg:%s", u8FmtString, u8Iso1ServiceCategoryString[pList->Service.array[i].ServiceCategory]);
        EN_SLOGD(TAG, "%s", u8InfoString);
        
        if(pList->Service.array[i].ServiceScope_isUsed == 1u)
        {
            memset(u8String, 0, sizeof(u8String));
            for(j=0; j<pList->Service.array[i].ServiceScope.charactersLen; j++)
            {
                u8String[j] = pList->Service.array[i].ServiceScope.characters[j];
            }
            memset(u8InfoString, 0, sizeof(u8InfoString));
            snprintf(u8InfoString, sizeof(u8InfoString), "%s         ->ServiceScope:%s", u8FmtString, u8String);
            EN_SLOGD(TAG, "%s", u8InfoString);
        }
        
        memset(u8InfoString, 0, sizeof(u8InfoString));
        snprintf(u8InfoString, sizeof(u8InfoString), "%s         ->FreeService:%d", u8FmtString, pList->Service.array[i].FreeService);
        EN_SLOGD(TAG, "%s", u8InfoString);
    }
}










//SessionSetupReq/Res
void sV2gIso1PrintSessionSetupReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1SessionSetupReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.SessionSetupReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<SessionSetup Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->EVCCID                  :", pReq->EVCCID.bytes, pReq->EVCCID.bytesLen);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintSessionSetupRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1SessionSetupResType *pRes = NULL;
    
    u16 i;
    char u8EvseId[iso1SessionSetupResType_EVSEID_CHARACTERS_SIZE];
    
    pRes = &pDoc->V2G_Message.Body.SessionSetupRes;
    
    memset(u8EvseId, 0, sizeof(u8EvseId));
    for(i=0; i<pRes->EVSEID.charactersLen; i++)
    {
        u8EvseId[i] = pRes->EVSEID.characters[i];
    }
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>SessionSetup Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->EVSEID                  :%s", u8EvseId);
    if(pRes->EVSETimeStamp_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->EVSETimeStamp           :%lld", pRes->EVSETimeStamp);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}






//ServiceDiscoveryReq/Res
void sV2gIso1PrintServiceDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1ServiceDiscoveryReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ServiceDiscoveryReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ServiceDiscovery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                           :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body                                                      ");
    if(pReq->ServiceScope_isUsed == 1u)
    {
        EN_HLOGD(TAG, "    ->ServiceScope                                        :", (const u8 *)pReq->ServiceScope.characters, pReq->ServiceScope.charactersLen * sizeof(exi_string_character_t));
    }
    if(pReq->ServiceCategory_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ServiceCateg                                        :%s", u8Iso1ServiceCategoryString[pReq->ServiceCategory]);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintServiceDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1ServiceDiscoveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ServiceDiscoveryRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ServiceDiscovery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                           :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                        :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->PaymentOptionList(支持的支付方式列表):");
    for(i=0; i<pRes->PaymentOptionList.PaymentOption.arrayLen; i++)
    {
        EN_SLOGD(TAG, "                       ->PaymentOption#%d                  :%s", i, u8Iso1PaymentOptionTypeString[pRes->PaymentOptionList.PaymentOption.array[i]]);
    }
    EN_SLOGD(TAG, "    ->ChargeService->ServiceType->ServiceID               :%d", pRes->ChargeService.ServiceID);
    
    if(pRes->ChargeService.ServiceName_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                ->ServiceName             :", (const u8*)pRes->ChargeService.ServiceName.characters, pRes->ChargeService.ServiceName.charactersLen * sizeof(exi_string_character_t));
    }
    EN_SLOGD(TAG, "                                ->ServiceCateg            :%s", u8Iso1ServiceCategoryString[pRes->ChargeService.ServiceCategory]);
    
    if(pRes->ChargeService.ServiceScope_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                ->ServiceScope            :", (const u8*)pRes->ChargeService.ServiceScope.characters, pRes->ChargeService.ServiceScope.charactersLen * sizeof(exi_string_character_t));
    }
    EN_SLOGD(TAG, "                                ->FreeService             :%d", pRes->ChargeService.FreeService);
    EN_SLOGD(TAG, "                   ->SupportedEnergyTransferMode          ");
    for(i=0; i<pRes->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen; i++)
    {
        EN_SLOGD(TAG, "                                                ->Mode#%d  :%s", i, u8Iso1EVSESupportedEnergyTransferTypeString[pRes->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array[i]]);
    }
    
    if(pRes->ServiceList_isUsed == 1u)
    {
        sV2gIso1PrintServiceList(4, &pRes->ServiceList);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}








//ServiceDetailReq/Res
void sV2gIso1PrintServiceDetailReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1ServiceDetailReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ServiceDetailReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ServiceDetail Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                           :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ServiceID                                           :%d", pReq->ServiceID);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintServiceDetailRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    u16 i,j;
    struct iso1ServiceDetailResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ServiceDetailRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ServiceDetail Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                           :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                        :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->ServiceID                                           :%d", pRes->ServiceID);
    if(pRes->ServiceParameterList_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ServiceParameterList(服务参数列表)");
        for(i=0; i<pRes->ServiceParameterList.ParameterSet.arrayLen; i++)
        {
            EN_SLOGD(TAG, "                       ->ParameterSet#%d                  ", i);
            EN_SLOGD(TAG, "                       ->ParameterSet->ParameterSetID     :%d", pRes->ServiceParameterList.ParameterSet.array[i].ParameterSetID);
            for(j=0; j<pRes->ServiceParameterList.ParameterSet.array[i].Parameter.arrayLen; j++)
            {
                    EN_SLOGD(TAG, "                                     ->Parameter->Name       :%s", pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].Name.characters);
                if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].boolValue_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                            ->boolValue      :%d", pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].boolValue);
                }
                else if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].byteValue_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                            ->byteValue      :%d", pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].byteValue);
                }
                else if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].shortValue_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                            ->shortValue     :%d", pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].shortValue);
                }
                else if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].intValue_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                            ->intValue       :%d", pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].intValue);
                }
                else if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].physicalValue_isUsed == 1u)
                {
                    memset(u8Unit ,0, sizeof(u8Unit));
                    f32Value = sV2gIso1GetPhyValue(&pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].physicalValue, u8Unit);
                    EN_SLOGD(TAG, "                                            ->physicalValue  :%0.3f %s", f32Value, u8Unit);
                }
                else if(pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].stringValue_isUsed == 1u)
                {
                    EN_HLOGD(TAG, "                                            ->stringValue    :", (const u8*)pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].stringValue.characters, pRes->ServiceParameterList.ParameterSet.array[i].Parameter.array[j].stringValue.charactersLen * sizeof(exi_string_character_t));
                }
            }
        }
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//PaymentServiceSelectionReq/Res
void sV2gIso1PrintPaymentServiceSelectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1PaymentServiceSelectionReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PaymentServiceSelectionReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PaymentServiceSelection Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->PaymentSelectedOption                                   :%s", u8Iso1PaymentOptionTypeString[pReq->SelectedPaymentOption]);
    EN_SLOGD(TAG, "    ->SelectedServiceList                                      ");
    for(i=0; i<pReq->SelectedServiceList.SelectedService.arrayLen; i++)
    {
        EN_SLOGD(TAG, "                         ->SelectedService#%d                  ", i);
        EN_SLOGD(TAG, "                                             ->ServiceID      :%d", pReq->SelectedServiceList.SelectedService.array[i].ServiceID);
        if(pReq->SelectedServiceList.SelectedService.array[i].ParameterSetID_isUsed == 1u)
        {
        EN_SLOGD(TAG, "                                             ->ParameterSetID :%d", pReq->SelectedServiceList.SelectedService.array[i].ParameterSetID);
        }
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintPaymentServiceSelectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1PaymentServiceSelectionResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PaymentServiceSelectionRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PaymentServiceSelection Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//CertificateInstallationReq/Res
void sV2gIso1PrintCertificateInstallationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1CertificateInstallationReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.CertificateInstallationReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CertificateInstallation Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->Id                                                      :", (const u8*)pReq->Id.characters, pReq->Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "    ->OEMProvisioningCert                                     :", pReq->OEMProvisioningCert.bytes, pReq->OEMProvisioningCert.bytesLen);
    for(i=0;i<pReq->ListOfRootCertificateIDs.RootCertificateID.arrayLen;i++)
    {
        EN_SLOGD(TAG, "    ->ListOfRootCertificateIDs->RootCertificateID#%d          ", i);
        EN_HLOGD(TAG, "                              ->RootCertificateID->X509IssuerName    :", (const u8*)pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509IssuerName.characters, pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509IssuerName.charactersLen * sizeof(exi_string_character_t));
        EN_HLOGD(TAG, "                                                 ->X509SerialNumber  :", (const u8*)pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509SerialNumber.data, pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509SerialNumber.len * sizeof(exi_string_character_t));
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintCertificateInstallationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1CertificateInstallationResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CertificateInstallationRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CertificateInstallation Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    
    EN_SLOGD(TAG, "    ->SAProvisioningCertificateChain                          ");
    if(pRes->SAProvisioningCertificateChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                    ->Id                      :", (const u8*)pRes->SAProvisioningCertificateChain.Id.characters, pRes->SAProvisioningCertificateChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                    ->Certificate             :", pRes->SAProvisioningCertificateChain.Certificate.bytes, pRes->SAProvisioningCertificateChain.Certificate.bytesLen);
    
    if(pRes->SAProvisioningCertificateChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.array[i].bytes, pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    
    EN_SLOGD(TAG, "    ->ContractSignatureCertChain                              ");
    if(pRes->ContractSignatureCertChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                    ->Id                      :", (const u8*)pRes->ContractSignatureCertChain.Id.characters, pRes->ContractSignatureCertChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                    ->Certificate             :", pRes->ContractSignatureCertChain.Certificate.bytes, pRes->ContractSignatureCertChain.Certificate.bytesLen);
    if(pRes->ContractSignatureCertChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pRes->ContractSignatureCertChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pRes->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytes, pRes->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    
    EN_HLOGD(TAG, "    ->ContractSignatureEncryptedPrivateKey->Id                 :", (const u8*)pRes->ContractSignatureEncryptedPrivateKey.Id.characters, pRes->ContractSignatureEncryptedPrivateKey.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "                                          ->CONTENT            :", pRes->ContractSignatureEncryptedPrivateKey.CONTENT.bytes, pRes->ContractSignatureEncryptedPrivateKey.CONTENT.bytesLen);
    
    EN_HLOGD(TAG, "    ->DHpublickey->Id                                          :", (const u8*)pRes->DHpublickey.Id.characters, pRes->DHpublickey.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "                 ->CONTENT                                     :", pRes->DHpublickey.CONTENT.bytes, pRes->DHpublickey.CONTENT.bytesLen);
    
    EN_HLOGD(TAG, "    ->eMAID->Id                                                :", (const u8*)pRes->eMAID.Id.characters, pRes->eMAID.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "           ->CONTENT                                           :", (const u8*)pRes->eMAID.CONTENT.characters, pRes->eMAID.CONTENT.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}











//CertificateUpdateReq/Res
void sV2gIso1PrintCertificateUpdateReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1CertificateUpdateReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.CertificateUpdateReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CertificateUpdate Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->Id                                                      :", (const u8*)pReq->Id.characters, pReq->Id.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, "    ->ContractSignatureCertChain                              ");
    if(pReq->ContractSignatureCertChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                ->Id                          :",(const u8*) pReq->ContractSignatureCertChain.Id.characters, pReq->ContractSignatureCertChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                ->Certificate                 :", pReq->ContractSignatureCertChain.Certificate.bytes, pReq->ContractSignatureCertChain.Certificate.bytesLen);
    
    if(pReq->ContractSignatureCertChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pReq->ContractSignatureCertChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pReq->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytes, pReq->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    
    EN_HLOGD(TAG, "    ->eMAID                                                   :", (const u8*)pReq->eMAID.characters, pReq->eMAID.charactersLen * sizeof(exi_string_character_t));
    
    for(i=0;i<pReq->ListOfRootCertificateIDs.RootCertificateID.arrayLen;i++)
    {
        EN_SLOGD(TAG, "    ->ListOfRootCertificateIDs->RootCertificateID#%d          ", i);
        EN_HLOGD(TAG, "                              ->RootCertificateID->X509IssuerName    :", (const u8*)pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509IssuerName.characters, pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509IssuerName.charactersLen * sizeof(exi_string_character_t));
        EN_HLOGD(TAG, "                                                 ->X509SerialNumber  :", pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509SerialNumber.data, pReq->ListOfRootCertificateIDs.RootCertificateID.array[i].X509SerialNumber.len);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintCertificateUpdateRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1CertificateUpdateResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CertificateUpdateRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CertificateUpdate Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    
    EN_SLOGD(TAG, "    ->SAProvisioningCertificateChain                          ");
    if(pRes->SAProvisioningCertificateChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                    ->Id                      :", (const u8*)pRes->SAProvisioningCertificateChain.Id.characters, pRes->SAProvisioningCertificateChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                    ->Certificate             :", pRes->SAProvisioningCertificateChain.Certificate.bytes, pRes->SAProvisioningCertificateChain.Certificate.bytesLen);
    if(pRes->SAProvisioningCertificateChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.array[i].bytes, pRes->SAProvisioningCertificateChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    
    EN_SLOGD(TAG, "    ->ContractSignatureCertChain                              ");
    if(pRes->ContractSignatureCertChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                ->Id                          :", (const u8*)pRes->ContractSignatureCertChain.Id.characters, pRes->ContractSignatureCertChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                ->Certificate                 :", pRes->ContractSignatureCertChain.Certificate.bytes, pRes->ContractSignatureCertChain.Certificate.bytesLen);
    if(pRes->ContractSignatureCertChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pRes->ContractSignatureCertChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pRes->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytes, pRes->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    
    EN_HLOGD(TAG, "    ->ContractSignatureEncryptedPrivateKey->Id                 :", (const u8*)pRes->ContractSignatureEncryptedPrivateKey.Id.characters, pRes->ContractSignatureEncryptedPrivateKey.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "                                          ->CONTENT            :", pRes->ContractSignatureEncryptedPrivateKey.CONTENT.bytes, pRes->ContractSignatureEncryptedPrivateKey.CONTENT.bytesLen);
    
    EN_HLOGD(TAG, "    ->DHpublickey->Id                                          :", (const u8*)pRes->DHpublickey.Id.characters, pRes->DHpublickey.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "                 ->CONTENT                                     :", pRes->DHpublickey.CONTENT.bytes, pRes->DHpublickey.CONTENT.bytesLen);
    
    EN_HLOGD(TAG, "    ->eMAID->Id                                                :", (const u8*)pRes->eMAID.Id.characters, pRes->eMAID.Id.charactersLen * sizeof(exi_string_character_t));
    EN_HLOGD(TAG, "           ->CONTENT                                           :", (const u8*)pRes->eMAID.CONTENT.characters, pRes->eMAID.CONTENT.charactersLen * sizeof(exi_string_character_t));
    
    if(pRes->RetryCounter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->RetryCounter                                             :%d", pRes->RetryCounter);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}











//PaymentDetailsReq/Res
void sV2gIso1PrintPaymentDetailsReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1PaymentDetailsReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PaymentDetailsReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PaymentDetails Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_HLOGD(TAG, "Body->eMAID                                                   :", (const u8*)pReq->eMAID.characters, pReq->eMAID.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, "    ->ContractSignatureCertChain->                            ");
    if(pReq->ContractSignatureCertChain.Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "                                ->Id                          :", (const u8*)pReq->ContractSignatureCertChain.Id.characters, pReq->ContractSignatureCertChain.Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "                                ->Certificate                 :", pReq->ContractSignatureCertChain.Certificate.bytes, pReq->ContractSignatureCertChain.Certificate.bytesLen);
    if(pReq->ContractSignatureCertChain.SubCertificates_isUsed == 1u)
    {
        for(i=0;i<pReq->ContractSignatureCertChain.SubCertificates.Certificate.arrayLen;i++)
        {
            EN_SLOGD(TAG, "                                    ->SubCertificates#%d", i);
            EN_HLOGD(TAG, "                                                     ->Certificate    :", pReq->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytes, pReq->ContractSignatureCertChain.SubCertificates.Certificate.array[i].bytesLen);
        }
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintPaymentDetailsRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1PaymentDetailsResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PaymentDetailsRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PaymentDetails Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_HLOGD(TAG, "    ->GenChallenge                                            :", pRes->GenChallenge.bytes, pRes->GenChallenge.bytesLen);
    EN_SLOGD(TAG, "    ->EVSETimeStamp                                           :%d", pRes->EVSETimeStamp);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}











//AuthorizationReq/Res
void sV2gIso1PrintAuthorizationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1AuthorizationReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.AuthorizationReq;
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<Authorization Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->                                                        ");
    if(pReq->Id_isUsed == 1u)
    {
        EN_HLOGD(TAG, "    ->Id                                                      :", (const u8*)pReq->Id.characters, pReq->Id.charactersLen * sizeof(exi_string_character_t));
    }
    
    if(pReq->GenChallenge_isUsed == 1u)
    {
        EN_HLOGD(TAG, "    ->GenChallenge                                            :", pReq->GenChallenge.bytes, pReq->GenChallenge.bytesLen);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintAuthorizationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1AuthorizationResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.AuthorizationRes;
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>Authorization Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->EVSEProcessing                                          :%s", u8Iso1EVSEProcessingTypeString[pRes->EVSEProcessing]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}












//ChargeParameterDiscoveryReq/Res
void sV2gIso1PrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    struct iso1ChargeParameterDiscoveryReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ChargeParameterDiscoveryReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ChargeParameterDiscovery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                    :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->RequestedEnergyTransferMode                                  :%s", u8Iso1EVSESupportedEnergyTransferTypeString[pReq->RequestedEnergyTransferMode]);
    if(pReq->MaxEntriesSAScheduleTuple_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->MaxEntriesSAScheduleTuple                                    :%d", pReq->MaxEntriesSAScheduleTuple);
    }
    if(pReq->AC_EVChargeParameter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->AC_EVChargeParameter                                         ");
        if(pReq->AC_EVChargeParameter.DepartureTime_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                          ->DepartureTime                          :%d", pReq->AC_EVChargeParameter.DepartureTime);
        }
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->AC_EVChargeParameter.EAmount, u8Unit);
        EN_SLOGD(TAG, "                          ->EAmount                                :%0.3f %s", f32Value, u8Unit);
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->AC_EVChargeParameter.EVMaxVoltage, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMaxVoltage                           :%0.3f %s", f32Value, u8Unit);
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->AC_EVChargeParameter.EVMaxCurrent, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMaxCurrent                           :%0.3f %s", f32Value, u8Unit);
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->AC_EVChargeParameter.EVMinCurrent, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMinCurrent                           :%0.3f %s", f32Value, u8Unit);
    }
    
    if(pReq->AC_EVChargeParameter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVChargeParameter                                             ");
        if(pReq->DC_EVChargeParameter.DepartureTime_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                          ->DepartureTime                          :%d", pReq->DC_EVChargeParameter.DepartureTime);
        }
        EN_SLOGD(TAG, "                          ->DC_EVStatus->EVReady                   :%d", pReq->DC_EVChargeParameter.DC_EVStatus.EVReady);
        EN_SLOGD(TAG, "                                       ->EVErrorCode               :%s", u8Iso1DC_EVErrorCodeTypeString[pReq->DC_EVChargeParameter.DC_EVStatus.EVErrorCode]);
        EN_SLOGD(TAG, "                                       ->EVRESSSOC                 :%d", pReq->DC_EVChargeParameter.DC_EVStatus.EVRESSSOC);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMaximumCurrentLimit                  :%0.3f %s", f32Value, u8Unit);
        
        if(pReq->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed == 1u)
        {
            memset(u8Unit ,0, sizeof(u8Unit));
            f32Value = sV2gIso1GetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumPowerLimit, u8Unit);
            EN_SLOGD(TAG, "                          ->EVMaximumPowerLimit                    :%0.3f %s", f32Value, u8Unit);
        }
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->DC_EVChargeParameter.EVMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "                          ->EVMaximumVoltageLimit                  :%0.3f %s", f32Value, u8Unit);
        
        if(pReq->DC_EVChargeParameter.EVEnergyCapacity_isUsed == 1u)
        {
            memset(u8Unit ,0, sizeof(u8Unit));
            f32Value = sV2gIso1GetPhyValue(&pReq->DC_EVChargeParameter.EVEnergyCapacity, u8Unit);
            EN_SLOGD(TAG, "                          ->EVEnergyCapacity                       :%0.3f %s", f32Value, u8Unit);
        }
        
        if(pReq->DC_EVChargeParameter.EVEnergyRequest_isUsed == 1u)
        {
            memset(u8Unit ,0, sizeof(u8Unit));
            f32Value = sV2gIso1GetPhyValue(&pReq->DC_EVChargeParameter.EVEnergyRequest, u8Unit);
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
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u8   i,j;
    f32  f32Value;
    char u8Unit[8];
    struct iso1ChargeParameterDiscoveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ChargeParameterDiscoveryRes;
    
    //此消息打印量较大 增加一些延时 否则日志会有丢失
    vTaskDelay(200 / portTICK_RATE_MS);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ChargeParameterDiscovery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                    :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                                 :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    vTaskDelay(20 / portTICK_RATE_MS);
    EN_SLOGD(TAG, "    ->EVSEProcessing                                               :%s", u8Iso1EVSEProcessingTypeString[pRes->EVSEProcessing]);
    if(pRes->SAScheduleList_isUsed == 1u)
    {
        for(i = 0; i< pRes->SAScheduleList.SAScheduleTuple.arrayLen; i++)
        {
            EN_SLOGD(TAG, "    ->SAScheduleList->SAScheduleTuple#%d", i);
            EN_SLOGD(TAG, "                    ->SAScheduleTuple->SAScheduleTupleID           :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].SAScheduleTupleID);
            EN_SLOGD(TAG, "                                     ->PMaxSchedule                ");
            for(j=0;j<pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.arrayLen; j++)
            {
                EN_SLOGD(TAG, "                                                   ->PMaxScheduleEntry#%d", j);
                EN_SLOGD(TAG, "                                                   ->PMaxScheduleEntry");
                if(pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval_isUsed == 1u)
                {
                    EN_SLOGD(TAG, "                                                                      ->RelativeTimeInterval");
                    if(pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.duration_isUsed == 1u)
                    {
                        EN_SLOGD(TAG, "                                                                                            ->duration :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.duration);
                    }
                    EN_SLOGD(TAG, "                                                                                            ->start    :%d", pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].RelativeTimeInterval.start);
                }
                
                memset(u8Unit ,0, sizeof(u8Unit));
                f32Value = sV2gIso1GetPhyValue(&pRes->SAScheduleList.SAScheduleTuple.array[i].PMaxSchedule.PMaxScheduleEntry.array[j].PMax, u8Unit);
                EN_SLOGD(TAG, "                                                                      ->PMax                           :%0.3f %s", f32Value, u8Unit);
            }
        }
    }
    vTaskDelay(20 / portTICK_RATE_MS);
    
    if(pRes->AC_EVSEChargeParameter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->AC_EVSEChargeParameter->AC_EVSEStatus->NotificationMaxDelay  :%d", pRes->AC_EVSEChargeParameter.AC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                                           ->EVSENotification      :%s", u8Iso1EVSENotificationTypeString[pRes->AC_EVSEChargeParameter.AC_EVSEStatus.EVSENotification]);
        EN_SLOGD(TAG, "                                           ->RCD                   :%d", pRes->AC_EVSEChargeParameter.AC_EVSEStatus.RCD);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->AC_EVSEChargeParameter.EVSENominalVoltage, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSENominalVoltage                   :%0.3f %s", f32Value, u8Unit);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->AC_EVSEChargeParameter.EVSEMaxCurrent, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMaxCurrent                       :%0.3f %s", f32Value, u8Unit);
    }
    
    if(pRes->DC_EVSEChargeParameter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEChargeParameter->DC_EVSEStatus->NotificationMaxDelay  :%d", pRes->DC_EVSEChargeParameter.DC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                                           ->EVSENotification      :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSENotification]);
        
        if(pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                                           ->EVSEIsolationStatus   :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEIsolationStatus]);
        }
        EN_SLOGD(TAG, "                                           ->EVSEStatusCode        :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEChargeParameter.DC_EVSEStatus.EVSEStatusCode]);
        vTaskDelay(20 / portTICK_RATE_MS);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMaximumCurrentLimit              :%0.3f %s", f32Value, u8Unit);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMaximumPowerLimit                :%0.3f %s", f32Value, u8Unit);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMaximumVoltageLimit              :%0.3f %s", f32Value, u8Unit);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMinimumCurrentLimit              :%0.3f %s", f32Value, u8Unit);
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEMinimumVoltageLimit              :%0.3f %s", f32Value, u8Unit);
        
        if(pRes->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed == 1u)
        {
            memset(u8Unit ,0, sizeof(u8Unit));
            f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance, u8Unit);
            EN_SLOGD(TAG, "                            ->EVSECurrentRegulationTolerance       :%0.3f %s", f32Value, u8Unit);
        }
        
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEPeakCurrentRipple, u8Unit);
        EN_SLOGD(TAG, "                            ->EVSEPeakCurrentRipple                :%0.3f %s", f32Value, u8Unit);
        
        if(pRes->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed == 1u)
        {
            memset(u8Unit ,0, sizeof(u8Unit));
            f32Value = sV2gIso1GetPhyValue(&pRes->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered, u8Unit);
            EN_SLOGD(TAG, "                            ->EVSEEnergyToBeDelivered              :%0.3f %s", f32Value, u8Unit);
        }
    }
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//CableCheckReq/Res
void sV2gIso1PrintCableCheckReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1DC_EVStatusType *pDcEvStatus = NULL;
    
    pDcEvStatus = &pDoc->V2G_Message.Body.CableCheckReq.DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CableCheck Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDoc->V2G_Message.Body.CableCheckReq.DC_EVStatus.EVReady);
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8Iso1DC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintCableCheckRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1CableCheckResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CableCheckRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CableCheck Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    EN_SLOGD(TAG, "    ->EVSEProcessing                        :%s", u8Iso1EVSEProcessingTypeString[pRes->EVSEProcessing]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//PreChargeReq/Res
void sV2gIso1PrintPreChargeReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1PreChargeReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PreChargeReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PreCharge Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pReq->DC_EVStatus.EVReady);
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8Iso1DC_EVErrorCodeTypeString[pReq->DC_EVStatus.EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pReq->DC_EVStatus.EVRESSSOC);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pReq->EVTargetVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetVoltage                       :%0.3f%s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pReq->EVTargetCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetCurrent                       :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintPreChargeRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1PreChargeResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PreChargeRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PreCharge Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}










//PowerDeliveryReq/Res
void sV2gIso1PrintPowerDeliveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    u16 i;
    struct iso1PowerDeliveryReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.PowerDeliveryReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<PowerDelivery Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                   :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ChargeProgress                                              :%s", u8Iso1ChargeProgressTypeString[pReq->ChargeProgress]);
    EN_SLOGD(TAG, "    ->SAScheduleTupleID                                           :%d", pReq->SAScheduleTupleID);
    if(pReq->ChargingProfile_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ChargingProfile                                                 ");
        for(i=0; i<pReq->ChargingProfile.ProfileEntry.arrayLen; i++)
        {
            EN_SLOGD(TAG, "                     ->ProfileEntry#%d", i);
            EN_SLOGD(TAG, "                     ->ProfileEntry->ChargingProfileEntryStart    :%d", pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryStart);
            EN_SLOGD(TAG, "                                   ->ChargingProfileEntryMaxPower :%d", pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryMaxPower);
            if(pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed == 1)
            {
                EN_SLOGD(TAG, "                                   ->ChargingProfileEntryMaxNumberOfPhasesInUse :%d", pReq->ChargingProfile.ProfileEntry.array[i].ChargingProfileEntryMaxNumberOfPhasesInUse);
            }
        }
    }
    
    if(pReq->DC_EVPowerDeliveryParameter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->DC_EVChargeParameter->DC_EVStatus->EVReady                  :%d", pReq->DC_EVPowerDeliveryParameter.DC_EVStatus.EVReady);
        EN_SLOGD(TAG, "                                       ->EVErrorCode              :%s", u8Iso1DC_EVErrorCodeTypeString[pReq->DC_EVPowerDeliveryParameter.DC_EVStatus.EVErrorCode]);
        EN_SLOGD(TAG, "                                       ->EVRESSSOC                :%d%%", pReq->DC_EVPowerDeliveryParameter.DC_EVStatus.EVRESSSOC);
        
        if(pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                          ->BulkChargingComplete(快充完成标识)    :%d", pReq->DC_EVPowerDeliveryParameter.BulkChargingComplete);
        }
        EN_SLOGD(TAG, "                          ->ChargingComplete(充电完成标识)        :%d", pReq->DC_EVPowerDeliveryParameter.ChargingComplete);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintPowerDeliveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1PowerDeliveryResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.PowerDeliveryRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>PowerDelivery Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                                                   :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                                                :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    
    if(pRes->AC_EVSEStatus_isUsed == 1)
    {
        EN_SLOGD(TAG, "    ->AC_EVSEStatus->NotificationMaxDelay                         :%d", pRes->AC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                   ->EVSENotification                             :%s", u8Iso1EVSENotificationTypeString[pRes->AC_EVSEStatus.EVSENotification]);
        EN_SLOGD(TAG, "                   ->RCD                                          :%d", pRes->AC_EVSEStatus.RCD);
    }
    
    if(pRes->DC_EVSEStatus_isUsed == 1)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
        if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
        }
        EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}









//ChargingStatusReq/Res
void sV2gIso1PrintChargingStatusReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1ChargingStatusReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.ChargingStatusReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<ChargingStatus Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintChargingStatusRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    struct iso1ChargingStatusResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.ChargingStatusRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>ChargingStatus Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    
    EN_HLOGD(TAG, "    ->EVSEID                                :", (const u8*)pRes->EVSEID.characters, pRes->EVSEID.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, "    ->SAScheduleTupleID                     :%d", pRes->SAScheduleTupleID);
    
    if(pRes->MeterInfo_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->MeterInfo->MeterID                    :%s", (const u8*)pRes->MeterInfo.MeterID.characters, pRes->MeterInfo.MeterID.charactersLen * sizeof(exi_string_character_t));
        
        if(pRes->MeterInfo.MeterReading_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->MeterReading               :%d", pRes->MeterInfo.MeterReading);
        }
        
        if(pRes->MeterInfo.SigMeterReading_isUsed == 1u)
        {
            EN_HLOGD(TAG, "               ->SigMeterReading            :", pRes->MeterInfo.SigMeterReading.bytes, pRes->MeterInfo.SigMeterReading.bytesLen);
        }
        
        if(pRes->MeterInfo.MeterStatus_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->MeterStatus                :%d", pRes->MeterInfo.MeterStatus);
        }
        
        if(pRes->MeterInfo.TMeter_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->TMeter                     :%d", pRes->MeterInfo.TMeter);
        }
    }
    
    if(pRes->ReceiptRequired_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ReceiptRequired                       :%d", pRes->ReceiptRequired);
    }
    
    EN_SLOGD(TAG, "    ->AC_EVSEStatus->NotificationMaxDelay                         :%d", pRes->AC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification                             :%s", u8Iso1EVSENotificationTypeString[pRes->AC_EVSEStatus.EVSENotification]);
    EN_SLOGD(TAG, "                   ->RCD                                          :%d", pRes->AC_EVSEStatus.RCD);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}












//CurrentDemandReq/Res
void sV2gIso1PrintCurrentDemandReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1CurrentDemandReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.CurrentDemandReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<CurrentDemand Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pReq->DC_EVStatus.EVReady);
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8Iso1DC_EVErrorCodeTypeString[pReq->DC_EVStatus.EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pReq->DC_EVStatus.EVRESSSOC);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pReq->EVTargetCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetCurrent                       :%0.3f%s", f32Value, u8Unit);
    
    if(pReq->EVMaximumVoltageLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->EVMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVMaximumVoltageLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->EVMaximumCurrentLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->EVMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVMaximumCurrentLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->EVMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->EVMaximumPowerLimit, u8Unit);
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
        f32Value = sV2gIso1GetPhyValue(&pReq->RemainingTimeToFullSoC, u8Unit);
        EN_SLOGD(TAG, "    ->RemainingTimeToFullSoC                :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pReq->RemainingTimeToBulkSoC_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pReq->RemainingTimeToBulkSoC, u8Unit);
        EN_SLOGD(TAG, "    ->RemainingTimeToBulkSoC                :%0.3f%s", f32Value, u8Unit);
    }
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pReq->EVTargetVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVTargetVoltage                       :%0.3f%s", f32Value, u8Unit);
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintCurrentDemandRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1CurrentDemandResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.CurrentDemandRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>CurrentDemand Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    
    memset(u8Unit ,0, sizeof(u8Unit));
    f32Value = sV2gIso1GetPhyValue(&pRes->EVSEPresentCurrent, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentCurrent                    :%0.3f%s", f32Value, u8Unit);
    
    EN_SLOGD(TAG, "    ->EVSECurrentLimitAchieved              :%d", pRes->EVSECurrentLimitAchieved);
    EN_SLOGD(TAG, "    ->EVSEVoltageLimitAchieved              :%d", pRes->EVSEVoltageLimitAchieved);
    EN_SLOGD(TAG, "    ->EVSEPowerLimitAchieved                :%d", pRes->EVSEPowerLimitAchieved);
    
    if(pRes->EVSEMaximumVoltageLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->EVSEMaximumVoltageLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumVoltageLimit               :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pRes->EVSEMaximumCurrentLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->EVSEMaximumCurrentLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumCurrentLimit               :%0.3f%s", f32Value, u8Unit);
    }
    
    if(pRes->EVSEMaximumPowerLimit_isUsed == 1u)
    {
        memset(u8Unit ,0, sizeof(u8Unit));
        f32Value = sV2gIso1GetPhyValue(&pRes->EVSEMaximumPowerLimit, u8Unit);
        EN_SLOGD(TAG, "    ->EVSEMaximumPowerLimit                 :%0.3f%s", f32Value, u8Unit);
    }
    
    EN_HLOGD(TAG, "    ->EVSEID                                :", (const u8*)pRes->EVSEID.characters, pRes->EVSEID.charactersLen * sizeof(exi_string_character_t));
    EN_SLOGD(TAG, "    ->SAScheduleTupleID                     :%d", pRes->SAScheduleTupleID);
    
    if(pRes->MeterInfo_isUsed == 1u)
    {
        EN_HLOGD(TAG, "    ->MeterInfo->MeterID                    :", (const u8*)pRes->MeterInfo.MeterID.characters, pRes->MeterInfo.MeterID.charactersLen * sizeof(exi_string_character_t));
        
        if(pRes->MeterInfo.MeterReading_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->MeterReading               :%d", pRes->MeterInfo.MeterReading);
        }
        
        if(pRes->MeterInfo.SigMeterReading_isUsed == 1u)
        {
            EN_HLOGD(TAG, "               ->SigMeterReading            :", pRes->MeterInfo.SigMeterReading.bytes, pRes->MeterInfo.SigMeterReading.bytesLen);
        }
        
        if(pRes->MeterInfo.MeterStatus_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->MeterStatus                :%d", pRes->MeterInfo.MeterStatus);
        }
        
        if(pRes->MeterInfo.TMeter_isUsed == 1u)
        {
            EN_SLOGD(TAG, "               ->TMeter                     :%d", pRes->MeterInfo.TMeter);
        }
    }
    
    if(pRes->ReceiptRequired_isUsed == 1u)
    {
        EN_SLOGD(TAG, "    ->ReceiptRequired                       :%d", pRes->ReceiptRequired);
    }
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}








//MeteringReceiptReq/Res
void sV2gIso1PrintMeteringReceiptReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1MeteringReceiptReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.MeteringReceiptReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<MeteringReceipt Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->                                      ");
    if(pReq->Id_isUsed == 1)
    {
        EN_HLOGD(TAG, "    ->Id                                    :", (const u8*)pReq->Id.characters, pReq->Id.charactersLen * sizeof(exi_string_character_t));
    }
    EN_HLOGD(TAG, "    ->SessionID                             :", pReq->SessionID.bytes, pReq->SessionID.bytesLen);
    if(pReq->SAScheduleTupleID_isUsed == 1)
    {
        EN_SLOGD(TAG, "    ->SAScheduleTupleID                     :%d", pReq->SAScheduleTupleID);
    }
    
    EN_HLOGD(TAG, "    ->MeterInfo->MeterID                    :", (const u8*)pReq->MeterInfo.MeterID.characters, pReq->MeterInfo.MeterID.charactersLen * sizeof(exi_string_character_t));
    
    if(pReq->MeterInfo.MeterReading_isUsed == 1u)
    {
        EN_SLOGD(TAG, "               ->MeterReading               :%d", pReq->MeterInfo.MeterReading);
    }
    
    if(pReq->MeterInfo.SigMeterReading_isUsed == 1u)
    {
        EN_HLOGD(TAG, "               ->SigMeterReading            :", pReq->MeterInfo.SigMeterReading.bytes, pReq->MeterInfo.SigMeterReading.bytesLen);
    }
    
    if(pReq->MeterInfo.MeterStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "               ->MeterStatus                :%d", pReq->MeterInfo.MeterStatus);
    }
    
    if(pReq->MeterInfo.TMeter_isUsed == 1u)
    {
        EN_SLOGD(TAG, "               ->TMeter                     :%d", pReq->MeterInfo.TMeter);
    }
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintMeteringReceiptRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1MeteringReceiptResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.MeteringReceiptRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>MeteringReceipt Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    
    if(pRes->AC_EVSEStatus_isUsed == 1)
    {
        EN_SLOGD(TAG, "    ->AC_EVSEStatus->NotificationMaxDelay                         :%d", pRes->AC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                   ->EVSENotification                             :%s", u8Iso1EVSENotificationTypeString[pRes->AC_EVSEStatus.EVSENotification]);
        EN_SLOGD(TAG, "                   ->RCD                                          :%d", pRes->AC_EVSEStatus.RCD);
    }
    
    if(pRes->DC_EVSEStatus_isUsed == 1)
    {
        EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
        EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
        if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
        {
            EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
        }
        EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    }
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}









//WeldingDetectionReq/Res
void sV2gIso1PrintWeldingDetectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1DC_EVStatusType *pDcEvStatus = NULL;
    
    pDcEvStatus = &pDoc->V2G_Message.Body.WeldingDetectionReq.DC_EVStatus;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<WeldingDetection Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->DC_EVStatus->EVReady                  :%d", pDcEvStatus->EVReady);
    EN_SLOGD(TAG, "                 ->EVErrorCode              :%s", u8Iso1DC_EVErrorCodeTypeString[pDcEvStatus->EVErrorCode]);
    EN_SLOGD(TAG, "                 ->EVRESSSOC                :%d%%", pDcEvStatus->EVRESSSOC);
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintWeldingDetectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    f32  f32Value;
    char u8Unit[8];
    
    struct iso1WeldingDetectionResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.WeldingDetectionRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>WeldingDetection Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID                             :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode                          :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, "    ->DC_EVSEStatus->NotificationMaxDelay   :%d", pRes->DC_EVSEStatus.NotificationMaxDelay);
    EN_SLOGD(TAG, "                   ->EVSENotification       :%s", u8Iso1EVSENotificationTypeString[pRes->DC_EVSEStatus.EVSENotification]);
    if(pRes->DC_EVSEStatus.EVSEIsolationStatus_isUsed == 1u)
    {
        EN_SLOGD(TAG, "                   ->EVSEIsolationStatus    :%s", u8Iso1IsolationLevelString[pRes->DC_EVSEStatus.EVSEIsolationStatus]);
    }
    EN_SLOGD(TAG, "                   ->EVSEStatusCode         :%s", u8Iso1DC_EVSEStatusCodeTypeString[pRes->DC_EVSEStatus.EVSEStatusCode]);
    
    memset(u8Unit ,0, sizeof(u8Unit));
     f32Value = sV2gIso1GetPhyValue(&pRes->EVSEPresentVoltage, u8Unit);
    EN_SLOGD(TAG, "    ->EVSEPresentVoltage                    :%0.3f%s", f32Value, u8Unit);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}









//SessionStopReq/Res
void sV2gIso1PrintSessionStopReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1SessionStopReqType *pReq = NULL;
    
    pReq = &pDoc->V2G_Message.Body.SessionStopReq;
    
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<SessionStop Req", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ChargingSession         :%s", u8Iso1ChargeSessionTypeString[pReq->ChargingSession]);
    EN_SLOGD(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gIso1PrintSessionStopRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc)
{
    struct iso1SessionStopResType *pRes = NULL;
    
    pRes = &pDoc->V2G_Message.Body.SessionStopRes;
    
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送>>>>SessionStop Res", i32DevIndex);
    EN_HLOGD(TAG, "Head->SessionID               :", pDoc->V2G_Message.Header.SessionID.bytes, pDoc->V2G_Message.Header.SessionID.bytesLen);
    EN_SLOGD(TAG, "Body->ResponseCode            :%s", u8Iso1ResponseCodeTypeString[pRes->ResponseCode]);
    EN_SLOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}













