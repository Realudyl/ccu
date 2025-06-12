/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_pcu_fault.h
* Description                           :   PCU相关故障实现 & 模块相关故障实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-09-12
* notice                                :   
****************************************************************************************************/
#ifndef _mod_app_opt_fault_h_
#define _mod_app_opt_fault_h_
#include "fault_chk.h"








//故障标志位和模块类型挂钩,永联模块故障位4字节,EN故障位6字节,且不同位故障涵义不一样,
#define cWinLineErrorCodeMask           0xdb8343bb
#define cWinLineScpErrCode              0x10000000                              //永联模块短路故障位







































extern void sModAppOptPcuFault_MCE(void);
extern void sModAppOptPcuFault_CPCE(void);
extern void sModAppOptNoneMode_NME(void);
extern void sModAppOptModeFault_MDE(void);
extern void sModAppOptModeFault_OV(void);
extern void sModAppOptModeFault_UV(void);





#endif
















