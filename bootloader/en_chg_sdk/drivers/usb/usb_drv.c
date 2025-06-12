/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     usb_drv.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-8-28
 * @Attention             :     
 * @Brief                 :     USB驱动程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-8-28
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 移植自GD官方USB例程<GD32H7xx_Firmware_Library_V1.2.0\Examples\USBHS\usb_host\usb_host_msc_udisk\src\gd32h7xx_usb_hw.c>
 * 
******************************************************************************/
#include "usb_drv.h"

#include "drv_usb_regs.h"
#include "drv_usb_hw.h"
#include "drv_usbh_int.h"











//这两个结构体由用户程序进行全局定义
extern usbh_host usb_host_msc;
extern usb_core_driver msc_host_core;




__IO uint32_t delay_time = 0U;









bool sUsbDrvInit(usbh_user_cb *pCallbackList);


/* local function prototypes ('static') */
static void hw_time_set (uint8_t unit);
static void hw_delay    (uint32_t ntime, uint8_t unit);











/*******************************************************************************
 * @FunctionName   :      sUsbDrvInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  19:30:53
 * @Description    :      USB驱动初始化
 * @Input          :      pCallbackList     USB回调函数列表指针
 * @Return         :      
*******************************************************************************/
bool sUsbDrvInit(usbh_user_cb *pCallbackList)
{
    usb_rcu_config();
    
    usb_timer_init();
    
    /* configure GPIO pin used for switching VBUS power and charge pump I/O */
//    usb_vbus_config();
    
    /* register device class */
    usbh_class_register(&usb_host_msc, &usbh_msc);
    
#ifdef USE_USBHS0
    #ifdef USE_USB_FS
        usb_para_init (&msc_host_core, USBHS0, USB_SPEED_FULL);
    #endif
    
    #ifdef USE_USB_HS
        usb_para_init (&msc_host_core, USBHS0, USB_SPEED_HIGH);
    #endif
#endif /* USE_USBHS0 */
    
#ifdef USE_USBHS1
    #ifdef USE_USB_FS
        usb_para_init (&msc_host_core, USBHS1, USB_SPEED_FULL);
    #endif
    
    #ifdef USE_USB_HS
        usb_para_init (&msc_host_core, USBHS1, USB_SPEED_HIGH);
    #endif
#endif /* USE_USBHS1 */
    
    usbh_init(&usb_host_msc, &msc_host_core, pCallbackList);
    
#ifdef USE_USB_HS
    #ifndef USE_ULPI_PHY
        #ifdef USE_USBHS0
            pllusb_rcu_config(USBHS0);
        #elif defined USE_USBHS1
            pllusb_rcu_config(USBHS1);
        #else
            
        #endif
    #endif /* !USE_ULPI_PHY */
#endif /* USE_USB_HS */
    
    usb_intr_config();
    
    return true;
}







/*!
    \brief      configure USB GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config (void)
{
#ifdef USE_USBHS0
//    rcu_periph_clock_enable(RCU_GPIOA);
//    rcu_periph_clock_enable(RCU_GPIOB);
//    rcu_periph_clock_enable(RCU_GPIOC);
//    rcu_periph_clock_enable(RCU_GPIOF);
//
//    /* ULPI_STP(PC0) GPIO pin configuration */
//    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
//    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0);
//
//    /* ULPI_CK(PA5) GPIO pin configuration */
//    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
//    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_5);
//
//    /* ULPI_NXT(PC3) GPIO pin configuration */
//    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
//    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);
//
//    /* ULPI_DIR(PI11) GPIO pin configuration */
//    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
//    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2);
//
//    /* ULPI_D1(PB0), ULPI_D2(PB1), ULPI_D3(PB10), ULPI_D4(PB11) \
//       ULPI_D5(PB12), ULPI_D6(PB13) and ULPI_D7(PB5) GPIO pin configuration */
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, \
//                  GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, \
//                            GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
//
//    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, \
//                    GPIO_PIN_2 |\
//                    GPIO_PIN_1 | GPIO_PIN_0);
//    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, \
//                    GPIO_PIN_2   |\
//                    GPIO_PIN_1 | GPIO_PIN_0);
//
//    /* ULPI_D0(PA3) GPIO pin configuration */
//    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
//    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);
//    
//
//    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_1);
//    gpio_af_set(GPIOC, GPIO_AF_10, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3);
//    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_3);
//    gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_5  |\
//                                    GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
#endif

