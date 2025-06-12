/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     flash_drv_gd5f4gm8_bbm.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-7-10
 * @Attention             :     
 * @Brief                 :     GD5F4GM8 SPI NAND FLASH芯片驱动实现 ———— 坏块管理
 * 
 * @History:
 * 
 * 1.@Date: 2024-7-10
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _flash_drv_gd5f4gm8_bbm_H
#define _flash_drv_gd5f4gm8_bbm_H
#include "flash_drv_gd5f4gm8_data_def.h"















//nand flash 坏块管理缓存结构体定义
typedef struct
{
    u8  u8TempBuf[cGd5f4gm8PerPageTotalByteNum];                                //中间数据缓存数组
    
    u16 u16Dbt[cDbtSize];                                                       //Defect Block Table
    u16 u16Rbt[cRbtSize];                                                       //Replace Block Table
    
    u16 u16DbtBlock[cDbtBLockNum];
    u8  u8DbtCrtBlockIdx;                                                       //0 or 1
    u8  u8DbtCrtPage;
    
    u16 u16RbtBlock[cRbtBLockNum];
    u8  u8RbtCrtBlockIdx;
    u8  u8RbtCrtPage;
    
    u16 u16LastValidBlock;
    u16 u16BadBlockNum;
    
}stFlashDrvBbmCache_t;



















//nand flash 坏块管理 DBT/RBT加载函数
extern bool sFlashDrvBbmLoadDbt(stFlashDrvBbmCache_t *pCache);
extern bool sFlashDrvBbmLoadRbt(stFlashDrvBbmCache_t *pCache);

//nand flash 坏块管理 DBT块检查函数
extern bool sFlashDrvBbmCheckBlockInDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block);

//nand flash 坏块管理 DBT/RBT更新函数
extern bool sFlashDrvBbmUpdateDbt(stFlashDrvBbmCache_t *pCache);
extern bool sFlashDrvBbmUpdateRbt(stFlashDrvBbmCache_t *pCache);


extern bool sFlashDrvBbmAddBadBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block);

extern bool sFlashDrvBbmReMappingRbt(stFlashDrvBbmCache_t *pCache, u16 u16OriBlock,u16 u16OldReplaceBlock);

extern u16  sFlashDrvBbmGetReplaceBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block);

extern bool sFlashDrvBbmAddNewBlockToDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block);

extern bool sFlashDrvBbmAllocBadBlockMap(stFlashDrvBbmCache_t *pCache);

extern bool sFlashDrvBbmInitBadBlock(stFlashDrvBbmCache_t *pCache);

extern bool sFlashDrvBbmRebuildDbtRbt(stFlashDrvBbmCache_t *pCache);














#endif
