/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_opt.c
* Description                           :   模块电源管理接口
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-30
* notice                                :   
****************************************************************************************************/
#include "en_shell.h"
#include "mod_drv_opt.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_drv_opt";





//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------

//模块功能映射表
static stModDrvOpt_t stModDrvOptMap[] = 
{
    //EN+ 模块
    {
        .eType                          = eModTypeEn,
        .pInit                          = sModDrvEnInit,
        .pGetOnlineAddrList             = sModDrvEnGetOnlineAddrList,
        .pSetLowPowerSts                = sModDrvEnSetLowPowerSts,
        
        //api操作接口函数
        .pOpen                          = sModDrvEnOpen,
        .pClose                         = sModDrvEnClose,
        .pSetOutput                     = sModDrvEnSetOutput,
        .pSetGroupNum                   = sModDrvEnSetGroupNum,
        .pSetAddrMode                   = NULL,
        .pSetAltitude                   = NULL,
        .pSetWorkMode                   = NULL,
         
        .pGetCurve                      = sModDrvEnGetCurve,
        .pGetOutput                     = sModDrvEnGetOutput,
        .pGetGroupNum                   = NULL,
        .pGetAddrBySwitch               = NULL,
        .pGetAltitude                   = NULL,
        .pGetFanSpeed                   = sModDrvEnGetFanSpeed,
        .pGetErrCode                    = sModDrvEnGetErrCode,
        .pGetTenv                       = sModDrvEnGetTenv,
        .pGetTempDetail                 = sModDrvEnGetTempDetail,
        .pGetUacInput                   = sModDrvEnGetUacInput,
        .pGetWorkStatus                 = sModDrvEnGetWorkStatus,
        .pGetRunTime                    = sModDrvEnGetRunTime,
        .pGetIacInput                   = sModDrvEnGetIacInput,
        .pGetPacFac                     = sModDrvEnGetPacFac,
        .pGetEnergy                     = sModDrvEnGetEnergy,
        .pGetMaterialNum                = sModDrvEnGetMaterialNum,
        .pGetMatchCode                  = sModDrvEnGetMatchCode,
        .pGetModelType                  = sModDrvEnGetModelType,
        .pGetFeatureWord                = sModDrvEnGetFeatureWord,
        .pGetSerialNum                  = sModDrvEnGetSerialNum,
        .pGetModelName                  = sModDrvEnGetModelName,
        .pGetManufacturerId             = sModDrvEnGetManufacturerId,
        .pGetSwVer                      = sModDrvEnGetSwVer,
        
    },
    
    //永联科技 NXR 系列充电模块
    {
        .eType                          = eModTypeWinline,
        .pInit                          = sModDrvWinlineInit,
        .pGetOnlineAddrList             = sModDrvWinlineGetOnlineAddrList,
        .pSetLowPowerSts                = sModDrvWinlineSetLowPowerSts,
        .pSetWorkMode                   = NULL,
        //api操作接口函数
        .pOpen                          = sModDrvWinlineOpen,
        .pClose                         = sModDrvWinlineClose,
        .pSetOutput                     = sModDrvWinlineSetOutput,
        .pSetGroupNum                   = sModDrvWinlineSetGroup,
        .pSetAddrMode                   = sModDrvWinlineSetAddrMode,
        .pSetAltitude                   = sModDrvWinlineSetAltitude,
        
        .pGetCurve                      = sModDrvWinlineGetCurve,
        .pGetOutput                     = sModDrvWinlineGetOutput,
        .pGetGroupNum                   = sModDrvWinlineGetGroup,
        .pGetAddrBySwitch               = sModDrvWinlineGetAddrBySwitch,
        .pGetAltitude                   = sModDrvWinlineGetAltitude,
        .pGetFanSpeed                   = sModDrvWinlineGetFanSpeed,
        .pGetErrCode                    = sModDrvWinlineGetErrCode,
        .pGetTenv                       = sModDrvWinlineGetTenv,
        .pGetTempDetail                 = sModDrvWinlineGetTempDetail,
        .pGetUacInput                   = sModDrvWinlineGetUacInput,
        .pGetWorkStatus                 = NULL,
        .pGetRunTime                    = NULL,
        .pGetIacInput                   = NULL,
        .pGetPacFac                     = sModDrvWinlineGetPacFac,
        .pGetEnergy                     = NULL,
        .pGetMaterialNum                = NULL,
        .pGetMatchCode                  = NULL,
        .pGetModelType                  = NULL,
        .pGetFeatureWord                = NULL,
        .pGetSerialNum                  = NULL,
        .pGetModelName                  = NULL,
        .pGetManufacturerId             = NULL,
        .pGetSwVer                      = sModDrvWinlineGetSwVer,
        
    },
    //V2H
    {
        .eType                          = eModTypeV2H,
        .pInit                          = ModDrvV2HInit,
        .pGetOnlineAddrList             = NULL,
        .pSetLowPowerSts                = NULL,
        
        //api操作接口函数
        .pOpen                          = sModDrvV2HOpen,
        .pClose                         = sModDrvV2HClose,
        .pSetOutput                     = sModDrvV2HSetOutput,
        .pSetGroupNum                   = NULL,
        .pSetAddrMode                   = NULL,
        .pSetAltitude                   = NULL,
        .pSetWorkMode                   = sModDrvSerWorkMode,
        
        .pGetCurve                      = NULL,
        .pGetOutput                     = sModDrvV2HGetOutput,
        .pGetGroupNum                   = NULL,
        .pGetAddrBySwitch               = NULL,
        .pGetAltitude                   = NULL,
        .pGetFanSpeed                   = NULL,
        .pGetErrCode                    = NULL,
        .pGetTenv                       = NULL,
        .pGetTempDetail                 = NULL,
        .pGetUacInput                   = NULL,
        .pGetWorkStatus                 = NULL,
        .pGetRunTime                    = NULL,
        .pGetIacInput                   = NULL,
        .pGetPacFac                     = NULL,
        .pGetEnergy                     = NULL,
        .pGetMaterialNum                = NULL,
        .pGetMatchCode                  = NULL,
        .pGetModelType                  = NULL,
        .pGetFeatureWord                = NULL,
        .pGetSerialNum                  = NULL,
        .pGetModelName                  = NULL,
        .pGetManufacturerId             = NULL,
        .pGetSwVer                      = NULL,
        
    },
};

