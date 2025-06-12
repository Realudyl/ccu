/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   order_top_down_1.c
* Description                           :   充电订单计量计费管理 之 尖峰平谷费率1
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
#include "order.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "order_top_down_1";






extern stOrderCache_t stOrderCache;







void sOrderProcTopDown1Start(ePileGunIndex_t eGunIndex);
void sOrderProcTopDown1Run  (ePileGunIndex_t eGunIndex);
void sOrderProcTopDown1Stop (ePileGunIndex_t eGunIndex);


void sOrderProcTopDown1ProcLastSection        (ePileGunIndex_t eGunIndex, stPrivDrvMoneyTopDown1_t *pMoney);
void sOrderProcTopDown1ProcLastSectionForCross(ePileGunIndex_t eGunIndex);


i32  sOrderProcTopDown1GetMoneyDataLen   (u8 u8Section);
i32  sOrderProcTopDown1GetSection        (stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate);
bool sOrderProcTopDown1NewSectionMerge   (stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate, stPrivDrvMoneyTopDown1_t *pMoney, u8 *pSectionWithoutMerge);
bool sOrderProcTopDown1NewSectionNotMerge(stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate, stPrivDrvMoneyTopDown1_t *pMoney);









/***************************************************************************************************
* Description                           :   充电订单计量计费管理 之 尖峰平谷费率1 之 start阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcTopDown1Start(ePileGunIndex_t eGunIndex)
{
    u8  u8UnitIndex;
    u8  u8RateIndex;
    u32 u32MeterStart = 0;
    stTime_t stTime;
    
    stPrivDrvRateTopDown1_t  *pRate     = NULL;
    stPrivDrvMoneyTopDown1_t *pMoney    = NULL;
    stEepromAppBlockChgRcd_t *pRcd      = NULL;
    
    pRcd        =  stOrderCache.pRcd[eGunIndex];
    pMoney      = &pRcd->stRcdData.unMoney.stTopDown1;
    pRate       = &stOrderCache.pRate->stTopDown1;
    
    
    //1:充电记录部分字段赋初值
    stTime  = sGetTime();
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterStart, NULL);
    pRcd->stRcdData.u32ChargingTime = 0;
    pRcd->stRcdData.stTimeStart     = stTime;
    pRcd->stRcdData.u32MeterStart   = u32MeterStart;
    pRcd->stRcdData.u32MeterStop    = u32MeterStart;
    pRcd->stRcdData.u8SocStart      = -1;                                       //先给负数无效值
    pRcd->stRcdData.u32UsedMoney    = 0;
    pMoney->eMode = pRate->stHead.eMode;
    pMoney->eModeCover = pRate->stHead.eModeCover;
    pMoney->eModeParking = pRate->stHead.eModeParking;
    pMoney->eModeBooking = pRate->stHead.eModeBooking;
    pMoney->u32MoneyCoverFix = 0;
    pMoney->u32MoneyParkingFix = 0;
    pMoney->u32MoneyBookingFix = 0;
    pMoney->u8Section = 1;
    pRcd->stRcdData.u16MoneyDataLen = sOrderProcTopDown1GetMoneyDataLen(pMoney->u8Section);
    
    //初始化第一段的起点时间
    u8UnitIndex = pMoney->u8Section - 1;
    u8RateIndex = sOrderProcTopDown1GetSection(&stTime, pRate);
    pMoney->stUnit[u8UnitIndex].eType = pRate->stUint[u8RateIndex].eElectType;
    pMoney->stUnit[u8UnitIndex].u8StartDay  = stTime.day;
    pMoney->stUnit[u8UnitIndex].u8StartHour = stTime.hour;
    pMoney->stUnit[u8UnitIndex].u8StartMin  = stTime.min;
    
    //2:计量资源初始化---这3个 stUnitXxx 字段每次启动计量都要清零 避免残值影响订单电量
    memset(&stOrderCache.stUnitTemp      [eGunIndex], 0, sizeof(stPrivDrvMoneyTopDownUnit));
    memset(&stOrderCache.stUnitTempOld   [eGunIndex], 0, sizeof(stPrivDrvMoneyTopDownUnit));
    memset(&stOrderCache.stUnitSectionMax[eGunIndex], 0, sizeof(stPrivDrvMoneyTopDownUnit));
    stOrderCache.u32TimeWrEeprom[eGunIndex] = sGetTimestamp();
    stOrderCache.u32MeterStart[eGunIndex] = u32MeterStart;
    stOrderCache.u8Section[eGunIndex] = pMoney->u8Section;                      //
}







/***************************************************************************************************
* Description                           :   充电订单计量计费管理 之 尖峰平谷费率1 之 run 阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcTopDown1Run(ePileGunIndex_t eGunIndex)
{
    i32  i;
    bool bFix;
    i32  i32Soc;
    u32  u32Price;
    stTime_t stTime;
    
    u8   u8UnitIndex;                                                           //当  前电量分段序号
    u8   u8RateIndex;                                                           //当  前费率时段的序号
    u32  u32MeterNow = 0;                                                       //当前电表读数
    bool bNewSectionFlag;
    
    stPrivDrvRateTopDown1_t  *pRate     = NULL;
    stPrivDrvMoneyTopDown1_t *pMoney    = NULL;
    stEepromAppBlockChgRcd_t *pRcd      = NULL;
    
    stPrivDrvMoneyTopDownUnit *pUnit    = NULL;
    
    pRcd    =  stOrderCache.pRcd[eGunIndex];
    pMoney  = &pRcd->stRcdData.unMoney.stTopDown1;
    pRate   = &stOrderCache.pRate->stTopDown1;
    
    
    //检查当前时间是否进入新的费率时段
    stTime  = sGetTime();
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterNow, NULL);
    
    
    if(pRate->eCrossDay == ePrivDrvRateCrossDayNoMerge)
    {
        bNewSectionFlag = sOrderProcTopDown1NewSectionNotMerge(&stTime, pRate, pMoney);
    }
    else
    {
        bNewSectionFlag = sOrderProcTopDown1NewSectionMerge(&stTime, pRate, pMoney, &stOrderCache.u8Section[eGunIndex]);
    }
    u8UnitIndex = pMoney->u8Section - 1;                                        //这俩分段序号需要一起更新之后才去计量, 不可在计量逻辑前后分开更新
    u8RateIndex = sOrderProcTopDown1GetSection(&stTime, pRate);
    pRcd->stRcdData.u16MoneyDataLen = sOrderProcTopDown1GetMoneyDataLen(pMoney->u8Section);
    if(bNewSectionFlag == true)
    {
        EN_SLOGI(TAG, "费率模型:尖峰平谷1, 进入新的时段, 电表值:%d, %d, %d", stOrderCache.u32MeterStart[eGunIndex], pRcd->stRcdData.u32MeterStop, u32MeterNow);
        
        
        //u32MeterStop 是上一次电表读数
        //u32MeterNow  是本  次电表读数---不能取此值 否则会丢失它俩之间的电量差值---此差值可能是0可能不是0，取决于电表读数更新
        //费率切换时段时 更新这一时段电表的起始值
        stOrderCache.u32MeterStart[eGunIndex] = pRcd->stRcdData.u32MeterStop;
        
        
        if(pMoney->u8Section == cOrderTopDown1TimeSection)
        {
            //电量分段数达到最大值以后再跨时段时 需要执行这个清零动作
            sOrderProcTopDown1ProcLastSectionForCross(eGunIndex);
        }
    }
    
    //电量累加---前者<后者 代表 u32MeterNow 的值异常了
    if(u32MeterNow >= stOrderCache.u32MeterStart[eGunIndex])
    {
        //这一行很关键, 当 pMoney->u8Section 达到 cOrderTopDown1TimeSection 以后,   pUnit 就取 &stOrderCache.stUnitTemp[eGunIndex]
        //此后每一个时段期间, 电量及金额就都用 pUnit 来累积, 并在进入新的时段时将 pUnit 累积值往 pMoney->stUnit 的最后一段里面叠加
        pUnit = (pMoney->u8Section == cOrderTopDown1TimeSection) ? &stOrderCache.stUnitTemp[eGunIndex] : &pMoney->stUnit[u8UnitIndex];
        
        //电损补偿 电损补偿系数默认值 = cEleLossDefault, 表示不补偿
        pUnit->u32Elect = sOrderProcCalcElectAdj(u32MeterNow - stOrderCache.u32MeterStart[eGunIndex]);
    }
    else
    {
        //那就直接返回，不要执行电量累加 避免产生极大值（负数）
        EN_SLOGI(TAG, "费率模型:尖峰平谷1, 电表值异常, 本次不予计算, u8UnitIndex:%d, u32MeterNow:%d, u32MeterStart:%d", u8UnitIndex, u32MeterNow, stOrderCache.u32MeterStart[eGunIndex]);
        return;
    }
    
    
    //分段电费
    u32Price = pRate->stHead.u32RateElect[pRate->stUint[u8RateIndex].eElectType];
    pUnit->u32MoneyElect = sOrderProcCalcMoney(u32Price, pUnit->u32Elect);
    
    //分段服务费
    bFix = (pRate->stHead.eModeCover == ePrivDrvFeeModeFix);                    //选择服务费价格
    u32Price = (bFix) ? pRate->stHead.u32RateCoverFix : pRate->stHead.u32RateCover[pRate->stUint[u8RateIndex].eCoverType];
    pUnit->u32MoneyCover = sOrderProcCalcMoney(u32Price, pUnit->u32Elect);
    
    //分段占桩费
    bFix = (pRate->stHead.eModeParking == ePrivDrvFeeModeFix);                  //选择占桩费价格
    u32Price = (bFix) ? pRate->stHead.u32RateParkingFix : pRate->stHead.u32RateParking[pRate->stUint[u8RateIndex].eParkingType];
    pUnit->u32MoneyParking = sOrderProcCalcMoney(u32Price, pUnit->u32Elect);
    
    //分段预约费
    bFix = (pRate->stHead.eModeBooking == ePrivDrvFeeModeFix);                  //选择预约费价格
    u32Price = (bFix) ? pRate->stHead.u32RateBookingFix : pRate->stHead.u32RateBooking[pRate->stUint[u8RateIndex].eBookingType];
    pUnit->u32MoneyBooking = sOrderProcCalcMoney(u32Price, pUnit->u32Elect);
    
    //当使用 stUnitTemp[] 来累积电量时, 需要动态的将电量明细数据合并到最后一个时段上
    if(pUnit == (&stOrderCache.stUnitTemp[eGunIndex]))
    {
        sOrderProcTopDown1ProcLastSection(eGunIndex, pMoney);
    }
    
    //总电费和总服务费
    pRcd->stRcdData.u32UsedMoney = 0;
    for(i = 0; i < pMoney->u8Section; i++)
    {
        pRcd->stRcdData.u32UsedMoney += pMoney->stUnit[i].u32MoneyElect + pMoney->stUnit[i].u32MoneyCover + pMoney->stUnit[i].u32MoneyParking + pMoney->stUnit[i].u32MoneyBooking;
        if(pRate->stHead.eModeCover == ePrivDrvFeeModeFix)
        {
            pMoney->u32MoneyCoverFix += pMoney->stUnit[i].u32MoneyCover;
        }
        if(pRate->stHead.eModeParking == ePrivDrvFeeModeFix)
        {
            pMoney->u32MoneyParkingFix += pMoney->stUnit[i].u32MoneyParking;
        }
        if(pRate->stHead.eModeBooking == ePrivDrvFeeModeFix)
        {
            pMoney->u32MoneyBookingFix += pMoney->stUnit[i].u32MoneyBooking;
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
* Description                           :   充电订单计量计费管理 之 尖峰平谷费率1 之 stop阶段动作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderProcTopDown1Stop(ePileGunIndex_t eGunIndex)
{
    stTime_t stTime;
    u8   u8UnitIndex;                                                           //当  前电量分段序号
    u32  u32MeterStop = 0;
    
    stEepromAppBlockChgRcd_t *pRcd =  stOrderCache.pRcd[eGunIndex];
    
    
    vTaskDelay(2000 / portTICK_RATE_MS);                                        //2s 等待电表更新最终计量数据
    sMeterDrvOptGetEnergy(eGunIndex, &u32MeterStop, NULL);
    
    sOrderProcTopDown1Run(eGunIndex);                                           //最后更新一次电量明细数据
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
    
    //为最后一段电量数据更新结束时间
    stTime = sGetTime();
    u8UnitIndex = pRcd->stRcdData.unMoney.stTopDown1.u8Section - 1;
    pRcd->stRcdData.unMoney.stTopDown1.stUnit[u8UnitIndex].u8EndDay  = stTime.day;
    pRcd->stRcdData.unMoney.stTopDown1.stUnit[u8UnitIndex].u8EndHour = stTime.hour;
    pRcd->stRcdData.unMoney.stTopDown1.stUnit[u8UnitIndex].u8EndMin  = stTime.min;
    
    pRcd->bChargingFlag = false;
    stOrderCache.u32Stopreason[eGunIndex] = 0;
    
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
* Description                           :   在不合并模式下, 当电量分段数达到最大值 cOrderTopDown1TimeSection 时 
* Author                                :   Hall
* Creat Date                            :   2024-09-06
* notice                                :   需要将后续电量明细合并到最后一个时段上
****************************************************************************************************/
void sOrderProcTopDown1ProcLastSection(ePileGunIndex_t eGunIndex, stPrivDrvMoneyTopDown1_t *pMoney)
{
    //1:先更新 stUnitSectionMax
    //因为计算过程中要减去 old 值，再加上当前值, 为了避免此过程中（错误的）电量明细值被外部上报给网关
    //所以要使用此 中间值
    stOrderCache.stUnitSectionMax[eGunIndex].u32Elect            += stOrderCache.stUnitTemp[eGunIndex].u32Elect        - stOrderCache.stUnitTempOld[eGunIndex].u32Elect;
    stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyElect       += stOrderCache.stUnitTemp[eGunIndex].u32MoneyElect   - stOrderCache.stUnitTempOld[eGunIndex].u32MoneyElect;
    stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyCover       += stOrderCache.stUnitTemp[eGunIndex].u32MoneyCover   - stOrderCache.stUnitTempOld[eGunIndex].u32MoneyCover;
    stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyParking     += stOrderCache.stUnitTemp[eGunIndex].u32MoneyParking - stOrderCache.stUnitTempOld[eGunIndex].u32MoneyParking;
    stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyBooking     += stOrderCache.stUnitTemp[eGunIndex].u32MoneyBooking - stOrderCache.stUnitTempOld[eGunIndex].u32MoneyBooking;
    
    //2:stUnitSectionMax 是 pMoney->stUnit[11] 的中间值
    //中间值 完成加减计算以后再赋值给 pMoney->stUnit[11]
    pMoney->stUnit[cOrderTopDown1TimeSection - 1].u32Elect        = stOrderCache.stUnitSectionMax[eGunIndex].u32Elect;
    pMoney->stUnit[cOrderTopDown1TimeSection - 1].u32MoneyElect   = stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyElect;
    pMoney->stUnit[cOrderTopDown1TimeSection - 1].u32MoneyCover   = stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyCover;
    pMoney->stUnit[cOrderTopDown1TimeSection - 1].u32MoneyParking = stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyParking;
    pMoney->stUnit[cOrderTopDown1TimeSection - 1].u32MoneyBooking = stOrderCache.stUnitSectionMax[eGunIndex].u32MoneyBooking;
    
    
    //3:更新 stUnitTempOld
    stOrderCache.stUnitTempOld[eGunIndex].u32Elect                = stOrderCache.stUnitTemp[eGunIndex].u32Elect;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyElect           = stOrderCache.stUnitTemp[eGunIndex].u32MoneyElect;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyCover           = stOrderCache.stUnitTemp[eGunIndex].u32MoneyCover;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyParking         = stOrderCache.stUnitTemp[eGunIndex].u32MoneyParking;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyBooking         = stOrderCache.stUnitTemp[eGunIndex].u32MoneyBooking;
    
}







