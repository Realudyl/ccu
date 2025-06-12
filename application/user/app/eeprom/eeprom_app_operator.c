/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_operator.c
* Description                           :   eeprom芯片用户程序实现 之 运营商参数部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-07
* notice                                :   
****************************************************************************************************/
#include "eeprom_app.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_app_operator";





extern stEepromAppCache_t stEepromAppCache;






void sEepromAppSetBlockOperatorDefault(void);
void sEepromAppSetBlockOperatorDefaultRateDiv(void);
void sEepromAppSetBlockOperatorDefaultRateTopDown1(void);



bool sEepromAppGetBlockOperatorRate(unPrivDrvRate_t *pRate);
bool sEepromAppGetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32MaxLen, char *pCode);
u32  sEepromAppGetBlockOperatorEleLoss(void);


bool sEepromAppSetBlockOperatorRate(unPrivDrvRate_t *pRate);
bool sEepromAppSetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32Len, const char *pCode);
bool sEepromAppSetBlockOperatorEleLoss(u32 u32EleLoss);












/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区 恢复默认值
* Author                                :   Hall
* modified Date                         :   2024-06-22
* notice                                :   
***********************************************************************************************/
void sEepromAppSetBlockOperatorDefault(void)
{
    i32  i;
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockOperator], portMAX_DELAY);
    memset(pOperator, 0, sizeof(stEepromAppBlockOperatorInfo_t));
    sEepromAppSetBlockOperatorDefaultRateTopDown1();
    pOperator->u32EleLoss               = cEleLossDefault;                      //电损率
    
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockOperator]);
    
}






/**********************************************************************************************
* Description                           :   生成一个默认的平均分段费率模型
* Author                                :   Hall
* modified Date                         :   2024-05-24
* notice                                :   
***********************************************************************************************/
void sEepromAppSetBlockOperatorDefaultRateDiv(void)
{
    i32  i;
    unPrivDrvRate_t *pRate = &stEepromAppCache.stOperator.unRate;
    
    memset(pRate, 0, sizeof(unPrivDrvRate_t));
    pRate->stDiv.eMode = ePrivDrvRateModelDivEqually;                           //平均分段
    pRate->stDiv.eModeCover = ePrivDrvFeeModeSection;                           //服务费率分段
    pRate->stDiv.u32RateCoverFix = 0;
    pRate->stDiv.u8Section = cPrivDrvRateSection;                               //默认48段
    for(i = 0; i < cPrivDrvRateSection; i++)
    {
        pRate->stDiv.u32RateElect[i] = 100000;                                  //电  费 1.0元/kWh
        pRate->stDiv.u32RateCover[i] = 50000;                                   //服务费 0.5元/kWh
    }
    
    
}





/**********************************************************************************************
* Description                           :   生成一个默认的尖峰平谷1费率模型
* Author                                :   Hall
* modified Date                         :   2024-06-22
* notice                                :   
***********************************************************************************************/
void sEepromAppSetBlockOperatorDefaultRateTopDown1(void)
{
    i32  i;
    unPrivDrvRate_t *pRate = &stEepromAppCache.stOperator.unRate;
    
    memset(pRate, 0, sizeof(unPrivDrvRate_t));
    pRate->stTopDown1.stHead.eMode = ePrivDrvRateModelTopDown1;             //尖峰平谷1
    pRate->stTopDown1.stHead.eModeCover = ePrivDrvFeeModeSection;           //
    pRate->stTopDown1.stHead.eModeParking = ePrivDrvFeeModeSection;         //
    pRate->stTopDown1.stHead.eModeBooking = ePrivDrvFeeModeSection;         //
    pRate->stTopDown1.stHead.u32RateCoverFix = 0;
    pRate->stTopDown1.stHead.u32RateParkingFix = 0;
    pRate->stTopDown1.stHead.u32RateBookingFix = 0;
    for(i = 0; i < ePrivDrvRateTypeMax; i++)
    {
        pRate->stTopDown1.stHead.u32RateElect[i] = 200000 - (i * 50000);
        pRate->stTopDown1.stHead.u32RateCover[i] = 100000;
        pRate->stTopDown1.stHead.u32RateParking[i] = 0;
        pRate->stTopDown1.stHead.u32RateBooking[i] = 0;
    }
    snprintf((char *)pRate->stTopDown1.u8RateModelId, sizeof(pRate->stTopDown1.u8RateModelId), "TopDown1_DefaultId_20240622");
    pRate->stTopDown1.eCrossDay = ePrivDrvRateCrossDayNoMerge;
    pRate->stTopDown1.u8Section = 4;
    pRate->stTopDown1.stUint[0].u8Hour = 0;
    pRate->stTopDown1.stUint[0].u8Minute = 0;
    pRate->stTopDown1.stUint[0].eElectType = ePrivDrvRateTypeValley;
    pRate->stTopDown1.stUint[0].eCoverType = ePrivDrvRateTypeValley;
    pRate->stTopDown1.stUint[0].eParkingType = ePrivDrvRateTypeValley;
    pRate->stTopDown1.stUint[0].eBookingType = ePrivDrvRateTypeValley;
    pRate->stTopDown1.stUint[1].u8Hour = 6;
    pRate->stTopDown1.stUint[1].u8Minute = 0;
    pRate->stTopDown1.stUint[1].eElectType = ePrivDrvRateTypePeak;
    pRate->stTopDown1.stUint[1].eCoverType = ePrivDrvRateTypePeak;
    pRate->stTopDown1.stUint[1].eParkingType = ePrivDrvRateTypePeak;
    pRate->stTopDown1.stUint[1].eBookingType = ePrivDrvRateTypePeak;
    pRate->stTopDown1.stUint[2].u8Hour = 12;
    pRate->stTopDown1.stUint[2].u8Minute = 0;
    pRate->stTopDown1.stUint[2].eElectType = ePrivDrvRateTypeFlat;
    pRate->stTopDown1.stUint[2].eCoverType = ePrivDrvRateTypeFlat;
    pRate->stTopDown1.stUint[2].eParkingType = ePrivDrvRateTypeFlat;
    pRate->stTopDown1.stUint[2].eBookingType = ePrivDrvRateTypeFlat;
    pRate->stTopDown1.stUint[3].u8Hour = 18;
    pRate->stTopDown1.stUint[3].u8Minute = 0;
    pRate->stTopDown1.stUint[3].eElectType = ePrivDrvRateTypeSharp;
    pRate->stTopDown1.stUint[3].eCoverType = ePrivDrvRateTypeSharp;
    pRate->stTopDown1.stUint[3].eParkingType = ePrivDrvRateTypeSharp;
    pRate->stTopDown1.stUint[3].eBookingType = ePrivDrvRateTypeSharp;
    
    
}