stModDrvOpt_t *pModDrvOpt = &stModDrvOptMap[eModTypeEn];


//---------------------------------------------------------------------------------------------------------
bool  sModDrvOptGetData(const stShellPkt_t *pkg);
bool  sModDrvOptSetModOnOff(const stShellPkt_t *pkg);



bool  sModDrvOptInit(i32 i32ModType);
bool  sModDrvOptGetOnlineAddrList(u8 u8MaxNum, i8 *pList);
void  sModDrvOptSetLowPowerSts(bool bSts);

bool  sModDrvOptOpen(u8 u8Addr, bool bGroupFlag);
bool  sModDrvOptClose(u8 u8Addr, bool bGroupFlag);
bool  sModDrvOptSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
bool  sModDrvOptSetGroupNum(u8 u8Addr, u8 u8GroupNum);
bool  sModDrvOptSetAddrMod(u8 u8Addr, u32 u32Mode);
bool  sModDrvOptSetSetAltitude(u8 u8Addr, u32 u32Altitude);

bool  sModDrvOptGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
bool  sModDrvOptGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
bool  sModDrvOptGetGroupNum(u8 u8Addr, u32* pGroup);
bool  sModDrvOptGetAltitude(u8 u8Addr, u32* pAltitude);
bool  sModDrvOptGetFanSpeed(u8 u8Addr, u8 *pSpeed);
bool  sModDrvOptGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
bool  sModDrvOptGetTenv(u8 u8Addr, f32 *pTenv);
bool  sModDrvOptGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
bool  sModDrvOptGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
bool  sModDrvOptGetWorkStatus(u8 u8Addr, u8 *pStatus);
bool  sModDrvOptGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime);
bool  sModDrvOptGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc);
bool  sModDrvOptGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
bool  sModDrvOptGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime);
bool  sModDrvOptGetMaterialNum(u8 u8Addr, u32 *pNum);
bool  sModDrvOptGetMatchCode(u8 u8Addr, u16 *pCode);
bool  sModDrvOptGetModelType(u8 u8Addr, u32 *pType);
bool  sModDrvOptGetFeatureWord(u8 u8Addr, u32 *pWord);
bool  sModDrvOptGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum);
bool  sModDrvOptGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName);
bool  sModDrvOptGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId);
bool  sModDrvOptGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);





