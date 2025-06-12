/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   flash_drv_gd5f4gm8.c
* Description                           :   GD5F4GM8 SPI NAND FLASH芯片驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-05
* notice                                :   
*                                           flash 芯片对cs/nss信号的时序有要求
*                                           一个命令整个执行期间，该信号必须持续拉低，
*                                           因此不能使用spi驱动内置的nss操作逻辑，
*                                           而要由flash芯片驱动自己来控制cs/nss信号
****************************************************************************************************/
#include "en_log.h"

#include "flash_drv_gd5f4gm8.h"










//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "flash_drv_gd5f4gm8";






stFlashDrvDevice_t stFlashDrvDev = 
{
    false,
    eSpiNum4,
    {SPI_TRANSMODE_FULLDUPLEX, SPI_MASTER, SPI_DATASIZE_8BIT, SPI_NSS_SOFT, SPI_ENDIAN_MSB, SPI_CK_PL_LOW_PH_1EDGE, SPI_PSC_4},
    4096,
    64,
    2175,
    NULL,
    NULL
};







bool sFlashDrvInit(void);




//nand flash芯片数据读/写函数
bool sFlashDrvRdData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
bool sFlashDrvWrData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);

//nand flash芯片页数据转移函数
bool sFlashDrvMovePageData(u16 u16DesBlock, u16 u16SrcBloc, u8 u8Page);

//nand flash芯片页数据读/写函数
bool sFlashDrvRdPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
bool sFlashDrvWrPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);

//nand flash芯片Spare空间读/写数据函数
bool sFlashDrvRdSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
bool sFlashDrvWrSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);




