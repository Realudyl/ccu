/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv.c
* Description                           :   串口驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-27
* notice                                :   
****************************************************************************************************/
#include "usart_drv.h"











//串口DMA缓存区
//此缓存区的起始地址和长度都必须保证是32字节的整数倍---注意起始地址
//所以不能使用MALLOC动态分配 只能按如下格式定义，
//并且这里全部串口都直接定义，就不考虑该串口是否启用了，会浪费一些ram，没办法
__attribute__((aligned(32))) u8         u8UsartRxDmaBuf[eUsartNumMax][cUsartDmaBufSize];
__attribute__((aligned(32))) u8         u8UsartTxDmaBuf[eUsartNumMax][cUsartDmaBufSize];






extern stUsartDrvDevice_t stUsartDrvDevMap[eUsartNumMax];










bool sUsartDrvInitDev(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg);
bool sUsartDrvInitDevRcu(eUsartNum_t eUsartNum);
bool sUsartDrvInitDevGpio(eUsartNum_t eUsartNum);
bool sUsartDrvInitDevNvic(eUsartNum_t eUsartNum);
bool sUsartDrvInitDevUsart(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg);
bool sUsartDrvInitDevDma(eUsartNum_t eUsartNum);













/**********************************************************************************************
* Description                           :   串口驱动 设备初始化
* Author                                :   Hall
* modified Date                         :   2023-11-27
* notice                                :   
***********************************************************************************************/
bool sUsartDrvInitDev(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg)
{
    stUsartDrvDevice_t *pDevice = NULL;
    
    if(eUsartNum >= eUsartNumMax)
    {
        //串口号超限
        return(false);
    }
    
    
    pDevice = &stUsartDrvDevMap[eUsartNum];
    if(pDevice->i32PeriphRcuUsart < 0)
    {
        //串口未启用
        return(false);
    }
    
    //1:定位收发缓存区
    pDevice->i32DmaBufSize = cUsartDmaBufSize;
    pDevice->pDmaRxBuf = (pDevice->stDmaRx.i32Periph >= 0) ? u8UsartRxDmaBuf[eUsartNum] : NULL;
    pDevice->pDmaTxBuf = (pDevice->stDmaTx.i32Periph >= 0) ? u8UsartTxDmaBuf[eUsartNum] : NULL;
    
    //2:时钟信号初始化
    sUsartDrvInitDevRcu(eUsartNum);
    
    //3:GPIO初始化
    sUsartDrvInitDevGpio(eUsartNum);
    
    //4:中断配置
    sUsartDrvInitDevNvic(eUsartNum);
    
    //5:串口外设初始化
    sUsartDrvInitDevUsart(eUsartNum, pCfg);
    
    //6:DMA初始化
    sUsartDrvInitDevDma(eUsartNum);
    
    
    //7:如果是485端口 需要禁止发送
    if((pDevice->stGpioRs485.i32Periph >= 0) && (pDevice->pTxDisable != NULL))
    {
        pDevice->pTxDisable(pDevice->stGpioRs485.i32Periph, pDevice->stGpioRs485.i32Pin);
    }
    
    return(true);
}










