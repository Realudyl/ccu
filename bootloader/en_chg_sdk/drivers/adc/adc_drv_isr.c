/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   adc_drv_isr.c
* Description                           :   ADC驱动 采样中断函数实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#include "adc_drv.h"
#include "adc_drv_isr.h"

#include "io_drv_api.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "adc_drv_isr";







stAdcDrvCache_t stAdcDrvCache;

extern stAdcDrvDevice_t stAdcDrvDevMap[eAdcNumMax];




bool sAdcDrvInit(TaskHandle_t xTask);
void sAdcDrvInitCache(TaskHandle_t xTask);

stAdcDrvDataTotal_t *sAdcDrvGetData(void);


void sAdcDrvIsrReg(eAdcNum_t eAdcNum);
void sAdcDrvIsrIns(eAdcNum_t eAdcNum);











/**********************************************************************************************
* Description                           :   ADC驱动 初始化函数
* Author                                :   Hall
* modified Date                         :   2023-12-06
* notice                                :   xTask           :采样任务句柄
*                                           i32GunNum       :充电枪数量，对应CP信号的路数
*                                           pCpAdcDevIndex  :传入每把枪，对应CP信号所在的ADC设备号码
***********************************************************************************************/
bool sAdcDrvInit(TaskHandle_t xTask)
{
    i32  i;
    bool bRst= true;
    
    
    sAdcDrvInitCache(xTask);
    
    for(i = eAdcNum0; i < eAdcNumMax; i++)
    {
        bRst &= sAdcDrvInitDev(i);
    }
    
    
    return(bRst);
}








/**********************************************************************************************
* Description                           :   ADC驱动 cache初始化函数
* Author                                :   Hall
* modified Date                         :   2024-02-21
* notice                                :   xTask           :采样任务句柄
*                                           i32GunNum       :充电枪数量，对应CP信号的路数
*                                           pCpAdcDevIndex  :传入每把枪，对应CP信号所在的ADC设备号码
***********************************************************************************************/
void sAdcDrvInitCache(TaskHandle_t xTask)
{
    i32  i, j;
    i32  i32AdcPin;
    
    memset(&stAdcDrvCache, 0, sizeof(stAdcDrvCache));
    stAdcDrvCache.xTaskSample = xTask;
    
    for(i = 0; i < eAdcNumMax; i++)
    {
        for(j = 0; j < stAdcDrvDevMap[i].i32ChanNum; j++)
        {
            i32AdcPin = stAdcDrvDevMap[i].stCfgChan[j].u8Pin;
            stAdcDrvCache.stDataTotal.stDataPin[i32AdcPin].eMode            = stAdcDrvDevMap[i].stCfgChan[j].eMode;
            stAdcDrvCache.stDataTotal.stDataPin[i32AdcPin].eType            = stAdcDrvDevMap[i].stCfgChan[j].eType;
            stAdcDrvCache.stDataTotal.stDataPin[i32AdcPin].f32Coef          = stAdcDrvDevMap[i].stCfgChan[j].f32Coef;
            stAdcDrvCache.stDataTotal.stDataPin[i32AdcPin].f32Offset        = stAdcDrvDevMap[i].stCfgChan[j].f32Offset;
            
        }
        
    }
}







/**********************************************************************************************
* Description                           :   ADC驱动  采样值获取函数
* Author                                :   Hall
* modified Date                         :   2024-02-21
* notice                                :   
***********************************************************************************************/
stAdcDrvDataTotal_t *sAdcDrvGetData(void)
{
    return(&stAdcDrvCache.stDataTotal);
}















