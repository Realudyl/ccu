/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   spi_drv.h
* Description                           :   SPI驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#ifndef _spi_drv_H_
#define _spi_drv_H_
#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_mem.h"






//定义SPI口驱动DMA缓存区长度 必须保持 32字节 的整数倍
//原因是此缓存区在经DMA操作时需要用 SCB_CleanDCache_by_Addr 等api函数去操作cache
//这些cache操作函数的dsize参数需要保持  32字节 的整数倍
#define cSpiDmaBufSize                  256


//Spi 设备号码定义
typedef enum
{
    eSpiNum0                            = 0,                                    //
    eSpiNum1                            = 1,                                    //
    eSpiNum2                            = 2,                                    //
    eSpiNum3                            = 3,                                    //
    eSpiNum4                            = 4,                                    //
    eSpiNum5                            = 5,                                    //
    
    eSpiNumMax,
}eSpiNum_t;








//Spi DMA配置
typedef struct
{
    i32                                 i32Periph;                              //DMA     外设
    i32                                 i32PeriphRcu;                           //DMA RCU 外设
    i32                                 i32Chan;                                //DMA     通道
    i32                                 i32Req;                                 //DMAMUX  请求路由通道的DMA请求输入源
}stSpiDrvCfgDma_t;



//Spi GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
    
}stSpiDrvCfgGpio_t;



//Spi 设备
typedef struct
{
    i32                                 i32PeriphSpi;                           //Spi     外设
    i32                                 i32PeriphRcuSpi;                        //Spi RCU 外设
    stSpiDrvCfgDma_t                    stDmaMosi;                              //Spi MOSI DMA配置
    stSpiDrvCfgDma_t                    stDmaMiso;                              //Spi MISO DMA配置
    stSpiDrvCfgGpio_t                   stGpioMosi;                             //Spi MOSI 脚配置
    stSpiDrvCfgGpio_t                   stGpioMiso;                             //Spi MISO 脚配置
    stSpiDrvCfgGpio_t                   stGpioSck;                              //Spi SCK  脚配置
    stSpiDrvCfgGpio_t                   stGpioNss;                              //Spi NSS  脚配置
    
    void                                (*pNssLow)(u32 u32Gpio, u32 u32Pin);    //Spi NSS  脚拉低---NSS信号收发api自主控制时使用
    void                                (*pNssHigh)(u32 u32Gpio, u32 u32Pin);   //Spi NSS  脚拉高
    
    u8                                  *pDmaRxBuf;                             //Spi DMA接收缓存地址
    u8                                  *pDmaTxBuf;                             //Spi DMA发送缓存地址
    i32                                 i32DmaBufSize;                          //Spi DMA缓存大小
    
}stSpiDrvDevice_t;











//Spi 配置
typedef struct
{
    i32                                 i32TransMode;                           //SPI 传输类型
    i32                                 i32DeviceMode;                          //SPI master/slave
    i32                                 i32DataSize;                            //SPI data frame size
    i32                                 i32Nss;                                 //SPI NSS control by handware or software
    i32                                 i32Endian;                              //SPI big endian or little endian
    i32                                 i32ClockPolarityPhase;                  //SPI clock phase and polarity
    i32                                 i32Prescale;                            //SPI prescale factor
    
    
}stSpiDrvCfg_t;























extern bool sSpiDrvInitDev(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg);






#endif






















