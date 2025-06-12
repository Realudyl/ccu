/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_drv.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-22
 * @Attention             :     
 * @Brief                 :     LED驱动程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-22
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _led_drv_H
#define _led_drv_H
#include "en_chg_sdk_cfg.h"
#include "en_common.h"

#include "io_pwm_app.h"








//根据数据手册设置, 典型频率800KHz, LED PWM重装载值为125
#define T1H                             83                                      //LED灯珠信号1对应的装载值
#define T0H                             32                                      //LED灯珠信号0对应的装载值


//风冷终端LED灯板分配情况
#define cLedNum                         10                                      //LED灯珠数量
#define cLedNum_Black                   0                                       //不亮的灯珠数
#define cLedStartIndex_GunA             0                                       //A枪亮的首颗灯珠
#define cLedNum_GunA                    5                                       //A枪亮的灯珠数
#define cLedStartIndex_GunB             (cLedNum_GunA + cLedNum_Black)          //B枪亮的首颗灯珠
#define cLedNum_GunB                    5                                       //B枪亮的灯珠数

//20240624_hason_add:液冷终端LED灯板分配情况
#define cLedNum_spec                    10                                      //LED灯珠数量
#define cLedNum_Black_spec              0                                       //不亮的灯珠数
#define cLedStartIndex_GunA_spec        0                                       //A枪亮的首颗灯珠
#define cLedNum_GunA_spec               10                                      //A枪亮的灯珠数
#define cLedStartIndex_GunB_spec        (cLedNum_GunA + cLedNum_Black)          //B枪亮的首颗灯珠
#define cLedNum_GunB_spec               0                                       //B枪亮的灯珠数



//总的一串灯时间, 240代表reset时间(240*1.2us > 280us)
#define cLedBufSize                     (24 * cLedNum + 240)                    //缓存数组大小








//LED颜色枚举
typedef enum
{
    eLedColorBlack                      = 0x000000,                             //黑色
    eLedColorBlue                       = 0x0000FF,                             //蓝色
    eLedColorRed                        = 0x00FF00,                             //红色
    eLedColorGreen                      = 0xFF0000,                             //绿色
    eLedColorYellow                     = 0xFFFF00,                             //黄色
    eLedColorWhite                      = 0xFFFFFF                              //白色
}eLedColor_t;



//单枪LED灯板配置结构体
typedef struct
{
    u8                                  u8LedStartIndex;                        //对应枪的LED灯板首颗灯
    u8                                  u8LedNum;                               //对应枪的LED灯板灯数
    
}stLedConfig_t;



//LED驱动缓存结构体
typedef struct
{
    stLedConfig_t                       stLedConfig[ePileGunIndexNum];          //对应枪号灯板配置
    
    u16                                 u16LedBuff[cLedBufSize];                //灯板数据缓存数组
    
}stLedDrvCache_t;










//RGB灯板资源初始化
extern void sLedDrvInit(void);
extern void sLedDrvChgParam(bool bIsSuperChg);



//RGB灯板亮灯效果
extern void sLedDrvLight(i32 i32GunIndex, u32 u32Color);
extern void sLedDrvWaterfall(i32 i32GunIndex, u32 u32Color);
extern void sLedDrvBreath(i32 i32GunIndex, u32 u32Color);
extern void sLedDrvBLink(i32 i32GunIndex, u32 u32Color);








#endif
