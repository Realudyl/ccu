/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app.c
* Description                           :   eeprom芯片用户程序实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
#include "eeprom_app.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_app";





stEepromAppCache_t stEepromAppCache;





stEepromAppBlock_t stEepromAppBlockMap[eEepromAppBlockMax] = 
{
    {eEepromAppBlockFactory,    cEepromAppBlockFactoryAddr,     cEepromAppBlockFactoryLenMax,   sizeof(stEepromAppBlockFactoryInfo_t),  true,   (u8 *) stEepromAppCache.stFactory,  "工  厂参数存储区"  },
    {eEepromAppBlockOperator,   cEepromAppBlockOperatorAddr,    cEepromAppBlockOperatorLenMax,  sizeof(stEepromAppBlockOperatorInfo_t), false,  (u8 *)&stEepromAppCache.stOperator, "运营商参数存储区"  },
    {eEepromAppBlockUser,       cEepromAppBlockUserAddr,        cEepromAppBlockUserLenMax,      sizeof(stEepromAppBlockUserInfo_t),     false,  (u8 *)&stEepromAppCache.stUser,     "用  户参数存储区"  },
    {eEepromAppBlockChgRcdHead, cEepromAppBlockChgRcdHeadAddr,  cEepromAppBlockChgRcdHeadLenMax,sizeof(stEepromAppBlockChgRcdHead_t),   false,  (u8 *)&stEepromAppCache.stRcdHead,  "充电记录头存储区"  },
    {eEepromAppBlockChgRcd,     cEepromAppBlockChgRcdAddr,      cEepromAppBlockChgRcdLenMax,    cEepromAppBlockChgRcdLen,               false,  NULL,                               "充电记录  存储区"  },
    
};







//基础函数
bool sEepromAppInit(void (*pCbDefaultF)(void));
bool sEepromAppBlockSizeCheck(void);
void sEepromAppBlockCrcCalc(eEepromAppBlock_t eBlock);
bool sEepromAppBlockCrcCheck(eEepromAppBlock_t eBlock);

//block 擦除/读/写
bool sEepromAppRstBlock(void);
bool sEepromAppGetBlock(eEepromAppBlock_t eBlock);
bool sEepromAppSetBlock(eEepromAppBlock_t eBlock);

//block of rcd 读写---rcd 读写操作有特殊 一次只读写一条充电记录，按位置序号
i32  sEepromAppGetBlockRcd(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd);
bool sEepromAppSetBlockRcd(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd);













