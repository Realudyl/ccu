/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_opt.h
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-01
* notice                                :     
****************************************************************************************************/
#ifndef _private_drv_opt_H_
#define _private_drv_opt_H_


#include "en_common.h"
#include "private_drv_op_m.h"
#include "private_drv_op_s.h"
#include "private_drv_com.h"
#include "private_drv_basic.h"
#include "private_drv_02a2.h"
#include "private_drv_02a2_30.h"
#include "private_drv_03a3.h"
#include "private_drv_04a4.h"
#include "private_drv_05a5.h"
#include "private_drv_06a6.h"
#include "private_drv_07a7.h"
#include "private_drv_08a8.h"
#include "private_drv_09a9.h"
#include "private_drv_0aaa.h"
#include "private_drv_0bab.h"
#include "private_drv_0bab_30.h"
#include "private_drv_0bab_update.h"
#include "private_drv_0cac.h"
#include "private_drv_0cac_update.h"
#include "private_drv_0dad.h"
#include "private_drv_0eae.h"
#include "private_drv_11b1.h"
#include "private_drv_13b3.h"
#include "private_drv_14b4.h"
#include "private_drv_15b5.h"
#include "private_drv_16b6.h"
#include "private_drv_shell.h"
#include "private_drv_api_m.h"
#include "private_drv_api_s.h"
#include "private_drv_frame_def.h"







//内网私有协议组件日志等级定义
#ifndef cSdkLogLevel
#define cPrivDrvLogLevel                (ESP_LOG_DEBUG)
#else
#define cPrivDrvLogLevel                (cSdkLogLevel)
#endif




//桩端通讯地址默认值
#ifndef cSdkPrivDrvAddrDefault
#define cPrivDrvAddrDefault             (0x0001)
#else
#define cPrivDrvAddrDefault             (cSdkPrivDrvAddrDefault)
#endif





//桩端通讯Pkt超时使能
#ifndef cSdkPrivDrvPktTimeOutEn
#define cPrivDrvPktTimeOutEn            (1)
#else
#define cPrivDrvPktTimeOutEn            (cSdkPrivDrvPktTimeOutEn)
#endif



//桩端通讯Pkt超时时间(Pkt超时使能才有效)
#ifndef cSdkPrivDrvPktTimeOutSec
#define cPrivDrvPktTimeOutSec           (5)
#else
#define cPrivDrvPktTimeOutSec           (cSdkPrivDrvPktTimeOutSec)
#endif



//桩端通讯Pkt打印使能
#ifndef cSdkPrivDrvPrintfEn
#define cPrivDrvPktPrintfEn             (0)
#else
#define cPrivDrvPktPrintfEn             (cSdkPrivDrvPrintfEn)
#endif









//串口数据发送任务参数
#ifndef cSdkPrivDrvSendTaskStackSize
#define cPrivDrvSendTaskStackSize       (3 * 1024)
#else
#define cPrivDrvSendTaskStackSize       (cSdkPrivDrvSendTaskStackSize)
#endif

#ifndef cSdkPrivDrvSendTaskPrio
#define cPrivDrvSendTaskPrio            (23)
#else
#define cPrivDrvSendTaskPrio            (cSdkPrivDrvSendTaskPrio)
#endif







//08/A8/0E/AE/13/B3报文 打印周期
#ifndef cSdkPrivDrvPrintCycle
#define cPrivDrvPrintCycle              (10)
#else
#define cPrivDrvPrintCycle              (cSdkPrivDrvPrintCycle)
#endif











//内网协议物理层硬件类型
typedef enum
{
    ePrivDrvHwTypeCom                   = 0,                                    //串口
    ePrivDrvHwTypeCan                   = 1,                                    //CAN
    
    ePrivDrvHwTypeMax
}__attribute__((packed)) ePrivDrvHwType_t;




//内网私有协议驱动功能map
typedef struct 
{
    //硬件类型
    ePrivDrvHwType_t                    eType;
    
    //描述字符串
    const char                          *pString;
    
    //初始化函数
    bool                                (*pInit)(void);
    
    //数据发送函数
    bool                                (*pSend)(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag);
    
}stPrivDrvOptMap_t;

















