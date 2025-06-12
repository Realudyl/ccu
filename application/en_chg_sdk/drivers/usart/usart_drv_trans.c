/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv_trans.c
* Description                           :   串口驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-27
* notice                                :   
****************************************************************************************************/
#include "usart_drv_buff.h"
#include "usart_drv_trans.h"




extern stUsartDrvDevice_t stUsartDrvDevMap[];








bool sUsartDrvOpen(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg, i32 i32RxRingBufSize, i32 i32TxRingBufSize);
i32  sUsartDrvRecv(eUsartNum_t eUsartNum, i32 i32MaxLen, u8 *pBuf);
bool sUsartDrvSend(eUsartNum_t eUsartNum, i32 i32Len, const u8 *pBuf);
void sUsartDrvSendCmBackTrace(eUsartNum_t eUsartNum, u16 u16Data);



void sUsartDrvIsr(eUsartNum_t eUsartNum, i32 i32PeriphUsart);
void sUsartDrvIsrRecv(eUsartNum_t eUsartNum, i32 i32PeriphUsart);
void sUsartDrvIsrSend(eUsartNum_t eUsartNum, i32 i32PeriphUsart);



void sUsartDrvIsrRecvWithDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart);
void sUsartDrvIsrRecvWithoutDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart);


void sUsartDrvIsrSendWithDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart);
void sUsartDrvIsrSendWithoutDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart);




















/**********************************************************************************************
* Description                           :   串口驱动 串口打开函数
* Author                                :   Hall
* modified Date                         :   2023-12-01
* notice                                :   
*                                           eUsartNum:串口通道
*                                           pCfg     :串口参数
***********************************************************************************************/
bool sUsartDrvOpen(eUsartNum_t eUsartNum, stUsartDrvCfg_t *pCfg, i32 i32RxRingBufSize, i32 i32TxRingBufSize)
{
    bool bRst;
    
    
    bRst  = sUsartDrvBufInit(eUsartNum, i32RxRingBufSize, i32TxRingBufSize);
    bRst &= sUsartDrvInitDev(eUsartNum, pCfg);
    
    return(bRst);
}







/**********************************************************************************************
* Description                           :   串口驱动 接收函数
* Author                                :   Hall
* modified Date                         :   2023-12-01
* notice                                :   
*                                           eUsartNum:串口通道
*                                           i32MaxLen:pBuf指向的缓存区最大长度
*                                           pBuf     :    指向缓存区，用于接收数据
*
*                                           返回值:
*                                           0~正整数 :收到的数据长度
*                                           负数     :输入参数出错
***********************************************************************************************/
i32 sUsartDrvRecv(eUsartNum_t eUsartNum, i32 i32MaxLen, u8 *pBuf)
{
    i32  i32ReadLen;
    
    i32ReadLen = sUsartDrvBufRead(eUsartNum, eUsartDrvOpTypeRx, i32MaxLen, pBuf);
    
    return(i32ReadLen);
}





/**********************************************************************************************
* Description                           :   串口驱动 发送函数
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eUsartNum:串口通道
*                                           i32Len   :要发送的数据长度
*                                           pBuf     :要发送的数据内容
***********************************************************************************************/
bool sUsartDrvSend(eUsartNum_t eUsartNum, i32 i32Len, const u8 *pBuf)
{
    u8  u8Byte = 0;
    stUsartDrvDevice_t *pDevice = NULL;
    
    //0:输入参数限幅
    if((eUsartNum >= eUsartNumMax) || (i32Len < 0) || (pBuf == NULL))
    {
        return(false);
    }
    pDevice = &stUsartDrvDevMap[eUsartNum];
    
    
    //1:发送数据写入环形缓存
    if(sUsartDrvBufWrite(eUsartNum, eUsartDrvOpTypeTx, i32Len, pBuf) == false)
    {
        return(false);
    }
    
    
    //2:启动发送---发送完成中断使能 并发送首字节
    if(pDevice->bFirstByteFlag == true)
    {
        i32Len = sUsartDrvBufReadByte(eUsartNum, eUsartDrvOpTypeTx, &u8Byte);
        if(i32Len > 0)
        {
            //有数据发送的时候才需要将首字节发送标志位清除
            pDevice->bFirstByteFlag = false;
            //如果是485端口 需要使能发送
            if((pDevice->stGpioRs485.i32Periph >= 0) && (pDevice->pTxEnable != NULL))
            {
                pDevice->pTxEnable(pDevice->stGpioRs485.i32Periph, pDevice->stGpioRs485.i32Pin);
            }
            
            //读取成功，发送该字节
            usart_data_transmit(pDevice->i32PeriphUsart, u8Byte);
        }
    }
    
    return(true);
}








