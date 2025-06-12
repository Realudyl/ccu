/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_backtrace.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-8
 * @Attention             :     
 * @Brief                 :     CmBackTrace驱动 ———— 根据以往工程整理
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-8
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _en_backtrace_H
#define _en_backtrace_H
#include "gd32h7xx_libopt.h"

#include "en_common.h"
















extern bool sCmBackTraceInit(const char *pFirmwareName);


//有关cmbacktrace故障追踪打印
extern void sCmBackTracePrint(const char *fmt, ...);
















#endif
