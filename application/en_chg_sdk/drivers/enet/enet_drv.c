/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     enet_drv.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-9-9
 * @Attention             :     
 * @Brief                 :     以太网驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-9-9
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "enet_drv.h"









extern stEnetDrvDevice_t stEnetDrvDevMap[eEnetNumMax];

extern xSemaphoreHandle gd32h7xx_rx_semaphore[2];







bool sEnetDrvStatusRegGet(eEnetNum_t eEnetNum, u16* pStatusValue);
bool sEnetDrvMacAddrSet(eEnetNum_t eEnetNum, u8* pMacAddr);

bool sEnetDrvSendData(eEnetNum_t eEnetNum, u8* pBuf, u32 u32Len);


bool sEnetDrvInitDevInit(eEnetNum_t eEnetNum);


bool sEnetDrvInit(eEnetNum_t eEnetNum);
bool sEnetDrvInitDevRcu(eEnetNum_t eEnetNum);
bool sEnetDrvInitDevGpio(eEnetNum_t eEnetNum);
bool sEnetDrvInitDevNvic(eEnetNum_t eEnetNum);
bool sEnetDrvInitDevDma(eEnetNum_t eEnetNum);
bool sEnetDrvInitDevEnet(eEnetNum_t eEnetNum);


void mpu_config(void);


void ENET0_IRQHandler(void);
void ENET1_IRQHandler(void);









/*******************************************************************************
 * @FunctionName   :      sEnetDrvStatusRegGet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月19日  10:00:24
 * @Description    :      ENET驱动 API函数 ———— 设备状态寄存器获取
 * @Input          :      eEnetNum    
 * @Input          :      pStatusValue
 * @Return         :      
*******************************************************************************/
bool sEnetDrvStatusRegGet(eEnetNum_t eEnetNum, u16* pStatusValue)
{
    stEnetDrvDevice_t *pDevice = NULL;
    
    if(eEnetNum >= eEnetNumMax)
    {
        //ENET接口超限
        return false;
    }
    
    pDevice = &stEnetDrvDevMap[eEnetNum];
    if(pDevice->i32PeriphEnet < 0)
    {
        //ENET接口未启用
        return false;
    }
    
    return enet_phy_write_read(pDevice->i32PeriphEnet, ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, pStatusValue);
}




/*******************************************************************************
 * @FunctionName   :      sEnetDrvMacAddrSet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月25日  11:37:31
 * @Description    :      ENET驱动 API函数 ———— MAC地址设置
 * @Input          :      eEnetNum    
 * @Input          :      pMacAddr    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvMacAddrSet(eEnetNum_t eEnetNum, u8* pMacAddr)
{
    stEnetDrvDevice_t *pDevice = NULL;
    enet_macaddress_enum eMacAddrFilter;
    
    if((eEnetNum >= eEnetNumMax) || (pMacAddr == NULL))
    {
        return false;
    }
    
    pDevice = &stEnetDrvDevMap[eEnetNum];
    if(pDevice->i32PeriphEnet < 0)
    {
        //ENET接口未启用
        return false;
    }
    
    switch (eEnetNum)
    {
    case eEnetNum0:
        eMacAddrFilter = ENET_MAC_ADDRESS0;
        break;
    case eEnetNum1:
        eMacAddrFilter = ENET_MAC_ADDRESS1;
        break;
    default:
        break;
    }
    
    /* initialize MAC address in ethernet MAC */
    enet_mac_address_set(pDevice->i32PeriphEnet, eMacAddrFilter, pMacAddr);
    
    return true;
}




/*******************************************************************************
 * @FunctionName   :      sEnetDrvSendData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年10月9日  15:05:22
 * @Description    :      ENET驱动 API函数 ———— 发送数据
 * @Input          :      eEnetNum    
 * @Input          :      pBuf        
 * @Input          :      u32Len      
 * @Return         :      
*******************************************************************************/
bool sEnetDrvSendData(eEnetNum_t eEnetNum, u8* pBuf, u32 u32Len)
{
    stEnetDrvDevice_t *pDevice = NULL;
    
    if((eEnetNum >= eEnetNumMax) || (pBuf == NULL) || (u32Len == 0))
    {
        return false;
    }
    
    pDevice = &stEnetDrvDevMap[eEnetNum];
    if(pDevice->i32PeriphEnet < 0)
    {
        //ENET接口未启用
        return false;
    }
    
    return enet_frame_transmit(pDevice->i32PeriphEnet, pBuf, u32Len);
}





