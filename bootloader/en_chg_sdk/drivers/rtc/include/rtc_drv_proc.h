/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv_proc.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-4
 * @Attention             :     
 * @Brief                 :     Rtc驱动实现 之 数据处理
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-4
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _rtc_drv_proc_H
#define _rtc_drv_proc_H

#include "gd32h7xx_libopt.h"

#include "en_common.h"

#include "rtc_drv.h"








extern bool sIsLeapYear(u16 u16Year);
extern i32  sGetWeekDay(i32 i32Year, i32 i32Month, i32 i32Day);

extern bool sTimeStampToDateTime(eTimeZone_t eTimeZone, u32 u32TimeStamp, stRtcCalendar_t* pCalendar);















#endif

