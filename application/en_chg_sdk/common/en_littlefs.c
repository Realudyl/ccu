/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_littlefs.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-23
 * @Attention             :     
 * @Brief                 :     
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-23
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "en_littlefs.h"
#include "en_log.h"

#include "flash_drv_gd5f4gm8.h"
#include "flash_drv_gd5f4gm8_api.h"









//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "en_littlefs";





//首次上电FLASH初始化状态
static bool bFlashDrvInited = false;    //false : 未初始化    true : 已初始化




// 文件系统相关定义
SemaphoreHandle_t   xSemLittleFsQueue;              // lfs 文件系统访问信号量

lfs_t               stLittleFs;                     // lfs 文件系统对象
lfs_dir_t           stDir;                          // lfs 目录








static i32 sLittleFsRead(const struct lfs_config* pCfg, lfs_block_t u32Block, lfs_off_t u32Off, void* pBuffer, lfs_size_t u32Size);
static i32 sLittleFsProg(const struct lfs_config* pCfg, lfs_block_t u32Block, lfs_off_t u32Off, const void* pBuffer, lfs_size_t u32Size);
static i32 sLittleFsErase(const struct lfs_config* pCfg, lfs_block_t u32Block);
static i32 sLittleFsSync(const struct lfs_config* pCfg);

static i32 sLittleFsLock(const struct lfs_config* pCfg);
static i32 sLittleFsUnLock(const struct lfs_config* pCfg);


bool sLittleFsMount(void);
bool sLittleFsUnmount(void);
bool sLittleFsFormat(void);
bool sLittleFsRemove(const char *pPath);
bool sLittleFsRename(const char *pOldPath, const char *pNewPath);
bool sLittleFsMkdir(const char *pPath);
i32  sLittleFsSize(void);
bool sLittleFsStat(struct lfs_fsinfo *pInfo);
bool sLittleFsPathStat(const char *pPath, struct lfs_info *pInfo);

bool sLittleFsOpenFile(lfs_file_t *pFile, const char *pFilePath);
bool sLittleFsCloseFile(lfs_file_t *pFile);
bool sLittleFsSyncFile(lfs_file_t *pFile);
bool sLittleFsReadFile(lfs_file_t *pFile, void *pBuffer, lfs_size_t u32Size);
bool sLittleFsWriteFile(lfs_file_t *pFile, const void *pBuffer, lfs_size_t u32Size);
i32  sLittleFsSeekFile(lfs_file_t *pFile, lfs_soff_t i32Offset, i32 i32WhenceFlag);
i32  sLittleFsSizeFile(lfs_file_t *pFile);
i32  sLittleFsTellFile(lfs_file_t *pFile);

bool sLittleFsOpenDir(const char *pDirPath);
bool sLittleFsCloseDir(void);
bool sLittleFsReadDir(struct lfs_info *pInfo);
bool sLittleFsRewindDir(void);










struct lfs_config stLfsCfg =
{
    // custom user data
    .context = NULL,
    
    // block device operations
    .read   = sLittleFsRead,
    .prog   = sLittleFsProg,
    .erase  = sLittleFsErase,
    .sync   = sLittleFsSync,
    
    .lock   = sLittleFsLock,
    .unlock = sLittleFsUnLock,
    
    // block device configuration
    .read_size      = cGd5f4gm8PerPageUserByteNum,                                  // 最小的读取字节数,所有的读取操作字节数必须是它的整数倍
    .prog_size      = cGd5f4gm8PerPageUserByteNum,                                  // 最小的写入字节数,所有的读取操作字节数必须是它的整数倍
    .block_size     = (cGd5f4gm8PerBlockPageNum * cGd5f4gm8PerPageUserByteNum),     // 擦除块操作的字节数,该选项不影响 RAM 消耗,可以比物理擦除尺寸大,但是每个文件至少占用一个块,必须是读取和写入操作字节数的整数倍
    .block_count    = cBlockNumForUserData,                                         // 设备上可擦除块的数量,即容量
    .block_cycles   = 500,                                                          // 
    .cache_size     = cGd5f4gm8PerPageUserByteNum,                                  // 缓存区大小
    .lookahead_size = cGd5f4gm8PerPageUserByteNum,                                  // 预读缓冲区大小
    .metadata_max   = cGd5f4gm8PerPageUserByteNum,
    