//---------------------------------------------------------------------------------------------------------

stShellCmd_t stSellCmdGetModData = 
{
    .pCmd       = "getmoddata",
    .pFormat    = "格式:getmoddata addr",
    .pFunction  = "功能:打印指定地址模块电源的数据",
    .pRemarks   = "备注:address--->1~16",
    .pFunc      = sModDrvOptGetData,
};

stShellCmd_t stSellCmdSetModOnOff = 
{
    .pCmd       = "setmodonoff",
    .pFormat    = "格式:setmodonoff address on/off",
    .pFunction  = "功能:开启或关闭指定地址模块电源",
    .pRemarks   = "备注:address--->1~16, on/off--->0:off, 1:on",
    .pFunc      = sModDrvOptSetModOnOff,
};





/**********************************************************************************************
* Description                           :   shell界面 模块电源数据打印函数
* Author                                :   Hall
* modified Date                         :   2024-03-05
* notice                                :   
***********************************************************************************************/
bool sModDrvOptGetData(const stShellPkt_t *pkg)
{
    i32 i32Addr = 0;
    u8  u8Value;
    u8  u8Data[36];
    
    u16 u16Value1, u16Value2, u16Value3;
    u32 u32Value1, u32Value2;
    
    f32 f32Data[18];
    
    i32Addr = atoi(pkg->para[0]);
    
    sModDrvOptGetOutput(i32Addr, &f32Data[0], &f32Data[1]);
    EN_SLOGI(TAG, "模块电源数据--->Udc:%0.2fV, Idc:%0.2fA", f32Data[0], f32Data[1]);
    
    sModDrvOptGetFanSpeed(i32Addr, &u8Value);
    EN_SLOGI(TAG, "模块电源数据--->FanSpeed:%d%%", u8Value);
    
    
    memset(f32Data, 0, sizeof(f32Data));
    sModDrvOptGetTenv(i32Addr, &f32Data[0]);
    sModDrvOptGetTempDetail(i32Addr, sizeof(f32Data), f32Data);
    EN_SLOGI(TAG, "模块电源数据--->Tenv:%0.1f度", f32Data[0]);
    EN_SLOGI(TAG, "模块电源数据--->Tmos1/Tdc:%0.1f度, Tmos2/Tpfc:%0.1f度, Tdiod1:%0.1f度, Tdiod2:%0.1f度, Ttsf:%0.1f度, Tcpu:%0.1f度, Trad1:%0.1f度, Trad2:%0.1f度", 
             f32Data[0], f32Data[1], f32Data[2], f32Data[3], f32Data[4], f32Data[5], f32Data[6], f32Data[7]);
    
    sModDrvOptGetUacInput(i32Addr, &f32Data[0], &f32Data[1], &f32Data[2]);
    EN_SLOGI(TAG, "模块电源数据--->Uab:%0.1fV, Ubc:%0.1fV, Uca:%0.1fV", f32Data[0], f32Data[1], f32Data[2]);
    
    sModDrvOptGetWorkStatus(i32Addr, &u8Value);
    EN_SLOGI(TAG, "模块电源数据--->工作状态:%d", u8Value);
    
    sModDrvOptGetRunTime(i32Addr, &u32Value1, &u32Value2);
    EN_SLOGI(TAG, "模块电源数据--->H-total:%ds, H-this time:%ds", u32Value1 * 360, u32Value2);
    
    sModDrvOptGetEnergy(i32Addr, &u32Value1, &u32Value2);
    EN_SLOGI(TAG, "模块电源数据--->E-total:%dwh, E-this time:%dwh", u32Value1, u32Value2);
    
    sModDrvOptGetIacInput(i32Addr, &f32Data[0], &f32Data[1], &f32Data[2]);
    EN_SLOGI(TAG, "模块电源数据--->Ia:%0.1fA, Ib:%0.1fA, Ic:%0.1fA", f32Data[0], f32Data[1], f32Data[2]);
    
    sModDrvOptGetPacFac(i32Addr, &f32Data[0], &f32Data[1]);
    EN_SLOGI(TAG, "模块电源数据--->Pac:%dw, Fac:%0.2fHz", f32Data[0], f32Data[1]);
    
    
    memset(u8Data, 0, sizeof(u8Data));
    sModDrvOptGetSerialNum(i32Addr, sizeof(u8Data), (char *)u8Data);
    EN_SLOGI(TAG, "模块电源数据--->序列号:%s", u8Data);
    
    memset(u8Data, 0, sizeof(u8Data));
    sModDrvOptGetModelName(i32Addr, sizeof(u8Data), (char *)u8Data);
    EN_SLOGI(TAG, "模块电源数据--->ModelName:%s", u8Data);
    
    memset(u8Data, 0, sizeof(u8Data));
    sModDrvOptGetManufacturerId(i32Addr, sizeof(u8Data), (char *)u8Data);
    EN_SLOGI(TAG, "模块电源数据--->制造商ID:%s", u8Data);
    
    memset(u8Data, 0, sizeof(u8Data));
    sModDrvOptGetSwVer(i32Addr, sizeof(u8Data), (char *)u8Data);
    EN_SLOGI(TAG, "模块电源数据--->软件版本:%s", u8Data);
    
    return(true);
}






