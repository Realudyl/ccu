/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   model_dc_eu_en_40kw.c
* Description                           :   国标480kw分体桩机型代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-26
* notice                                :   
****************************************************************************************************/
#include "model_dc_eu_en_40kw.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "model_dc_eu_en_40kw";











bool sModelInit_dc_eu_en_40k(void);
void sModelInit_dc_eu_en_40k_eeprom(void);














/***************************************************************************************************
* Description                           :   欧标40KW单枪桩用户程序初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sModelInit_dc_eu_en_40k(void)
{
    //GPIO口初始化
    sIoAppInit();
    sIoPwmAppInit();
    
    //采样初始化
    sAdcAppSampleInit();
    sAdcAppSampleSetMuxType(ePileGunIndex0, eAdcPinMuxTypeCcs);
    
    //ISO绝缘检测资源初始化
    sIsoCheckInit(sModAppAvgSetGunOpen, sModAppAvgSetGunClose, sModAppAvgSetGunOutput);
    
    //eeprom用户程序初始化
    sEepromAppInit(sModelInit_dc_eu_en_40k_eeprom);
    sRtcSetTimeZone(sEepromAppGetBlockFactoryTimeZone());
    sOrderSetEleLoss(sEepromAppGetBlockOperatorEleLoss());
    
    //模块电源用户程序初始化--->均充算法，1把枪，1个模块，枪线电流最大允许150A
    sModAppAvgInit(1, 1, sEepromAppGetBlockFactoryCurrLimitByGun());
    
    //电表初始化
    sMeterDrvOptInit(eMeterTypeAcrelDjsf1352rn);
    
    //内网通讯资源初始化
    sPrivDrvInit(ePrivDrvHwTypeCom);
    
    //计量计费组件初始化
    sOrderInit();
    
    //欧标充电桩用户代码初始化
    sPileEuInit();
    
    return(true);
}










/***************************************************************************************************
* Description                           :   欧标40KW直流桩 eeprom参数初始化函数
* Author                                :   Hall
* Creat Date                            :   2024-07-26
* notice                                :   
****************************************************************************************************/
void sModelInit_dc_eu_en_40k_eeprom(void)
{
    char u8DeviceCode[cPrivDrvLenDevCode];
    
    sEepromAppSetBlockFactoryChargerType(ePrivDrvChargerTypeDcThree);           //充电桩类型
    sEepromAppSetBlockFactoryPayAt(ePrivDrvPayAtPlugout);                       //结算时机
    sEepromAppSetBlockFactoryMoneyCalcBy(ePrivDrvMoneyCalcByChg);               //计费端
    sEepromAppSetBlockFactoryDisableFlag(ePrivDrvChargerDisableFlagFalse);      //桩禁用
    sEepromAppSetBlockFactoryPowerMode(ePrivDrvStartPowerModeNormal);           //功率分配启动模式
    sEepromAppSetBlockFactoryWorkMode(ePrivDrvChargerWorkModeNetwork);          //桩工作模式
    sEepromAppSetBlockFactoryBalancePhase(ePrivDrvBalancePhaseAll);             //三相平衡挂载相位
    sEepromAppSetBlockFactoryBookingViaBle(ePrivDrvBookingViaBleFalse);         //是否支持蓝牙预约
    sEepromAppSetBlockFactoryRebootReason(ePrivDrvRebootReasonNormal);          //重启原因
    sEepromAppSetBlockFactoryResFlag(false);                                    //非电阻负载
    
    
    snprintf(u8DeviceCode, sizeof(u8DeviceCode), "%d", 255);
    sEepromAppSetBlockFactoryDevCode(u8DeviceCode, sizeof(u8DeviceCode));       //设备型号代码已经作废，设置"255"即可
    
    sEepromAppSetBlockFactoryPowerMax(40000);
    sEepromAppSetBlockFactoryCurrMax(133);
    sEepromAppSetBlockFactoryPowerRated(40000);
    sEepromAppSetBlockFactoryCurrRated(133);
    sEepromAppSetBlockFactoryCurrLimitByGun(150);
    sEepromAppSetBlockFactoryDevName("DC480K-G2SF311-600", strlen("DC480K-G2SF311-600"));
    sEepromAppSetBlockFactoryTimeZone(eTimeZoneUtcPos8);                        //北京时间东八区
    sEepromAppSetBlockFactoryFormatCnt(0);
    sEepromAppSetBlockFactoryIsoChkFlag(true);                                  //使能ISO绝缘检测
    sEepromAppSetBlockFactoryModNum(1);                                         //
    sEepromAppSetBlockFactoryModType(eModTypeWinline);                          //
    sEepromAppSetBlockFactoryMeterType(eMeterTypeAcrelDjsf1352rn);
    sEepromAppSetBlockFactorySocMax(100);
    sEepromAppSetBlockFactoryUreqNoBms(500);
    sEepromAppSetBlockFactoryIreqNoBms(50);
}



