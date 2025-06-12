/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv_trans.h
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
#ifndef _can_drv_trans_H
#define _can_drv_trans_H
#include "can_drv.h"
#include "can_drv_buff.h"










//传输单包数据的超时时间 —— 以50kbps,传输数据为扩展帧 为例
#define cCanDrvTransDataTimeoutCnt      (353000)                                   //353000 理论约3ms 3000us










extern bool sCanDrvOpen(eCanNum_t eCanNum, u32 u32BaudRate);
extern bool sCanDrvRecv(eCanNum_t eCanNum, stCanMsg_t *pMsg);
extern bool sCanDrvSend(eCanNum_t eCanNum, const stCanMsg_t *pMsg);























#endif

