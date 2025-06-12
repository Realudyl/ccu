/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_factory.h
* Description                           :   eeprom芯片用户程序实现 之 来自工厂的配置参数结构
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-25
* notice                                :   
****************************************************************************************************/
#ifndef _eeprom_app_factory_H_
#define _eeprom_app_factory_H_
#include "private_drv_opt.h"
#include "eeprom_app_factory_data_def.h"











//EEPROM 工厂配置参数 的类型
//工厂参数有存储区和备份区, 外部读写api函数一律只对存储区操作
//备份区仅在存储区校验值错误时用于恢复存储区内容。
typedef enum
{
    eEepromAppBlockFactory0             = 0,                                    //工厂配置参数存储区
    eEepromAppBlockFactory1             = 1,                                    //工厂配置参数备份区
    
    eEepromAppBlockFactoryMax
}eEepromAppBlockFactory_t;






































//来自工厂的出厂参数结构定义
typedef struct
{
    u16                                 u16CheckCrc;                            //校验值
    ePrivDrvChargerType_t               eChargerType;                           //充电桩类型
    ePrivDrvPayAt_t                     ePayAt;                                 //结算时机
    ePrivDrvMoneyCalcBy_t               eMoneyCalcBy;                           //计费端
    ePrivDrvChargerDisableFlag_t        eDisableFlag;                           //桩禁用
    ePrivDrvStartPowerMode_t            ePowerMode;                             //功率分配启动模式
    ePrivDrvChargerWorkMode_t           eWorkMode;                              //桩工作模式
    ePrivDrvBalancePhase_t              ePhase;                                 //三相平衡挂载相位
    ePrivDrvBookingViaBle_t             eBookingViaBle;                         //是否支持蓝牙预约
    ePrivDrvRebootReason_t              eReason;                                //重启原因
    bool                                bResFlag;                               //电阻负载标志---为true时表示接电阻负载,测试用
    f32                                 f32PowerMax;                            //最大功率
    f32                                 f32CurrMax;                             //最大电流
    f32                                 f32PowerRated;                          //额定功率
    f32                                 f32CurrRated;                           //额定电流
    f32                                 f32CurrLimitByGun;                      //枪线最大允许电流
    
    
    
    char                                u8Sn[cPrivDrvLenSn];                    //SN号码
    char                                u8DeviceCode[cPrivDrvLenDevCode];       //设备型号代码
    char                                u8DeviceName[cPrivDrvLenDevName];       //设备名称
    char                                u8UiMtPsd[8];                           //运维界面密码
    
    
    eTimeZone_t                         eTimeZone;                              //时区
    u8                                  u8FormatCnt;                            //文件系统格式化次数
    
    bool                                bIsoChkFlag;                            //iso绝缘检测使能开关
    bool                                bNoBmsFlag;                             //无bms充电开关
    bool                                bElockFlag;                             //枪锁开关
    bool                                bDoorFlag;                              //门磁开关
    bool                                bAuthenFlag;                            //认证时序开关
    bool                                bProtocolFlag;                          //协议一致性开关
    
    u8                                  u8ModNum;                               //个数
    u8                                  u8ModType;                              //模块类型
    u8                                  u8MeterType;                            //电表类型
    u8                                  u8TgunHighMax;                          //枪温保护上限 1℃
    u8                                  u8SocMax;                               //停充soc值    1%
    u16                                 u16UdcOutMax;                           //输出电压上限 1V
    u16                                 u16IdcOutMax;                           //输出电流上限 1A
    u16                                 u16UreqNoBms;                           //无bms充电时需求电压 1V
    u16                                 u16IreqNoBms;                           //无bms充电时需求电流 1A
    
    u8                                  u8Rsvd[512];                            //预留字段
    
}__attribute__((aligned(2))) stEepromAppBlockFactoryInfo_t;

//u16CheckCrc 字段 size
#define cEepromAppBlockFactoryCrcSize   (sizeof(u16))




























extern void sEepromAppSetBlockFactoryDefault(void);




