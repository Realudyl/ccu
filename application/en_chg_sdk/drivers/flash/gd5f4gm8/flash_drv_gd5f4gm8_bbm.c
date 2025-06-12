/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     flash_drv_gd5f4gm8_bbm.c
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
#include "en_log.h"

#include "flash_drv_gd5f4gm8.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "flash_drv_gd5f4gm8_bbm";












//nand flash 坏块管理 DBT/RBT加载函数
bool sFlashDrvBbmLoadDbt(stFlashDrvBbmCache_t *pCache);
bool sFlashDrvBbmLoadRbt(stFlashDrvBbmCache_t *pCache);

//nand flash 坏块管理 DBT块检查函数
bool sFlashDrvBbmCheckBlockInDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block);

//nand flash 坏块管理 DBT/RBT更新函数
bool sFlashDrvBbmUpdateDbt(stFlashDrvBbmCache_t *pCache);
bool sFlashDrvBbmUpdateRbt(stFlashDrvBbmCache_t *pCache);


bool sFlashDrvBbmAddBadBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block);

bool sFlashDrvBbmReMappingRbt(stFlashDrvBbmCache_t *pCache, u16 u16OriBlock,u16 u16OldReplaceBlock);

u16  sFlashDrvBbmGetReplaceBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block);

bool sFlashDrvBbmAddNewBlockToDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block);

bool sFlashDrvBbmAllocBadBlockMap(stFlashDrvBbmCache_t *pCache);

bool sFlashDrvBbmInitBadBlock(stFlashDrvBbmCache_t *pCache);

bool sFlashDrvBbmRebuildDbtRbt(stFlashDrvBbmCache_t *pCache);