/**********************************************************************************************
* Description                           :   shell界面 模块电源数据打印函数
* Author                                :   Hall
* modified Date                         :   2024-03-12
* notice                                :   
***********************************************************************************************/
bool sModDrvOptSetModOnOff(const stShellPkt_t *pkg)
{
    bool bRst;
    i32  i32Addr  = 0;
    i32  i32OnOff = 0;
    i32  i32Timer = 0;
    f32  f32Udc = 0, f32Idc = 0;
    
    i32Addr  = atoi(pkg->para[0]);
    i32OnOff = atoi(pkg->para[1]);
    
    sModDrvOptSetOutput(0, false, 300, 10);
    bRst = (i32OnOff == 1) ? sModDrvOptOpen(i32Addr, false) : sModDrvOptClose(i32Addr, false);
    EN_SLOGI(TAG, "开启或关闭模块电源:%d, 结果:%d(0---失败, 1---成功)", i32Addr, bRst);
    if(i32OnOff != 1)
    {
        while(i32Timer < 4000)
        {
            i32Timer = i32Timer + 100;
            vTaskDelay(100 / portTICK_RATE_MS);
            sModDrvOptGetOutput(i32Addr, &f32Udc, &f32Idc);
            EN_SLOGI(TAG, "关闭模块电源, Udc:%0.2fV, Idc:%0.2fA", f32Udc, f32Idc);
        }
    }
    
    return(true);
}










/***************************************************************************************************
* Description                           :   模块电源管理接口 资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-07-29
****************************************************************************************************/
bool sModDrvOptInit(i32 i32ModType)
{
    if(i32ModType >= eModTypeMax)
    {
        EN_SLOGE(TAG, "模块电源通讯 资源初始化失败, 参数错误");
        return(false);
    }
    pModDrvOpt = &stModDrvOptMap[i32ModType];
    sShellCmdRegister(&stSellCmdGetModData);
    sShellCmdRegister(&stSellCmdSetModOnOff);
    
    return ((pModDrvOpt->pInit == NULL) ? false : pModDrvOpt->pInit(cModCanPort, cModCanBaudrate));
}










/***************************************************************************************************
* Description                           :   模块电源管理接口 获取在线模块地址的list
* Author                                :   Hall
* Creat Date                            :   2023-08-19
* notice                                :   u8MaxNum：pList指针指向的内存空间长度
*                                           pList   ：指向一个内存空间 用于接收返回的地址列表
****************************************************************************************************/
bool sModDrvOptGetOnlineAddrList(u8 u8MaxNum, i8 *pList)
{
    return ((pModDrvOpt->pGetOnlineAddrList == NULL) ? false : pModDrvOpt->pGetOnlineAddrList(u8MaxNum, pList));
}








