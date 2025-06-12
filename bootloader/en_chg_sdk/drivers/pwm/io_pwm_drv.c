/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   io_pwm_drv.c
* Description                           :   PWM驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-31
* notice                                :   
****************************************************************************************************/
#include "io_pwm_drv.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_pwm_drv";






//RGB DMA缓存区
//此缓存区的起始地址和长度都必须保证是32字节的整数倍---注意起始地址
//所以不能使用MALLOC动态分配 只能按如下格式定义，
__attribute__((aligned(32))) u16 u8PwmDmaBuf[cIoPwmDrvDmaChanNum][cIoPwmDmaBufSize];








extern stIoPwmDrvDevice_t stIoPwmDrvDevMap[cSdkIoPwmDrvPwmTimerNum];







bool sIoPwmDrvInitDev(void);

void sIoPwmDrvInitDevRcu(i32 i32IoPwmIndex);
void sIoPwmDrvInitDevGpio(i32 i32IoPwmIndex);
void sIoPwmDrvInitDevDma(i32 i32IoPwmIndex);
void sIoPwmDrvInitDevTimer(i32 i32IoPwmIndex);














/**********************************************************************************************
* Description                           :   PWM驱动 外设初始化
* Author                                :   Hall
* modified Date                         :   2024-05-09
* notice                                :   
***********************************************************************************************/
bool sIoPwmDrvInitDev(void)
{
    i8 i, j, k = 0;
    
    
    
    //初始化
    for(i = 0; i < sArraySize(stIoPwmDrvDevMap); i++)
    {
        if(stIoPwmDrvDevMap[i].ePwmAndPwmMode == ePwmIndependent)
        {
            for(j = 0; j < stIoPwmDrvDevMap[i].stPwmIndependent.stTimer.i8ChanUsed; j++)
            {
                if(stIoPwmDrvDevMap[i].stPwmIndependent.stTimer.stChan[j].stDma.i32Periph >= 0)
                {
                    stIoPwmDrvDevMap[i].stPwmIndependent.stTimer.stChan[j].stDma.i32DmaBufSize = cIoPwmDmaBufSize;
                    if(k < cIoPwmDrvDmaChanNum)
                    {
                        stIoPwmDrvDevMap[i].stPwmIndependent.stTimer.stChan[j].stDma.pDmaBuf   = u8PwmDmaBuf[k];
                        k++;
                    }
                    else
                    {
                        EN_SLOGE(TAG, "PWM %s 初始化出错! 超出定义的DMA Buff数组个数! 无法正确初始化!", stIoPwmDrvDevMap[i].stPwmIndependent.stTimer.stChan[j].pString);
                    }
                }
            }
        }
        else if(stIoPwmDrvDevMap[i].ePwmAndPwmMode == ePwmParallelSynchro)
        {
            for(j = 0; j < stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.i8ChanUsed; j++)
            {
                if(stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.stChan[j].stDma.i32Periph >= 0)
                {
                    stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.stChan[j].stDma.i32DmaBufSize = cIoPwmDmaBufSize;
                    if(k < cIoPwmDrvDmaChanNum)
                    {
                        stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.stChan[j].stDma.pDmaBuf   = u8PwmDmaBuf[k];
                        k++;
                    }
                    else
                    {
                        EN_SLOGE(TAG, "PWM %s 初始化出错! 超出定义的DMA Buff数组个数! 无法正确初始化!", stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.stChan[j].pString);
                    }
                }
            }
        }
        
        sIoPwmDrvInitDevRcu(i);
        sIoPwmDrvInitDevGpio(i);
        sIoPwmDrvInitDevDma(i);
        sIoPwmDrvInitDevTimer(i);
    }
    
    
    for(i= 0; i < sArraySize(stIoPwmDrvDevMap); i++)
    {
        //并联同步需要主从按次序初始化好后才能使能定时器
        if((stIoPwmDrvDevMap[i].ePwmAndPwmMode == ePwmParallelSynchro)
        && (stIoPwmDrvDevMap[i].stPwmParallelSynchro.eRelation == eParallelMaster))
        {
            /* TIMERX counter enable */
            timer_enable(stIoPwmDrvDevMap[i].stPwmParallelSynchro.stTimer.i32Periph);
        }
    }
    
    return true;
}








