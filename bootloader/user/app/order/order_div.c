/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   order_div.c
* Description                           :   充电订单计量计费管理 之 平均分段费率
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
#include "order.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "order_div";






extern stOrderCache_t stOrderCache;







void sOrderProcDivStart(ePileGunIndex_t eGunIndex);
void sOrderProcDivRun  (ePileGunIndex_t eGunIndex);
void sOrderProcDivStop (ePileGunIndex_t eGunIndex);




i32  sOrderProcDivGetSection(u8 u8Section);











/***************************************************************************************************
* Description                           :   充电订单计量计费管理 之 平均分段费率 之 start阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcDivStart(ePileGunIndex_t eGunIndex)
{
    u32 u32MeterStart = 0;
    stPrivDrvRateDiv_t  *pRate  = NULL;
    stPrivDrvMoneyDiv_t *pMoney = NULL;
    stEepromAppBlockChgRcd_t *pRcd = NULL;
    
    pRcd        =  stOrderCache.pRcd[eGunIndex];
    pMoney      = &pRcd->stRcdData.unMoney.stDiv;
    pRate       = &stOrderCache.pRate->stDiv;
    
    
    //1:充电记录部分字段赋初值
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterStart, NULL);
    pRcd->stRcdData.u32ChargingTime = 0;
    pRcd->stRcdData.stTimeStart     = sGetTime();
    pRcd->stRcdData.u32MeterStart   = u32MeterStart;
    pRcd->stRcdData.u32MeterStop    = u32MeterStart;
    pRcd->stRcdData.u8SocStart      = -1;                                       //先给负数无效值
    pRcd->stRcdData.u32UsedMoney    = 0;
    pRcd->stRcdData.u16MoneyDataLen = sizeof(stPrivDrvMoneyDiv_t);              //平均分段模型的电量明细数据长度固定不变
    pMoney->eMode = pRate->eMode;
    pMoney->eModeCover = pRate->eModeCover;
    pMoney->u32MoneyCoverFix = 0;
    pMoney->u8Section = pRate->u8Section;
    
    //2:计量资源初始化
    stOrderCache.u32TimeWrEeprom[eGunIndex] = sGetTimestamp();
    stOrderCache.u32MeterStart[eGunIndex] = u32MeterStart;
    stOrderCache.u8RateIndex[eGunIndex] = sOrderProcDivGetSection(pRate->u8Section);
}







/***************************************************************************************************
* Description                           :   充电订单计量计费管理 之 平均分段费率 之 run 阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcDivRun(ePileGunIndex_t eGunIndex)
{
    i32  i;
    bool bFix;
    i32  i32Soc;
    u32  u32Price;
    
    
    u8   u8RateIndex;
    u8   u8RateIndexNext;
    u32  u32MeterNow = 0;                                                       //当前电表读数
    
    stPrivDrvRateDiv_t  *pRate  = NULL;
    stPrivDrvMoneyDiv_t *pMoney = NULL;
    stEepromAppBlockChgRcd_t *pRcd = NULL;
    
    pRcd    =  stOrderCache.pRcd[eGunIndex];
    pMoney  = &pRcd->stRcdData.unMoney.stDiv;
    pRate   = &stOrderCache.pRate->stDiv;
    
    
    //电量计算---前者<后者 代表 u32MeterNow 的值异常了
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterNow, NULL);
    u8RateIndex     = stOrderCache.u8RateIndex[eGunIndex];                      //当前时段序号
    if(u32MeterNow >=  stOrderCache.u32MeterStart[eGunIndex])
    {
        //电损补偿 电损补偿系数默认值 = cEleLossDefault, 表示不补偿
        pMoney->u32Elect[u8RateIndex] = sOrderProcCalcElectAdj(u32MeterNow - stOrderCache.u32MeterStart[eGunIndex]);
    }
    else
    {
        //那就直接返回，不要执行电量累加 避免产生极大值（负数）
        return;
    }
    
    
    //分段电费
    pMoney->u32MoneyElect[u8RateIndex] = sOrderProcCalcMoney(pRate->u32RateElect[u8RateIndex], pMoney->u32Elect[u8RateIndex]);
    
    //分段服务费
    bFix = (pRate->eModeCover == ePrivDrvFeeModeFix);                           //选择服务费价格
    u32Price = (bFix) ? pRate->u32RateCoverFix : pRate->u32RateCover[u8RateIndex];
    pMoney->u32MoneyCover[u8RateIndex] = sOrderProcCalcMoney(u32Price, pMoney->u32Elect[u8RateIndex]);
    
    //总电费和总服务费
    pRcd->stRcdData.u32UsedMoney = 0;
    for(i = 0; i < cPrivDrvRateSection; i++)
    {
        pRcd->stRcdData.u32UsedMoney += pMoney->u32MoneyElect[i] + pMoney->u32MoneyCover[i];
        if(pRate->eModeCover == ePrivDrvFeeModeFix)
        {
            pMoney->u32MoneyCoverFix += pMoney->u32MoneyCover[i];
        }
    }
    
    //结束时间和结束电表读数更新为当前值---充电中实时数据上报需要用
    pRcd->stRcdData.stTimeStop = sGetTime();
    pRcd->stRcdData.u32ChargingTime = sGetTimeByStructTime(pRcd->stRcdData.stTimeStart, pRcd->stRcdData.stTimeStop);
    pRcd->stRcdData.u32MeterStop = u32MeterNow;
    
    i32Soc = stOrderCache.pCbGetSoc(eGunIndex);
    if(((i8)pRcd->stRcdData.u8SocStart) < 0)
    {
        //u8SocStart 只要有效 以后就不要再重复更新
        pRcd->stRcdData.u8SocStart = i32Soc;
    }
    if(i32Soc >= 0)
    {
        //只有当返回值为有效值时才更新---避免停充时被更新无效值
        pRcd->stRcdData.u8SocStop = i32Soc;
    }
    
    
    //费率的时段序号变化
    u8RateIndexNext = sOrderProcDivGetSection(pRate->u8Section);                //未来时段序号
    if(stOrderCache.u8RateIndex[eGunIndex] != u8RateIndexNext)
    {
        stOrderCache.u8RateIndex[eGunIndex] = u8RateIndexNext;
        stOrderCache.u32MeterStart[eGunIndex] = u32MeterNow;
    }
    
    //写eeprom
    //eeprom损坏的情况下， i32RcdIndex 是负数,
    //允许这种情况执行计量逻辑, 只是不存eeprom, 减小损坏等级
    if((stOrderCache.i32RcdIndex[eGunIndex] >= 0)
    && (sGetTimestamp() - stOrderCache.u32TimeWrEeprom[eGunIndex]) >= 120)
    {
        stOrderCache.u32TimeWrEeprom[eGunIndex] = sGetTimestamp();
        memset(&stOrderCache.stRcdTemp, 0, sizeof(stEepromAppBlockChgRcd_t));
        memcpy(&stOrderCache.stRcdTemp, pRcd, sizeof(stEepromAppBlockChgRcd_t));
        sEepromAppSetBlockChgRcdByIndex(stOrderCache.i32RcdIndex[eGunIndex], &stOrderCache.stRcdTemp);
    }
    
}







/***************************************************************************************************
* Description                           :   充电订单计量计费管理 之 平均分段费率 之 stop阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcDivStop(ePileGunIndex_t eGunIndex)
{
    u32  u32MeterStop = 0;
    
    stEepromAppBlockChgRcd_t *pRcd =  stOrderCache.pRcd[eGunIndex];
    
    vTaskDelay(2000 / portTICK_RATE_MS);                                        //等待2s 等待电表更新最终计量数据
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterStop, NULL);
    sOrderProcDivRun(eGunIndex);                                                //最后更新一次电量明细数据
    pRcd->stRcdData.eStopReason = stOrderCache.u32Stopreason[eGunIndex];        //停充原因
    pRcd->stRcdData.stTimeStop = sGetTime();                                    //取停充时间和电表读数
    pRcd->stRcdData.u32MeterStop = u32MeterStop;
    if(((i8)pRcd->stRcdData.u8SocStart) < 0)
    {
        //仍然为无效值 代表此次充电未能进入充电态 给0
        pRcd->stRcdData.u8SocStart = 0;
    }
    if(((i8)pRcd->stRcdData.u8SocStop) < 0)
    {
        //仍然为无效值 代表此次充电未能进入充电态 给0
        pRcd->stRcdData.u8SocStop = 0;
    }
    pRcd->bChargingFlag = false;
    
    //写eeprom
    //eeprom损坏的情况下， i32RcdIndex 是负数,
    //允许这种情况执行计量逻辑, 只是不存eeprom, 减小损坏等级
    if(stOrderCache.i32RcdIndex[eGunIndex] >= 0)
    {
        memset(&stOrderCache.stRcdTemp, 0, sizeof(stEepromAppBlockChgRcd_t));
        memcpy(&stOrderCache.stRcdTemp, pRcd, sizeof(stEepromAppBlockChgRcd_t));
        sEepromAppSetBlockChgRcdByIndex(stOrderCache.i32RcdIndex[eGunIndex], &stOrderCache.stRcdTemp);
    }
}

















/***************************************************************************************************
* Description                           :   计算当前时间所属的时段序号
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   u8Section:实际的分段数
****************************************************************************************************/
i32 sOrderProcDivGetSection(u8 u8Section)
{
    stTime_t stTime;
    i32  u8RateIndex = 0;
    
    stTime = sGetTime();
    
    //先按默认的分段数 cPrivDrvRateSection 去计算
    u8RateIndex = (stTime.hour * 2) + ((stTime.min >= 30) ? 1 : 0);
    
    //再根据实际的分段数去等比例缩放定位到实际的段序号上
    u8RateIndex = u8RateIndex / (cPrivDrvRateSection / u8Section);
    
    if(u8RateIndex >= u8Section)
    {
        u8RateIndex = 0;
    }
    
    return u8RateIndex;
}
































