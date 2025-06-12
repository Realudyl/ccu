/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-4
 * @Attention             :     
 * @Brief                 :     Rtc驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-4
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _rtc_drv_H
#define _rtc_drv_H
#include <time.h>

#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_log.h"
#include "en_shell.h"









//这个宏定义用来标识RTC是否已经初始化
#define cRtcInitFinshedFlag             0x0429

//默认RTC时区(东八区)
#define cRtcTimeZoneDefault             eTimeZoneUtcPos8











//RTC 日历结构体
typedef struct
{
    u16 u16Year;
    u8  u8Month;
    u8  u8Date;
    u8  u8DayOfWeek;
    u8  u8Hour;
    u8  u8Minute;
    u8  u8Second;
    
}stRtcCalendar_t;
















extern bool sRtcDrvInitDev(void);

















#endif