//nand flash芯片基本操作命令
bool sFlashDrvCmdSetWrEnable(void);
bool sFlashDrvCmdSetWrDisable(void);
u8   sFlashDrvCmdGetFt(eFlashDrvReg_t eReg);
bool sFlashDrvCmdSetFt(eFlashDrvReg_t eReg, u8 u8Data);
bool sFlashDrvCmdRdPage(u32 u32Page);
bool sFlashDrvCmdRdCache(u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
bool sFlashDrvCmdRdId(u8 *pIdM, u8 *pIdD);
bool sFlashDrvCmdProgramLoad(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf);
bool sFlashDrvCmdProgramExe(u32 u32Page);
bool sFlashDrvCmdProgramLoadRandom(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf);
bool sFlashDrvCmdBlockErase(u32 u32Block);
bool sFlashDrvCmdReset(void);
















/**********************************************************************************************
* Description                           :   nand flash芯片驱动资源初始化
* Author                                :   Hall
* modified Date                         :   2024-07-05
* notice                                :   
***********************************************************************************************/
bool sFlashDrvInit(void)
{
    bool bRst = false, bDebugErase = false;
    stFlashDrvDevice_t *pDevice = &stFlashDrvDev;
    
    pDevice->pBbmCache = (stFlashDrvBbmCache_t*)MALLOC(sizeof(stFlashDrvBbmCache_t));
    if(pDevice->pBbmCache == NULL)
    {
        return bRst;
    }
    memset(pDevice->pBbmCache, 0, sizeof(stFlashDrvBbmCache_t));
    
    // 初始化SPI驱动
    bRst = sSpiDrvOpen(pDevice->eSpiNum, &pDevice->stSpiCfg);
    
    bRst &= sFlashDrvCmdReset();
    bRst &= sFlashDrvCmdSetFt(eFlashDrvRegProtection, 0x00);
    bRst &= sFlashDrvCmdSetFt(eFlashDrvRegFeature1, 0x10);
    bRst &= sFlashDrvCmdRdId(NULL, NULL);
    
    if(bDebugErase == true)
    {
        //全片擦除
        for(int i = 0; i < cGd5f4gm8BlockNum; i++)
        {
            sFlashDrvCmdBlockErase(i);
        }
    }
    
    if(bRst == true)
    {
        if(sFlashDrvBbmRebuildDbtRbt(pDevice->pBbmCache) == true)
        {
            pDevice->eInitState = true;
        }
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvRdData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月15日  9:52:58
 * @Description    :      nand flash芯片基本操作命令--->读数据
 * @Input          :      u16Block          
 * @Input          :      u8Page            
 * @Input          :      u16AddrColumn     
 * @Input          :      i32Len            
 * @Input          :      pBuf              
 * @Return         :      
*******************************************************************************/
bool sFlashDrvRdData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    stFlashDrvDevice_t *pDevice = &stFlashDrvDev;
    bool bRst = false;
    u16 u16ReplaceBlock;
    
    
    //1:参数限幅
    if((u16Block > cUserAreaEnd) || (u8Page >= cGd5f4gm8PerBlockPageNum) || (i32Len > cGd5f4gm8PerPageUserByteNum))
    {
        return bRst;
    }
    
    
    //2:判断要读的块是否在DBT中
    if(sFlashDrvBbmCheckBlockInDbt(pDevice->pBbmCache, u16Block) == true)
    {
        //若是，则说明该块被标记为坏块，从RBT中获取该块所属的替代块
        u16ReplaceBlock = sFlashDrvBbmGetReplaceBlock(pDevice->pBbmCache, u16Block);
    }
    else
    {
        //若否，则说明该块未被标记为坏块
        u16ReplaceBlock = u16Block;
    }
    
    
    //3:读数据
    bRst = sFlashDrvRdPageData((u16ReplaceBlock * cGd5f4gm8PerBlockPageNum + u8Page), u16AddrColumn, i32Len, pBuf);
    
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sFlashDrvWrData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月15日  9:52:56
 * @Description    :      nand flash芯片基本操作命令--->写数据
 * @Input          :      u16Block          
 * @Input          :      u8Page            
 * @Input          :      u16AddrColumn     
 * @Input          :      i32Len            
 * @Input          :      pBuf              
 * @Return         :      
*******************************************************************************/
bool sFlashDrvWrData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    stFlashDrvDevice_t *pDevice = &stFlashDrvDev;
    bool bRst = false;
    u8   u8PageIndex;
    u16  u16ReplaceBlock, u16NewReplaceBlock;
    
    
    //1:参数限幅
    if((u16Block > cUserAreaEnd) || (u8Page >= cGd5f4gm8PerBlockPageNum) || (i32Len > cGd5f4gm8PerPageUserByteNum))
    {
        return bRst;
    }
    
    
    //2:判断要读的块是否在DBT中
    if(sFlashDrvBbmCheckBlockInDbt(pDevice->pBbmCache, u16Block) == true)
    {
        //若是，则说明该块被标记为坏块，从RBT中获取该块所属的替代块
        u16ReplaceBlock = sFlashDrvBbmGetReplaceBlock(pDevice->pBbmCache, u16Block);
    }
    else
    {
        //若否，则说明该块未被标记为坏块
        u16ReplaceBlock = u16Block;
    }
    
    
    memset(pDevice->pBbmCache->u8TempBuf, 0x5A, cGd5f4gm8PerPageUserByteNum);
    memcpy(pDevice->pBbmCache->u8TempBuf, pBuf, i32Len);
    
    
    //3:写数据
    bRst = sFlashDrvWrPageData((u16ReplaceBlock * cGd5f4gm8PerBlockPageNum + u8Page), u16AddrColumn, cGd5f4gm8PerPageUserByteNum, pDevice->pBbmCache->u8TempBuf);
    if(bRst == false)
    {
        //若写数据失败，则说明该块为坏块
        
        //1.新增该块为坏块到DBT
        sFlashDrvBbmAddNewBlockToDbt(pDevice->pBbmCache, u16ReplaceBlock);
        
        //2.重映射RBT
        sFlashDrvBbmReMappingRbt(pDevice->pBbmCache, u16Block,u16ReplaceBlock);
        
        //3.更新DBT和RBT
        sFlashDrvBbmUpdateDbt(pDevice->pBbmCache);
        sFlashDrvBbmUpdateRbt(pDevice->pBbmCache);
        
        //4.将旧的块内容复制到新分配的块中
        for(u8PageIndex = 0; u8PageIndex < u8Page; u8PageIndex++)
        {
            u16NewReplaceBlock = sFlashDrvBbmGetReplaceBlock(pDevice->pBbmCache, u16ReplaceBlock);
            sFlashDrvMovePageData(u16NewReplaceBlock, u16ReplaceBlock, u8PageIndex);
        }
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvMovePageData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  16:57:59
 * @Description    :      nand flash芯片基本操作命令--->移动数据
 * @Input          :      u16DesBlock       
 * @Input          :      u16SrcBloc        
 * @Input          :      u8Page            
 * @Return         :      
*******************************************************************************/
bool sFlashDrvMovePageData(u16 u16DesBlock, u16 u16SrcBloc, u8 u8Page)
{
    stFlashDrvDevice_t *pDevice = &stFlashDrvDev;
    bool bRst;
    
    bRst = sFlashDrvRdPageData((u16SrcBloc * cGd5f4gm8PerBlockPageNum + u8Page), 0, cGd5f4gm8PerPageTotalByteNum, pDevice->pBbmCache->u8TempBuf);
    
    if(bRst == true)
    {
        bRst = sFlashDrvWrPageData((u16DesBlock * cGd5f4gm8PerBlockPageNum + u8Page), 0, cGd5f4gm8PerPageTotalByteNum, pDevice->pBbmCache->u8TempBuf);
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvRdPageData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月9日  19:43:37
 * @Description    :      nand flash芯片基本操作命令--->页读数据
 * @Input          :      u32Page           要读数据的页数
 * @Input          :      u16AddrColumn     要读数据的页数偏移地址
 * @Input          :      i32Len            要读数据的长度
 * @Input          :      pBuf              指向储存数据的u8指针
 * @Return         :      
*******************************************************************************/
bool sFlashDrvRdPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    bool bRst = false;
    u8 u8RetryCount = 0;
    unFlashDrvRegData_t unData;
    
    if(((u16AddrColumn + i32Len) > cGd5f4gm8PerPageTotalByteNum)
    || (u32Page > cGd5f4gm8TotalPageNum))
    {
        //u16AddrColumn与i32Len之和需在每页可容纳最大字节数范围内
        return bRst;
    }
    
cReadRetry:
    bRst  = sFlashDrvCmdRdPage(u32Page);
    bRst &= sFlashDrvCmdRdCache(u16AddrColumn, i32Len, pBuf);
    
    if(bRst == true)
    {
        unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
        if((unData.stSts1.bEccs == 0b10) && (u8RetryCount < 3))
        {
            //根据<GD5F4GM8UE-Rev1.2.pdf> Table 12-3. ECC Error Bits Descriptions
            //ECCS = 0b10 表示 Bit errors greater than ECC capability(8 bits) and not corrected
            //这里给予3次重试机会
            u8RetryCount++;
            goto cReadRetry;
        }
        
        if(u8RetryCount >= 3)
        {
            bRst = false;
        }
    }
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvWrPageData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  9:44:07
 * @Description    :      nand flash芯片基本操作命令--->页写数据
 * @Input          :      u32Page           要写数据的页数
 * @Input          :      u16AddrColumn     要写数据的页数偏移地址
 * @Input          :      i32Len            要写数据的长度
 * @Input          :      pBuf              指向储存数据的u8指针
 * @Return         :      
*******************************************************************************/
bool sFlashDrvWrPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    bool bRst;
    
    bRst  = sFlashDrvCmdProgramLoad(u16AddrColumn, i32Len, pBuf);
    bRst &= sFlashDrvCmdProgramExe(u32Page);
    
    return bRst;
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvRdSpareData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  10:22:45
 * @Description    :      nand flash芯片基本操作命令--->读存在Spare空间的数据
 * @Input          :      u32Page           要读数据的页数
 * @Input          :      u16AddrColumn     要读数据的页数偏移地址
 * @Input          :      i32Len            要读数据的长度
 * @Input          :      pBuf              指向储存数据的u8指针
 * @Return         :      
*******************************************************************************/
bool sFlashDrvRdSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    if(i32Len > cGd5f4gm8PageSpareAreaByteNum)
    {
        return false;
    }
    
    return sFlashDrvRdPageData(u32Page, (cGd5f4gm8PerPageUserByteNum + u16AddrColumn), i32Len, pBuf);
}






/*******************************************************************************
 * @FunctionName   :      sFlashDrvWrSpareData
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年7月10日  10:26:30
 * @Description    :      nand flash芯片基本操作命令--->往Spare空间写入数据
 * @Input          :      u32Page           要写数据的页数
 * @Input          :      u16AddrColumn     要写数据的页数偏移地址
 * @Input          :      i32Len            要写数据的长度
 * @Input          :      pBuf              指向储存数据的u8指针
 * @Return         :      
*******************************************************************************/
bool sFlashDrvWrSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    if(i32Len > cGd5f4gm8PageSpareAreaByteNum)
    {
        return false;
    }
    
    return sFlashDrvWrPageData(u32Page, (cGd5f4gm8PerPageUserByteNum + u16AddrColumn), i32Len, pBuf);
}






/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Write Enable
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdSetWrEnable(void)
{
    bool bRst;
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stReset.eCmd = eFlashDrvCmd06;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stReset), unCmd.u8Byte);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}






/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Write Disable
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdSetWrDisable(void)
{
    bool bRst;
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stReset.eCmd = eFlashDrvCmd04;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stReset), unCmd.u8Byte);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->读特性
* Author                                :   Hall
* modified Date                         :   2024-07-05
* notice                                :   
***********************************************************************************************/
u8 sFlashDrvCmdGetFt(eFlashDrvReg_t eReg)
{
    u8 u8Data = 0;
    unFlashDrvCmd_t unCmd;
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stGetFt.eCmd = eFlashDrvCmd0f;
    unCmd.stGetFt.eReg = eReg;
    sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stGetFt), unCmd.u8Byte);
    
    //发送完命令头以后 读取一个字节的特性值
    sSpiDrvRecv(stFlashDrvDev.eSpiNum, 1, &u8Data);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return u8Data;
}





