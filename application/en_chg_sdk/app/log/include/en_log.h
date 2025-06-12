/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_log.h
* Description                           :   日志打印功能
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-18
* notice                                :   
****************************************************************************************************/
#ifndef _en_log_H_
#define _en_log_H_

#include "en_common.h"
#include "en_queue.h"










//定义单条日志的最大长度
#define cLogBufSize                     (2048)


//日志中 tag 标签的长度
#define cLogTagLen                      (20)













//日志组件缓存结构
typedef struct
{
    bool                                bLogPrintSwitch;                        //日志打印开关
    
    //日志缓存队列
    stQueue_t                           stLogQueue;                             //日志队列
    SemaphoreHandle_t                   xSemLogQueue;                           //日志队列访问信号量
    char                                u8LogBuf[cLogBufSize];                  //日志生成buf---此buf同样由 xSemLogQueue 保护
    
    //日志打印发送buf
    u8                                  u8TxBuf[cLogBufSize];                   
}stLogCache_t;








//日志打印开关
#ifndef cSdkLogPrintSwitch
#define cLogPrintSwitch                 (false)
#else
#define cLogPrintSwitch                 (cSdkLogPrintSwitch)
#endif








//日志等级定义
typedef enum
{
    eEnLogLevelError                    = 0,
    eEnLogLevelWarn                     = 1,
    eEnLogLevelInfo                     = 2,
    eEnLogLevelDebug                    = 3,
    
    eEnLogLevelMax,
}eEnLogLevel_t;


















//字符串打印宏
#define EN_SLOG_LEVEL(level, color_n, tag, format, ...)\
    do\
    {\
        if(cLogLevel >= level)\
        {\
            if      (level == eEnLogLevelError ) { sEnlogString("E", color_n,  tag, __LINE__,   format, ##__VA_ARGS__); } \
            else if (level == eEnLogLevelWarn  ) { sEnlogString("W", color_n,  tag, __LINE__,   format, ##__VA_ARGS__); } \
            else if (level == eEnLogLevelInfo  ) { sEnlogString("I", color_n,  tag, __LINE__,   format, ##__VA_ARGS__); } \
            else if (level == eEnLogLevelDebug ) { sEnlogString("D", color_n,  tag, __LINE__,   format, ##__VA_ARGS__); } \
            else                                 { sEnlogString("D", color_n,  tag, __LINE__,   format, ##__VA_ARGS__); } \
        }\
    }while(0)


//hex序列打印宏
#define EN_HLOG_LEVEL(level, color_n, tag, string, log, len)\
    do\
    {\
        if(cLogLevel >= level)\
        {\
            if      (level == eEnLogLevelError ) { sEnlogHex("E", color_n, tag, __LINE__, string, log, len); } \
            else if (level == eEnLogLevelWarn  ) { sEnlogHex("W", color_n, tag, __LINE__, string, log, len); } \
            else if (level == eEnLogLevelInfo  ) { sEnlogHex("I", color_n, tag, __LINE__, string, log, len); } \
            else if (level == eEnLogLevelDebug ) { sEnlogHex("D", color_n, tag, __LINE__, string, log, len); } \
            else                                 { sEnlogHex("D", color_n, tag, __LINE__, string, log, len); } \
        }\
    }while(0)








//以下日志打印函数均不可在中断内调用
#define EN_SLOGD(tag, fmt, ...)         EN_SLOG_LEVEL(eEnLogLevelDebug, 0,  tag, fmt, ##__VA_ARGS__)
#define EN_SLOGI(tag, fmt, ...)         EN_SLOG_LEVEL(eEnLogLevelInfo,  32, tag, fmt, ##__VA_ARGS__)
#define EN_SLOGW(tag, fmt, ...)         EN_SLOG_LEVEL(eEnLogLevelWarn,  33, tag, fmt, ##__VA_ARGS__)
#define EN_SLOGE(tag, fmt, ...)         EN_SLOG_LEVEL(eEnLogLevelError, 31, tag, fmt, ##__VA_ARGS__)

#define EN_HLOGD(tag, string, log, len) EN_HLOG_LEVEL(eEnLogLevelDebug, 0,  tag, string, log, len)
#define EN_HLOGI(tag, string, log, len) EN_HLOG_LEVEL(eEnLogLevelInfo,  32, tag, string, log, len)
#define EN_HLOGW(tag, string, log, len) EN_HLOG_LEVEL(eEnLogLevelWarn,  33, tag, string, log, len)
#define EN_HLOGE(tag, string, log, len) EN_HLOG_LEVEL(eEnLogLevelError, 31, tag, string, log, len)












extern bool sEnLogInit(void);
extern void sEnLogPrintSwitch(bool bSwitch);
extern void sEnlogString(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pFmt, ...);
extern void sEnlogHex(const char *pLvl, u8 u8Color, const char *pTag, i32 i32Line, const char *pString, const u8 *pLog, i32 i32Len);













#endif





































