/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_shell.c
* Description                           :   eeprom芯片用户程序实现 之 eeprom 相关的 shell 命令实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-07
* notice                                :   
****************************************************************************************************/
#include "eeprom_app.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_app_shell";







bool  sEepromAppShellRegister(void);

bool  shell_get_sn(const stShellPkt_t *pkg);
bool  shell_set_sn(const stShellPkt_t *pkg);
bool  shell_set_res(const stShellPkt_t *pkg);
bool  shell_set_iso(const stShellPkt_t *pkg);

bool  shell_set_eeprom(const stShellPkt_t *pkg);






stShellCmd_t stSellCmdGetSn = 
{
    .pCmd       = "getsn",
    .pFormat    = "格式:getsn",
    .pFunction  = "功能:打印桩端SN号码",
    .pRemarks   = "备注:无",
    .pFunc      = shell_get_sn,
};

stShellCmd_t stSellCmdSetSn = 
{
    .pCmd       = "setsn",
    .pFormat    = "格式:setsn SN00000000000000",
    .pFunction  = "功能:设置桩端SN号码",
    .pRemarks   = "备注:无",
    .pFunc      = shell_set_sn,
};

stShellCmd_t stSellCmdSetResFlag = 
{
    .pCmd       = "setresflag",
    .pFormat    = "格式:setresflag 0/1",
    .pFunction  = "功能:设置负载类型",
    .pRemarks   = "备注:0---非电阻载 1---电阻负载",
    .pFunc      = shell_set_res,
};

stShellCmd_t stSellCmdSetIsoFlag = 
{
    .pCmd       = "setisoflag",
    .pFormat    = "格式:setisoflag 0/1",
    .pFunction  = "功能:设置ISO绝缘检测开关",
    .pRemarks   = "备注:0---关 1---开",
    .pFunc      = shell_set_iso,
};

stShellCmd_t stSellCmdSetEeprom = 
{
    .pCmd       = "seteeprom",
    .pFormat    = "格式:seteeprom code",
    .pFunction  = "功能:管理eeprom",
    .pRemarks   = "备注:code:0---擦除, 1---新建充电记录",
    .pFunc      = shell_set_eeprom,
};









/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 命令注册
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool sEepromAppShellRegister(void)
{
    bool bRst = true;
    
    bRst = bRst & sShellCmdRegister(&stSellCmdGetSn);
    bRst = bRst & sShellCmdRegister(&stSellCmdSetSn);
    bRst = bRst & sShellCmdRegister(&stSellCmdSetResFlag);
    bRst = bRst & sShellCmdRegister(&stSellCmdSetIsoFlag);
    bRst = bRst & sShellCmdRegister(&stSellCmdSetEeprom);
    
    return bRst;
}









/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 SN号码打印
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool shell_get_sn(const stShellPkt_t *pkg)
{
    char u8Sn[cPrivDrvLenSn];
    
    memset(u8Sn, 0, sizeof(u8Sn));
    sEepromAppGetBlockFactorySn(u8Sn, sizeof(u8Sn));
    EN_SLOGI(TAG, "eeprom 工厂参数区, SN编号:%s", u8Sn);
    
    return(true);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 SN号码设置
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool shell_set_sn(const stShellPkt_t *pkg)
{
    bool bRst = true;
    
    bRst =  bRst & sEepromAppSetBlockFactorySn((const char *)pkg->para[0], strlen(pkg->para[0]));
    bRst =  bRst & sEepromAppSetBlock(eEepromAppBlockFactory);
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 电阻负载标志 设置
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool shell_set_res(const stShellPkt_t *pkg)
{
    bool bFlag;
    bool bRst = true;
    
    bFlag = (atoi(pkg->para[0]) == 1) ? true : false;
    bRst =  bRst & sEepromAppSetBlockFactoryResFlag(bFlag);
    bRst =  bRst & sEepromAppSetBlock(eEepromAppBlockFactory);
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 ISO检测开关 设置
* Author                                :   Hall
* modified Date                         :   2024-08-23
* notice                                :   
***********************************************************************************************/
bool shell_set_iso(const stShellPkt_t *pkg)
{
    bool bFlag;
    bool bRst = true;
    
    bFlag = (atoi(pkg->para[0]) == 1) ? true : false;
    bRst =  bRst & sEepromAppSetBlockFactoryIsoChkFlag(bFlag);
    bRst =  bRst & sEepromAppSetBlock(eEepromAppBlockFactory);
    
    return(bRst);
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->shell界面 eeprom管理功能
* Author                                :   Hall
* modified Date                         :   2024-05-09
* notice                                :   
***********************************************************************************************/
bool shell_set_eeprom(const stShellPkt_t *pkg)
{
    i32 bRet;
    static stEepromAppBlockChgRcd_t *pRcd = NULL;
    
    if(pRcd == NULL)
    {
        pRcd = (stEepromAppBlockChgRcd_t *)MALLOC(sizeof(stEepromAppBlockChgRcd_t));
    }
    
    switch(atoi(pkg->para[0]))
    {
    case 0:                                                                     //eeprom 擦除
        bRet = sEepromAppRstBlock();
        break;
    case 1:                                                                     //新建充电记录
        memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
        pRcd->bRptFlag = false;
        pRcd->bChargingFlag = true;
        pRcd->stRcdData.u8GunId = 1;
        pRcd->stRcdData.eType = ePrivDrvStartTransTypeVin;
        pRcd->stRcdData.stTimeStart = sGetTime();
        bRet = sEepromAppNewBlockChgRcd(pRcd);
        break;
    case 2:
        memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
        bRet = sEepromAppGetBlockChgRcdByIndex(atoi(pkg->para[1]), pRcd);
        EN_SLOGI(TAG, "记录序号:%d, 上报标志:%d, 充电标志:%d, 充电启动时间:20%02d-%02d-%02d %02d:%02d:%02d",
                bRet, pRcd->bRptFlag, pRcd->bChargingFlag,
                pRcd->stRcdData.stTimeStart.year,
                pRcd->stRcdData.stTimeStart.mon,
                pRcd->stRcdData.stTimeStart.day,
                pRcd->stRcdData.stTimeStart.hour,
                pRcd->stRcdData.stTimeStart.min,
                pRcd->stRcdData.stTimeStart.sec);
        break;
    default:
        break;
    }
    
    
    return(true);
}













