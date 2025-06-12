/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     usb_app.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-8-28
 * @Attention             :     
 * @Brief                 :     USB用户程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-8-28
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "usb_app.h"

#include <string.h>

#include "drv_usb_hw.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bbb.h"

#include "en_common.h"
#include "en_log.h"
#include "en_log_storage.h"
#include "en_littlefs.h"
#include "en_mem.h"
#include "en_shell.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "usb_app";




usbh_host usb_host_msc;
usb_core_driver msc_host_core;


stUsbhAppCache_t *pUsbhCache = NULL;









bool sUsbAppOpen(void);
void sUsbAppTask(void *pParam);
void sUsbAppShellCopyCmdTask(void *pParam);



//由用户自定义的USB回调函数
void sUsbhAppInit(void);
void sUsbhAppDeinit(void);
void sUsbhAppDevConnected(void);
void sUsbhAppDevReset(void);
void sUsbhAppDevDisconnected(void);
void sUsbhAppOverCurrentDetected(void);
void sUsbhAppDevSpeedDetected(u32 u32DevSpeed);
void sUsbhAppDevDescAvailable(void *pDevDesc);
void sUsbhAppDevAddAssigned(void);
void sUsbhAppCfgDescAvailable(usb_desc_config *pCfgDesc, usb_desc_itf *pItfDesc, usb_desc_ep *pEpDesc);
void sUsbhAppManufacturerString(void *pString);
void sUsbhAppProductString(void *pString);
void sUsbhAppSerialNumString(void *pString);
void sUsbhAppEnumerationFinish(void);
usbh_user_status sUsbhAppUserInput(void);
int sUsbhAppMscApplication(void);
void sUsbhAppDevNotSupported(void);
void sUsbhAppUnrecoveredError(void);


static bool sUsbAppExploreDisk(const char* pPath, u8 u8RecuLevel);
static bool sUsbAppFileCopyFromHostProc(void);
static bool sUsbAppFileCopyToHostProc(const char* pSrcPath, const char* pDestPath);


bool sEnlogShellUsbCmd(const stShellPkt_t *pkg);





//用户定义用于USB底层驱动回调的回调函数结构体
usbh_user_cb stUsbhUsrCallback =
{
    sUsbhAppInit,
    sUsbhAppDeinit,
    sUsbhAppDevConnected,
    sUsbhAppDevReset,
    sUsbhAppDevDisconnected,
    sUsbhAppOverCurrentDetected,
    sUsbhAppDevSpeedDetected,
    sUsbhAppDevDescAvailable,
    sUsbhAppDevAddAssigned,
    sUsbhAppCfgDescAvailable,
    sUsbhAppManufacturerString,
    sUsbhAppProductString,
    sUsbhAppSerialNumString,
    sUsbhAppEnumerationFinish,
    sUsbhAppUserInput,
    sUsbhAppMscApplication,
    sUsbhAppDevNotSupported,
    sUsbhAppUnrecoveredError
};







stShellCmd_t stSellCmdUsbCmd = 
{
    .pCmd       = "usb",
    .pFormat    = "格式:usb 命令 命令格式",
    .pFunction  = "功能:USB文件操作命令",
    .pRemarks   = "备注:",
    .pFunc      = sEnlogShellUsbCmd,
};