/**********************************************************************************************
* Description                           :   ADC驱动 规则组采样中断处理函数
* Author                                :   Hall
* modified Date                         :   2023-11-21
* notice                                :   
***********************************************************************************************/
void sAdcDrvIsrReg(eAdcNum_t eAdcNum)
{
    i32  i;
    i32  i32RegIndex;
    bool bCycleFlag;
    BaseType_t HighTaskWoken = pdFALSE;
    
    stAdcDrvDataUnit_t  *pDataUnit    = NULL;
    stAdcDrvDataTotal_t *pDataTotal   = NULL;
    
    stAdcDrvDevice_t    *pDevice      = NULL;
    stAdcDrvCfgChan_t   *pCfgChan     = NULL;
    
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    pDataTotal = &stAdcDrvCache.stDataTotal;
    
    //1:cache内的数据写到内存中---规则组必然是要启用DMA的
    if((pDevice->stDma.i32Periph >= 0))
    {
        SCB_InvalidateDCache_by_Addr((u32 *)pDevice->pBuf, pDevice->i32Size);
    }
    
    
    //2:数据累加
    i32RegIndex = 0;
    bCycleFlag  = false;
    for(i = 0; i < pDevice->i32ChanNum; i++)
    {
        //2.1:通道定位
        pCfgChan  = &pDevice->stCfgChan[i];
        pDataUnit = &pDataTotal->stDataPin[pCfgChan->u8Pin];
        if(pDataUnit->eMode != eAdcDrvSampleModeReg)
        {
            //不是规则采样的通道直接略过
            continue;
        }
        
        
        //2.2:取ADC采样值
        pDataUnit->f32Ad = (f32)pDevice->pBuf[i32RegIndex];
        i32RegIndex++;
        
        
        //2.3:rms通道需要先计算真实值
        if(pDataUnit->eType == eAdcDrvSampleTypeRms)
        {
            pDataUnit->f32Real = (pDataUnit->f32Ad - pDataUnit->f32Offset) * pDataUnit->f32Coef;
        }
        
        
        //2.4:累加求和
        pDataUnit->f32SumTemp += ((pDataUnit->eType == eAdcDrvSampleTypeRms) ? (pDataUnit->f32Real * pDataUnit->f32Real) : (pDataUnit->f32Ad - pDataUnit->f32Offset));
        
        
        //2.5:累加次数
        pDataUnit->i32SumCntTemp++;
        if(pDataUnit->i32SumCntTemp >= cAdcDrvCycleReg)
        {
            pDataUnit->f32Sum           = pDataUnit->f32SumTemp;
            pDataUnit->i32SumCnt        = pDataUnit->i32SumCntTemp;
            pDataUnit->f32SumTemp       = 0;
            pDataUnit->i32SumCntTemp    = 0;
            
            bCycleFlag                  = true;
        }
    }
    
    
    //3:发送任务通知 通知采样任务执行计算
    if((bCycleFlag == true) && (stAdcDrvCache.xTaskSample != NULL))
    {
        xTaskNotifyFromISR(stAdcDrvCache.xTaskSample, eAdcDrvSampleModeReg, eSetValueWithOverwrite, &HighTaskWoken);
        portYIELD_FROM_ISR(HighTaskWoken);
    }
}







