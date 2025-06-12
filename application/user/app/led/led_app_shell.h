/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_app_shell.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-23
 * @Attention             :     
 * @Brief                 :     LED灯语shell命令
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-23
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _led_app_shell_H
#define _led_app_shell_H

#include "en_shell.h"

#include "led_app.h"










typedef struct
{
    char                               *pString;
    eLedColor_t                         eLedColor;
}stLedColorShell_t;




typedef struct
{
    char                               *pString;
    eLedSignalType_t                    eLedSignalType;
}stLedSignalTypeShell_t;








extern bool sLedAppShellRegister(void);










#endif
