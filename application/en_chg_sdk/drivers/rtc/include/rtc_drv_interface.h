/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv_interface.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-4
 * @Attention             :     
 * @Brief                 :     Rtc驱动实现 之 对外接口
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-4
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _rtc_drv_interface_H
#define _rtc_drv_interface_H

#include "gd32h7xx_libopt.h"

#include "en_common.h"

#include "rtc_drv.h"









extern bool sRtcTimeDisplay(void);

extern i8   sRtcGetTimeZone(void);
extern bool sRtcSetTimeZone(i8 i8TimeZone);

extern bool sRtcGetCalendarTime(unFullTime_t* pFullTime);
extern u32  sRtcGetTimeStamp(void);

extern bool sRtcSetByCalendar(stRtcCalendar_t* pCalendar);
extern bool sRtcSetByTimeStamp(u32 u32TimeStamp);











#endif
