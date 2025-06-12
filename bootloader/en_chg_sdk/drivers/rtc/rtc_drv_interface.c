/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv_interface.c
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
#include "en_log.h"

#include "rtc_drv_interface.h"
#include "rtc_drv_proc.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelInfo
static const char *TAG = "rtc_drv_interface";




extern i8 i8RtcTimeZone;
extern bool bRtcSrcFlag;
extern bool bRtcInitFlag;
extern SemaphoreHandle_t xSemRtcQueue;










bool sRtcTimeDisplay(void);


i8   sRtcGetTimeZone(void);
bool sRtcSetTimeZone(i8 i8TimeZone);


bool sRtcGetCalendarTime(unFullTime_t* pFullTime);
u32  sRtcGetTimeStamp(void);


bool sRtcSetByCalendar(stRtcCalendar_t* pCalendar);
bool sRtcSetByTimeStamp(u32 u32TimeStamp);













/*******************************************************************************
 * @FunctionName   :      sRtcTimeDisplay
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月11日  20:45:50
 * @Description    :      Rtc对外接口 之 RTC时间打印
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sRtcTimeDisplay(void)
{
    rtc_parameter_struct stRtcInitPara;
    
    u32 u32TimeSubSecond = 0;
    u8  u8SubSecondSs = 0, u8SubSecondTs = 0, u8SubSecondHs = 0;
    
    if(bRtcInitFlag != true)
    {
        return(false);
    }
    
    if(xSemaphoreTake(xSemRtcQueue, 2000 / portTICK_RATE_MS) != pdTRUE)
    {
        return(false);
    }
    
    //等待同步
    if(ERROR == rtc_register_sync_wait())
    {
        EN_SLOGE(TAG, "等待同步超时! 获取当前时间失败!");
        
        return(false);
    }
    
    //获取当前时间
    rtc_current_time_get(&stRtcInitPara);
    
    xSemaphoreGive(xSemRtcQueue);
    
    //获取当前时间的亚秒值,并将其转换为小数格式
    u32TimeSubSecond = rtc_subsecond_get();
    
    u8SubSecondSs = (1000 - (u32TimeSubSecond * 1000 + 1000) / 400) / 100;
    u8SubSecondTs = (1000 - (u32TimeSubSecond * 1000 + 1000) / 400) % 100 / 10;
    u8SubSecondHs = (1000 - (u32TimeSubSecond * 1000 + 1000) / 400) % 10;
    
    EN_SLOGI(TAG, "当前时间: %0.2x:%0.2x:%0.2x .%d%d%d", \
           stRtcInitPara.hour, stRtcInitPara.minute, stRtcInitPara.second, \
           u8SubSecondSs, u8SubSecondTs, u8SubSecondHs);
    
    return(true);
}






/**********************************************************************************************
* Description                           :   Rtc对外接口 之 RTC时区获取
* Author                                :   Hall
* modified Date                         :   2024-06-19
* notice                                :   
***********************************************************************************************/
i8 sRtcGetTimeZone(void)
{
    if(bRtcInitFlag != true)
    {
        return(eTimeZoneUtcMin);
    }
    
    return i8RtcTimeZone;
}