/**********************************************************************************************
* Description                           :   PWM驱动 时钟信号初始化
* Author                                :   Hall
* modified Date                         :   2024-05-09
* notice                                :   
***********************************************************************************************/
void sIoPwmDrvInitDevRcu(i32 i32IoPwmIndex)
{
    i8  i;
    stIoPwmDrvDevice_t *pDevice = &stIoPwmDrvDevMap[i32IoPwmIndex];
    
    
    if(pDevice->ePwmAndPwmMode == ePwmIndependent)
    {
        //触发用定时器外设时钟、定时器DMA时钟及发波GPIO时钟
        if(pDevice->stPwmIndependent.stTimer.i32PeriphRcu >= 0)
        {
            rcu_periph_clock_enable(pDevice->stPwmIndependent.stTimer.i32PeriphRcu);
        }
        for(i = 0; i < pDevice->stPwmIndependent.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32PeriphRcu >= 0)
            {
                rcu_periph_clock_enable(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32PeriphRcu);
                rcu_periph_clock_enable(RCU_DMAMUX);
            }
            if(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32PeriphRcu >= 0)
            {
                rcu_periph_clock_enable(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32PeriphRcu);
            }
        }
    }
    else if(pDevice->ePwmAndPwmMode == ePwmParallelSynchro)
    {
        //触发用定时器外设时钟、定时器DMA时钟及发波GPIO时钟
        if(pDevice->stPwmParallelSynchro.stTimer.i32PeriphRcu >= 0)
        {
            rcu_periph_clock_enable(pDevice->stPwmParallelSynchro.stTimer.i32PeriphRcu);
        }
        for(i = 0; i < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32PeriphRcu >= 0)
            {
                rcu_periph_clock_enable(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32PeriphRcu);
                rcu_periph_clock_enable(RCU_DMAMUX);
            }
            if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32PeriphRcu >= 0)
            {
                rcu_periph_clock_enable(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32PeriphRcu);
            }
        }
        rcu_periph_clock_enable(RCU_SYSCFG);
        rcu_periph_clock_enable(RCU_TRIGSEL);
    }
}







/**********************************************************************************************
* Description                           :   PWM驱动 GPIO初始化
* Author                                :   Hall
* modified Date                         :   2024-05-09
* notice                                :   
***********************************************************************************************/
void sIoPwmDrvInitDevGpio(i32 i32IoPwmIndex)
{
    i8  i;
    stIoPwmDrvDevice_t *pDevice = &stIoPwmDrvDevMap[i32IoPwmIndex];
    
    
    if(pDevice->ePwmAndPwmMode == ePwmIndependent)
    {
        for(i = 0; i < pDevice->stPwmIndependent.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32PeriphRcu >= 0)
            {
                //设置为 GPIO_MODE_OUTPUT 并关闭发波
                gpio_mode_set(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Periph,
                              GPIO_MODE_OUTPUT,
                              pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Pull,
                              pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Pin);
                
                gpio_output_options_set(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Periph,
                                        GPIO_OTYPE_PP,
                                        pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Speed,
                                        pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Pin);
                
                pDevice->stPwmIndependent.stTimer.stChan[i].pCbClose(pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Periph,
                                                                     pDevice->stPwmIndependent.stTimer.stChan[i].stGpio.i32Pin);
            }
        }
    }
    else if(pDevice->ePwmAndPwmMode == ePwmParallelSynchro)
    {
        for(i = 0; i < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32PeriphRcu >= 0)
            {
                //设置为 GPIO_MODE_OUTPUT 并关闭发波
                gpio_mode_set(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Periph,
                GPIO_MODE_OUTPUT,
                pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Pull,
                pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Pin);
                
                gpio_output_options_set(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Periph,
                                        GPIO_OTYPE_PP,
                                        pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Speed,
                                        pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Pin);
                
                pDevice->stPwmParallelSynchro.stTimer.stChan[i].pCbClose(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Periph,
                                                                         pDevice->stPwmParallelSynchro.stTimer.stChan[i].stGpio.i32Pin);
            }
        }
    }
}





