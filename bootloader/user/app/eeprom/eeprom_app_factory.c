/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_factory.c
* Description                           :   eeprom芯片用户程序实现 之 工厂参数部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-07
* notice                                :   
*                                           工厂参数有存储区和备份区, 外部读写api函数一律只对存储区操作
*                                           备份区仅在存储区校验值错误时用于恢复存储区内容。
****************************************************************************************************/
#include "eeprom_app.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_app_factory";





extern stEepromAppCache_t stEepromAppCache;







void sEepromAppSetBlockFactoryDefault(void);



















/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->工厂参数区 初始化
* Author                                :   Hall
* modified Date                         :   2024-05-27
* notice                                :   
***********************************************************************************************/
void sEepromAppSetBlockFactoryDefault(void)
{
    if(stEepromAppCache.pCbDefaultFactory != NULL)
    {
        stEepromAppCache.pCbDefaultFactory();
    }
}










ePrivDrvChargerType_t sEepromAppGetBlockFactoryChargerType(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eChargerType;
}



ePrivDrvPayAt_t sEepromAppGetBlockFactoryPayAt(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePayAt;
}



ePrivDrvMoneyCalcBy_t sEepromAppGetBlockFactoryMoneyCalcBy(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eMoneyCalcBy;
}



ePrivDrvChargerDisableFlag_t sEepromAppGetBlockFactoryDisableFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eDisableFlag;
}



ePrivDrvStartPowerMode_t sEepromAppGetBlockFactoryPowerMode(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePowerMode;
}



ePrivDrvChargerWorkMode_t sEepromAppGetBlockFactoryWorkMode(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eWorkMode;
}



ePrivDrvBalancePhase_t sEepromAppGetBlockFactoryBalancePhase(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePhase;
}



ePrivDrvBookingViaBle_t sEepromAppGetBlockFactoryBookingViaBle(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eBookingViaBle;
}



ePrivDrvRebootReason_t sEepromAppGetBlockFactoryRebootReason(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].eReason;
}



bool sEepromAppGetBlockFactoryResFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bResFlag;
}



f32 sEepromAppGetBlockFactoryPowerMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32PowerMax;
}



f32 sEepromAppGetBlockFactoryCurrMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrMax;
}



f32 sEepromAppGetBlockFactoryPowerRated(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32PowerRated;
}



f32 sEepromAppGetBlockFactoryCurrRated(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrRated;
}



f32 sEepromAppGetBlockFactoryCurrLimitByGun(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrLimitByGun;
}



















/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->工厂参数区--->读取 u8Sn 字段
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool sEepromAppGetBlockFactorySn(char *pSn, i32 i32MaxLen)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pSn != NULL) && (i32MaxLen >= sizeof(pFactory->u8Sn)))
    {
        memset(pSn, 0, i32MaxLen);
        memcpy(pSn, pFactory->u8Sn, sizeof(pFactory->u8Sn));
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->工厂参数区--->读取 u8DeviceCode 字段
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool sEepromAppGetBlockFactoryDevCode(char *pCode, i32 i32MaxLen)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pCode != NULL) && (i32MaxLen >= sizeof(pFactory->u8DeviceCode)))
    {
        memset(pCode, 0, i32MaxLen);
        memcpy(pCode, pFactory->u8DeviceCode, sizeof(pFactory->u8DeviceCode));
        
        return(true);
    }
    
    return(false);
}







/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->工厂参数区--->读取 u8DeviceName 字段
* Author                                :   Hall
* modified Date                         :   2024-05-07
* notice                                :   
***********************************************************************************************/
bool sEepromAppGetBlockFactoryDevName(char *pName, i32 i32MaxLen)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pName != NULL) && (i32MaxLen >= sizeof(pFactory->u8DeviceName)))
    {
        memset(pName, 0, i32MaxLen);
        memcpy(pName, pFactory->u8DeviceName, sizeof(pFactory->u8DeviceName));
        
        return(true);
    }
    
    return(false);
}






bool sEepromAppGetBlockFactoryUiMtPsd(char *pPsd, i32 i32MaxLen)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pPsd != NULL) && (i32MaxLen >= sizeof(pFactory->u8UiMtPsd)))
    {
        memset(pPsd, 0, i32MaxLen);
        memcpy(pPsd, pFactory->u8UiMtPsd, sizeof(pFactory->u8UiMtPsd));
        
        return(true);
    }
    
    return(false);
}







eTimeZone_t sEepromAppGetBlockFactoryTimeZone(void)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    return pFactory->eTimeZone;
}



u8 sEepromAppGetBlockFactoryFormatCnt(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8FormatCnt;
}



bool sEepromAppGetBlockFactoryIsoChkFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bIsoChkFlag;
}




bool sEepromAppGetBlockFactoryNoBmsFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bNoBmsFlag;
}




bool sEepromAppGetBlockFactoryElockFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bElockFlag;
}




bool sEepromAppGetBlockFactoryDoorFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bDoorFlag;
}




bool sEepromAppGetBlockFactoryAuthenFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bAuthenFlag;
}




bool sEepromAppGetBlockFactoryProtocolFlag(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].bProtocolFlag;
}




u8 sEepromAppGetBlockFactoryModNum(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8ModNum;
}




u8 sEepromAppGetBlockFactoryModType(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8ModType;
}




u8 sEepromAppGetBlockFactoryMeterType(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8MeterType;
}




u8 sEepromAppGetBlockFactoryTgunHighMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8TgunHighMax;
}




u8 sEepromAppGetBlockFactorySocMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8SocMax;
}




u16 sEepromAppGetBlockFactoryUdcOutMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16UdcOutMax;
}




u16 sEepromAppGetBlockFactoryIdcOutMax(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16IdcOutMax;
}