/*******************************************************************************
 * @FunctionName   :      sUsbAppOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  19:38:32
 * @Description    :      USB用户程序 ———— USB设备初始化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sUsbAppOpen(void)
{
    //USB设备硬件初始化
    sUsbDrvInit(&stUsbhUsrCallback);
    
    //USB设备缓存申请
    pUsbhCache = (stUsbhAppCache_t *)MALLOC(sizeof(stUsbhAppCache_t));
    if(pUsbhCache == NULL)
    {
        EN_SLOGD(TAG, ">>>     USB设备缓存申请失败!");
        return false;
    }
    memset(pUsbhCache, 0, sizeof(stUsbhAppCache_t));
    
    //USB设备任务创建
    xTaskCreate(sUsbAppTask, "sUsbAppTask", (1024), NULL, 29, NULL);
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdUsbCmd);
    
    return true;
}





/*******************************************************************************
 * @FunctionName   :      sUsbAppTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  20:20:16
 * @Description    :      USB用户任务
 * @Input          :      pParam      
*******************************************************************************/
void sUsbAppTask(void *pParam)
{
    EN_SLOGD(TAG, "USB用户任务创建成功!");
    
    vTaskDelay(10000 / portTICK_RATE_MS);
    
    while(1)
    {
        usbh_core_task(&usb_host_msc);
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}





/*******************************************************************************
 * @FunctionName   :      sUsbAppShellCopyCmdTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月2日  18:35:10
 * @Description    :      USB用户任务 ———— Shell命令拷贝数据
 * @Input          :      pParam      
*******************************************************************************/
void sUsbAppShellCopyCmdTask(void *pParam)
{
    bool bRst;
    u32 u32StartSec = 0;
    
    EN_SLOGD(TAG, "USB用户拷贝数据任务创建成功!");
    
    const stShellPkt_t *pkg = (stShellPkt_t *)pParam;
    
    u32StartSec = sGetTimestamp();
    
    bRst = sUsbAppFileCopyToHostProc((const char *)pkg->para[1], (const char *)pkg->para[2]);
    
    for(i8 i = 0; i < pkg->paraNum; i++)
    {
        FREE(pkg->para[i]);
    }
    
    EN_SLOGD(TAG, "USB用户拷贝数据任务运行结束! 结果:%d 耗时:%dS", bRst, (u32)(sGetTimestamp()- u32StartSec));
    
    vTaskDelete(NULL);
}









/*******************************************************************************
 * @FunctionName   :      sUsbhAppInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  11:25:05
 * @Description    :      USB主机模式用户资源初始化
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppInit(void)
{
    
}






/*******************************************************************************
 * @FunctionName   :      sUsbhAppDeinit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  11:32:34
 * @Description    :      取消初始化用户状态和相关变量
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDeinit(void)
{
    pUsbhCache->eState = eStateFsInit;
    memset(pUsbhCache, 0, sizeof(stUsbhAppCache_t));
}






/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevConnected
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  11:38:01
 * @Description    :      检测到USB设备连接状态:已连接 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDevConnected(void)
{
    EN_SLOGD(TAG, ">>>     USB设备连接状态 : 已连接");
}






/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevReset
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  11:44:01
 * @Description    :      重置USB设备 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDevReset(void)
{
    EN_SLOGD(TAG, ">>>     复位USB设备!");
}






/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevDisconnected
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  13:48:53
 * @Description    :      检测到USB设备连接状态:已断开 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDevDisconnected(void)
{
    EN_SLOGD(TAG, ">>>     USB设备连接状态 : 已断开");
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppOverCurrentDetected
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  13:54:34
 * @Description    :      对USB设备过流检测事件 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppOverCurrentDetected(void)
{
    EN_SLOGE(TAG, ">>>     检测到设备过流!");
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevSpeedDetected
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:00:20
 * @Description    :      检测USB设备速度 的用户操作
 * @Input          :      u32DevSpeed       USB设备速度
*******************************************************************************/
void sUsbhAppDevSpeedDetected(u32 u32DevSpeed)
{
    if(PORT_SPEED_HIGH == u32DevSpeed)
    {
        EN_SLOGD(TAG, ">>>     检测到高速设备!");
    }
    else if(PORT_SPEED_FULL == u32DevSpeed)
    {
        EN_SLOGD(TAG, ">>>     检测到全速设备!");
    }
    else if(PORT_SPEED_LOW == u32DevSpeed)
    {
        EN_SLOGD(TAG, ">>>     检测到低速设备!");
    }
    else
    {
        EN_SLOGE(TAG, ">>>     检测到故障设备!");
    }
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevDescAvailable
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:12:47
 * @Description    :      USB设备描述符可用时 的用户操作
 * @Input          :      pDevDesc    
*******************************************************************************/
void sUsbhAppDevDescAvailable(void *pDevDesc)
{
    usb_desc_dev *pDevStr = (usb_desc_dev *)pDevDesc;
    
    EN_SLOGD(TAG, ">>>     USB产品的    厂商ID: %04Xh", pDevStr->idVendor);
    EN_SLOGD(TAG, ">>>     USB产品的唯一产品ID: %04Xh", pDevStr->idProduct);
}




/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevAddAssigned
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:15:23
 * @Description    :      成功为USB设备分配地址时 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDevAddAssigned(void)
{
    
}




/*******************************************************************************
 * @FunctionName   :      sUsbhAppCfgDescAvailable
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:22:31
 * @Description    :      配置USB描述符可用时 的用户操作
 * @Input          :      pCfgDesc    
 * @Input          :      pItfDesc    
 * @Input          :      pEpDesc     
*******************************************************************************/
void sUsbhAppCfgDescAvailable(usb_desc_config *pCfgDesc, usb_desc_itf *pItfDesc, usb_desc_ep *pEpDesc)
{
    usb_desc_itf *pItf = pItfDesc;
    
    if(0x08U  == pItf->bInterfaceClass)
    {
        EN_SLOGD(TAG, ">>>     已连接大容量存储设备!");
    }
    else if(0x03U  == pItf->bInterfaceClass)
    {
        EN_SLOGD(TAG, ">>>     已连接HID设备!");
    }
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppManufacturerString
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:44:06
 * @Description    :      制造商字符串存在时 的用户操作
 * @Input          :      pString
*******************************************************************************/
void sUsbhAppManufacturerString(void *pString)
{
    EN_SLOGD(TAG, ">>>     设备制造商: %s", (char *)pString);
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppProductString
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:58:03
 * @Description    :      产品字符串存在时 的用户操作
 * @Input          :      pString     
*******************************************************************************/
void sUsbhAppProductString(void *pString)
{
    EN_SLOGD(TAG, ">>>     产品: %s", (char *)pString);
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppSerialNumString
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  14:59:58
 * @Description    :      序列号字符串存在时 的用户操作
 * @Input          :      pString     
*******************************************************************************/
void sUsbhAppSerialNumString(void *pString)
{
    EN_SLOGD(TAG, ">>>     序列号: %s", (char *)pString);
}




/*******************************************************************************
 * @FunctionName   :      sUsbhAppEnumerationFinish
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  15:06:22
 * @Description    :      显示用户响应请求以请求应用程序跳转到类
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppEnumerationFinish(void)
{
    EN_SLOGD(TAG, ">>>     列举完成!");
    EN_SLOGD(TAG, ">>>     查看磁盘信息 ↓↓↓");
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppUserInput
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  15:16:02
 * @Description    :      
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
usbh_user_status sUsbhAppUserInput(void)
{
    usbh_user_status eStatus = USBH_USER_RESP_OK;
    
    return eStatus;
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppMscApplication
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  15:18:07
 * @Description    :      
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
int sUsbhAppMscApplication(void)
{
    stUsbhAppCache_t *pCache = NULL;
    FRESULT eRst;
    msc_lun stInfo;
    u32 u32StartSec;
    
    if(pUsbhCache == NULL)
    {
        return(-1);
    }
    pCache = pUsbhCache;
    
    switch(pCache->eState)
    {
    case eStateFsInit:
        //初始化文件系统
        if (FR_OK != f_mount(&pUsbhCache->stFatfs, "0:/", 0))
        {
            EN_SLOGE(TAG, ">>>     无法初始化FAT文件系统!");
            return(-1);
        }
        
        EN_SLOGD(TAG, ">>>     FAT文件系统已初始化!");
        
        if (USBH_OK == usbh_msc_lun_info_get(&usb_host_msc, 0, &stInfo))
        {
            EN_SLOGD(TAG, ">>>     磁盘容量: %llu Bytes", (u64)stInfo.capacity.block_nbr * stInfo.capacity.block_size);
        }
        
        pCache->eState = eStateFsReadList;
        break;
        
    case eStateFsReadList:
        EN_SLOGD(TAG, ">>>     探索磁盘! 查看磁盘的根目录内容!");
//        sExploreDisk("0:/", 1);
        pCache->eState = eStateFsWriteFile;
        break;
        
    case eStateFsWriteFile:
        usb_mdelay(100);
        EN_SLOGD(TAG, ">>>     开始向磁盘写入文件!");
        
        //为逻辑驱动器注册工作区
        f_mount(&pUsbhCache->stFatfs, "0:/", 1);
        
        //
        u32StartSec = sGetTimestamp();
        
        if(!sUsbAppFileCopyFromHostProc())
        {
            EN_SLOGE(TAG, ">>>     文件拷贝任务失败!  耗时:%dS" , (u32)(sGetTimestamp()- u32StartSec));
        }
        else
        {
            EN_SLOGD(TAG, ">>>     文件拷贝任务成功!  耗时:%dS" , (u32)(sGetTimestamp()- u32StartSec));
        }
        
        /* unmount file system */
//        f_mount(NULL, "0:/", 1);
        
        pCache->eState = eStateFsFinished;
        EN_SLOGD(TAG, ">>>     文件写入操作结束!");
        EN_SLOGD(TAG, ">>>     可选择继续shell命令操作拷贝文件 或 卸载FAT文件系统!");
        break;
        
    case eStateFsFinished:
        break;
        
    default:
        break;
    }
    
    return(0);
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppDevNotSupported
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  16:01:09
 * @Description    :      不支持设备时 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppDevNotSupported(void)
{
    EN_SLOGE(TAG, ">>>     当前驱动不支持该设备!");
}





/*******************************************************************************
 * @FunctionName   :      sUsbhAppUnrecoveredError
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  16:01:10
 * @Description    :      发生未恢复错误时 的用户操作
 * @Input          :      void        
*******************************************************************************/
void sUsbhAppUnrecoveredError(void)
{
    EN_SLOGE(TAG, ">>>     未恢复的错误状态!");
}






/*******************************************************************************
 * @FunctionName   :      sUsbAppExploreDisk
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月28日  17:16:29
 * @Description    :      扫描磁盘指定目录下所有文件
 * @Input          :      pPath       
 * @Input          :      u8RecuLevel 
 * @Return         :      
*******************************************************************************/
static bool sUsbAppExploreDisk(const char* pPath, u8 u8RecuLevel)
{
    FRESULT eRst;
    FILINFO stFileInfo;
    DIR stDir;
    char *pFileName;
    char u8String[50], u8TempString[50];;
    
    eRst = f_opendir(&stDir, pPath);
    
    if (eRst == FR_OK)
    {
        while(msc_host_core.host.connect_status)
        {
            eRst = f_readdir(&stDir, &stFileInfo);
            if (FR_OK != eRst || 0U == stFileInfo.fname[0])
            {
                break;
            }
            
            if ('.' == stFileInfo.fname[0])
            {
                continue;
            }
            
            pFileName = stFileInfo.fname;
            
            if(1U == u8RecuLevel)
            {
                snprintf(u8String, sizeof(u8String), "%s", ">>>     |__");
            }
//            else if(2U == u8RecuLevel)
//            {
//                snprintf(u8String, sizeof(u8String), "%s", ">>>     |   |__");
//            }
            
            snprintf(u8TempString, sizeof(u8TempString), "%s %s", u8String, stFileInfo.fname);
            EN_SLOGD(TAG, "%s", u8TempString);
            vTaskDelay(20 / portTICK_RATE_MS);
            
            memset(u8String, 0, sizeof(u8String));
            memset(u8TempString, 0, sizeof(u8TempString));
//            
//            if((AM_DIR == stFileInfo.fattrib) && (1U == u8RecuLevel))
//            {
//                sExploreDisk(pFileName, 2);
//            }
        }
    }
    
    return true;
}




/*******************************************************************************
 * @FunctionName   :      sUsbAppFileCopyFromHostProc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月29日  15:43:15
 * @Description    :      从主机拷贝数据到USB设备
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
static bool sUsbAppFileCopyFromHostProc(void)
{
    bool bRst = false;
    
    lfs_file_t stSrcFile;
    struct lfs_info stInfo;
    FILINFO stFileInfo;
    
    static u8 *pSrcData = NULL;
    char u8DestFilePath[128], u8SrcFilePath[128];
    u32 u32RawDataLen = 0, u32ToCopyDataLen = 10*1024, u32CopiedDataLen = 0;
    
    
    //1:申请用于数据传输的数据缓存
    pSrcData = (u8*)MALLOC(u32ToCopyDataLen);                   //申请10K缓存 --- 用于缓存原文件要传输的数据段
    if(pSrcData == NULL)
    {
        return bRst;
    }
    
    
    //2:打开目录
    if(d_Open(cNetAppLogPath) == true)
    {
        if(FR_OK != f_stat("0:/log", &stFileInfo))
        {
            if(FR_OK != f_mkdir("0:/log"))
            {
                EN_SLOGE(TAG, "在USB设备下创建log文件夹失败!");
            }
        }
        
        //3:轮询搜寻目录下的所有文件
        while((d_Read(&stInfo) == true) && (msc_host_core.host.connect_status))
        {
            if(stInfo.type == LFS_TYPE_REG)
            {
                memset(u8SrcFilePath, 0, sizeof(u8SrcFilePath));
                snprintf(u8SrcFilePath, sizeof(u8SrcFilePath), "%s/%s", cNetAppLogPath, stInfo.name);
                
                //4:打开要读取传输数据的文件, 开始传输进程
                if(f_Open(&stSrcFile, u8SrcFilePath) == true)
                {
                    // 获取要传输的源文件大小
                    u32RawDataLen = f_Size(&stSrcFile);
                    EN_SLOGI(TAG, "找到文件:%s  文件大小为 %.2f KB", u8SrcFilePath, (f32)(u32RawDataLen/1024.0));
                    
                    // 获取目标文件名称
                    memset(u8DestFilePath, 0, sizeof(u8DestFilePath));
                    snprintf(u8DestFilePath, sizeof(u8DestFilePath), "0:/log/%s", stInfo.name);
                    
                    // 打开要写入数据的目标文件
                    if(FR_OK == f_open(&pUsbhCache->stFile, u8DestFilePath, FA_CREATE_ALWAYS | FA_WRITE))
                    {
                        if((f_Seek(&stSrcFile, 0, LFS_SEEK_SET) < 0) || (f_lseek(&pUsbhCache->stFile, 0) != FR_OK))
                        {
                            u32RawDataLen = 0;
                        }
                        
                        while((u32RawDataLen > 0) && (msc_host_core.host.connect_status))
                        {
                            /*由于要传输的文件大小一般比较大, 因此这里采用分段传输的方式进行文件数据的传输*/
                            
                            // 要传输的文件大小 小于 用户定义的单次可传输数据的大小
                            if(u32RawDataLen <= u32ToCopyDataLen)
                            {
                                u32ToCopyDataLen = u32RawDataLen;
                            }
                            else
                            {
                                u32ToCopyDataLen = 10*1024;
                            }
                            
                            // 初始化缓存
                            memset(pSrcData, 0, u32ToCopyDataLen);
                            
                            // 读数据
                            if(false == f_Read(&stSrcFile, pSrcData, u32ToCopyDataLen))
                            {
                                break;
                            }
                            
                            // 写数据
                            if(FR_OK != f_write(&pUsbhCache->stFile, pSrcData, u32ToCopyDataLen, (void *)&u32CopiedDataLen))
                            {
                                break;
                            }
                            
                            u32RawDataLen = u32RawDataLen - u32ToCopyDataLen;
                            if(u32RawDataLen == 0)
                            {
                                bRst = true;
                                break;
                            }
                        }
                        
                        if(FR_OK != f_close(&pUsbhCache->stFile))
                        {
                            bRst = false;
                        }
                    }
                    
                    if(f_Close(&stSrcFile) == false)
                    {
                        bRst = false;
                        break;
                    }
                }
                
                EN_SLOGI(TAG, "文件:%s 传输完成 结果: %d", stInfo.name, bRst);
            }
        }
        
        bRst &= d_Close();
    }
    
    
    //4:释放缓存
    FREE(pSrcData);
    pSrcData = NULL;
    
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sUsbAppFileCopyToHostProc
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月2日  16:51:15
 * @Description    :      从USB设备拷贝数据到主机
 * @Input          :      pSrcPath    
 * @Input          :      pDestPath   
 * @Return         :      