//内网私有协议报文类型 map 结构定义
typedef struct 
{
    ePrivDrvCmd_t                       eCmd;                                   //报文类型
    bool                                (*pRecv)(const u8 *pBuf, i32 i32Len);   //该报文的接收处理函数
    SemaphoreHandle_t                   hAckMutex;                              //该报文的接收信号量句柄
    bool                                bProcRst;                               //该报文的数据处理结果
    u32                                 u32WaitTime;                            //该报文的最大等待时间 单位：ms 为0表示该报文信号量无需创建和检查
    const char                          *pString;                               //该报文类型描述字符串
}stPrivDrvCmdMap_t;


























//串口驱动缓存结构
typedef struct
{
    //给串口接收任务的队列句柄
    QueueHandle_t                       hUartQueue;
    
    
    //收发缓存
    u8                                  u8RxBuf[cPrivDrvBufSize];               //    接收 buf
    u8                                  u8AckTxBuf[cPrivDrvBufSize];            //主动发送 buf
    u8                                  u8RptTxBuf[cPrivDrvBufSize];            //应答发送 buf
    SemaphoreHandle_t                   hAckTxBufMutex;                         //主动发送 buf 保护锁
    SemaphoreHandle_t                   hRptTxBufMutex;                         //应答发送 buf 保护锁
    
    
    //断帧处理资源
    u8                                  u8Pkt[cPrivDrvBufSize];
    i32                                 i32PktIndex;
    u32                                 u32PktHeadTime;                         //记录本帧收到head的时间戳
    
    
    //各周期性报文上报时间戳
    u32                                 u32Time02;
    u32                                 u32Time03[cPrivDrvGunNumMax + 1];
    u32                                 u32Time04;
    u32                                 u32Time08[cPrivDrvGunNumMax + 1];
    u32                                 u32Time0E[cPrivDrvGunNumMax + 1];
    u32                                 u32Time13[cPrivDrvGunNumMax + 1];
    
    //启动充电的时间戳
    u32                                 u32TimeStartCharging[cPrivDrvGunNumMax + 1];
    
    //几个报文的打印时间戳
    u32                                 u32Time08Print[cPrivDrvGunNumMax + 1];
    u32                                 u32Time0EPrint[cPrivDrvGunNumMax + 1];
    u32                                 u32Time13Print[cPrivDrvGunNumMax + 1];
    u32                                 u32Time15Print;
    u32                                 u32Time16Print;
    
    bool                                bPkt08EnableFlag[cPrivDrvGunNumMax + 1];//08报文发送使能标志
    bool                                bPkt0EEnableFlag[cPrivDrvGunNumMax + 1];//0E报文发送使能标志
    bool                                bPkt13EnableFlag[cPrivDrvGunNumMax + 1];//13报文发送使能标志
    
    
    //通讯超时故障资源
    u32                                 u32TimeA4;                              //最近一次收到A4报文的时间戳
    bool                                bCmdA3Flag;                             //收到A3报文的标志
    bool                                bTimeoutFlag;                           //超时故障置位标志
    
    //从内网协议文本中抽象出来的数据缓存 供给上层读写使用
    unPrivDrvData_t                     unData;
    
    
}stPrivDrvCache_t;






















//初始化
extern bool sPrivDrvInit(ePrivDrvHwType_t eType);

//map操作
extern i32  sPrivDrvCmdSearch(ePrivDrvCmd_t eCmd);

//发送处理相关函数
extern void sPrivDrvSetHead30(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len);
extern void sPrivDrvSetHead(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len);
extern void sPrivDrvSetCrc(u8 *pBuf, u16 u16Len);
extern bool sPrivDrvSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag);


//接收处理相关函数
extern void sPrivDrvPktCheck(const u8 *pBuf, i32 i32Len);
extern void sPrivDrvPktCheckTimeout(void);
extern void sPrivDrvPktRecvRstSet(ePrivDrvCmd_t eCmd, bool bRst);
extern void sPrivDrvPktRecvReset(void);









#endif

