/***************************************************************************************************
* Description                           :   在不合并模式下, 当电量分段数达到最大值 cOrderTopDown1TimeSection 时 
* Author                                :   Hall
* Creat Date                            :   2024-09-07
* notice                                :   在此发生跨时段事件时 需要清零几个电量缓存字段
****************************************************************************************************/
void sOrderProcTopDown1ProcLastSectionForCross(ePileGunIndex_t eGunIndex)
{
    EN_SLOGI(TAG, "费率模型:尖峰平谷1, 电量分段数达上限%d情况下进入新的时段", cOrderTopDown1TimeSection);
    EN_SLOGI(TAG, "费率模型:尖峰平谷1, stUnitTemp      [%d].u32Elect:%u", eGunIndex, stOrderCache.stUnitTemp[eGunIndex].u32Elect);
    EN_SLOGI(TAG, "费率模型:尖峰平谷1, stUnitTempOld   [%d].u32Elect:%u", eGunIndex, stOrderCache.stUnitTempOld[eGunIndex].u32Elect);
    EN_SLOGI(TAG, "费率模型:尖峰平谷1, stUnitSectionMax[%d].u32Elect:%u", eGunIndex, stOrderCache.stUnitSectionMax[eGunIndex].u32Elect);
    
    stOrderCache.stUnitTemp[eGunIndex].u32Elect             = 0;
    stOrderCache.stUnitTemp[eGunIndex].u32MoneyElect        = 0;
    stOrderCache.stUnitTemp[eGunIndex].u32MoneyCover        = 0;
    stOrderCache.stUnitTemp[eGunIndex].u32MoneyParking      = 0;
    stOrderCache.stUnitTemp[eGunIndex].u32MoneyBooking      = 0;
    
    stOrderCache.stUnitTempOld[eGunIndex].u32Elect          = 0;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyElect     = 0;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyCover     = 0;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyParking   = 0;
    stOrderCache.stUnitTempOld[eGunIndex].u32MoneyBooking   = 0;
}