/*******************************************************************************
 * @FunctionName   :      sUsartDrvSendCmBackTrace
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月8日  19:58:00
 * @Description    :      CmBackTrace驱动 之 串口发送
 * @Input          :      i32PeriphUsart    串口外设号
 * @Input          :      u16Data           要发送的数据
 * @Return         :      
*******************************************************************************/
void sUsartDrvSendCmBackTrace(eUsartNum_t eUsartNum, u16 u16Data)
{
    i32 i32Time = 0;
    
    usart_data_transmit(stUsartDrvDevMap[eUsartNum].i32PeriphUsart, u16Data);
    
    //等待发送完成 10300 传输单个字节数据的超时时间 实测约87us---对应115200波特率时的一个字节
    while((i32Time < 10300) && (RESET == usart_interrupt_flag_get(stUsartDrvDevMap[eUsartNum].i32PeriphUsart, USART_INT_FLAG_TC)))
    {
        i32Time++;
    }
}













/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数
* Author                                :   Hall
* modified Date                         :   2023-12-06
* notice                                :   本中断处理函数可以提供给所有8个串口的中断函数调用  处理过程完全一样
*                                           本函数仅被8个中断函数调用且各通道资源独立  因此不用考虑函数重入问题
*
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsr(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    //1:接收处理
    if((RESET != usart_interrupt_flag_get(i32PeriphUsart, USART_INT_FLAG_RBNE))
    || (RESET != usart_interrupt_flag_get(i32PeriphUsart, USART_INT_FLAG_IDLE)))
    {
        sUsartDrvIsrRecv(eUsartNum, i32PeriphUsart);
    }
    
    
    //2:发送处理
    if(RESET != usart_interrupt_flag_get(i32PeriphUsart, USART_INT_FLAG_TC))
    {
        sUsartDrvIsrSend(eUsartNum, i32PeriphUsart);
    }
    
    
    //3:其他中断标志处理
    if(RESET != usart_flag_get(i32PeriphUsart, USART_FLAG_FERR))
    {
        usart_flag_clear(i32PeriphUsart, USART_FLAG_FERR);                      //帧错误  中断标志
    }
    if(RESET != usart_flag_get(i32PeriphUsart, USART_FLAG_NERR))
    {
        usart_flag_clear(i32PeriphUsart, USART_FLAG_NERR);                      //噪声错误中断标志
    }
    if(RESET != usart_flag_get(i32PeriphUsart, USART_FLAG_ORERR))
    {
        usart_flag_clear(i32PeriphUsart, USART_FLAG_ORERR);                     //溢出错误中断标志
    }
    if(RESET != usart_flag_get(i32PeriphUsart, USART_FLAG_PERR))
    {
        usart_flag_clear(i32PeriphUsart, USART_FLAG_PERR);                      //校验错误中断标志
    }
    
}






/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 接收处理
* Author                                :   Hall
* modified Date                         :   2023-12-06
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrRecv(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    if(RESET != usart_interrupt_flag_get(i32PeriphUsart, USART_INT_FLAG_RBNE))
    {
        //接收非空中断 用于非DMA接收
        sUsartDrvIsrRecvWithoutDma(eUsartNum, i32PeriphUsart);
    }
    else if(RESET != usart_interrupt_flag_get(i32PeriphUsart, USART_INT_FLAG_IDLE))
    {
        //空闲中断 用于DMA接收
        sUsartDrvIsrRecvWithDma(eUsartNum, i32PeriphUsart);
    }
}






/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 发送处理
* Author                                :   Hall
* modified Date                         :   2023-12-06
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrSend(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    //发送完成中断 用于DMA发送和非DMA发送
    usart_interrupt_flag_clear(i32PeriphUsart, USART_INT_FLAG_TC);
    if(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph < 0)
    {
        //非DMA发送
        sUsartDrvIsrSendWithoutDma(eUsartNum, i32PeriphUsart);
    }
    else
    {
        //DMA发送
        sUsartDrvIsrSendWithDma(eUsartNum, i32PeriphUsart);
        
    }
}







/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 接收处理 之 DMA接收
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrRecvWithDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    i32 i32Len;
    
    usart_interrupt_flag_clear(i32PeriphUsart, USART_INT_FLAG_IDLE);
    if(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph >= 0)
    {
        //禁止DMA通道--->计算已经接收的长度--->数据写入 接收ringbuffer--->重新设置接收数据个数--->使能DMA通道 继续接收
        dma_channel_disable(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaRx.i32Chan);
        i32Len = stUsartDrvDevMap[eUsartNum].i32DmaBufSize - dma_transfer_number_get(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaRx.i32Chan);
        SCB_InvalidateDCache_by_Addr((u32 *)stUsartDrvDevMap[eUsartNum].pDmaRxBuf, stUsartDrvDevMap[eUsartNum].i32DmaBufSize);
        sUsartDrvBufWrite(eUsartNum, eUsartDrvOpTypeRx, i32Len, stUsartDrvDevMap[eUsartNum].pDmaRxBuf);
        dma_transfer_number_config(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaRx.i32Chan, stUsartDrvDevMap[eUsartNum].i32DmaBufSize); ///< 重新设置接收数据个数
        
        dma_flag_clear(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaRx.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        
        dma_channel_enable(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaRx.i32Chan);
        
    }
}






