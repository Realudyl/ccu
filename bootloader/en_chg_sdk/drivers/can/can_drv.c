/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-20
 * @Attention             :     
 * @Brief                 :     Can驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-20
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "can_drv.h"










extern stCanDrvDevice_t stCanDrvDevMap[eCanNumMax];






bool sCanDrvInitDev(eCanNum_t eCanNum, u32 u32BaudRate);
bool sCanDrvInitDevRcu(eCanNum_t eCanNum);
bool sCanDrvInitDevGpio(eCanNum_t eCanNum);
bool sCanDrvInitDevNvic(eCanNum_t eCanNum);
bool sCanDrvInitDevCan(eCanNum_t eCanNum, u32 u32BaudRate);
bool sCanDrvInitDevMailBoxDesc(eCanNum_t eCanNum);
bool sCanDrvInitDevDma(eCanNum_t eCanNum);









/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDev
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  13:58:31
 * @Description    :      Can驱动 设备初始化
 * @Input          :      eCanNum           CAN外设号
 * @Input          :      u32BaudRate       CAN波特率
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDev(eCanNum_t eCanNum, u32 u32BaudRate)
{
    stCanDrvDevice_t *pDevice = NULL;
    
    
    if(eCanNum >= eCanNumMax)
    {
        //CAN外设超限
        return(false);
    }
    
    pDevice = &stCanDrvDevMap[eCanNum];
    if(pDevice->i32PeriphRcuCan < 0)
    {
        //CAN外设未启用
        return(false);
    }
    
    
    //1:时钟信号初始化
    sCanDrvInitDevRcu(eCanNum);
    
    
    //2:GPIO初始化
    sCanDrvInitDevGpio(eCanNum);
    
    
    //3:中断配置
    sCanDrvInitDevNvic(eCanNum);
    
    
    //4:CAN外设初始化
    sCanDrvInitDevCan(eCanNum, u32BaudRate);
    
    
    //5:邮箱初始化
    sCanDrvInitDevMailBoxDesc(eCanNum);
    
    return(true);
}








/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDevRcu
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  14:00:35
 * @Description    :      Can驱动 设备初始化 之 RCU使能
 * @Input          :      eCanNum     CAN外设号
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDevRcu(eCanNum_t eCanNum)
{
    stCanDrvDevice_t *pDevice = &stCanDrvDevMap[eCanNum];
    
    //配置CAN外设时钟
    rcu_can_clock_config((can_idx_enum)eCanNum, RCU_CANSRC_APB2);
    
    
    //使能CAN外设时钟
    rcu_periph_clock_enable(pDevice->i32PeriphRcuCan);
    
    
    //使能GPIO外设时钟
    rcu_periph_clock_enable(pDevice->stGpioRx.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioTx.i32PeriphRcu);
    
    return(true);
}