    // filesystem configuration
    .name_max = LFS_NAME_MAX,
    .file_max = LFS_FILE_MAX,
    .attr_max = LFS_ATTR_MAX,
};
















/*******************************************************************************
 * @FunctionName   :      sLittleFsRead
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月24日  16:44:49
 * @Description    :      LittleFS库驱动接口函数 之 读
 * @Input          :      pCfg          lfs_config格式参数
 * @Input          :      u32Block      逻辑块索引号,从0开始
 * @Input          :      u32Off        块内偏移，该值需能被read_size整除
 * @Input          :      pBuffer       指向储存数据的指针buff
 * @Input          :      u32Size       要读取的字节数,该值需能被read_size整除,lfs在读取时会确保不会跨块
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsRead(const struct lfs_config* pCfg, lfs_block_t u32Block, lfs_off_t u32Off, void* pBuffer, lfs_size_t u32Size)
{
    if(sFlashDrvRdData(u32Block, (u32Off / cGd5f4gm8PerPageUserByteNum), (u32Off % cGd5f4gm8PerPageUserByteNum), u32Size, (u8 *)pBuffer) != true)
    {
        return(LFS_ERR_IO);
    }
    
    return(LFS_ERR_OK);
}








/*******************************************************************************
 * @FunctionName   :      sLittleFsProg
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月24日  16:44:40
 * @Description    :      LittleFS库驱动接口函数 之 写
 * @Input          :      pCfg          lfs_config格式参数
 * @Input          :      u32Block      逻辑块索引号,从0开始
 * @Input          :      u32Off        块内偏移，该值需能被read_size整除
 * @Input          :      pBuffer       指向写入数据的指针buff
 * @Input          :      u32Size       要写入的字节数,该值需能被read_size整除,lfs在写入时会确保不会跨块
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsProg(const struct lfs_config* pCfg, lfs_block_t u32Block, lfs_off_t u32Off, const void* pBuffer, lfs_size_t u32Size)
{
    if(sFlashDrvWrData(u32Block, (u32Off / cGd5f4gm8PerPageUserByteNum), (u32Off % cGd5f4gm8PerPageUserByteNum), u32Size, (u8 *)pBuffer) != true)
    {
        return(LFS_ERR_IO);
    }
    
    return(LFS_ERR_OK);
}








/*******************************************************************************
 * @FunctionName   :      sLittleFsErase
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月24日  16:44:30
 * @Description    :      LittleFS库驱动接口函数 之 擦除
 * @Input          :      pCfg          lfs_config格式参数
 * @Input          :      u32Block      逻辑块索引号,从0开始
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsErase(const struct lfs_config* pCfg, lfs_block_t u32Block)
{
    if(sFlashDrvCmdBlockErase(u32Block) != true)
    {
        return(LFS_ERR_IO);
    }
    
    return(LFS_ERR_OK);
}








/*******************************************************************************
 * @FunctionName   :      sLittleFsSync
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月24日  16:44:15
 * @Description    :      LittleFS库驱动接口函数 之 同步
 * @Input          :      pCfg          lfs_config格式参数
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsSync(const struct lfs_config* pCfg)
{
    return(LFS_ERR_OK);
}










/*******************************************************************************
 * @FunctionName   :      sLittleFsLock
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月26日  13:25:42
 * @Description    :      LittleFS库驱动接口函数 之 上锁
 * @Input          :      pCfg        lfs_config格式参数
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsLock(const struct lfs_config* pCfg)
{
    if(xSemaphoreTake(xSemLittleFsQueue, portMAX_DELAY) != pdPASS)
    {
        return(LFS_ERR_IO);
    }
    
    return(LFS_ERR_OK);
}









/*******************************************************************************
 * @FunctionName   :      sLittleFsUnLock
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月26日  13:25:44
 * @Description    :      LittleFS库驱动接口函数 之 解锁
 * @Input          :      pCfg        lfs_config格式参数
 * @Return         :      lfs_error     lfs错误码
*******************************************************************************/
static i32 sLittleFsUnLock(const struct lfs_config* pCfg)
{
    if(xSemaphoreGive(xSemLittleFsQueue) != pdPASS)
    {
        return(LFS_ERR_IO);
    }
    
    return(LFS_ERR_OK);
}