/***************************************************************************************************
* Description                           :   根据当前的电量明细数据分段数计算其数据长度
* Author                                :   Hall
* Creat Date                            :   2024-06-21
* notice                                :   
****************************************************************************************************/
i32 sOrderProcTopDown1GetMoneyDataLen(u8 u8Section)
{
    return (sizeof(stPrivDrvMoneyTopDown1_t) - (sizeof(stPrivDrvMoneyTopDownUnit) * (cOrderTopDown1TimeSection - u8Section)));
}








/***************************************************************************************************
* Description                           :   计算当前时间在费率模型分段费率中所属的段序号
* Author                                :   Hall
* Creat Date                            :   2024-05-23
* notice                                :   
****************************************************************************************************/
i32 sOrderProcTopDown1GetSection(stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate)
{
    i32  i;
    i32  i32TimeMinutes;
    
    //先将 pTime 转换为分钟值
    i32TimeMinutes = sGetMinutesFromTime(pTime->hour, pTime->min);
    
    //遍历费率模型中的所有分段
    for(i = 0; i < (pRate->u8Section - 1); i++)
    {
        if((i32TimeMinutes >= sGetMinutesFromTime(pRate->stUint[i + 0].u8Hour, pRate->stUint[i + 0].u8Minute))
        && (i32TimeMinutes <  sGetMinutesFromTime(pRate->stUint[i + 1].u8Hour, pRate->stUint[i + 1].u8Minute)))
        {
            break;
        }
    }
    
    return i;
}








