/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   adc_app_sample.h
* Description                           :   ADC采样计算
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-20
* notice                                :   
****************************************************************************************************/
#ifndef _adc_app_sample_H_
#define _adc_app_sample_H_
#include "en_common.h"
#include "en_log.h"
#include "en_shell.h"

#include "adc_drv.h"
#include "adc_drv_isr.h"
#include "adc_app_sample_ntc.h"
#include "adc_app_sample_spec.h"








//ADC采样管脚定义---这里对应电路图上的采样引脚图标
typedef enum
{
    //枪1专用
    eAdcPin_A_FB_K                      = 0,                                    //ADC2.10:枪1->接触器组反馈
    eAdcPin_A_GUN_DC_P                  = 1,                                    //ADC0.18:枪1->枪线电压采样
    eAdcPin_A_GUN_TEMP1                 = 2,                                    //ADC1. 2:枪1->枪头温度检测1
    eAdcPin_A_GUN_TEMP2                 = 3,                                    //ADC0. 6:枪1->枪头温度检测2
    eAdcPin_A_ISO_AD                    = 4,                                    //ADC0. 5:枪1->枪线绝缘检测
    eAdcPin_A_PP_Pd_AD                  = 5,                                    //ADC0. 2:枪1->复用 <CCS:PP电压模拟采样口, CHADEMO:Pd电压模拟采样口>
    eAdcPin_A_CC1_CP_J_AD               = 6,                                    //ADC0.19:枪1->复用 <GBT:CC1电压采样, CCS:CP电压采样, CHADEMO:J电压采样>
    
    //枪2专用
    eAdcPin_B_FB_K                      = 7,                                    //ADC0.15:枪2->接触器组反馈
    eAdcPin_B_GUN_DC_P                  = 8,                                    //ADC0. 3:枪2->枪线电压采样
    eAdcPin_B_GUN_TEMP1                 = 9,                                    //ADC2. 6:枪2->枪头温度检测1
    eAdcPin_B_GUN_TEMP2                 = 10,                                   //ADC2. 2:枪2->枪头温度检测2
    eAdcPin_B_ISO_AD                    = 11,                                   //ADC2. 4:枪2->枪线绝缘检测
    eAdcPin_B_PP_Pd_AD                  = 12,                                   //ADC2. 9:枪2->复用 <CCS:PP电压模拟采样口, CHADEMO:Pd电压模拟采样口>
    eAdcPin_B_CC1_CP_J_AD               = 13,                                   //ADC0. 9:枪2->复用 <GBT:CC1电压采样, CCS:CP电压采样, CHADEMO:J电压采样>
    
    //公用
    eAdcPin_FB_X                        = 14,                                   //ADC2. 0:接触器X组反馈
    eAdcPin_FB_Y                        = 15,                                   //ADC2. 1:接触器Y组反馈
    eAdcPin_FB_Z                        = 16,                                   //ADC0.16:接触器Z组反馈
    eAdcPin_HW_VERSION                  = 17,                                   //ADC1. 6:硬件版本
    
    eAdcPinMax                                                                  //AD采样通道数量
}eAdcPin_t;









//部分复用采样通道的类型定义
typedef enum
{
    eAdcPinMuxTypeGbt                   = 0,                                    //国标GBT
    eAdcPinMuxTypeCcs                   = 0,                                    //欧标CCS
    eAdcPinMuxTypeCha                   = 0,                                    //日标CHAdeMO
    
    eAdcPinMuxTypeMax                                                           //
}eAdcPinMuxType_t;



































