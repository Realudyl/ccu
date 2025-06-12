/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     boot.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-6-12
 * @Attention             :     
 * @Brief                 :     Bootloader主文件
 * 
 * @History:
 * 
 * 1.@Date: 2024-6-12
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _boot_H
#define _boot_H
#include "eeprom_app.h"
#include "led_app.h"









extern void sBootOtaInit(void);

extern void sBootOtaJumpToApp(void);











#endif
