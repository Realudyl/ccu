/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     io_app_fault.h
* Description                           :     输入输出信号 之 故障逻辑
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-09-12
* notice                                :     
****************************************************************************************************/
#ifndef _io__app_fault_h_
#define _io__app_fault_h_
#include "fault.h"
#include "private_drv_opt.h"











//辅助电源反馈异常检测时间(ms)
#define cPowerFaultDetectTime           (5000)

//直流接触器反馈异常检测时间(ms)
#define cDcContactFaultDetectTime       (500)

































extern void sIoAppFaultCheck_CDO(void);
extern void sIoAppFaultCheck_ESBP(void);
extern void sIoAppFaultCheck_SPD(void);
extern void sIoAppFaultCheck_DCF(void);
extern void sIoAppFaultCheck_CAE(void);
extern void sIoAppFaultCheck_DCX(void);
extern void sIoAppFaultCheck_DCY(void);
extern void sIoAppFaultCheck_DCZ(void);


#endif






















