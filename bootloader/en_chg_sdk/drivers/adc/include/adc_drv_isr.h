/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   adc_drv_isr.h
* Description                           :   ADC驱动 采样中断函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#ifndef _adc_drv_isr_H_
#define _adc_drv_isr_H_
#include "adc_drv.h"








//ADC采样 部分可配置参数定义

//注入采样周期 默认值 20 * 1ms = 20ms
#ifndef cSdkAdcDrvCycleIns
#define cAdcDrvCycleIns                 (20)
#else
#define cAdcDrvCycleIns                 (cSdkAdcDrvCycleIns)
#endif



//规则采样周期 默认值 360 * 55.5us = 20ms
#ifndef cSdkAdcDrvCycleReg
#define cAdcDrvCycleReg                 (360)
#else
#define cAdcDrvCycleReg                 (cSdkAdcDrvCycleReg)
#endif



//采样pin脚数量
#ifndef cSdkAdcDrvPinNum
#define cAdcDrvPinNum                   (18)
#else
#define cAdcDrvPinNum                   (cSdkAdcDrvPinNum)
#endif














//ADC采样  单个管脚的数据
typedef struct
{
    eAdcDrvSampleMode_t                 eMode;                                  //
    eAdcDrvSampleType_t                 eType;                                  //
    f32                                 f32Coef;                                //采样系数
    f32                                 f32Offset;                              //采样偏移
    
    f32                                 f32Ad;                                  //单次AD采样值
    f32                                 f32Real;                                //单次AD采样值的真实值---计算Rms值的通道需要先计算真实值再累加
    f32                                 f32Sum;                                 //累加和
    f32                                 f32SumTemp;                             //累加和 temp
    i32                                 i32SumCnt;                              //累加次数
    i32                                 i32SumCntTemp;                          //累加次数 temp
    
}stAdcDrvDataUnit_t;






//ADC采样  所有通道的数据
typedef struct
{
    //注意, 这里有 cAdcDrvPinNum 个采样管脚,也对应同样个数的采样通道
    //但是数组下标是按采样管脚即 u8Pin 字段去排序 而不是按采样通道即 u8Channel 字段去排序
    //这个数组下标是外部应用程序访问AD驱动采样数据的依据
    stAdcDrvDataUnit_t                  stDataPin[cAdcDrvPinNum];               //
    
}stAdcDrvDataTotal_t;

















//Sample 组件需要用到的一些缓存数据
typedef struct
{
    TaskHandle_t                        xTaskSample;
    
    stAdcDrvDataTotal_t                 stDataTotal;                            //ADC采样数据
}stAdcDrvCache_t;






















extern bool sAdcDrvInit(TaskHandle_t xTask);
extern stAdcDrvDataTotal_t *sAdcDrvGetData(void);






#endif

















