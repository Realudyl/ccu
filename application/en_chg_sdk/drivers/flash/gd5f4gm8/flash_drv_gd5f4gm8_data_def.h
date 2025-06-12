/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   flash_drv_gd5f4gm8_data_def.h
* Description                           :   GD5F4GM8 SPI NAND FLASH芯片驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-05
* notice                                :   
****************************************************************************************************/
#ifndef _flash_drv_gd5f4gm8_data_def_H_
#define _flash_drv_gd5f4gm8_data_def_H_
#include "spi_drv_trans.h"








//根据<GD5F4GM8UE-Rev1.2.pdf> —— 3.MEMORY MAPPING & 4.ARRAY ORGANIZATION 定义
#define cGd5f4gm8BlockNum               4096                                    //FLASH 设备块总数
#define cGd5f4gm8PerBlockPageNum        64                                      //每块可操作页数
#define cGd5f4gm8PerPageUserByteNum     2048                                    //每页用户区域可读写字节数
#define cGd5f4gm8PageSpareAreaByteNum   64                                      //每页备用区域可读写字节数(上电默认 Internal ECC = ON)
#define cGd5f4gm8PerPageTotalByteNum    (cGd5f4gm8PerPageUserByteNum + cGd5f4gm8PageSpareAreaByteNum)
#define cGd5f4gm8TotalPageNum           (cGd5f4gm8BlockNum * cGd5f4gm8PerBlockPageNum)





#define cBlockNumForUserData            3800
#define cBlockNumForReplaceBlock        196
#define cBlockNumForTable               (cGd5f4gm8BlockNum - cBlockNumForUserData - cBlockNumForReplaceBlock)

#define cDbtSize                        200
#define cRbtSize                        cBlockNumForUserData

#define cDbtBLockNum                    2
#define cRbtBLockNum                    2




#define cUserAreaStart                  0                                                           //0
#define cUserAreaEnd                    (cUserAreaStart + cBlockNumForUserData - 1)                 //3799
#define cReplaceBlockAreaStart          (cUserAreaEnd + 1)                                          //3800
#define cReplaceBlockAreaEnd            (cReplaceBlockAreaStart + cBlockNumForReplaceBlock - 1)     //3995
#define cTableAreaStart                 (cReplaceBlockAreaEnd + 1)                                  //3996
#define cTableAreaEnd                   (cTableAreaStart + cBlockNumForTable - 1)                   //4095













//flash芯片命令集定义---<GD5F4GM8UE-Rev1.2.pdf>@Table 6-1. Commands Set
typedef enum
{
    eFlashDrvCmd06                      = 0x06,                                 //Write Enable
    eFlashDrvCmd04                      = 0x04,                                 //Write Disable
    
    eFlashDrvCmd0f                      = 0x0f,                                 //Get Features
    eFlashDrvCmd1f                      = 0x1f,                                 //Set Feature
    
    eFlashDrvCmd13                      = 0x13,                                 //Page Read (to cache)
    eFlashDrvCmd03                      = 0x03,                                 //Read From Cache (03H/0BH(9))
    eFlashDrvCmd0b                      = 0x0b,                                 //
    eFlashDrvCmd3b                      = 0x3b,                                 //Read From Cache x 2
    eFlashDrvCmd6b                      = 0x6b,                                 //Read From Cache x 4
    eFlashDrvCmdbb                      = 0xbb,                                 //Read From Cache Dual IO
    eFlashDrvCmdeb                      = 0xeb,                                 //Read From Cache Quad IO
    eFlashDrvCmdee                      = 0xee,                                 //Read From Cache Quad I/O DTR
    
    eFlashDrvCmd9f                      = 0x9f,                                 //Read ID
    //Read parameter page(8)                                                    //这俩也是0x13,就不重复定义了
    //Read UID(8)
    
    
    eFlashDrvCmd02                      = 0x02,                                 //Program Load
    eFlashDrvCmd32                      = 0x32,                                 //Program Load x4
    eFlashDrvCmd10                      = 0x10,                                 //Program Execute
    eFlashDrvCmd84                      = 0x84,                                 //Program Load Random Data
    eFlashDrvCmd34                      = 0x34,                                 //Program Load Random Data x4 (C4H/34H)
    eFlashDrvCmdc4                      = 0xc4,                                 //
    
    eFlashDrvCmdd8                      = 0xd8,                                 //Block Erase(128K)
    
    eFlashDrvCmdff                      = 0xff,                                 //Reset
    
    eFlashDrvCmd66                      = 0x66,                                 //Enable Power on Reset
    eFlashDrvCmd99                      = 0x99,                                 //Power on Reset
    eFlashDrvCmdb9                      = 0xb9,                                 //Deep Power Down(1.8V only)
    eFlashDrvCmdab                      = 0xab,                                 //ReleaseDeep Power Down(1.8V only)
    
    //eFlashDrvCmdMax,
}__attribute__((packed)) eFlashDrvCmd_t;





//dummy byte
typedef enum
{
    eFlashDrvDummyByte                  = 0xff,
    
}__attribute__((packed)) eFlashDrvDummyByte_t;





//flash芯片Feature命令寄存器定义---<GD5F4GM8UE-Rev1.2.pdf>@Table 12-1. Features Settings
typedef enum
{
    eFlashDrvRegProtection              = 0xa0,                                 //Protection
    eFlashDrvRegFeature1                = 0xb0,                                 //Feature1
    eFlashDrvRegStatus1                 = 0xc0,                                 //Status1
    eFlashDrvRegFeature2                = 0xd0,                                 //Feature2
    eFlashDrvRegStatus2                 = 0xf0,                                 //Status2
    
    eFlashDrvRegMax,
}__attribute__((packed)) eFlashDrvReg_t;




