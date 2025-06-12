/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv_trans.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-21
 * @Attention             :     
 * @Brief                 :     Can驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-21
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "can_drv_trans.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "can_drv_trans";








extern stCanDrvDevice_t stCanDrvDevMap[];

SemaphoreHandle_t xSemCan[eCanNumMax];









bool sCanDrvOpen(eCanNum_t eCanNum, u32 u32BaudRate);
bool sCanDrvRecv(eCanNum_t eCanNum, stCanMsg_t *pMsg);
bool sCanDrvSend(eCanNum_t eCanNum, const stCanMsg_t *pMsg);


void sCanDrvIsr(eCanNum_t eCanNum);
bool sCanDrvRecvIsr(eCanNum_t eCanNum);















/*******************************************************************************
 * @FunctionName   :      sCanDrvOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:47:38
 * @Description    :      Can驱动 Can口打开函数
 * @Input          :      eCanNum               CAN外设号
 * @Input          :      u32BaudRate           CAN波特率
 * @Return         :      
*******************************************************************************/
bool sCanDrvOpen(eCanNum_t eCanNum, u32 u32BaudRate)
{
    bool bRst;
    
    
    xSemCan[eCanNum] = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemCan[eCanNum]);
    
    bRst  = sCanMsgBufInit(eCanNum, 128, 128);
    bRst &= sCanDrvInitDev(eCanNum, u32BaudRate);
    
    return(bRst);
}








/*******************************************************************************
 * @FunctionName   :      sCanDrvRecv
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年2月29日  15:27:18
 * @Description    :      Can驱动 接收函数
 * @Input          :      eCanNum           CAN外设号
 * @Input          :      pMsg              CAN消息结构体指针
 * @Return         :      
*******************************************************************************/
bool sCanDrvRecv(eCanNum_t eCanNum, stCanMsg_t *pMsg)
{
    return(sCanMsgBufRead(eCanNum, eCanMsgOpTypeRx, pMsg));
}











/*******************************************************************************
 * @FunctionName   :      sCanDrvSend
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年2月29日  16:07:07
 * @Description    :      Can驱动 发送函数
 * @Input          :      eCanNum           CAN外设号
 * @Input          :      pMsg              CAN消息结构体指针
 * @Return         :      
*******************************************************************************/
bool sCanDrvSend(eCanNum_t eCanNum, const stCanMsg_t* pMsg)
{
    i32  i32Time;
    stCanDrvDevice_t    *pDevice    = NULL;
    
    //0:输入参数限幅
    if((eCanNum >= eCanNumMax) || (pMsg == NULL))
    {
        return(false);
    }
    pDevice = &stCanDrvDevMap[eCanNum];
    
    //上锁
    xSemaphoreTake(xSemCan[eCanNum], portMAX_DELAY);
    
    //初始化结构体
    can_struct_para_init(CAN_MDSC_STRUCT, (can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc);
    
    //配置接收邮箱
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->ide         = (pMsg->u32Id & 0x80000000) >> 31;   //帧格式为扩展帧
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->rtr         = (pMsg->u32Id & 0x40000000) >> 30;   //发送数据帧
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->esi         = (pMsg->u32Id & 0x20000000) >> 29;   //帧格式为扩展帧
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->code        = CAN_MB_TX_STATUS_DATA;              //激活邮箱发送功能
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->id          = pMsg->u32Id & 0x1fffffff;           //消息ID
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->data        = (u32 *)pMsg->u8Data;                //数据
    ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc)->data_bytes  = pMsg->u8Len;                        //数据长度
    
    //发送
    can_mailbox_config(pDevice->i32PeriphCan, pDevice->stMailBox.i32TxMailBoxIndex, (can_mailbox_descriptor_struct *)pDevice->stMailBox.pTxMailBoxDesc);
    
    
    i32Time = 0;
    while((i32Time < cCanDrvTransDataTimeoutCnt) && (RESET == can_flag_get(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanTxMailBoxFlag)))
    {
        //等待发送完成，清除标志位
        i32Time++;
    }
    can_flag_clear(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanTxMailBoxFlag);
    
    
    //解锁
    xSemaphoreGive(xSemCan[eCanNum]);
    
    return((i32Time == cCanDrvTransDataTimeoutCnt) ? false : true);
}
















