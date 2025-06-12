/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_shell.h
* Description                           :   国标充电桩shell命令
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-05-10
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_shell_h_
#define _pile_cn_shell_h_

#include "pile_cn.h"
#include "en_chg_sdk_cfg.h"

















extern bool sPileCnShellGunStopChg(const stShellPkt_t *pkg);
extern bool sPileCnShellDisCc1Link(const stShellPkt_t *pkg);
extern bool sPileCnShellResumeCc1Link(const stShellPkt_t *pkg);





#endif



