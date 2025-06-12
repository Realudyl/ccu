/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     usb_drv.h
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
#ifndef _usb_drv_H
#define _usb_drv_H

#include "en_common.h"

#include "usbh_msc_core.h"








#define TIM_MSEC_DELAY                          0x01U
#define TIM_USEC_DELAY                          0x02U

#ifndef USE_ULPI_PHY

#ifdef USE_USBHS0
//#define HOST_POWERSW_PORT_RCC                   RCU_GPIOA
//#define HOST_POWERSW_PORT                       GPIOA
//#define HOST_POWERSW_VBUS                       GPIO_PIN_6
#endif

#ifdef USE_USBHS1
//#define HOST_POWERSW_PORT_RCC                   RCU_GPIOB
//#define HOST_POWERSW_PORT                       GPIOB
//#define HOST_POWERSW_VBUS                       GPIO_PIN_2
#endif

#else

#ifdef USE_USBHS0
//#define HOST_POWERSW_PORT_RCC                   RCU_GPIOC
//#define HOST_POWERSW_PORT                       GPIOC
//#define HOST_POWERSW_VBUS                       GPIO_PIN_7
#endif

#ifdef USE_USBHS1
//#define HOST_POWERSW_PORT_RCC                   RCU_GPIOC
//#define HOST_POWERSW_PORT                       GPIOC
//#define HOST_POWERSW_VBUS                       GPIO_PIN_9
#endif 

#endif

//#define HOST_SOF_OUTPUT_RCC                     RCC_APB2PERIPH_GPIOA
//#define HOST_SOF_PORT                           GPIOA
//#define HOST_SOF_SIGNAL                         GPIO_PIN_8








extern bool sUsbDrvInit(usbh_user_cb *pUsbhUsrCb);



#endif

