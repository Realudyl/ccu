/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv_proc.c
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
#include "rtc_drv_proc.h"












bool sIsLeapYear(u16 u16Year);
i32  sGetWeekDay(i32 i32Year, i32 i32Month, i32 i32Day);

bool sTimeStampToDateTime(eTimeZone_t eTimeZone, u32 u32TimeStamp, stRtcCalendar_t* pCalendar);











/*******************************************************************************
 * @FunctionName   :      sIsLeapYear
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:56
 * @Description    :      闰年判断
 * @Input          :      u16Year     传入要判断的年份
 * @Return         :      
*******************************************************************************/
bool sIsLeapYear(u16 u16Year)
{
    return ((u16Year % 4 == 0 && u16Year % 100 != 0) || u16Year % 400 == 0);
}









/*******************************************************************************
 * @FunctionName   :      sGetWeekDay
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:48
 * @Description    :      通过 蔡勒公式 以及 闰年与非闰年调整 进行 星期 计算
 * @Input          :      i32Year     传入要计算的年份
 * @Input          :      i32Month    传入要计算的月份
 * @Input          :      i32Day      传入要计算的日期
 * @Return         :      i32         星期值(1~7)
*******************************************************************************/
i32 sGetWeekDay(i32 i32Year, i32 i32Month, i32 i32Day)
{
    u8 u8Leap = 0;
    u8 u8Adjust[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4}; // 加上闰年的调整
    
    if (i32Month < 3)
    {
        //因为蔡勒公式中的月份是从 3 月开始算的，即 3 代表 3 月，4 代表 4 月，以此类推。
        //而对于一月和二月，我们需要将它们看作上一年的第 13 个月和第 14 个月。
        i32Year--;
    }
    
    //判断年份是否为闰年
    u8Leap = sIsLeapYear(i32Year);
    
    //返回值通过 蔡勒公式 计算
    return (i32Day + u8Adjust[i32Month-1] + i32Year + i32Year / 4 - i32Year / 100 + i32Year / 400 + u8Leap) % 7;
}










/*******************************************************************************
 * @FunctionName   :      sTimeStampToDateTime
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:30:00
 * @Description    :      将 时间戳 转换为 日历
 * @Input          :      eTimeZone         时区
 * @Input          :      u32TimeStamp      时间戳(传入的时间戳默认为零时区)
 * @Input          :      pCalendar         传入的日历结构体
 * @Return         :      
*******************************************************************************/
bool sTimeStampToDateTime(eTimeZone_t eTimeZone, u32 u32TimeStamp, stRtcCalendar_t* pCalendar)
{
    u8  u8DaysInMonth[]        = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};      //十二个月份的各个天数
    u32 u32SecondsOfPerDay     = 24 * 60 * 60;                                          //一天的秒钟数
    u32 u32DaysSinceEpoch      = 0;                                                     //距离纪元以来的天数
    u32 u32SecondsOfToday      = 0;                                                     //当天已经过去的秒数
    
    
    //根据时区对时间戳进行加减
    u32TimeStamp = u32TimeStamp + (eTimeZone * (60 * 60));
    
    
    //计算距离纪元以来的天数 及 当天已经过去的秒数
    u32DaysSinceEpoch  = u32TimeStamp / u32SecondsOfPerDay;
    u32SecondsOfToday  = u32TimeStamp % u32SecondsOfPerDay;
    
    
    // 计算年份
    pCalendar->u16Year = 1970;
    
    while(u32DaysSinceEpoch >= (sIsLeapYear(pCalendar->u16Year) ? 366 : 365))
    {
        u32DaysSinceEpoch -= sIsLeapYear(pCalendar->u16Year) ? 366 : 365;
        (pCalendar->u16Year)++;
    }
    
    
    // 计算月份和日期
    if(sIsLeapYear(pCalendar->u16Year))
    {
        u8DaysInMonth[1] = 29;
    }
    
    for(pCalendar->u8Month = 1; u32DaysSinceEpoch >= u8DaysInMonth[pCalendar->u8Month - 1]; (pCalendar->u8Month)++)
    {
        u32DaysSinceEpoch -= u8DaysInMonth[pCalendar->u8Month - 1];
    }
    pCalendar->u8Date = u32DaysSinceEpoch + 1;
    
    
    // 计算星期
    pCalendar->u8DayOfWeek = sGetWeekDay(pCalendar->u16Year, pCalendar->u8Month, pCalendar->u8Date);
    
    
    // 计算小时、分钟和秒
    pCalendar->u8Hour   =  u32SecondsOfToday / 3600;
    pCalendar->u8Minute = (u32SecondsOfToday % 3600) / 60;
    pCalendar->u8Second = (u32SecondsOfToday % 3600) % 60;
    
    
    return(true);
}











