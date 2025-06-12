/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   io_pwm_drv_api.h
* Description                           :   PWM驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-31
* notice                                :   
****************************************************************************************************/
#ifndef _io_pwm_drv_api_H_
#define _io_pwm_drv_api_H_
#include "io_pwm_drv.h"



















extern bool sIoPwmDrvSetPwm(i32 i32IoPwmIndex, bool bOnFlag);
extern bool sIoPwmDrvSetPwmDuty(i32 i32IoPwmIndex, u32 u32Duty);
extern bool sIoPwmDrvSendData(i32 i32IoPwmIndex, u16 *pData, u32 u32DataByte);




#endif