/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->写特性
* Author                                :   Hall
* modified Date                         :   2024-07-05
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdSetFt(eFlashDrvReg_t eReg, u8 u8Data)
{
    bool bRst;
    i32  i32Timeout = 0;
    unFlashDrvCmd_t unCmd;
    unFlashDrvRegData_t unData;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stSetFt.eCmd = eFlashDrvCmd1f;
    unCmd.stSetFt.eReg = eReg;
    unCmd.stSetFt.u8Data = u8Data;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stSetFt), unCmd.u8Byte);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    while((i32Timeout < 1000) && (bRst == true))
    {
        i32Timeout++;
        unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
        if(unData.stSts1.bOip == eFlashDrvRegOipReady)
        {
            return bRst;
        }
    }
    
    bRst = false;
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Page Read (to cache)
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdRdPage(u32 u32Page)
{
    bool bRst;
    i32  i32Timeout = 0;
    unFlashDrvCmd_t unCmd;
    unFlashDrvRegData_t unData;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stRdPage.eCmd = eFlashDrvCmd13;
    unCmd.stRdPage.u8Addr[0] = (u32Page >> 16) & 0xff;
    unCmd.stRdPage.u8Addr[1] = (u32Page >>  8) & 0xff;
    unCmd.stRdPage.u8Addr[2] = (u32Page >>  0) & 0xff;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stRdPage), unCmd.u8Byte);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    while((i32Timeout < 1000) && (bRst == true))
    {
        i32Timeout++;
        unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
        if(unData.stSts1.bOip == eFlashDrvRegOipReady)
        {
            return bRst;
        }
    }
    
    bRst = false;
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Read From Cache
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
*                                           u16AddrColumn:列地址。12位地址能够寻址从0到4095字节；
*                                                         但是，只有0到2175/2111字节有效。
*                                                         每页的字节2176/2112到4095都是“越界”的，
*                                                         在设备中不存在，并且无法寻址。
***********************************************************************************************/
bool sFlashDrvCmdRdCache(u16 u16AddrColumn, i32 i32Len, u8 *pBuf)
{
    bool bRst;
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stRdCache.eCmd = eFlashDrvCmd03;
    unCmd.stRdCache.u8AddrH = (u16AddrColumn >> 8) & 0xff;
    unCmd.stRdCache.u8AddrL =  u16AddrColumn & 0xff;
    unCmd.stRdCache.u8Dummy = 0xA5;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stRdCache), unCmd.u8Byte);
    
    bRst = bRst & sSpiDrvRecv(stFlashDrvDev.eSpiNum, i32Len, pBuf);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}










