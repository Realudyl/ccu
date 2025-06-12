/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   cooling_app_shell.c
* Description                           :   液冷机通讯 shell命令
* Version                               :   V1.0.0
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-06-21
* notice                                :   
*                                           
****************************************************************************************************/
#include "cooling_app.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "cooling_app_shell";










extern stCoolingAppCache_t *pCoolingAppCache;







void sCoolingAppShellInit(void);

bool sCoolingAppShellSetAddr(const stShellPkt_t *pkg);
bool sCoolingAppShellSetBaud(const stShellPkt_t *pkg);
bool scoolingAppShellSetMode(const stShellPkt_t *pkg);
bool sCoolingAppShellSetLvTemp(const stShellPkt_t *pkg);
bool sCoolingAppShellSetPumpSpeed(const stShellPkt_t *pkg);
bool sCoolingAppShellSetFanSpeed(const stShellPkt_t *pkg);
bool sCoolingAppShellSetHighTemp(const stShellPkt_t *pkg);
bool sCoolingAppShellGetSysInfo(const stShellPkt_t *pkg);


















stShellCmd_t stSellCmdCoolingApp[eCmdMax] = 
{
    {
        .pCmd       = "setcool_addr",
        .pFormat    = "格式: setcool_addr 30",
        .pFunction  = "功能: 设置液冷机地址",
        .pRemarks   = "备注: 设置液冷机地址为30(十进制)，默认为48",
        .pFunc      = sCoolingAppShellSetAddr,
    },
    {
        .pCmd       = "setcool_baud",
        .pFormat    = "格式: setcool_baud 3",
        .pFunction  = "功能: 设置液冷机波特率",
        .pRemarks   = "备注: 0-4800 1-9600 2-19200 3-38400 4-57600 5-115200",
        .pFunc      = sCoolingAppShellSetBaud,
    },
    {
        .pCmd       = "setcool_mode",
        .pFormat    = "格式: setcool_mode 0",
        .pFunction  = "功能: 设置液冷机运行模式",
        .pRemarks   = "备注: 0：自动运行 1：手动调试 2：枪线加液 3：枪线排液 4：储液罐加液 5：储液罐排液",
        .pFunc      = scoolingAppShellSetMode,
    },
    {
        .pCmd       = "setcool_lvt",
        .pFormat    = "格式: setcool_lvt 1 50",
        .pFunction  = "功能: 设置液冷机温度等级和对应温度上限",
        .pRemarks   = "备注: 1表示Lv1等级，50表示50摄氏度，等级范围1~5",
        .pFunc      = sCoolingAppShellSetLvTemp,
    },
    {
        .pCmd       = "setcool_lvp",
        .pFormat    = "格式: setcool_lvp 1 20",
        .pFunction  = "功能: 设置液冷机温度等级和循环泵转速",
        .pRemarks   = "备注: 1表示Lv1等级，20表示20%转速，等级范围1~5，转速设置范围0~100",
        .pFunc      = sCoolingAppShellSetPumpSpeed,
    },
    {
        .pCmd       = "setcool_lvf",
        .pFormat    = "格式: setcool_lvf 1 20",
        .pFunction  = "功能: 设置液冷机温度等级和风机转速",
        .pRemarks   = "备注: 1表示Lv1等级，20表示20%转速，等级范围1~5，设置范围0~100",
        .pFunc      = sCoolingAppShellSetFanSpeed,
    },
    {
        .pCmd       = "setcool_ht",
        .pFormat    = "格式: setcool_ht 90",
        .pFunction  = "功能: 设置液冷机高温报警值",
        .pRemarks   = "备注: 默认设置值90，设置范围0~150",
        .pFunc      = sCoolingAppShellSetHighTemp,
    },
    {
        .pCmd       = "getcool_sys",
        .pFormat    = "格式: getcool_info",
        .pFunction  = "功能: 获取液冷机系统配置参数",
        .pRemarks   = "备注: 无",
        .pFunc      = sCoolingAppShellGetSysInfo,
    },
};



















