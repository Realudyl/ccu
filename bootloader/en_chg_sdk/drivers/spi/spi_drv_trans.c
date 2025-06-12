/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   spi_drv_trans.c
* Description                           :   SPI驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-07
* notice                                :   
****************************************************************************************************/
#include "spi_drv_trans.h"






extern stSpiDrvDevice_t stSpiDrvDevMap[];
SemaphoreHandle_t xSemSpi[eSpiNumMax];




bool sSpiDrvOpen(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg);
bool sSpiDrvRecv(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf);
bool sSpiDrvSend(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf);
void sSpiDrvSetCs(eSpiNum_t eSpiNum, bool bOnOff);


bool sSpiDrvRecvWithDma(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf);
bool sSpiDrvRecvWithoutDma(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf);


bool sSpiDrvSendWithDma(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf);
bool sSpiDrvSendWithoutDma(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf);















/**********************************************************************************************
* Description                           :   Spi驱动 Spi口打开函数
* Author                                :   Hall
* modified Date                         :   2023-12-01
* notice                                :   
*                                           eSpiNum  :Spi口通道
*                                           pCfg     :Spi口参数
***********************************************************************************************/
bool sSpiDrvOpen(eSpiNum_t eSpiNum, stSpiDrvCfg_t *pCfg)
{
    bool bRst;
    
    if(eSpiNum >= eSpiNumMax)
    {
        //SPI接口超限
        return(false);
    }
    
    xSemSpi[eSpiNum] = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemSpi[eSpiNum]);
    
    bRst = sSpiDrvInitDev(eSpiNum, pCfg);
    
    return(bRst);
}







