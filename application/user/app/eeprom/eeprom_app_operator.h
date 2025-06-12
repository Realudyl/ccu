/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_operator.h
* Description                           :   eeprom芯片用户程序实现 之 来自运营商的配置参数结构
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-25
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_operator_H_
#define _eeprom_app_operator_H_
#include "private_drv_opt.h"














//来自运营商的出厂参数结构定义
typedef struct
{
    u16                                 u16CheckCrc;                            //校验值
    unPrivDrvRate_t                     unRate;                                 //费率模型
    char                                u8Qrcode[ePileGunIndexNum][cPrivDrvLenParamQrcode];//二维码
    u32                                 u32EleLoss;                             //电损率
    u8                                  u8Rsvd[512];                            //预留字段
}__attribute__((aligned(2))) stEepromAppBlockOperatorInfo_t;

//u16CheckCrc 字段 size
#define cEepromAppBlockOperatorCrcSize  (sizeof(u16))













//电损补偿系数的基数
#define cEleLossDefault                 (1000)
































extern void sEepromAppSetBlockOperatorDefault(void);


extern bool sEepromAppGetBlockOperatorRate(unPrivDrvRate_t *pRate);
extern bool sEepromAppGetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32MaxLen, char *pCode);
extern u32  sEepromAppGetBlockOperatorEleLoss(void);

extern bool sEepromAppSetBlockOperatorRate(unPrivDrvRate_t *pRate);
extern bool sEepromAppSetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32Len, const char *pCode);
extern bool sEepromAppSetBlockOperatorEleLoss(u32 u32EleLoss);





#endif
















