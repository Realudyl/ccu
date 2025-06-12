/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_littlefs.h
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
#ifndef _en_littlefs_H
#define _en_littlefs_H

#include "en_common.h"
#include "lfs.h"




//对应的littlefs库函数详细用法请参见 <littlefs-master/lfs.h>
#define mount()                         sLittleFsMount()
#define unmount()                       sLittleFsUnmount()
#define format()                        sLittleFsFormat()
#define remove(path)                    sLittleFsRemove(path)
#define rename(oldpath, newpath)        sLittleFsRename(oldpath, newpath)
#define mkdir(path)                     sLittleFsMkdir(path)
#define memsize()                       sLittleFsSize()
#define stat(info)                      sLittleFsStat(info)
#define pathstat(path, info)            sLittleFsPathStat(path, info)


#define f_Open(file, path)              sLittleFsOpenFile(file, path)
#define f_Close(file)                   sLittleFsCloseFile(file)
#define f_Sync(file)                    sLittleFsSyncFile(file)
#define f_Read(file, buffer, size)      sLittleFsReadFile(file, buffer, size)
#define f_Write(file, buffer, size)     sLittleFsWriteFile(file, buffer, size)
#define f_Seek(file, off, whence)       sLittleFsSeekFile(file, off, whence)
#define f_Size(file)                    sLittleFsSizeFile(file)
#define f_Tell(file)                    sLittleFsTellFile(file)

#define d_Open(path)                    sLittleFsOpenDir(path)
#define d_Close()                       sLittleFsCloseDir()
#define d_Read(info)                    sLittleFsReadDir(info)
#define d_Rewind()                      sLittleFsRewindDir()







extern bool sLittleFsMount(void);
extern bool sLittleFsUnmount(void);
extern bool sLittleFsFormat(void);
extern bool sLittleFsRemove(const char *pPath);
extern bool sLittleFsRename(const char *pOldPath, const char *pNewPath);
extern bool sLittleFsMkdir(const char *pPath);
extern i32  sLittleFsSize(void);
extern bool sLittleFsStat(struct lfs_fsinfo *pInfo);
extern bool sLittleFsPathStat(const char *pPath, struct lfs_info *pInfo);

extern bool sLittleFsOpenFile(lfs_file_t *pFile, const char *pFilePath);
extern bool sLittleFsCloseFile(lfs_file_t *pFile);
extern bool sLittleFsSyncFile(lfs_file_t *pFile);
extern bool sLittleFsReadFile(lfs_file_t *pFile, void *pBuffer, lfs_size_t u32Size);
extern bool sLittleFsWriteFile(lfs_file_t *pFile, const void *pBuffer, lfs_size_t u32Size);
extern i32  sLittleFsSeekFile(lfs_file_t *pFile, lfs_soff_t i32Offset, i32 i32WhenceFlag);
extern i32  sLittleFsSizeFile(lfs_file_t *pFile);
extern i32  sLittleFsTellFile(lfs_file_t *pFile);

extern bool sLittleFsOpenDir(const char *pDirPath);
extern bool sLittleFsCloseDir(void);
extern bool sLittleFsReadDir(struct lfs_info *pInfo);
extern bool sLittleFsRewindDir(void);







#endif

