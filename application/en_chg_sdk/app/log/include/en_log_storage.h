/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_log_storage.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-8-13
 * @Attention             :     
 * @Brief                 :     
 * 
 * @History:
 * 
 * 1.@Date: 2024-8-13
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _en_log_storage_H
#define _en_log_storage_H

#include "en_common.h"
#include "en_queue.h"






//定义单条日志储存的最大长度
#define cLogStorageBufSize              (2048)




//log日志文件路径长度
#define cNetAppLogPathLen               (128)
#define cNetAppLogRemainingCap          (32)



//log日志文件存储路径
#define cNetAppLogPath                  "/log"



//原始日志文件的后缀类型
#define cNetAppLogFileTypeLog           ".log"
#define cNetAppLogFileTypeZip           ".zip"





#define cNetAppLogGenLen1               256
#define cNetAppLogGenLen2               64
#define cNetAppLogTimeLen               10

//log上传任务参数结构体
typedef struct
{
    u16                                 u16Port;
    char                                u8Host[cNetAppLogGenLen1 + 1];          //格式:127.0.0.1:8080
    char                                u8Username[cNetAppLogGenLen2 + 1];
    char                                u8Password[cNetAppLogGenLen2 + 1];
    char                                u8TargetDirectory[cNetAppLogGenLen2 + 1];
    char                                u8QueryTime[cNetAppLogTimeLen + 1];     //所查询的日期，格式：2022-10-09
    stFullTime_t                        stTime;                                 //所查询的日期，结构体形式
}stNetAppLogPostTaskParam_t;






//日志储存组件缓存结构
typedef struct
{
    bool                                bLogStoreSwitch;                        //日志储存开关
    
    //日志缓存队列
    stQueue_t                           stLogQueue;                             //日志储存队列
    SemaphoreHandle_t                   xSemLogQueue;                           //日志储存队列访问信号量
    
    //日志储存写buf
    u8                                  u8WriteBuf[cLogStorageBufSize];         
}stLogStoreCache_t;












extern bool sEnLogStorageInit(void);

extern bool sEnlogStorageDataPush(i32 i32Len, const u8* pData);


extern void sNetAppLogInit(void);

extern bool sNetAppLogPostTaskCreat(const stNetAppLogPostTaskParam_t *pParam);

extern bool sNetAppLogPostTaskSts(void);

extern void sNetAppLogPathGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pPath);
extern void sNetAppLogNameGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pName);









#endif
