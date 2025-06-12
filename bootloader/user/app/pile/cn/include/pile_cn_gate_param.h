/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_param.h
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 参数管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_gate_param_h_
#define _pile_cn_gate_param_h_


















//参数管理映射结构体定义
typedef struct
{
    //参数地址
    ePrivDrvParamAddr_t                 eAddr;
    
    //该地址的处理函数
    bool                                (*pProc)(const stPrivDrvCmdAA_t *pCmdAa);
    
    //该参数功能描述
    const char                          *pString;                               //该报文类型描述字符串
    
}sPrivDrvParamMap_t;













































//参数地址表:67--->使用此地址传输 网关对桩端设置设备编码
typedef struct 
{
    char                                   u8PileDeviceType[32];                  //桩端设备类型
    char                                   u8GateDeviceType[32];                  //网关设备类型
    
}__attribute__((packed)) stPrivDrvParamDeviceType_t;





//参数地址表:80--->使用此地址传输 网关对桩端设置开关使能
typedef struct
{
    u16                                  bCertificatsequence  : 1;              //认证时序  （1开启， 0关闭）
    u16                                  bElock               : 1;              //电子锁    （1开启， 0关闭）
    u16                                  bRsvd1               : 1;              //预留
    u16                                  bResLoadType         : 1;              //负载类型  （1电阻， 0电池）
    u16                                  bIsoFlag             : 1;              //绝缘检测  （1开启， 0关闭）
    u16                                  bDoorFlag            : 1;              //门禁检测  （1开启， 0关闭）
    u16                                  bprotocolconformance : 1;              //协议一致性（1开启， 0关闭）
    u16                                  bVinMode             : 1;              //Vin码充电 （1开启， 0关闭）
    u16                                  bRsvd2               : 1;              //预留
    u16                                  bRsvd3               : 1;              //预留
    u16                                  bRsvd4               : 1;              //预留
    u16                                  bCardReader          : 1;              //读卡器（1开启， 0关闭）
    u16                                  bRsvd5               : 1;              //预留
    u16                                  bRsvd6               : 1;              //预留
    u16                                  bRsvd7               : 1;              //预留
    u16                                  bRsvd8               : 1;              //预留
    
}__attribute__((packed)) stPrivDrvParamSwitch_t;






//参数地址表:167--->使用此地址传输 网关对桩端发送Vin码启动
typedef struct 
{
    u8                                  u8VinChargeValid;                       //触发Vin码启动（0XAA有效）
    
}__attribute__((packed)) stPrivDrvParamVinStart_t;

//触发Vin码充电有效值
#define cVinChargeValid                 (0xAA)








//参数地址表:192--->使用此地址传输 网关对桩端发送寻卡请求
typedef struct 
{
    ePrivDrvChargingMode_t              eMode;                                  //充电模式
    unPrivDrvChargingParam_t            unParam;                                //充电参数
    u16                                 u16CardAllowt;                          //寻卡开始和结束
    
}__attribute__((packed)) stPrivDrvParamFindCard_t;





//参数地址表:198--->使用此地址传输 网关对桩端发送输出电压
typedef struct 
{
    u32                                 u32UdcMax;                              //充电电压上限
    u8                                  u8Rsvd[4];                              //预留
    
}__attribute__((packed)) stPrivDrvParamUdcOut_t;





//参数地址表:199--->使用此地址传输 网关对桩端发送输出电流
typedef struct 
{
    u32                                 u32IdcMax;                              //充电电流上限
    u8                                  u8Rsvd[4];                              //预留
    
}__attribute__((packed)) stPrivDrvParamIdcOut_t;





//参数地址表:200--->使用此地址传输 网关对桩端发送枪温
typedef struct 
{
    u16                                 u16TgunHighMax;                         //充电枪温上限
    u8                                  u8Rsvd[2];                              //预留
    
}__attribute__((packed)) stPrivDrvParamGunTemp_t;





















extern bool sPileCnGateParamProc(const stPrivDrvCmdAA_t *pCmdAa);





#endif




















