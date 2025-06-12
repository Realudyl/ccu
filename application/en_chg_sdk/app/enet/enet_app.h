/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     enet_app.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-9-9
 * @Attention             :     
 * @Brief                 :     以太网用户驱动实现程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-9-9
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _enet_app_H
#define _enet_app_H
#include "enet_drv.h"
#include "en_common.h"
#include "netif_gd32h7xx.h"




// DHCP最大重试次数
#define cDhcpRetryMax                   4




// DHCP状态枚举
typedef enum
{
    eDhcpStart                          = 0,
    eDhcpWaitAddress,
    eDhcpAddressAssigned,
    eDhcpTimeout,
    
    eDhcpStateMax
}eDhcpState_t;







extern bool sEnetAppOpen(i32 i32EnetDevIndex, bool bDttFlag, void (*pPlugInFunc)(struct netif *pNetif), void (*pPlugOutFunc)(struct netif *pNetif));


extern i8 sEnetAppPhyLinkStatusGet(i32 i32EnetDevIndex);
extern const char* sEnetAppNetifIpGet(i32 i32EnetDevIndex);
extern bool sEnetAppNetifMacAddrSet(i32 i32EnetDevIndex, const u8* pMacAddr);





#endif