/**********************************************************************************************
* Description                           :   ADC驱动 注入组采样中断处理函数
* Author                                :   Hall
* modified Date                         :   2023-12-13
* notice                                :   
***********************************************************************************************/
void sAdcDrvIsrIns(eAdcNum_t eAdcNum)
{
    i32  i;
    i32  i32InsIndex;
    bool bCycleFlag;
    BaseType_t HighTaskWoken = pdFALSE;
    
    i32 *pSum = NULL;
    i32 *pSumTemp = NULL;
    
    stAdcDrvDataUnit_t  *pDataUnit    = NULL;
    stAdcDrvDataTotal_t *pDataTotal   = NULL;
    
    stAdcDrvDevice_t    *pDevice      = NULL;
    stAdcDrvCfgChan_t   *pCfgChan     = NULL;
    
    
    pDevice = &stAdcDrvDevMap[eAdcNum];
    pDataTotal = &stAdcDrvCache.stDataTotal;
    
    
    
    //1:数据累加
    i32InsIndex = 0;
    bCycleFlag  = false;
    for(i = 0; i < pDevice->i32ChanNum; i++)
    {
        //2.1:通道定位
        pCfgChan  = &pDevice->stCfgChan[i];
        pDataUnit = &pDataTotal->stDataPin[pCfgChan->u8Pin];
        if(pDataUnit->eMode != eAdcDrvSampleModeIns)
        {
            //不是注入采样的通道直接略过
            continue;
        }
        
        //2.2:取ADC采样值
        pDataUnit->f32Ad = adc_inserted_data_read(pDevice->i32PeriphAdc, i32InsIndex);
        i32InsIndex++;
        
        //2.3:rms通道需要先计算真实值
        if(pDataUnit->eType == eAdcDrvSampleTypeRms)
        {
            pDataUnit->f32Real = (pDataUnit->f32Ad - pDataUnit->f32Offset) * pDataUnit->f32Coef;
        }
        
        
        //2.4:累加求和
        pDataUnit->f32SumTemp += ((pDataUnit->eType == eAdcDrvSampleTypeRms) ? pDataUnit->f32Real : (pDataUnit->f32Ad - pDataUnit->f32Offset));
        
        
        //2.5:累加一个工频周期时
        pDataUnit->i32SumCntTemp++;
        if(pDataUnit->i32SumCntTemp >= cAdcDrvCycleIns)
        {
            pDataUnit->f32Sum           = pDataUnit->f32SumTemp;
            pDataUnit->i32SumCnt        = pDataUnit->i32SumCntTemp;
            pDataUnit->f32SumTemp       = 0;
            pDataUnit->i32SumCntTemp    = 0;
            
            bCycleFlag = true;
        }
    }
    
    //3:发送任务通知 通知采样任务执行计算
    if((bCycleFlag == true) && (stAdcDrvCache.xTaskSample != NULL))
    {
        xTaskNotifyFromISR(stAdcDrvCache.xTaskSample, eAdcDrvSampleModeIns, eSetValueWithOverwrite, &HighTaskWoken);
        portYIELD_FROM_ISR(HighTaskWoken);
    }
}















void ADC0_1_IRQHandler(void)
{
    if(RESET != adc_interrupt_flag_get(ADC0, ADC_INT_FLAG_EOIC))
    {
        adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
        sAdcDrvIsrIns(eAdcNum0);
    }
    if(dma_flag_get(stAdcDrvDevMap[eAdcNum0].stDma.i32Periph, stAdcDrvDevMap[eAdcNum0].stDma.i32Chan, DMA_FLAG_FTF) == SET)
    {
        dma_flag_clear(stAdcDrvDevMap[eAdcNum0].stDma.i32Periph, stAdcDrvDevMap[eAdcNum0].stDma.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        sAdcDrvIsrReg(eAdcNum0);
    }
    
    
    if(RESET != adc_interrupt_flag_get(ADC1, ADC_INT_FLAG_EOIC))
    {
        adc_interrupt_flag_clear(ADC1, ADC_INT_FLAG_EOIC);
        sAdcDrvIsrIns(eAdcNum1);
    }
    if(dma_flag_get(stAdcDrvDevMap[eAdcNum1].stDma.i32Periph, stAdcDrvDevMap[eAdcNum1].stDma.i32Chan, DMA_FLAG_FTF) == SET)
    {
        dma_flag_clear(stAdcDrvDevMap[eAdcNum1].stDma.i32Periph, stAdcDrvDevMap[eAdcNum1].stDma.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        sAdcDrvIsrReg(eAdcNum1);
    }
}





void ADC2_IRQHandler(void)
{
    if(RESET != adc_interrupt_flag_get(ADC2, ADC_INT_FLAG_EOIC))
    {
        adc_interrupt_flag_clear(ADC2, ADC_INT_FLAG_EOIC);
        sAdcDrvIsrIns(eAdcNum2);
    }
    if(dma_flag_get(stAdcDrvDevMap[eAdcNum2].stDma.i32Periph, stAdcDrvDevMap[eAdcNum2].stDma.i32Chan, DMA_FLAG_FTF) == SET)
    {
        dma_flag_clear(stAdcDrvDevMap[eAdcNum2].stDma.i32Periph, stAdcDrvDevMap[eAdcNum2].stDma.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        sAdcDrvIsrReg(eAdcNum2);
    }
}





