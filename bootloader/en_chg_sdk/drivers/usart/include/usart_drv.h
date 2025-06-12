/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv.h
* Description                           :   串口驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-27
* notice                                :   
****************************************************************************************************/
#ifndef _usart_drv_H_
#define _usart_drv_H_
#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_mem.h"





//定义串口驱动DMA缓存区长度 必须保持 32字节 的整数倍
//原因是此缓存区在经DMA操作时需要用 SCB_CleanDCache_by_Addr 等api函数去操作cache
//这些cache操作函数的dsize参数需要保持  32字节 的整数倍
#ifndef cSdkUsartDmaBufSize
#define cUsartDmaBufSize                (256)
#else
#define cUsartDmaBufSize                (cSdkUsartDmaBufSize)
#endif


//Usart 设备号码定义
typedef enum
{
    eUsartNum0                          = 0,                                    //
    eUsartNum1                          = 1,                                    //
    eUsartNum2                          = 2,                                    //
    eUsartNum3                          = 3,                                    //
    eUsartNum4                          = 4,                                    //
    eUsartNum5                          = 5,                                    //
    eUsartNum6                          = 6,                                    //
    eUsartNum7                          = 7,                                    //
    
    eUsartNumMax,
}eUsartNum_t;








//Usart DMA配置
typedef struct
{
    i32                                 i32Periph;                              //DMA     外设
    i32                                 i32PeriphRcu;                           //DMA RCU 外设
    i32                                 i32Chan;                                //DMA     通道
    i32                                 i32Req;                                 //DMAMUX  请求路由通道的DMA请求输入源
}stUsartDrvCfgDma_t;



//Usart GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
    
}stUsartDrvCfgGpio_t;



//Usart 设备
typedef struct
{
    i32                                 i32PeriphUsart;                         //Usart     外设
    i32                                 i32PeriphRcuUsart;                      //Usart RCU 外设
    i32                                 i32Irq;                                 //Usart 中断
    stUsartDrvCfgDma_t                  stDmaRx;                                //Usart 接收DMA配置
    stUsartDrvCfgDma_t                  stDmaTx;                                //Usart 发送DMA配置
    stUsartDrvCfgGpio_t                 stGpioRx;                               //Usart 接收脚配置
    stUsartDrvCfgGpio_t                 stGpioTx;                               //Usart 发送脚配置
    stUsartDrvCfgGpio_t                 stGpioRs485;                            //Usart 485发送使能脚配置
    void                                (*pTxEnable)(u32 u32Gpio, u32 u32Pin);  //Usart 485发送使能函数
    void                                (*pTxDisable)(u32 u32Gpio, u32 u32Pin); //Usart 485发送禁止函数
    
    u8                                  *pDmaRxBuf;                             //Usart DMA接收缓存地址
    u8                                  *pDmaTxBuf;                             //Usart DMA发送缓存地址
    i32                                 i32DmaBufSize;                          //Usart DMA缓存大小
    
    //Usart串口设备从不发送状态 开始发送新的一帧时
    //第一个字节需要由发送函数启动 后续字节由中断自主完成
    //需要本标志来实现该逻辑
    bool                                bFirstByteFlag;                         //Usart 首字节标志
}stUsartDrvDevice_t;










//Usart 配置
typedef struct
{
    i32                                 i32Baudrate;                            //波特率
    i32                                 i32WordLen;                             //数据长度
    i32                                 i32StopBit;                             //停止位
    i32                                 i32Praity;                              //奇偶校验
}stUsartDrvCfg_t;









































extern bool sUsartDrvInitDev(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg);








#endif



