/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmBadBlockDetect
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  17:56:28
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 检查该块是否为 Bad Block
 * @Input          :      u32Block    
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmBadBlockDetect(u32 u32Block)
{
    bool bRst = false, bIsBadBlock = false;
    u8 u8Temp;
    unFlashDrvRegData_t unData;
    
    unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
    
    bRst = sFlashDrvCmdSetFt(eFlashDrvRegStatus1, (unData.u8Data & 0xEF));      // 关闭ECC
    
    bRst &= sFlashDrvRdSpareData((u32Block * cGd5f4gm8PerBlockPageNum), 0, 1, &u8Temp);
    if((bRst == true) && (u8Temp != 0xFF))
    {
        bIsBadBlock = true;
    }
    
    bRst &= sFlashDrvRdSpareData((u32Block * cGd5f4gm8PerBlockPageNum + 1), 0, 1, &u8Temp);
    if((bRst == true) && (u8Temp != 0xFF))
    {
        bIsBadBlock = true;
    }
    
    bRst &= sFlashDrvCmdSetFt(eFlashDrvRegStatus1, unData.u8Data);              // 开启ECC
    
    return bIsBadBlock;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmLoadDbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  13:32:33
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 Defect Block Table 加载
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmLoadDbt(stFlashDrvBbmCache_t *pCache)
{
    bool bRst = false;
    
    
    bRst = sFlashDrvRdPageData((pCache->u16DbtBlock[pCache->u8DbtCrtBlockIdx] * cGd5f4gm8PerBlockPageNum + pCache->u8DbtCrtPage), 0, cGd5f4gm8PerPageUserByteNum, pCache->u8TempBuf);
    
    if(bRst == true)
    {
        memcpy(pCache->u16Dbt, pCache->u8TempBuf, cDbtSize*sizeof(pCache->u16Dbt[0]));
    }
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmLoadRbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  13:32:46
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 Replace Block Table 加载
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmLoadRbt(stFlashDrvBbmCache_t *pCache)
{
    bool bRst = false;
    
    
    bRst  = sFlashDrvRdPageData((pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx] * cGd5f4gm8PerBlockPageNum + pCache->u8RbtCrtPage - 3), 0, cGd5f4gm8PerPageUserByteNum, pCache->u8TempBuf);
    if(bRst == true)
    {
        memcpy(pCache->u16Rbt, pCache->u8TempBuf, cGd5f4gm8PerPageUserByteNum);
    }
    
    bRst &= sFlashDrvRdPageData((pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx] * cGd5f4gm8PerBlockPageNum + pCache->u8RbtCrtPage - 2), 0, cGd5f4gm8PerPageUserByteNum, pCache->u8TempBuf);
    if(bRst == true)
    {
        memcpy(&pCache->u16Rbt[1024], pCache->u8TempBuf, cGd5f4gm8PerPageUserByteNum);
    }
    
    bRst &= sFlashDrvRdPageData((pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx] * cGd5f4gm8PerBlockPageNum + pCache->u8RbtCrtPage - 1), 0, cGd5f4gm8PerPageUserByteNum, pCache->u8TempBuf);
    if(bRst == true)
    {
        memcpy(&pCache->u16Rbt[2048], pCache->u8TempBuf, cGd5f4gm8PerPageUserByteNum);
    }
    
    bRst &= sFlashDrvRdPageData((pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx] * cGd5f4gm8PerBlockPageNum + pCache->u8RbtCrtPage)    , 0, cGd5f4gm8PerPageUserByteNum, pCache->u8TempBuf);
    if(bRst == true)
    {
        memcpy(&pCache->u16Rbt[3072],pCache->u8TempBuf,((cRbtSize * sizeof(pCache->u16Rbt[0]))- (cGd5f4gm8PerPageUserByteNum * 3)));
    }
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmCheckBlockInDbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  14:01:12
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 坏块检查
 * @Input          :      pCache      
 * @Input          :      u16Block    
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmCheckBlockInDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block)
{
    bool bRst = false;
    u16  u16OldDB;
    u16  u16OldDBNum;
    u16  u16Idx;
    
    
    u16OldDBNum = pCache->u16Dbt[0];
    
    for(u16Idx = 0; u16Idx < u16OldDBNum; u16Idx++)
    {
        u16OldDB = pCache->u16Dbt[2 + u16Idx];
        
        if(u16OldDB == u16Block)
        {
            bRst = true;
            break;
        }
    }
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmUpdateDbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  14:30:47
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 Defect Block Table 更新
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmUpdateDbt(stFlashDrvBbmCache_t *pCache)
{
    bool bRst = false;
    u16  u16BlockNo;
    u16  u16PageNo;
    u8   u8DbtEraseFlag;
    
    
    if((pCache->u8DbtCrtPage >= (cGd5f4gm8PerBlockPageNum-1)) && (pCache->u8DbtCrtPage != 0xFF))
    {
        pCache->u8DbtCrtBlockIdx = 1 - pCache->u8DbtCrtBlockIdx;                //toggle block idx
        pCache->u8DbtCrtPage = 0;
        u8DbtEraseFlag = true;
    }
    else
    {
        if(pCache->u8DbtCrtPage == 0xFF)
        {
            pCache->u8DbtCrtPage = 0;
        }
        else
        {
            pCache->u8DbtCrtPage ++;
        }
        u8DbtEraseFlag = false;
    }
    
    u16BlockNo = pCache->u16DbtBlock[pCache->u8DbtCrtBlockIdx];
    u16PageNo  = pCache->u8DbtCrtPage;
    
    memset(pCache->u8TempBuf, 0xFF, cGd5f4gm8PerPageTotalByteNum);
    memcpy(pCache->u8TempBuf, pCache->u16Dbt, (cDbtSize * sizeof(pCache->u16Dbt[0])));
    
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 0] = 'D';               //0x44
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 1] = 'B';               //0x42
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 2] = 'T';               //0x54
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 3] = '!';               //0x21
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 4] = (pCache->u16DbtBlock[1 - pCache->u8DbtCrtBlockIdx] & 0xFF);         //record the pair block No
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 5] = (pCache->u16DbtBlock[1 - pCache->u8DbtCrtBlockIdx] & 0xFF00) >> 8;
    
    bRst = sFlashDrvWrPageData((u16BlockNo * cGd5f4gm8PerBlockPageNum + u16PageNo), 0, cGd5f4gm8PerPageTotalByteNum, pCache->u8TempBuf);
    if((bRst == true) && (u8DbtEraseFlag == true))
    {
        bRst = sFlashDrvCmdBlockErase(pCache->u16DbtBlock[1 - pCache->u8DbtCrtBlockIdx]);
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmUpdateRbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  15:05:18
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 Replace Block Table 更新
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmUpdateRbt(stFlashDrvBbmCache_t *pCache)
{
    bool bRst = false;
    u16  u16BlockNo;
    u16  u16PageNo;
    u8   u8RbtEraseFlag;
    
    
    if((pCache->u8RbtCrtPage >= (cGd5f4gm8PerBlockPageNum - 4)) && (pCache->u8RbtCrtPage != 0xFF))
    {
        pCache->u8RbtCrtBlockIdx = 1 - pCache->u8RbtCrtBlockIdx;                //toggle block idx
        pCache->u8RbtCrtPage = 0;
        u8RbtEraseFlag = true;
    }
    else
    {
        if(pCache->u8RbtCrtPage == 0xFF)
        {
            pCache->u8RbtCrtPage = 0;
        }
        else
        {
            pCache->u8RbtCrtPage ++;
        }
        u8RbtEraseFlag = false;
    }
    
    u16BlockNo = pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx];
    u16PageNo  = pCache->u8RbtCrtPage;
    
    memset(pCache->u8TempBuf, 0xFF, cGd5f4gm8PerPageTotalByteNum);
    memcpy(pCache->u8TempBuf, pCache->u16Rbt, cGd5f4gm8PerPageUserByteNum);
    
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 0] = 'R';               //0x52
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 1] = 'B';               //0x42
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 2] = 'T';               //0x54
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 3] = '!';               //0x21
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 4] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF);         //record the pair block No
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 5] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF00) >> 8;
    
    bRst = sFlashDrvWrPageData((u16BlockNo * cGd5f4gm8PerBlockPageNum + u16PageNo), 0, cGd5f4gm8PerPageTotalByteNum, pCache->u8TempBuf);
    if(bRst == false)
    {
        return bRst;
    }
    
    
    pCache->u8RbtCrtPage++;
    u16BlockNo = pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx];
    u16PageNo  = pCache->u8RbtCrtPage;
    
    memset(pCache->u8TempBuf, 0xFF, cGd5f4gm8PerPageTotalByteNum);
    memcpy(pCache->u8TempBuf, &pCache->u16Rbt[1024], cGd5f4gm8PerPageUserByteNum);
    
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 0] = 'R';               //0x52
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 1] = 'B';               //0x42
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 2] = 'T';               //0x54
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 3] = '!';               //0x21
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 4] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF);         //record the pair block No
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 5] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF00) >> 8;
    
    bRst = sFlashDrvWrPageData((u16BlockNo * cGd5f4gm8PerBlockPageNum + u16PageNo), 0, cGd5f4gm8PerPageTotalByteNum, pCache->u8TempBuf);
    if(bRst == false)
    {
        return bRst;
    }
    
    
    pCache->u8RbtCrtPage++;
    u16BlockNo = pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx];
    u16PageNo  = pCache->u8RbtCrtPage;
    
    memset(pCache->u8TempBuf, 0xFF, cGd5f4gm8PerPageTotalByteNum);
    memcpy(pCache->u8TempBuf, &pCache->u16Rbt[2048], cGd5f4gm8PerPageUserByteNum);
    
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 0] = 'R';               //0x52
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 1] = 'B';               //0x42
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 2] = 'T';               //0x54
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 3] = '!';               //0x21
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 4] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF);         //record the pair block No
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 5] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF00) >> 8;
    
    bRst = sFlashDrvWrPageData((u16BlockNo * cGd5f4gm8PerBlockPageNum + u16PageNo), 0, cGd5f4gm8PerPageTotalByteNum, pCache->u8TempBuf);
    if(bRst == false)
    {
        return bRst;
    }
    
    
    pCache->u8RbtCrtPage++;
    u16BlockNo = pCache->u16RbtBlock[pCache->u8RbtCrtBlockIdx];
    u16PageNo  = pCache->u8RbtCrtPage;
    
    memset(pCache->u8TempBuf, 0xFF, cGd5f4gm8PerPageTotalByteNum);
    memcpy(pCache->u8TempBuf, &pCache->u16Rbt[3072], ((cRbtSize * sizeof(pCache->u16Rbt[0])) - (cGd5f4gm8PerPageUserByteNum * 3)));
    
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 0] = 'R';               //0x52
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 1] = 'B';               //0x42
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 2] = 'T';               //0x54
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 3] = '!';               //0x21
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 4] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF);         //record the pair block No
    pCache->u8TempBuf[cGd5f4gm8PerPageUserByteNum + 4 + 5] = (pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx] & 0xFF00) >> 8;
    
    bRst = sFlashDrvWrPageData((u16BlockNo * cGd5f4gm8PerBlockPageNum + u16PageNo), 0, cGd5f4gm8PerPageTotalByteNum, pCache->u8TempBuf);
    
    if((bRst == true) && (u8RbtEraseFlag == true))
    {
        bRst = sFlashDrvCmdBlockErase(pCache->u16RbtBlock[1 - pCache->u8RbtCrtBlockIdx]);
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmAddBadBlock
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  16:06:35
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 新增坏块
 * @Input          :      pCache      
 * @Input          :      u16Block    
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmAddBadBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block)
{
    if(u16Block <= cUserAreaEnd)
    {
        pCache->u16Rbt[u16Block] = pCache->u16LastValidBlock;
        pCache->u16LastValidBlock --;
    }
    
    if(pCache->u16LastValidBlock <= cReplaceBlockAreaStart)
    {
        EN_SLOGE(TAG, "Replace Block are not enough");
        //while(1);
        return false;
    }
    
    pCache->u16BadBlockNum ++; 
    pCache->u16Dbt[0] = pCache->u16BadBlockNum;                                 //存放坏块个数
    pCache->u16Dbt[1] = pCache->u16LastValidBlock;                              //Replace Block 中的当前可使用的块号(由大到小使用顺序)
    pCache->u16Dbt[1 + pCache->u16BadBlockNum] = u16Block;
    
    return true;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmReMappingRbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  16:26:07
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 重新映射 Replace Block Table
 * @Input          :      pCache      
 * @Input          :      u16OriBlock 
 * @Input          :      u16OldReplaceBlock
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmReMappingRbt(stFlashDrvBbmCache_t *pCache, u16 u16OriBlock,u16 u16OldReplaceBlock)
{
    if(0 == ((u16OriBlock <= cUserAreaEnd) && (cReplaceBlockAreaStart <= u16OldReplaceBlock) && (u16OldReplaceBlock <= cReplaceBlockAreaEnd)))
    {
        return false;
    }
    
    if(pCache->u16Rbt[u16OriBlock] != u16OldReplaceBlock)
    {
        return false;
    }
    
    if(u16OriBlock == u16OldReplaceBlock)
    {
        return true;
    }
    
    pCache->u16Rbt[u16OriBlock] = pCache->u16LastValidBlock;
    pCache->u16LastValidBlock --;
    
    if(pCache->u16LastValidBlock <= cReplaceBlockAreaStart)
    {
        EN_SLOGE(TAG, "Replace Block are not enough");
        //while(1);
        return false;
    }
    
    pCache->u16Dbt[1] = pCache->u16LastValidBlock;
    
    return true;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmGetReplaceBlock
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  17:10:32
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 获取 Replace Block
 * @Input          :      pCache      
 * @Input          :      u16Block    
 * @Return         :      
*******************************************************************************/
u16 sFlashDrvBbmGetReplaceBlock(stFlashDrvBbmCache_t *pCache, u16 u16Block)
{
    return pCache->u16Rbt[u16Block];
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmAddNewBlockToDbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  17:19:40
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 新增坏块到 Defect Block Table
 * @Input          :      pCache      
 * @Input          :      u16Block    
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmAddNewBlockToDbt(stFlashDrvBbmCache_t *pCache, u16 u16Block)
{
    bool bRst;
    
    bRst = sFlashDrvBbmCheckBlockInDbt(pCache, u16Block);
    
    if(bRst == false)
    {
        bRst = sFlashDrvBbmAddBadBlock(pCache, u16Block);
    }
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmAllocBadBlockMap
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  19:48:19
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 分配建立 DBT和RBT 映射表
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmAllocBadBlockMap(stFlashDrvBbmCache_t *pCache)
{
    u16 u16Block = cTableAreaEnd;
    u8  u8Idx;
    
    
    for(u8Idx = 0; u8Idx < cDbtBLockNum; u8Idx++)
    {
        for( ; u16Block > cTableAreaStart; u16Block --)
        {
            if(sFlashDrvBbmBadBlockDetect(u16Block) == false)
            {
                pCache->u16DbtBlock[u8Idx] = u16Block;
                u16Block --;
                break;
            }
            else
            {
                pCache->u16BadBlockNum ++;
                pCache->u16Dbt[1 + pCache->u16BadBlockNum] = u16Block;
            }
        }
    }
    pCache->u8DbtCrtBlockIdx = 0;
    pCache->u8DbtCrtPage     = 0xFF;
    
    
    for(u8Idx = 0; u8Idx < cRbtBLockNum; u8Idx++)
    {
        for( ; u16Block > cTableAreaStart; u16Block --)
        {
            if(sFlashDrvBbmBadBlockDetect(u16Block) == false)
            {
                pCache->u16RbtBlock[u8Idx] = u16Block;
                u16Block --;
                break;
            }
            else
            {
                pCache->u16BadBlockNum ++;
                pCache->u16Rbt[1 + pCache->u16BadBlockNum] = u16Block;
            }
        }
    }
    pCache->u8RbtCrtBlockIdx = 0;
    pCache->u8RbtCrtPage     = 0xFF;
    
    return (true);
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmInitBadBlock
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  20:24:37
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 初始化 Bad Block
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmInitBadBlock(stFlashDrvBbmCache_t *pCache)
{
    bool bRst = true;
    u16  u16Block;
    
    pCache->u16LastValidBlock = cReplaceBlockAreaEnd;
    
    for(u16Block = 0; u16Block <= cUserAreaEnd; u16Block++)
    {
        if(pCache->u16LastValidBlock < cReplaceBlockAreaStart)
        {
            break;
        }
        
        if(sFlashDrvBbmBadBlockDetect(u16Block) == 1)
        {
            while(sFlashDrvBbmBadBlockDetect(pCache->u16LastValidBlock) == 1)
            {
                pCache->u16BadBlockNum++;
                pCache->u16Dbt[1+pCache->u16BadBlockNum] = pCache->u16LastValidBlock;
                pCache->u16LastValidBlock--;
            }
            bRst &= sFlashDrvBbmAddBadBlock(pCache, u16Block);
            if(bRst == false)
            {
                break;
            }
        }
        else
        {
            pCache->u16Rbt[u16Block] = u16Block;
        }
    }
    
    pCache->u16Dbt[0] = pCache->u16BadBlockNum;
    pCache->u16Dbt[1] = pCache->u16LastValidBlock;
    
    return bRst;
}










/*******************************************************************************
 * @FunctionName   :      sFlashDrvBbmRebuildDbtRbt
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月12日  17:59:21
 * @Description    :      nand flash芯片驱动 ———— 坏块管理 之 重构 DBT和RBT
 * @Input          :      pCache      
 * @Return         :      
*******************************************************************************/
bool sFlashDrvBbmRebuildDbtRbt(stFlashDrvBbmCache_t *pCache)
{
    u16  u16Block = cGd5f4gm8BlockNum - 1;
    u16  u16Page  = 0;
    u16  u16PairBlock;
    u8   u8SpareData1[6];
    u8   u8SpareData2[6];
    bool bFlagDbtExist = false, bFlagRbtExist = false;
    bool bRst = true;
    
    
    //检查nand中是否存在DBT
    for(u16Block = cTableAreaEnd; u16Block >= cTableAreaStart; u16Block --)
    {
        u16Page = 0;
        bRst = sFlashDrvRdSpareData((u16Block * cGd5f4gm8PerBlockPageNum + u16Page), 4, 6, u8SpareData1);
        if((bRst == true) && ((u8SpareData1[0] == 'D') && (u8SpareData1[1] == 'B') && (u8SpareData1[2] == 'T') && (u8SpareData1[3] == '!')))
        {
            //检测到DBT标记
            bFlagDbtExist = true;
            break;
        }
    }
    
    if(bFlagDbtExist == true)
    {
        for(u16Page = cGd5f4gm8PerBlockPageNum - 1; (u16Page < cGd5f4gm8PerBlockPageNum); u16Page--)
        {
            bRst &= sFlashDrvRdSpareData((u16Block * cGd5f4gm8PerBlockPageNum + u16Page), 4, 6, u8SpareData1);
            if((bRst == true) && ((u8SpareData1[0] == 'D') && (u8SpareData1[1] == 'B') && (u8SpareData1[2] == 'T') && (u8SpareData1[3] == '!')))
            {
                break;
            }
        }
        
        pCache->u8DbtCrtBlockIdx = 0;
        u16PairBlock = u8SpareData1[4] + (u8SpareData1[5] << 8);
        bRst &= sFlashDrvRdSpareData((u16PairBlock * cGd5f4gm8PerBlockPageNum + 0), 4, 6, u8SpareData2);
        
        //block的最后一个page，说明最新的Table可能在另一个block的Page0中。
        if((bRst == true) && (u16Page == (cGd5f4gm8PerBlockPageNum - 1)))
        {
            //另一个block有内容，说明Erase时掉电了
            if((u8SpareData2[0] == 'D') && (u8SpareData2[1] == 'B') && (u8SpareData2[2] == 'T') && (u8SpareData2[3] == '!'))
            {
                u16Page = 0;
                pCache->u8DbtCrtBlockIdx = 1;
                bRst &= sFlashDrvCmdBlockErase(u16Block);                       //擦掉(因掉电造成未擦除的)写满的block
            }
        }
        if(bRst == true)
        {
            pCache->u16DbtBlock[0] = u16Block;
            pCache->u16DbtBlock[1] = u16PairBlock;
            pCache->u8DbtCrtPage   = u16Page;
        }
        
        bRst &= sFlashDrvBbmLoadDbt(pCache);
        if(bRst == true)
        {
            pCache->u16BadBlockNum    = pCache->u16Dbt[0];                      //bad block number
            pCache->u16LastValidBlock = pCache->u16Dbt[1];                      //record last valid block
            EN_SLOGD(TAG, "目前FLASH已知 坏块 数量 为 %d , 可用有效块 数量 为 %d ", pCache->u16BadBlockNum, pCache->u16LastValidBlock);
        }
    }
    else
    {
        //如果没找到就重新全盘扫描，重建DBT、RBT
        bRst  = sFlashDrvBbmAllocBadBlockMap(pCache);
        bRst &= sFlashDrvBbmInitBadBlock(pCache);
        bRst &= sFlashDrvBbmUpdateDbt(pCache);
        bRst &= sFlashDrvBbmUpdateRbt(pCache);
        
        return bRst;
    }
    
    
    
    //检查nand中是否存在RBT
    for(u16Block = cTableAreaEnd; u16Block >= cTableAreaStart; u16Block --)
    {
        u16Page = 0;
        bRst = sFlashDrvRdSpareData((u16Block * cGd5f4gm8PerBlockPageNum + u16Page), 4, 6, u8SpareData1);
        if((bRst == true) && ((u8SpareData1[0] == 'R') && (u8SpareData1[1] == 'B') && (u8SpareData1[2] == 'T') && (u8SpareData1[3] == '!')))
        {
            bFlagRbtExist = true;
            break;
        }
    }
    
    if(bFlagRbtExist == true)
    {
        for(u16Page = cGd5f4gm8PerBlockPageNum - 1; (u16Page < cGd5f4gm8PerBlockPageNum); u16Page--)
        {
            bRst &= sFlashDrvRdSpareData((u16Block * cGd5f4gm8PerBlockPageNum + u16Page), 4, 6, u8SpareData1);
            if((bRst == true) && ((u8SpareData1[0] == 'R') && (u8SpareData1[1] == 'B') && (u8SpareData1[2] == 'T') && (u8SpareData1[3] == '!')))
            {
                break;
            }
        }
        
        pCache->u8RbtCrtBlockIdx = 0;
        u16PairBlock = u8SpareData1[4] + (u8SpareData1[5] << 8);
        bRst &= sFlashDrvRdSpareData((u16PairBlock * cGd5f4gm8PerBlockPageNum + 0), 4, 6, u8SpareData2);
        
        //block的最后一个page，说明最新的Table可能在另一个block的Page0中。
        if((bRst == true) && (u16Page == (cGd5f4gm8PerBlockPageNum - 1)))
        {
            //另一个block有内容，说明Erase时掉电了
            if((u16Page == cGd5f4gm8PerBlockPageNum - 1)||(u16Page == cGd5f4gm8PerBlockPageNum - 2)||(u16Page == cGd5f4gm8PerBlockPageNum - 3)||(u16Page == cGd5f4gm8PerBlockPageNum - 4))
            {
                if((u8SpareData2[0] == 'R') && (u8SpareData2[1] == 'B') && (u8SpareData2[2] == 'T') && (u8SpareData2[3] == '!'))
                {
                    u16Page = 3;
                    pCache->u8RbtCrtBlockIdx = 1;
                }
            }
        }
        if(bRst == true)
        {
            pCache->u16RbtBlock[0] = u16Block;
            pCache->u16RbtBlock[1] = u16PairBlock;
            pCache->u8RbtCrtPage   = u16Page;
            if(pCache->u8RbtCrtPage > 0)
            {
                bRst &= sFlashDrvBbmLoadRbt(pCache);
            }
            else
            {
                bRst = false;
            }
        }
    }
    else
    {
        //DBT exist but RBT not exist
        bRst = false;
    }
    
    return bRst;
}











