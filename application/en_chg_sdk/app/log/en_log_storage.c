/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_log_storage.c
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
#include <stdio.h>

#include "en_log_storage.h"

#include "eeprom_app_factory.h"
#include "en_common.h"
#include "en_littlefs.h"
#include "en_log.h"
#include "en_mem.h"
#include "en_shell.h"
//#include "en_zip.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "en_log_storage";




stLogStoreCache_t stLogStoreCache;
//extern stZipTaskParam_t stZipParam;








bool sEnLogStorageInit(void);


void sEnLogStorageTask(void *pvParam);
void sEnLogShellReadDirTask(void *pParam);
void sEnLogShellRemoveFileTask(void *pvParam);


bool sEnlogStorageDataPush(i32 i32Len, const u8* pData);


bool sEnlogShellLittleFsCmd(const stShellPkt_t *pkg);



void sNetAppLogManage(bool *pFlag, char **pPath);
void sNetAppLogDateChange(stFullTime_t stTime, stFullTime_t stTimeOld, char **pPath);
void sNetAppLogOldCheck(stFullTime_t stTime);
void sNetAppLogDelete(stFullTime_t stTimeOld);
bool sNetAppLogMemoryCheck(void);
void sNetAppLogPathGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pPath);
void sNetAppLogNameGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pName);






stShellCmd_t stSellCmdLittleFsCmd = 
{
    .pCmd       = "lfscmd",
    .pFormat    = "格式:lfscmd 命令 文件路径",
    .pFunction  = "功能:LittleFS操作命令",
    .pRemarks   = "备注:lfscmd opendir /log         ———— 打开/log目录\
\n\r\t\t\t\t\t\t\t\t  lfscmd closedir             ———— 关闭当前所在目录\
\n\r\t\t\t\t\t\t\t\t  lfscmd readdir              ———— 浏览当前所在目录\
\n\r\t\t\t\t\t\t\t\t  lfscmd remove /log/xxx.log  ———— 删除指定文件\
\n\r\t\t\t\t\t\t\t\t  lfscmd memszie              ———— 查看当前文件系统已使用块个数",
    .pFunc      = sEnlogShellLittleFsCmd,
};






