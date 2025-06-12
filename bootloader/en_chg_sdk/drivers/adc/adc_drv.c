/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   adc_drv.c
* Description                           :   ADC驱动 硬件配置
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#include <string.h>
#include "en_log.h"
#include "en_mem.h"

#include "adc_drv.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "adc_drv";












//ADC外设运行规则采样时，需要DMA缓存区
//此缓存区的起始地址和长度都必须保证是32字节的整数倍，所以按如下格式定义
//不管实际配置几个通道，直接按照最大通道数，浪费不了多少ram
__attribute__((aligned(32))) i32        u8AdcDmaBuf[eAdcNumMax][cAdcDrvMaxChan * sizeof(i32)];


extern stAdcDrvDevice_t stAdcDrvDevMap[eAdcNumMax];







bool sAdcDrvInitDev(eAdcNum_t eAdcNum);
void sAdcDrvInitDevRcu(eAdcNum_t eAdcNum);
void sAdcDrvInitDevGpio(eAdcNum_t eAdcNum);
void sAdcDrvInitDevNvic(eAdcNum_t eAdcNum);
void sAdcDrvInitDevDma(eAdcNum_t eAdcNum);
void sAdcDrvInitDevTimer(eAdcNum_t eAdcNum);
void sAdcDrvInitDevAdc(eAdcNum_t eAdcNum);





















/**********************************************************************************************
* Description       :     ADC驱动  初始化
* Author            :     Hall
* modified Date     :     2023-11-22
* notice            :     
***********************************************************************************************/
bool sAdcDrvInitDev(eAdcNum_t eAdcNum)
{
    stAdcDrvDevice_t *pDevice = NULL;
    
    if(eAdcNum >= eAdcNumMax)
    {
        //ADC外设超限
        return(false);
    }
    
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    if(pDevice->i32PeriphRcuAdc < 0)
    {
        //未配置(未启用)的AD外设 不执行初始化 直接返回true
        return(true);
    }
    
    if(pDevice->stDma.i32PeriphRcu >= 0)
    {
        //DMA管理时才需要分配buf
        pDevice->pBuf = u8AdcDmaBuf[eAdcNum];
        pDevice->i32Size = cAdcDrvMaxChan * sizeof(i32);
        if(pDevice->pBuf == NULL)
        {
            return(false);
        }
    }
    sAdcDrvInitDevRcu(eAdcNum);
    sAdcDrvInitDevGpio(eAdcNum);
    sAdcDrvInitDevNvic(eAdcNum);
    sAdcDrvInitDevDma(eAdcNum);
    sAdcDrvInitDevAdc(eAdcNum);
    sAdcDrvInitDevTimer(eAdcNum);
    
    return(true);
}








