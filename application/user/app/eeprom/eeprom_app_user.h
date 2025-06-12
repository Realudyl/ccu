/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_user.h
* Description                           :   eeprom芯片用户程序实现 之 来自用户的配置参数结构
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-25
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_user_H_
#define _eeprom_app_user_H_























//来自运营商的出厂参数结构定义
typedef struct
{
    u16                                 u16CheckCrc;                            //校验值
    u8                                  u8Rsvd[512];                            //预留字段
    
}__attribute__((aligned(2))) stEepromAppBlockUserInfo_t;

//u16CheckCrc 字段 size
#define cEepromAppBlockUserCrcSize      (sizeof(u16))












#endif
















