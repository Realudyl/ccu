/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     bms_ccs_v2g_iso1_print.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-3-26
 * @Attention             :     
 * @Brief                 :     ISO15118v1 协议消息内容打印功能实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-3-26
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _bms_ccs_v2g_iso1_print_H
#define _bms_ccs_v2g_iso1_print_H
#include "bms_ccs_v2g_iso1.h"
#include "bms_ccs_v2g_iso1_basic.h"









//ISO15118 消息打印函数
extern void sV2gIso1PrintSessionSetupReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintSessionSetupRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintServiceDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintServiceDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintServiceDetailReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintServiceDetailRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPaymentServiceSelectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPaymentServiceSelectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCertificateInstallationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCertificateInstallationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCertificateUpdateReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCertificateUpdateRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPaymentDetailsReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPaymentDetailsRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintAuthorizationReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintAuthorizationRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintChargeParameterDiscoveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintChargeParameterDiscoveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCableCheckReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCableCheckRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPreChargeReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPreChargeRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPowerDeliveryReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintPowerDeliveryRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintChargingStatusReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintChargingStatusRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCurrentDemandReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintCurrentDemandRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintMeteringReceiptReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintMeteringReceiptRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintWeldingDetectionReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintWeldingDetectionRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintSessionStopReq(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
extern void sV2gIso1PrintSessionStopRes(i32 i32DevIndex, struct iso1EXIDocument *pDoc);
































#endif

