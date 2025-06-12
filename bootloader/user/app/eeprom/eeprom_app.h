/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app.h
* Description                           :   eeprom芯片用户程序实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_H_
#define _eeprom_app_H_
#include "eeprom_drv_at24c256c.h"

#include "eeprom_app_factory.h"
#include "eeprom_app_operator.h"
#include "eeprom_app_user.h"
#include "eeprom_app_chg_rcd.h"
#include "eeprom_app_shell.h"






//EEPROM 存储器内 存储块(block) 的类型
typedef enum
{
    eEepromAppBlockFactory              = 0,                                    //来自工  厂的配置参数区
    eEepromAppBlockOperator             = 1,                                    //来自运营商的配置参数区
    eEepromAppBlockUser                 = 2,                                    //来自用  户的配置参数区
    eEepromAppBlockChgRcdHead           = 3,                                    //充电记录head    存储区
    eEepromAppBlockChgRcd               = 4,                                    //充电记录        存储区
    
    eEepromAppBlockMax
}eEepromAppBlock_t;




//EEPROM 存储器内 存储块(block) 的特征信息结构定义
typedef struct
{
    eEepromAppBlock_t                   eBlock;                                 //存储块的类型
    u32                                 u32Addr;                                //        起始地址
    u32                                 u32LenMax;                              //        分配的最大长度
    u32                                 u32Len;                                 //        实际长度
    bool                                bBackup;                                //        备份标志
    u8                                  *pBuf;                                  //        对应的内存地址
    char                                *pString;                               //        描述字符串
}stEepromAppBlock_t;









//工厂配置参数块的属性--->存储起始地址 & 最大存储空间
#define cEepromAppBlockFactoryAddr      (0)
#define cEepromAppBlockFactoryLenMax    (2048)

//运营商配置参数块的属性--->存储起始地址 & 最大存储空间
#define cEepromAppBlockOperatorAddr     (cEepromAppBlockFactoryAddr + cEepromAppBlockFactoryLenMax)
#define cEepromAppBlockOperatorLenMax   (2048)

//用户配置参数块的属性--->存储起始地址 & 最大存储空间
#define cEepromAppBlockUserAddr         (cEepromAppBlockOperatorAddr + cEepromAppBlockOperatorLenMax)
#define cEepromAppBlockUserLenMax       (2048)

//充电记录head存储区的属性--->存储起始地址 & 最大存储空间
#define cEepromAppBlockChgRcdHeadAddr   (cEepromAppBlockUserAddr + cEepromAppBlockUserLenMax)
#define cEepromAppBlockChgRcdHeadLenMax (64)

//充电记录存储区的属性--->存储起始地址 & 实际存储空间 & 最大存储空间
#define cEepromAppBlockChgRcdAddr       (cEepromAppBlockChgRcdHeadAddr + cEepromAppBlockChgRcdHeadLenMax)
#define cEepromAppBlockChgRcdLen        (cEepromAppBlockChgRcdNumMax   * sizeof(stEepromAppBlockChgRcd_t))
#define cEepromAppBlockChgRcdLenMax     (cEepromDrvAt24c256cSize       - cEepromAppBlockChgRcdAddr)


















//EEPROM 用户程序缓存数据结构定义
typedef struct
{
    stEepromAppBlockFactoryInfo_t       stFactory[eEepromAppBlockFactoryMax];   //工  厂参数----有备份存储，所以是结构体数组
    stEepromAppBlockOperatorInfo_t      stOperator;                             //运营商参数
    stEepromAppBlockUserInfo_t          stUser;                                 //用  户参数
    stEepromAppBlockChgRcdHead_t        stRcdHead;                              //充电记录head
    
    
    void                                (*pCbDefaultFactory)(void);             //工  厂参数区初始化的回调函数
    
    //<block 写入操作> 与 <block 在内存中的缓存值修改操作> 需要互斥
    //如果同时执行 会造成实际校验值与存储校验值不匹配的问题 导致数据损坏
    SemaphoreHandle_t                   xSemBlock[eEepromAppBlockMax];          //block 写保护信号量
}__attribute__((aligned(2))) stEepromAppCache_t;































//基础函数
extern bool sEepromAppInit(void (*pCbDefaultF)(void));

//block 擦除/读/写
extern bool sEepromAppRstBlock(void);
extern bool sEepromAppGetBlock(eEepromAppBlock_t eBlock);
extern bool sEepromAppSetBlock(eEepromAppBlock_t eBlock);

//block of rcd 读写---rcd 读写操作有特殊 一次只读写一条充电记录，按位置序号
extern i32  sEepromAppGetBlockRcd(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd);
extern bool sEepromAppSetBlockRcd(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd);









#endif
















