/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_chg_rcd.h
* Description                           :   eeprom芯片用户程序实现 之 充电记录存储
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-25
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_chg_rcd_H_
#define _eeprom_app_chg_rcd_H_
#include "private_drv_opt.h"








//定义eeprom中充电记录存储的最大条数
#define cEepromAppBlockChgRcdNumMax     (32)





//充电记录head结构定义
typedef struct
{
    u16                                 u16CheckCrc;                            //校验值
    u16                                 u16Total;                               //总条数
    u16                                 u16WriteIndex;                          //当前写的第几条
    u16                                 u16ReadIndex;                           //当前应该读取的位置
    u16                                 u16Rsvd[8];                             //保留字段
}__attribute__((aligned(2))) stEepromAppBlockChgRcdHead_t;

//u16CheckCrc 字段 size
#define cEepromAppBlockRcdHeadCrcSize   (sizeof(u16))




















//充电记录存储结构定义---充电记录数据部分搭配用于管理的附加字段
typedef struct
{
    u16                                 u16CheckCrc;                            //        校验值
    bool                                bRptFlag;                               //本记录上报标志
    bool                                bChargingFlag;                          //本记录是否正在充电中
    stPrivDrvCmd09_t                    stRcdData;                              //        充电记录数据部分
    
}__attribute__((aligned(2))) stEepromAppBlockChgRcd_t;

//u16CheckCrc 字段 size
#define cEepromAppBlockRcdCrcSize       (sizeof(u16))




























//基于内容(xxx Id)去查找一条充电记录并返回其序号
extern i32  sEepromAppGetBlockChgRcdById(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd,
                        bool (*pCbCheck)(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd));



//充电记录head管理api
//Upd 上报充电记录成功后head需要更新
//Get 读取充电记录head
extern void sEepromAppUpdBlockChgRcdHead(stEepromAppBlockChgRcd_t *pRcd);
extern bool sEepromAppGetBlockChgRcdHead(bool bReadFlag, stEepromAppBlockChgRcdHead_t **pHead);

//充电记录管理api
//New 新建一条充电记录并返回其序号:i32RcdIndex
//Get/Set 基于该序号读写充电记录
extern i32  sEepromAppNewBlockChgRcd(stEepromAppBlockChgRcd_t *pRcd);
extern i32  sEepromAppGetBlockChgRcdByIndex(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd);
extern bool sEepromAppSetBlockChgRcdByIndex(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd);









#endif
















