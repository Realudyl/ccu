/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_shell.h
* Description                           :   shell界面实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-10-26
* notice                                :   2023-10-26重构
****************************************************************************************************/
#ifndef _en_shell_H_
#define _en_shell_H_

#include "en_common.h"


#define UART_TASK_DEFAULT_PRIOTY        31
#define UART_STACK_SIZE                 (1024)




//shell界面串口
#ifndef cSdkShellComUartNum
#define cShellComUartNum                (eUsartNum0)
#else
#define cShellComUartNum                (cSdkShellComUartNum)
#endif


//shell界面串口波特率
#ifndef cSdkShellComBaudrate
#define cShellComBaudrate               (230400)
#else
#define cShellComBaudrate               (cSdkShellComBaudrate)
#endif
















#define cShellBufSize                   (512)
#define cShellParamNum                  (10)


typedef struct
{
    u8                                  paraNum;
    char                                *cmd;
    u8                                  cmdLen;
    char                                *para[cShellParamNum];
    u8                                  paraLen[cShellParamNum];
}stShellPkt_t;





//单个shell 指令
typedef struct  
{
    char                                *pCmd;                                  //命令        字符串
    char                                *pFormat;                               //格式        字符串
    char                                *pFunction;                             //功能描述    字符串
    char                                *pRemarks;                              //参数描述    字符串
    
    bool                                (*pFunc)(const stShellPkt_t *pkg);
}stShellCmd_t;



//shell 命令 map
#ifndef cSdkShellCmdNumMax
#define cShellCmdNumMax                 (128)
#else
#define cShellCmdNumMax                 (cSdkShellCmdNumMax)
#endif

typedef struct
{
    i32                                 i32CmdNum;
    stShellCmd_t                        *pCmd[cShellCmdNumMax];
}stShellCmdMap_t;













































//shell界面 缓存结构
typedef struct
{
    u8                                  u8RxBuf[cShellBufSize];
    u8                                  u8ShellBuf[cShellBufSize];
    
    u16                                 u16RxCnt;
    u32                                 u32BeginTime;                           //本帧开始接收的时间
    bool                                bRxFlag;
}stShellCache_t;
























extern bool sShellInit(void);
extern bool sShellCmdRegister(stShellCmd_t *pCmd);












#endif







