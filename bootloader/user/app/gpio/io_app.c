/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   io_app.c
* Description                           :   充电桩输入输出信号部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
#include "io_app.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_app";







stIoAppCache_t stIoAppCache;
extern stIoDrvDevice_t stIoDrvDevMap;









bool  sIoAppInit(void);
bool  sIoAppReInitForMux(void);




//--------------------------------------以下是对IO口操作进一步封装的api函数------------------------------
u8    sIoGetSwitchAddr(void);

//与充电枪有关的一些IO口操作api--基于枪序号操作
eIo_t sIoGetDrK(ePileGunIndex_t eGunIndex);
eIo_t sIoGetRelayD1(ePileGunIndex_t eGunIndex);
eIo_t sIoGetRelayD2(ePileGunIndex_t eGunIndex);
eIo_t sIoGetGbBms(ePileGunIndex_t eGunIndex);
eIo_t sIoGetLockFb(ePileGunIndex_t eGunIndex);
eIo_t sIoGetLockOnOff(ePileGunIndex_t eGunIndex);
eIo_t sIoGetLockRelayEn(ePileGunIndex_t eGunIndex);
eIo_t sIoGetRelayIsoDrv1(ePileGunIndex_t eGunIndex);
eIo_t sIoGetRelayIsoDrv2(ePileGunIndex_t eGunIndex);
eIo_t sIoGetRelayIsoDrv3(ePileGunIndex_t eGunIndex);


//几个带反馈检测的IO信号控制函数---需要在控制后检查反馈信号
bool  sIoSetDrK(ePileGunIndex_t eGunIndex, bool bOnOff);
bool  sIoSetDrXyz(u8 u8Xyz, bool bOnOff);

//eIo_GATE_POWER---为网关供电及复位
void  sIoSetGatePower(FlagStatus eStatus1, u16 u16Time1, FlagStatus eStatus2, u16 u16Time2);



















/***************************************************************************************************
* Description                           :   GPIO口初始化
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
bool sIoAppInit(void)
{
    stIoAppCache_t *pIo = &stIoAppCache;
    memset(&stIoAppCache, 0, sizeof(stIoAppCache));
    pIo->eIoDrK[ePileGunIndex0]         = eIo_A_DR_K;
    pIo->eIoRelayD1[ePileGunIndex0]     = eIo_A_D1_RELAY;
    pIo->eIoRelayD2[ePileGunIndex0]     = eIo_A_D2_RELAY;
    pIo->eIoGbBms[ePileGunIndex0]       = eIo_A_GB_BMS;
    pIo->eIoLockFb[ePileGunIndex0]      = eIo_A_LOCK_FB;
    pIo->eIoLockOnOff[ePileGunIndex0]   = eIo_A_LOCK_ON_OFF;
    pIo->eIoLockRelayEn[ePileGunIndex0] = eIo_A_LOCK_RELAY_EN;
    pIo->eIoRelayIsoDrv1[ePileGunIndex0]= eIo_A_ISO_RELAY_DRV1;
    pIo->eIoRelayIsoDrv2[ePileGunIndex0]= eIo_A_ISO_RELAY_DRV2;
    pIo->eIoRelayIsoDrv3[ePileGunIndex0]= eIo_A_ISO_RELAY_DRV3;
    
    pIo->eIoDrK[ePileGunIndex1]         = eIo_B_DR_K;
    pIo->eIoRelayD1[ePileGunIndex1]     = eIo_B_D1_RELAY;
    pIo->eIoRelayD2[ePileGunIndex1]     = eIo_B_D2_RELAY;
    pIo->eIoGbBms[ePileGunIndex1]       = eIo_B_GB_BMS;
    pIo->eIoLockFb[ePileGunIndex1]      = eIo_B_LOCK_FB;
    pIo->eIoLockOnOff[ePileGunIndex1]   = eIo_B_LOCK_ON_OFF;
    pIo->eIoLockRelayEn[ePileGunIndex1] = eIo_B_LOCK_RELAY_EN;
    pIo->eIoRelayIsoDrv1[ePileGunIndex1]= eIo_B_ISO_RELAY_DRV1;
    pIo->eIoRelayIsoDrv2[ePileGunIndex1]= eIo_B_ISO_RELAY_DRV2;
    pIo->eIoRelayIsoDrv3[ePileGunIndex1]= eIo_B_ISO_RELAY_DRV3;
    
    return sIoDrvInitDev();
}









/***************************************************************************************************
* Description                           :   GPIO口初始化---重新初始化
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   以下管脚是复用的，上电时先按 eIo_SITE1、eIo_SITE4 初始化
*                                           读取拨码地址值以后再按输出口初始化并输出高电平
*                                           eIo_SITE1<--->B_CC_PP_RLY_EN
*                                           eIo_SITE4<--->A_CC_PP_RLY_EN
****************************************************************************************************/
bool sIoAppReInitForMux(void)
{
    bool bRst;
    
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinInOutMode      = GPIO_MODE_OUTPUT;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinPullUpDownMode = GPIO_PUPD_PULLUP;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinOutType        = GPIO_OTYPE_PP;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinOutSpeed       = GPIO_OSPEED_60MHZ;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinInPeriod       = 0;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE1].u32PinInFType        = 0;
    
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinInOutMode      = GPIO_MODE_OUTPUT;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinPullUpDownMode = GPIO_PUPD_PULLUP;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinOutType        = GPIO_OTYPE_PP;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinOutSpeed       = GPIO_OSPEED_60MHZ;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinInPeriod       = 0;
    stIoDrvDevMap.stIoDrvParaMap[eIo_SITE4].u32PinInFType        = 0;
    
    bRst = sIoDrvInitDev();
    
    bRst = bRst & sIoDrvSetOutput(eIo_SITE1, SET);                              //初始化为输出口以后全部输出高电平
    bRst = bRst & sIoDrvSetOutput(eIo_SITE4, SET);
    
    return bRst;
}













