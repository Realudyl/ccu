/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_log.c
* Description                           :   日志打印功能
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-18
* notice                                :   
****************************************************************************************************/
#include <stdio.h>
#include <stdarg.h>

#include "en_log.h"
#include "en_log_storage.h"
#include "en_mem.h"
#include "en_shell.h"

#include "flash_drv_gd5f4gm8_api.h"
#include "usart_drv_trans.h"


//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "en_log";




stLogCache_t stLogCache;










bool sEnLogInit(void);
void sEnLogTask(void *pvParam);
void sEnLogPrintSwitch(bool bSwitch);
void sEnlogString(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pFmt, ...);
void sEnlogHex(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pString, const u8 *pLog, i32 i32Len);





//shell相关
bool sEnlogShellLogSwitch(const stShellPkt_t *pkg);



stShellCmd_t stSellCmdLogSwtichCmd = 
{
    .pCmd       = "logswitch",
    .pFormat    = "格式:logswitch 0/1",
    .pFunction  = "功能:开启或关闭日志打印功能",
    .pRemarks   = "备注:参数值, 0---关闭, 1---开启",
    .pFunc      = sEnlogShellLogSwitch,
};









/**********************************************************************************************
* Description                           :   日志打印功能 资源初始化
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   
***********************************************************************************************/
bool sEnLogInit(void)
{
    bool bRst;
    
    memset(&stLogCache, 0, sizeof(stLogCache));
    stLogCache.bLogPrintSwitch = cLogPrintSwitch;
    stLogCache.xSemLogQueue = xSemaphoreCreateBinary();
    xSemaphoreGive(stLogCache.xSemLogQueue);
    bRst = en_queueInit(&stLogCache.stLogQueue, cLogBufSize * 4 , 100, 0);
    if(bRst == false)
    {
        return(false);
    }
    
    
    if(pdPASS != xTaskCreate(sEnLogTask, "sEnLogTask", (1536), NULL, 30, NULL))
    {
        bRst = false;
    }
    
    if(sFlashDrvApiGetInitState() == true)
    {
        //日志储存功能资源初始化
        bRst &= sEnLogStorageInit();
    }
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdLogSwtichCmd);
    
    return(bRst);
}










