/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     mse102x_drv.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-8
 * @Attention             :     根据 联芯通<VCAN-102X-I07-110B_MSE102X SPI Application Note_V1.0.pdf>开发
 * @Brief                 :     
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-8
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _mse102x_drv_H
#define _mse102x_drv_H

#include "gd32h7xx_libopt.h"

#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "spi_drv_trans.h"







//--------------------------------------mse102x设备驱动部分---------------------


//PLC/MSE102x 通道数
#ifndef cSdkMse102xDevNum
#define cMse102xDevNum                  (1)
#else
#define cMse102xDevNum                  (cSdkMse102xDevNum)
#endif







//mse102x GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
}stMse102xDrvCfgGpio_t;



//mse102x GPIO管脚定义
typedef enum
{
    eMse102xDrvCfgGpioIntRx             = 0,                                    //SPI_S_INTR
    eMse102xDrvCfgGpioReset             = 1,                                    //SPI_RESET
    
    eMse102xDrvCfgGpioMax,
}eMse102xDrvCfgGpio_t;





//mse102x 设备
typedef struct
{
    eSpiNum_t                           eSpiNum;                                //SPI端口
    stSpiDrvCfg_t                       stSpiCfg;                               //SPI配置
    stMse102xDrvCfgGpio_t               stGpio[eMse102xDrvCfgGpioMax];          //GPIO配置
    
}stMse102xDrvDevice_t;



























//--------------------------------------spi协议部分-----------------------------

//mse102x spi协议 分隔符（16bit） 定义
typedef enum
{
    eMse102xDrvSpiDelimiterCmd          = 0x01,                                 //命令帧
    eMse102xDrvSpiDelimiterSof          = 0x02,                                 //数据帧
    
    eMse102xDrvSpiDelimiterMax          = 0xffff                                //确保字段占16位
}__attribute__((packed)) eMse102xDrvSpiDelimiter_t;



//mse102x spi协议 命令类型 定义
typedef enum
{
    eMse102xDrvSpiCmdTypeRts            = 0x01,                                 //请求发送
    eMse102xDrvSpiCmdTypeCtr            = 0x02,                                 //确认接收
    
    eMse102xDrvSpiCmdTypeMax
}__attribute__((packed)) eMse102xDrvSpiCmdType_t;




//mse102x spi协议 命令帧格式 定义
typedef struct 
{
    u16                                 bPara   : 12;                           //Command Parameters
    eMse102xDrvSpiCmdType_t             bType   : 4;                            //Command Type
    
}__attribute__((packed)) stMse102xDrvSpiCmd_t;


//mse102x spi协议 命令帧格式 定义
typedef union 
{
    stMse102xDrvSpiCmd_t                stCmd;                                  //结构体访问
    u16                                 u16Cmd;                                 //16位访问
}__attribute__((packed)) unMse102xDrvSpiCmd_t;




//mse102x spi协议 命令帧格式 定义
typedef struct 
{
    eMse102xDrvSpiDelimiter_t           eDelimiter;                             //分隔符
    unMse102xDrvSpiCmd_t                unCmd;                                  //命令码
}__attribute__((packed)) stMse102xDrvSpiFrameCmd_t;




//mse102x spi协议 数据帧 最大数据长度
#define cMse102xDrvSpiSofDataLenMax     (1024 + 512)

//mse102x spi协议 Data Frame Tail (DFT) 定义---16bit
typedef enum
{
    eMse102xDrvSpiDft                   = 0x55aa,
    
    eMse102xDrvSpiDftMax                = 0xffff
}__attribute__((packed)) eMse102xDrvSpiDft_t;


//mse102x spi协议 数据帧格式 定义
typedef struct 
{
    eMse102xDrvSpiDelimiter_t           eDelimiter;                             //分隔符
    u8                                  u8Data[cMse102xDrvSpiSofDataLenMax];    //数据部分
    eMse102xDrvSpiDft_t                 eDft;                                   //帧尾
}__attribute__((packed)) stMse102xDrvSpiFrameSof_t;













#define cMse102xDrvHpavQueueSize        (20)
#define cMse102xDrvHpavQueueBufLen      (cMse102xDrvSpiSofDataLenMax * 4)

#ifdef DTT
#define cMse102xDrvEnetQueueSize        (20)
#define cMse102xDrvEnetQueueBufLen      (ETH_PACKET_MAX_VLAN_SIZE * 4)
#endif

//mse102x spi协议 驱动缓存结构
typedef struct 
{
    stMse102xDrvSpiFrameSof_t           stRxFrameSof[cMse102xDevNum];           //接收缓存
    stMse102xDrvSpiFrameSof_t           stTxFrameSof[cMse102xDevNum];           //发送缓存
    
    //收到的homeplug帧无法通过lwip协议栈处理
    //需要入列, 接收方直接出列
    stQueue_t                           stHpavQueue[cMse102xDevNum];            //homeplug帧队列
    SemaphoreHandle_t                   xSemHpavQueue[cMse102xDevNum];          //homeplug帧队列访问信号量
    
    //发送函数锁
    SemaphoreHandle_t                   xSemSend[cMse102xDevNum];               
    
#ifdef DTT
    //收到的以太网帧报文通过出入队列交由上层处理
    //需要入列, 上层直接出列
    stQueue_t                           stEnetQueue[cMse102xDevNum];            //以太网帧报文队列
    SemaphoreHandle_t                   xSemEnetQueue[cMse102xDevNum];          //以太网帧报文队列访问信号量
#endif
}stMse102xDrvCache_t;














//MSE102X消息中断————MSE102X有消息要发送时会拉高该引脚电平
#define sMse102xDrvHasData(port, pin)      (gpio_input_bit_get(port, pin) ? true : false)






















extern bool sMse102xOpen(i32 i32Channel);
extern i32  sMse102xReadHpav(i32 i32Channel, i32 i32MaxLen, u8 *pBuf);
extern bool sMse102xSend(i32 i32Channel, i32 i32DataLen, const u8 *pData);

#ifdef DTT
extern i32  sMse102xReadEnet(i32 i32Channel, i32 i32MaxLen, u8 *pBuf);
#endif













#endif

