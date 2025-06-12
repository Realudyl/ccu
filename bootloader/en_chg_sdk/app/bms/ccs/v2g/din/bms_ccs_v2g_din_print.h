/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_din_print.h
* Description                           :   DIN70121 协议实现 中的一些数据处理函数
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-24
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_din_print_h_
#define _bms_ccs_v2g_din_print_h_
#include "bms_ccs_v2g_din.h"
#include "bms_ccs_v2g_din_basic.h"























//DIN70121 消息打印函数
extern void  sV2gDinPrintSessionSetupReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintSessionSetupRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintServiceDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintServiceDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintServicePaymentSelectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintServicePaymentSelectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintContractAuthenticationReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintContractAuthenticationRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintPowerDeliveryReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintPowerDeliveryRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintSessionStopReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintSessionStopRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintCableCheckReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintCableCheckRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintPreChargeReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintPreChargeRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintCurrentDemandReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintCurrentDemandRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintWeldingDetectionReq(i32 i32DevIndex, struct dinEXIDocument *pDoc);
extern void  sV2gDinPrintWeldingDetectionRes(i32 i32DevIndex, struct dinEXIDocument *pDoc);












#endif














