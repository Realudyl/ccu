/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     usb_app.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-8-28
 * @Attention             :     
 * @Brief                 :     USB用户程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-8-28
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _usb_app_H
#define _usb_app_H
#include <stdio.h>

#include "usb_drv.h"

#include "ff.h"







//USB用户程序状态机枚举
typedef enum
{
    eStateFsInit                        = 0,
    eStateFsReadList,                   //1
    eStateFsWriteFile,                  //2
    eStateFsFinished,                   //3
    
    eStateFsMax
}eUsbhUserAppState_t;





//USB用户程序缓存结构体
typedef struct
{
    eUsbhUserAppState_t                 eState;
    
    FATFS                               stFatfs;
    FIL                                 stFile;
    
}stUsbhAppCache_t;






extern bool sUsbAppOpen(void);





#endif