/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->初始化
* Author                                :   Hall
* modified Date                         :   2024-04-23
* notice                                :   
***********************************************************************************************/
bool sEepromAppInit(void (*pCbDefaultF)(void))
{
    bool bRst = true;
    eEepromAppBlock_t eBlock;
    
    //eeprom map size分配检查
    if(sEepromAppBlockSizeCheck() == false)
    {
        EN_SLOGE(TAG, "eeprom芯片用户程序初始化失败, block size超限");
        return(false);
    }
    
    
    //cache 初始化
    memset(&stEepromAppCache, 0, sizeof(stEepromAppCache));
    stEepromAppCache.pCbDefaultFactory = pCbDefaultF;
    for(eBlock = eEepromAppBlockFactory; eBlock < eEepromAppBlockMax; eBlock++)
    {
        stEepromAppCache.xSemBlock[eBlock] = xSemaphoreCreateBinary();
        xSemaphoreGive(stEepromAppCache.xSemBlock[eBlock]);
    }
    
    
    //上电首次读取eeprom内容---block eEepromAppBlockChgRcd 除外
    bRst = bRst & sEepromOpen();
    bRst = bRst & sEepromAppGetBlock(eEepromAppBlockFactory   );
    bRst = bRst & sEepromAppGetBlock(eEepromAppBlockOperator  );
    bRst = bRst & sEepromAppGetBlock(eEepromAppBlockUser      );
    bRst = bRst & sEepromAppGetBlock(eEepromAppBlockChgRcdHead);
    
    
    bRst = bRst & sEepromAppShellRegister();
    
    return bRst;
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->block size 检查
* Author                                :   Hall
* modified Date                         :   2024-04-26
* notice                                :   检查配置Map中有没有超限的 block
***********************************************************************************************/
bool sEepromAppBlockSizeCheck(void)
{
    u32 u32Len;
    eEepromAppBlock_t eBlock;
    stEepromAppBlock_t *pBlock = NULL;
    
    for(eBlock = eEepromAppBlockFactory; eBlock < eEepromAppBlockMax; eBlock++)
    {
        //带备份标志的 block 实际存储大小翻倍---有备份
        pBlock = &stEepromAppBlockMap[eBlock];
        u32Len = (pBlock->bBackup == false) ? pBlock->u32Len : (pBlock->u32Len * 2);
        EN_SLOGI(TAG, "eeprom芯片用户程序 Block Size 检查, 分区:%s, 分配Size:%05d字节, 实际Size:%05d字节", pBlock->pString, pBlock->u32LenMax, u32Len);
        if(u32Len > pBlock->u32LenMax)
        {
            EN_SLOGE(TAG, "Block Size 错误, 实际值超出分配值");
            return(false);
        }
    }
    
    return(true);
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->指定 block 的crc校验值计算
* Author                                :   Hall
* modified Date                         :   2024-05-06
* notice                                :   
***********************************************************************************************/
void sEepromAppBlockCrcCalc(eEepromAppBlock_t eBlock)
{
    stEepromAppBlockFactoryInfo_t   *pFactory   = NULL;
    stEepromAppBlockOperatorInfo_t  *pOperator  = NULL;
    stEepromAppBlockUserInfo_t      *pUser      = NULL;
    stEepromAppBlockChgRcdHead_t    *pRcdHead   = NULL;
    
    switch(eBlock)
    {
    case eEepromAppBlockFactory:
        pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
        pFactory->u16CheckCrc = sCrc16Modbus(((const u8 *)pFactory) + cEepromAppBlockFactoryCrcSize, sizeof(stEepromAppBlockFactoryInfo_t) - cEepromAppBlockFactoryCrcSize);
        
        //写入操作时 备份区内容从存储区直接拷贝
        memset(&stEepromAppCache.stFactory[eEepromAppBlockFactory1], 0, sizeof(stEepromAppBlockFactoryInfo_t));
        memcpy(&stEepromAppCache.stFactory[eEepromAppBlockFactory1], &stEepromAppCache.stFactory[eEepromAppBlockFactory0], sizeof(stEepromAppBlockFactoryInfo_t));
        break;
    case eEepromAppBlockOperator:
        pOperator = &stEepromAppCache.stOperator;
        pOperator->u16CheckCrc = sCrc16Modbus(((const u8 *)pOperator) + cEepromAppBlockOperatorCrcSize, sizeof(stEepromAppBlockOperatorInfo_t) - cEepromAppBlockOperatorCrcSize);
        break;
    case eEepromAppBlockUser:
        pUser = &stEepromAppCache.stUser;
        pUser->u16CheckCrc = sCrc16Modbus(((const u8 *)pUser) + cEepromAppBlockUserCrcSize, sizeof(stEepromAppBlockUserInfo_t) - cEepromAppBlockUserCrcSize);
        break;
    case eEepromAppBlockChgRcdHead:
        pRcdHead = &stEepromAppCache.stRcdHead;
        pRcdHead->u16CheckCrc = sCrc16Modbus(((const u8 *)pRcdHead) + cEepromAppBlockRcdHeadCrcSize, sizeof(stEepromAppBlockChgRcdHead_t) - cEepromAppBlockRcdHeadCrcSize);
        break;
    default:
        break;
    }
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->指定 block 的crc校验
* Author                                :   Hall
* modified Date                         :   2024-05-06
* notice                                :   
***********************************************************************************************/
bool sEepromAppBlockCrcCheck(eEepromAppBlock_t eBlock)
{
    u16 u16CrcCalc;
    stEepromAppBlockFactoryInfo_t   *pFactory   = NULL;
    stEepromAppBlockOperatorInfo_t  *pOperator  = NULL;
    stEepromAppBlockUserInfo_t      *pUser      = NULL;
    stEepromAppBlockChgRcdHead_t    *pRcdHead   = NULL;
    
    switch(eBlock)
    {
    case eEepromAppBlockFactory:
        pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
        u16CrcCalc = sCrc16Modbus(((const u8 *)pFactory) + cEepromAppBlockFactoryCrcSize, sizeof(stEepromAppBlockFactoryInfo_t) - cEepromAppBlockFactoryCrcSize);
        if(u16CrcCalc != pFactory->u16CheckCrc)
        {
            //存储区校验值失效 则检查备份区
            pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory1];
            u16CrcCalc = sCrc16Modbus(((const u8 *)pFactory) + cEepromAppBlockFactoryCrcSize, sizeof(stEepromAppBlockFactoryInfo_t) - cEepromAppBlockFactoryCrcSize);
            if(u16CrcCalc == pFactory->u16CheckCrc)
            {
                //备份区OK时 需要覆盖存储区
                memset(&stEepromAppCache.stFactory[eEepromAppBlockFactory0], 0, sizeof(stEepromAppBlockFactoryInfo_t));
                memcpy(&stEepromAppCache.stFactory[eEepromAppBlockFactory0], &stEepromAppCache.stFactory[eEepromAppBlockFactory1], sizeof(stEepromAppBlockFactoryInfo_t));
                sEepromAppSetBlock(eBlock);
            }
            else if(stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16CheckCrc == 0xffff)
            {
                EN_SLOGI(TAG, "eeprom芯片用户程序初始化, 工  厂参数存储区及备份区未初始化, 执行初始化操作");
                memset(&stEepromAppCache.stFactory[eEepromAppBlockFactory0], 0, sizeof(stEepromAppBlockFactoryInfo_t));
                sEepromAppSetBlockFactoryDefault();
                sEepromAppSetBlock(eBlock);
            }
            else
            {
                EN_SLOGE(TAG, "eeprom芯片用户程序初始化, 工  厂参数存储区及备份区校验值均不正确, 恢复默认值");
                memset(&stEepromAppCache.stFactory[eEepromAppBlockFactory0], 0, sizeof(stEepromAppBlockFactoryInfo_t));
                sEepromAppSetBlockFactoryDefault();
                sEepromAppSetBlock(eBlock);
            }
        }
        break;
    case eEepromAppBlockOperator:
        pOperator = &stEepromAppCache.stOperator;
        u16CrcCalc = sCrc16Modbus(((const u8 *)pOperator) + cEepromAppBlockOperatorCrcSize, sizeof(stEepromAppBlockOperatorInfo_t) - cEepromAppBlockOperatorCrcSize);
        if(u16CrcCalc != pOperator->u16CheckCrc)
        {
            if(pOperator->u16CheckCrc == 0xffff)
            {
                EN_SLOGI(TAG, "eeprom芯片用户程序初始化, 运营商参数存储区未初始化, 执行初始化操作");
                memset(&stEepromAppCache.stOperator, 0, sizeof(stEepromAppBlockOperatorInfo_t));
                sEepromAppSetBlockOperatorDefault();
                sEepromAppSetBlock(eBlock);
            }
            else
            {
                EN_SLOGE(TAG, "eeprom芯片用户程序初始化, 运营商参数存储区校验值不正确");
                return(false);
            }
        }
        break;
    case eEepromAppBlockUser:
        pUser = &stEepromAppCache.stUser;
        u16CrcCalc = sCrc16Modbus(((const u8 *)pUser) + cEepromAppBlockUserCrcSize, sizeof(stEepromAppBlockUserInfo_t) - cEepromAppBlockUserCrcSize);
        if(u16CrcCalc != pUser->u16CheckCrc)
        {
            if(pUser->u16CheckCrc == 0xffff)
            {
                EN_SLOGI(TAG, "eeprom芯片用户程序初始化, 用  户参数存储区未初始化, 执行初始化操作");
                memset(&stEepromAppCache.stUser, 0, sizeof(stEepromAppBlockUserInfo_t));
                sEepromAppSetBlock(eBlock);
            }
            else
            {
                EN_SLOGE(TAG, "eeprom芯片用户程序初始化, 用  户参数存储区校验值不正确");
                return(false);
            }
        }
        break;
    case eEepromAppBlockChgRcdHead:
        pRcdHead = &stEepromAppCache.stRcdHead;
        u16CrcCalc = sCrc16Modbus(((const u8 *)pRcdHead) + cEepromAppBlockRcdHeadCrcSize, sizeof(stEepromAppBlockChgRcdHead_t) - cEepromAppBlockRcdHeadCrcSize);
        if(u16CrcCalc != pRcdHead->u16CheckCrc)
        {
            if(pRcdHead->u16CheckCrc == 0xffff)
            {
                EN_SLOGI(TAG, "eeprom芯片用户程序初始化, 充电记录头存储区未初始化, 执行初始化操作");
                memset(&stEepromAppCache.stRcdHead, 0, sizeof(stEepromAppBlockChgRcdHead_t));
                sEepromAppSetBlock(eBlock);
            }
            else
            {
                EN_SLOGE(TAG, "eeprom芯片用户程序初始化, 充电记录头存储区校验值不正确");
                return(false);
            }
        }
        break;
    default:
        break;
    }
    
    return true;
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->读指定的 block
* Author                                :   Hall
* modified Date                         :   2024-05-08
* notice                                :   调用本函数 将整个eeprom擦除---即全部内容恢复 0xff 默认值
***********************************************************************************************/
bool sEepromAppRstBlock(void)
{
    return sEepromErase();
}









/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->读指定的 block
* Author                                :   Hall
* modified Date                         :   2024-04-26
* notice                                :   调用本函数 将指定的 block 数据读取存放到对应内存位置
*                                           同时还会执行crc值校验并返回结果
***********************************************************************************************/
bool sEepromAppGetBlock(eEepromAppBlock_t eBlock)
{
    bool bRst;
    u32  u32Len;
    
    //根据是否有备份存储 取计算实际存储大小
    u32Len = stEepromAppBlockMap[eBlock].u32Len;
    u32Len = (stEepromAppBlockMap[eBlock].bBackup == false) ? u32Len : (u32Len * 2);
    
    //读取及校验
    bRst = sEepromRecv(stEepromAppBlockMap[eBlock].u32Addr, u32Len / sizeof(u16), (u16 *)stEepromAppBlockMap[eBlock].pBuf);
    bRst = (bRst == true) ? sEepromAppBlockCrcCheck(eBlock) : false;
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->写指定的 block
* Author                                :   Hall
* modified Date                         :   2024-04-26
* notice                                :   调用本函数 将对应内存位置的数据写入到指定的 block
*                                           写入之前需要先更新校验值
*
*                                           写入过程不允许外部修改内容，因此需要信号量锁定
***********************************************************************************************/
bool sEepromAppSetBlock(eEepromAppBlock_t eBlock)
{
    bool bRst;
    u32  u32Len;
    
    //根据是否有备份存储 取计算实际存储大小
    u32Len = stEepromAppBlockMap[eBlock].u32Len;
    u32Len = (stEepromAppBlockMap[eBlock].bBackup == false) ? u32Len : (u32Len * 2);
    
    //锁定资源, 更新校验值并写入
    xSemaphoreTake(stEepromAppCache.xSemBlock[eBlock], portMAX_DELAY);
    sEepromAppBlockCrcCalc(eBlock);
    bRst = sEepromSend(stEepromAppBlockMap[eBlock].u32Addr, u32Len / sizeof(u16), (const u16 *)stEepromAppBlockMap[eBlock].pBuf);
    xSemaphoreGive(stEepromAppCache.xSemBlock[eBlock]);
    
    return(bRst);
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->从充电记录存储区读取一条 rcd 
* Author                                :   Hall
* modified Date                         :   2024-04-26
* notice                                :   i32RcdIndex :读取目标充电记录的序号
*                                           pRcd        :读取到的充电记录数据的存放地址
***********************************************************************************************/
i32 sEepromAppGetBlockRcd(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd)
{
    u32  u32WordAddr;
    
    //1:参数限制
    if((i32RcdIndex <  0)
    || (i32RcdIndex >= cEepromAppBlockChgRcdNumMax)
    || (i32RcdIndex >= stEepromAppCache.stRcdHead.u16Total))
    {
        EN_SLOGE(TAG, "读充电记录序号%d溢出, 调整为读取第0条", i32RcdIndex);
        i32RcdIndex = 0;
    }
    
    //2:计算偏移的 word address
    u32WordAddr = stEepromAppBlockMap[eEepromAppBlockChgRcd].u32Addr + (i32RcdIndex * sizeof(stEepromAppBlockChgRcd_t));
    
    
    //3:读取
    if(sEepromRecv(u32WordAddr, sizeof(stEepromAppBlockChgRcd_t) / sizeof(u16), (u16 *)pRcd) != true)
    {
        i32RcdIndex = -1;
    }
    
    return i32RcdIndex;
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->向充电记录存储区写入一条 rcd 
* Author                                :   Hall
* modified Date                         :   2024-05-06
* notice                                :   i32RcdIndex :写入目标充电记录的序号
*                                           pRcd        :传入的充电记录数据地址
***********************************************************************************************/
bool sEepromAppSetBlockRcd(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd)
{
    bool bRst;
    u32 u32WordAddr;
    
    //1:参数限制
    if((i32RcdIndex <  0)
    || (i32RcdIndex >= cEepromAppBlockChgRcdNumMax)
    || (i32RcdIndex >  stEepromAppCache.stRcdHead.u16Total))
    {
        EN_SLOGE(TAG, "写充电记录出错, 目标充电记录序号超范围:%d, 当前总条数:%d", i32RcdIndex, stEepromAppCache.stRcdHead.u16Total);
        return(false);
    }
    
    
    //3:计算偏移的 word address
    u32WordAddr = stEepromAppBlockMap[eEepromAppBlockChgRcd].u32Addr + (i32RcdIndex * sizeof(stEepromAppBlockChgRcd_t));
    
    
    //4:写入---加锁避免多线程同时执行
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcd], portMAX_DELAY);
    bRst = sEepromSend(u32WordAddr, sizeof(stEepromAppBlockChgRcd_t) / sizeof(u16), (const u16 *)pRcd);
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcd]);
    
    return(bRst);
}






