//flash芯片Feature命令寄存器值定义--->OIP(Operation In Progress)
//<GD5F4GM8UE-Rev1.2.pdf>@Table 12-2. Status Register Bit Descriptions
typedef enum
{
    eFlashDrvRegOipReady                = 0,                                    //设备就绪
    eFlashDrvRegOipBusy                 = 1,                                    //设备繁忙
    
    //eFlashDrvRegOipMax,
}__attribute__((packed)) eFlashDrvRegOip_t;










//flash芯片特性寄存器值定义--->A0H--->Protection
typedef struct
{
    u8                                  bRsvd1  : 1;                            //保留
    u8                                  bCmp    : 1;                            //CMP
    u8                                  bInv    : 1;                            //INV
    u8                                  bBp     : 3;                            //BP0~2
    u8                                  bRsvd2  : 1;                            //保留
    u8                                  bBrwd   : 1;                            //BRWD
    
}__attribute__((packed)) stFlashDrvRegPt_t;



//flash芯片特性寄存器值定义--->B0H--->Feature
typedef struct
{
    u8                                  bQe     : 1;                            //QE
    u8                                  bRsvd1  : 2;                            //保留
    u8                                  bBpl    : 1;                            //BPL
    u8                                  bEccEn  : 1;                            //ECC_EN
    u8                                  bRsvd2  : 1;                            //保留
    u8                                  bQtpEn  : 1;                            //OTP_EN
    u8                                  bQtpPrt : 1;                            //OTP_PRT
    
}__attribute__((packed)) stFlashDrvRegFt1_t;



//flash芯片特性寄存器值定义--->C0H--->Status
typedef struct
{
    eFlashDrvRegOip_t                   bOip    : 1;                            //OIP
    u8                                  bWel    : 1;                            //WEL
    u8                                  bFailE  : 1;                            //E_FAIL
    u8                                  bFailP  : 1;                            //P_FAIL
    u8                                  bEccs   : 2;                            //ECCS
    u8                                  bRsvd   : 2;                            //保留
    
}__attribute__((packed)) stFlashDrvRegSts1_t;



//flash芯片特性寄存器值定义--->D0H--->Feature
typedef struct
{

    u8                                  bRsvd1  : 5;                            //保留
    u8                                  bDsS0   : 1;                            //DS_S0
    u8                                  bDsS1   : 1;                            //DS_S1
    u8                                  bRsvd2  : 1;                            //保留
    
}__attribute__((packed)) stFlashDrvRegFt2_t;



//flash芯片特性寄存器值定义--->F0H--->Status
typedef struct
{
    u8                                  bRsvd1  : 3;                            //OIP
    u8                                  bBps    : 1;                            //BPS
    u8                                  bEccse  : 2;                            //ECCSE
    u8                                  bRsvd   : 2;                            //保留
    
}__attribute__((packed)) stFlashDrvRegSts2_t;



//flash芯片特性寄存器值 union
typedef union
{
    stFlashDrvRegPt_t                   stPt;
    stFlashDrvRegFt1_t                  stFt1;
    stFlashDrvRegFt2_t                  stFt2;
    stFlashDrvRegSts1_t                 stSts1;
    stFlashDrvRegSts2_t                 stSts2;
    
    u8                                  u8Data;
}__attribute__((packed)) unFlashDrvRegData_t;























//flash芯片命令--->附加数据长度为0的命令
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
}__attribute__((packed)) stFlashDrvCmdData0_t;


//flash芯片命令--->附加数据为16位地址的命令
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
    u8                                  u8AddrH;
    u8                                  u8AddrL;
}__attribute__((packed)) stFlashDrvCmdAddr16_t;


//flash芯片命令--->附加数据为24位地址的命令
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
    u8                                  u8Addr[3];
}__attribute__((packed)) stFlashDrvCmdAddr24_t;



//flash芯片命令--->Get Features
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
    eFlashDrvReg_t                      eReg;
}__attribute__((packed)) stFlashDrvCmdGetFt_t;


//flash芯片命令--->Set Feature
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
    eFlashDrvReg_t                      eReg;
    u8                                  u8Data;
}__attribute__((packed)) stFlashDrvCmdSetFt_t;


//flash芯片命令--->Read From Cache
typedef struct
{
    eFlashDrvCmd_t                      eCmd;
    u8                                  u8AddrH;
    u8                                  u8AddrL;
    u8                                  u8Dummy;
}__attribute__((packed)) stFlashDrvCmdRdCache_t;













//flash芯片命令最大长度
#define cFlashDrvCmdLenMax              (7)

//flash芯片命令 union
typedef union
{
    stFlashDrvCmdData0_t                stWrEn;
    stFlashDrvCmdData0_t                stWrDis;
    stFlashDrvCmdGetFt_t                stGetFt;
    stFlashDrvCmdSetFt_t                stSetFt;
    stFlashDrvCmdAddr24_t               stRdPage;
    stFlashDrvCmdRdCache_t              stRdCache;
    stFlashDrvCmdData0_t                stGetId;
    stFlashDrvCmdAddr16_t               stProgramLoad;
    stFlashDrvCmdAddr24_t               stProgramExe;
    stFlashDrvCmdAddr16_t               stProgramLoadRandom;
    stFlashDrvCmdAddr24_t               stBlockErase;
    
    stFlashDrvCmdData0_t                stReset;
    
    u8                                  u8Byte[cFlashDrvCmdLenMax];
}__attribute__((packed)) unFlashDrvCmd_t;









#endif

