/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   i2c_drv.h
* Description                           :   GPIO软件模拟I2C接口驱动程序实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-03
* notice                                :   
****************************************************************************************************/
#ifndef _i2c_drv_H
#define _i2c_drv_H
#include "gd32h7xx_libopt.h"

#include "en_common.h"
#include "en_mem.h"







//I2c 设备号码定义
typedef enum
{
    eI2cNum0                            = 0,                                    //
    
    eI2cNumMax,
}eI2cNum_t;






//I2c GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Speed;                               //GPIO     速率
    
}stI2cDrvCfgGpio_t;







//I2c 设备
typedef struct
{
    stI2cDrvCfgGpio_t                   stGpioScl;                              //I2c Scl 脚配置
    stI2cDrvCfgGpio_t                   stGpioSda;                              //I2c Sda 脚配置
    
}stI2cDrvDevice_t;




















extern void sI2cDrvStart   (eI2cNum_t eI2cNum);
extern void sI2cDrvStop    (eI2cNum_t eI2cNum);
extern u8   sI2cDrvWaitAck (eI2cNum_t eI2cNum);
extern void sI2cDrvSendAck (eI2cNum_t eI2cNum);
extern void sI2cDrvSendNAck(eI2cNum_t eI2cNum);
extern u8   sI2cDrvRecvByte(eI2cNum_t eI2cNum);
extern void sI2cDrvSendByte(eI2cNum_t eI2cNum, u8 u8Byte);


extern bool sI2cDrvOpen(eI2cNum_t eI2cNum);














#endif








