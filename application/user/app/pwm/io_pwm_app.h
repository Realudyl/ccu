/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   io_pwm_app.h
* Description                           :   pwm用户程序
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-09
* notice                                :   
****************************************************************************************************/
#ifndef _io_pwm_app_H_
#define _io_pwm_app_H_
#include "io_pwm_drv_api.h"











//PWM通道定义
typedef enum
{
    eIoPwm_CP_DR_A                      = 0,                                    //A枪      PWM打波
    eIoPwm_CP_DR_B                      = 1,                                    //B枪      PWM打波
    eIoPwm_LED_Board                    = 2,                                    //LED灯板  PWM打波
    eIoPwm_LED_Band                     = 3,                                    //LED灯带  PWM打波
    eIoPwm_FAN                          = 4,                                    //散热风扇
    
    eIoPwmMax,
}eIoPwm_t;







//PWM通道管理app层数据结构定义
typedef struct
{
    //----------------------------------与充电枪相关的资源----------------------
    //供各用户程序基于枪序号 ePileGunIndex_t 去操作
    eIoPwm_t                            eIoPwmCp[ePileGunIndexNum];             //CP振荡器输出脚
    
}stIoPwmAppCache_t;



















extern bool  sIoPwmAppInit(void);

//对外的api接口函数---基于枪序号操作
extern eIoPwm_t sIoPwmGetCp(ePileGunIndex_t eGunIndex);
extern void sIoPwmSetCpOnOff(ePileGunIndex_t eGunIndex, bool bOnOff);


extern bool sIoPwmSetFanDuty(u32 u32Duty);





#endif



















