/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   hmi_drv_dwin._api.h
* Description                           :   迪文显示屏串口通讯 驱动 api接口函数实现
* Version                               :   
* Author                                :   hh
* Creat Date                            :   2024-03-19
* notice                                :   使用DMG80480T070_05WTC迪文屏幕，迪文自定串口协议 分辨率800x480
*                                           使用迪文自定义协议
****************************************************************************************************/

#ifndef _hmi_drv_dwin_api_H_
#define _hmi_drv_dwin_api_H_

#include "hmi_drv_dwin.h"









extern bool sHmiDwinDrvSetPage(u16 u16page);
extern bool sHmiDwinDrvGetDataIdBtnessRtc(u16 *Id, u8 *Brightness, stTime_t *Rtc);
extern i32 sHmiDwinDrvGetData(i32 i32MaxLen, u8 *pBuf);


#endif
















