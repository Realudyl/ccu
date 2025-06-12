/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   adc_drv.h
* Description                           :   ADC驱动 硬件配置
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#ifndef _adc_drv_H_
#define _adc_drv_H_
#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_log.h"






//adc 设备号码定义
typedef enum
{
    eAdcNum0                            = 0,                                    //
    eAdcNum1                            = 1,                                    //
    eAdcNum2                            = 2,                                    //
    
    eAdcNumMax,
}eAdcNum_t;








//规则组和注入组最大通道数
#define cAdcDrvMaxChanReg               (16)
#define cAdcDrvMaxChanIns               (4)
#define cAdcDrvMaxChan                  (cAdcDrvMaxChanReg + cAdcDrvMaxChanIns)



//ADC采样模式
typedef enum
{
    eAdcDrvSampleModeReg                = 0,                                    //规则采样
    eAdcDrvSampleModeIns                = 1,                                    //注入采样
    
    eAdcDrvSampleModeMax,
}eAdcDrvSampleMode_t;



//ADC采样通道计算方式
typedef enum
{
    eAdcDrvSampleTypeAvg                = 0,                                    //计算有效值
    eAdcDrvSampleTypeRms                = 1,                                    //计算平均值
    eAdcDrvSampleTypeNtc                = 2,                                    //计算温度值
    
    eAdcDrvSampleTypeMax,
}eAdcDrvSampleType_t;






//ADC规则组采样触发 TIMER配置
typedef struct
{
    i32                                 i32Periph;                              //Timer     外设
    i32                                 i32PeriphRcu;                           //Timer RCU 外设
    i32                                 i32Chan;                                //Timer     通道
    i32                                 i32Psc;                                 //          预分频系数
    i32                                 i32Period;                              //          周期值
    
}stAdcDrvCfgTimer_t;








//ADC采样 DMA配置
typedef struct
{
    i32                                 i32Periph;                              //DMA     外设
    i32                                 i32PeriphRcu;                           //DMA RCU 外设
    i32                                 i32Chan;                                //DMA     通道
    i32                                 i32Req;                                 //DMAMUX  请求路由通道的DMA请求输入源
}stAdcDrvCfgDma_t;






//一个ADC采样管脚的配置
typedef struct
{
    u8                                  u8Pin;                                  //ADC采样管脚
    u8                                  u8Channel;                              //该管脚对应的Channel
    i32                                 i32Periph;                              //            GPIO 外设
    i32                                 i32PeriphRcu;                           //            GPIO RCU 外设
    i32                                 i32Pin;                                 //            GPIO 引脚
    f32                                 f32Coef;                                //            采样系数
    f32                                 f32Offset;                              //            采样偏移
    eAdcDrvSampleType_t                 eType;                                  //            采样计算方式
    eAdcDrvSampleMode_t                 eMode;                                  //            采样模式
    
}stAdcDrvCfgChan_t;






//adc 设备
typedef struct
{
    i32                                 i32PeriphAdc;                           //ad外设：ADC0,1,2
    i32                                 i32PeriphRcuAdc;                        //该外设的RCU
    i32                                 i32Irq;                                 //        中断函数
    i32                                 i32PeriphRcuTrigsel;                    //        触发控制器RCU
    i32                                 i32TrigSourceIns;                       //        注入组触发源
    i32                                 i32TrigSourceReg;                       //        规则组触发源
    i32                                 i32TrigTargetIns;                       //        注入组触发源
    i32                                 i32TrigTargetReg;                       //        规则组触发源
    
    stAdcDrvCfgTimer_t                  stTimer;                                //        规则组触发定时器配置---注入组的定时器放到PWM组件里配置(因为要发PWM波)
    stAdcDrvCfgDma_t                    stDma;                                  //        DMA配置
    
    i32                                 *pBuf;                                  //        数据缓存区---规则组使用
    i32                                 i32Size;                                //        数据缓存区大小
    
    i32                                 i32ChanNum;                             //        实际通道数
    stAdcDrvCfgChan_t                   stCfgChan[cAdcDrvMaxChan];              //        通道配置
}stAdcDrvDevice_t;















extern bool sAdcDrvInitDev(eAdcNum_t eAdcNum);







#endif

















