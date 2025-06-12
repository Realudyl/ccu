/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     rtc_drv.c
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
#include <stdio.h>
#include "rtc_drv.h"
#include "rtc_drv_interface.h"
#include "rtc_drv_proc.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "rtc_drv";





//RTC时钟源标志 ———— true : LXTAL  false : IRC32K
bool bRtcSrcFlag = false;

//RTC初始化标志 ———— true : 成功  false : 失败
bool bRtcInitFlag = false;


//全局RTC时区
i8 i8RtcTimeZone;

#if (cSdkRtcTimeZone == true)
extern i8 i8SdkRtcTimeZone;
#endif

// RTC对时保护信号量
SemaphoreHandle_t xSemRtcQueue = NULL;








void sRtcTimeZoneInit(void);


bool sRtcDrvInitDev(void);
bool sRtcDrvInitDevRcu(void);
bool sRtcDrvInitDevRtc(void);


bool sEnlogShellRtcConfig(const stShellPkt_t *pkg);
bool sEnlogShellTimeZoneConfig(const stShellPkt_t *pkg);









stShellCmd_t stSellCmdRtcConfigCmd = 
{
    .pCmd       = "setrtc",
    .pFormat    = "格式:setrtc YYYY-MM-DD hh:mm:ss",
    .pFunction  = "功能:设置RTC时间",
    .pRemarks   = "备注:参数值, YYYY---年, MM---月, DD---日, hh---时, mm---分, ss---秒",
    .pFunc      = sEnlogShellRtcConfig,
};




stShellCmd_t stSellCmdTimeZoneConfigCmd = 
{
    .pCmd       = "settimezone",
    .pFormat    = "格式:settimezone %d",
    .pFunction  = "功能:设置时区",
    .pRemarks   = "备注:参数值，东八区(8) 西八区(-8) 格林威治时区(0)",
    .pFunc      = sEnlogShellTimeZoneConfig,
};













/*******************************************************************************
 * @FunctionName   :      sRtcTimeZoneInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月1日  11:30:22
 * @Description    :      
 * @Input          :      void        
*******************************************************************************/
void sRtcTimeZoneInit(void)
{
    static u8 u8RtcTimeZoneInitFlag = 0;
    
    //配置覆盖
#if (cSdkRtcTimeZone == true)
    if(u8RtcTimeZoneInitFlag == 0)
    {
        u8RtcTimeZoneInitFlag = 1;
        i8RtcTimeZone = i8SdkRtcTimeZone;
    }
#else
    //设置默认RTC时区
    i8RtcTimeZone = cRtcTimeZoneDefault;
#endif
    
}