/**********************************************************************************************
* Description                           :   ADC驱动 时钟信号初始化
* Author                                :   Hall
* modified Date                         :   2023-11-20
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevRcu(eAdcNum_t eAdcNum)
{
    i32  j;
    stAdcDrvDevice_t *pDevice = NULL;
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    
    //ADC外设时钟
    if(pDevice->i32PeriphRcuAdc >= 0)
    {
        if(pDevice->i32PeriphRcuAdc == RCU_ADC1)
        {
            //ADC1 外设与 ADC0 外设用同一个时钟信号 RCU_ADC0
            //因此使能 RCU_ADC1 时也要使能 RCU_ADC0
            rcu_periph_clock_enable(RCU_ADC0);
        }
        rcu_periph_clock_enable(pDevice->i32PeriphRcuAdc);
    }
    
    
    //规则组采样用的触发定时器及其控制器
    if(pDevice->stTimer.i32PeriphRcu >= 0)
    {
        rcu_periph_clock_enable(pDevice->stTimer.i32PeriphRcu);
    }
    if(pDevice->i32PeriphRcuTrigsel >= 0)
    {
        rcu_periph_clock_enable(pDevice->i32PeriphRcuTrigsel);
    }
    
    
    //DMA时钟及DMAMUX时钟
    if(pDevice->stDma.i32PeriphRcu >= 0)
    {
        rcu_periph_clock_enable(pDevice->stDma.i32PeriphRcu);
        rcu_periph_clock_enable(RCU_DMAMUX);
    }
    
    
    //GPIO时钟
    for(j = 0; j < pDevice->i32ChanNum; j++)
    {
        rcu_periph_clock_enable(pDevice->stCfgChan[j].i32PeriphRcu);
    }
    
}







/**********************************************************************************************
* Description                           :   ADC驱动 GPIO初始化
* Author                                :   Hall
* modified Date                         :   2023-11-20
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevGpio(eAdcNum_t eAdcNum)
{
    i32  j;
    stAdcDrvDevice_t *pDevice = NULL;
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    for(j = 0; j < pDevice->i32ChanNum; j++)
    {
        gpio_mode_set(pDevice->stCfgChan[j].i32Periph, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pDevice->stCfgChan[j].i32Pin);
    }
    
}







/**********************************************************************************************
* Description                           :   ADC驱动 中断配置
* Author                                :   Hall
* modified Date                         :   2023-11-24
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevNvic(eAdcNum_t eAdcNum)
{
    stAdcDrvDevice_t *pDevice = NULL;
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    if(pDevice->i32Irq >= 0)
    {
        //可设置范围：抢占优先级0~3,响应优先级0~3
        nvic_irq_enable(pDevice->i32Irq, 2U, 1U);
    }
}









/**********************************************************************************************
* Description                           :   ADC驱动 DMA初始化
* Author                                :   Hall
* modified Date                         :   2023-11-20
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevDma(eAdcNum_t eAdcNum)
{
    i32  j;
    i32  i32RegNum   = 0;
    stAdcDrvDevice_t *pDevice = NULL;
    dma_single_data_parameter_struct dma_single_data_parameter;
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    for(j = 0; j < pDevice->i32ChanNum; j++)
    {
        //提取规则采样路数
        if(pDevice->stCfgChan[j].eMode == eAdcDrvSampleModeReg)
        {
            i32RegNum++;
        }
    }
    
    if(pDevice->stDma.i32PeriphRcu >= 0)
    {
        /* ADC DMA_channel configuration */
        dma_deinit(pDevice->stDma.i32Periph, pDevice->stDma.i32Chan);
        
        /* initialize DMA single data mode */
        dma_single_data_parameter.request               = pDevice->stDma.i32Req;
        dma_single_data_parameter.periph_addr           = (uint32_t)(&ADC_RDATA(pDevice->i32PeriphAdc));
        dma_single_data_parameter.periph_inc            = DMA_PERIPH_INCREASE_DISABLE;
        dma_single_data_parameter.memory0_addr          = (uint32_t)(pDevice->pBuf);
        dma_single_data_parameter.memory_inc            = DMA_MEMORY_INCREASE_ENABLE;
        dma_single_data_parameter.periph_memory_width   = DMA_PERIPH_WIDTH_32BIT;
        dma_single_data_parameter.direction             = DMA_PERIPH_TO_MEMORY;
        dma_single_data_parameter.number                = i32RegNum;            //这里要填入规则采样的通道数,注入采样是不要DMA处理的
        dma_single_data_parameter.priority              = DMA_PRIORITY_HIGH;
        dma_single_data_mode_init(pDevice->stDma.i32Periph, pDevice->stDma.i32Chan, &dma_single_data_parameter);
        
        /* enable DMA circulation mode */
        dma_circulation_enable(pDevice->stDma.i32Periph, pDevice->stDma.i32Chan);
        
        /* enable DMA channel */
        dma_channel_enable(pDevice->stDma.i32Periph, pDevice->stDma.i32Chan);
    }
}








/**********************************************************************************************
* Description                           :   ADC驱动 timer初始化
* Author                                :   Hall
* modified Date                         :   2023-11-21
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevTimer(eAdcNum_t eAdcNum)
{
    stAdcDrvDevice_t *pDevice = NULL;
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    
    
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    if(pDevice->stTimer.i32PeriphRcu >= 0)
    {
        /* TIMERx configuration */
        timer_initpara.prescaler         = pDevice->stTimer.i32Psc;
        timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
        timer_initpara.counterdirection  = TIMER_COUNTER_UP;
        timer_initpara.period            = pDevice->stTimer.i32Period;
        timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
        timer_initpara.repetitioncounter = 0;
        timer_init(pDevice->stTimer.i32Periph, &timer_initpara);
        
        
        /* CHx configuration in PWM mode0 */
        timer_ocintpara.outputstate     = TIMER_CCX_ENABLE;
        timer_ocintpara.ocpolarity      = TIMER_OC_POLARITY_HIGH;
        timer_ocintpara.ocidlestate     = TIMER_OC_IDLE_STATE_LOW;
        timer_channel_output_config(pDevice->stTimer.i32Periph, pDevice->stTimer.i32Chan, &timer_ocintpara);
        
        //初始比较值先给 i32Period/2 确保外设内部可以发波
        //以便能产生触发信号
        timer_channel_output_pulse_value_config(pDevice->stTimer.i32Periph, pDevice->stTimer.i32Chan, pDevice->stTimer.i32Period / 2);
        timer_channel_output_mode_config(pDevice->stTimer.i32Periph, pDevice->stTimer.i32Chan, TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(pDevice->stTimer.i32Periph, pDevice->stTimer.i32Chan, TIMER_OC_SHADOW_ENABLE);
        
        /* enable TIMERx */
        timer_enable(pDevice->stTimer.i32Periph);
    }
    
}