/*******************************************************************************
 * @FunctionName   :      sIoPwmDrvInitDevDma
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月15日  19:53:25
 * @Description    :      PWM驱动 DMA初始化
 * @Input          :      i32IoPwmIndex
*******************************************************************************/
void sIoPwmDrvInitDevDma(i32 i32IoPwmIndex)
{
    i8  i;
    stIoPwmDrvDevice_t *pDevice = &stIoPwmDrvDevMap[i32IoPwmIndex];
    dma_single_data_parameter_struct dma_single_data_parameter;
    
    
    if(pDevice->ePwmAndPwmMode == ePwmIndependent)
    {
        for(i = 0; i < pDevice->stPwmIndependent.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32PeriphRcu >= 0)
            {
                /* ADC DMA_channel configuration */
                dma_deinit(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Chan);
                
                /* initialize DMA single data mode */
                dma_single_data_parameter.request               = pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Req;
                dma_single_data_parameter.periph_addr           = pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32PeriphAddr;
                dma_single_data_parameter.periph_inc            = DMA_PERIPH_INCREASE_DISABLE;
                dma_single_data_parameter.memory0_addr          = (uint32_t)(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.pDmaBuf);
                dma_single_data_parameter.memory_inc            = DMA_MEMORY_INCREASE_ENABLE;
                dma_single_data_parameter.periph_memory_width   = DMA_PERIPH_WIDTH_16BIT;
                dma_single_data_parameter.direction             = DMA_MEMORY_TO_PERIPH;
                dma_single_data_parameter.number                = pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32DmaBufSize;
                dma_single_data_parameter.priority              = DMA_PRIORITY_ULTRA_HIGH;
                dma_single_data_mode_init(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Chan, &dma_single_data_parameter);
                
                SCB_CleanDCache_by_Addr((u32 *)pDevice->stPwmIndependent.stTimer.stChan[i].stDma.pDmaBuf, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32DmaBufSize);
                
                /* enable DMA circulation mode */
                dma_circulation_enable(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Chan);
                
                /* enable DMA channel */
                dma_channel_enable(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Chan);
            }
        }
    }
    else if(pDevice->ePwmAndPwmMode == ePwmParallelSynchro)
    {
        for(i = 0; i < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; i++)
        {
            if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32PeriphRcu >= 0)
            {
                /* ADC DMA_channel configuration */
                dma_deinit(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Chan);
                
                /* initialize DMA single data mode */
                dma_single_data_parameter.request               = pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Req;
                dma_single_data_parameter.periph_addr           = pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32PeriphAddr;
                dma_single_data_parameter.periph_inc            = DMA_PERIPH_INCREASE_DISABLE;
                dma_single_data_parameter.memory0_addr          = (uint32_t)(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.pDmaBuf);
                dma_single_data_parameter.memory_inc            = DMA_MEMORY_INCREASE_ENABLE;
                dma_single_data_parameter.periph_memory_width   = DMA_PERIPH_WIDTH_16BIT;
                dma_single_data_parameter.direction             = DMA_MEMORY_TO_PERIPH;
                dma_single_data_parameter.number                = pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32DmaBufSize;
                dma_single_data_parameter.priority              = DMA_PRIORITY_ULTRA_HIGH;
                dma_single_data_mode_init(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Chan, &dma_single_data_parameter);
                
                SCB_CleanDCache_by_Addr((u32 *)pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.pDmaBuf, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32DmaBufSize);
                
                /* enable DMA circulation mode */
                dma_circulation_enable(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Chan);
                
                /* enable DMA channel */
                dma_channel_enable(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Chan);
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   PWM驱动 timer初始化
* Author                                :   Hall
* modified Date                         :   2024-05-09
* notice                                :   
***********************************************************************************************/
void sIoPwmDrvInitDevTimer(i32 i32IoPwmIndex)
{
    i8  i;
    stIoPwmDrvDevice_t *pDevice = &stIoPwmDrvDevMap[i32IoPwmIndex];
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    
    
    if(pDevice->ePwmAndPwmMode == ePwmIndependent)
    {
        if(pDevice->stPwmIndependent.stTimer.i32PeriphRcu >= 0)
        {
            timer_deinit(pDevice->stPwmIndependent.stTimer.i32Periph);
            
            /* TIMERx configuration */
            timer_struct_para_init(&timer_initpara);
            timer_initpara.prescaler         = pDevice->stPwmIndependent.stTimer.i32Psc;
            timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
            timer_initpara.counterdirection  = TIMER_COUNTER_UP;
            timer_initpara.period            = pDevice->stPwmIndependent.stTimer.i32Period;
            timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
            timer_initpara.repetitioncounter = 0;
            timer_init(pDevice->stPwmIndependent.stTimer.i32Periph, &timer_initpara);
            
            
            timer_channel_output_struct_para_init(&timer_ocintpara);
            /* CHx configuration in PWM mode0 */
            timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
            timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
            timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
            timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
            timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
            timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
            
            for(i = 0; i < pDevice->stPwmIndependent.stTimer.i8ChanUsed; i++)
            {
                if(pDevice->stPwmIndependent.stTimer.stChan[i].i32Chan >= 0)
                {
                    timer_channel_output_config(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].i32Chan, &timer_ocintpara);
                    
                    //初始比较值先给 i32Period/2 确保外设内部可以发波
                    //以便能产生触发信号
                    timer_channel_output_pulse_value_config(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].i32Chan, pDevice->stPwmIndependent.stTimer.i32Period / 2);
                    timer_channel_output_mode_config(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].i32Chan, TIMER_OC_MODE_PWM0);
                    timer_channel_output_shadow_config(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].i32Chan, TIMER_OC_SHADOW_ENABLE);
                }
                
                if(pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32Periph >= 0)
                {
                    /* TIMERX update DMA request enable */
                    timer_dma_transfer_config(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32AccessBaseAddr, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32TransTime);
                    timer_dma_enable(pDevice->stPwmIndependent.stTimer.i32Periph, pDevice->stPwmIndependent.stTimer.stChan[i].stDma.i32UpdateSource);
                }
            }
            
            
            /* TIMERX primary output enable */
            timer_primary_output_config(pDevice->stPwmIndependent.stTimer.i32Periph, ENABLE);
            
            /* auto-reload preload enable */
            timer_auto_reload_shadow_enable(pDevice->stPwmIndependent.stTimer.i32Periph);
            
            /* TIMERX counter enable */
            timer_enable(pDevice->stPwmIndependent.stTimer.i32Periph);
        }
    }
    else if(pDevice->ePwmAndPwmMode == ePwmParallelSynchro)
    {
        /* select TriggerSource to trigger TargetPeriph */
        trigsel_init(pDevice->stPwmParallelSynchro.stTrigsel.i32TargetPeriph, pDevice->stPwmParallelSynchro.stTrigsel.i32TriggerSource);
        
        
        if(pDevice->stPwmParallelSynchro.stTimer.i32PeriphRcu >= 0)
        {
            timer_deinit(pDevice->stPwmParallelSynchro.stTimer.i32Periph);
            
            /* TIMERx configuration */
            timer_struct_para_init(&timer_initpara);
            timer_initpara.prescaler         = pDevice->stPwmParallelSynchro.stTimer.i32Psc;
            timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
            timer_initpara.counterdirection  = TIMER_COUNTER_UP;
            timer_initpara.period            = pDevice->stPwmParallelSynchro.stTimer.i32Period;
            timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
            timer_initpara.repetitioncounter = 0;
            timer_init(pDevice->stPwmParallelSynchro.stTimer.i32Periph, &timer_initpara);
            
            
            timer_channel_output_struct_para_init(&timer_ocintpara);
            /* CHx configuration in PWM mode0 */
            timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
            timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
            timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
            timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
            timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
            timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
            
            for(i = 0; i < pDevice->stPwmParallelSynchro.stTimer.i8ChanUsed; i++)
            {
                if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].i32Chan >= 0)
                {
                    timer_channel_output_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].i32Chan, &timer_ocintpara);
                    
                    //初始比较值先给 i32Period/2 确保外设内部可以发波
                    //以便能产生触发信号
                    timer_channel_output_pulse_value_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].i32Chan, pDevice->stPwmParallelSynchro.stTimer.i32Period / 2);
                    timer_channel_output_mode_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].i32Chan, TIMER_OC_MODE_PWM0);
                    timer_channel_output_shadow_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].i32Chan, TIMER_OC_SHADOW_ENABLE);
                }
                
                if(pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32Periph >= 0)
                {
                    /* TIMERX update DMA request enable */
                    timer_dma_transfer_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32AccessBaseAddr, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32TransTime);
                    timer_dma_enable(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTimer.stChan[i].stDma.i32UpdateSource);
                }
            }
            
            
            /* select the master slave mode */
            timer_master_slave_mode_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, TIMER_MASTER_SLAVE_MODE_ENABLE);
            
            if((pDevice->stPwmParallelSynchro.eRelation == eParallelMaster)
            && (pDevice->stPwmParallelSynchro.stTrigsel.i32MasterTrigger >= 0))
            {
                /* TIMERX update event is used as trigger output */
                timer_master_output0_trigger_source_select(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTrigsel.i32MasterTrigger);
            }
            else if((pDevice->stPwmParallelSynchro.eRelation == eParallelSlave)
            && (pDevice->stPwmParallelSynchro.stTrigsel.i32SlaveTrigger >= 0)
            && (pDevice->stPwmParallelSynchro.stTrigsel.i32SlaveMode >= 0))
            {
                /* slave mode selection: external clock mode 0 */
                timer_slave_mode_select(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTrigsel.i32SlaveMode);
                
                /* select TIMER input trigger source: internal trigger 1(ITI1) */
                timer_input_trigger_source_select(pDevice->stPwmParallelSynchro.stTimer.i32Periph, pDevice->stPwmParallelSynchro.stTrigsel.i32SlaveTrigger);
            }
            
            /* TIMERX primary output enable */
            timer_primary_output_config(pDevice->stPwmParallelSynchro.stTimer.i32Periph, ENABLE);
            
            /* auto-reload preload enable */
            timer_auto_reload_shadow_enable(pDevice->stPwmParallelSynchro.stTimer.i32Periph);
        }
    }
}