/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月19日  14:50:21
 * @Description    :      ENET驱动 API函数 ———— 设备初始化
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevInit(eEnetNum_t eEnetNum)
{
    stEnetDrvDevice_t *pDevice = NULL;
    
    if(eEnetNum >= eEnetNumMax)
    {
        //ENET接口超限
        return false;
    }
    
    pDevice = &stEnetDrvDevMap[eEnetNum];
    if(pDevice->i32PeriphEnet < 0)
    {
        //ENET接口未启用
        return false;
    }
    
    if(ERROR == enet_init(pDevice->i32PeriphEnet, pDevice->i32MediaModeConf, pDevice->i32CheckSumConf, pDevice->i32FramFiltConf))
    {
        return false;
    }
    
    return true;
}





/*******************************************************************************
 * @FunctionName   :      sEnetDrvInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月6日  18:14:03
 * @Description    :      ENET驱动 初始化
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInit(eEnetNum_t eEnetNum)
{
    bool bRst = true;
    stEnetDrvDevice_t *pDevice = NULL;
    
    if(eEnetNum >= eEnetNumMax)
    {
        //ENET接口超限
        bRst = false;
        return bRst;
    }
    
    pDevice = &stEnetDrvDevMap[eEnetNum];
    if(pDevice->i32PeriphEnet < 0)
    {
        //ENET接口未启用
        bRst = false;
        return bRst;
    }
    
    /* configure the MPU */
    mpu_config();
    
    //1:时钟信号初始化
    bRst &= sEnetDrvInitDevRcu(eEnetNum);
    
    //2:GPIO初始化
    bRst &= sEnetDrvInitDevGpio(eEnetNum);
    
    //3:中断配置
    bRst &= sEnetDrvInitDevNvic(eEnetNum);
    
    //4:SPI外设初始化
    bRst &= sEnetDrvInitDevEnet(eEnetNum);
    
    //5:DMA初始化
    bRst &= sEnetDrvInitDevDma(eEnetNum);
    
    return bRst;
}




/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevRcu
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月6日  18:14:25
 * @Description    :      ENET驱动 设备初始化 之 RCU使能
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevRcu(eEnetNum_t eEnetNum)
{
    i8 i;
    stEnetDrvDevice_t *pDevice = &stEnetDrvDevMap[eEnetNum];
    
    if((pDevice->i32PeriphRcuEnet < 0) || (pDevice->i32PeriphRcuEnetTx < 0) || (pDevice->i32PeriphRcuEnetRx < 0)
    || (pDevice->i32CkOut0Src < 0) || (pDevice->i32CkOut0Div < 0))
    {
        return false;
    }
    
    /* enable gpio clock  */
    for(i = 0; i < eEnetMax; i++)
    {
        if(pDevice->stGpio[i].i32PeriphRcu < 0)
        {
            return false;
        }
        rcu_periph_clock_enable(pDevice->stGpio[i].i32PeriphRcu);
    }
    
    /* enable ethernet clock  */
    rcu_periph_clock_enable(pDevice->i32PeriphRcuEnet);
    rcu_periph_clock_enable(pDevice->i32PeriphRcuEnetTx);
    rcu_periph_clock_enable(pDevice->i32PeriphRcuEnetRx);
    
    /* choose DIV12 to get 50MHz from 600MHz on CKOUT0 pin (PA1) to clock the PHY */
    rcu_ckout0_config(pDevice->i32CkOut0Src, pDevice->i32CkOut0Div);
    
    return true;
}



/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevGpio
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月6日  18:14:29
 * @Description    :      ENET驱动 设备初始化 之 GPIO配置
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevGpio(eEnetNum_t eEnetNum)
{
    i8 i;
    stEnetDrvDevice_t *pDevice = &stEnetDrvDevMap[eEnetNum];
    
    for(i = 0; i < eEnetMax; i++)
    {
        if(pDevice->stGpio[i].i32PeriphRcu > 0)
        {
            gpio_af_set(pDevice->stGpio[i].i32Periph, pDevice->stGpio[i].i32Af, pDevice->stGpio[i].i32Pin);
            gpio_mode_set(pDevice->stGpio[i].i32Periph, pDevice->stGpio[i].i32Mode, pDevice->stGpio[i].i32Pull, pDevice->stGpio[i].i32Pin);
            gpio_output_options_set(pDevice->stGpio[i].i32Periph, GPIO_OTYPE_PP, pDevice->stGpio[i].i32Speed, pDevice->stGpio[i].i32Pin);
        }
    }
    
    return true;
}