/***************************************************************************************************
* Description                           :   (合并计算模式下)当前费率切换时段时 电量明细数据字段 操作函数
* Author                                :   Hall
* Creat Date                            :   2024-05-24
* notice                                :    
*                                           以费率模型4段为例           即 pRate->u8Section = 4 时
*                                           逻辑运算数值表
*
*→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→时间流逝方向→→→→→→→→→→→→→→→→
*   u8RateIndex                                                         费率模型分段序号， 对 pRate->stUint[] 操作全部基于此序号
* 1:u8RateIndex         :   1   2   3   4   1   2   3   4   1...
* 2:u8RateIndex         :   2   3   4   1   2   3   4   1   2...
* 3:u8RateIndex         :   3   4   1   2   3   4   1   2   3...
* 4:u8RateIndex         :   4   1   2   3   4   1   2   3   4...
*                           ↓------------------------------------------→启动充电时可以处于任一个费率时段(1,2,3,4...)
*
* *pSectionWithoutMerge :   1   2   3   4   5   6   7   8   9...        不合并的段数
*
* pMoney->u8Section     :   1   2   3   4   4   4   4   4   4...        合并的段数，止步于 pRate->u8Section
*
* u8UnitIndex           :   0   1   2   3   0   1   2   3   0...        分段电量序号， 对 pMoney->stUnit[] 操作全部基于此序号
*
*
* u8StartDay
* u8StartHour
* u8StartMin            :   x   1   1   1   0   0   0   0   0...        分段的起始时间更新动作， 1---更新, 0---不更新
*                           ↓------------------------------------------→首段的起始时间本函数无法更新，由 start 函数负责更新
*
* u8EndtDay
* u8EndHour
* u8EndMin              :       1   1   1   1   1   1   1   1   x...    分段的结束时间更新动作， 1---更新, 0---不更新
*                                                               ↓------→末段的结束时间本函数无法更新，由 stop 函数负责更新
****************************************************************************************************/
bool sOrderProcTopDown1NewSectionMerge(stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate, stPrivDrvMoneyTopDown1_t *pMoney, u8 *pSectionWithoutMerge)
{
    u8   u8UnitIndex;                                                           //当  前电量分段序号
    u8   u8RateIndex;                                                           //当  前费率时段的序号 见上面的数值表
    bool bFlag1;
    bool bFlag2;
    
    u8UnitIndex = ((*pSectionWithoutMerge) - 1) % pRate->u8Section;
    u8RateIndex = sOrderProcTopDown1GetSection(pTime, pRate);
    
    //当前电量分段的结束时间就是新费率时段的起始时间
    pMoney->stUnit[u8UnitIndex].u8EndDay  = pTime->day;
    pMoney->stUnit[u8UnitIndex].u8EndHour = pTime->hour;
    pMoney->stUnit[u8UnitIndex].u8EndMin  = pTime->min;
    
    
    //跨天时刻
    bFlag1 = ((pTime->hour == 0) && (pTime->min == 0) && (pTime->sec == 0));
    //尚未新建起始时间为0点0分的分段
    bFlag2 = ((pMoney->stUnit[u8UnitIndex].u8StartHour != 0) || (pMoney->stUnit[u8UnitIndex].u8StartMin != 0));
    
    if((bFlag1 == true) && (bFlag2 == true)//跨天的时刻，时段更新(费率相同情况)
    || (pMoney->stUnit[u8UnitIndex].eType != pRate->stUint[u8RateIndex].eElectType))//费率变化 跨越了一个新的费率时段
    {
        //跨天充电的时刻，时段更新的同时可以将当天最后一个时段结束时间赋值24:00，方便平台计算
        if((bFlag1 == true) && (bFlag2 == true))
        {
            pMoney->stUnit[u8UnitIndex].u8EndDay  = pTime->day;
            pMoney->stUnit[u8UnitIndex].u8EndHour = 24;
            pMoney->stUnit[u8UnitIndex].u8EndMin  = 0;
        }
        
        //2：电量分段数加1
        (*pSectionWithoutMerge)++;
        pMoney->u8Section = ((*pSectionWithoutMerge) <= pRate->u8Section) ? (*pSectionWithoutMerge) : pRate->u8Section;
        EN_SLOGI(TAG, "费率模型:尖峰平谷1, 进入新的时段:%d (合并算法)", pMoney->u8Section);
        
        
        //3：初始化新的电量分段部分字段
        //根据定义,电量明细段数超过费率模型时段数以后，开始合并计算时, 就不要再更新起点时间了
        u8UnitIndex = ((*pSectionWithoutMerge) - 1) % pRate->u8Section;
        if((*pSectionWithoutMerge) == pMoney->u8Section)
        {
            pMoney->stUnit[u8UnitIndex].eType       = pRate->stUint[u8RateIndex].eElectType;
            pMoney->stUnit[u8UnitIndex].u8StartDay  = pTime->day;
            pMoney->stUnit[u8UnitIndex].u8StartHour = pRate->stUint[u8RateIndex].u8Hour;
            pMoney->stUnit[u8UnitIndex].u8StartMin  = pRate->stUint[u8RateIndex].u8Minute;
        }
        
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   (不合并计算模式下)当前费率切换时段时 电量明细数据字段 操作函数
* Author                                :   Hall
* Creat Date                            :   2024-06-21
* notice                                :    
*                                           以费率模型4段为例                           即 pRate->u8Section = 4 时
*                                           逻辑运算数值表
*
*→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→时间流逝方向→→→→→→→→→→→→→→→→→→→→→→→→
*   u8RateIndex                                                                         费率模型分段序号， 对 pRate->stUint[] 操作全部基于此序号
* 1:u8RateIndex         :   1   2   3   4   1   2   3   4   1   2   3   4   1...
* 2:u8RateIndex         :   2   3   4   1   2   3   4   1   2   3   4   1   2...
* 3:u8RateIndex         :   3   4   1   2   3   4   1   2   3   4   1   2   3...
* 4:u8RateIndex         :   4   1   2   3   4   1   2   3   4   1   2   3   4...
*                           ↓----------------------------------------------------------→启动充电时可以处于任一个费率时段(1,2,3,4...)
*
*
* pMoney->u8Section     :   1   2   3   4   5   6   7   8   9   10  11  12  12...       不合并的段数，止步于 cOrderTopDown1TimeSection
*
* u8UnitIndex           :   0   1   2   3   4   5   6   7   8   9   10  11  11..        分段电量序号， 对 pMoney->stUnit[] 操作全部基于此序号
*
*
* u8StartDay
* u8StartHour
* u8StartMin            :   x   1   1   1   1   1   1   1   1   1   1   1   0...        分段的起始时间更新动作， 1---更新, 0--不更新
*                           ↓----------------------------------------------------------→首段的起始时间本函数无法更新，由 start 函数负责更新
*                                                                           ↓----------→计满 cOrderTopDown1TimeSection 段以后再不更新
* u8EndtDay
* u8EndHour
* u8EndMin              :       1   1   1   1   1   1   1   1   1   1   1   1   x...    分段的结束时间更新动作， 1---更新, 0--不更新
*                                                                               ↓------→末段的结束时间本函数无法更新，由 stop 函数负责更新
*
*
*
* 函数跨天逻辑有个bug:  由于计量逻辑执行周期是500ms，但跨天逻辑是 ((bFlag1 == true) && (bFlag2 == true))
*                       在电量分段数达到 cOrderTopDown1TimeSection 段以后 u8StartHour&u8StartMin 值不再更新
*                       因此 00:00:00 这一秒内会执行2次跨天逻辑, 比期望的多执行1次。
* 不过实际分析数据:     发现并不影响计量, 因为此时电量分段数已经达到上限，不会显示多出的分段 切电量和电费并不丢失
* 实际分析数据如下：
09报文电量明细---合并后的数据
第1段  分段类型:平 起始日：7 起始时：16 起始分：22 结束日：7 结束时：17 结束分：0  电量：4848    电费：809616   服务费：969600  占桩费：0 预约费：0
第2段  分段类型:尖 起始日：7 起始时：17 起始分：0  结束日：8 结束时：24 结束分：0  电量：1577    电费：704919   服务费：157700  占桩费：0 预约费：0
第3段  分段类型:谷 起始日：8 起始时：0  起始分：0  结束日：8 结束时：10 结束分：30 电量：87      电费：20271    服务费：8700    占桩费：0 预约费：0
第4段  分段类型:峰 起始日：8 起始时：10 起始分：30 结束日：8 结束时：11 结束分：0  电量：2145    电费：714285   服务费：214500  占桩费：0 预约费：0
第5段  分段类型:平 起始日：8 起始时：11 起始分：0  结束日：8 结束时：11 结束分：30 电量：4241    电费：708247   服务费：848200  占桩费：0 预约费：0
第6段  分段类型:尖 起始日：8 起始时：11 起始分：30 结束日：8 结束时：12 结束分：0  电量：4229    电费：1890363  服务费：422900  占桩费：0 预约费：0
第7段  分段类型:谷 起始日：8 起始时：12 起始分：0  结束日：8 结束时：12 结束分：30 电量：4229    电费：985357   服务费：422900  占桩费：0 预约费：0
第8段  分段类型:峰 起始日：8 起始时：12 起始分：30 结束日：8 结束时：13 结束分：0  电量：4227    电费：1407591  服务费：422700  占桩费：0 预约费：0
第9段  分段类型:平 起始日：8 起始时：13 起始分：0  结束日：8 结束时：13 结束分：30 电量：4228    电费：706076   服务费：845600  占桩费：0 预约费：0
第10段 分段类型:尖 起始日：8 起始时：13 起始分：30 结束日：8 结束时：14 结束分：0  电量：4224    电费：1888128  服务费：422400  占桩费：0 预约费：0
第11段 分段类型:谷 起始日：8 起始时：14 起始分：0  结束日：8 结束时：15 结束分：0  电量：8453    电费：1969549  服务费：845300  占桩费：0 预约费：0
第12段 分段类型:平 起始日：8 起始时：15 起始分：0  结束日：9 结束时：11 结束分：27 电量：172977  电费：53423511 服务费：18529100 占桩费：0 预约费：0


                               电量wh   电价.元/kwh   服务费价.元/kwh     电费0.00001元  服务费0.00001元
第12段(合并前)  15:00---16:00：8454     3.33          1                   2815182        845400
第13段(合并前)  16:00---17:00：8454     1.67          2                   1411818        1690800
第14段(合并前)  17:00---24:00：59171    4.47          1                   26449437       5917100
第15段(合并前)  17:00---24:00：2        2.33          1                   466            200------------多出来的那个跨天时段，电量较小
第16段(合并前)  00:00---10:30：88790    2.33          1                   20688070       8879000
第17段(合并前)  10:30---11:00：4246     3.33          1                   1413918        424600
第18段(合并前)  11:00---11:27：3860     1.67          2                   644620         772000

第12段(合并后)                                                            53423511       18529100-------与上面合并后的12段数据对得上

****************************************************************************************************/
bool sOrderProcTopDown1NewSectionNotMerge(stTime_t *pTime, stPrivDrvRateTopDown1_t *pRate, stPrivDrvMoneyTopDown1_t *pMoney)
{
    u8      u8UnitIndex;                                                        //当  前电量分段序号
    u8      u8RateIndex;                                                        //当  前费率时段的序号 见上面的数值表
    bool    bFlag1;
    bool    bFlag2;
    
    
    u8UnitIndex = pMoney->u8Section - 1;
    u8RateIndex = sOrderProcTopDown1GetSection(pTime, pRate);
    
    
    //1：更新当前电量分段的结束时间
    //实时更新结束时间，防止异常场景(如掉电)没设置时间
    pMoney->stUnit[u8UnitIndex].u8EndDay  = pTime->day;
    pMoney->stUnit[u8UnitIndex].u8EndHour = pTime->hour;
    pMoney->stUnit[u8UnitIndex].u8EndMin  = pTime->min;
    
    //跨天时刻
    bFlag1 = ((pTime->hour == 0) && (pTime->min == 0) && (pTime->sec == 0));
    //尚未新建起始时间为0点0分的分段
    bFlag2 = ((pMoney->stUnit[u8UnitIndex].u8StartHour != 0) || (pMoney->stUnit[u8UnitIndex].u8StartMin != 0));
    
    if(((bFlag1 == true) && (bFlag2 == true))//跨天的时刻，时段更新(费率相同情况)
    || (pMoney->stUnit[u8UnitIndex].eType != pRate->stUint[u8RateIndex].eElectType))//费率变化 跨越了一个新的费率时段
    {
        //跨天充电的时刻，时段更新的同时可以将当天最后一个时段结束时间赋值24:00，方便平台计算
        if((bFlag1 == true) && (bFlag2 == true))
        {
            pMoney->stUnit[u8UnitIndex].u8EndDay  = pTime->day;
            pMoney->stUnit[u8UnitIndex].u8EndHour = 24;
            pMoney->stUnit[u8UnitIndex].u8EndMin  = 0;
        }
        
        
        //2：电量分段数加1
        pMoney->u8Section++;
        EN_SLOGI(TAG, "费率模型:尖峰平谷1, 进入新的时段:%d (不合并算法)", pMoney->u8Section);
        if(pMoney->u8Section > cOrderTopDown1TimeSection)
        {
            //超过 cOrderTopDown1TimeSection 段以后的数据全部叠加到最后一段
            pMoney->u8Section = cOrderTopDown1TimeSection;
            u8UnitIndex = pMoney->u8Section - 1;
            
            //此字段需要更新, 否则每次调用本函数都会满足切换时段的条件
            pMoney->stUnit[u8UnitIndex].eType = pRate->stUint[u8RateIndex].eElectType;
        }
        else
        {
            //根据定义,电量明细段数超过 cOrderTopDown1TimeSection 以后, 就不要再更新起点时间了
            u8UnitIndex = pMoney->u8Section - 1;
            pMoney->stUnit[u8UnitIndex].eType       = pRate->stUint[u8RateIndex].eElectType;
            pMoney->stUnit[u8UnitIndex].u8StartDay  = pTime->day;
            pMoney->stUnit[u8UnitIndex].u8StartHour = pRate->stUint[u8RateIndex].u8Hour;
            pMoney->stUnit[u8UnitIndex].u8StartMin  = pRate->stUint[u8RateIndex].u8Minute;
        }
        
        return true;
    }
    
    return false;
}










