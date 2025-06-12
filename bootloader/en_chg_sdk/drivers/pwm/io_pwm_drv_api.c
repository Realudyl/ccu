/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   io_pwm_drv_api.c
* Description                           :   PWM驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-31
* notice                                :   
****************************************************************************************************/
#include "io_pwm_drv_api.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_pwm_drv_api";






extern stIoPwmDrvDevice_t stIoPwmDrvDevMap[cSdkIoPwmDrvPwmTimerNum];






bool sIoPwmDrvSetPwm(i32 i32IoPwmIndex, bool bOnFlag);
bool sIoPwmDrvSetPwmDuty(i32 i32IoPwmIndex, u32 u32Duty);

bool sIoPwmDrvSendData(i32 i32IoPwmIndex, u16 *pData, u32 u32DataByte);








/**********************************************************************************************
* Description                           :   PWM驱动 PWM开/关
* Author                                :   Hall
* modified Date                         :   2024-01-02
* notice                                :   
***********************************************************************************************/
bool sIoPwmDrvSetPwm(i32 i32IoPwmIndex, bool bOnFlag)
{
    i8 i, j;
    stIoPwmDrvDevice_t      *pDevice  = NULL;
    stIoPwmDrvCfgTimerCh_t  *pChannel = NULL;
    
    
    for(i = 0; i < sArraySize(stIoPwmDrvDevMap); i++)
    {
        pDevice = &stIoPwmDrvDevMap[i];
        
        if((pDevice->ePwmAndPwmMode == ePwmIndependent) && (pDevice->stPwmIndependent.stTimer.i32Periph >= 0))
        {
            for(j = 0; j < pDevice->stPwmIndependent.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmIndependent.stTimer.stChan[j];
                
                if(pChannel->i8IoPwm == i32IoPwmIndex)
                {
                    if(bOnFlag == true)
                    {
                        EN_SLOGI(TAG, "%s开启输出", pChannel->pString);
                        gpio_af_set(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Af, pChannel->stGpio.i32Pin);
                        gpio_mode_set(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Mode, pChannel->stGpio.i32Pull, pChannel->stGpio.i32Pin);
                        gpio_output_options_set(pChannel->stGpio.i32Periph, GPIO_OTYPE_PP, pChannel->stGpio.i32Speed, pChannel->stGpio.i32Pin);
                    }
                    else
                    {
                        EN_SLOGI(TAG, "%s关闭输出", pChannel->pString);
                        gpio_mode_set(pChannel->stGpio.i32Periph, GPIO_MODE_OUTPUT, pChannel->stGpio.i32Pull, pChannel->stGpio.i32Pin);
                        gpio_output_options_set(pChannel->stGpio.i32Periph, GPIO_OTYPE_PP, pChannel->stGpio.i32Speed, pChannel->stGpio.i32Pin);
                        pChannel->pCbClose(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Pin);
                    }
                }
            }
        }
        else if((pDevice->ePwmAndPwmMode == ePwmParallelSynchro) && (pDevice->stPwmParallelSynchro.stTimer.i32Periph >= 0))
        {
            for(j = 0; j < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmParallelSynchro.stTimer.stChan[j];
                
                if(pChannel->i8IoPwm == i32IoPwmIndex)
                {
                    if(bOnFlag == true)
                    {
                        EN_SLOGI(TAG, "%s开启输出", pChannel->pString);
                        gpio_af_set(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Af, pChannel->stGpio.i32Pin);
                        gpio_mode_set(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Mode, pChannel->stGpio.i32Pull, pChannel->stGpio.i32Pin);
                        gpio_output_options_set(pChannel->stGpio.i32Periph, GPIO_OTYPE_PP, pChannel->stGpio.i32Speed, pChannel->stGpio.i32Pin);
                    }
                    else
                    {
                        EN_SLOGI(TAG, "%s关闭输出", pChannel->pString);
                        gpio_mode_set(pChannel->stGpio.i32Periph, GPIO_MODE_OUTPUT, pChannel->stGpio.i32Pull, pChannel->stGpio.i32Pin);
                        gpio_output_options_set(pChannel->stGpio.i32Periph, GPIO_OTYPE_PP, pChannel->stGpio.i32Speed,pChannel->stGpio.i32Pin);
                        pChannel->pCbClose(pChannel->stGpio.i32Periph, pChannel->stGpio.i32Pin);
                    }
                }
            }
        }
    }
    
    return(true);
}










