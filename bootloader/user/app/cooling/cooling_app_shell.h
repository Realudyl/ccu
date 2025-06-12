/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   cooling_app_shell.h
* Description                           :   液冷机通讯 shell命令
* Version                               :   V1.0.0
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-06-21
* notice                                :   
*                                           
****************************************************************************************************/
#ifndef _cooling_app_shell_h_
#define _cooling_app_shell_h_

#include "en_common.h"
#include "en_shell.h"
#include "en_log.h"


















typedef enum
{
    eCmdSetAddr                         = 0,
    eCmdSetBaud                         = 1,
    eCmdSetMode                         = 2,
    eCmdSetLvTemp                       = 3,
    eCmdSetPSpeed                       = 4,
    eCmdSetFSpeed                       = 5,
    eCmdSetHighTep                      = 6,
    eCmdGetSysInfo                      = 7,
    eCmdMax,
}eCoolingAppShellCmd_t;







extern void sCoolingAppShellInit(void);


#endif



