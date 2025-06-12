/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_opt.h
* Description                           :   模块电源管理用户代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-09-12
* notice                                :   
****************************************************************************************************/
#ifndef _mod_app_opt_h_
#define _mod_app_opt_h_
#include "mod_app_avg.h"
#include "mod_app_flex.h"
#include "mod_app_pcu.h"
#include "mod_app_opt_fault.h"












//模块电源管理 类型定义
typedef enum 
{
    eModAppTypeAvg                      = 0,                                    //均充算法
    eModAppTypeFlex                     = 1,                                    //柔性分配算法
    eModAppTypePcu                      = 2,                                    //由Pcu管理
    
    eModAppTypeMax
}eModAppType_t;






//模块电源管理 功能映射
typedef struct
{
    //类型
    eModAppType_t                       eType;
    
    
    //一系列api操作接口
    bool                                (*pOpen)(ePileGunIndex_t eGunIndex);
    bool                                (*pClose)(ePileGunIndex_t eGunIndex);
    bool                                (*pCloseContactor)(void);
    bool                                (*pSetOutput)(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
    bool                                (*pGetUdcMax)(ePileGunIndex_t eGunIndex, i32 *pUdc);
    bool                                (*pGetIdcMax)(ePileGunIndex_t eGunIndex, i32 *pIdc);
    bool                                (*pGetUdcNow)(ePileGunIndex_t eGunIndex, i32 *pUdc);
    bool                                (*pGetIdcMaxAct)(ePileGunIndex_t eGunIndex, i32 *pIdc);
    
}stModAppOpt_t;


























extern bool sModAppOptInit(eModAppType_t eType, u32 u3P1, u32 u3P2, bool bP3);

extern bool sModAppOptOpen(ePileGunIndex_t eGunIndex);
extern bool sModAppOptClose(ePileGunIndex_t eGunIndex);
extern bool sModAppOptCloseContactor(void);
extern bool sModAppOptSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
extern bool sModAppOptGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
extern bool sModAppOptGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
extern bool sModAppOptGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);



#endif


