*******************************************************************************/
static bool sUsbAppFileCopyToHostProc(const char* pSrcPath, const char* pDestPath)
{
    bool bRst = false;
    
    FRESULT eRst;
    FILINFO stFileInfo;
    FIL     stSrcFile;
    
    lfs_file_t stDestFile;
    struct lfs_info stLfsInfo;
    
    static u8 *pSrcData = NULL;
    u32 u32RawDataLen = 0, u32ToCopyDataLen = 10*1024, u32CopiedDataLen = 0;
    
    
    //1:申请用于数据传输的数据缓存
    pSrcData = (u8*)MALLOC(u32ToCopyDataLen);                   //申请10K缓存 --- 用于缓存原文件要传输的数据段
    if(pSrcData == NULL)
    {
        return bRst;
    }
    
    if(msc_host_core.host.connect_status == false)
    {
        return bRst;
    }
    
    if(pathstat(pDestPath, &stLfsInfo) == true)
    {
        remove(pDestPath);                                       //如果目标文件已经存在,应该先删除它
    }
    
    //2:打开文件
    if(FR_OK == f_open(&stSrcFile, pSrcPath, FA_OPEN_EXISTING | FA_READ))
    {
        //3:打开要读取传输数据的文件, 开始传输进程
        if(f_Open(&stDestFile, pDestPath) == true)
        {
            if(f_stat(pSrcPath, &stFileInfo) == FR_OK)
            {
                // 获取文件大小
                u32RawDataLen = stFileInfo.fsize;
            }
            
            if((f_Seek(&stDestFile, 0, LFS_SEEK_SET) < 0) || (f_lseek(&stSrcFile, 0) != FR_OK))
            {
                u32RawDataLen = 0;
                bRst = false;
            }
            
            while((u32RawDataLen > 0) && (msc_host_core.host.connect_status))
            {
                /*由于要传输的文件大小一般比较大, 因此这里采用分段传输的方式进行文件数据的传输*/
                
                // 要传输的文件大小 小于 用户定义的单次可传输数据的大小
                if(u32RawDataLen <= u32ToCopyDataLen)
                {
                    u32ToCopyDataLen = u32RawDataLen;
                }
                else
                {
                    u32ToCopyDataLen = 10*1024;
                }
                
                // 初始化缓存
                memset(pSrcData, 0, u32ToCopyDataLen);
                
                // 读数据
                if(FR_OK != f_read(&stSrcFile, pSrcData, u32ToCopyDataLen, (void *)&u32CopiedDataLen))
                {
                    break;
                }
                
                if(u32CopiedDataLen != u32ToCopyDataLen)
                {
                    break;
                }
                
                // 写数据
                if(false == f_Write(&stDestFile, pSrcData, u32CopiedDataLen))
                {
                    break;
                }
                
                u32RawDataLen = u32RawDataLen - u32CopiedDataLen;
                if(u32RawDataLen == 0)
                {
                    bRst = true;
                    break;
                }
            }
            
            if(f_Close(&stDestFile) == false)
            {
                bRst = false;
            }
        }
        
        if(FR_OK != f_close(&stSrcFile))
        {
            bRst = false;
        }
    }
    
    EN_SLOGI(TAG, "文件:%s 传输完成 结果: %d", pSrcPath, bRst);
    
    
    //4:释放缓存
    FREE(pSrcData);
    pSrcData = NULL;
    
    
    return bRst;
}