/*******************************************************************************
 * @FunctionName   :      sRtcSetTimeZone
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月1日  13:40:20
 * @Description    :      Rtc对外接口 之 RTC时区设置
 * @Input          :      i8TimeZone    时区 ———— 东八区(8) 西八区(-8) 格林威治时区(0)
 * @Return         :      
*******************************************************************************/
bool sRtcSetTimeZone(i8 i8TimeZone)
{
    if(bRtcInitFlag != true)
    {
        return(false);
    }
    
    //时区限幅
    if((i8TimeZone > eTimeZoneUtcMax) || (i8TimeZone < eTimeZoneUtcMin))
    {
        EN_SLOGE(TAG, "时区超幅! 时间戳获取失败!");
        return(false);
    }
    
    i8RtcTimeZone = i8TimeZone;
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sRtcGetCalendarTime
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月12日  11:28:02
 * @Description    :      Rtc对外接口 之 获取RTC日历
 * @Input          :      pFullTime     年份完整值联合体(en_common.h)
 * @Return         :      
*******************************************************************************/
bool sRtcGetCalendarTime(unFullTime_t* pFullTime)
{
    rtc_parameter_struct stRtcInitPara;
    
    if(bRtcInitFlag != true)
    {
        return(false);
    }
    
    if(xSemaphoreTake(xSemRtcQueue, 2000 / portTICK_RATE_MS) != pdTRUE)
    {
        return(false);
    }
    
    //等待同步
    if(ERROR == rtc_register_sync_wait())
    {
        EN_SLOGE(TAG, "等待同步超时! 获取RTC日历失败!");
        return(false);
    }
    
    //获取当前时间
    rtc_current_time_get(&stRtcInitPara);
    
    xSemaphoreGive(xSemRtcQueue);
    
    pFullTime->rtc.year = 2000 + ((stRtcInitPara.year   >> 4) * 10 + (stRtcInitPara.year   & 0x0F));
    pFullTime->rtc.mon  =         (stRtcInitPara.month  >> 4) * 10 + (stRtcInitPara.month  & 0x0F);
    pFullTime->rtc.day  =         (stRtcInitPara.date   >> 4) * 10 + (stRtcInitPara.date   & 0x0F);
    pFullTime->rtc.hour =         (stRtcInitPara.hour   >> 4) * 10 + (stRtcInitPara.hour   & 0x0F);
    pFullTime->rtc.min  =         (stRtcInitPara.minute >> 4) * 10 + (stRtcInitPara.minute & 0x0F);
    pFullTime->rtc.sec  =         (stRtcInitPara.second >> 4) * 10 + (stRtcInitPara.second & 0x0F);
    
    return(true);
}











/*******************************************************************************
 * @FunctionName   :      sRtcGetTimeStamp
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:43
 * @Description    :      Rtc对外接口 之 获取RTC时间戳
 * @Return         :      u32           时间戳(经过时区处理的时间戳)
*******************************************************************************/
u32 sRtcGetTimeStamp(void)
{
    rtc_parameter_struct stRtcInitPara;
    
    u32 u32Year = 0, u32Month = 0, u32Day = 0;
    u32 u32Hour = 0, u32Minute = 0, u32Second = 0;
    u32 u32TimeStamp = 0;
    
    if(bRtcInitFlag != true)
    {
        return(0);
    }
    
    if(xSemaphoreTake(xSemRtcQueue, 2000 / portTICK_RATE_MS) != pdTRUE)
    {
        return(false);
    }
    
    //等待同步
    if(ERROR == rtc_register_sync_wait())
    {
        EN_SLOGE(TAG, "等待同步超时! 获取RTC时间戳失败!");
        return(0);
    }
    
    //获取当前时间
    rtc_current_time_get(&stRtcInitPara);
    
    xSemaphoreGive(xSemRtcQueue);
    
    u32Year     = 2000 + ((stRtcInitPara.year   >> 4) * 10 + (stRtcInitPara.year   & 0x0F));
    u32Month    =         (stRtcInitPara.month  >> 4) * 10 + (stRtcInitPara.month  & 0x0F);
    u32Day      =         (stRtcInitPara.date   >> 4) * 10 + (stRtcInitPara.date   & 0x0F);
    u32Hour     =         (stRtcInitPara.hour   >> 4) * 10 + (stRtcInitPara.hour   & 0x0F);
    u32Minute   =         (stRtcInitPara.minute >> 4) * 10 + (stRtcInitPara.minute & 0x0F);
    u32Second   =         (stRtcInitPara.second >> 4) * 10 + (stRtcInitPara.second & 0x0F);
    
    EN_SLOGD(TAG, "当前时间: %04d年%02d月%02d日  星期%d  %02d:%02d:%02d (UTC/GMT %d)",
        u32Year, u32Month, u32Day, stRtcInitPara.day_of_week, u32Hour, u32Minute, u32Second, i8RtcTimeZone);
    
    struct tm stTimeInfo =
    {
        .tm_year    = u32Year  - 1900,
        .tm_mon     = u32Month - 1,
        .tm_mday    = u32Day,
        .tm_hour    = u32Hour,
        .tm_min     = u32Minute,
        .tm_sec     = u32Second
    };
    
    
    //将N时区时间戳还原为零时区时间戳
    u32TimeStamp = mktime(&stTimeInfo) - (i8RtcTimeZone * (60 * 60));
    
    EN_SLOGD(TAG, "当前时间戳: %d", u32TimeStamp);
    
    
    return(u32TimeStamp);
}





/*******************************************************************************
 * @FunctionName   :      sRtcSetByCalendar
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  20:58:00
 * @Description    :      Rtc对外接口 之 通过传入日历更新设置RTC
 * @Input          :      pCalendar     传入要设置的日历
 * @Return         :      
*******************************************************************************/
bool sRtcSetByCalendar(stRtcCalendar_t* pCalendar)
{
    rtc_parameter_struct stRtcParameter;
    
    if(bRtcInitFlag != true)
    {
        return(false);
    }
    
    //配置 RTC 参数
    if(bRtcSrcFlag == true)
    {
        // LXTAL_CLK / 32768
        stRtcParameter.factor_asyn      = 0x7F;
        stRtcParameter.factor_syn       = 0xFF;
    }
    else if(bRtcSrcFlag == false)
    {
        // IRC32K_CLK / 32000
        stRtcParameter.factor_asyn      = 0x13F;
        stRtcParameter.factor_syn       = 0x63;
    }
    
    stRtcParameter.year             = ((pCalendar->u16Year + 1900) % 100  / 10) << 4 | ((pCalendar->u16Year + 1900) % 100  % 10);
    stRtcParameter.month            =  (pCalendar->u8Month                / 10) << 4 |  (pCalendar->u8Month                % 10);
    stRtcParameter.date             =  (pCalendar->u8Date                 / 10) << 4 |  (pCalendar->u8Date                 % 10);
    stRtcParameter.hour             =  (pCalendar->u8Hour                 / 10) << 4 |  (pCalendar->u8Hour                 % 10);
    stRtcParameter.minute           =  (pCalendar->u8Minute               / 10) << 4 |  (pCalendar->u8Minute               % 10);
    stRtcParameter.second           =  (pCalendar->u8Second               / 10) << 4 |  (pCalendar->u8Second               % 10);
    
    stRtcParameter.day_of_week      = pCalendar->u8DayOfWeek;
    stRtcParameter.am_pm            = RTC_AM;
    stRtcParameter.display_format   = RTC_24HOUR;
    
    if(xSemaphoreTake(xSemRtcQueue, 2000 / portTICK_RATE_MS) != pdTRUE)
    {
        return(false);
    }
    
    //等待同步
    if(ERROR == rtc_register_sync_wait())
    {
        EN_SLOGE(TAG, "等待同步超时! RTC 时钟更新失败!");
        return(false);
    }
    
    
    //初始化 RTC
    if(ERROR == rtc_init(&stRtcParameter))
    {
        EN_SLOGE(TAG, "RTC 时钟更新失败! 尝试rtc_deinit");
        
        rtc_deinit();
        
        if(ERROR == rtc_init(&stRtcParameter))
        {
            EN_SLOGE(TAG, "RTC 时钟更新失败!");
            return(false);
        }
    }
    else
    {
        EN_SLOGD(TAG, "RTC 时钟更新成功!");
        
        //显示当前时间
        //sRtcGetTimeStamp();
    }
    
    xSemaphoreGive(xSemRtcQueue);
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sRtcSetByTimeStamp
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:45
 * @Description    :      Rtc对外接口 之 通过传入时间戳更新设置RTC
 * @Input          :      u32TimeStamp  传入要设置的时间戳(格林威治时间)
 * @Return         :      
*******************************************************************************/
bool sRtcSetByTimeStamp(u32 u32TimeStamp)
{
    stRtcCalendar_t stRtcCalendar;
    
    if(bRtcInitFlag != true)
    {
        return(false);
    }
    
    //将时间戳转换为日历
    sTimeStampToDateTime(i8RtcTimeZone, u32TimeStamp, &stRtcCalendar);
    
    //传入日历更新设置RTC
    return(sRtcSetByCalendar(&stRtcCalendar));
}












