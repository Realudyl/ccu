/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   flash_drv_gd5f4gm8.h
* Description                           :   GD5F4GM8 SPI NAND FLASH芯片驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-05
* notice                                :   
****************************************************************************************************/
#ifndef _flash_drv_gd5f4gm8_H_
#define _flash_drv_gd5f4gm8_H_
#include "flash_drv_gd5f4gm8_bbm.h"
#include "flash_drv_gd5f4gm8_data_def.h"












//flash芯片 设备
typedef struct
{
    bool                                eInitState;                             //flash初始化状态：true 成功 ; false 失败
    eSpiNum_t                           eSpiNum;                                //flash芯片设备的SPI端口号码
    stSpiDrvCfg_t                       stSpiCfg;                               //flash芯片设备的SPI端口配置
    
    u32                                 u32BlockNum;                            //flash芯片设备block数
    u32                                 u32PageNum;                             //flash芯片设备每个block下的页数
    u32                                 u32PageSize;                            //flash芯片设备页大小
    
    SemaphoreHandle_t                   xSemDev;                                //flash芯片设备操作互斥锁
    
    stFlashDrvBbmCache_t                *pBbmCache;                             //flash芯片设备坏块管理缓存结构体
    
}stFlashDrvDevice_t;




















//nand flash芯片数据读/写函数
extern bool sFlashDrvRdData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
extern bool sFlashDrvWrData(u16 u16Block, u8 u8Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);

//nand flash芯片页数据转移函数
extern bool sFlashDrvMovePageData(u16 u16DesBlock, u16 u16SrcBloc, u8 u8Page);

//nand flash芯片页数据读/写函数
extern bool sFlashDrvRdPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
extern bool sFlashDrvWrPageData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);

//nand flash芯片Spare空间读/写数据函数
extern bool sFlashDrvRdSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
extern bool sFlashDrvWrSpareData(u32 u32Page, u16 u16AddrColumn, i32 i32Len, u8 *pBuf);



//nand flash芯片基本操作命令
extern bool sFlashDrvCmdSetWrEnable(void);
extern bool sFlashDrvCmdSetWrDisable(void);
extern u8   sFlashDrvCmdGetFt(eFlashDrvReg_t eReg);
extern bool sFlashDrvCmdSetFt(eFlashDrvReg_t eReg, u8 u8Data);
extern bool sFlashDrvCmdRdPage(u32 u32Page);
extern bool sFlashDrvCmdRdCache(u16 u16AddrColumn, i32 i32Len, u8 *pBuf);
extern bool sFlashDrvCmdRdId(u8 *pIdM, u8 *pIdD);
extern bool sFlashDrvCmdProgramLoad(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf);
extern bool sFlashDrvCmdProgramExe(u32 u32Page);
extern bool sFlashDrvCmdProgramLoadRandom(u16 u16AddrColumn, i32 i32Len, const u8 *pBuf);
extern bool sFlashDrvCmdBlockErase(u32 u32Block);
extern bool sFlashDrvCmdReset(void);






extern bool sFlashDrvInit(void);














#endif

