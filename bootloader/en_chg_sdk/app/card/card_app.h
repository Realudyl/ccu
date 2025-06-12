/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_app.h
* Description                           :   刷卡数据管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :   
****************************************************************************************************/
#ifndef _card_app_h_
#define _card_app_h_
#include "private_drv_opt.h"
#include "card_app_en.h"
#include "card_app_fault.h"







//读卡器断线判断时间 单位：s
#define cCardReaderErrTimeCheck         (30)                                    







//卡数据 结构定义
typedef struct
{
    //卡自身数据
    ePrivDrvCardType_t                  eType;                                  //卡类型
    u32                                 u32Mefare1Sn;                           //Mefare1 卡序列号
    u32                                 u32Balance;                             //余额--------------单位0.01元
    bool                                bOnlineFlag;                            //联网卡标志--------true:联网卡，false:离网卡
    bool                                bUnlockFlag;                            //未锁卡标志--------true:未锁卡，false:已锁卡
    u8                                  u8CardNo[8];                            //卡号--------------自定义卡号
    u8                                  u8Sn[cCardAppEnBlockLen];               //充电桩SN码
    
    //附加数据
    ePileGunIndex_t                     eGunIndex;                              //本次刷卡对应的枪号
    ePrivDrvStartTransType_t            eStartType;                             //启动类型
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    stTime_t                            stTime;                                 //刷卡时间
    
}stCardData_t;




















//刷卡数据管理 缓存结构定义
typedef struct
{
    bool                                bReadFlag;                              //读数据标志
    bool                                bReadedFlag;                            //读数据成功标志
    stCardData_t                        stData;                                 //卡数据
    bool                                bCardReaderBreakline;                   //读卡器异常
    u32                                 u32ReadTimeStamp;                       //开始读卡的时间戳
    u16                                 u16ReadCardTimeOut;                      //读卡超时时间
}stCardAppCache_t;











extern bool sCardAppInit(void);
extern void sCardAppStop(void);
extern void sCardAppStartForStop(void);
extern void sCardAppStartForStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param);

extern stCardData_t *sCardAppGetData(void);
extern void sCardAppSetDataClear(void);

extern void sCardReaderBreaklineCheck(void);

#endif




















