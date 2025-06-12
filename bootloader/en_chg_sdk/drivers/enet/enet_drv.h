/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     enet_drv.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-9-9
 * @Attention             :     
 * @Brief                 :     以太网驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-9-9
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _enet_drv_H
#define _enet_drv_H

#include "gd32h7xx_libopt.h"
#include "en_common.h"







//根据<C703537_以太网收发器_IP101GRI_规格书_IC+(九阳电子)以太网芯片规格书.PDF> 21页定义
#define cEnetPhyLinkStatus              BIT(2)







//Enet 设备号码定义
typedef enum
{
    eEnetNum0                           = 0,                                    //ENET0
    eEnetNum1                           = 1,                                    //ENET1
    
    eEnetNumMax,
}eEnetNum_t;








// enet gpio管脚定义
typedef enum
{
    eEnetRefClk = 0,                    //REF_CLK
    eEnetMdio,                          //MDIO
    eEnetCrsDv,                         //CRS_DV
    eEnetTxEn,                          //TX_EN
    eEnetTxD0,                          //TXD0
    eEnetTxD1,                          //TXD1
    eEnetMdc,                           //MDC
    eEnetRxD0,                          //RXD0
    eEnetRxD1,                          //RXD1
    
    eEnetMax,
}eEnetGpio_t;





//enet gpio管脚的配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
}stEnetDrvCfgGpio_t;





//enet 设备
typedef struct
{
    i32                                 i32PeriphEnet;                          //enet外设：ENET0,1
    i32                                 i32PeriphRcuEnet;                       //该外设     的RCU
    i32                                 i32PeriphRcuEnetTx;                     //该外设TX   的RCU
    i32                                 i32PeriphRcuEnetRx;                     //该外设RX   的RCU
    i32                                 i32Irq;                                 //          中断函数
    
    bool                                bDmaEnable;                             //          DMA使能
    
    i32                                 i32MediaModeConf;                       //PHY模式和MAC环回配置
    i32                                 i32CheckSumConf;                        //IP帧校验和卸载功能
    i32                                 i32FramFiltConf;                        //帧滤波功能
    
    i32                                 i32CkOut0Src;                           //CK_OUT0时钟源
    i32                                 i32CkOut0Div;                           //CK_OUT0分频器
    
    i32                                 i32PhyInterface;                        //ENET媒体接口模式
    
    stEnetDrvCfgGpio_t                  stGpio[eEnetMax];                       //GPIO配置
    
    
}stEnetDrvDevice_t;





// ENET驱动 API函数 ———— 设备状态寄存器获取
extern bool sEnetDrvStatusRegGet(eEnetNum_t eEnetNum, u16* pStatusValue);

// ENET驱动 API函数 ———— MAC地址设置
extern bool sEnetDrvMacAddrSet(eEnetNum_t eEnetNum, u8* pMacAddr);

// ENET驱动 API函数 ———— 发送数据(供网口为透传模式时调用)
bool sEnetDrvSendData(eEnetNum_t eEnetNum, u8* pBuf, u32 u32Len);

// ENET驱动 API函数 ———— 设备初始化
extern bool sEnetDrvInitDevInit(eEnetNum_t eEnetNum);

// ENET驱动 初始化
extern bool sEnetDrvInit(eEnetNum_t eEnetNum);







#endif