#ifdef USE_USBHS1 
//    rcu_periph_clock_enable(RCU_GPIOH);
//    rcu_periph_clock_enable(RCU_GPIOG);
//
//    /* ULPI_STP(PH2) GPIO pin configuration */
//    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
//    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_2);
//
//    /* ULPI_CK(PH5) GPIO pin configuration */
//    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
//    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_5);
//
//    /* ULPI_NXT(PH4) GPIO pin configuration */
//    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
//    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_4);
//
//    /* ULPI_DIR(PH3) GPIO pin configuration */
//    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
//    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);
//
//    /* ULPI_D0(PH6) GPIO pin configuration */
//    /* ULPI_D1(PH7), ULPI_D2(PH8), ULPI_D3(PH9), ULPI_D4(PH10) \
//       ULPI_D5(PH11), ULPI_D6(PH12) and ULPI_D7(PG5) GPIO pin configuration */
//    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, \
//                    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |\
//                    GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
//    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, \
//                    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |\
//                    GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
//
//
//    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
//    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_5);
//
//    gpio_af_set(GPIOG, GPIO_AF_8, GPIO_PIN_5);
//    gpio_af_set(GPIOH, GPIO_AF_8, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |\
//                                   GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8| GPIO_PIN_9| GPIO_PIN_10| GPIO_PIN_11| GPIO_PIN_12);
#endif
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void)
{
    pmu_usb_regulator_enable();
    pmu_usb_voltage_detector_enable();
    while (pmu_flag_get(PMU_FLAG_USB33RF) != SET)
    {
    }

#ifdef USE_USB_FS

#ifndef USE_IRC48M

    /* configure the pll1 input and output clock range */
    rcu_pll_input_output_clock_range_config(IDX_PLL1,RCU_PLL1RNG_4M_8M,RCU_PLL1VCO_192M_836M);

    rcu_pll1_config(5, 96, 2, 10, 2);
    /* enable PLL1Q clock output */
    rcu_pll_clock_output_enable(RCU_PLL1Q);
    rcu_osci_on(RCU_PLL1_CK);

#ifdef USE_USBHS0
    rcu_usbhs_pll1qpsc_config (IDX_USBHS0, RCU_USBHSPSC_DIV1);

    rcu_usb48m_clock_config(IDX_USBHS0, RCU_USB48MSRC_PLL1Q);
#endif /* USE_USBHS0 */

#ifdef USE_USBHS1
    rcu_usbhs_pll1qpsc_config (IDX_USBHS1, RCU_USBHSPSC_DIV1);

    rcu_usb48m_clock_config(IDX_USBHS1, RCU_USB48MSRC_PLL1Q);
#endif /* USE_USBHS1 */

#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);

#ifdef USE_USBHS0
    rcu_usb48m_clock_config(IDX_USBHS0, RCU_USB48MSRC_IRC48M);
#endif /* USE_USBHS0 */

#ifdef USE_USBHS1
    rcu_usb48m_clock_config(IDX_USBHS1, RCU_USB48MSRC_IRC48M);
#endif /* USE_USBHS1 */

#endif /* USE_IRC48M */

#endif /* USE_USB_FS */

#ifdef USE_USBHS0
    rcu_periph_clock_enable(RCU_USBHS0);
#endif /* USE_USBHS0 */

#ifdef USE_USBHS1
    rcu_periph_clock_enable(RCU_USBHS1);
#endif /* USE_USBHS1 */

#ifdef USE_ULPI_PHY
#ifdef USE_USBHS0
    rcu_periph_clock_enable(RCU_USBHS0ULPI);
#endif
    
#ifdef USE_USBHS1
    rcu_periph_clock_enable(RCU_USBHS1ULPI);
#endif
#endif /* USE_ULPI_PHY */
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config(void)
{
#ifdef USE_USBHS0
    nvic_irq_enable((uint8_t)USBHS0_IRQn, 3U, 0U);
#endif /* USE_USBHS0 */

#ifdef USE_USBHS1
    nvic_irq_enable((uint8_t)USBHS1_IRQn,3U, 0U);
#endif /* USE_USBHS0 */

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

#ifdef USE_USBHS0
    /* USB wakeup EXTI line configuration */
//    exti_interrupt_flag_clear(EXTI_31);
//    exti_init(EXTI_31, EXTI_INTERRUPT, EXTI_TRIG_RISING);
//    exti_interrupt_enable(EXTI_31);
//    
//    nvic_irq_enable((uint8_t)USBHS0_WKUP_IRQn, 3U, 1U);
#endif /* USE_USBHS0 */

#ifdef USE_USBHS1
    /* USB wakeup EXTI line configuration */
//    exti_interrupt_flag_clear(EXTI_32);
//    exti_init(EXTI_32, EXTI_INTERRUPT, EXTI_TRIG_RISING);
//    exti_interrupt_enable(EXTI_32);
//    
//    nvic_irq_enable((uint8_t)USBHS1_WKUP_IRQn, 3U, 1U);
#endif /* USE_USBHS1 */
}

/*!
    \brief      drives the VBUS signal through GPIO
    \param[in]  state: VBUS states
    \param[out] none
    \retval     none
*/
void usb_vbus_drive (uint8_t state)
{
//    if (0U == state)
//    {
//        /* disable is needed on output of the power switch */
//        gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
//    }
//    else
//    {
//        /* enable the power switch by driving the enable high */
//        gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
//    }
}

/*!
    \brief      configures the GPIO for the VBUS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_vbus_config (void)
{
//    rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);
//    
//    /* USBFS_VBUS_CTRL(PD13) GPIO pin configuration */
//    gpio_mode_set(HOST_POWERSW_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, HOST_POWERSW_VBUS);
//    gpio_output_options_set(HOST_POWERSW_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, HOST_POWERSW_VBUS);
//    
//    /* by default, disable is needed on output of the power switch */
//    usb_vbus_drive(0U);
//    
//    /* delay is need for stabilizing the VBUS low in reset condition, 
//     * when VBUS = 1 and reset-button is pressed by user 
//     */
//    usb_mdelay(200);
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init (void)
{
    /* enable the TIM2 global interrupt */
    nvic_irq_enable((uint8_t)TIMER3_IRQn, 1U, 0U);
    
    rcu_periph_clock_enable(RCU_TIMER3);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    hw_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milli seconds
    \param[in]  msec: value of delay required in milli seconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    hw_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      time base IRQ
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_irq (void)
{
    if (timer_interrupt_flag_get(TIMER3, TIMER_INT_UP) != RESET)
    {
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_UP);
        
        if (delay_time > 0x00U)
        {
            delay_time--;
        }
        else
        {
            timer_disable(TIMER3);
        }
    }
}

/*!
    \brief      delay routine based on TIMER2
    \param[in]  nTime: delay Time 
    \param[in]  unit: delay Time unit = mili sec / micro sec
    \param[out] none
    \retval     none
*/
static void hw_delay(uint32_t ntime, uint8_t unit)
{
    delay_time = ntime;
    
    hw_time_set(unit);
    
    while (0U != delay_time)
    {
    }
    
    timer_disable(TIMER3);
}

/*!
    \brief      configures TIMER2 for delay routine based on TIMER2
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hw_time_set(uint8_t unit)
{
    timer_parameter_struct  timer_basestructure;
    
    timer_disable(TIMER3);
    timer_interrupt_disable(TIMER3, TIMER_INT_UP);
    
    if (TIM_USEC_DELAY == unit)
    {
        timer_basestructure.period = (10 - 1);                                  //10M / 10 = 1M Hz      (1us)
    }
    else if(TIM_MSEC_DELAY == unit)
    {
        timer_basestructure.period = (10000 - 1);                               //10M / 10000 = 1K Hz   (1ms)
    }
    else
    {
        /* no operation */
    }
    
    timer_basestructure.prescaler         = (10 - 1);                           //SYSCLK = 400M; APB1 = 100M; APB1 / 10 = 10M Hz
    timer_basestructure.alignedmode       = TIMER_COUNTER_EDGE;
    timer_basestructure.counterdirection  = TIMER_COUNTER_UP;
    timer_basestructure.clockdivision     = TIMER_CKDIV_DIV1;
    timer_basestructure.repetitioncounter = 0U;
    
    timer_init(TIMER3, &timer_basestructure);
    
    timer_interrupt_flag_clear(TIMER3, TIMER_INT_UP);
    
    timer_auto_reload_shadow_enable(TIMER3);
    
    /* TIMER IT enable */
    timer_interrupt_enable(TIMER3, TIMER_INT_UP);
    
    /* TIMER enable counter */ 
    timer_enable(TIMER3);
}

