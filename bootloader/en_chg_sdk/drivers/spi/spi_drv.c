/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   spi_drv.c
* Description                           :   SPI驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-06
* notice                                :   
****************************************************************************************************/
#include "spi_drv.h"














//SPI DMA缓存区
//此缓存区的起始地址和长度都必须保证是32字节的整数倍---注意起始地址
//所以不能使用MALLOC动态分配 只能按如下格式定义，
//并且这里全部SPI口都直接定义，就不考虑该SPI口是否启用了，会浪费一些ram，没办法
__attribute__((aligned(32))) u8         u8SpiRxDmaBuf[eSpiNumMax][cSpiDmaBufSize];
__attribute__((aligned(32))) u8         u8SpiTxDmaBuf[eSpiNumMax][cSpiDmaBufSize];





extern stSpiDrvDevice_t stSpiDrvDevMap[eSpiNumMax];








bool sSpiDrvInitDev(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg);
bool sSpiDrvInitDevRcu(eSpiNum_t eSpiNum);
bool sSpiDrvInitDevGpio(eSpiNum_t eSpiNum);
bool sSpiDrvInitDevNvic(eSpiNum_t eSpiNum);
bool sSpiDrvInitDevSpi(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg);
bool sSpiDrvInitDevDma(eSpiNum_t eSpiNum);