extern ePrivDrvChargerType_t sEepromAppGetBlockFactoryChargerType(void);
extern ePrivDrvPayAt_t sEepromAppGetBlockFactoryPayAt(void);
extern ePrivDrvMoneyCalcBy_t sEepromAppGetBlockFactoryMoneyCalcBy(void);
extern ePrivDrvChargerDisableFlag_t sEepromAppGetBlockFactoryDisableFlag(void);
extern ePrivDrvStartPowerMode_t sEepromAppGetBlockFactoryPowerMode(void);
extern ePrivDrvChargerWorkMode_t sEepromAppGetBlockFactoryWorkMode(void);
extern ePrivDrvBalancePhase_t sEepromAppGetBlockFactoryBalancePhase(void);
extern ePrivDrvBookingViaBle_t sEepromAppGetBlockFactoryBookingViaBle(void);
extern ePrivDrvRebootReason_t sEepromAppGetBlockFactoryRebootReason(void);
extern bool sEepromAppGetBlockFactoryResFlag(void);
extern f32  sEepromAppGetBlockFactoryPowerMax(void);
extern f32  sEepromAppGetBlockFactoryCurrMax(void);
extern f32  sEepromAppGetBlockFactoryPowerRated(void);
extern f32  sEepromAppGetBlockFactoryCurrRated(void);
extern f32  sEepromAppGetBlockFactoryCurrLimitByGun(void);
extern bool sEepromAppGetBlockFactorySn(char *pSn, i32 i32MaxLen);
extern bool sEepromAppGetBlockFactoryDevCode(char *pCode, i32 i32MaxLen);
extern bool sEepromAppGetBlockFactoryDevName(char *pName, i32 i32MaxLen);
extern bool sEepromAppGetBlockFactoryUiMtPsd(char *pPsd,  i32 i32MaxLen);
extern eTimeZone_t sEepromAppGetBlockFactoryTimeZone(void);
extern u8   sEepromAppGetBlockFactoryFormatCnt(void);
extern bool sEepromAppGetBlockFactoryIsoChkFlag(void);
extern bool sEepromAppGetBlockFactoryNoBmsFlag(void);
extern bool sEepromAppGetBlockFactoryElockFlag(void);
extern bool sEepromAppGetBlockFactoryDoorFlag(void);
extern bool sEepromAppGetBlockFactoryAuthenFlag(void);
extern bool sEepromAppGetBlockFactoryProtocolFlag(void);
extern u8   sEepromAppGetBlockFactoryModNum(void);
extern u8   sEepromAppGetBlockFactoryModType(void);
extern u8   sEepromAppGetBlockFactoryMeterType(void);
extern u8   sEepromAppGetBlockFactoryTgunHighMax(void);
extern u8   sEepromAppGetBlockFactorySocMax(void);
extern u16  sEepromAppGetBlockFactoryUdcOutMax(void);
extern u16  sEepromAppGetBlockFactoryIdcOutMax(void);
extern u16  sEepromAppGetBlockFactoryUreqNoBms(void);
extern u16  sEepromAppGetBlockFactoryIreqNoBms(void);

extern bool sEepromAppSetBlockFactoryChargerType(ePrivDrvChargerType_t eType);
extern bool sEepromAppSetBlockFactoryPayAt(ePrivDrvPayAt_t ePayAt);
extern bool sEepromAppSetBlockFactoryMoneyCalcBy(ePrivDrvMoneyCalcBy_t eMoneyCalcBy);
extern bool sEepromAppSetBlockFactoryDisableFlag(ePrivDrvChargerDisableFlag_t eFlag);
extern bool sEepromAppSetBlockFactoryPowerMode(ePrivDrvStartPowerMode_t eMode);
extern bool sEepromAppSetBlockFactoryWorkMode(ePrivDrvChargerWorkMode_t eMode);
extern bool sEepromAppSetBlockFactoryBalancePhase(ePrivDrvBalancePhase_t ePhase);
extern bool sEepromAppSetBlockFactoryBookingViaBle(ePrivDrvBookingViaBle_t eBookingViaBle);
extern bool sEepromAppSetBlockFactoryRebootReason(ePrivDrvRebootReason_t eReason);
extern bool sEepromAppSetBlockFactoryResFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryPowerMax(f32 f32PowerMax);
extern bool sEepromAppSetBlockFactoryCurrMax(f32 f32CurrMax);
extern bool sEepromAppSetBlockFactoryPowerRated(f32 f32PowerRated);
extern bool sEepromAppSetBlockFactoryCurrRated(f32 f32CurrRated);
extern bool sEepromAppSetBlockFactoryCurrLimitByGun(f32 f32CurrLimitByGun);
extern bool sEepromAppSetBlockFactorySn(const char *pSn, i32 i32Len);
extern bool sEepromAppSetBlockFactoryDevCode(const char *pCode, i32 i32Len);
extern bool sEepromAppSetBlockFactoryDevName(const char *pName, i32 i32Len);
extern bool sEepromAppSetBlockFactoryUiMtPsd(const char *pPsd,  i32 i32Len);
extern bool sEepromAppSetBlockFactoryTimeZone(eTimeZone_t eTimeZone);
extern bool sEepromAppSetBlockFactoryFormatCnt(u8 u8Cnt);
extern bool sEepromAppSetBlockFactoryIsoChkFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryNoBmsFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryElockFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryDoorFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryAuthenFlag(bool bFlag);
extern bool sEepromAppSetBlockFactoryProtocolFlag(bool bFlag);
extern u8   sEepromAppSetBlockFactoryModNum(u8 u8ModNum);
extern u8   sEepromAppSetBlockFactoryModType(u8 u8ModType);
extern u8   sEepromAppSetBlockFactoryMeterType(u8 u8MeterType);
extern u8   sEepromAppSetBlockFactoryTgunHighMax(u8 u8TgunHighMax);
extern u8   sEepromAppSetBlockFactorySocMax(u8 u8SocMax);
extern u16  sEepromAppSetBlockFactoryUdcOutMax(u16 u16Udc);
extern u16  sEepromAppSetBlockFactoryIdcOutMax(u16 u16Idc);
extern u16  sEepromAppSetBlockFactoryUreqNoBms(u16 u16Ureq);
extern u16  sEepromAppSetBlockFactoryIreqNoBms(u16 u16Ireq);





#endif
















