/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_mme_ven_spec_print.h
* Description                           :   MME协议实现 之 供应商特定的MME帧 打印功能
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-07-10
* notice                                :   基于联芯通文档 <MSE102x Vertexcom Vender-Specific MME Specification> 开发
****************************************************************************************************/
#ifndef _bms_ccs_mme_ven_spec_print_h
#define _bms_ccs_mme_ven_spec_print_h































extern const char u8MmeVsRstStr[eMmeVsRstMax][16];
extern const char u8MmeVsLinkStateStr[eMmeVsGetStatusLinkStateMax][32];
extern const char u8MmeVsReadyStr[eMmeVsGetStatusReadyMax][16];









extern void sMmePrintVsGetVersionCnf(i32 i32MmeDevIndex, void *pParam);
extern void sMmePrintVsResetCnf(i32 i32MmeDevIndex, void *pParam);
extern void sMmePrintVsGetStatusCnf(i32 i32MmeDevIndex, void *pParam);
extern void sMmePrintVsGetLinkStatsCnf(i32 i32MmeDevIndex, void *pParam, i32 i32Size);
extern void sMmePrintVsGetNwInfoCnf(i32 i32MmeDevIndex, void *pParam);








#endif 



