/***************************************************************************************************
* Description                           :   模块电源管理接口 低功耗状态设定
* Author                                :   Hall
* Creat Date                            :   2024-12-18
****************************************************************************************************/
void sModDrvOptSetLowPowerSts(bool bSts)
{
    if(pModDrvOpt->pSetLowPowerSts != NULL)
    {
        pModDrvOpt->pSetLowPowerSts(bSts);
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理接口 工作模式，见 @eModV2HOperationMode_t
* Author                                :   Anlada
* Creat Date                            :   2025-2-13
****************************************************************************************************/
void sModDrvOptSetWorkMode(u8 u8Addr, u16 u16Mode)
{
    if(pModDrvOpt->pSetWorkMode != NULL)
    {
        pModDrvOpt->pSetWorkMode(u8Addr, u16Mode);
    }
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 开启输出
* Author                                :   Hall
* Creat Date                            :   2022-08-05
* notice                                :   
****************************************************************************************************/
bool sModDrvOptOpen(u8 u8Addr, bool bGroupFlag)
{
    //EN_SLOGI(TAG, "TEST2:模块%d开机", u8Addr);
    return ((pModDrvOpt->pOpen == NULL) ? false : pModDrvOpt->pOpen(u8Addr, bGroupFlag));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 关闭输出
* Author                                :   Hall
* Creat Date                            :   2022-08-05
* notice                                :   
****************************************************************************************************/
bool sModDrvOptClose(u8 u8Addr, bool bGroupFlag)
{
    //EN_SLOGI(TAG, "TEST2:模块%d关机", u8Addr);
    return ((pModDrvOpt->pClose == NULL) ? false : pModDrvOpt->pClose(u8Addr, bGroupFlag));
}









/***************************************************************************************************
* Description                           :   模块电源管理接口 设置输出电压和限流点
* Author                                :   Hall
* Creat Date                            :   2022-08-05
* notice                                :   
****************************************************************************************************/
bool sModDrvOptSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit)
{
    //EN_SLOGI(TAG, "TEST3:模块%d:%.1f, %.1f", u8Addr, f32Udc, f32IdcLimit);
    return ((pModDrvOpt->pSetOutput == NULL) ? false : pModDrvOpt->pSetOutput(u8Addr, bGroupFlag, f32Udc, f32IdcLimit));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 设置组号
* Author                                :   Hall
* Creat Date                            :   2024-03-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptSetGroupNum(u8 u8Addr, u8 u8GroupNum)
{
    return ((pModDrvOpt->pSetGroupNum == NULL) ? false : pModDrvOpt->pSetGroupNum(u8Addr, u8GroupNum));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 设置分组模式
* Author                                :   anlada
* Creat Date                            :   2024-09-04
* notice                                :   
****************************************************************************************************/
bool sModDrvOptSetAddrMod(u8 u8Addr, u32 u32Mode)
{
    return ((pModDrvOpt->pSetAddrMode == NULL) ? false : pModDrvOpt->pSetAddrMode(u8Addr, u32Mode));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 设置模块海拔
* Author                                :   anlada
* Creat Date                            :   2024-09-04
* notice                                :   
****************************************************************************************************/
bool sModDrvOptSetSetAltitude(u8 u8Addr, u32 u32Altitude)
{
    return ((pModDrvOpt->pSetAltitude == NULL) ? false : pModDrvOpt->pSetAltitude(u8Addr, u32Altitude));
}







/***************************************************************************************************
* Description                           :   模块电源管理接口 输入输出特性曲线
* Author                                :   Hall
* Creat Date                            :   2024-03-26
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc)
{
    return ((pModDrvOpt->pGetCurve == NULL) ? false : pModDrvOpt->pGetCurve(pUdcMax, pUdcMin, pIdcMax, pPdc));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 获取模块的输出电压和电流
* Author                                :   Hall
* Creat Date                            :   2023-08-19
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc)
{
    return ((pModDrvOpt->pGetOutput == NULL) ? false : pModDrvOpt->pGetOutput(u8Addr, pUdc, pIdc));
}








/***************************************************************************************************
* Description                           :   模块电源管理接口 获取模块组编号
* Author                                :   anlada
* Creat Date                            :   2024-09-04
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetGroupNum(u8 u8Addr, u32* pGroup)
{
    return ((pModDrvOpt->pGetGroupNum == NULL) ? false : pModDrvOpt->pGetGroupNum(u8Addr, pGroup));
}








/***************************************************************************************************
* Description                           :   模块电源管理接口 输入输出特性曲线
* Author                                :   Hall
* Creat Date                            :   2024-03-26
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetAltitude(u8 u8Addr, u32* pAltitude)
{
    return ((pModDrvOpt->pGetAltitude == NULL) ? false : pModDrvOpt->pGetAltitude(u8Addr, pAltitude));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 获取风扇转速
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetFanSpeed(u8 u8Addr, u8 *pSpeed)
{
    return ((pModDrvOpt->pGetFanSpeed == NULL) ? false : pModDrvOpt->pGetFanSpeed(u8Addr, pSpeed));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 获取错误码
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode)
{
    return ((pModDrvOpt->pGetErrCode == NULL) ? false : pModDrvOpt->pGetErrCode(u8Addr, u8MaxLen, pCode));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 获取环境温度
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetTenv(u8 u8Addr, f32 *pTenv)
{
    return ((pModDrvOpt->pGetTenv == NULL) ? false : pModDrvOpt->pGetTenv(u8Addr, pTenv));
}






/***************************************************************************************************
* Description                           :   模块电源管理接口 获取温度详情
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp)
{
    return ((pModDrvOpt->pGetTempDetail == NULL) ? false : pModDrvOpt->pGetTempDetail(u8Addr, u8MaxLen, pTemp));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取输入电压
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca)
{
    return ((pModDrvOpt->pGetUacInput == NULL) ? false : pModDrvOpt->pGetUacInput(u8Addr, pUab, pUbc, pUca));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取工作状态
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetWorkStatus(u8 u8Addr, u8 *pStatus)
{
    return ((pModDrvOpt->pGetWorkStatus == NULL) ? false : pModDrvOpt->pGetWorkStatus(u8Addr, pStatus));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取运行时间
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime)
{
    return ((pModDrvOpt->pGetRunTime == NULL) ? false : pModDrvOpt->pGetRunTime(u8Addr, pHtotal, pHthistime));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取输入电流
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc)
{
    return ((pModDrvOpt->pGetIacInput == NULL) ? false : pModDrvOpt->pGetIacInput(u8Addr, pIa, pIb, pIc));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取输入功率和输入频率
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac)
{
    return ((pModDrvOpt->pGetPacFac == NULL) ? false : pModDrvOpt->pGetPacFac(u8Addr, pPac, pFac));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取电量信息
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime)
{
    return ((pModDrvOpt->pGetEnergy == NULL) ? false : pModDrvOpt->pGetEnergy(u8Addr, pEtotal, pEthistime));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取软件物料代码
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetMaterialNum(u8 u8Addr, u32 *pNum)
{
    return ((pModDrvOpt->pGetMaterialNum == NULL) ? false : pModDrvOpt->pGetMaterialNum(u8Addr, pNum));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取软件匹配码
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetMatchCode(u8 u8Addr, u16 *pCode)
{
    return ((pModDrvOpt->pGetMatchCode == NULL) ? false : pModDrvOpt->pGetMatchCode(u8Addr, pCode));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取机型
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetModelType(u8 u8Addr, u32 *pType)
{
    return ((pModDrvOpt->pGetModelType == NULL) ? false : pModDrvOpt->pGetModelType(u8Addr, pType));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取模块特征字
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetFeatureWord(u8 u8Addr, u32 *pWord)
{
    return ((pModDrvOpt->pGetFeatureWord == NULL) ? false : pModDrvOpt->pGetFeatureWord(u8Addr, pWord));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取序列号
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum)
{
    return ((pModDrvOpt->pGetSerialNum == NULL) ? false : pModDrvOpt->pGetSerialNum(u8Addr, u8MaxLen, pSerialNum));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 获取model name
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName)
{
    return ((pModDrvOpt->pGetModelName == NULL) ? false : pModDrvOpt->pGetModelName(u8Addr, u8MaxLen, pModelName));
}




/***************************************************************************************************
* Description                           :   模块电源管理接口 获取制造商ID
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId)
{
    return ((pModDrvOpt->pGetManufacturerId == NULL) ? false : pModDrvOpt->pGetManufacturerId(u8Addr, u8MaxLen, pId));
}





/***************************************************************************************************
* Description                           :   模块电源管理接口 软件版本
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvOptGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer)
{
    return ((pModDrvOpt->pGetSwVer == NULL) ? false : pModDrvOpt->pGetSwVer(u8Addr, u8MaxLen, pSwVer));
}










