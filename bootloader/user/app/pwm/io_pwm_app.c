/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   io_pwm_app.c
* Description                           :   pwm用户程序
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-09
* notice                                :   
****************************************************************************************************/
#include "io_pwm_app.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_pwm_app";









stIoPwmAppCache_t stIoPwmAppCache;





bool  sIoPwmAppInit(void);

//对外的api接口函数---基于枪序号操作
eIoPwm_t sIoPwmGetCp(ePileGunIndex_t eGunIndex);
void sIoPwmSetCpOnOff(ePileGunIndex_t eGunIndex, bool bOnOff);


bool sIoPwmSetFanDuty(u32 u32Duty);










/***************************************************************************************************
* Description                           :   PWM资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-09
* notice                                :   
****************************************************************************************************/
bool sIoPwmAppInit(void)
{
    memset(&stIoPwmAppCache, 0, sizeof(stIoPwmAppCache));
    stIoPwmAppCache.eIoPwmCp[ePileGunIndex0]    = eIoPwm_CP_DR_A;
    stIoPwmAppCache.eIoPwmCp[ePileGunIndex1]    = eIoPwm_CP_DR_B;
    
    return sIoPwmDrvInitDev();
}









/***************************************************************************************************
* Description                           :   PWM APP层 api接口函数:获取PWM通道序号 之 CP振荡器
* Author                                :   Hall
* Creat Date                            :   2024-05-09
* notice                                :   
****************************************************************************************************/
eIoPwm_t sIoPwmGetCp(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoPwmMax : stIoPwmAppCache.eIoPwmCp[eGunIndex]);
}






/*******************************************************************************
 * @FunctionName   :      sIoPwmSetCpOnOff
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月21日  11:10:07
 * @Description    :      PWM APP层 api接口函数:设置 CP振荡器 开关
 * @Input          :      eGunIndex   
 * @Input          :      bOnOff      
 * @Return         :      
*******************************************************************************/
void sIoPwmSetCpOnOff(ePileGunIndex_t eGunIndex, bool bOnOff)
{
    sIoPwmDrvSetPwm(sIoPwmGetCp(eGunIndex), bOnOff);
}








/*******************************************************************************
 * @FunctionName   :      sIoPwmSetFanDuty
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年10月17日  14:05:30
 * @Description    :      PWM APP层 api接口函数:设置 散热风扇 PWM占空比
 * @Input          :      u32Duty   
 * @Return         :      
*******************************************************************************/
bool sIoPwmSetFanDuty(u32 u32Duty)
{
    static bool bFanState = false;
    
    if((bFanState == false) && (u32Duty > 0))
    {
        bFanState = true;
        sIoPwmDrvSetPwm(eIoPwm_FAN, bFanState);
    }
    else if((bFanState == true) && (u32Duty == 0))
    {
        bFanState = false;
        sIoPwmDrvSetPwm(eIoPwm_FAN, bFanState);
    }
    
    return sIoPwmDrvSetPwmDuty(eIoPwm_FAN, u32Duty);
}