//ADC采样数据结构定义
typedef struct
{
    //各通道采样值
    f32                                 f32Value[eAdcPinMax];
    
    //采样任务句柄
    TaskHandle_t                        xTaskSample;
    
    //复用采样通道的类型
    eAdcPinMuxType_t                    eMuxType[ePileGunIndexNum];
    bool                                bGunTempPrtFlag;                        //枪温打印标志
    
    
    //接触器Xyx组反馈状态:eAdcPin_FB_X & eAdcPin_FB_Y & eAdcPin_FB_Z
    stAdcAppFbData_t                    stFbDataX;
    stAdcAppFbData_t                    stFbDataY;
    stAdcAppFbData_t                    stFbDataZ;
    
    //----------------------------------与充电枪相关的资源----------------------
    //接触器组反馈:eAdcPin_A_FB_K & eAdcPin_B_FB_K
    f32                                 f32Ufbk[ePileGunIndexNum];
    stAdcAppFbData_t                    stFbDataK[ePileGunIndexNum];
    
    //枪线电压采样:eAdcPin_A_GUN_DC_P & eAdcPin_B_GUN_DC_P
    stAdcAppGunDcOutData_t              stGunDcOutData[ePileGunIndexNum];
    
    //枪头温度检测1:eAdcPin_A_GUN_TEMP1 & eAdcPin_B_GUN_TEMP1
    f32                                 f32GunTemp1[ePileGunIndexNum];
    //枪头温度检测2:eAdcPin_A_GUN_TEMP2 & eAdcPin_B_GUN_TEMP2
    f32                                 f32GunTemp2[ePileGunIndexNum];
    
    //枪ISO绝缘检测:eAdcPin_A_ISO_AD & eAdcPin_B_ISO_AD
    f32                                 f32Uiso[ePileGunIndexNum];              //枪ISO绝缘检测采样电压
    
    //枪ISO绝缘检测:eAdcPin_A_PP_Pd_AD & eAdcPin_B_PP_Pd_AD
    f32                                 f32Upppd[ePileGunIndexNum];             //Upp或Upd
    stAdcAppPpData_t                    stPpData[ePileGunIndexNum];             //PP状态
    stAdcAppPdData_t                    stPdData[ePileGunIndexNum];             //PD状态
    
    //枪cp cc:eAdcPin_A_CC1_CP_J_AD & eAdcPin_B_CC1_CP_J_AD
    f32                                 f32Ucpcc[ePileGunIndexNum];             //Ucp或Ucc
    stAdcAppCc1Data_t                   stCc1Data[ePileGunIndexNum];            //CC1状态
    stAdcAppCpData_t                    stCpData[ePileGunIndexNum];             //CP 状态
    
}stAdcAppData_t;










extern bool sAdcAppSampleInit(void);

//对外的api接口函数---------------------------------------------------------------
//获取指定采样管脚的采样值---根据 ePin 获取
extern bool sAdcAppSampleGetData(eAdcPin_t ePin, f32 *pValue);
extern eAdcAppFbState_t sAdcAppSampleGetFbState(eAdcPin_t ePin);

//获取指定采样管脚的采样值---根据 枪序号 获取
extern bool sAdcAppSampleGetUfbk(ePileGunIndex_t eGunIndex, f32 *pUfbk);
extern bool sAdcAppSampleGetGunDcOut(ePileGunIndex_t eGunIndex, f32 *pGunDcOut);
extern bool sAdcAppSampleGetGunTemp(ePileGunIndex_t eGunIndex, f32 *pTemp1, f32 *pTemp2);
extern bool sAdcAppSampleGetUiso(ePileGunIndex_t eGunIndex, f32 *pUiso);
extern bool sAdcAppSampleGetUpppd(ePileGunIndex_t eGunIndex, f32 *pUpppd);
extern bool sAdcAppSampleGetUcpcc(ePileGunIndex_t eGunIndex, f32 *pUcpcc);

//获取指定采样管脚(根据采样值换算后)的(xxx)值
extern eAdcAppFbState_t  sAdcAppSampleGetFbStateK(ePileGunIndex_t eGunIndex);
extern eAdcAppPpState_t  sAdcAppSampleGetPpState(ePileGunIndex_t eGunIndex);
extern eAdcAppPdState_t  sAdcAppSampleGetPdState(ePileGunIndex_t eGunIndex);
extern eAdcAppCc1State_t sAdcAppSampleGetCc1State(ePileGunIndex_t eGunIndex);
extern eChgCpState_t     sAdcAppSampleGetCpState(ePileGunIndex_t eGunIndex);
extern f32  sAdcAppSampleGetUpp(ePileGunIndex_t eGunIndex);
extern f32  sAdcAppSampleGetUpd(ePileGunIndex_t eGunIndex);
extern f32  sAdcAppSampleGetUcc1(ePileGunIndex_t eGunIndex);
extern f32  sAdcAppSampleGetUcp(ePileGunIndex_t eGunIndex);

//复用采样通道类型配置
extern bool sAdcAppSampleSetMuxType(ePileGunIndex_t eGunIndex, eAdcPinMuxType_t eType);






#endif











