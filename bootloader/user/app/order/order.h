/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   order.c
* Description                           :   充电订单计量计费管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
#ifndef _order_H_
#define _order_H_
#include "order_div.h"
#include "order_top_down_1.h"
#include "order_top_down_2.h"
#include "order_user.h"

#include "eeprom_app.h"
#include "meter_drv_opt.h"












//订单计量计费的阶段
typedef enum
{
    eOrderStepWait                      = 0,                                    //等待计量
    eOrderStepStart                     = 1,                                    //开始计量
    eOrderStepRun                       = 2,                                    //持续计量
    eOrderStepStop                      = 3,                                    //停止计量
    
    eOrderStepMax
}eOrderStep_t;









//充电订单计量计费管理组件缓存数据结构定义
typedef struct
{
    eOrderStep_t                        eStep[ePileGunIndexNum];                //计量阶段
    ePrivDrvRateModel_t                 eMode;                                  //费率模型类型
    unPrivDrvRate_t                     *pRate;                                 //计量对象订单的费率模型
    u32                                 u32EleLoss;                             //电损补偿系数
    u32                                 u32Stopreason[ePileGunIndexNum];        //停充原因
    i32                                 i32RcdIndex[ePileGunIndexNum];          //计量对象订单在EEPROM中的序号
    i32                                 (*pCbGetSoc)(ePileGunIndex_t eGunIndex);
    stEepromAppBlockChgRcd_t            *pRcd[ePileGunIndexNum];                //计量对象订单在ram中的结构体
    
    //缓存上面订单写eeprom用
    //上面的订单会被多处读写,直接用它写eeprom会造成校验和出错破坏订单
    //这个缓存不会被多线程使用 无需保护锁
    stEepromAppBlockChgRcd_t            stRcdTemp;                              //
    
    //计量资源
    u8                                  u8Section[ePileGunIndexNum];            //不合并的实际电量段数---------尖峰平谷1模型用
    u8                                  u8RateIndex[ePileGunIndexNum];          //当前时段所属的费率分段序号---平均分段 模型用
    u32                                 u32MeterStart[ePileGunIndexNum];        //当前时段开始时电表的读数
    u32                                 u32TimeWrEeprom[ePileGunIndexNum];      //上一次写入eeprom的时间戳
    
    //尖峰平谷1模型下 电量分段数达到最大值时
    stPrivDrvMoneyTopDownUnit           stUnitTemp[ePileGunIndexNum];           //用来累积 时段电量明细
    stPrivDrvMoneyTopDownUnit           stUnitTempOld[ePileGunIndexNum];        //该明细上一拍的值
    
    stPrivDrvMoneyTopDownUnit           stUnitSectionMax[ePileGunIndexNum];     //用累积后续所有分段的电量数据（含第 cOrderTopDown1TimeSection 段）
    
}stOrderCache_t;
























extern bool sOrderInit(void);
extern u32  sOrderProcCalcMoney(u32 u32Price, u32 u32Elect);
extern u32  sOrderProcCalcElectAdj(u32 u32Elect);

extern bool sOrderStop (ePileGunIndex_t eGunIndex, u32 u32Reason);
extern bool sOrderStart(ePileGunIndex_t eGunIndex, i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd, unPrivDrvRate_t *pRate, i32 (*pCbGetSoc)(ePileGunIndex_t eGunIndex));

extern eOrderStep_t sOrderGetStep(ePileGunIndex_t eGunIndex);
extern void sOrderSetEleLoss(u32 u32Coef);














#endif





