u16 sEepromAppGetBlockFactoryUreqNoBms(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16UreqNoBms;
}




u16 sEepromAppGetBlockFactoryIreqNoBms(void)
{
    return stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16IreqNoBms;
}




























bool sEepromAppSetBlockFactoryChargerType(ePrivDrvChargerType_t eType)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eChargerType = eType;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryPayAt(ePrivDrvPayAt_t ePayAt)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePayAt = ePayAt;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryMoneyCalcBy(ePrivDrvMoneyCalcBy_t eMoneyCalcBy)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eMoneyCalcBy = eMoneyCalcBy;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryDisableFlag(ePrivDrvChargerDisableFlag_t eFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eDisableFlag = eFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryPowerMode(ePrivDrvStartPowerMode_t eMode)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePowerMode = eMode;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryWorkMode(ePrivDrvChargerWorkMode_t eMode)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eWorkMode = eMode;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryBalancePhase(ePrivDrvBalancePhase_t ePhase)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].ePhase = ePhase;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryBookingViaBle(ePrivDrvBookingViaBle_t eBookingViaBle)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eBookingViaBle = eBookingViaBle;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryRebootReason(ePrivDrvRebootReason_t eReason)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].eReason = eReason;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}






bool sEepromAppSetBlockFactoryResFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bResFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryPowerMax(f32 f32PowerMax)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32PowerMax = f32PowerMax;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryCurrMax(f32 f32CurrMax)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrMax = f32CurrMax;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryPowerRated(f32 f32PowerRated)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32PowerRated = f32PowerRated;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryCurrRated(f32 f32CurrRated)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrRated = f32CurrRated;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryCurrLimitByGun(f32 f32CurrLimitByGun)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].f32CurrLimitByGun = f32CurrLimitByGun;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}

















bool sEepromAppSetBlockFactorySn(const char *pSn, i32 i32Len)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pSn != NULL) && (i32Len <= sizeof(pFactory->u8Sn)))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
        memset(pFactory->u8Sn, 0, sizeof(pFactory->u8Sn));
        memcpy(pFactory->u8Sn, pSn, i32Len);
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
        
        return(true);
    }
    
    return(false);
}





bool sEepromAppSetBlockFactoryDevCode(const char *pCode, i32 i32Len)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pCode != NULL) && (i32Len <= sizeof(pFactory->u8DeviceCode)))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
        memset(pFactory->u8DeviceCode, 0, sizeof(pFactory->u8DeviceCode));
        memcpy(pFactory->u8DeviceCode, pCode, i32Len);
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
        
        return(true);
    }
    
    return(false);
}





bool sEepromAppSetBlockFactoryDevName(const char *pName, i32 i32Len)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pName != NULL) && (i32Len <= sizeof(pFactory->u8DeviceName)))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
        memset(pFactory->u8DeviceName, 0, sizeof(pFactory->u8DeviceName));
        memcpy(pFactory->u8DeviceName, pName, i32Len);
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
        
        return(true);
    }
    
    return(false);
}





bool sEepromAppSetBlockFactoryUiMtPsd(const char *pPsd,  i32 i32Len)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((pPsd != NULL) && (i32Len <= sizeof(pFactory->u8UiMtPsd)))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
        memset(pFactory->u8UiMtPsd, 0, sizeof(pFactory->u8UiMtPsd));
        memcpy(pFactory->u8UiMtPsd, pPsd, i32Len);
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
        
        return(true);
    }
    
    return(false);
}








bool sEepromAppSetBlockFactoryTimeZone(eTimeZone_t eTimeZone)
{
    stEepromAppBlockFactoryInfo_t *pFactory = &stEepromAppCache.stFactory[eEepromAppBlockFactory0];
    
    if((eTimeZone < eTimeZoneUtcMax) && (eTimeZone > eTimeZoneUtcMin))
    {
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
        pFactory->eTimeZone = eTimeZone;
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
        
        return(true);
    }
    
    return(false);
}





bool sEepromAppSetBlockFactoryFormatCnt(u8 u8Cnt)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8FormatCnt = u8Cnt;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryIsoChkFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bIsoChkFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}





bool sEepromAppSetBlockFactoryNoBmsFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bNoBmsFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}





bool sEepromAppSetBlockFactoryElockFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bElockFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryDoorFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bDoorFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryAuthenFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bAuthenFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




bool sEepromAppSetBlockFactoryProtocolFlag(bool bFlag)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].bProtocolFlag = bFlag;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u8 sEepromAppSetBlockFactoryModNum(u8 u8ModNum)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8ModNum = u8ModNum;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u8 sEepromAppSetBlockFactoryModType(u8 u8ModType)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8ModType = u8ModType;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u8 sEepromAppSetBlockFactoryMeterType(u8 u8MeterType)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8MeterType = u8MeterType;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u8 sEepromAppSetBlockFactoryTgunHighMax(u8 u8TgunHighMax)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8TgunHighMax = u8TgunHighMax;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u8 sEepromAppSetBlockFactorySocMax(u8 u8SocMax)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u8SocMax = u8SocMax;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u16 sEepromAppSetBlockFactoryUdcOutMax(u16 u16Udc)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16UdcOutMax = u16Udc;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u16 sEepromAppSetBlockFactoryIdcOutMax(u16 u16Idc)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16IdcOutMax = u16Idc;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u16 sEepromAppSetBlockFactoryUreqNoBms(u16 u16Ureq)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16UreqNoBms = u16Ureq;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}




u16 sEepromAppSetBlockFactoryIreqNoBms(u16 u16Ireq)
{
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockFactory], portMAX_DELAY);
    stEepromAppCache.stFactory[eEepromAppBlockFactory0].u16IreqNoBms = u16Ireq;
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockFactory]);
    
    return(true);
}
























