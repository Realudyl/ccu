/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_shell.c
* Description                           :   shell界面实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-10-26
* notice                                :   2023-10-26重构
****************************************************************************************************/
#include "usart_drv_trans.h"

#include "en_log.h"
#include "en_shell.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "EN_SHELL";






stShellCache_t stShellCache;
stShellCmdMap_t stShellCmdMap;






bool  sShellInit(void);
bool  sShellHwInit(void);
bool  sShellCmdRegister(stShellCmd_t *pCmd);



void  sShellComRecvTask(void *pvParam);
char* parseShellCmd(uint8_t *buf, stShellPkt_t *argss);
void  shell_exec(u8 *data, int len);
void  shell_read(eUsartNum_t eUsartNum, i32 i32Len, u8 *pBuf);



bool  shell_list_dir(const stShellPkt_t *pkg);







stShellCmd_t stSellCmdListCmd = 
{
    .pCmd       = "ls",
    .pFormat    = "格式:ls",
    .pFunction  = "功能:打印支持的shell指令列表",
    .pRemarks   = "备注:",
    .pFunc      = shell_list_dir,
};








/**********************************************************************************************
* Description                           :   shell 界面资源初始化
* Author                                :   Hall
* modified Date                         :   2023-11-08
* notice                                :   
***********************************************************************************************/
bool sShellInit(void)
{
    bool bRst;
    
    memset(&stShellCmdMap, 0, sizeof(stShellCmdMap));
    stShellCmdMap.i32CmdNum = 0;
    sShellCmdRegister(&stSellCmdListCmd);
    
    bRst  = sEnLogInit();
    bRst &= sShellHwInit();
    
    return(bRst);
}







/**********************************************************************************************
* Description                           :   shell 界面硬件资源初始化
* Author                                :   Hall
* modified Date                         :   2023-11-08
* notice                                :   
***********************************************************************************************/
bool sShellHwInit(void)
{
    bool bRst;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cShellComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    sUsartDrvOpen(cShellComUartNum, &stUsartCfg_t, 512, 3072);
    
    
    bRst = true;
    if(pdPASS != xTaskCreate(sShellComRecvTask, "sShellComRecvTask", UART_STACK_SIZE, NULL, UART_TASK_DEFAULT_PRIOTY, NULL))
    {
        bRst = false;
        EN_SLOGE(TAG, "shell界面接收任务创建出错!!!");
    }
    
    return(bRst);
}







/**********************************************************************************************
* Description                           :   shell 界面 指令注册
* Author                                :   Hall
* modified Date                         :   2023-11-08
* notice                                :   
***********************************************************************************************/
bool sShellCmdRegister(stShellCmd_t *pCmd)
{
    bool bRst;
    
    bRst = false;
    if(stShellCmdMap.i32CmdNum < (cShellCmdNumMax - 1))
    {
        stShellCmdMap.pCmd[stShellCmdMap.i32CmdNum] = pCmd;
        stShellCmdMap.i32CmdNum++;
        bRst = true;
    }
    
    return(bRst);
}