/*******************************************************************************
 * @FunctionName   :      sEnLogStorageInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  17:52:29
 * @Description    :      日志储存功能 资源初始化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sEnLogStorageInit(void)
{
    bool bRst;
    
    memset(&stLogStoreCache, 0, sizeof(stLogStoreCache));
    stLogStoreCache.xSemLogQueue = xSemaphoreCreateBinary();
    xSemaphoreGive(stLogStoreCache.xSemLogQueue);
    bRst = en_queueInit(&stLogStoreCache.stLogQueue, cLogStorageBufSize * 4 , 100, 0);
    if(bRst == false)
    {
        return(false);
    }
    
    //初始化zip压缩资源
//    sZipInit();
    
    if(pdPASS != xTaskCreate(sEnLogStorageTask, "sEnLogStorageTask", (3*1024), NULL, 19, NULL))
    {
        bRst = false;
    }
    
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdLittleFsCmd);
    
    return (bRst);
}






/*******************************************************************************
 * @FunctionName   :      sEnLogStorageTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  17:15:36
 * @Description    :      日志储存任务
 * @Input          :      pvParam     
*******************************************************************************/
void sEnLogStorageTask(void *pvParam)
{
    bool bLfsRst = false;
    i32 i32Len = -1;
    u8 u8WriteFailCount = 0;
    
    bool bFullFlag = false;             //储存空间是否满了
    char *pPath = NULL;                 //日志文件路径
    
    lfs_file_t stFile;
    struct lfs_info stLfsInfo;
    
    vTaskDelay(5000 / portTICK_RATE_MS);
    
    // 挂载LittleFs文件系统
    bLfsRst = mount();
    
    // 检查/log目录是否存在
    if(pathstat(cNetAppLogPath, &stLfsInfo) == false)
    {
        //若不存在, 则创建/log目录
        d_Open(cNetAppLogPath);
    }
    
    while(bLfsRst)
    {
        if(bFullFlag == false)
        {
            //1:日志管理并生成日志路径
            sNetAppLogManage(&bFullFlag, &pPath);
            if((pPath == NULL) || (bFullFlag == true))
            {
                //增加一个容错保护
                vTaskDelay(1000 / portTICK_RATE_MS);
                continue;
            }
            
            while(en_queueSize(&stLogStoreCache.stLogQueue) > 0)
            {
                if(stLogStoreCache.bLogStoreSwitch == false)
                {
                    //2:打开日志文件
                    stLogStoreCache.bLogStoreSwitch = f_Open(&stFile, pPath);
                }
                
                if(stLogStoreCache.bLogStoreSwitch == true)
                {
                    //3:写入日志内容
                    memset(stLogStoreCache.u8WriteBuf, 0, sizeof(stLogStoreCache.u8WriteBuf));
                    xSemaphoreTake(stLogStoreCache.xSemLogQueue, portMAX_DELAY);
                    i32Len = en_queuePop(&stLogStoreCache.stLogQueue, NULL, stLogStoreCache.u8WriteBuf, sizeof(stLogStoreCache.u8WriteBuf));
                    xSemaphoreGive(stLogStoreCache.xSemLogQueue);
                    if(i32Len > 0)
                    {
                        u8WriteFailCount = 0;
                        
                        while(false == f_Write(&stFile, stLogStoreCache.u8WriteBuf, i32Len))
                        {
                            vTaskDelay(20 / portTICK_RATE_MS);
                            u8WriteFailCount ++;
                            if(u8WriteFailCount >= 100)
                            {
                                break;
                            }
                        }
                        if(u8WriteFailCount >= 100)
                        {
                            break;
                        }
                    }
                }
                
                vTaskDelay(20 / portTICK_RATE_MS);
            }
            
            if(stLogStoreCache.bLogStoreSwitch == true)
            {
                //4:关闭日志文件
                f_Close(&stFile);
                stLogStoreCache.bLogStoreSwitch = false;
            }
        }
        
        vTaskDelay(500 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}






/*******************************************************************************
 * @FunctionName   :      sEnLogShellReadDirTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月9日  17:45:11
 * @Description    :      日志储存 ———— Shell命令浏览目录操作任务
 * @Input          :      pParam      
*******************************************************************************/
void sEnLogShellReadDirTask(void *pParam)
{
    bool bRst = false;
    struct lfs_info stLfsInfo;
    u16 u16FileCount = 0;
    
    //将目录的位置更改为目录的开头
    if(d_Rewind() != true)
    {
        vTaskDelete(NULL);
    }
    
    do
    {
        //循环读取目录中的文件项
        bRst = d_Read(&stLfsInfo);
        if(bRst != true)
        {
            break;
        }
        
        switch (stLfsInfo.type)
        {
        case LFS_TYPE_REG:
            // 打印读取到的文件项
            EN_SLOGI(TAG, "(%05d) 文件名: %-30s, 文件大小: %-10.2lf Kbytes, 文件类型: %s", u16FileCount, stLfsInfo.name, (f32)(stLfsInfo.size / 1024.0), "文件");
            u16FileCount ++;
            break;
            
        case LFS_TYPE_DIR:
            // 打印读取到的文件项
            if((strcmp(stLfsInfo.name, ".") == 0) || (strcmp(stLfsInfo.name, "..") == 0))
            {
                break;
            }
            EN_SLOGI(TAG, "(%05d) 文件名: %-30s,                              文件类型: %s", u16FileCount, stLfsInfo.name, "目录");
            u16FileCount ++;
            break;
            
        default:
            break;
        }
    }while(bRst);
    
    
    vTaskDelete(NULL);
}






/*******************************************************************************
 * @FunctionName   :      sEnLogShellRemoveFileTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月2日  19:23:58
 * @Description    :      日志储存 ———— Shell命令删文件操作任务
 * @Input          :      pvParam     
*******************************************************************************/
void sEnLogShellRemoveFileTask(void *pParam)
{
    u32 u32StartSec = 0;
    const stShellPkt_t *pkg = (stShellPkt_t *)pParam;
    
    u32StartSec = sGetTimestamp();
    
    remove((const char *)pkg->para[1]);
    
    for(i8 i = 0; i < pkg->paraNum; i++)
    {
        FREE(pkg->para[i]);
    }
    
    EN_SLOGD(TAG, "Shell命令删除文件任务运行结束! 耗时:%dS", (u32)(sGetTimestamp()- u32StartSec));
    
    vTaskDelete(NULL);
}








/*******************************************************************************
 * @FunctionName   :      sEnlogStorageDataPush
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  19:14:50
 * @Description    :      日志储存 数据推入队列
 * @Input          :      i32Len      要推入的数据长度
 * @Input          :      pData       指向要推入的数据指针
 * @Return         :      
*******************************************************************************/
bool sEnlogStorageDataPush(i32 i32Len, const u8* pData)
{
    bool bRst = false;
    
    xSemaphoreTake(stLogStoreCache.xSemLogQueue, portMAX_DELAY);
    bRst = en_queuePush(&stLogStoreCache.stLogQueue, NULL, pData, i32Len);
    xSemaphoreGive(stLogStoreCache.xSemLogQueue);
    
    return (bRst);
}




/*******************************************************************************
 * @FunctionName   :      sEnlogShellLittleFsCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  17:01:00
 * @Description    :      日志打印 LittleFS操作命令
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellLittleFsCmd(const stShellPkt_t *pkg)
{
    static stShellPkt_t stPkg;
    
    bool bRst = false;
    i32 i32Size = 0;
    struct lfs_info *pLfsInfo = NULL;
    u16 u16FileCount = 0;
    
    memset(&stPkg, 0, sizeof(stShellPkt_t));
    
    if(strcmp((const char *)pkg->para[0], "opendir") == 0)                      //打开目录
    {
        return (d_Open((const char *)pkg->para[1]));
    }
    else if(strcmp((const char *)pkg->para[0], "closedir") == 0)                //关闭目录
    {
        return (d_Close());
    }
    else if(strcmp((const char *)pkg->para[0], "readdir") == 0)                 //读取目录
    {
        if(pdPASS != xTaskCreate(sEnLogShellReadDirTask, "sEnLogShellReadDirTask", (1024), (void * const)&stPkg, 10, NULL))
        {
            EN_SLOGE(TAG,  "sEnLogShellReadDirTask 创建失败!");
            return (false);
        }
        
        return (true);
    }
    else if(strcmp((const char *)pkg->para[0], "remove") == 0)                  //删除文件
    {
        if(pkg->paraNum == 2)
        {
            stPkg.paraNum = pkg->paraNum;
            for(i8 i = 0; i < pkg->paraNum; i++)
            {
                if(stPkg.para[i] == NULL)
                {
                    stPkg.para[i] = (char *)MALLOC(pkg->paraLen[i]);
                }
                strcpy(stPkg.para[i], pkg->para[i]);
                stPkg.paraLen[i] = pkg->paraLen[i];
            }
            
            if(pdPASS != xTaskCreate(sEnLogShellRemoveFileTask, "sEnLogShellRemoveFileTask", (1024), (void * const)&stPkg, 10, NULL))
            {
                EN_SLOGE(TAG,  "sEnLogShellRemoveFileTask 创建失败!");
                return (false);
            }
        }
        else
        {
            EN_SLOGE(TAG,  "指令格式错误!");
            return(false);
        }
        
        return (true);
    }
    else if(strcmp((const char *)pkg->para[0], "memsize") == 0)                 //查看系统已使用大小
    {
        i32Size = memsize();
        if(i32Size >= 0)
        {
            EN_SLOGI(TAG, "当前系统已使用块数量为 %d ", i32Size);
        }
        else
        {
            EN_SLOGE(TAG, "memsize error code: %d, 停止存储调试日志", i32Size);
        }
        return (true);
    }
    else
    {
        EN_SLOGE(TAG, "无效命令");
    }
    
    return (false);
}






/**********************************************************************************************
* Description       :     日志管理功能
* Author            :     Hall
* modified Date     :     2022-10-15
* notice            :     主要负责：日期变更逻辑和1小时主动检查旧日志逻辑
***********************************************************************************************/
void sNetAppLogManage(bool *pFlag, char **pPath)
{
    bool bFlag;
    
    stFullTime_t stTime;
    static stFullTime_t stTimeOld;
    
    bFlag = false;
    stTime = sGetFullTime();
    
    if((stTimeOld.year != stTime.year)
    || (stTimeOld.mon  != stTime.mon )
    || (stTimeOld.day  != stTime.day ))
    {
        //日期变更时逻辑
        sNetAppLogDateChange(stTime, stTimeOld, pPath);
        bFlag = sNetAppLogMemoryCheck();
        
        //更新stTimeOld
        memset(&stTimeOld, 0, sizeof(stFullTime_t));
        memcpy(&stTimeOld, &stTime, sizeof(stFullTime_t));
    }
    else if(stTimeOld.hour != stTime.hour)
    {
        //每个小时触发一次旧日志检查逻辑
//        sNetAppLogOldCheck(stTime);
        
        //更新 hour
        stTimeOld.hour = stTime.hour;
    }
    
    if(pFlag != NULL)
    {
        (*pFlag) = bFlag;
    }
    
}




/**********************************************************************************************
* Description       :     日期变更处理
* Author            :     Hall
* modified Date     :     2022-09-21
* notice            :     当日期发生变更时，需要生成新的log日志文件名和路径，由本函数负责实现
***********************************************************************************************/
void sNetAppLogDateChange(stFullTime_t stTime, stFullTime_t stTimeOld, char **pPath)
{
    static char u8OldPath[cNetAppLogPathLen] = { 0 };                           //日志文件路径
    
    EN_SLOGI(TAG, "日志文件管理:日期变更逻辑执行");
    EN_SLOGI(TAG, "stTime   :%04d-%02d-%02d", stTime.year, stTime.mon, stTime.day);
    EN_SLOGI(TAG, "stTimeOld:%04d-%02d-%02d", stTimeOld.year, stTimeOld.mon, stTimeOld.day);
    
    //1：启动压缩任务 将上一日的文件压缩为zip
//    if(stTimeOld.year > 1970)                                                   //避免 stChgTimeOld 的初值0 和无效年份1970触发压缩任务
//    {
//        stZipParam.bDeleteFlag = true;                                          //日期变更触发压缩 需要删除原始文件
//        memset(stZipParam.u8Path, 0, sizeof(stZipParam.u8Path));
//        memcpy(stZipParam.u8Path, u8OldPath, sizeof(u8OldPath));
//        sZipTaskCreat(&stZipParam);
//    }
    
    //2：生成新一天log文件的路径
    memset(u8OldPath, 0, sizeof(u8OldPath));
    sNetAppLogPathGenerate(stTime, sizeof(u8OldPath), cNetAppLogFileTypeLog, u8OldPath);
    EN_SLOGI(TAG, "生成新路径->:%s", u8OldPath);
    
    //3：删除相应的日志文件
    sNetAppLogDelete(stTimeOld);
    
    (*pPath) = u8OldPath;
}




/**********************************************************************************************
* Description       :     旧日志的检查
* Author            :     Hall
* modified Date     :     2022-10-15
* notice            :     实际场景下，业主经常给桩断电，要用的时候才上电，充完又断电了。导致当天
*                         的log文件无法在第二天的0点时刻触发压缩动作，多次这样的操作以后将会导致
*                         存储空间耗尽，
*                         因此需要增加本函数，定时检查存储器内日志文件，对于过去的日期尚未压缩的
*                         日志文件执行压缩功能。
***********************************************************************************************/
void sNetAppLogOldCheck(stFullTime_t stTime)
{
    int i32Year = 0;
    int i32Mon = 0;
    int i32Day = 0;
    
    char *pDate = NULL;
    
    struct lfs_info stInfo;
    
    EN_SLOGI(TAG, "日志文件管理:未压缩旧日志检查逻辑执行");
    
    if(d_Open(cNetAppLogPath) == true)
    {
        while(d_Read(&stInfo) == true)
        {
            if(stInfo.type == LFS_TYPE_REG)
            {
                EN_SLOGI(TAG, "找到文件:%s", stInfo.name);
                
                //END220905100001_20221011.zip
                //END220905100001_20221011.log
                pDate = strchr(stInfo.name, '_');
                if(pDate != NULL)
                {
                    i32Year = (pDate[4] - '0') * 1 + (pDate[3] - '0') * 10 + (pDate[2] - '0') * 100 + (pDate[1] - '0') * 1000;
                    i32Mon  = (pDate[6] - '0') * 1 + (pDate[5] - '0') * 10;
                    i32Day  = (pDate[8] - '0') * 1 + (pDate[7] - '0') * 10;
                    
                    EN_SLOGI(TAG, "文件日期:%04d-%02d-%02d", i32Year, i32Mon, i32Day);
                    
                    //只要后缀是.log，而且不是今天的日志 就应该触发压缩
//                    if((i32Year > 1970)                                             //避免无效年份1970触发压缩任务
//                    && (i32Day != stTime.day)
//                    && (strstr(stInfo.name, cNetAppLogFileTypeLog) != NULL))
//                    {
//                        //启动压缩任务
//                        EN_SLOGI(TAG, "压缩文件:%s", stInfo.name);
//                        stZipParam.bDeleteFlag = true;                              //需要删除原始文件
//                        memset(stZipParam.u8Path, 0, sizeof(stZipParam.u8Path));
//                        snprintf(stZipParam.u8Path, sizeof(stZipParam.u8Path), "%s/%s", cNetAppLogPath, stInfo.name);
//                        sZipTaskCreat(&stZipParam);
//                        
//                        //由于压缩需要时间 因此找到一个就直接退出整个检查逻辑
//                        break;
//                    }
                }
            }
        }
        d_Close();
    }
}










/**********************************************************************************************
* Description       :     删除最早一个log日志文件逻辑
* Author            :     Hall
* modified Date     :     2022-09-21
* notice            :     定义保留最近一个月的log文件，超出一个月的文件要及时删除，避免存储耗尽
***********************************************************************************************/
void sNetAppLogDelete(stFullTime_t stTimeOld)
{
    int i32Year = 0;
    int i32Mon = 0;
    int i32Day = 0;
    
    char *pDate = NULL;
    char u8DeletePath[cNetAppLogPathLen] = {0};
    struct lfs_info stInfo;
    
    stFullTime_t stChgTime;
    
    stChgTime = sGetFullTime();
    
    //日期变更同时 删除一个月之前同一天的 log 文件
    if(stChgTime.year != 1970)                                                  //从上电值0000-00-00到1970-01-01时不执行本逻辑 避免误删文件
    {
        if(d_Open(cNetAppLogPath) == true)
        {
            while(d_Read(&stInfo) == true)
            {
                if(stInfo.type == LFS_TYPE_REG)
                {
                    EN_SLOGI(TAG, "找到文件:%s", stInfo.name);
                    
                    //---/log/END220905100001_20221011.zip
                    pDate = strchr(stInfo.name, '_');
                    if(pDate != NULL)
                    {
                        i32Year = (pDate[4] - '0') * 1 + (pDate[3] - '0') * 10 + (pDate[2] - '0') * 100 + (pDate[1] - '0') * 1000;
                        i32Mon  = (pDate[6] - '0') * 1 + (pDate[5] - '0') * 10;
                        i32Day  = (pDate[8] - '0') * 1 + (pDate[7] - '0') * 10;
                        
                        EN_SLOGI(TAG, "文件日期:%04d-%02d-%02d", i32Year, i32Mon, i32Day);
                        
                        if((i32Day == stChgTime.day) 
                        && ((i32Year != stChgTime.year) || (i32Mon != stChgTime.mon)))
                        {
                            EN_SLOGI(TAG, "删除文件:%s", stInfo.name);
                            memset(u8DeletePath, 0, sizeof(u8DeletePath));
                            snprintf(u8DeletePath, sizeof(u8DeletePath), "%s/%s", cNetAppLogPath, stInfo.name);
                            remove(u8DeletePath);
                        }
                        else if((i32Year <= 1970) || (i32Year > 10000) || (i32Mon == 0) || (i32Mon > 12) || (i32Day == 0) || (i32Day > 31))
                        {
                            //每次日期变更时 删除1970年和年月日值错乱的文件
                            EN_SLOGI(TAG, "删除文件:%s", stInfo.name);
                            memset(u8DeletePath, 0, sizeof(u8DeletePath));
                            snprintf(u8DeletePath, sizeof(u8DeletePath), "%s/%s", cNetAppLogPath, stInfo.name);
                            remove(u8DeletePath);
                        }
                    }
                    else
                    {
                        //删除文件名格式不符合:END220905100001_20221011.zip 的文件
                        EN_SLOGI(TAG, "删除文件:%s", stInfo.name);
                        memset(u8DeletePath, 0, sizeof(u8DeletePath));
                        snprintf(u8DeletePath, sizeof(u8DeletePath), "%s/%s", cNetAppLogPath, stInfo.name);
                        remove(u8DeletePath);
                    }
                }
            }
            d_Close();
        }
    }
}








/**********************************************************************************************
* Description       :     日志存储部分 存储空间检查
* Author            :     Hall
* modified Date     :     2022-09-21
* notice            :     当剩余空间不足时 返回给上层，停止存储日志 防止存储空间耗尽 失去腾挪空间
***********************************************************************************************/
bool sNetAppLogMemoryCheck(void)
{
    struct lfs_fsinfo stInfo;
    i32 i32UsedBlock  = 0;
    i32 i32UnusedBlock  = 0;
    
    if(stat(&stInfo) != true)
    {
        return(true);
    }
    
    i32UsedBlock = memsize();
    if(i32UsedBlock < 0)
    {
        EN_SLOGE(TAG, "memsize error code: %d, 停止存储调试日志", i32UsedBlock);
        return(true);
    }
    
    i32UnusedBlock = stInfo.block_count - i32UsedBlock;
    if(i32UnusedBlock < cNetAppLogRemainingCap)
    {
        EN_SLOGI(TAG, "块总容量:%d, 可用:%d, 小于%d, 停止存储调试日志", stInfo.block_count, i32UnusedBlock, cNetAppLogRemainingCap);
        return(true);
    }
    else
    {
        EN_SLOGI(TAG, "块总容量:%d, 可用:%d, 大于%d, 继续存储调试日志", stInfo.block_count, i32UnusedBlock, cNetAppLogRemainingCap);
        return(false);
    }
}











/**********************************************************************************************
* Description       :     log日志上传任务创建函数
* Author            :     Hall
* modified Date     :     2022-10-09
* notice            :     创建一个任务用于日志上传，将传入的参数传递给该任务
***********************************************************************************************/
//bool sNetAppLogPostTaskCreat(const stNetAppLogPostTaskParam_t *pParam)
//{
//    if(xSemaphoreTake(hLogPostMutex, 10 / portTICK_PERIOD_MS) == pdTRUE)
//    {
//        ESP_LOGI(TAG, "上传任务未在运行,可以创建任务");
//        if(pdPASS != xTaskCreate(sNetAppLogPostTask, "sNetAppLogPostTask", (6 * 1024), (void * const)pParam, 12, NULL))
//        {
//            ESP_LOGE(TAG,  "Create sNetAppLogPostTask error!");
//            return(false);
//        }
//        
//        return(true);
//    }
//    else
//    {
//        //获取不到 hLogPostMutex 表示 sNetAppLogPostTask 任务正在执行 不重复创建，直接返回失败
//        ESP_LOGI(TAG, "上传任务正在运行,无法创建任务");
//        return(false);
//    }
//}







/**********************************************************************************************
* Description       :     log日志文件名和路径生成
* Author            :     Hall
* modified Date     :     2022-09-21
* notice            :     根据格式生成指定日期的log文件名和路径
***********************************************************************************************/
void sNetAppLogPathGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pPath)
{
    char u8Sn[cPrivDrvLenSn];
    memset(u8Sn, 0, sizeof(u8Sn));
    sEepromAppGetBlockFactorySn(u8Sn, sizeof(u8Sn));
    
    //   /log/EN0220921100001_20220921.log
    snprintf(pPath, u16MaxLen, "%s/%s_%04d%02d%02d%s", cNetAppLogPath, u8Sn, stTime.year, stTime.mon, stTime.day, pFileType);
}



/**********************************************************************************************
* Description       :     log日志文件名---纯文件名
* Author            :     Hall
* modified Date     :     2022-09-21
* notice            :     目的是生成带时分秒的文件名 后缀由参数决定
***********************************************************************************************/
void sNetAppLogNameGenerate(stFullTime_t stTime, u16 u16MaxLen, const char *pFileType, char *pName)
{
    char u8Sn[cPrivDrvLenSn];
    memset(u8Sn, 0, sizeof(u8Sn));
    sEepromAppGetBlockFactorySn(u8Sn, sizeof(u8Sn));
    
    //   EN0220921100001_20220921204430.zip
    snprintf(pName, u16MaxLen, "%s_%04d%02d%02d%02d%02d%02d%s", u8Sn, stTime.year, stTime.mon, stTime.day, stTime.hour, stTime.min, stTime.sec, pFileType);
}