/**********************************************************************************************
* Description                           :   日志打印任务
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   
***********************************************************************************************/
void sEnLogTask(void *pvParam)
{
    i32 i32Len;
    bool bFirstPrint = true;
    
    while(1)
    {
        if(en_queueSize(&stLogCache.stLogQueue) > 0)
        {
            memset(stLogCache.u8TxBuf, 0, sizeof(stLogCache.u8TxBuf));
            xSemaphoreTake(stLogCache.xSemLogQueue, portMAX_DELAY);
            i32Len = en_queuePop(&stLogCache.stLogQueue, NULL, stLogCache.u8TxBuf, sizeof(stLogCache.u8TxBuf));
            xSemaphoreGive(stLogCache.xSemLogQueue);
            
            if(i32Len > 0)
            {
                sUsartDrvSend(cShellComUartNum, i32Len, stLogCache.u8TxBuf);
                
                if(sFlashDrvApiGetInitState() == true)
                {
                    //将打印的日志push到日志储存队列里, 由"en_log_storage.c"中的sEnLogStorageTask任务进行出队写入FLASH处理
                    sEnlogStorageDataPush(i32Len, stLogCache.u8TxBuf);
                }
                else if(bFirstPrint == true)
                {
                    bFirstPrint = false;
                    EN_SLOGE(TAG, "日志储存功能未能初始化, 原因 ==> FLASH初始化状态: FALSE, 请检查FLASH初始化函数 或 检查硬件是否支持!");
                }
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}









/**********************************************************************************************
* Description                           :   日志打印开关
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   
***********************************************************************************************/
void sEnLogPrintSwitch(bool bSwitch)
{
    stLogCache.bLogPrintSwitch = (bSwitch == false) ? bSwitch : true;
}









/**********************************************************************************************
* Description                           :   日志打印功能 字符串日志生成函数
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   本函数不可在中断内调用
***********************************************************************************************/
void sEnlogString(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pFmt, ...)
{
    va_list list;
    
    stFullTime_t stTime;
    u8   u8TagLen;
    char u8FileTag[cLogTagLen];
    i32  i32FmtLen;
    i32  i32LogLen;
    char *pStr = NULL;
    
    
    //1:日志打印开关
    if(stLogCache.bLogPrintSwitch != true)
    {
         return;
    }
    
    
    //3:上锁 200ms超时
    if(xSemaphoreTake(stLogCache.xSemLogQueue, 200 / portTICK_RATE_MS) != pdTRUE)
    {
         return;
    }
    pStr = stLogCache.u8LogBuf;
    memset(pStr,  0, cLogBufSize);
    
    
    //4:日志内容生成
    memset(&stTime,  0, sizeof(stTime));
    stTime = sGetFullTime();
    u8TagLen = (strlen(pTag) > sizeof(u8FileTag)) ? sizeof(u8FileTag) : strlen(pTag);
    memset(u8FileTag, ' ', sizeof(u8FileTag));                                 //将文件名用空格符填充到长度固定 超过的部分截断
    memcpy(u8FileTag, pTag, u8TagLen);
    u8FileTag[sizeof(u8FileTag) - 1] = 0;
    
    //4.1:生成日志head部分
    snprintf(pStr, cLogBufSize, "%s (%04d-%02d-%02d %02d:%02d:%02d) [%s | %05d]: ",
             pLvl,
             stTime.year, stTime.mon, stTime.day,
             stTime.hour, stTime.min, stTime.sec,
             u8FileTag,
             i32Line);
    
    i32LogLen = strlen(pStr);
    pStr = pStr + i32LogLen;
    
    
    //4.2:生成日志内容部分      这里是为了留足空间避免字符串长度打满
    va_start(list, pFmt);
    i32FmtLen = vsnprintf(pStr, cLogBufSize - i32LogLen - strlen(cCrLfCrLf), pFmt, list);
    va_end(list);
    i32LogLen = i32LogLen + i32FmtLen;
    
    //4.3:补上回车换行
    strcat(pStr, cCrLf);
    i32LogLen = i32LogLen + strlen(cCrLf);
    
    //4.4:错误检查
    if(i32LogLen >= cLogBufSize)
    {
        i32LogLen = strlen("sEnlogString err 溢出\r\n");
        snprintf(pStr, cLogBufSize, "sEnlogString err 溢出\r\n");
    }
    
    //5:日志打印---已经上锁了 xSemLogQueue
    en_queuePush(&stLogCache.stLogQueue, NULL, (const u8 *)stLogCache.u8LogBuf, i32LogLen);
    
    
    //6:解锁
    xSemaphoreGive(stLogCache.xSemLogQueue);
}









/**********************************************************************************************
* Description                           :   日志打印功能 HEX序列日志生成函数
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   本函数不可在中断内调用
***********************************************************************************************/
void sEnlogHex(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pString, const u8 *pLog, i32 i32Len)
{
    i32  i, j;
    stFullTime_t stTime;
    
    u8   u8TagLen;
    char u8FileTag[cLogTagLen];
    i32  i32FmtLen;
    i32  i32LogLen, i32LogLenWithoutHexData;
    char *pStr = NULL;
    
    
    //1:日志打印开关
    if(stLogCache.bLogPrintSwitch != true)
    {
         return;
    }
    
    
    //3:上锁 200ms超时
    if(xSemaphoreTake(stLogCache.xSemLogQueue, 200 / portTICK_RATE_MS) != pdTRUE)
    {
         return;
    }
    pStr = stLogCache.u8LogBuf;
    memset(pStr,  0, cLogBufSize);
    
    
    //4:日志内容生成
    memset(&stTime,  0, sizeof(stTime));
    stTime = sGetFullTime();
    u8TagLen = (strlen(pTag) > sizeof(u8FileTag)) ? sizeof(u8FileTag) : strlen(pTag);
    memset(u8FileTag, ' ', sizeof(u8FileTag));                                 //将文件名用空格符填充到长度固定 超过的部分截断
    memcpy(u8FileTag, pTag, u8TagLen);
    u8FileTag[sizeof(u8FileTag) - 1] = 0;
    
    //4.1:生成日志head部分
    snprintf(pStr, cLogBufSize, "%s (%04d-%02d-%02d %02d:%02d:%02d) [%s | %05d]: ",
             pLvl,
             stTime.year, stTime.mon, stTime.day,
             stTime.hour, stTime.min, stTime.sec,
             u8FileTag,
             i32Line);
    
    i32LogLen = strlen(pStr);
    pStr = pStr + i32LogLen;
    
    //4.2:加入pString部分
    snprintf(pStr, cLogBufSize - i32LogLen, "%s",pString);
    i32LogLen = i32LogLen + strlen(pString);
    i32LogLenWithoutHexData = i32LogLen;                                        //记录此刻日志字符串已经达到的长度
    pStr = pStr +  strlen(pString);
    
    //4.3:生成日志内容部分----这里的 2 是每个HEX字节打印成字符串的长度
    for(i = 0, j = 0; ((i < i32Len) && (j < (cLogBufSize - i32LogLenWithoutHexData - strlen(cCrLf)))); i++, j += 2)
    {
        snprintf(pStr + j, 3, "%02X", pLog[i]);
        i32LogLen = i32LogLen + 2;
    }
    
    //4.4:补上回车换行
    strcat(pStr, cCrLf);
    i32LogLen = i32LogLen + strlen(cCrLf);
    
    //5:日志打印---已经上锁了 xSemLogQueue
    en_queuePush(&stLogCache.stLogQueue, NULL, (const u8 *)stLogCache.u8LogBuf, i32LogLen);
    
    
    //6:解锁
    xSemaphoreGive(stLogCache.xSemLogQueue);
}









/**********************************************************************************************
* Description                           :   日志打印 开关命令
* Author                                :   Hall
* modified Date                         :   2023-12-18
* notice                                :   
***********************************************************************************************/
bool sEnlogShellLogSwitch(const stShellPkt_t *pkg)
{
    bool bSwitch;
    char u8String[2][16] = { "关闭", "开启"};
    
    if(pkg->paraNum != 1)
    {
        EN_SLOGE(TAG, "指令格式错误, 参数个数不对");
        return(false);
    }
    
    bSwitch = (atoi(pkg->para[0]) == 0) ? false : true;
    EN_SLOGI(TAG, "%s日志打印", u8String[bSwitch]);
    vTaskDelay(20 / portTICK_RATE_MS);
    sEnLogPrintSwitch(bSwitch);
    
    return(true);
}