/*******************************************************************************
 * @FunctionName   :      sEnlogShellUsbCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月2日  14:57:16
 * @Description    :      USB用户程序 ———— Shell操作命令
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellUsbCmd(const stShellPkt_t *pkg)
{
    static stShellPkt_t stPkg;
    
    memset(&stPkg, 0, sizeof(stShellPkt_t));
    
    if(!msc_host_core.host.connect_status)
    {
        return(false);
    }
    
    if(strcmp((const char *)pkg->para[0], "list") == 0)                      //浏览指定目录
    {
        return(sUsbAppExploreDisk((const char *)pkg->para[1], 1));
    }
    else if(strcmp((const char *)pkg->para[0], "copy") == 0)                //从USB设备拷贝指定文件到主机
    {
        if(pkg->paraNum == 3)
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
            
            if(pdPASS != xTaskCreate(sUsbAppShellCopyCmdTask, "sUsbAppShellCopyCmdTask", (15 * 1024), (void * const)&stPkg, 17, NULL))
            {
                EN_SLOGE(TAG,  "sUsbAppShellCopyCmdTask 创建失败!");
                return(false);
            }
        }
        else
        {
            EN_SLOGE(TAG,  "指令格式错误!");
            return(false);
        }
        
        return(true);
    }
    else if(strcmp((const char *)pkg->para[0], "unmount") == 0)             //卸载文件系统
    {
        f_mount(NULL, "0:/", 1);
        return(true);
    }
    else
    {
        EN_SLOGE(TAG, "无效命令");
    }
    
    return(false);
}