/*******************************************************************************
 * @FunctionName   :      sLittleFsMount
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月3日  17:56:33
 * @Description    :      LittleFS API函数 之 挂载
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sLittleFsMount(void)
{
    static bool bFirstFormated = false;      //false : 芯片为出厂首次挂载文件系统, 需进行格式化    true : 已格式化
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    // 若为上电首次挂载, FLASH未初始化
    if(bFlashDrvInited == false)
    {
        // 初始化FLASH设备
        if(sFlashDrvApiGetInitState() == false)
        {
            // 初始化失败
            EN_SLOGE(TAG, "FLASH底层驱动初始化失败!!!");
            return false;
        }
        
        // 初始化成功, 初始化标志位置true, 防止后续调用该函数挂载重复初始化FLASH
        bFlashDrvInited = true;
        
        //信号量初始化
        xSemLittleFsQueue = xSemaphoreCreateBinary();
        xSemaphoreGive(xSemLittleFsQueue);
    }
    
    
    // 挂载文件系统
    i32Error = lfs_mount(&stLittleFs, &stLfsCfg);
    if(((i32Error == LFS_ERR_CORRUPT) || (i32Error == LFS_ERR_INVAL)) && (bFirstFormated == false))
    {
        //出现LFS_ERR_CORRUPT大概率是芯片为出厂首次挂载文件系统, 在此进行格式化
        i32Error = lfs_format(&stLittleFs, &stLfsCfg);
        if(i32Error == LFS_ERR_OK)
        {
            // 再次挂载文件系统
            i32Error = lfs_mount(&stLittleFs, &stLfsCfg);
        }
    }
    
    if(i32Error == LFS_ERR_OK)
    {
        //挂载成功
        bFirstFormated = true;
        u8FailCount = 0;
        return true;
    }
    else
    {
        //挂载失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 挂载 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}






/*******************************************************************************
 * @FunctionName   :      sLittleFsUnmount
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月3日  17:56:33
 * @Description    :      LittleFS API函数 之 卸载
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sLittleFsUnmount(void)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 卸载 失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    
    // 卸载文件系统
    i32Error = lfs_unmount(&stLittleFs);
    if(i32Error == LFS_ERR_OK)
    {
        //卸载成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //卸载失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 卸载 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}








/*******************************************************************************
 * @FunctionName   :      sLittleFsFormat
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  10:17:19
 * @Description    :      LittleFS API函数 之 格式化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sLittleFsFormat(void)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 格式化 失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_format(&stLittleFs, &stLfsCfg);
    if(i32Error == LFS_ERR_OK)
    {
        //格式化成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //格式化失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 格式化 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}






/*******************************************************************************
 * @FunctionName   :      sLittleFsRemove
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  10:48:34
 * @Description    :      LittleFS API函数 之 删除
 * @Input          :      pPath       
 * @Return         :      
*******************************************************************************/
bool sLittleFsRemove(const char *pPath)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 文件/目录删除 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_remove(&stLittleFs, pPath);
    if(i32Error == LFS_ERR_OK)
    {
        //文件删除成功
        u8FailCount = 0;
        EN_SLOGI(TAG, "%s 删除成功", pPath);
        return true;
    }
    else if(i32Error != LFS_ERR_NOENT)
    {
        //文件删除失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 文件/目录删除 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}







/*******************************************************************************
 * @FunctionName   :      sLittleFsRename
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  11:22:25
 * @Description    :      LittleFS API函数 之 重命名
 * @Input          :      pOldPath    
 * @Input          :      pNewPath    
 * @Return         :      
*******************************************************************************/
bool sLittleFsRename(const char *pOldPath, const char *pNewPath)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 文件/目录重命名 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_rename(&stLittleFs, pOldPath, pNewPath);
    if(i32Error == LFS_ERR_OK)
    {
        //文件/目录重命名成功
        u8FailCount = 0;
        EN_SLOGI(TAG, "重名成功 %s -> %s", pOldPath, pNewPath);
        return true;
    }
    else
    {
        //文件/目录重命名失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 文件/目录重命名 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}






/*******************************************************************************
 * @FunctionName   :      sLittleFsMkdir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  11:22:16
 * @Description    :      LittleFS API函数 之 创建目录
 * @Input          :      pPath       
 * @Return         :      
*******************************************************************************/
bool sLittleFsMkdir(const char *pPath)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 创建目录 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_mkdir(&stLittleFs, pPath);
    if(i32Error == LFS_ERR_OK)
    {
        //创建目录成功
        u8FailCount = 0;
        EN_SLOGI(TAG, "创建目录 %s 成功", pPath);
        return true;
    }
    else
    {
        //创建目录失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 创建目录 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsSize
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  20:54:56
 * @Description    :      LittleFS API函数 之 读取当前系统大小
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
i32 sLittleFsSize(void)
{
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 读取当前系统大小 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    return lfs_fs_size(&stLittleFs);
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsRewindDir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月14日  9:44:50
 * @Description    :      LittleFS API函数 之 读取系统信息
 * @Input          :      pInfo       
 * @Return         :      
*******************************************************************************/
bool sLittleFsStat(struct lfs_fsinfo *pInfo)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 获取系统信息 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_fs_stat(&stLittleFs, pInfo);
    if(i32Error == LFS_ERR_OK)
    {
        //获取系统信息成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //获取系统信息失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 获取系统信息 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsPathStat
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月14日  20:04:25
 * @Description    :      LittleFS API函数 之 获取指定目录/文件信息
 * @Input          :      pPath       
 * @Input          :      pInfo       
 * @Return         :      
*******************************************************************************/
bool sLittleFsPathStat(const char *pPath, struct lfs_info *pInfo)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 获取指定目录/文件信息 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_stat(&stLittleFs, pPath, pInfo);
    if(i32Error == LFS_ERR_OK)
    {
        //获取指定目录/文件信息成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //获取指定目录/文件信息失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 获取指定目录/文件信息 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsOpenFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:03:32
 * @Description    :      LittleFS API函数 之 打开文件
 * @Input          :      pFile             
 * @Input          :      pFilePath         
 * @Return         :      
*******************************************************************************/
bool sLittleFsOpenFile(lfs_file_t *pFile, const char *pFilePath)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 打开文件 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_file_open(&stLittleFs, pFile, pFilePath, LFS_O_RDWR | LFS_O_CREAT | LFS_O_APPEND);
    if(i32Error == LFS_ERR_OK)
    {
        //打开文件成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //打开文件失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 打开文件 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsCloseFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:12:27
 * @Description    :      LittleFS API函数 之 关闭文件
 * @Input          :      pFile             
 * @Return         :      
*******************************************************************************/
bool sLittleFsCloseFile(lfs_file_t *pFile)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 关闭文件 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_file_close(&stLittleFs, pFile);
    if(i32Error == LFS_ERR_OK)
    {
        //关闭文件成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //关闭文件失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 关闭文件 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsSyncFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:17:35
 * @Description    :      LittleFS API函数 之 同步文件
 * @Input          :      pFile             
 * @Return         :      
*******************************************************************************/
bool sLittleFsSyncFile(lfs_file_t *pFile)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 同步文件 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_file_sync(&stLittleFs, pFile);
    if(i32Error == LFS_ERR_OK)
    {
        //同步文件成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //同步文件失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 同步文件 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsReadFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:29:25
 * @Description    :      LittleFS API函数 之 读文件
 * @Input          :      pFile             
 * @Input          :      pBuffer           
 * @Input          :      u32Size           
 * @Return         :      
*******************************************************************************/
bool sLittleFsReadFile(lfs_file_t *pFile, void *pBuffer, lfs_size_t u32Size)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 读文件 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_file_read(&stLittleFs, pFile, pBuffer, u32Size);
    if(i32Error >= LFS_ERR_OK)
    {
        //读文件成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //读文件失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 读文件 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsWriteFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:34:19
 * @Description    :      LittleFS API函数 之 写文件
 * @Input          :      pFile             
 * @Input          :      pBuffer           
 * @Input          :      u32Size           
 * @Return         :      
*******************************************************************************/
bool sLittleFsWriteFile(lfs_file_t *pFile, const void *pBuffer, lfs_size_t u32Size)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 写文件 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_file_write(&stLittleFs, pFile, pBuffer, u32Size);
    if(i32Error >= LFS_ERR_OK)
    {
        //写文件成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //写文件失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 写文件 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsSeekFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  14:58:49
 * @Description    :      LittleFS API函数 之 指定文件位置
 * @Input          :      pFile             
 * @Input          :      i32Offset         
 * @Input          :      i32WhenceFlag     
 * @Return         :      
*******************************************************************************/
i32 sLittleFsSeekFile(lfs_file_t *pFile, lfs_soff_t i32Offset, i32 i32WhenceFlag)
{
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 指定文件位置 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    return lfs_file_seek(&stLittleFs, pFile, i32Offset, i32WhenceFlag);
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsSizeFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月7日  10:51:43
 * @Description    :      LittleFS API函数 之 读取文件大小
 * @Input          :      pFile             
 * @Return         :      
*******************************************************************************/
i32 sLittleFsSizeFile(lfs_file_t *pFile)
{
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 读取文件大小 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return (-1);
    }
    
    return lfs_file_size(&stLittleFs, pFile);
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsTellFile
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月30日  10:05:27
 * @Description    :      LittleFS API函数 之 返回文件位置
 * @Input          :      pFile       
 * @Return         :      
*******************************************************************************/
i32 sLittleFsTellFile(lfs_file_t *pFile)
{
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 返回文件位置 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    return lfs_file_tell(&stLittleFs, pFile);
}






/*******************************************************************************
 * @FunctionName   :      sLittleFsOpenDir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  15:09:52
 * @Description    :      LittleFS API函数 之 打开目录
 * @Input          :      pDirPath    
 * @Return         :      
*******************************************************************************/
bool sLittleFsOpenDir(const char *pDirPath)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 打开目录 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    //打开目录
    i32Error = lfs_dir_open(&stLittleFs, &stDir, pDirPath);
    if(i32Error == LFS_ERR_NOENT)
    {
        //若打开的目录不存在, 则先创建该目录
        i32Error = lfs_mkdir(&stLittleFs, pDirPath);
        EN_SLOGI(TAG, "%s目录创建成功", pDirPath);
        
        if(i32Error == LFS_ERR_OK)
        {
            //再次打开目录
            i32Error = lfs_dir_open(&stLittleFs, &stDir, pDirPath);
        }
    }
    
    if(i32Error == LFS_ERR_OK)
    {
        //打开目录成功
        u8FailCount = 0;
        EN_SLOGI(TAG, "打开 %s 目录成功", pDirPath);
        return true;
    }
    else
    {
        //打开目录失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 打开目录 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsCloseDir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  15:14:38
 * @Description    :      LittleFS API函数 之 关闭目录
 * @Return         :      
*******************************************************************************/
bool sLittleFsCloseDir(void)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 关闭目录 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_dir_close(&stLittleFs, &stDir);
    if(i32Error == LFS_ERR_OK)
    {
        //关闭目录成功
        u8FailCount = 0;
        EN_SLOGI(TAG, "关闭目录成功");
        return true;
    }
    else
    {
        //关闭目录失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 关闭目录 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}





/*******************************************************************************
 * @FunctionName   :      sLittleFsReadDir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月6日  15:20:07
 * @Description    :      LittleFS API函数 之 读目录
 * @Input          :      pInfo        
 * @Return         :      
*******************************************************************************/
bool sLittleFsReadDir(struct lfs_info *pInfo)
{
    i32 i32Error = -1;
    u16 u16FileCount = 0;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 读目录 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    
    //循环读取目录中的文件项
    i32Error = lfs_dir_read(&stLittleFs, &stDir, pInfo);
    if((i32Error < LFS_ERR_OK) || (pInfo->type == 0))
    {
        return false;
    }
    
    
    return true;
}




/*******************************************************************************
 * @FunctionName   :      sLittleFsRewindDir
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年8月13日  19:54:15
 * @Description    :      LittleFS API函数 之 指向目录头
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sLittleFsRewindDir(void)
{
    static u8 u8FailCount = 0;          //操作失败次数
    i32 i32Error = -1;
    
    
    if(bFlashDrvInited == false)
    {
        EN_SLOGE(TAG, "LittleFs 指向目录头 操作失败!!! FLASH底层驱动未初始化, 请先初始化FLASH底层驱动!!!");
        return false;
    }
    
    i32Error = lfs_dir_rewind(&stLittleFs, &stDir);
    if(i32Error == LFS_ERR_OK)
    {
        //指向目录头成功
        u8FailCount = 0;
        return true;
    }
    else
    {
        //指向目录头失败
        u8FailCount++;
        EN_SLOGE(TAG, "错误码: %d    LittleFs 指向目录头 失败 第%d次!!!", i32Error, u8FailCount);
    }
    
    
    return false;
}