/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->读ID
* Author                                :   Hall
* modified Date                         :   2024-07-05
* notice                                :   
*                                           pIdM    :   Manufacturer ID
*                                           pIdD    :   Device ID
***********************************************************************************************/
bool sFlashDrvCmdRdId(u8 *pIdM, u8 *pIdD)
{
    bool bRst = false;
    u8   u8Data[3] = {0};
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stGetId.eCmd = eFlashDrvCmd9f;
    sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stGetId), unCmd.u8Byte);
    
    if(sSpiDrvRecv(stFlashDrvDev.eSpiNum, sizeof(u8Data), u8Data) == true)      //发送完命令头以后 读取3个字节的特性值
    {
        if(pIdM != NULL)
        {
            (*pIdM) = u8Data[1];
        }
        
        if(pIdD != NULL)
        {
            (*pIdD) = u8Data[2];
        }
        
        bRst = true;
        EN_SLOGD(TAG, "Flash Cmd:Read Id--->Manufacturer ID:0x%02X, Device ID:0x%02X", u8Data[1], u8Data[2]);
    }
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Program Load
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
*                                           u16AddrColumn:列地址。12位地址能够寻址从0到4095字节；
*                                                         但是，只有0到2175/2111字节有效。
*                                                         每页的字节2176/2112到4095都是“越界”的，
*                                                         在设备中不存在，并且无法寻址。
***********************************************************************************************/
bool sFlashDrvCmdProgramLoad(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf)
{
    bool bRst;
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stProgramLoad.eCmd = eFlashDrvCmd02;
    unCmd.stProgramLoad.u8AddrH = (u16AddrColumn >> 8) & 0xff;
    unCmd.stProgramLoad.u8AddrL =  u16AddrColumn & 0xff;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stProgramLoad), unCmd.u8Byte);
    
    bRst = bRst & sSpiDrvSend(stFlashDrvDev.eSpiNum, i32Len, pBuf);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Program Execute
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdProgramExe(u32 u32Page)
{
    bool bRst;
    i32  i32Timeout = 0;
    unFlashDrvCmd_t unCmd;
    unFlashDrvRegData_t unData;
    
    
    if(sFlashDrvCmdSetWrEnable() == true)
    {
        sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
        
        memset(&unCmd, 0, sizeof(unCmd));
        unCmd.stProgramExe.eCmd = eFlashDrvCmd10;
        unCmd.stProgramExe.u8Addr[0] = (u32Page >> 16) & 0xff;
        unCmd.stProgramExe.u8Addr[1] = (u32Page >>  8) & 0xff;
        unCmd.stProgramExe.u8Addr[2] = (u32Page >>  0) & 0xff;
        bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stProgramExe), unCmd.u8Byte);
        
        sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
        
        
        while((i32Timeout < 1000) && (bRst == true))
        {
            i32Timeout++;
            unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
            if(unData.stSts1.bOip == eFlashDrvRegOipReady)
            {
                return bRst;
            }
        }
    }
    
    bRst = false;
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Program Load Random Data
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
*                                           u16AddrColumn:列地址。12位地址能够寻址从0到4095字节；
*                                                         但是，只有0到2175/2111字节有效。
*                                                         每页的字节2176/2112到4095都是“越界”的，
*                                                         在设备中不存在，并且无法寻址。
***********************************************************************************************/
bool sFlashDrvCmdProgramLoadRandom(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf)
{
    bool bRst;
    unFlashDrvCmd_t unCmd;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stProgramLoadRandom.eCmd = eFlashDrvCmd84;
    unCmd.stProgramLoadRandom.u8AddrH = (u16AddrColumn >> 8) & 0xff;
    unCmd.stProgramLoadRandom.u8AddrL =  u16AddrColumn & 0xff;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stProgramLoadRandom), unCmd.u8Byte);
    
    bRst = bRst & sSpiDrvSend(stFlashDrvDev.eSpiNum, i32Len, pBuf);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    return bRst;
}