/***************************************************************************************************
* Description                           :   注册液冷机shell命令
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
void sCoolingAppShellInit(void)
{
    u16 i;
    
    for(i=0; i<eCmdMax; i++)
    {
        sShellCmdRegister(&stSellCmdCoolingApp[i]);
    }
}


















/***************************************************************************************************
* Description                           :   设置液冷机通讯地址
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetAddr(const stShellPkt_t *pkg)
{
    u16 u16DevAddr = atoi(pkg->para[0]);
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(eAdjustRegDevAddr, u16DevAddr) == true)
        {
            EN_SLOGI(TAG, "设置地址命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}











/***************************************************************************************************
* Description                           :   设置液冷机通讯波特率
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetBaud(const stShellPkt_t *pkg)
{
    u16 u16BaudType = atoi(pkg->para[0]);
    u16 u16BaudRate;
    
    switch(u16BaudType)
    {
    case 0:
        u16BaudRate = 4800;
        break;
    case 1:
        u16BaudRate = 9600;
        break;
    case 2:
        u16BaudRate = 19200;
        break;
    case 3:
        u16BaudRate = 38400;
        break;
    case 4:
        u16BaudRate = 57600;
        break;
    case 5:
        u16BaudRate = (u16)115200;
        break;
    default:
        EN_SLOGI(TAG, "设置参数不合法，下发默认波特率参数");
        u16BaudRate = 38400;
        break;
    }
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(eAdjustRegBaudrate, u16BaudRate) == true)
        {
            EN_SLOGI(TAG, "设置波特率命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}











/***************************************************************************************************
* Description                           :   设置液冷机运行模式
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-26
* notice                                :   
****************************************************************************************************/
bool scoolingAppShellSetMode(const stShellPkt_t *pkg)
{
    u16 u16RunMode = atoi(pkg->para[0]);
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(eAdjustRegRunMode, u16RunMode) == true)
        {
            EN_SLOGI(TAG, "设置地址命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}












/***************************************************************************************************
* Description                           :   设置液冷机等级温度
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetLvTemp(const stShellPkt_t *pkg)
{
    u16 u16Level = atoi(pkg->para[0]);
    u16 u16Temp = atoi(pkg->para[1]);
    u16 u16Reg;
    
    switch(u16Level)
    {
    case 1:
        u16Reg = eAdjustRegLv1Temp;
        break;
    case 2:
        u16Reg = eAdjustRegLv2Temp;
        break;
    case 3:
        u16Reg = eAdjustRegLv3Temp;
        break;
    case 4:
        u16Reg = eAdjustRegLv4Temp;
        break;
    case 5:
        u16Reg = eAdjustRegLv5Temp;
        break;
    default:
        EN_SLOGI(TAG, "设置参数不合法");
        break;
    }
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(u16Reg, ((u16Temp + 50) * 10)) == true)
        {
            EN_SLOGI(TAG, "设置等级温度命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}











/***************************************************************************************************
* Description                           :   设置液冷机循环泵转速
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetPumpSpeed(const stShellPkt_t *pkg)
{
    u16 u16Level = atoi(pkg->para[0]);
    u16 u16Speed = atoi(pkg->para[1]);
    u16 u16Reg;
    
    switch(u16Level)
    {
    case 1:
        u16Reg = eAdjustRegLv1PumpsSpeed;
        break;
    case 2:
        u16Reg = eAdjustRegLv2PumpsSpeed;
        break;
    case 3:
        u16Reg = eAdjustRegLv3PumpsSpeed;
        break;
    case 4:
        u16Reg = eAdjustRegLv4PumpsSpeed;
        break;
    case 5:
        u16Reg = eAdjustRegLv5PumpsSpeed;
        break;
    default:
        EN_SLOGI(TAG, "设置参数不合法");
        break;
    }
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(u16Reg, u16Speed) == true)
        {
            EN_SLOGI(TAG, "设置循环泵转速命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}











/***************************************************************************************************
* Description                           :   设置液冷机风机转速
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetFanSpeed(const stShellPkt_t *pkg)
{
    u16 u16Level = atoi(pkg->para[0]);
    u16 u16Speed = atoi(pkg->para[1]);
    u16 u16Reg;
    
    switch(u16Level)
    {
    case 1:
        u16Reg = eAdjustRegLv1FanSpeed;
        break;
    case 2:
        u16Reg = eAdjustRegLv2FanSpeed;
        break;
    case 3:
        u16Reg = eAdjustRegLv3FanSpeed;
        break;
    case 4:
        u16Reg = eAdjustRegLv4FanSpeed;
        break;
    case 5:
        u16Reg = eAdjustRegLv5FanSpeed;
        break;
    default:
        EN_SLOGI(TAG, "设置参数不合法");
        break;
    }
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(u16Reg, u16Speed) == true)
        {
            EN_SLOGI(TAG, "设置风扇转速命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}











/***************************************************************************************************
* Description                           :   设置液冷机高温报警值
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellSetHighTemp(const stShellPkt_t *pkg)
{
    u16 u16HighTemp = atoi(pkg->para[0]);
    
    if(sCoolingAppGetCommSt() == eCommStateNormal)
    {
        if(sCoolingAppSetSysParam(eAdjustRegHighTempAlarm, ((u16HighTemp + 50) * 10)) == true)
        {
            EN_SLOGI(TAG, "设置地址命令下发成功");
        }
    }
    else
    {
        EN_SLOGI(TAG, "错误：液冷机通讯故障，设置失败");
    }
    
    return true;
}













/***************************************************************************************************
* Description                           :   获取液冷机系统配置
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
bool sCoolingAppShellGetSysInfo(const stShellPkt_t *pkg)
{
    //备注：用串口工具发送读取寄存器0x1700数据，返回一次之后再也读不出来，暂时不做了
    //Tx:30 03 17 00 00 01 85 9F
    
    EN_SLOGI(TAG, "错误：暂不支持读取液冷机系统配置");
    
    return true;
}