/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   
***********************************************************************************************/
bool sSpiDrvInitDev(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg)
{
    stSpiDrvDevice_t *pDevice = NULL;
    
    if(eSpiNum >= eSpiNumMax)
    {
        //SPI接口超限
        return(false);
    }
    
    
    pDevice = &stSpiDrvDevMap[eSpiNum];
    if(pDevice->i32PeriphRcuSpi < 0)
    {
        //SPI接口未启用
        return(false);
    }
    
    //1:申请设备内存
    pDevice->i32DmaBufSize = cSpiDmaBufSize;
    pDevice->pDmaRxBuf = (pDevice->stDmaMiso.i32Periph >= 0) ? u8SpiRxDmaBuf[eSpiNum] : NULL;
    pDevice->pDmaTxBuf = (pDevice->stDmaMosi.i32Periph >= 0) ? u8SpiTxDmaBuf[eSpiNum] : NULL;
    
    //2:时钟信号初始化
    sSpiDrvInitDevRcu(eSpiNum);
    
    //3:GPIO初始化
    sSpiDrvInitDevGpio(eSpiNum);
    
    //4:中断配置
    sSpiDrvInitDevNvic(eSpiNum);
    
    //5:SPI外设初始化
    sSpiDrvInitDevSpi(eSpiNum, pCfg);
    
    //6:DMA初始化
    sSpiDrvInitDevDma(eSpiNum);
    
    
    //7:NSS控制
    if(pDevice->pNssHigh != NULL)
    {
        pDevice->pNssHigh(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    
    return(true);
}








/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化 之 RCU使能
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   
***********************************************************************************************/
bool sSpiDrvInitDevRcu(eSpiNum_t eSpitNum)
{
    stSpiDrvDevice_t *pDevice = &stSpiDrvDevMap[eSpitNum];
    
    
    rcu_periph_clock_enable(pDevice->i32PeriphRcuSpi);
    
    if(pDevice->stDmaMiso.i32PeriphRcu >= 0)
    {
        //启用DMA时 DMAMUX时钟也必须使能
        rcu_periph_clock_enable(pDevice->stDmaMiso.i32PeriphRcu);
        rcu_periph_clock_enable(RCU_DMAMUX);
    }
    
    if(pDevice->stDmaMosi.i32PeriphRcu >= 0)
    {
        //启用DMA时 DMAMUX时钟也必须使能
        rcu_periph_clock_enable(pDevice->stDmaMosi.i32PeriphRcu);
        rcu_periph_clock_enable(RCU_DMAMUX);
    }
    
    rcu_periph_clock_enable(pDevice->stGpioMiso.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioMosi.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioSck.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioNss.i32PeriphRcu);
    
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化 之 GPIO配置
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   
***********************************************************************************************/
bool sSpiDrvInitDevGpio(eSpiNum_t eSpiNum)
{
    stSpiDrvDevice_t *pDevice = &stSpiDrvDevMap[eSpiNum];
    
    //MISO脚配置
    gpio_af_set(pDevice->stGpioMiso.i32Periph, pDevice->stGpioMiso.i32Af, pDevice->stGpioMiso.i32Pin);
    gpio_mode_set(pDevice->stGpioMiso.i32Periph, pDevice->stGpioMiso.i32Mode, pDevice->stGpioMiso.i32Pull, pDevice->stGpioMiso.i32Pin);
    gpio_output_options_set(pDevice->stGpioMiso.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioMiso.i32Speed, pDevice->stGpioMiso.i32Pin);
    
    //MOSI脚配置
    gpio_af_set(pDevice->stGpioMosi.i32Periph, pDevice->stGpioMosi.i32Af, pDevice->stGpioMosi.i32Pin);
    gpio_mode_set(pDevice->stGpioMosi.i32Periph, pDevice->stGpioMosi.i32Mode, pDevice->stGpioMosi.i32Pull, pDevice->stGpioMosi.i32Pin);
    gpio_output_options_set(pDevice->stGpioMosi.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioMosi.i32Speed, pDevice->stGpioMosi.i32Pin);
    
    //SCK脚配置
    gpio_af_set(pDevice->stGpioSck.i32Periph, pDevice->stGpioSck.i32Af, pDevice->stGpioSck.i32Pin);
    gpio_mode_set(pDevice->stGpioSck.i32Periph, pDevice->stGpioSck.i32Mode, pDevice->stGpioSck.i32Pull, pDevice->stGpioSck.i32Pin);
    gpio_output_options_set(pDevice->stGpioSck.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioSck.i32Speed, pDevice->stGpioSck.i32Pin);
    
    //NSS脚配置---使用软控时 不要配置af 否则SPI工作异常
    if(pDevice->stGpioNss.i32Af >= 0)
    {
        gpio_af_set(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Af, pDevice->stGpioNss.i32Pin);
    }
    gpio_mode_set(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Mode, pDevice->stGpioNss.i32Pull, pDevice->stGpioNss.i32Pin);
    gpio_output_options_set(pDevice->stGpioNss.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioNss.i32Speed, pDevice->stGpioNss.i32Pin);
    
    
    return(true);
}







/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化 之 中断配置
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   SPI驱动不需要使用中断  目前全屏蔽
***********************************************************************************************/
bool sSpiDrvInitDevNvic(eSpiNum_t eSpiNum)
{
    //stSpiDrvDevice_t *pDevice = &stSpiDrvDevMap[eSpiNum];
    
    
    //if(pDevice->i32Irq >= 0)
    //{
    //}
    
    return(true);
}











/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化 之 SPI外设初始化
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   
***********************************************************************************************/
bool sSpiDrvInitDevSpi(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg)
{
    stSpiDrvDevice_t *pDevice = &stSpiDrvDevMap[eSpiNum];
    
    spi_parameter_struct spi_init_struct;
    
    if(pDevice->i32PeriphSpi >= 0)
    {
        /* deinitilize SPIx and the parameters */
        spi_i2s_deinit(pDevice->i32PeriphSpi);
        spi_struct_para_init(&spi_init_struct);
        
        /* SPIx parameter configuration */
        spi_init_struct.trans_mode           = pCfg->i32TransMode;
        spi_init_struct.device_mode          = pCfg->i32DeviceMode;
        spi_init_struct.data_size            = pCfg->i32DataSize;
        spi_init_struct.clock_polarity_phase = pCfg->i32ClockPolarityPhase;
        spi_init_struct.nss                  = pCfg->i32Nss;
        spi_init_struct.prescale             = pCfg->i32Prescale;
        spi_init_struct.endian               = pCfg->i32Endian;
        spi_init(pDevice->i32PeriphSpi, &spi_init_struct);
        
        
        /* enable SPIx byte access */
        spi_byte_access_enable(pDevice->i32PeriphSpi);
        
        /* SPI NSS pin low level in software mode */
        spi_nss_internal_low(pDevice->i32PeriphSpi);
        
        /* enable SPIx NSS output */
        spi_nss_output_enable(pDevice->i32PeriphSpi);
        
        // NSS活动边缘与SPI主模式下开始传输或接收数据之间的延迟————经调整为1.5us
        spi_nss_idleness_delay_set(pDevice->i32PeriphSpi, SPI_NSS_IDLENESS_15CYCLE);
        
        
        /* SPI enable */
        spi_enable(pDevice->i32PeriphSpi);
        
        //使能SPI接口的DMA
        if(pDevice->stDmaMiso.i32Periph >= 0)
        {
            spi_dma_enable(pDevice->i32PeriphSpi, SPI_DMA_RECEIVE);
        }
        if(pDevice->stDmaMosi.i32Periph >= 0)
        {
            spi_dma_enable(pDevice->i32PeriphSpi, SPI_DMA_TRANSMIT);
        }
        
        
    }
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   SPI驱动 设备初始化 之 DMA初始化
* Author                                :   Hall
* modified Date                         :   2023-12-07
* notice                                :   
***********************************************************************************************/
bool sSpiDrvInitDevDma(eSpiNum_t eSpiNum)
{
    dma_single_data_parameter_struct dma_init_struct;
    stSpiDrvDevice_t *pDevice = &stSpiDrvDevMap[eSpiNum];
    
    
    //接收DMA通道
    if(pDevice->stDmaMiso.i32Periph >= 0)
    {
        /* initialize DMA channel 0 */
        dma_deinit(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_single_data_para_struct_init(&dma_init_struct);
        dma_init_struct.periph_addr  = (uint32_t)(&SPI_RDATA(pDevice->i32PeriphSpi));
        dma_init_struct.memory0_addr = (uint32_t)pDevice->pDmaRxBuf;
        dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
        dma_init_struct.number       = pDevice->i32DmaBufSize;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_init_struct.request      = pDevice->stDmaMiso.i32Req;
        dma_single_data_mode_init(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, &dma_init_struct);
        
        //通道禁止
        dma_channel_disable(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_flag_clear(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
    }
    
    
    //发送DMA通道
    if(pDevice->stDmaMosi.i32Periph >= 0)
    {
        /* initialize DMA channel 1 */
        dma_deinit(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        dma_single_data_para_struct_init(&dma_init_struct);
        dma_init_struct.periph_addr  = (uint32_t)(&SPI_TDATA(pDevice->i32PeriphSpi));
        dma_init_struct.memory0_addr = (uint32_t)pDevice->pDmaTxBuf;
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPH;
        dma_init_struct.number       = pDevice->i32DmaBufSize;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_init_struct.request      = pDevice->stDmaMosi.i32Req;
        dma_single_data_mode_init(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, &dma_init_struct);
        
        //通道禁止
        dma_channel_disable(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        dma_flag_clear(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
    }
    
    
    return(true);
}






























