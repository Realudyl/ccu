/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   fault_chk.c
* Description                           :   fault check 故障告警检查实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#ifndef _fault_chk_H_
#define _fault_chk_H_
#include "fault.h"

#include "card_app.h"
#include "io_app.h"
#include "meter_drv_opt.h"






//故障检测注册最大数量
#define cPileFaultChkNumMax             (256)





//故障注册表
typedef struct
{
    i32                                 i32CmdNum;
    void                                (*pFunc[cPileFaultChkNumMax])(void);
}stFaultChkMap_t;























extern bool sFaultChkInit(void);



extern void sFaultChkRegister(void (*pFunc)(void));





#endif






























