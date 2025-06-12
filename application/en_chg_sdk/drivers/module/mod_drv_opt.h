/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_opt.h
* Description                           :   对模块通讯协议应用层管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-30
* notice                                :   
****************************************************************************************************/
#ifndef __mod_drv_opt_h_
#define __mod_drv_opt_h_
#include "mod_drv_en_api.h"
#include "mod_drv_winline_api.h"
#include "mod_drv_env2h.h"







//端口配置
#ifndef cSdkModCanPort
#define cModCanPort                     (eCanNum0)
#else
#define cModCanPort                     (cSdkModCanPort)
#endif

#ifndef cSdkModCanBaudrate
#define cModCanBaudrate                 (125000)
#else
#define cModCanBaudrate                 (cSdkModCanBaudrate)
#endif










//模块电源类型定义
typedef enum 
{
    eModTypeEn                          = 0,                                    //EN+ 模块
    eModTypeWinline                     = 1,                                    //永联科技 NXR 系列充电模块
    eModTypeV2H                         = 2,                                    //V2H 系列充电模块

    eModTypeMax
}eModType_t;





//模块电源CAN通讯协议 功能映射
typedef struct
{
    //模块类型
    eModType_t                          eType;
    
    //模块初始化
    bool                                (*pInit)(i32 i32Port, i32 i32Baudrate);
    
    //获取在线模块地址列表
    bool                                (*pGetOnlineAddrList)(u8 u8MaxNum, i8 *pList);
    
    //低功耗状态设定
    void                                (*pSetLowPowerSts)(bool bSts);
    
    
    //一系列api操作接口
    bool                                (*pOpen)(u8 u8Addr, bool bGroupFlag);                           //开启输出
    bool                                (*pClose)(u8 u8Addr, bool bGroupFlag);                          //关闭输出
    bool                                (*pSetOutput)(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32Idc);//设置模块电源输出电压和电流
    bool                                (*pSetGroupNum)(u8 u8Addr, u32 u32GroupNum);                    //设置模块电源组号
    bool                                (*pSetAddrMode)(u8 u8Addr, u32 u32Mode);                        //设置模块电源分组组模式
    bool                                (*pSetAltitude)(u8 u8Addr, u32 u32Altitude);                    //设置模块海拔高度
    
    bool                                (*pGetCurve)(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
    bool                                (*pGetOutput)(u8 u8Addr, f32 *pUdc, f32 *pIdc);
    bool                                (*pGetGroupNum)(u8 u8Addr, u32 *pGroupNum);
    bool                                (*pGetAddrBySwitch)(u8 u8Addr, u32 *pAddrBySwitchd);
    bool                                (*pGetAltitude)(u8 u8Addr, u32 *pAltitude);
    bool                                (*pGetFanSpeed)(u8 u8Addr, u8 *pSpeed);
    bool                                (*pGetErrCode)(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
    bool                                (*pGetTenv)(u8 u8Addr, f32 *pTenv);
    bool                                (*pGetTempDetail)(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
    bool                                (*pGetUacInput)(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
    bool                                (*pGetWorkStatus)(u8 u8Addr, u8 *pStatus);
    bool                                (*pGetRunTime)(u8 u8Addr, u32 *pHtotal, u32 *pHthistime);
    bool                                (*pGetIacInput)(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc);
    bool                                (*pGetPacFac)(u8 u8Addr, f32 *pPac, f32 *pFac);
    bool                                (*pGetEnergy)(u8 u8Addr, u32 *pEtotal, u32 *pEthistime);
    bool                                (*pGetMaterialNum)(u8 u8Addr, u32 *pNum);
    bool                                (*pGetMatchCode)(u8 u8Addr, u16 *pCode);
    bool                                (*pGetModelType)(u8 u8Addr, u32 *pType);
    bool                                (*pGetFeatureWord)(u8 u8Addr, u32 *pWord);
    bool                                (*pGetSerialNum)(u8 u8Addr, u8 u8MaxLen, char *pSerialNum);
    bool                                (*pGetModelName)(u8 u8Addr, u8 u8MaxLen, char *pModelName);
    bool                                (*pGetManufacturerId)(u8 u8Addr, u8 u8MaxLen, char *pId);
    bool                                (*pGetSwVer)(u8 u8Addr, u8 u8MaxLen, char *pSwVer);
    bool                                (*pSetWorkMode)(u8 u8Addr, u16 u16Mode);
    
}stModDrvOpt_t;



























































extern bool  sModDrvOptInit(i32 i32ModType);
extern bool  sModDrvOptGetOnlineAddrList(u8 u8MaxNum, i8 *pList);
extern void  sModDrvOptSetLowPowerSts(bool bSts);

extern bool  sModDrvOptOpen(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvOptClose(u8 u8Addr, bool bGroupFlag);
extern bool  sModDrvOptSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
extern bool  sModDrvOptSetGroupNum(u8 u8Addr, u8 u8GroupNum);
extern bool  sModDrvOptSetAddrMod(u8 u8Addr, u32 u32Mode);
extern bool  sModDrvOptSetSetAltitude(u8 u8Addr, u32 u32Altitude);

extern bool  sModDrvOptGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
extern bool  sModDrvOptGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
extern bool  sModDrvOptGetGroupNum(u8 u8Addr, u32* pGroup);
extern bool  sModDrvOptGetAltitude(u8 u8Addr, u32* pAltitude);
extern bool  sModDrvOptGetFanSpeed(u8 u8Addr, u8 *pSpeed);
extern bool  sModDrvOptGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
extern bool  sModDrvOptGetTenv(u8 u8Addr, f32 *pTenv);
extern bool  sModDrvOptGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
extern bool  sModDrvOptGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
extern bool  sModDrvOptGetWorkStatus(u8 u8Addr, u8 *pStatus);
extern bool  sModDrvOptGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime);
extern bool  sModDrvOptGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc);
extern bool  sModDrvOptGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
extern bool  sModDrvOptGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime);
extern bool  sModDrvOptGetMaterialNum(u8 u8Addr, u32 *pNum);
extern bool  sModDrvOptGetMatchCode(u8 u8Addr, u16 *pCode);
extern bool  sModDrvOptGetModelType(u8 u8Addr, u32 *pType);
extern bool  sModDrvOptGetFeatureWord(u8 u8Addr, u32 *pWord);
extern bool  sModDrvOptGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum);
extern bool  sModDrvOptGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName);
extern bool  sModDrvOptGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId);
extern bool  sModDrvOptGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);










#endif














