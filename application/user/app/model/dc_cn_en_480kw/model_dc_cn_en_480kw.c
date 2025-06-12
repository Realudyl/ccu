/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   model_dc_cn_en_480kw.c
* Description                           :   国标480kw分体桩机型代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-26
* notice                                :   
****************************************************************************************************/
#include "model_dc_cn_en_480kw.h"








//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "model_dc_cn_en_480kw";











bool sModelInit_dc_cn_en_480k(void);
void sModelInit_dc_cn_en_480k_eeprom(void);














/***************************************************************************************************
* Description                           :   充电桩用户程序初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   
****************************************************************************************************/
bool sModelInit_dc_cn_en_480k(void)
{
    u8   u8Addr;
    
    
    //故障告警检查组件初始化
    sFaultChkInit();
    
    //gpio初始化
    sIoAppInit();
    u8Addr = sIoGetSwitchAddr();                                                //先获取地址
    sIoAppReInitForMux();                                                       //然后重新初始化
    sIoSetGatePower(RESET, 500, SET, 500);                                      //给网关供电---带复位
    EN_SLOGI(TAG, "拨码地址:%d  超充标志:%d", u8Addr, cModelSuperChargerFlag);
    sFaultChkRegister(sIoAppFaultCheck_CDO);                                    //由于前面有重新初始化操作,
    sFaultChkRegister(sIoAppFaultCheck_ESBP);                                   //所以故障逻辑注册放在后面执行---防止意外触发故障
    sFaultChkRegister(sIoAppFaultCheck_SPD);
    sFaultChkRegister(sIoAppFaultCheck_DCF);
    sFaultChkRegister(sIoAppFaultCheck_CAE);
    
    //PWM初始化
    sIoPwmAppInit();
    
    //Led灯初始化
    sLedAppOpen(cModelSuperChargerFlag);
    
    //计量计费组件初始化
    sOrderInit();
    
    //eeprom用户程序初始化
    sEepromAppInit(sModelInit_dc_cn_en_480k_eeprom);
    sRtcSetTimeZone(sEepromAppGetBlockFactoryTimeZone());
    sOrderSetEleLoss(sEepromAppGetBlockOperatorEleLoss());
    
    //内网通讯资源初始化
    sPrivDrvInit(ePrivDrvHwTypeCom);
    
    //协议栈初始化
    tcpip_init(NULL, NULL);
    
    //以太网初始化
    sEnetAppOpen(0, false, NULL, NULL);
    
    //看门狗初始化
    sFwdog_Init();
    
    
    //采样初始化
    sAdcAppSampleInit();
    sAdcAppSampleSetMuxType(ePileGunIndex0, eAdcPinMuxTypeGbt);
    sAdcAppSampleSetMuxType(ePileGunIndex1, eAdcPinMuxTypeGbt);
    
    //模块电源管理用户代码初始化
    sModAppOptInit(eModAppTypePcu, 0, u8Addr, 0,cModelSuperChargerFlag);
    
    //电表初始化
    sMeterDrvOptInit(eMeterTypeEnlytaDjsf2669d);
    sFaultChkRegister(sMeterDrvOptFaultCheck_MCF);
    
    
    //卡数据管理组件初始化
    sCardAppInit();
    sFaultChkRegister(sCardAppFaultCheck_CRF);
    
    
    
#if defined(DC480K_G2SF311_600)
    //液冷机用户程序初始化
    sCoolingAppInit();
#endif
    
    //绝缘检测初始化
    sIsoCheckInit(sModAppOptOpen, sModAppOptClose, sModAppOptSetOutput);
    
    //国标充电桩用户代码初始化
    sPileCnInit(cModelSuperChargerFlag, sEepromAppGetBlockFactoryCurrLimitByGun());
    
    return true;
}










/***************************************************************************************************
* Description                           :   国标480KW分体桩 eeprom参数初始化函数
* Author                                :   Hall
* Creat Date                            :   2024-07-26
* notice                                :   
****************************************************************************************************/
void sModelInit_dc_cn_en_480k_eeprom(void)
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
    if(cModelSuperChargerFlag == true)                                          //液冷终端
    {
        sEepromAppSetBlockFactoryPowerMax(480000);
        sEepromAppSetBlockFactoryCurrMax(600);
        sEepromAppSetBlockFactoryPowerRated(480000);
        sEepromAppSetBlockFactoryCurrRated(600);
        sEepromAppSetBlockFactoryCurrLimitByGun(600);
        sEepromAppSetBlockFactoryDevName("DC480K-G2SF311-600", strlen("DC480K-G2SF311-600"));
        sEepromAppSetBlockFactoryIdcOutMax(600);
    }
    else                                                                        //双枪---风冷终端
    {
        sEepromAppSetBlockFactoryPowerMax(250000);
        sEepromAppSetBlockFactoryCurrMax(250);
        sEepromAppSetBlockFactoryPowerRated(250000);
        sEepromAppSetBlockFactoryCurrRated(250);
        sEepromAppSetBlockFactoryCurrLimitByGun(250);
        sEepromAppSetBlockFactoryDevName("DC480K-G2SF312-250", strlen("DC480K-G2SF312-250"));
        sEepromAppSetBlockFactoryIdcOutMax(250);
    }
    
    sEepromAppSetBlockFactoryTimeZone(eTimeZoneUtcPos8);                        //北京时间东八区
    sEepromAppSetBlockFactoryFormatCnt(0);
    sEepromAppSetBlockFactoryIsoChkFlag(true);                                  //使能ISO绝缘检测
    sEepromAppSetBlockFactoryNoBmsFlag(false);
    sEepromAppSetBlockFactoryElockFlag(true);
    sEepromAppSetBlockFactoryDoorFlag(true);
    sEepromAppSetBlockFactoryAuthenFlag(false);
    sEepromAppSetBlockFactoryProtocolFlag(true);
    sEepromAppSetBlockFactoryModNum(12);                                        //此字段在480kw CCU上无用
    sEepromAppSetBlockFactoryModType(eModTypeWinline);                          //此字段在480kw CCU上无用
    sEepromAppSetBlockFactoryMeterType(eMeterTypeEnlytaDjsf2669d);
    sEepromAppSetBlockFactoryTgunHighMax(100);
    sEepromAppSetBlockFactorySocMax(100);
    sEepromAppSetBlockFactoryUdcOutMax(1000);
    sEepromAppSetBlockFactoryUreqNoBms(500);
    sEepromAppSetBlockFactoryIreqNoBms(50);
}




