/*******************************************************************************
 * @FunctionName   :      sCanDrvIsr
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年2月29日  14:12:41
 * @Description    :      Can驱动 中断处理函数
 * @Input          :      eCanNum           CAN外设号
*******************************************************************************/
void sCanDrvIsr(eCanNum_t eCanNum)
{
    //接收中断
    if((stCanDrvDevMap[eCanNum].stMailBox.i32CanRxMailBoxInterrupt >= 0)
    && (RESET != can_interrupt_flag_get(stCanDrvDevMap[eCanNum].i32PeriphCan, stCanDrvDevMap[eCanNum].stMailBox.i32CanRxMailBoxFlag)))
    {
        //接收数据
        sCanDrvRecvIsr(eCanNum);
        
        //清除标志位
        can_interrupt_flag_clear(stCanDrvDevMap[eCanNum].i32PeriphCan, stCanDrvDevMap[eCanNum].stMailBox.i32CanRxMailBoxFlag);
    }
}











/*******************************************************************************
 * @FunctionName   :      sCanDrvRecvIsr
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月22日  17:54:26
 * @Description    :      Can驱动 中断处理函数之 接收处理
 * @Input          :      eCanNum           CAN外设号
 * @Return         :      
*******************************************************************************/
bool sCanDrvRecvIsr(eCanNum_t eCanNum)
{
    stCanMsg_t                      stMsg;
    stCanDrvDevice_t                *pDevice    = NULL;
    can_rx_fifo_struct              *pRxFifo    = NULL;
    can_mailbox_descriptor_struct   *pRxMbDes   = NULL;
    
    //0:输入参数限幅
    if(eCanNum >= eCanNumMax)
    {
        return(false);
    }
    pDevice = &stCanDrvDevMap[eCanNum];
    
    
    //等待邮箱就绪
    while(CAN_MB_RX_STATUS_BUSY & can_mailbox_code_get(pDevice->i32PeriphCan, pDevice->stMailBox.i32RxMailBoxIndex));
    
    
    //开始接收数据
    if(true == pDevice->stCanDrvRxFifoCfg.bRxFifoEnable)
    {
        //Rx FIFO接收
        can_rx_fifo_read(pDevice->i32PeriphCan, (can_rx_fifo_struct *)pDevice->stMailBox.pRxMailBoxDesc);
        
        //提取Buf中的有效信息---fifo模式下似乎并没有寄存器位去填充---msgId的bit29:错误消息位
        pRxFifo = (can_rx_fifo_struct*)pDevice->stMailBox.pRxMailBoxDesc;
        stMsg.u32Id       = pRxFifo->id + (pRxFifo->ide << 31) + (pRxFifo->rtr << 30);
        stMsg.u8Len       = pRxFifo->dlc;
        memcpy(stMsg.u8Data, (u8*)pRxFifo->data, stMsg.u8Len);
    }
    else
    {
        //正常接收
        if(ERROR == can_mailbox_receive_data_read(pDevice->i32PeriphCan, pDevice->stMailBox.i32RxMailBoxIndex, (can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc))
        {
            //邮箱繁忙,准备接收等待超时
            return(false);
        }
        
        //提取Buf中的有效信息
        pRxMbDes = (can_mailbox_descriptor_struct*)pDevice->stMailBox.pRxMailBoxDesc;
        stMsg.u32Id       = pRxMbDes->id + (pRxMbDes->ide << 31) + (pRxMbDes->rtr << 30) + (pRxMbDes->esi << 29);
        stMsg.u8Len       = pRxMbDes->dlc;
        memcpy(stMsg.u8Data, (u8*)pRxMbDes->data, stMsg.u8Len);
        
        
        //邮箱解锁
        can_mailbox_receive_unlock(pDevice->i32PeriphCan);
    }
    
    
    //将数据存入RingBuff
    sCanMsgBufWrite(eCanNum, eCanMsgOpTypeRx, &stMsg);
    
    
    if((pDevice->i32Irq < 0) && (pDevice->stMailBox.i32CanRxMailBoxInterrupt < 0))
    {
        //若非中断,需在此清除标志位
        can_flag_clear(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanRxMailBoxFlag);
    }
    
    return(true);
}
















/*******************************************************************************
 * @FunctionName   :      CAN0_Message_IRQHandler
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月20日  11:04:56
 * @Description    :      Can驱动 之 CAN0中断服务函数
 * @Input          :      void        
*******************************************************************************/
void CAN0_Message_IRQHandler(void)
{
    sCanDrvIsr(eCanNum0);
}










/*******************************************************************************
 * @FunctionName   :      CAN1_Message_IRQHandler
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月20日  11:05:25
 * @Description    :      Can驱动 之 CAN1中断服务函数
 * @Input          :      void        
*******************************************************************************/
void CAN1_Message_IRQHandler(void)
{
    sCanDrvIsr(eCanNum1);
}










/*******************************************************************************
 * @FunctionName   :      CAN2_Message_IRQHandler
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月20日  11:05:37
 * @Description    :      Can驱动 之 CAN2中断服务函数
 * @Input          :      void        
*******************************************************************************/
void CAN2_Message_IRQHandler(void)
{
    sCanDrvIsr(eCanNum2);
}