/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->读取 费率模型 字段
* Author                                :   Hall
* modified Date                         :   2024-05-23
* notice                                :   
***********************************************************************************************/
bool sEepromAppGetBlockOperatorRate(unPrivDrvRate_t *pRate)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    if(pRate != NULL)
    {
        memcpy(pRate, &pOperator->unRate, sizeof(pOperator->unRate));
        
        return(true);
    }
    
    return(false);
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->读取 二维码 字段
* Author                                :   Hall
* modified Date                         :   2024-05-23
* notice                                :   
***********************************************************************************************/
bool sEepromAppGetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32MaxLen, char *pCode)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    if((eGunIndex < ePileGunIndexNum) && (pCode != NULL) && (i32MaxLen >= strlen(pOperator->u8Qrcode[eGunIndex])))
    {
        memset(pCode, 0, i32MaxLen);
        memcpy(pCode, pOperator->u8Qrcode[eGunIndex], strlen(pOperator->u8Qrcode[eGunIndex]));
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->读取 电损率 字段
* Author                                :   Hall
* modified Date                         :   2024-07-24
* notice                                :   
***********************************************************************************************/
u32 sEepromAppGetBlockOperatorEleLoss(void)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    return pOperator->u32EleLoss;
}













/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->写入 费率模型 字段
* Author                                :   Hall
* modified Date                         :   2024-05-23
* notice                                :   
***********************************************************************************************/
bool sEepromAppSetBlockOperatorRate(unPrivDrvRate_t *pRate)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    if(pRate != NULL)
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockOperator], portMAX_DELAY);
        memset(&pOperator->unRate, 0, sizeof(pOperator->unRate));
        memcpy(&pOperator->unRate, pRate, sizeof(pOperator->unRate));
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockOperator]);
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->写入 二维码 字段
* Author                                :   Hall
* modified Date                         :   2024-05-23
* notice                                :   
***********************************************************************************************/
bool sEepromAppSetBlockOperatorQrcode(ePileGunIndex_t eGunIndex, i32 i32Len, const char *pCode)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    if((eGunIndex < ePileGunIndexNum) && (pCode != NULL) && (i32Len < sizeof(pOperator->u8Qrcode[eGunIndex])))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockOperator], portMAX_DELAY);
        memset(pOperator->u8Qrcode[eGunIndex], 0, sizeof(pOperator->u8Qrcode[eGunIndex]));
        memcpy(pOperator->u8Qrcode[eGunIndex], pCode, i32Len);
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockOperator]);
        
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->运营商参数区--->写入 电损率 字段
* Author                                :   Hall
* modified Date                         :   2024-07-24
* notice                                :   
***********************************************************************************************/
bool sEepromAppSetBlockOperatorEleLoss(u32 u32EleLoss)
{
    stEepromAppBlockOperatorInfo_t *pOperator = &stEepromAppCache.stOperator;
    
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockOperator], portMAX_DELAY);
    pOperator->u32EleLoss = u32EleLoss;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockOperator]);
    
    return(true);
}
















