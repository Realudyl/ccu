/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_factory_data_def.h
* Description                           :   eeprom芯片用户程序实现 之 充电桩参数配置
* Version                               :   
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-23
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_factory_data_def_H_
#define _eeprom_app_factory_data_def_H_

#include "en_common.h"
#include "en_log.h"























//电子锁类型
typedef enum
{
    eELockType_Pulse                    = 0,                                    //脉冲锁
    eELockType_Level                    = 1,                                    //电平锁
    eElockTypeMax,
}eEepromAppELockType_t;


















#endif


