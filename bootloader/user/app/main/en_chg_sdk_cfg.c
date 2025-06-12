/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_chg_sdk_cfg.c
* Description                           :   EN+ chg SDK 参数配置文件
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-16
* notice                                :   本文件为SDK内部参数配置文件,开放给SDK用户使用
*
*                                           定制品应该按照实际的需求去配置以下参数
*                                           如果屏蔽某一项， SDK内部会按照默认值初始化
****************************************************************************************************/
#include "en_common.h"

#include "en_chg_sdk_cfg.h"

#include "adc_app_sample.h"
#include "can_drv_trans.h"
#include "eeprom_drv_at24c256c.h"
#include "enet_drv.h"
#include "i2c_drv.h"
#include "io_app.h"
#include "io_pwm_app.h"
#include "mse102x_drv.h"
#include "spi_drv_trans.h"
#include "usart_drv_trans.h"









//******************************************************************************
//                                      ADC采样驱动配置
//******************************************************************************
//-1表示不开启
//TIMER_CK 目前为200MHz 预分频后的 PSC_CLK = TIMER_CK / (i32Psc + 1) (Hz)
//定时器周期 Period = (i32Period + 1) / PSC_CLK (s)
//规则采样周期55.5us 注入采样周期1ms
stAdcDrvDevice_t stAdcDrvDevMap[eAdcNumMax] = 
{
    //ADC0
    {
        ADC0,
        RCU_ADC0,
        ADC0_1_IRQn,
        RCU_TRIGSEL,
        TRIGSEL_INPUT_TIMER1_CH2,                                                       //ADx的注入组触发源-----无注入采样设置 -1
        TRIGSEL_INPUT_TIMER2_CH1,                                                       //ADx的规则组触发源-----无规则采样设置 -1
        TRIGSEL_OUTPUT_ADC0_INSTRG,                                                     //ADx的注入组触发目标---固定此值---有无注入采样都是这值
        TRIGSEL_OUTPUT_ADC0_REGTRG,                                                     //ADx的规则组触发目标---固定此值---有无规则采样都是这值
        {TIMER2,    RCU_TIMER2,     TIMER_CH_1,     9,          1109                },
        {DMA0,      RCU_DMA0,       DMA_CH0,        DMA_REQUEST_ADC0                },
        NULL,
        0,
        9,                                                                              //规则采样+注入采样总路数
        {
            {eAdcPin_A_PP_Pd_AD,    ADC_CHANNEL_2,  GPIOF,      RCU_GPIOF,          GPIO_PIN_11,       (0.00108000),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_GUN_DC_P,    ADC_CHANNEL_3,  GPIOA,      RCU_GPIOA,          GPIO_PIN_6,        (0.26940516),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_A_ISO_AD,      ADC_CHANNEL_5,  GPIOB,      RCU_GPIOB,          GPIO_PIN_1,          (0.002685), (1862),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_A_GUN_TEMP2,   ADC_CHANNEL_6,  GPIOF,      RCU_GPIOF,          GPIO_PIN_12,                (1),    (0),  eAdcDrvSampleTypeNtc,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_CC1_CP_J_AD, ADC_CHANNEL_9,  GPIOB,      RCU_GPIOB,          GPIO_PIN_0,        (0.00676473), (1848),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeIns    },//eAdcDrvSampleModeIns
            {eAdcPin_B_FB_K,        ADC_CHANNEL_15, GPIOA,      RCU_GPIOA,          GPIO_PIN_3,        (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_FB_Z,          ADC_CHANNEL_16, GPIOA,      RCU_GPIOA,          GPIO_PIN_0,        (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_A_GUN_DC_P,    ADC_CHANNEL_18, GPIOA,      RCU_GPIOA,          GPIO_PIN_4,        (0.26940516),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_A_CC1_CP_J_AD, ADC_CHANNEL_19, GPIOA,      RCU_GPIOA,          GPIO_PIN_5,        (0.00676473), (1848),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeIns    },//eAdcDrvSampleModeIns
            
        },
    },
    
    
    //ADC1
    {
        ADC1,
        RCU_ADC1,
        ADC0_1_IRQn,
        RCU_TRIGSEL,
        -1,                                                                             //ADx的注入组触发源-----无注入采样设置 -1
        TRIGSEL_INPUT_TIMER2_CH1,                                                       //ADx的规则组触发源-----无规则采样设置 -1
        TRIGSEL_OUTPUT_ADC1_INSTRG,                                                     //ADx的注入组触发目标---固定此值---有无注入采样都是这值
        TRIGSEL_OUTPUT_ADC1_REGTRG,                                                     //ADx的规则组触发目标---固定此值---有无规则采样都是这值
        {-1,        -1,             -1,             0,          0                   },  //此定时器是规则组触发定时器,3个ADC外设公用，不要重复配置配置
        {DMA0,      RCU_DMA0,       DMA_CH1,        DMA_REQUEST_ADC1                },
        NULL,
        0,
        2,                                                                              //规则采样+注入采样总路数
        {
            {eAdcPin_A_GUN_TEMP1,   ADC_CHANNEL_2,  GPIOF,      RCU_GPIOF,          GPIO_PIN_13,                (1),    (0),  eAdcDrvSampleTypeNtc,  eAdcDrvSampleModeReg    },
            {eAdcPin_HW_VERSION,    ADC_CHANNEL_2,  GPIOF,      RCU_GPIOF,          GPIO_PIN_14,       (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            
        },
    },
    
    
    //ADC2
    {
        ADC2,
        RCU_ADC2,
        ADC2_IRQn,
        RCU_TRIGSEL,
        -1,                                                                             //ADx的注入组触发源-----无注入采样设置 -1
        TRIGSEL_INPUT_TIMER2_CH1,                                                       //ADx的规则组触发源-----无规则采样设置 -1
        TRIGSEL_OUTPUT_ADC2_INSTRG,                                                     //ADx的注入组触发目标---固定此值---有无注入采样都是这值
        TRIGSEL_OUTPUT_ADC2_REGTRG,                                                     //ADx的规则组触发目标---固定此值---有无规则采样都是这值
        {-1,        -1,             -1,             0,          0                   },  //此定时器是规则组触发定时器,3个ADC外设公用，不要重复配置配置
        {DMA0,      RCU_DMA0,       DMA_CH2,        DMA_REQUEST_ADC2                },
        NULL,
        0,
        7,                                                                              //规则采样+注入采样总路数
        {
            {eAdcPin_FB_X,          ADC_CHANNEL_0,  GPIOC,      RCU_GPIOC,          GPIO_PIN_2,        (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_FB_Y,          ADC_CHANNEL_1,  GPIOC,      RCU_GPIOC,          GPIO_PIN_3,        (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_GUN_TEMP2,   ADC_CHANNEL_2,  GPIOF,      RCU_GPIOF,          GPIO_PIN_9,                 (1),    (0),  eAdcDrvSampleTypeNtc,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_ISO_AD,      ADC_CHANNEL_4,  GPIOF,      RCU_GPIOF,          GPIO_PIN_5,          (0.002685), (1862),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_GUN_TEMP1,   ADC_CHANNEL_6,  GPIOF,      RCU_GPIOF,          GPIO_PIN_10,                (1),    (0),  eAdcDrvSampleTypeNtc,  eAdcDrvSampleModeReg    },
            {eAdcPin_B_PP_Pd_AD,    ADC_CHANNEL_9,  GPIOF,      RCU_GPIOF,          GPIO_PIN_4,        (0.00108000),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            {eAdcPin_A_FB_K,        ADC_CHANNEL_10, GPIOC,      RCU_GPIOC,          GPIO_PIN_0,        (0.00081176),    (0),  eAdcDrvSampleTypeAvg,  eAdcDrvSampleModeReg    },
            
        },
    },
};










//******************************************************************************
//                                      CAN驱动配置
// 1.CAN FIFO滤波器底层自动配置成不滤波
// 2.CAN MAilBox滤波器，因为只有一个接收邮箱，所以配置三个值分别是接收ID掩码，公共过滤器，私有过滤器
// 目前开启的是私有滤波器。公共滤波器无效。
// 例1: 如ID_MASK = 0x0000000F, PrivateFilter = 0x0000000F
//      则只有ID的低4位为1的帧才会被接收
//******************************************************************************
//-1表示不开启
stCanDrvDevice_t stCanDrvDevMap[eCanNumMax] = 
{
    {
        CAN0,
        RCU_CAN0,
        
        CAN0_Message_IRQn,
        
        {true},
        
        //邮箱配置
        {0,     8,          CAN_INT_FIFO_AVAILABLE,     CAN_INT_FLAG_FIFO_AVAILABLE,    CAN_FLAG_MB8,   NULL,   NULL,  0x00000000,  0x00000000},
        
        //GPIO配置
        {GPIOB, RCU_GPIOB,  GPIO_PIN_8,    GPIO_MODE_AF,            GPIO_PUPD_NONE,     GPIO_AF_9,  GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_9,    GPIO_MODE_AF,            GPIO_PUPD_NONE,     GPIO_AF_9,  GPIO_OSPEED_60MHZ   },
    },
    
    {
        CAN1,
        RCU_CAN1,
        
        CAN1_Message_IRQn,
        
        {true},
        
        //邮箱配置
        {0,     8,          CAN_INT_FIFO_AVAILABLE,     CAN_INT_FLAG_FIFO_AVAILABLE,    CAN_FLAG_MB8,   NULL,   NULL,  0x00000000,  0x00000000},
        
        //GPIO配置
        {GPIOB, RCU_GPIOB,  GPIO_PIN_5,    GPIO_MODE_AF,            GPIO_PUPD_NONE,     GPIO_AF_9,  GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_6,    GPIO_MODE_AF,            GPIO_PUPD_NONE,     GPIO_AF_9,  GPIO_OSPEED_60MHZ   },
    },
    
    {
        CAN2,
        RCU_CAN2,
        
        CAN2_Message_IRQn,
        
        {false},
        
        //邮箱配置
        //{0,     8,          CAN_INT_FIFO_AVAILABLE,     CAN_INT_FLAG_FIFO_AVAILABLE,    CAN_FLAG_MB8,   NULL,   NULL,  0x00000000,  0x00000000},
        {0,     8,          CAN_INT_MB0,   CAN_INT_FLAG_MB0,       CAN_FLAG_MB8,       NULL,       NULL,  0x00000000,  0x00000000},
        
        //GPIO配置
        {GPIOF, RCU_GPIOF,  GPIO_PIN_6,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_2,  GPIO_OSPEED_60MHZ   },
        {GPIOF, RCU_GPIOF,  GPIO_PIN_7,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_2,  GPIO_OSPEED_60MHZ   },
    },
};









//******************************************************************************
//                                      eeprom 驱动配置
//******************************************************************************
stEepromDrvDevice_t stEepromDrvDev = 
{
    .eI2cNum        = eI2cNum0,                                                 //此EEPROM芯片使用的I2C端口
    .unDevAddr      = {.u8Addr = 0xA0},                                         //此EEPROM芯片的地址
    .u16DevPageNum  = 512,                                                      //              页数
    .u8DevPageSize  = 64,                                                       //              页大小
    .xSemEeprom     = NULL,                                                     //              互斥锁
};









//******************************************************************************
//                                      I2C驱动配置
//******************************************************************************
//-1表示不开启
stI2cDrvDevice_t stI2cDrvDevMap[eI2cNumMax] = 
{
    {
        {GPIOF, RCU_GPIOF,  GPIO_PIN_1,     GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,  GPIO_OSPEED_60MHZ   },
        {GPIOF, RCU_GPIOF,  GPIO_PIN_0,     GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,  GPIO_OSPEED_60MHZ   },
    },
};









//******************************************************************************
//                                      io驱动配置
//******************************************************************************
stIoDrvDevice_t stIoDrvDevMap = 
{
    //io数量
    eIoMax,
    
    //io配置参数
    {
        //枪1专用
        {eIo_A_DR_K,                RCU_GPIOG, GPIOG, GPIO_PIN_1,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_D1_RELAY,            RCU_GPIOD, GPIOD, GPIO_PIN_13, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_D2_RELAY,            RCU_GPIOJ, GPIOJ, GPIO_PIN_8,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_GB_BMS,              RCU_GPIOD, GPIOD, GPIO_PIN_14, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_A_LOCK_FB,             RCU_GPIOD, GPIOD, GPIO_PIN_15, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_A_LOCK_ON_OFF,         RCU_GPIOK, GPIOK, GPIO_PIN_2,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_LOCK_RELAY_EN,       RCU_GPIOJ, GPIOJ, GPIO_PIN_9,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_ISO_RELAY_DRV1,      RCU_GPIOD, GPIOD, GPIO_PIN_10, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_ISO_RELAY_DRV2,      RCU_GPIOB, GPIOB, GPIO_PIN_13, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_A_ISO_RELAY_DRV3,      RCU_GPIOB, GPIOB, GPIO_PIN_12, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        
        //枪2专用
        {eIo_B_DR_K,                RCU_GPIOE, GPIOE, GPIO_PIN_10, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_D1_RELAY,            RCU_GPIOD, GPIOD, GPIO_PIN_4,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_D2_RELAY,            RCU_GPIOG, GPIOG, GPIO_PIN_14, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_GB_BMS,              RCU_GPIOG, GPIOG, GPIO_PIN_15, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_B_LOCK_FB,             RCU_GPIOG, GPIOG, GPIO_PIN_10, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_B_LOCK_ON_OFF,         RCU_GPIOC, GPIOC, GPIO_PIN_11, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_LOCK_RELAY_EN,       RCU_GPIOC, GPIOC, GPIO_PIN_10, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_ISO_RELAY_DRV1,      RCU_GPIOB, GPIOB, GPIO_PIN_2,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_ISO_RELAY_DRV2,      RCU_GPIOF, GPIOF, GPIO_PIN_15, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_B_ISO_RELAY_DRV3,      RCU_GPIOF, GPIOF, GPIO_PIN_8,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        
        //公用
        {eIo_DR_X,                  RCU_GPIOE, GPIOE, GPIO_PIN_9,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_DR_Y,                  RCU_GPIOE, GPIOE, GPIO_PIN_8,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_DR_Z,                  RCU_GPIOE, GPIOE, GPIO_PIN_7,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_PEN_FAULT_AC_RLY_FB,   RCU_GPIOE, GPIOE, GPIO_PIN_11, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_DOOR,                  RCU_GPIOE, GPIOE, GPIO_PIN_12, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_FA_STOP,               RCU_GPIOE, GPIOE, GPIO_PIN_13, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_SPD,                   RCU_GPIOE, GPIOE, GPIO_PIN_14, GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_FAN_STATUS,            RCU_GPIOG, GPIOG, GPIO_PIN_3,  GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_DO1,                   RCU_GPIOD, GPIOD, GPIO_PIN_12, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_DO2,                   RCU_GPIOD, GPIOD, GPIO_PIN_11, GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_DO3,                   RCU_GPIOB, GPIOB, GPIO_PIN_7,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        {eIo_SITE1,                 RCU_GPIOG, GPIOG, GPIO_PIN_8,  GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_SITE2,                 RCU_GPIOA, GPIOA, GPIO_PIN_8,  GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_SITE3,                 RCU_GPIOC, GPIOC, GPIO_PIN_9,  GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_SITE4,                 RCU_GPIOC, GPIOC, GPIO_PIN_8,  GPIO_MODE_INPUT,    GPIO_PUPD_NONE,     0,              0,                      GPIO_ISPERIOD(1),   GPIO_IFTYPE_6_SAMPLE    },
        {eIo_GATE_POWER,            RCU_GPIOD, GPIOD, GPIO_PIN_3,  GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   GPIO_OTYPE_PP,  GPIO_OSPEED_60MHZ,      0,                  0                       },
        
        
    },
};










//******************************************************************************
//                                      io_pwm驱动配置
//******************************************************************************
stIoPwmDrvDevice_t stIoPwmDrvDevMap[cSdkIoPwmDrvPwmTimerNum] = 
{
    //PWM1
    {
        ePwmParallelSynchro,
        .stPwmParallelSynchro = 
        {
            eParallelMaster,
            {TRIGSEL_OUTPUT_TIMER31_ITI14,      TRIGSEL_INPUT_TIMER1_TRGO0,     TIMER_TRI_OUT0_SRC_ENABLE,      -1,     -1},
            {
                TIMER1,     RCU_TIMER1,     199,    999,    1,
                {
                    {
                        TIMER_CH_2,
                        eIoPwm_CP_DR_A,
                        {GPIOB,     RCU_GPIOB,      GPIO_PIN_10,    GPIO_MODE_AF,   GPIO_PUPD_PULLUP,   GPIO_AF_1,      GPIO_OSPEED_60MHZ},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        gpio_bit_set,                                                           //此PWM通道拉高是关闭输出
                        "充电枪1, CP振荡器"
                    },
                    
                    {
                        -1,
                        eIoPwmMax,
                        {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        NULL,
                        " "
                    },
                    
                    {
                        -1,
                        eIoPwmMax,
                        {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        NULL,
                        " "
                    },
                    
                    {
                        -1,
                        eIoPwmMax,
                        {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        NULL,
                        " "
                    }
                }
            }
        }
    },
    
    //PWM2
    {
        ePwmParallelSynchro,
        .stPwmParallelSynchro = 
        {
            eParallelSlave,
            {TRIGSEL_OUTPUT_TIMER31_ITI14,      TRIGSEL_INPUT_TIMER1_TRGO0,     -1,     TIMER_SMCFG_TRGSEL_ITI1,    TIMER_SLAVE_MODE_EVENT},
            {
                TIMER31,    RCU_TIMER31,    199,    999,    2,
                {
                    {
                        TIMER_CH_0,
                        eIoPwm_CP_DR_B,
                        {GPIOG,     RCU_GPIOG,      GPIO_PIN_0,     GPIO_MODE_AF,   GPIO_PUPD_PULLUP,   GPIO_AF_4,      GPIO_OSPEED_60MHZ},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        gpio_bit_set,                                                           //此PWM通道拉高是关闭输出
                        "充电枪2, CP振荡器"
                    },
                    
                    {
                        TIMER_CH_2,
                        eIoPwm_FAN,
                        {GPIOG,     RCU_GPIOG,      GPIO_PIN_2,     GPIO_MODE_AF,   GPIO_PUPD_PULLUP,   GPIO_AF_4,      GPIO_OSPEED_60MHZ},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        gpio_bit_reset,                                                         //此PWM通道拉低是关闭输出
                        "散热风扇"
                    },
                    
                    {
                        -1,
                        eIoPwmMax,
                        {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        NULL,
                        " "
                    },
                    
                    {
                        -1,
                        eIoPwmMax,
                        {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                        {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                        NULL,
                        " "
                    }
                }
            }
        }
    },
    
    //PWM3-4
    {
        ePwmIndependent,
        .stPwmIndependent = 
        {
            TIMER30,    RCU_TIMER30,    1,      124,    2,
            {
                {
                    TIMER_CH_1,
                    eIoPwm_LED_Board,
                    {GPIOG,     RCU_GPIOG,      GPIO_PIN_6,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_4,      GPIO_OSPEED_60MHZ},
                    {DMA0,      RCU_DMA0,       DMA_CH3,        DMA_REQUEST_TIMER30_UP,     (uint32_t)(&TIMER_CH1CV(TIMER30)),      NULL,   0,      TIMER_DMACFG_DMATA_CH1CV,   TIMER_DMACFG_DMATC_1TRANSFER,   TIMER_DMA_UPD},
                    gpio_bit_set,                                                           //此PWM通道拉高是关闭输出
                    "LED灯板, LedBoard_PWM"
                },
                
                {
                    TIMER_CH_0,
                    eIoPwm_LED_Band,
                    {GPIOG,     RCU_GPIOG,      GPIO_PIN_5,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_4,      GPIO_OSPEED_60MHZ},
                    {DMA0,      RCU_DMA0,       DMA_CH4,        DMA_REQUEST_TIMER30_UP,     (uint32_t)(&TIMER_CH0CV(TIMER30)),      NULL,   0,      TIMER_DMACFG_DMATA_CH0CV,   TIMER_DMACFG_DMATC_1TRANSFER,   TIMER_DMA_UPD},
                    gpio_bit_set,                                                           //此PWM通道拉高是关闭输出
                    "LED灯带, LedBand_PWM"
                },
                
                {
                    -1,
                    eIoPwmMax,
                    {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                    {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                    NULL,
                    " "
                },
                
                {
                    -1,
                    eIoPwmMax,
                    {-1,     -1,      -1,    -1,   -1,   -1,      -1},
                    {-1,    -1,     -1,     -1,     -1,     NULL,       0,      -1,     -1,     -1},
                    NULL,
                    " "
                }
            }
        }
    },
};











//******************************************************************************
//                                      PLC/MSE102x驱动配置
//******************************************************************************
stMse102xDrvDevice_t stMse102xDrvDevMap[cMse102xDevNum] = 
{
    //一把ccs枪对应一个 MSE102x 芯片
    {
        eSpiNum0,
        {SPI_TRANSMODE_FULLDUPLEX, SPI_MASTER, SPI_DATASIZE_8BIT, SPI_NSS_SOFT, SPI_ENDIAN_MSB, SPI_CK_PL_HIGH_PH_2EDGE, SPI_PSC_32},
        
        //GPIO配置  PG4 => SPI_INTR, PG7 => MSE1_RESET
        {
            {GPIOG, RCU_GPIOG,  GPIO_PIN_4,     GPIO_MODE_INPUT,    GPIO_PUPD_PULLUP,   -1,  GPIO_OSPEED_60MHZ   },
            {GPIOG, RCU_GPIOG,  GPIO_PIN_7,     GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   -1,  GPIO_OSPEED_60MHZ   }
        }
        
    },
    
    {
        eSpiNum3,
        {SPI_TRANSMODE_FULLDUPLEX, SPI_MASTER, SPI_DATASIZE_8BIT, SPI_NSS_SOFT, SPI_ENDIAN_MSB, SPI_CK_PL_HIGH_PH_2EDGE, SPI_PSC_32},
        
        //GPIO配置  PF2 => SPI_INTR, PE3 => MSE1_RESET
        {
            {GPIOF, RCU_GPIOF,  GPIO_PIN_2,     GPIO_MODE_INPUT,    GPIO_PUPD_PULLUP,   -1,  GPIO_OSPEED_60MHZ   },
            {GPIOE, RCU_GPIOE,  GPIO_PIN_3,     GPIO_MODE_OUTPUT,   GPIO_PUPD_PULLUP,   -1,  GPIO_OSPEED_60MHZ   }
        }
        
    },
};









//******************************************************************************
//                                      SPI驱动配置
//******************************************************************************
//-1表示不开启
stSpiDrvDevice_t stSpiDrvDevMap[eSpiNumMax] = 
{
    {
        SPI0,
        RCU_SPI0,
        
        //DMA配置
        {DMA0,  RCU_DMA0,   DMA_CH5,        DMA_REQUEST_SPI0_TX     },
        {DMA0,  RCU_DMA0,   DMA_CH6,        DMA_REQUEST_SPI0_RX     },
        
        //GPIO配置 
        {GPIOD, RCU_GPIOD,  GPIO_PIN_7,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOG, RCU_GPIOG,  GPIO_PIN_9,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOG, RCU_GPIOG,  GPIO_PIN_11,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_15,    GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_reset,                                                         //如果不需要收发api自主控制CS/NSS，给NULL
        gpio_bit_set,                                                           //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
    
    {
        SPI1,
        -1,//RCU_SPI1,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_SPI1_TX     },
        {-1,    -1,         -1,             DMA_REQUEST_SPI1_RX     },
        
        //GPIO配置 
        {GPIOB, RCU_GPIOB,  GPIO_PIN_15,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_14,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_13,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_12,    GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_reset,                                                         //如果不需要收发api自主控制CS/NSS，给NULL
        gpio_bit_set,                                                           //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
    
    {
        SPI2,
        -1,//RCU_SPI2,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_SPI2_TX     },
        {-1,    -1,         -1,             DMA_REQUEST_SPI2_RX     },
        
        //GPIO配置 
        {GPIOC, RCU_GPIOC,  GPIO_PIN_12,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_6,  GPIO_OSPEED_60MHZ   },
        {GPIOC, RCU_GPIOC,  GPIO_PIN_11,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_6,  GPIO_OSPEED_60MHZ   },
        {GPIOC, RCU_GPIOC,  GPIO_PIN_10,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_6,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_15,    GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_reset,                                                         //如果不需要收发api自主控制CS/NSS，给NULL
        gpio_bit_set,                                                           //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
    
    {
        SPI3,
        RCU_SPI3,
        
        //DMA配置
        {DMA0,  RCU_DMA0,   DMA_CH7,        DMA_REQUEST_SPI3_TX     },
        {DMA1,  RCU_DMA1,   DMA_CH0,        DMA_REQUEST_SPI3_RX     },
        
        //GPIO配置 
        {GPIOE, RCU_GPIOE,  GPIO_PIN_6,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOE, RCU_GPIOE,  GPIO_PIN_5,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOE, RCU_GPIOE,  GPIO_PIN_2,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOE, RCU_GPIOE,  GPIO_PIN_4,     GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_reset,                                                         //如果不需要收发api自主控制CS/NSS，给NULL
        gpio_bit_set,                                                           //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
    
    {
        SPI4,
        RCU_SPI4,
        
        //DMA配置
        //{DMA1,  RCU_DMA1,   DMA_CH1,        DMA_REQUEST_SPI4_TX     },
        //{DMA1,  RCU_DMA1,   DMA_CH2,        DMA_REQUEST_SPI4_RX     },
        {-1,  -1,   -1,        DMA_REQUEST_SPI4_TX     },
        {-1,  -1,   -1,        DMA_REQUEST_SPI4_RX     },
        
        //GPIO配置 
        {GPIOJ, RCU_GPIOJ,  GPIO_PIN_10,    GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOJ, RCU_GPIOJ,  GPIO_PIN_11,    GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOK, RCU_GPIOK,  GPIO_PIN_0,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_5,  GPIO_OSPEED_60MHZ   },
        {GPIOK, RCU_GPIOK,  GPIO_PIN_1,     GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        NULL,                                                                   //如果不需要收发api自主控制CS/NSS，给NULL
        NULL,                                                                   //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
    
    {
        SPI5,
        -1,//RCU_SPI5,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_SPI5_TX     },
        {-1,    -1,         -1,             DMA_REQUEST_SPI5_RX     },
        
        //GPIO配置 
        {GPIOA, RCU_GPIOA,  GPIO_PIN_7,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_6,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_5,     GPIO_MODE_AF,           GPIO_PUPD_NONE,     GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_4,     GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_reset,                                                         //如果不需要收发api自主控制CS/NSS，给NULL
        gpio_bit_set,                                                           //如果不需要收发api自主控制CS/NSS，给NULL
        
        //收发缓存地址及大小
        NULL,
        NULL,
        0,
        
    },
    
};








//******************************************************************************
//                                      UART/USART驱动配置
//******************************************************************************
//-1表示不开启
stUsartDrvDevice_t stUsartDrvDevMap[eUsartNumMax] = 
{
    {
        USART0,
        RCU_USART0,
        USART0_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_USART0_RX   },
        {DMA1,  RCU_DMA1,   DMA_CH3,        DMA_REQUEST_USART0_TX   },
        
        //GPIO配置 
        {GPIOA, RCU_GPIOA,  GPIO_PIN_10,    GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {GPIOA, RCU_GPIOA,  GPIO_PIN_9,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {-1,    -1,         -1,             -1,                     GPIO_PUPD_PULLUP,   -1,         -1   },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        USART1,
        RCU_USART1,
        USART1_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_USART1_RX   },
        {-1,    -1,         -1,             DMA_REQUEST_USART1_TX   },
        
        //GPIO配置
        {GPIOD, RCU_GPIOD,  GPIO_PIN_6,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {GPIOD, RCU_GPIOD,  GPIO_PIN_5,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {-1,    -1,         -1,             -1,                     GPIO_PUPD_PULLUP,   -1,         -1                  },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        USART2,
        RCU_USART2,
        USART2_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_USART2_RX   },
        {-1,    -1,         -1,             DMA_REQUEST_USART2_TX   },
        
        //GPIO配置
        {GPIOD, RCU_GPIOD,  GPIO_PIN_9,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {GPIOD, RCU_GPIOD,  GPIO_PIN_8,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {GPIOE, RCU_GPIOE,  GPIO_PIN_15,    GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        UART3,
        RCU_UART3,
        UART3_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_UART3_RX    },
        {-1,    -1,         -1,             DMA_REQUEST_UART3_TX    },
        
        //GPIO配置
        {GPIOD, RCU_GPIOD,  GPIO_PIN_0,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOD, RCU_GPIOD,  GPIO_PIN_1,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {-1,    -1,         -1,             -1,                     GPIO_PUPD_PULLUP,   -1,         -1                  },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        UART4,
        RCU_UART4,
        UART4_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_UART4_RX    },
        {-1,    -1,         -1,             DMA_REQUEST_UART4_TX    },
        
        //GPIO配置
        {GPIOD, RCU_GPIOD,  GPIO_PIN_2,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOC, RCU_GPIOC,  GPIO_PIN_12,    GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {-1,    -1,         -1,             -1,                     GPIO_PUPD_PULLUP,   -1,         -1                  },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        USART5,
        RCU_USART5,
        USART5_IRQn,
        
        //DMA配置
        {DMA1,  RCU_DMA1,   DMA_CH4,        DMA_REQUEST_USART5_RX   },
        {DMA1,  RCU_DMA1,   DMA_CH5,        DMA_REQUEST_USART5_TX   },
        
        //GPIO配置
        {GPIOC, RCU_GPIOC,  GPIO_PIN_7,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {GPIOC, RCU_GPIOC,  GPIO_PIN_6,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_7,  GPIO_OSPEED_60MHZ   },
        {-1,    -1,         -1,             -1,                     GPIO_PUPD_PULLUP,   -1,         -1                  },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        UART6,
        RCU_UART6,
        UART6_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_UART6_RX    },
        {-1,    -1,         -1,             DMA_REQUEST_UART6_TX    },
        
        //GPIO配置
        {GPIOB, RCU_GPIOB,  GPIO_PIN_3,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_11, GPIO_OSPEED_60MHZ   },
        {GPIOB, RCU_GPIOB,  GPIO_PIN_4,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_11, GPIO_OSPEED_60MHZ   },
        {GPIOC, RCU_GPIOC,  GPIO_PIN_13,    GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
    
    {
        UART7,
        RCU_UART7,
        UART7_IRQn,
        
        //DMA配置
        {-1,    -1,         -1,             DMA_REQUEST_UART7_RX    },
        {-1,    -1,         -1,             DMA_REQUEST_UART7_TX    },
        
        //GPIO配置
        {GPIOE, RCU_GPIOE,  GPIO_PIN_0,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOE, RCU_GPIOE,  GPIO_PIN_1,     GPIO_MODE_AF,           GPIO_PUPD_PULLUP,   GPIO_AF_8,  GPIO_OSPEED_60MHZ   },
        {GPIOF, RCU_GPIOF,  GPIO_PIN_3,     GPIO_MODE_OUTPUT,       GPIO_PUPD_NONE,     -1,         GPIO_OSPEED_60MHZ   },
        gpio_bit_set,                                                           //拉高开始发送
        gpio_bit_reset,                                                         //拉低停止发送
        
        //DMA buf地址及大小
        NULL,
        NULL,
        cUsartDmaBufSize,
        true,
        
    },
};








//******************************************************************************
//                                      ENET驱动配置
//******************************************************************************
//-1表示不开启
stEnetDrvDevice_t stEnetDrvDevMap[eEnetNumMax] = 
{
    {
        ENET0,
        RCU_ENET0,
        RCU_ENET0TX,
        RCU_ENET0RX,
        ENET0_IRQn,
        
        true,
        
        ENET_AUTO_NEGOTIATION,
        ENET_AUTOCHECKSUM_DROP_FAILFRAMES,
        ENET_BROADCAST_FRAMES_PASS,
        
        RCU_CKOUT0SRC_PLL0P,
        RCU_CKOUT0_DIV12,
        
        SYSCFG_ENET_PHY_RMII,
        
        {
            {GPIOA,     RCU_GPIOA,      GPIO_PIN_1,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOA,     RCU_GPIOA,      GPIO_PIN_2,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOA,     RCU_GPIOA,      GPIO_PIN_7,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOB,     RCU_GPIOB,      GPIO_PIN_11,    GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOG,     RCU_GPIOG,      GPIO_PIN_13,    GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOG,     RCU_GPIOG,      GPIO_PIN_12,    GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOC,     RCU_GPIOC,      GPIO_PIN_1,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOC,     RCU_GPIOC,      GPIO_PIN_4,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {GPIOC,     RCU_GPIOC,      GPIO_PIN_5,     GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
        },
    },
    
    {
        -1,//ENET1,
        -1,//RCU_ENET1,
        -1,//RCU_ENET1TX,
        -1,//RCU_ENET1RX,
        -1,//ENET1_IRQn,
        
        false,//true,
        
        -1,//ENET_AUTO_NEGOTIATION,
        -1,//ENET_AUTOCHECKSUM_DROP_FAILFRAMES,
        -1,//ENET_BROADCAST_FRAMES_PASS,
        
        -1,//RCU_CKOUT0SRC_PLL0P,
        -1,//RCU_CKOUT0_DIV12,
        
        -1,//SYSCFG_ENET_PHY_RMII,
        
        {
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
            {-1,     -1,      -1,       GPIO_MODE_AF,   GPIO_PUPD_NONE,     GPIO_AF_11,     GPIO_OSPEED_100_220MHZ},
        },
    },
};














//******************************************************************************
//                                      模块电源用户程序配置
//******************************************************************************
const u32 u32ModAppAvgAddrYl[cSdkModAppAvgModNumMax / 2] = { 0, 1 };            //均充算法:接触器Y 靠近左侧(A枪一侧)的模块地址列表
const u32 u32ModAppAvgAddrYr[cSdkModAppAvgModNumMax / 2] = { 2, 3 };            //均充算法:接触器Y 靠近右侧(B枪一侧)的模块地址列表