/**********************************************************************************************
* Description                           :   PWM驱动 PWM信号占空比设定
* Author                                :   Hall
* modified Date                         :   2024-01-02
* notice                                :   
***********************************************************************************************/
bool sIoPwmDrvSetPwmDuty(i32 i32IoPwmIndex, u32 u32Duty)
{
    i8 i, j;
    i32  i32CompareCnt;
    stIoPwmDrvDevice_t      *pDevice  = NULL;
    stIoPwmDrvCfgTimerCh_t  *pChannel = NULL;
    
    
    if(u32Duty > 100)
    {
        u32Duty = 100;
    }
    
    
    for(i = 0; i < sArraySize(stIoPwmDrvDevMap); i++)
    {
        pDevice = &stIoPwmDrvDevMap[i];
        
        if((pDevice->ePwmAndPwmMode == ePwmIndependent) && (pDevice->stPwmIndependent.stTimer.i32Periph >= 0))
        {
            for(j = 0; j < pDevice->stPwmIndependent.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmIndependent.stTimer.stChan[j];
                
                if(pChannel->i8IoPwm == i32IoPwmIndex)
                {
                    i32CompareCnt = (i32)(((((u32)pDevice->stPwmIndependent.stTimer.i32Period) + 1) / 100) * u32Duty);
                    i32CompareCnt = i32CompareCnt == 0 ? 0 : (i32CompareCnt - 1);
                    timer_channel_output_pulse_value_config(pDevice->stPwmIndependent.stTimer.i32Periph, pChannel->i32Chan, i32CompareCnt);
                }
            }
        }
        else if((pDevice->ePwmAndPwmMode == ePwmParallelSynchro) && (pDevice->stPwmParallelSynchro.stTimer.i32Periph >= 0))
        {
            for(j = 0; j < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmParallelSynchro.stTimer.stChan[j];
                
                if(pChannel->i8IoPwm == i32IoPwmIndex)
                {
                    i32CompareCnt = (i32)(((((u32)pDevice->stPwmParallelSynchro.stTimer.i32Period) + 1) / 100) * u32Duty);
                    i32CompareCnt = i32CompareCnt == 0 ? 0 : (i32CompareCnt - 1);
                    timer_channel_output_pulse_value_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pChannel->i32Chan, i32CompareCnt);
                }
            }
        }
    }
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sIoPwmDrvSendData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月21日  19:24:56
 * @Description    :      PWM驱动 DMA数据发送
 * @Input          :      i32IoPwmIndex     PWM外设号
 * @Input          :      pData             要发送的数据
 * @Input          :      u32DataByte       数据字节数
 * @Return         :      
*******************************************************************************/
bool sIoPwmDrvSendData(i32 i32IoPwmIndex, u16 *pData, u32 u32DataByte)
{
    i8 i, j;
    stIoPwmDrvDevice_t      *pDevice  = NULL;
    stIoPwmDrvCfgTimerCh_t  *pChannel = NULL;
    
    
    for(i = 0; i < sArraySize(stIoPwmDrvDevMap); i++)
    {
        pDevice = &stIoPwmDrvDevMap[i];
    
        if(pDevice->ePwmAndPwmMode == ePwmIndependent)
        {
            for(j = 0; j < pDevice->stPwmIndependent.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmIndependent.stTimer.stChan[j];
                
                if((pChannel->i8IoPwm == i32IoPwmIndex) && (pChannel->stDma.i32Periph >= 0))
                {
                    SCB_CleanInvalidateDCache_by_Addr((u32 *)pChannel->stDma.pDmaBuf, pChannel->stDma.i32DmaBufSize);
                    memcpy(pChannel->stDma.pDmaBuf, pData, u32DataByte);
                }
            }
        }
        else if(pDevice->ePwmAndPwmMode == ePwmParallelSynchro)
        {
            for(j = 0; j < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; j++)
            {
                pChannel = &pDevice->stPwmParallelSynchro.stTimer.stChan[j];
                
                if((pChannel->i8IoPwm == i32IoPwmIndex) && (pChannel->stDma.i32Periph >= 0))
                {
                    SCB_CleanInvalidateDCache_by_Addr((u32 *)pChannel->stDma.pDmaBuf, pChannel->stDma.i32DmaBufSize);
                    memcpy(pChannel->stDma.pDmaBuf, pData, u32DataByte);
                }
            }
        }
    }
    
    return(true);
}