/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevNvic
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月6日  18:14:32
 * @Description    :      ENET驱动 设备初始化 之 中断使能
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevNvic(eEnetNum_t eEnetNum)
{
    stEnetDrvDevice_t *pDevice = &stEnetDrvDevMap[eEnetNum];
    
    if(pDevice->i32Irq > 0)
    {
        nvic_irq_enable(pDevice->i32Irq, 2, 0);
    }
    
    return true;
}



/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevDma
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月6日  18:14:35
 * @Description    :      ENET驱动 设备初始化 之 DMA使能
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevDma(eEnetNum_t eEnetNum)
{
    stEnetDrvDevice_t *pDevice = &stEnetDrvDevMap[eEnetNum];
    
    if(pDevice->bDmaEnable == true)
    {
        enet_interrupt_enable(pDevice->i32PeriphEnet, ENET_DMA_INT_NIE);
        enet_interrupt_enable(pDevice->i32PeriphEnet, ENET_DMA_INT_RIE);
    }
    
    return true;
}



/*******************************************************************************
 * @FunctionName   :      sEnetDrvInitDevEnet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月9日  9:11:13
 * @Description    :      ENET驱动 设备初始化 之 ENET外设初始化
 * @Input          :      eEnetNum    
 * @Return         :      
*******************************************************************************/
bool sEnetDrvInitDevEnet(eEnetNum_t eEnetNum)
{
    stEnetDrvDevice_t *pDevice = &stEnetDrvDevMap[eEnetNum];
    u16 u16StatusValue = 0;
    
    /* configure the PHY interface for the ethernet MAC */
    syscfg_enet_phy_interface_config(pDevice->i32PeriphEnet, pDevice->i32PhyInterface);
    
    /* reset ethernet on AHB bus */
    enet_deinit(pDevice->i32PeriphEnet);
    
    if(ERROR == enet_software_reset(pDevice->i32PeriphEnet))
    {
        return false;
    }
    
    
    if(sEnetDrvStatusRegGet(eEnetNum, &u16StatusValue))
    {
        if(u16StatusValue & cEnetPhyLinkStatus)
        {
            if(ERROR == enet_init(pDevice->i32PeriphEnet, pDevice->i32MediaModeConf, pDevice->i32CheckSumConf, pDevice->i32FramFiltConf))
            {
                return false;
            }
        }
    }
    
    return true;
}



/*!
    \brief      configure the MPU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);
    
    /* disable the MPU */
    ARM_MPU_SetRegion(0U, 0U);
    
    /* Configure the DMA descriptors and Rx/Tx buffer*/
    mpu_init_struct.region_base_address = 0x30000000;
    mpu_init_struct.region_size = MPU_REGION_SIZE_16KB;
    mpu_init_struct.access_permission = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();
    
    /* Configure the LwIP RAM heap */
    mpu_init_struct.region_base_address = 0x30004000;
    mpu_init_struct.region_size = MPU_REGION_SIZE_16KB;
    mpu_init_struct.access_permission = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type = MPU_TEX_TYPE1;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();
    
    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}






/*!
    \brief      this function handles ethernet interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ENET0_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    /* frame received */
    if(SET == enet_interrupt_flag_get(ENET0, ENET_DMA_INT_FLAG_RS))
    {
        /* give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(gd32h7xx_rx_semaphore[0], &xHigherPriorityTaskWoken);
    }
    
    /* clear the enet DMA Rx interrupt pending bits */
    enet_interrupt_flag_clear(ENET0, ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET0, ENET_DMA_INT_FLAG_NI_CLR);
    
    /* switch tasks if necessary */
    if(pdFALSE != xHigherPriorityTaskWoken)
    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}



/*!
    \brief      this function handles ethernet interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ENET1_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    /* frame received */
    if(SET == enet_interrupt_flag_get(ENET1, ENET_DMA_INT_FLAG_RS))
    {
        /* give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(gd32h7xx_rx_semaphore[1], &xHigherPriorityTaskWoken);
    }
    
    /* clear the enet DMA Rx interrupt pending bits */
    enet_interrupt_flag_clear(ENET1, ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET1, ENET_DMA_INT_FLAG_NI_CLR);
    
    /* switch tasks if necessary */
    if(pdFALSE != xHigherPriorityTaskWoken)
    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}




