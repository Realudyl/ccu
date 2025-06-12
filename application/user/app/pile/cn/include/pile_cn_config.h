/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_config.h
* Description                           :   国标bms协议的一些配置信息
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-30
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_config_h_
#define _pile_cn_config_h_

#include "can_drv.h"





//端口配置
#ifndef cSdkEvcc01CanPort
#define cEvcc01CanPort                     (eCanNum1)
#else
#define cEvcc01CanPort                     (cSdkEvcc01CanPort)
#endif

#ifndef cSdkEvcc01CanBaudrate
#define cEvcc01CanBaudrate                 (250000)
#else
#define cEvcc01CanBaudrate                 (cSdkEvcc01CanBaudrate)
#endif




#ifndef cSdkEvcc02CanPort
#define cEvcc02CanPort                     (eCanNum2)
#else
#define cEvcc02CanPort                     (cSdkEvcc02CanPort)
#endif

#ifndef cSdkEvcc02CanBaudrate
#define cEvcc02CanBaudrate                 (250000)
#else
#define cEvcc02CanBaudrate                 (cSdkEvcc02CanBaudrate)
#endif









#endif


