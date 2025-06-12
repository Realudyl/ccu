/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_mem.c
* Description                           :   动态内存分配部分
* Version                               :   
* Author                                :   LV
* Creat Date                            :   2023-04-11
* notice                                :   内存分配、内存释放、内存获取
****************************************************************************************************/
#include "en_mem.h"

#include "en_log.h"

#include "task.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "en_mem";







/**********************************************************************************************
* Description                           :   内存获取函数
* Author                                :   LV
* modified Date                         :   2023-02-16
* notice                                :   单独打印使用情况
***********************************************************************************************/
void sys_print_memory(void)
{
    EN_SLOGD(TAG, "Minimum:%d FreeHeap:%d", xPortGetMinimumEverFreeHeapSize(), xPortGetFreeHeapSize());
}







/**********************************************************************************************
* Description                           :   内存获取函数
* Author                                :   LV
* modified Date                         :   2023-02-16
* notice                                :   获取内存分配最小剩余和当前剩余容量
***********************************************************************************************/
void sys_print_memory_used_info(void)
{
    #define PRINTMAX  1024
    static char *pStr = NULL;
    
    if(pStr == NULL)
    {
       pStr =(char *)MALLOC(PRINTMAX);
    }
    if(pStr ==NULL)
    {
        EN_SLOGE(TAG, " pStr MALLOC err...");
        return;
    }
    
    memset(pStr, 0, PRINTMAX);
    
    EN_SLOGD(TAG, "Minimum:%d FreeHeap:%d", xPortGetMinimumEverFreeHeapSize(), xPortGetFreeHeapSize());
    
    strcat((char *)pStr, "\r\n");
    
    strcat((char *)pStr, "name          stat     pri  lestack  seri\r\n" );
    
    strcat((char *)pStr, "---------------------------------------------\r\n");
    
    /* The list of tasks and their status */
    
    vTaskList((char *)(pStr + strlen(pStr)));
    
    strcat((char *)pStr, "\r\n---------------------------------------------\r\n");
    
    strcat((char *)pStr, "B : 阻塞, R : 就绪, D : 删除, S : 暂停\r\n");
    
    EN_SLOGD(TAG, "%s",pStr);
    
}









/**********************************************************************************************
* Description                           :   内存分配函数
* Author                                :   LV
* modified Date                         :   2023-02-16
* notice                                :   利用freertos系统函数调用封装
***********************************************************************************************/
void *MALLOC(u32 size)
{
    return pvPortMalloc(size);
}








/**********************************************************************************************
* Description                           :   内存释放函数
* Author                                :   LV
* modified Date                         :   2023-02-16
* notice                                :   利用freertos系统函数调用封装
***********************************************************************************************/
void FREE(void *ptr)
{
    vPortFree(ptr);
}






/**********************************************************************************************
* Description                           :   堆栈溢出检测钩子函数 
* Author                                :   Hall
* modified Date                         :   2024-01-09
* notice                                :   <FreeRTOSConfig.h>文件内 configCHECK_FOR_STACK_OVERFLOW 参数决定是否执行
***********************************************************************************************/
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
    EN_SLOGD(TAG, "任务：%s 发现栈溢出", pcTaskName);
}