/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 接收处理 之 非DMA接收
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrRecvWithoutDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    u8  u8Byte = 0;
    
    usart_interrupt_flag_clear(i32PeriphUsart, USART_INT_FLAG_RBNE);
    if(stUsartDrvDevMap[eUsartNum].stDmaRx.i32Periph < 0)
    {
        u8Byte = usart_data_receive(i32PeriphUsart);
        sUsartDrvBufWriteByte(eUsartNum, eUsartDrvOpTypeRx, u8Byte);
    }
}






/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 发送处理 之 DMA发送
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrSendWithDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    i32 i32Len;
    
    //从 发送ringbuffer 读取一个序列（长度不超过底层DMA发送buf）
    i32Len = sUsartDrvBufRead(eUsartNum, eUsartDrvOpTypeTx, stUsartDrvDevMap[eUsartNum].i32DmaBufSize, stUsartDrvDevMap[eUsartNum].pDmaTxBuf);
    SCB_CleanDCache_by_Addr((uint32_t *)stUsartDrvDevMap[eUsartNum].pDmaTxBuf, stUsartDrvDevMap[eUsartNum].i32DmaBufSize);
    
    if(i32Len > 0)
    {
        //读取到数据
        //禁止DMA通道--->设置DMA通道 x 计数寄存器--->使能DMA通道 开始发送
        dma_channel_disable(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaTx.i32Chan);
        dma_transfer_number_config(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaTx.i32Chan, i32Len);
        dma_flag_clear(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaTx.i32Chan, DMA_FLAG_FEE | DMA_FLAG_SDE | DMA_FLAG_TAE | DMA_FLAG_HTF | DMA_FLAG_FTF);
        dma_channel_enable(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaTx.i32Chan);
    }
    else if(i32Len <= 0)
    {
        //发送ringbuffer 空 关中断 下次发送时再开启
        dma_channel_disable(stUsartDrvDevMap[eUsartNum].stDmaTx.i32Periph, stUsartDrvDevMap[eUsartNum].stDmaTx.i32Chan);
        
        //需要置首字节标志 以便下一个帧的首字节能由外部启动发送
        stUsartDrvDevMap[eUsartNum].bFirstByteFlag = true;
        
        //如果是485端口 需要禁止发送
        if((stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Periph >= 0) && (stUsartDrvDevMap[eUsartNum].pTxDisable != NULL))
        {
            stUsartDrvDevMap[eUsartNum].pTxDisable(stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Periph, stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Pin);
        }
    }

}







/**********************************************************************************************
* Description                           :   串口驱动 中断处理函数之 发送处理 之 非DMA发送
* Author                                :   Hall
* modified Date                         :   2023-12-11
* notice                                :   
*                                           eUsartNum         :串口通道
*                                           i32PeriphUsart    :串口外设
***********************************************************************************************/
void sUsartDrvIsrSendWithoutDma(eUsartNum_t eUsartNum, i32 i32PeriphUsart)
{
    u8  u8Byte = 0;
    i32 i32Len;
    
    //从 ring buffer 读取一个字节
    i32Len = sUsartDrvBufReadByte(eUsartNum, eUsartDrvOpTypeTx, &u8Byte);
    if(i32Len > 0)
    {
        //读取到字节，发送该字节
        usart_data_transmit(i32PeriphUsart, u8Byte);
    }
    else if(i32Len <= 0)
    {
        //发送ringbuffer 空
        //需要置首字节标志 以便下一个帧的首字节能由外部启动发送
        stUsartDrvDevMap[eUsartNum].bFirstByteFlag = true;
        
        //如果是485端口 需要禁止发送
        if((stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Periph >= 0) && (stUsartDrvDevMap[eUsartNum].pTxDisable != NULL))
        {
            stUsartDrvDevMap[eUsartNum].pTxDisable(stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Periph, stUsartDrvDevMap[eUsartNum].stGpioRs485.i32Pin);
        }
    }
}











void USART0_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum0, stUsartDrvDevMap[eUsartNum0].i32PeriphUsart);
}






void USART1_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum1, stUsartDrvDevMap[eUsartNum1].i32PeriphUsart);
}






void USART2_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum2, stUsartDrvDevMap[eUsartNum2].i32PeriphUsart);
}






void UART3_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum3, stUsartDrvDevMap[eUsartNum3].i32PeriphUsart);
}






void UART4_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum4, stUsartDrvDevMap[eUsartNum4].i32PeriphUsart);
}






void USART5_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum5, stUsartDrvDevMap[eUsartNum5].i32PeriphUsart);
}






void UART6_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum6, stUsartDrvDevMap[eUsartNum6].i32PeriphUsart);
}






void UART7_IRQHandler(void)
{
    sUsartDrvIsr(eUsartNum7, stUsartDrvDevMap[eUsartNum7].i32PeriphUsart);
}