/*!
    \brief      configure the PLL of USB
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pllusb_rcu_config(uint32_t usb_periph)
{
    if(USBHS0 == usb_periph)
    {
        rcu_pllusb0_config(RCU_PLLUSBHSPRE_HXTAL, RCU_PLLUSBHSPRE_DIV5, RCU_PLLUSBHS_MUL96, RCU_USBHS_DIV10);
        RCU_ADDCTL1 |= RCU_ADDCTL1_PLLUSBHS0EN;
        while((RCU_ADDCTL1 & RCU_ADDCTL1_PLLUSBHS0STB) == 0U)
        {
        }
        
        rcu_usb48m_clock_config(IDX_USBHS0, RCU_USB48MSRC_PLLUSBHS);
    }
    else
    {
        rcu_pllusb1_config(RCU_PLLUSBHSPRE_HXTAL, RCU_PLLUSBHSPRE_DIV5, RCU_PLLUSBHS_MUL96, RCU_USBHS_DIV10);
        RCU_ADDCTL1 |= RCU_ADDCTL1_PLLUSBHS1EN;
        while((RCU_ADDCTL1 & RCU_ADDCTL1_PLLUSBHS1STB) == 0U)
        {
        }
        
        rcu_usb48m_clock_config(IDX_USBHS1, RCU_USB48MSRC_PLLUSBHS);
    }
}





/*!
    \brief      this function handles USBHS interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
#ifdef USE_USBHS0

void USBHS0_IRQHandler (void)
{
    usbh_isr (&msc_host_core);
}

#endif /* USE_USBHS0 */

#ifdef USE_USBHS1

void USBHS1_IRQHandler (void)
{
    usbh_isr (&msc_host_core);
}

#endif /* USE_USBHS1 */



/*!
    \brief      this function handles Timer update interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER3_IRQHandler(void)
{
    usb_timer_irq();
}
