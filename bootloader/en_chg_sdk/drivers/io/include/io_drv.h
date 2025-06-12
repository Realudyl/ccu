/**
 * @file io_drv.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief io driver data base
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"


//io最大数量
#ifndef cSdkIoDrvPinMaxNum
#define cIoDrvPinMaxNum                 (39)
#else
#define cIoDrvPinMaxNum                 (cSdkIoDrvPinMaxNum)
#endif



/**
 * @brief 引脚参数配置表
*/
typedef struct
{
    u32                                 u32IoIndex;                             //GPIO引脚序号
    u32                                 u32Rcu;                                 //RCU时钟
    u32                                 u32Port;                                //GPIO端口
    u32                                 u32Pin;                                 //GPIO引脚
    u32                                 u32PinInOutMode;                        //GPIO引脚模式
    u32                                 u32PinPullUpDownMode;                   //GPIO引脚上拉下拉电阻设置
    u32                                 u32PinOutType;                          //GPIO输出引脚输出类型
    u32                                 u32PinOutSpeed;                         //GPIO输出引脚速度
    u8                                  u32PinInPeriod;                         //GPIO输入引脚过滤采样周期
    u32                                 u32PinInFType;                          //GPIO输入引脚过滤方式
}stIoDrvPinPara_t;





/**
 * @brief io驱动设备参数配置表控制块
*/
typedef struct
{
    u32                                 u32PinNum;                              //io数量
    stIoDrvPinPara_t                    stIoDrvParaMap[cIoDrvPinMaxNum];        //引脚参数配置表
}stIoDrvDevice_t;





extern bool sIoDrvInitDev(void);
extern bool sIoDrvInit(stIoDrvDevice_t* io);



#ifdef __cplusplus
}
#endif