void sShellComRecvTask(void *pvParam)
{
    i32 i32Len;
    
    while(1)
    {
        memset(stShellCache.u8RxBuf, 0, sizeof(stShellCache.u8RxBuf));
        i32Len = sUsartDrvRecv(cShellComUartNum, sizeof(stShellCache.u8RxBuf), stShellCache.u8RxBuf);
        if(i32Len > 0)
        {
            shell_read(cShellComUartNum, i32Len, stShellCache.u8RxBuf);
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}













char* parseShellCmd(uint8_t *buf, stShellPkt_t *argss)
{
    char *p = (char*)buf;
    int   argv = 0;
    char *pstr;
    
    if(!argss)
    {
        return NULL;
    }
    
    memset(argss, 0, sizeof(stShellPkt_t));
    do
    {
        pstr = strtok(p, " \n\t");
        p = NULL;
        
        if(pstr)
        {
            if(argss->cmd)
            {
                argss->para[argv] = pstr;
                argss->paraLen[argv] = strlen(pstr);
                argv ++;
            }
            else
            {
                argss->cmd = pstr;
                argss->cmdLen = strlen(pstr);
            }
        }
        else
        {
            break;
        }
        
    }while(argv < cShellParamNum - 1);
    
    argss->paraNum = argv;
    
    return argss->cmd;
}






void shell_exec(u8 *data, int len)
{
    int nr;
    stShellPkt_t shellPkg;
    
    memset((u8 *)&shellPkg, '\0', sizeof(shellPkg));
    
    if(parseShellCmd(data, &shellPkg))
    {
        for(nr = 0; nr < stShellCmdMap.i32CmdNum; nr++)
        {
            if((strlen(stShellCmdMap.pCmd[nr]->pCmd) == shellPkg.cmdLen)
            && (memcmp(stShellCmdMap.pCmd[nr]->pCmd, shellPkg.cmd, strlen(stShellCmdMap.pCmd[nr]->pCmd)) == 0))
            {
                if(stShellCmdMap.pCmd[nr]->pFunc)
                {
                    stShellCmdMap.pCmd[nr]->pFunc(&shellPkg);
                }
                
                break;
            }
        }
    }
}





void shell_read(eUsartNum_t eUsartNum, i32 i32Len, u8 *pBuf)
{
    i32 i;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pBuf[i])
        {
        case 8: //backspace
            sUsartDrvSend(eUsartNum, 1, (const u8 *)"\b");
            if(stShellCache.u16RxCnt > 0)
            {
                stShellCache.u16RxCnt--;
            }
            break;
        case 13: //enter
        case 10: //enter
            stShellCache.u8ShellBuf[stShellCache.u16RxCnt] = '\0';
            sUsartDrvSend(eUsartNum, 2, (const u8 *)cCrLf);
            if(stShellCache.u16RxCnt)
            {
                shell_exec(stShellCache.u8ShellBuf, stShellCache.u16RxCnt);
            }
            memset(stShellCache.u8ShellBuf, 0, sizeof(stShellCache.u8ShellBuf));
            stShellCache.u16RxCnt = 0;
            stShellCache.bRxFlag = false;
            break;
        case 37: //left
        case 38: //up
        case 39: //right
        case 40: //down
        case 9:  //tab
        case 127: //del
            stShellCache.bRxFlag = true;
            //stShellCache.u32BeginTime = sGetTimestamp();
            break;
        default:
            stShellCache.u8ShellBuf[stShellCache.u16RxCnt++] = pBuf[i];
            sUsartDrvSend(eUsartNum, 1, (const u8 *)&(pBuf[i]));
            stShellCache.bRxFlag = true;
            //stShellCache.u32BeginTime = sGetTimestamp();
            
            if(stShellCache.u16RxCnt >= cShellBufSize)
            {
                memset(stShellCache.u8ShellBuf, 0, sizeof(stShellCache.u8ShellBuf));
                stShellCache.u16RxCnt = 0;
                stShellCache.bRxFlag = false;
            }
            break;
        }
    }
    
    if(stShellCache.bRxFlag)
    {
        if(0)//(sGetTimestamp() - stShellCache.u32BeginTime > 30)
        {
            stShellCache.u16RxCnt = 0;
            stShellCache.bRxFlag = false;
        }
    }
}






bool shell_list_dir(const stShellPkt_t *pkg)
{
    i32  i;
    
    
    EN_SLOGI(TAG, "#Shell界面支持的命令列表:");
    for(i = 0; i < stShellCmdMap.i32CmdNum; i++)
    {
        EN_SLOGI(TAG, "#---> %s",   stShellCmdMap.pCmd[i]->pCmd);
        EN_SLOGI(TAG, "      %s",   stShellCmdMap.pCmd[i]->pFormat);
        EN_SLOGI(TAG, "      %s",   stShellCmdMap.pCmd[i]->pFunction);
        EN_SLOGI(TAG, "      %s\n", stShellCmdMap.pCmd[i]->pRemarks);
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    return(true);
}














