/**********************************************************************************************
* Description                           :   串口驱动 设备初始化 之 RCU使能
* Author                                :   Hall
* modified Date                         :   2023-11-28
* notice                                :   使能串口eUsartNum 要用到的一系列RCU时钟信号
***********************************************************************************************/
bool sUsartDrvInitDevRcu(eUsartNum_t eUsartNum)
{
    stUsartDrvDevice_t *pDevice = &stUsartDrvDevMap[eUsartNum];
    
    
    rcu_periph_clock_enable(pDevice->i32PeriphRcuUsart);
    
    if(pDevice->stDmaRx.i32PeriphRcu >= 0)
    {
        //启用DMA时 DMAMUX时钟也必须使能
        rcu_periph_clock_enable(pDevice->stDmaRx.i32PeriphRcu);
        rcu_periph_clock_enable(RCU_DMAMUX);
    }
    
    if(pDevice->stDmaTx.i32PeriphRcu >= 0)
    {
        //启用DMA时 DMAMUX时钟也必须使能
        rcu_periph_clock_enable(pDevice->stDmaTx.i32PeriphRcu);
        rcu_periph_clock_enable(RCU_DMAMUX);
    }
    
    rcu_periph_clock_enable(pDevice->stGpioRx.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioTx.i32PeriphRcu);
    
    if(pDevice->stGpioRs485.i32PeriphRcu >= 0)
    {
        rcu_periph_clock_enable(pDevice->stGpioRs485.i32PeriphRcu);
    }
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 设备初始化 之 GPIO配置
* Author                                :   Hall
* modified Date                         :   2023-11-28
* notice                                :   
***********************************************************************************************/
bool sUsartDrvInitDevGpio(eUsartNum_t eUsartNum)
{
    stUsartDrvDevice_t *pDevice = &stUsartDrvDevMap[eUsartNum];
    
    //接收脚配置
    gpio_af_set(pDevice->stGpioRx.i32Periph, pDevice->stGpioRx.i32Af, pDevice->stGpioRx.i32Pin);
    gpio_mode_set(pDevice->stGpioRx.i32Periph, pDevice->stGpioRx.i32Mode, pDevice->stGpioRx.i32Pull, pDevice->stGpioRx.i32Pin);
    gpio_output_options_set(pDevice->stGpioRx.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioRx.i32Speed, pDevice->stGpioRx.i32Pin);
    
    //发送脚配置
    gpio_af_set(pDevice->stGpioTx.i32Periph, pDevice->stGpioTx.i32Af, pDevice->stGpioTx.i32Pin);
    gpio_mode_set(pDevice->stGpioTx.i32Periph, pDevice->stGpioTx.i32Mode, pDevice->stGpioTx.i32Pull, pDevice->stGpioTx.i32Pin);
    gpio_output_options_set(pDevice->stGpioTx.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioTx.i32Speed, pDevice->stGpioTx.i32Pin);
    
    //485发送使能脚配置---如果有的话
    if(pDevice->stGpioRs485.i32Periph >= 0)
    {
        gpio_mode_set(pDevice->stGpioRs485.i32Periph, pDevice->stGpioRs485.i32Mode, pDevice->stGpioRs485.i32Pull, pDevice->stGpioRs485.i32Pin);
        gpio_output_options_set(pDevice->stGpioRs485.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioRs485.i32Speed, pDevice->stGpioRs485.i32Pin);
    }
    
    return(true);
}







/**********************************************************************************************
* Description                           :   串口驱动 设备初始化 之 中断配置
* Author                                :   Hall
* modified Date                         :   2023-11-28
* notice                                :   
***********************************************************************************************/
bool sUsartDrvInitDevNvic(eUsartNum_t eUsartNum)
{
    stUsartDrvDevice_t *pDevice = &stUsartDrvDevMap[eUsartNum];
    
    
    if(pDevice->i32Irq >= 0)
    {
        //可设置范围：抢占优先级0~3,响应优先级0~3
        nvic_irq_enable(pDevice->i32Irq, 2U, 0U);
    }
    
    return(true);
}











/**********************************************************************************************
* Description                           :   串口驱动 设备初始化 之 Usart外设初始化
* Author                                :   Hall
* modified Date                         :   2023-11-27
* notice                                :   
***********************************************************************************************/
bool sUsartDrvInitDevUsart(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg)
{
    i32 i32Time;
    stUsartDrvDevice_t *pDevice = &stUsartDrvDevMap[eUsartNum];
    
    
    /* USART configure */
    usart_deinit(pDevice->i32PeriphUsart);
    usart_word_length_set(pDevice->i32PeriphUsart, pCfg->i32WordLen);
    usart_stop_bit_set(pDevice->i32PeriphUsart, pCfg->i32StopBit);
    usart_parity_config(pDevice->i32PeriphUsart, pCfg->i32Praity);
    usart_baudrate_set(pDevice->i32PeriphUsart, pCfg->i32Baudrate);
    usart_receive_config(pDevice->i32PeriphUsart, USART_RECEIVE_ENABLE);
    usart_transmit_config(pDevice->i32PeriphUsart, USART_TRANSMIT_ENABLE);
    usart_enable(pDevice->i32PeriphUsart);
    
    i32Time = 0;
    while((i32Time < 10) && (RESET == usart_flag_get(pDevice->i32PeriphUsart, USART_FLAG_IDLE)))
    {
        //等待 USART_FLAG_IDLE 标志置位 然后再清除它
        i32Time++;
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    
    /* USART DMA enable for reception */
    if(pDevice->stDmaRx.i32Periph >= 0)
    {
        usart_dma_receive_config(pDevice->i32PeriphUsart, USART_RECEIVE_DMA_ENABLE);
        
        usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_IDLE);             //空    闲中断标志---用于DMA接收
        usart_interrupt_enable(pDevice->i32PeriphUsart, USART_INT_IDLE);
    }
    else
    {
        usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_RBNE);             //接收非空中断标志---用于非DMA接收
        usart_interrupt_enable(pDevice->i32PeriphUsart, USART_INT_RBNE);
    }
    
    /* USART DMA enable for transmission and reception */
    if(pDevice->stDmaTx.i32Periph >= 0)
    {
        usart_dma_transmit_config(pDevice->i32PeriphUsart, USART_TRANSMIT_DMA_ENABLE);
    }
    
    
    usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_FERR);                 //帧  错误中断标志
    usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_NERR);                 //噪声错误中断标志
    usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_ORERR);                //溢出错误中断标志
    usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_PERR);                 //校验错误中断标志
    
    usart_flag_clear(pDevice->i32PeriphUsart, USART_FLAG_TC);                   //发送完成中断标志---DMA发送和非DMA发送均使用本中断
    usart_interrupt_enable(pDevice->i32PeriphUsart, USART_INT_TC);
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 设备初始化 之 DMA初始化
* Author                                :   Hall
* modified Date                         :   2023-11-28
* notice                                :   
***********************************************************************************************/
bool sUsartDrvInitDevDma(eUsartNum_t eUsartNum)
{
    dma_single_data_parameter_struct dma_init_struct;
    stUsartDrvDevice_t *pDevice = &stUsartDrvDevMap[eUsartNum];
    
    //发送DMA通道
    if(pDevice->stDmaTx.i32Periph >= 0)
    {
        dma_deinit(pDevice->stDmaTx.i32Periph, pDevice->stDmaTx.i32Chan);
        dma_single_data_para_struct_init(&dma_init_struct);
        dma_init_struct.request      = pDevice->stDmaTx.i32Req;
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPH;
        dma_init_struct.memory0_addr = (uint32_t)pDevice->pDmaTxBuf;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.number       = pDevice->i32DmaBufSize;
        dma_init_struct.periph_addr  = (uint32_t)(&USART_TDATA(pDevice->i32PeriphUsart));
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_single_data_mode_init(pDevice->stDmaTx.i32Periph, pDevice->stDmaTx.i32Chan, &dma_init_struct);
        
        //禁止循环模式---见用户手册关于循环模式的说明
        dma_circulation_disable(pDevice->stDmaTx.i32Periph, pDevice->stDmaTx.i32Chan);
        
        //发送通道先要禁止，等要发送的时候才使能
        dma_channel_disable(pDevice->stDmaTx.i32Periph, pDevice->stDmaTx.i32Chan);
    }
    
    //接收DMA通道
    if(pDevice->stDmaRx.i32Periph >= 0)
    {
        dma_deinit(pDevice->stDmaRx.i32Periph, pDevice->stDmaRx.i32Chan);
        dma_single_data_para_struct_init(&dma_init_struct);
        dma_init_struct.request      = pDevice->stDmaRx.i32Req;
        dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
        dma_init_struct.memory0_addr = (uint32_t)pDevice->pDmaRxBuf;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.number       = pDevice->i32DmaBufSize;
        dma_init_struct.periph_addr  = (uint32_t)(&USART_RDATA(pDevice->i32PeriphUsart));
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_single_data_mode_init(pDevice->stDmaRx.i32Periph, pDevice->stDmaRx.i32Chan, &dma_init_struct);
        
        //禁止循环模式---见用户手册关于循环模式的说明
        dma_circulation_disable(pDevice->stDmaRx.i32Periph, pDevice->stDmaRx.i32Chan);
        
        //接收通道直接使能
        dma_channel_enable(pDevice->stDmaRx.i32Periph, pDevice->stDmaRx.i32Chan);
    }
    
    
    return(true);
}










