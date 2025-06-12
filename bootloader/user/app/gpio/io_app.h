/**
 * @file io_app.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief io app index define
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif



#include "io_drv_api.h"
#include "io_app_fault.h"
#include "adc_app_sample.h"                                                     //因为一些IO驱动的反馈信号是AD采样,需要调用AD的api获取
#include "eeprom_app.h"                                                         //io故障组件需要访问eeprom的配置项信息












//GPIO管脚定义---这里对应电路图上的GPIO引脚图标
typedef enum
{
    //枪1专用
    eIo_A_DR_K                          = 0,                                    //枪1->接触器组驱动
    eIo_A_D1_RELAY                      = 1,                                    //枪1->复用口---------<GBT:辅源+输出继电器使能---CHADEMO:d1继电器使能>
    eIo_A_D2_RELAY                      = 2,                                    //枪1->复用口---------<GBT:辅源-输出继电器使能---CHADEMO:d2继电器使能>
    eIo_A_GB_BMS                        = 3,                                    //枪1->辅源检测
    eIo_A_LOCK_FB                       = 4,                                    //枪1->枪锁反馈
    eIo_A_LOCK_ON_OFF                   = 5,                                    //枪1->枪锁解锁/锁止使能
    eIo_A_LOCK_RELAY_EN                 = 6,                                    //枪1->枪锁继电器使能
    eIo_A_ISO_RELAY_DRV1                = 7,                                    //枪1->绝缘检测高端光耦使能
    eIo_A_ISO_RELAY_DRV2                = 8,                                    //枪1->绝缘检测低端光耦使能
    eIo_A_ISO_RELAY_DRV3                = 9,                                    //枪1->绝缘检测继电器驱动
    
    //枪2专用
    eIo_B_DR_K                          = 10,                                   //枪2->接触器组驱动
    eIo_B_D1_RELAY                      = 11,                                   //枪2->复用口---------<GBT:辅源+输出继电器使能---CHADEMO:d1继电器使能>
    eIo_B_D2_RELAY                      = 12,                                   //枪2->复用口---------<GBT:辅源-输出继电器使能---CHADEMO:d2继电器使能>
    eIo_B_GB_BMS                        = 13,                                   //枪2->辅源检测
    eIo_B_LOCK_FB                       = 14,                                   //枪2->枪锁反馈
    eIo_B_LOCK_ON_OFF                   = 15,                                   //枪2->枪锁解锁/锁止使能
    eIo_B_LOCK_RELAY_EN                 = 16,                                   //枪2->枪锁继电器使能
    eIo_B_ISO_RELAY_DRV1                = 17,                                   //枪2->绝缘检测高端光耦使能
    eIo_B_ISO_RELAY_DRV2                = 18,                                   //枪2->绝缘检测低端光耦使能
    eIo_B_ISO_RELAY_DRV3                = 19,                                   //枪2->绝缘检测继电器驱动
    
    //公用
    eIo_DR_X                            = 20,                                   //接触器X组驱动
    eIo_DR_Y                            = 21,                                   //接触器Y组驱动
    eIo_DR_Z                            = 22,                                   //接触器Z组驱动
    eIo_PEN_FAULT_AC_RLY_FB             = 23,                                   //PEN_FAULT反馈/交流接触器反馈
    eIo_DOOR                            = 24,                                   //门禁
    eIo_FA_STOP                         = 25,                                   //急停
    eIo_SPD                             = 26,                                   //防雷
    eIo_FAN_STATUS                      = 27,                                   //风扇驱动反馈
    eIo_DO1                             = 28,                                   //交流接触器-继电器
    eIo_DO2                             = 29,                                   //硬重启
    eIo_DO3                             = 30,                                   //分励脱扣继电器
    eIo_SITE1                           = 31,                                   //地址设置拨码开关1~4------复用:B_CC_PP_RLY_EN
    eIo_SITE2                           = 32,                                   //
    eIo_SITE3                           = 33,                                   //
    eIo_SITE4                           = 34,                                   //-------------------------复用:A_CC_PP_RLY_EN
    eIo_GATE_POWER                      = 35,                                   //网关模块电源使能
    
    eIoMax,
}eIo_t;






//GPIO管脚app层数据结构定义
typedef struct
{
    //----------------------------------与充电枪相关的资源----------------------
    //供各用户程序基于枪序号 ePileGunIndex_t 去读取
    //枪IO口资源
    eIo_t                               eIoDrK[ePileGunIndexNum];               //枪x->枪接触器组驱动
    eIo_t                               eIoRelayD1[ePileGunIndexNum];           //枪x->复用口---------<GBT:辅源+输出继电器使能---CHADEMO:d1继电器使能>
    eIo_t                               eIoRelayD2[ePileGunIndexNum];           //枪x->复用口---------<GBT:辅源-输出继电器使能---CHADEMO:d2继电器使能>
    eIo_t                               eIoGbBms[ePileGunIndexNum];             //枪x->辅源检测
    eIo_t                               eIoLockFb[ePileGunIndexNum];            //枪x->枪锁反馈
    eIo_t                               eIoLockOnOff[ePileGunIndexNum];         //枪x->枪锁解锁/锁止使能
    eIo_t                               eIoLockRelayEn[ePileGunIndexNum];       //枪x->枪锁继电器使能
    eIo_t                               eIoRelayIsoDrv1[ePileGunIndexNum];      //枪x->绝缘检测高端光耦使能
    eIo_t                               eIoRelayIsoDrv2[ePileGunIndexNum];      //枪x->绝缘检测低端光耦使能
    eIo_t                               eIoRelayIsoDrv3[ePileGunIndexNum];      //枪x->绝缘检测继电器驱动
    
}stIoAppCache_t;
















extern bool  sIoAppInit(void);
extern bool  sIoAppReInitForMux(void);
extern u8    sIoGetSwitchAddr(void);

//与充电枪有关的一些IO口操作api---基于枪序号操作
extern eIo_t sIoGetDrK(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetRelayD1(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetRelayD2(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetGbBms(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetLockFb(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetLockOnOff(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetLockRelayEn(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetRelayIsoDrv1(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetRelayIsoDrv2(ePileGunIndex_t eGunIndex);
extern eIo_t sIoGetRelayIsoDrv3(ePileGunIndex_t eGunIndex);


//几个带反馈检测的IO信号控制函数---需要在控制后检查反馈信号
extern bool  sIoSetDrK(ePileGunIndex_t eGunIndex, bool bOnOff);
extern bool  sIoSetDrXyz(u8 u8Xyz, bool bOnOff);

//eIo_GATE_POWER---为网关供电及复位
extern void  sIoSetGatePower(FlagStatus eStatus1, u16 u16Time1, FlagStatus eStatus2, u16 u16Time2);










#ifdef __cplusplus
}
#endif









