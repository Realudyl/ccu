/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     private_drv_op_s.h
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-05-27
* notice                                :     
****************************************************************************************************/
#ifndef _private_drv_op_s_H_
#define _private_drv_op_s_H_
#include "en_common.h"
#include "private_drv_frame_def.h"










//各报文发送周期持续的时间
#ifndef cSdkPrivDrvCycleFastTime
#define cPrivDrvCycleFastTime           (180)
#else
#define cPrivDrvCycleFastTime           (cSdkPrivDrvCycleFastTime)
#endif








#if   (cSdkPrivDevType == cSdkPrivDevTypeS)                                     //Slave  侧有周期性主动发送报文
extern void sPrivDrvSendTask(void *pParam);
extern u8   sPrivDrvGetGunId(ePileGunIndex_t eGunIndex);
extern bool sPrivDrvSetCallCmd(u8 u8GunId, ePrivDrvCmd_t eCmd);
extern u32 sPrivDrvGetToatlEnergy(unPrivDrvMoney_t *pMoney);

#endif







#endif























