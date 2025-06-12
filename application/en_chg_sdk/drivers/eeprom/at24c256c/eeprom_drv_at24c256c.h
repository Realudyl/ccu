/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     eeprom_drv_at24c256c.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-2-23
 * @Attention             :     
 * @Brief                 :     
 * 
 * @History:
 * 
 * 1.@Date: 2024-2-23
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _eeprom_drv_at24c256c_H
#define _eeprom_drv_at24c256c_H
#include "i2c_drv.h"

















//AT24C256C的存储空间大小
#define cEepromDrvAt24c256cSize         (32 * 1024)

//AT24C256C的写周期时间 ms
#define cEepromDrvAt24c256cTwr          (5)









//at24c256c 设备地址结构定义
typedef struct
{
    u8                                  bRw     : 1;                            //读写控制位
    u8                                  bA012   : 3;                            //器件地址
    u8                                  bFix    : 4;                            //固定值, 保持0xA
    
}__attribute__((packed)) stEepromDrvAt24c256cAddr_t;
typedef union
{
    stEepromDrvAt24c256cAddr_t          stAddr;                                 //结构体访问
    u8                                  u8Addr;                                 //8位访问
}__attribute__((packed)) unEepromDrvAt24c256cAddr_t;


//bRw 值定义
#define cEepromDrvAt24c256cAddrWr       (0)                                     //写
#define cEepromDrvAt24c256cAddrRd       (1)                                     //读


//bFix 值定义
#define cEepromDrvAt24c256cAddrFix      (0x0A)                                  //主机地址





//EEPROM 设备
typedef struct
{
    eI2cNum_t                           eI2cNum;                                //eeprom设备的I2C端口号码
    
    unEepromDrvAt24c256cAddr_t          unDevAddr;                              //eeprom设备地址
    u16                                 u16DevPageNum;                          //eeprom设备页数
    u8                                  u8DevPageSize;                          //eeprom设备页大小
    
    SemaphoreHandle_t                   xSemEeprom;                             //eeprom设备操作互斥锁
    
}stEepromDrvDevice_t;






















extern bool sEepromOpen(void);
extern bool sEepromErase(void);
extern bool sEepromRecv(u16 u16WordAddr, u32 u32Len, u16 *pBuf);
extern bool sEepromSend(u16 u16WordAddr, u32 u32Len, const u16 * data);















#endif



