/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->Block Erase(128K)
* Author                                :   Hall
* modified Date                         :   2024-07-06
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdBlockErase(u32 u32Block)
{
    bool bRst;
    i32  i32Timeout = 0;
    unFlashDrvCmd_t unCmd;
    unFlashDrvRegData_t unData;
    
    
    if(sFlashDrvCmdSetWrEnable() == true)
    {
        //由于传进来的时Block号, 所以为了转换为每个Block的首地址, 这里得用Block号 * 每Block所含的Page数
        u32Block <<= 6;                     //u32Block = u32Block * 64
        
        sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
        
        memset(&unCmd, 0, sizeof(unCmd));
        unCmd.stBlockErase.eCmd = eFlashDrvCmdd8;
        unCmd.stBlockErase.u8Addr[0] = (u32Block >> 16) & 0xff;
        unCmd.stBlockErase.u8Addr[1] = (u32Block >>  8) & 0xff;
        unCmd.stBlockErase.u8Addr[2] = (u32Block >>  0) & 0xff;
        bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stBlockErase), unCmd.u8Byte);
        
        sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
        
        
        while((i32Timeout < 1000) && (bRst == true))
        {
            i32Timeout++;
            unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
            if(unData.stSts1.bOip == eFlashDrvRegOipReady)
            {
                return bRst;
            }
        }
    }
    
    bRst = false;
    
    return bRst;
}












/**********************************************************************************************
* Description                           :   nand flash芯片基本操作命令--->复位
* Author                                :   Hall
* modified Date                         :   2024-07-05
* notice                                :   
***********************************************************************************************/
bool sFlashDrvCmdReset(void)
{
    bool bRst;
    i32  i32Timeout = 0;
    unFlashDrvCmd_t unCmd;
    unFlashDrvRegData_t unData;
    
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, false);
    
    memset(&unCmd, 0, sizeof(unCmd));
    unCmd.stReset.eCmd = eFlashDrvCmdff;
    bRst = sSpiDrvSend(stFlashDrvDev.eSpiNum, sizeof(unCmd.stReset), unCmd.u8Byte);
    
    sSpiDrvSetCs(stFlashDrvDev.eSpiNum, true);
    
    
    while((i32Timeout < 1000) && (bRst == true))
    {
        i32Timeout++;
        unData.u8Data = sFlashDrvCmdGetFt(eFlashDrvRegStatus1);
        if(unData.stSts1.bOip == eFlashDrvRegOipReady)
        {
            return bRst;
        }
    }
    
    bRst = false;
    
    return bRst;
}






