/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 拨码开关地址值
* Author                                :   Hall
* Creat Date                            :   2024-06-27
* notice                                :   
****************************************************************************************************/
u8 sIoGetSwitchAddr(void)
{
    u8 u8Addr;
    
    u8Addr  = ((sIoDrvGetInput(eIo_SITE1) == SET) ? 1 : 0);
    u8Addr |= ((sIoDrvGetInput(eIo_SITE2) == SET) ? 1 : 0) << 1;
    u8Addr |= ((sIoDrvGetInput(eIo_SITE3) == SET) ? 1 : 0) << 2;
    u8Addr |= ((sIoDrvGetInput(eIo_SITE4) == SET) ? 1 : 0) << 3;
    
    return u8Addr;
}










/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x接触器组驱动
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetDrK(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoDrK[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x复用口 <GBT:辅源+输出继电器使能---CHADEMO:d1继电器使能>
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetRelayD1(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoRelayD1[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x复用口 <GBT:辅源-输出继电器使能---CHADEMO:d2继电器使能>
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetRelayD2(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoRelayD2[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x辅源检测
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetGbBms(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoGbBms[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x枪锁反馈
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetLockFb(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoLockFb[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x枪锁解锁/锁止使能
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetLockOnOff(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoLockOnOff[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 枪x枪锁继电器使能
* Author                                :   Hall
* Creat Date                            :   2024-05-11
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetLockRelayEn(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoLockRelayEn[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 绝缘检测高端光耦使能
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetRelayIsoDrv1(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoRelayIsoDrv1[eGunIndex]);
}






/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 绝缘检测低端光耦使能
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetRelayIsoDrv2(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoRelayIsoDrv2[eGunIndex]);
}








/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:获取IO口序号 之 绝缘检测继电器驱动
* Author                                :   Hall
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
eIo_t sIoGetRelayIsoDrv3(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eIoMax : stIoAppCache.eIoRelayIsoDrv3[eGunIndex]);
}











/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:控制K_A/B接触器驱动并直接返回结果
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   
****************************************************************************************************/
bool sIoSetDrK(ePileGunIndex_t eGunIndex, bool bOnOff)
{
    u8   u8Retry = 0;
    
    while(u8Retry < 3)
    {
        u8Retry++;
        sIoDrvSetOutput(sIoGetDrK(eGunIndex), (bOnOff == true) ? SET : RESET);
        vTaskDelay(300 / portTICK_RATE_MS);
        if(sAdcAppSampleGetFbStateK(eGunIndex) == ((bOnOff == true) ? eAdcAppFbState11 : eAdcAppFbState00))
        {
            return true;
        }
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:控制XYZ接触器驱动并直接返回结果
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   
****************************************************************************************************/
bool sIoSetDrXyz(u8 u8Xyz, bool bOnOff)
{
    u8    u8Retry = 0;
    eIo_t eIo = eIoMax;
    eAdcPin_t eAdcPin = eAdcPinMax;
    
    eIo     = (u8Xyz == 'X') ? eIo_DR_X : eIo;
    eIo     = (u8Xyz == 'Y') ? eIo_DR_Y : eIo;
    eIo     = (u8Xyz == 'Z') ? eIo_DR_Z : eIo;
    eAdcPin = (u8Xyz == 'X') ? eAdcPin_FB_X : eAdcPin;
    eAdcPin = (u8Xyz == 'Y') ? eAdcPin_FB_Y : eAdcPin;
    eAdcPin = (u8Xyz == 'Z') ? eAdcPin_FB_Z : eAdcPin;
    while(u8Retry < 3)
    {
        u8Retry++;
        sIoDrvSetOutput(eIo, (bOnOff == true) ? SET : RESET);
        vTaskDelay(300 / portTICK_RATE_MS);
        if(sAdcAppSampleGetFbState(eAdcPin) == ((bOnOff == true) ? eAdcAppFbState11 : eAdcAppFbState00))
        {
            return true;
        }
    }
    
    return false;
}









/***************************************************************************************************
* Description                           :   GPIO APP层 api接口函数:eIo_GATE_POWER---为网关供电及复位
* Author                                :   Hall
* Creat Date                            :   2024-10-11
* notice                                :   
****************************************************************************************************/
void sIoSetGatePower(FlagStatus eStatus1, u16 u16Time1, FlagStatus eStatus2, u16 u16Time2)
{
    sIoDrvSetOutput(eIo_GATE_POWER, eStatus1);
    vTaskDelay(u16Time1 / portTICK_RATE_MS);
    sIoDrvSetOutput(eIo_GATE_POWER, eStatus2);
    vTaskDelay(u16Time2 / portTICK_RATE_MS);
}

