/*******************************************************************************
 * @FunctionName   :      sRtcDrvInitDev
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:32
 * @Description    :      Rtc驱动 设备初始化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sRtcDrvInitDev(void)
{
    //1:使能 RTC 寄存器写访问
    pmu_backup_write_enable();
    
    
    //2:关闭 RTC 篡改
    rtc_tamper_disable(RTC_TAMPER0);
    
    
    //3:时钟信号初始化
    sRtcDrvInitDevRcu();
    
    
    // 创建RTC对时保护信号量
    xSemRtcQueue = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemRtcQueue);
    
    
    //4:检查 RTC 是否已经配置(备份数据寄存器值不正确或尚未编程,或没有配置RTC时钟源)
    if((cRtcInitFinshedFlag != RTC_BKP0) || (0x00 == GET_BITS(RCU_BDCTL, 8, 9)))
    {
        //5.1:RTC 初始化
        bRtcInitFlag = sRtcDrvInitDevRtc();
    }
    else
    {
        bRtcInitFlag = true;
        
        //5.2:检查复位源
        if(RESET != rcu_flag_get(RCU_FLAG_BORRST))
        {
            //欠压复位
            EN_SLOGE(TAG, "Brown-out Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_EPRST))
        {
            //外部引脚复位
            EN_SLOGE(TAG, "External Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_PORRST))
        {
            //电源复位
            EN_SLOGE(TAG, "Power On Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_SWRST))
        {
            //软件复位
            EN_SLOGE(TAG, "Software Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_FWDGTRST))
        {
            //独立看门狗复位
            EN_SLOGE(TAG, "FWDGT Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_WWDGTRST))
        {
            //窗口看门狗复位
            EN_SLOGE(TAG, "WWDGT Reset Occurred....");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_LPRST))
        {
            //低电压复位
            EN_SLOGE(TAG, "Low-power Reset Occurred....");
        }
        
        EN_SLOGE(TAG, "RTC IS READY NOW....");
        
        //显示当前时间
        //sRtcTimeDisplay();
    }
    
    
    //6:清空所有复位标志
    rcu_all_reset_flag_clear();
    
    
    //7:初始化RTC时区
    sRtcTimeZoneInit();
    
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdRtcConfigCmd);
    sShellCmdRegister(&stSellCmdTimeZoneConfigCmd);
    
    return(true);
}






/*******************************************************************************
 * @FunctionName   :      sRtcDrvInitDevRcu
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:36
 * @Description    :      Rtc驱动 设备初始化 之 RCU使能
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sRtcDrvInitDevRcu(void)
{
    //使能电源管理单元时钟
    rcu_periph_clock_enable(RCU_PMU);
    
    rcu_lxtal_drive_capability_config(RCU_LXTAL_MED_LOWDRI);
    
    //开启外部低速振荡器
    rcu_osci_on(RCU_LXTAL);
    
    //等待外部低速振荡器稳定
    rcu_osci_stab_wait(RCU_LXTAL);
    
    //选择外部低速振荡器为RTC时钟源
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    
    //使能RTC时钟
    rcu_periph_clock_enable(RCU_RTC);
    
    //等待同步
    if(ERROR == rtc_register_sync_wait())
    {
        //等待同步超时，启用内部低速振荡器
        
        //开启外部低速振荡器
        rcu_osci_on(RCU_IRC32K);
        
        //等待外部低速振荡器稳定
        rcu_osci_stab_wait(RCU_IRC32K);
        
        //选择外部低速振荡器为RTC时钟源
        rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
        
        //等待同步
        if(ERROR == rtc_register_sync_wait())
        {
            return(false);
        }
        
        //RTC时钟源成功初始化为IRC32K
        bRtcSrcFlag = false;
    }
    
    //RTC时钟源成功初始化为LXTAL
    bRtcSrcFlag = true;
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sRtcDrvInitDevRtc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月4日  15:29:38
 * @Description    :      Rtc驱动 设备初始化 之 Rtc初始化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sRtcDrvInitDevRtc(void)
{
    rtc_parameter_struct stRtcParameter;
    
    //还原 RTC 配置
    rtc_deinit();
    
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
    
    stRtcParameter.year             = 0x24;
    stRtcParameter.month            = RTC_JAN;
    stRtcParameter.date             = 0x01;
    stRtcParameter.day_of_week      = RTC_MONDAY;
    stRtcParameter.hour             = 0;
    stRtcParameter.minute           = 0;
    stRtcParameter.second           = 0;
    
    stRtcParameter.am_pm            = RTC_AM;
    stRtcParameter.display_format   = RTC_24HOUR;
    
    //等待同步
    rtc_register_sync_wait();
    
    
    //初始化 RTC
    if(ERROR == rtc_init(&stRtcParameter))
    {
        EN_SLOGD(TAG, "RTC 时钟初始化失败!");
        return(false);
    }
    else
    {
        EN_SLOGD(TAG, "RTC 时钟初始化成功!");
        
        //显示当前时间
        //sRtcTimeDisplay();
        
        //向 RTC 备份寄存器写入初始化完成标志
        RTC_BKP0 = cRtcInitFinshedFlag;
    }
    
    
    return(true);
}











bool sEnlogShellRtcConfig(const stShellPkt_t *pkg)
{
    stRtcCalendar_t stRtcCalendar;
    
    
    if(0 == sscanf((const char *)pkg->para[0], "%d-%d-%d", (int*)&stRtcCalendar.u16Year, (int*)&stRtcCalendar.u8Month, (int*)&stRtcCalendar.u8Date))
    {
        return(false);
    }
    if(0 == sscanf((const char *)pkg->para[1], "%d:%d:%d", (int*)&stRtcCalendar.u8Hour, (int*)&stRtcCalendar.u8Minute, (int*)&stRtcCalendar.u8Second))
    {
        return(false);
    }
    
    
    stRtcCalendar.u8DayOfWeek = sGetWeekDay(stRtcCalendar.u16Year, stRtcCalendar.u8Month, stRtcCalendar.u8Date);
    
    if(false == sRtcSetByCalendar(&stRtcCalendar))
    {
        return(false);
    }
    
    EN_SLOGI(TAG, "设置RTC时间: %04hd年%02hd月%02hd日  星期%d  %02hd:%02hd:%02hd",
        stRtcCalendar.u16Year, stRtcCalendar.u8Month, stRtcCalendar.u8Date, stRtcCalendar.u8DayOfWeek, stRtcCalendar.u8Hour, stRtcCalendar.u8Minute, stRtcCalendar.u8Second);
    
    return(true);
}








bool sEnlogShellTimeZoneConfig(const stShellPkt_t *pkg)
{
    i8 i8TimeZone;
    
    if(0 == sscanf((const char *)pkg->para[0], "%d", (int*)&i8TimeZone))
    {
        return(false);
    }
    
    if(false == sRtcSetTimeZone(i8TimeZone))
    {
        return(false);
    }
    
    EN_SLOGI(TAG, "当前RTC时区为:  (%d)区      将时区重新设置为:  (%d)区  注:  东(+)  西(-)  格林威治(0)", i8RtcTimeZone, i8TimeZone);
    
    return(true);
}