/**********************************************************************************************
* Description                           :   ADC驱动 ADC外设初始化
* Author                                :   Hall
* modified Date                         :   2023-11-21
* notice                                :   
***********************************************************************************************/
void sAdcDrvInitDevAdc(eAdcNum_t eAdcNum)
{
    i32  j;
    i32  i32InsNum   = 0;                                                       //规则通道和注入通道配置用 初值全给0
    i32  i32RegNum   = 0;
    i32  i32InsIndex = 0;
    i32  i32RegIndex = 0;
    stAdcDrvDevice_t *pDevice = NULL;
    
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    if(pDevice->i32PeriphAdc > 0)
    {
        /* reset ADC */
        adc_deinit(pDevice->i32PeriphAdc);
        
        /* ADC clock config */
        if(pDevice->i32PeriphAdc == ADC1)
        {
            //ADC1 外设与 ADC0 外设用同一个时钟信号 RCU_ADC0
            adc_clock_config(ADC0, ADC_CLK_SYNC_HCLK_DIV6);
        }
        else
        {
            adc_clock_config(pDevice->i32PeriphAdc, ADC_CLK_SYNC_HCLK_DIV6);
        }
        
        /* ADC contineous function enable */
        //禁止连续模式:每一轮采样由选定的信号触发
        adc_special_function_config(pDevice->i32PeriphAdc, ADC_CONTINUOUS_MODE, DISABLE);
        
        /* ADC scan mode enable */
        //使能扫描模式:每一个外部触发信号,启动一轮转换,转换(规则或注入)组内全部的channel
        adc_special_function_config(pDevice->i32PeriphAdc, ADC_SCAN_MODE, ENABLE);
        
        /* ADC resolution config */
        adc_resolution_config(pDevice->i32PeriphAdc, ADC_RESOLUTION_12B);
        
        /* ADC data alignment config */
        adc_data_alignment_config(pDevice->i32PeriphAdc, ADC_DATAALIGN_RIGHT);
        
        
        //规则通道和注入通道配置
        for(j = 0; j < pDevice->i32ChanNum; j++)
        {
            if(pDevice->stCfgChan[j].eMode == eAdcDrvSampleModeIns)
            {
                //获取注入采样通道数
                i32InsNum++;
            }
            else
            {
                //获取规则采样通道数
                i32RegNum++;
            }
        }
        if(i32InsNum > 0)
        {
            adc_channel_length_config(pDevice->i32PeriphAdc, ADC_INSERTED_CHANNEL, i32InsNum);
            for(j = 0; j < pDevice->i32ChanNum; j++)
            {
                if(pDevice->stCfgChan[j].eMode == eAdcDrvSampleModeIns)
                {
                    adc_inserted_channel_config(pDevice->i32PeriphAdc, i32InsIndex, pDevice->stCfgChan[j].u8Channel, 175);
                    i32InsIndex++;
                }
            }
            
            //触发配置
            adc_external_trigger_config(pDevice->i32PeriphAdc, ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_RISING);
            if(pDevice->i32TrigSourceIns > 0)
            {
                trigsel_init(pDevice->i32TrigTargetIns, pDevice->i32TrigSourceIns);
            }
            
            //中断配置
            adc_interrupt_flag_clear(pDevice->i32PeriphAdc, ADC_INT_FLAG_EOIC);
            adc_interrupt_enable(pDevice->i32PeriphAdc, ADC_INT_EOIC);
            
        }
        if(i32RegNum > 0)
        {
            adc_channel_length_config(pDevice->i32PeriphAdc, ADC_REGULAR_CHANNEL,  i32RegNum);
            for(j = 0; j < pDevice->i32ChanNum; j++)
            {
                if(pDevice->stCfgChan[j].eMode == eAdcDrvSampleModeReg)
                {
                    adc_regular_channel_config(pDevice->i32PeriphAdc, i32RegIndex, pDevice->stCfgChan[j].u8Channel, 10);
                    i32RegIndex++;
                }
            }
            
            //触发配置
            adc_external_trigger_config(pDevice->i32PeriphAdc, ADC_REGULAR_CHANNEL,  EXTERNAL_TRIGGER_RISING);
            if(pDevice->i32TrigSourceReg > 0)
            {
                trigsel_init(pDevice->i32TrigTargetReg, pDevice->i32TrigSourceReg);
            }
            
            //中断配置
            adc_interrupt_flag_clear(pDevice->i32PeriphAdc, ADC_INT_FLAG_EOC);
            adc_interrupt_enable(pDevice->i32PeriphAdc, ADC_INT_EOC);
            
            //DMA配置---规则采样才有DMA
            adc_dma_request_after_last_enable(pDevice->i32PeriphAdc);
            adc_dma_mode_enable(pDevice->i32PeriphAdc);
        }
        
        
        /* enable ADC interface */
        adc_enable(pDevice->i32PeriphAdc);
        
        /* wait for ADC stability */
        vTaskDelay(1 / portTICK_RATE_MS);
        
        /* ADC calibration mode config */
        adc_calibration_mode_config(pDevice->i32PeriphAdc, ADC_CALIBRATION_OFFSET_MISMATCH);
        
        /* ADC calibration number config */
        adc_calibration_number(pDevice->i32PeriphAdc, ADC_CALIBRATION_NUM1);
        
        /* ADC calibration and reset calibration */
        adc_calibration_enable(pDevice->i32PeriphAdc);
    }
}










































