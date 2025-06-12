/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-20
 * @Attention             :     
 * @Brief                 :     Can驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-20
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _can_drv_H
#define _can_drv_H
#include "gd32h7xx_libopt.h"

#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"












//Can 设备号码定义
typedef enum
{
    eCanNum0                            = 0,                                    //CAN0
    eCanNum1                            = 1,                                    //CAN1
    eCanNum2                            = 2,                                    //CAN2
    
    eCanNumMax,
    
}eCanNum_t;






//Can 消息
typedef struct
{
    u32                                 u32Id;
    u8                                  u8Len;
    u8                                  u8Data[8];
    
}stCanMsg_t;







//Can 邮箱配置
typedef struct
{
    i32                                 i32RxMailBoxIndex;                      //Can 接收邮箱号
    i32                                 i32TxMailBoxIndex;                      //Can 发送邮箱号
    
    i32                                 i32CanRxMailBoxInterrupt;               //Can 接收邮箱中断
    
    i32                                 i32CanRxMailBoxFlag;                    //Can 接收邮箱标志位
    i32                                 i32CanTxMailBoxFlag;                    //Can 发送邮箱标志位
    
    void                                *pRxMailBoxDesc;                        //Can 接收邮箱描述符
    void                                *pTxMailBoxDesc;                        //Can 发送邮箱描述符

    u32                                 u32IdMask;                              //Can 接收邮箱ID掩码
    u32                                 u32PrivateFilter;                       //Can 私有过滤器
    
}stCanDrvCfgMailBox_t;







//Can GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
}stCanDrvCfgGpio_t;





//RX FIFO配置
typedef struct
{
    bool                                bRxFifoEnable;                          //Can RxFIFO功能启用标识位
}stCanDrvRxFifoCfg_t;



//Can 设备
typedef struct
{
    i32                                 i32PeriphCan;                           //Can     外设
    i32                                 i32PeriphRcuCan;                        //Can RCU 外设
    
    i32                                 i32Irq;                                 //Can 中断
    
    stCanDrvRxFifoCfg_t                 stCanDrvRxFifoCfg;                      //Can RxFIFO配置
    
    stCanDrvCfgMailBox_t                stMailBox;                              //Can 邮箱配置
    
    stCanDrvCfgGpio_t                   stGpioRx;                               //Can 接收脚配置
    stCanDrvCfgGpio_t                   stGpioTx;                               //Can 发送脚配置
    
}stCanDrvDevice_t;



//Can 波特率配置
typedef struct
{
    /* Baud = CAN_CLK/((SJW + PTS + PBS1 + PBS2) * PRES) */
    /* CAN_CLK = APB2_CLK */
    i32                                 i32BaudRatePrescaler;                   //波特率预分频位
    i32                                 i32ResynchronizationJumpWidth;          //同步跳转宽度
    i32                                 i32PropagationTimeSegment;              //传播时间段
    i32                                 i32TimeSegment1;                        //时间段1
    i32                                 i32TimeSegment2;                        //时间段2
    
}stCanDrvBaudCfg_t;


















extern bool sCanDrvInitDev(eCanNum_t eCanNum, u32 u32BaudRate);
extern void sCanDrvSetCanIdMaskPrivateFilter(eCanNum_t eCanNum, u32 u32IdMask, u32 u32Filter);















#endif

