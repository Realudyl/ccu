/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   model_dc_cn_en_480kw.h
* Description                           :   国标480kw分体桩机型代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-26
* notice                                :   
****************************************************************************************************/
#ifndef _model_dc_cn_en_480kw_h_
#define _model_dc_cn_en_480kw_h_
#include "en_common.h"
#include "en_shell.h"
#include "en_mem.h"

#include "adc_app_sample.h"
#include "card_app.h"
#include "eeprom_app.h"
#include "enet_app.h"
#include "fault_chk.h"
#include "flash_drv_gd5f4gm8.h"
#include "fwdog.h"
#include "io_app.h"
#include "io_pwm_app.h"
#include "iso.h"
#include "mod_app_opt.h"
#include "meter_drv_opt.h"
#include "order.h"
#include "private_drv_opt.h"
#include "rtc_drv_interface.h"
#include "pile_cn.h"
#include "boot.h"
#include "cooling_app.h"







//液冷终端配置开关
#ifdef DC480K_G2SF312_250
#define cModelSuperChargerFlag          false
#elif defined(DC480K_G2SF311_600)
#define cModelSuperChargerFlag          true
#endif




























extern bool sModelInit_dc_cn_en_480k(void);
extern void sModelInit_dc_cn_en_480k_eeprom(void);











#endif




















