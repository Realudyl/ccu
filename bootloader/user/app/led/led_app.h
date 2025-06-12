/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_app.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-22
 * @Attention             :     
 * @Brief                 :     LED用户程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-22
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _led_app_H
#define _led_app_H
#include "led_drv.h"










//LED数据更新时间宏定义
#define cLedUpdateTimeLight             0                                       //常亮
#define cLedUpdateTimeBreath            4                                       //呼吸    周期1s      : 亮->灭 ; 灭->亮
#define cLedUpdateTimeBlink             500                                     //闪烁    周期500ms   : 亮->灭 ; 灭->亮
#define cLedUpdateTimeQuickBlink        200                                     //快闪    周期200ms   : 亮->灭 ; 灭->亮





//LED亮灭类型枚举
typedef enum
{
    eLedSignalTypeLight                 = 0,                                    //常亮
    eLedSignalTypeWaterfall,                                                    //流水
    eLedSignalTypeBreath,                                                       //呼吸
    eLedSignalTypeBlink,                                                        //闪烁
    
    eLedSignalTypeMax
}eLedSignalType_t;




//LED用户程序控制参数结构体
typedef struct
{
    eLedColor_t                         eLedColor;
    eLedSignalType_t                    eLedSignalType;
    
    u32                                 u32TimeStamp;                           //上一次更新数据的时间
    u32                                 u32UpdataTime;                          //数据更新周期
    
}stLedAppCtrlParam_t;










extern void sLedAppOpen(bool bIsSuperCharger);

extern bool sLedAppSet(i32 i32GunIndex, eLedColor_t eColor, eLedSignalType_t eSignalType, u32 u32UpdateTime);











#endif