/**********************************************************************************************
* Description                           :   Spi驱动 接收函数
* Author                                :   Hall
* modified Date                         :   2023-12-01
* notice                                :   
*                                           eSpiNum  :串口通道
*                                           i32Len   :要接收的数据长度
*                                           pBuf     :  接收数据的存储地址
*
*                                           由于接收和发送操作本质上相同 因此 一个通道的收发不能同时执行 需要上锁
***********************************************************************************************/
bool sSpiDrvRecv(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf)
{
    stSpiDrvDevice_t *pDevice = NULL;
    
    //0:输入参数限幅
    if((eSpiNum >= eSpiNumMax) || (i32Len < 0) || (pBuf == NULL))
    {
        return(false);
    }
    
    
    //1:上锁
    xSemaphoreTake(xSemSpi[eSpiNum], portMAX_DELAY);
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    
    //2:NSS拉低
    if(pDevice->pNssLow != NULL)
    {
        pDevice->pNssLow(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    
    
    //3:接收
    if(pDevice->stDmaMosi.i32Periph < 0)
    {
        //非DMA接收
        sSpiDrvRecvWithoutDma(eSpiNum, i32Len, pBuf);
    }
    else
    {
        //DMA接收
        sSpiDrvRecvWithDma(eSpiNum, i32Len, pBuf);
    }
    
    
    //4:NSS拉高
    if(pDevice->pNssHigh != NULL)
    {
        pDevice->pNssHigh(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    
    
    //5:解锁
    xSemaphoreGive(xSemSpi[eSpiNum]);
    
    return(true);
}






/**********************************************************************************************
* Description                           :   Spi驱动 发送函数
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eSpiNum  :串口通道
*                                           i32Len   :要发送的数据长度
*                                           pBuf     :要发送的数据内容
*
*                                           由于接收和发送操作本质上相同 因此 一个通道的收发不能同时执行 需要上锁
***********************************************************************************************/
bool sSpiDrvSend(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf)
{
    stSpiDrvDevice_t *pDevice = NULL;
    
    //0:输入参数限幅
    if((eSpiNum >= eSpiNumMax) || (i32Len < 0) || (pBuf == NULL))
    {
        return(false);
    }
    
    //1:上锁
    xSemaphoreTake(xSemSpi[eSpiNum], portMAX_DELAY);
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    
    //2:NSS拉低
    if(pDevice->pNssLow != NULL)
    {
        pDevice->pNssLow(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    
    
    
    //3:发送
    if(pDevice->stDmaMosi.i32Periph < 0)
    {
        //非DMA发送
        sSpiDrvSendWithoutDma(eSpiNum, i32Len, pBuf);
    }
    else
    {
        //DMA发送
        sSpiDrvSendWithDma(eSpiNum, i32Len, pBuf);
    }
    
    
    //4:NSS拉高
    if(pDevice->pNssHigh != NULL)
    {
        pDevice->pNssHigh(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    
    
    //5:解锁
    xSemaphoreGive(xSemSpi[eSpiNum]);
    
    return(true);
}








/**********************************************************************************************
* Description                           :   Spi驱动 CS 信号软件控制
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
void sSpiDrvSetCs(eSpiNum_t eSpiNum, bool bOnOff)
{
    stSpiDrvDevice_t *pDevice = NULL;
    
    //0:输入参数限幅
    if(eSpiNum >= eSpiNumMax)
    {
        return;
    }
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    
    //1:NSS/CS信号控制
    if(bOnOff == true)
    {
        gpio_bit_set(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
    else
    {
        gpio_bit_reset(pDevice->stGpioNss.i32Periph, pDevice->stGpioNss.i32Pin);
    }
}








/**********************************************************************************************
* Description                           :   Spi驱动 DMA接收函数
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
***********************************************************************************************/
bool sSpiDrvRecvWithDma(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf)
{
    i32  i32Time;
    i32  i32TimeoutCnt;
    
    i32  i32LenRecv = 0;                                                        //本次接收长度
    i32  i32LenRecved = 0;                                                      //已  接收长度
    stSpiDrvDevice_t *pDevice = NULL;
    
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    i32LenRecved = 0;
    while(i32LenRecved < i32Len)
    {
        i32LenRecv = ((i32Len - i32LenRecved) < pDevice->i32DmaBufSize) ? (i32Len - i32LenRecved) : pDevice->i32DmaBufSize;
        memset(pDevice->pDmaTxBuf, cSpiDrvDummySendData, pDevice->i32DmaBufSize);
        
        spi_current_data_num_config(pDevice->i32PeriphSpi, i32LenRecv);         //指定长度传输模式
        spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_START);      //主机开始传输
        
        //清除DMA通道 传输完成标志
        dma_flag_clear(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        dma_flag_clear(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        dma_channel_disable(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_channel_disable(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        dma_transfer_number_config(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, i32LenRecv);
        dma_transfer_number_config(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, i32LenRecv);
        dma_channel_enable(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_channel_enable(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        
        
        i32Time = 0;
        i32TimeoutCnt = (cSpiDrvTransDataTimeoutCnt * i32Len);
        while((i32Time < i32TimeoutCnt) && (dma_flag_get(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, DMA_FLAG_FTF) == RESET))
        {
            //等待 DMA发送通道 传输完成标志
            i32Time++;
        }
        
        
        i32Time = 0;
        while((i32Time < i32TimeoutCnt) && (dma_flag_get(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, DMA_FLAG_FTF) == RESET))
        {
            //等待 DMA接收通道 传输完成标志
            i32Time++;
        }
        
        
        SCB_InvalidateDCache_by_Addr((u32 *)pDevice->pDmaRxBuf, i32LenRecv);
        memcpy(&pBuf[i32LenRecved], pDevice->pDmaRxBuf, i32LenRecv);
        i32LenRecved = i32LenRecved + i32LenRecv;
    }
    
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_IDLE);           //主机进入空闲状态
    
    return(true);
}






/**********************************************************************************************
* Description                           :   Spi驱动 非DMA接收函数
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
***********************************************************************************************/
bool sSpiDrvRecvWithoutDma(eSpiNum_t eSpiNum, i32 i32Len, u8 *pBuf)
{
    i32  i32Time;
    
    i32  i32LenRecv = 0;                                                        //本次接收长度
    i32  i32LenRecved = 0;                                                      //已  接收长度
    stSpiDrvDevice_t *pDevice = NULL;
    
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    i32LenRecved = 0;
    spi_current_data_num_config(pDevice->i32PeriphSpi, 0);                      //无限传输模式
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_START);          //主机开始传输
    while(i32LenRecved < i32Len)
    {
        i32Time = 0;
        while((i32Time < cSpiDrvTransDataTimeoutCnt) && (spi_i2s_flag_get(pDevice->i32PeriphSpi, SPI_FLAG_TP) == RESET))
        {
            //等待 TxFIFO 有足够的空间去接收下个数据包
            i32Time++;
        }
        spi_i2s_data_transmit(pDevice->i32PeriphSpi, cSpiDrvDummySendData);     //dummy send
        
        
        i32Time = 0;
        while((i32Time < cSpiDrvTransDataTimeoutCnt) && ((spi_i2s_flag_get(pDevice->i32PeriphSpi, SPI_FLAG_RPLVL) | spi_i2s_interrupt_flag_get(pDevice->i32PeriphSpi, SPI_I2S_INT_FLAG_RP)) == RESET))
        {
            //等待 RxFIFO 非空---至少有一个完整的数据包
            i32Time++;
        }
        
        
        pBuf[i32LenRecved] = spi_i2s_data_receive(pDevice->i32PeriphSpi);       //数据接收
        i32LenRecved++;
    }
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_IDLE);           //主机进入空闲状态
    
    return(true);
}








/**********************************************************************************************
* Description                           :   Spi驱动 DMA发送函数
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
***********************************************************************************************/
bool sSpiDrvSendWithDma(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf)
{
    i32  i32Time;
    i32  i32TimeoutCnt;
    
    i32  i32LenSend = 0;                                                        //本次发送长度
    i32  i32LenSended = 0;                                                      //已  发送长度
    stSpiDrvDevice_t *pDevice = NULL;
    
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    i32LenSended = 0;
    while(i32LenSended < i32Len)
    {
        i32LenSend = ((i32Len - i32LenSended) < pDevice->i32DmaBufSize) ? (i32Len - i32LenSended) : pDevice->i32DmaBufSize;
        memset(pDevice->pDmaTxBuf, 0, pDevice->i32DmaBufSize);
        memcpy(pDevice->pDmaTxBuf, &pBuf[i32LenSended], i32LenSend);
        SCB_CleanDCache_by_Addr((uint32_t *)pDevice->pDmaTxBuf, pDevice->i32DmaBufSize);
        
        spi_current_data_num_config(pDevice->i32PeriphSpi, i32LenSend);         //指定长度传输模式
        spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_START);      //主机开始传输
        
        dma_flag_clear(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        dma_flag_clear(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        dma_channel_disable(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_channel_disable(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        dma_transfer_number_config(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, i32LenSend);
        dma_transfer_number_config(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, i32LenSend);
        dma_channel_enable(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan);
        dma_channel_enable(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan);
        
        
        i32Time = 0;
        i32TimeoutCnt = (cSpiDrvTransDataTimeoutCnt * i32Len);
        while((i32Time < i32TimeoutCnt) && (dma_flag_get(pDevice->stDmaMosi.i32Periph, pDevice->stDmaMosi.i32Chan, DMA_FLAG_FTF) == RESET))
        {
            //等待 DMA发送通道 传输完成标志
            i32Time++;
        }
        
        
        i32Time = 0;
        while((i32Time < i32TimeoutCnt) && (dma_flag_get(pDevice->stDmaMiso.i32Periph, pDevice->stDmaMiso.i32Chan, DMA_FLAG_FTF) == RESET))
        {
            //等待 DMA接收通道 传输完成标志
            i32Time++;
        }
        
        
        i32LenSended = i32LenSended + i32LenSend;
    }
    
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_IDLE);       //主机进入空闲状态
    
    return(true);
}






/**********************************************************************************************
* Description                           :   Spi驱动 非DMA发送函数
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
***********************************************************************************************/
bool sSpiDrvSendWithoutDma(eSpiNum_t eSpiNum, i32 i32Len, const u8 *pBuf)
{
    i32  i32Time;
    
    i32  i32LenSend = 0;                                                        //本次发送长度
    i32  i32LenSended = 0;                                                      //已  发送长度
    stSpiDrvDevice_t *pDevice = NULL;
    
    pDevice = &stSpiDrvDevMap[eSpiNum];
    
    i32LenSended = 0;
    spi_current_data_num_config(pDevice->i32PeriphSpi, 0);                      //无限传输模式
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_START);          //主机开始传输
    while(i32LenSended < i32Len)
    {
        i32Time = 0;
        while((i32Time < cSpiDrvTransDataTimeoutCnt) && (spi_i2s_flag_get(pDevice->i32PeriphSpi, SPI_FLAG_TP) == RESET))
        {
            //等待 TxFIFO 有足够的空间去接收下个数据包
            i32Time++;
        }
        spi_i2s_data_transmit(pDevice->i32PeriphSpi, pBuf[i32LenSended]);
        
        
        i32Time = 0;
        while((i32Time < cSpiDrvTransDataTimeoutCnt) && ((spi_i2s_flag_get(pDevice->i32PeriphSpi, SPI_FLAG_RPLVL) | spi_i2s_interrupt_flag_get(pDevice->i32PeriphSpi, SPI_I2S_INT_FLAG_RP)) == RESET))
        {
            //等待 RxFIFO 非空---至少有一个完整的数据包
            i32Time++;
        }
        spi_i2s_data_receive(pDevice->i32PeriphSpi);                            //执行 dummy recv 以便清空接收缓存 否则会溢出错误
        i32LenSended++;
    }
    spi_master_transfer_start(pDevice->i32PeriphSpi, SPI_TRANS_IDLE);           //主机进入空闲状态
    
    return(true);
}
























