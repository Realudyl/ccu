/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   order_top_down_1.c
* Description                           :   充电订单计量计费管理 之 尖峰平谷费率1
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
#ifndef _order_top_down_1_H_
#define _order_top_down_1_H_
#include "en_common.h"













#define cOrderTopDown1TimeSection       (cPrivDrvRateTimeSection)               //尖峰平谷模型最大时段数













extern void sOrderProcTopDown1Start(ePileGunIndex_t eGunIndex);
extern void sOrderProcTopDown1Run  (ePileGunIndex_t eGunIndex);
extern void sOrderProcTopDown1Stop (ePileGunIndex_t eGunIndex);








#endif





