/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDevGpio
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  14:02:00
 * @Description    :      Can驱动 设备初始化 之 GPIO配置
 * @Input          :      eCanNum     CAN外设号
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDevGpio(eCanNum_t eCanNum)
{
    stCanDrvDevice_t *pDevice = &stCanDrvDevMap[eCanNum];
    
    
    //接收脚配置
    gpio_af_set(pDevice->stGpioRx.i32Periph, pDevice->stGpioRx.i32Af, pDevice->stGpioRx.i32Pin);
    gpio_mode_set(pDevice->stGpioRx.i32Periph, pDevice->stGpioRx.i32Mode, pDevice->stGpioRx.i32Pull, pDevice->stGpioRx.i32Pin);
    gpio_output_options_set(pDevice->stGpioRx.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioRx.i32Speed, pDevice->stGpioRx.i32Pin);
    
    
    //发送脚配置
    gpio_af_set(pDevice->stGpioTx.i32Periph, pDevice->stGpioTx.i32Af, pDevice->stGpioTx.i32Pin);
    gpio_mode_set(pDevice->stGpioTx.i32Periph, pDevice->stGpioTx.i32Mode, pDevice->stGpioTx.i32Pull, pDevice->stGpioTx.i32Pin);
    gpio_output_options_set(pDevice->stGpioTx.i32Periph, GPIO_OTYPE_PP, pDevice->stGpioTx.i32Speed, pDevice->stGpioTx.i32Pin);
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDevNvic
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  14:03:17
 * @Description    :      Can驱动 设备初始化 之 中断使能
 * @Input          :      eCanNum     CAN外设号
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDevNvic(eCanNum_t eCanNum)
{
    stCanDrvDevice_t *pDevice = &stCanDrvDevMap[eCanNum];
    
    
    if(pDevice->i32Irq >= 0)
    {
        //可设置范围：抢占优先级0~3,响应优先级0~3
        nvic_irq_enable(pDevice->i32Irq, 1U, 1U);
    }
    
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDevCan
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  14:03:40
 * @Description    :      Can驱动 设备初始化 之 Can外设初始化
 * @Input          :      eCanNum     CAN外设号
 * @Input          :      pCfg        CAN外设配置
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDevCan(eCanNum_t eCanNum, u32 u32BaudRate)
{
    can_parameter_struct        stCanParameter;
    can_fifo_parameter_struct   stCanFifoParameter;
    stCanDrvDevice_t *pDevice = &stCanDrvDevMap[eCanNum];
    
    
    //还原CAN外设配置和配置结构体
    can_deinit(pDevice->i32PeriphCan);
    can_struct_para_init(CAN_INIT_STRUCT, &stCanParameter);
    if(true == pDevice->stCanDrvRxFifoCfg.bRxFifoEnable)
    {
        can_struct_para_init(CAN_FIFO_INIT_STRUCT, &stCanFifoParameter);
    }
    
    
    //波特率配置
    switch(u32BaudRate)
    {
        /* SJW值固定设为1 */
        /* BaudRate = CAN_CLK/((SJW + PTS + PBS1 + PBS2) * PRES) */
        /* SamplePoint = (SJW + PTS + PBS1)/(SJW + PTS + PBS1 + PBS2) × 100% */
        
        case (50000):             //50K bps   80%
            stCanParameter.prescaler                         = 400U;     //PRES
            stCanParameter.resync_jump_width                 = 1U;       //SJW
            stCanParameter.prop_time_segment                 = 2U;       //PTS
            stCanParameter.time_segment_1                    = 5U;       //PBS1
            stCanParameter.time_segment_2                    = 2U;       //PBS2
            break;
            
        case (125000):            //125K bps  80%
            stCanParameter.prescaler                         = 160U;     //PRES
            stCanParameter.resync_jump_width                 = 1U;       //SJW
            stCanParameter.prop_time_segment                 = 2U;       //PTS
            stCanParameter.time_segment_1                    = 5U;       //PBS1
            stCanParameter.time_segment_2                    = 2U;       //PBS2
            break;
            
        case (250000):            //250k bps  80%
            stCanParameter.prescaler                         = 80U;      //PRES
            stCanParameter.resync_jump_width                 = 1U;       //SJW
            stCanParameter.prop_time_segment                 = 2U;       //PTS
            stCanParameter.time_segment_1                    = 5U;       //PBS1
            stCanParameter.time_segment_2                    = 2U;       //PBS2
            break;
            
        case (500000):            //500k bps  80%
            stCanParameter.prescaler                         = 40U;      //PRES
            stCanParameter.resync_jump_width                 = 1U;       //SJW
            stCanParameter.prop_time_segment                 = 2U;       //PTS
            stCanParameter.time_segment_1                    = 5U;       //PBS1
            stCanParameter.time_segment_2                    = 2U;       //PBS2
            break;
            
        default:
            return(false);
            break;
    }
    
    
    stCanParameter.internal_counter_source           = CAN_TIMER_SOURCE_BIT_CLOCK;
    stCanParameter.self_reception                    = DISABLE;
    stCanParameter.mb_tx_order                       = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    stCanParameter.mb_tx_abort_enable                = ENABLE;
    stCanParameter.local_priority_enable             = DISABLE;
    stCanParameter.mb_rx_ide_rtr_type                = CAN_IDE_RTR_FILTERED;
    stCanParameter.mb_remote_frame                   = CAN_STORE_REMOTE_REQUEST_FRAME;
    stCanParameter.rx_private_filter_queue_enable    = ENABLE;
    stCanParameter.edge_filter_enable                = DISABLE;
    stCanParameter.protocol_exception_enable         = DISABLE;
    stCanParameter.rx_filter_order                   = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    stCanParameter.memory_size                       = CAN_MEMSIZE_32_UNIT;
    stCanParameter.mb_public_filter                  = 0x0u;
    
    //初始化 CAN
    can_init(pDevice->i32PeriphCan, &stCanParameter);
    can_operation_mode_enter(pDevice->i32PeriphCan, CAN_INACTIVE_MODE);
    //设置邮箱私有滤波器
    can_private_filter_config(pDevice->i32PeriphCan, pDevice->stMailBox.i32RxMailBoxIndex, pDevice->stMailBox.u32PrivateFilter);
    
    if(true == pDevice->stCanDrvRxFifoCfg.bRxFifoEnable)
    {
        stCanFifoParameter.dma_enable               = DISABLE;
        stCanFifoParameter.filter_format_and_number = CAN_RXFIFO_FILTER_A_NUM_8; //过滤器A，8个过滤器，默认值，此配置无用。因为所有filter为0x0，不会滤除任何数据。
        stCanFifoParameter.fifo_public_filter       = 0x0u;             //共有滤波器不起作用。私有滤波器默认为0x0，同样不起作用。
        //初始化 CAN Rx FIFO
        can_rx_fifo_config(pDevice->i32PeriphCan, &stCanFifoParameter);
    }
    
    
    //清除标志位
    can_flag_clear(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanRxMailBoxFlag);
    can_flag_clear(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanTxMailBoxFlag);
    
    
    //使能CAN 邮箱中断
    if(pDevice->stMailBox.i32CanRxMailBoxInterrupt >= 0)
    {
        can_interrupt_enable(pDevice->i32PeriphCan, pDevice->stMailBox.i32CanRxMailBoxInterrupt);
    }
    
    
    //进入对应的模式
    can_operation_mode_enter(pDevice->i32PeriphCan, CAN_NORMAL_MODE);
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sCanDrvInitDevMailBoxDesc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月21日  14:04:25
 * @Description    :      Can驱动 设备初始化 之 Can外设接收/发送邮箱描述符初始化
 * @Input          :      eCanNum     CAN外设号
 * @Return         :      
*******************************************************************************/
bool sCanDrvInitDevMailBoxDesc(eCanNum_t eCanNum)
{
    stCanDrvDevice_t *pDevice = &stCanDrvDevMap[eCanNum];
    
    
    if(pDevice->stCanDrvRxFifoCfg.bRxFifoEnable)
    {
        //申请RxFIFO结构体缓存
        pDevice->stMailBox.pRxMailBoxDesc = (can_rx_fifo_struct *)MALLOC(sizeof(can_rx_fifo_struct));
        if(pDevice->stMailBox.pRxMailBoxDesc == NULL)
        {
            //接收邮箱缓存申请失败
            return(false);
        }
    }
    else
    {
        //申请接收邮箱结构体缓存
        pDevice->stMailBox.pRxMailBoxDesc = (can_mailbox_descriptor_struct *)MALLOC(sizeof(can_mailbox_descriptor_struct));
        if(pDevice->stMailBox.pRxMailBoxDesc == NULL)
        {
            //接收邮箱缓存申请失败
            return(false);
        }
    }
    
    //申请发送邮箱结构体缓存
    pDevice->stMailBox.pTxMailBoxDesc = (can_mailbox_descriptor_struct *)MALLOC(sizeof(can_mailbox_descriptor_struct));
    if(pDevice->stMailBox.pTxMailBoxDesc == NULL)
    {
        //发送邮箱缓存申请失败
        if(pDevice->stMailBox.pRxMailBoxDesc != NULL)
        {
            //释放已申请的接收邮箱缓存
            FREE(pDevice->stMailBox.pRxMailBoxDesc);
        }
        return(false);
    }
    
    
    //初始化接收邮箱描述符
    if(pDevice->stCanDrvRxFifoCfg.bRxFifoEnable)
    {
        //Rx FIFO ,过滤功能无用，因为filter全部为0x0
        can_struct_para_init(CAN_FDES_STRUCT, (can_rx_fifo_struct *)pDevice->stMailBox.pRxMailBoxDesc);
        ((can_rx_fifo_struct *)pDevice->stMailBox.pRxMailBoxDesc)->rtr     = 0U;                                            //接收数据帧
        ((can_rx_fifo_struct *)pDevice->stMailBox.pRxMailBoxDesc)->ide     = 1U;                                            //帧格式为扩展帧
        ((can_rx_fifo_struct *)pDevice->stMailBox.pRxMailBoxDesc)->id      = 0x0;                                           //用于过滤的id_mask，
    }
    else
    {
        //Class
        can_struct_para_init(CAN_MDSC_STRUCT, (can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc);
        
        ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc)->rtr     = 0U;                                 //接收数据帧
        ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc)->ide     = 1U;                                 //帧格式为扩展帧
        ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc)->code    = CAN_MB_RX_STATUS_EMPTY;             //邮箱状态设置为EMPTY,激活邮箱接收功能
        ((can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc)->id      = pDevice->stMailBox.u32IdMask;       //接收所有消息ID
        //配置接收邮箱
        can_mailbox_config(pDevice->i32PeriphCan, pDevice->stMailBox.i32RxMailBoxIndex, (can_mailbox_descriptor_struct *)pDevice->stMailBox.pRxMailBoxDesc);
    }
    
    return(true);
}





/**
 * @brief 主动配置can接收邮箱ID掩码、私有过滤器
 * 
 * @param eCanNum 
 * @param u32IdMask 
 * @param u32Filter 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-05-21
 */
void sCanDrvSetCanIdMaskPrivateFilter(eCanNum_t eCanNum, u32 u32IdMask, u32 u32Filter)
{
    can_mailbox_descriptor_struct stCan = {0};
    
    
    can_operation_mode_enter(stCanDrvDevMap[eCanNum].i32PeriphCan, CAN_INACTIVE_MODE);
    can_private_filter_config(stCanDrvDevMap[eCanNum].i32PeriphCan, stCanDrvDevMap[eCanNum].stMailBox.i32RxMailBoxIndex, u32Filter);
    can_operation_mode_enter(stCanDrvDevMap[eCanNum].i32PeriphCan, CAN_NORMAL_MODE);

    can_struct_para_init(CAN_MDSC_STRUCT, &stCan);
    stCan.rtr     = 0U;                                 //接收数据帧
    stCan.ide     = 1U;                                 //帧格式为扩展帧
    stCan.code    = CAN_MB_RX_STATUS_EMPTY;             //邮箱状态设置为EMPTY,激活邮箱接收功能
    stCan.id      = u32IdMask;
    can_mailbox_config(stCanDrvDevMap[eCanNum].i32PeriphCan, stCanDrvDevMap[eCanNum].stMailBox.i32RxMailBoxIndex, &stCan);
}











