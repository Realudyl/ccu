/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mod_drv_winline.h
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#ifndef _mod_drv_winline_h_
#define _mod_drv_winline_h_
#include "mod_drv_winline_frame_def.h"
#include "can_drv_trans.h"











//收发任务周期 单位ms
#ifndef cSdkModDrvRecvTaskCycle
#define cModDrvWinlineRecvTaskCycle     (1)
#else
#define cModDrvWinlineRecvTaskCycle     (cSdkModDrvRecvTaskCycle)
#endif

#ifndef cSdkModDrvSendTaskCycle
#define cModDrvWinlineSendTaskCycle     (1200)
#else
#define cModDrvWinlineSendTaskCycle     (cSdkModDrvSendTaskCycle)
#endif



#ifndef cSdkModDrvRecvTaskPriority
#define cModDrvWinlineRecvTaskPriority     (21)
#else
#define cModDrvWinlineRecvTaskPriority     (cSdkModDrvRecvTaskPriority)
#endif

#ifndef cSdkModDrvSendTaskPriority
#define cModDrvWinlineSendTaskPriority     (22)
#else
#define cModDrvWinlineSendTaskPriority     (cSdkModDrvSendTaskPriority)
#endif



//模块电源输入输出特性曲线定义
#ifndef cSdkModDrvUdcMax                                                        //最大输出电压
#define cModDrvWinlineUdcMax            (1000.0f)
#else
#define cModDrvWinlineUdcMax            (cSdkModDrvUdcMax)
#endif

#ifndef cSdkModDrvUdcMin                                                        //最小输出电压
#define cModDrvWinlineUdcMin            (200.0f)
#else
#define cModDrvWinlineUdcMin            (cSdkModDrvUdcMin)
#endif

#ifndef cSdkModDrvIdcMax                                                        //最大输出电流
#define cModDrvWinlineIdcMax            (133.3f)
#else
#define cModDrvWinlineIdcMax            (cSdkModDrvIdcMax)
#endif

#ifndef cSdkModDrvPdc                                                           //最大输出功率
#define cModDrvWinlinePdc               (40000.0f)
#else
#define cModDrvWinlinePdc               (cSdkModDrvPdc)
#endif








//永联科技 NXR 系列充电模块CAN协议 地址相关定义
#define cModDrvWinlineAddrMaster        (0xf0)                                      //主机地址
#define cModDrvWinlineAddrBd            (0xff)                                      //广播地址
#define cModDrvWinlineAddrBdGroup       (0xfe)                                      //组内广播地址
#define cModDrvWinlineAddrExternGroupMax   (0xfd)                                   //扩展组地址最大值
#define cModDrvWinlineAddrExternGroupMin   (cModDrvWinlineAddrExternGroupMax - 59)  //扩展组地址最小值
//本来协议规定最大地址是0x3f,
//但本驱动设计最大支持16个模块，地址：0~15
#ifndef cModDrvWinlineAddrMin
#define cModDrvWinlineAddrMin           (0x00)                                  //模块地址最小值
#endif
#ifndef cModDrvWinlineAddrMax
#define cModDrvWinlineAddrMax           (0x0f)                                  //模块地址最大值
#endif
#define cModDrvWinlineAddrNumMax        (cModDrvWinlineAddrMax + 1)             //驱动支持的模块最大个数





//定义在线模块查询及在线标志清零时间 秒
#define cModDrvWinlineReadAllAddrTime   (4)
#define cModDrvWinlineExistStsClearTime (15)







//MsgId->PROTNO（似乎是协议号？） 定义
typedef enum 
{
    eModDrvWinlineProtNoDefault         = 0x60,                                 //默认值
    
    eModDrvWinlineProtNoMax             = 0xFFFF                                //确保占据2字节
}__attribute__((packed)) eModDrvWinlineProtNo_t;



//MsgId->PTP 定义
typedef enum 
{
    eModDrvWinlinePtpBd                 = 0,                                    //广播通信
    eModDrvWinlinePtpNormal             = 1,                                    //点对点通信
    
    eModDrvWinlinePtpMax
}__attribute__((packed)) eModDrvWinlinePtp_t;



//MsgId->错误消息帧标志定义
typedef enum 
{
    eModDrvWinlineMsgIdBit29DataFrame   = 0,                                    //数据帧
    eModDrvWinlineMsgIdBit29ErrMsg      = 1,                                    //错误消息
    
    //eModDrvWinlineMsgIdBit29Max
}eModDrvWinlineMsgIdBit29_t;



//MsgId->远程帧帧标志定义
typedef enum 
{
    eModDrvWinlineMsgIdBit30Data        = 0,                                    //数据帧
    eModDrvWinlineMsgIdBit30Rtr         = 1,                                    //远程帧
    
    //eModDrvWinlineMsgIdBit30Max
}eModDrvWinlineMsgIdBit30_t;



//MsgId->扩展帧帧标志定义
typedef enum 
{
    eModDrvWinlineMsgIdBit31Standard    = 0,                                    //标准帧 SFF
    eModDrvWinlineMsgIdBit31Extended    = 1,                                    //扩展帧 EFF
    
    //eModDrvWinlineMsgIdBit31Max
}eModDrvWinlineMsgIdBit31_t;






//MsgId 定义
typedef struct
{
    u32                                 bGroup  : 3;                            //组号
    u32                                 bAddSrc : 8;                            //  源地址
    u32                                 bAddDst : 8;                            //目的地址
    eModDrvWinlinePtp_t                 ePtp    : 1;                            //PTP
    eModDrvWinlineProtNo_t              eProtNo : 9;                            //PROTNO
    eModDrvWinlineMsgIdBit29_t          ebit29  : 1;
    eModDrvWinlineMsgIdBit30_t          ebit30  : 1;
    eModDrvWinlineMsgIdBit31_t          ebit31  : 1;
}__attribute__((packed)) stModDrvWinlineMsgId_t;


typedef union
{
    stModDrvWinlineMsgId_t              stValue;                                //结构体访问
    u32                                 u32Value;                               //32位访问
}__attribute__((packed)) unModDrvWinlineMsgId_t;

















//永联科技 NXR 系列充电模块电源的一些数据
typedef struct
{
    bool                                bModExistSts;                           //模块存在状态
    u32                                 u32RxTimestamp;                         //最近一次收到该模块广播报文的时间戳
    
    f32                                 f32UdcOut;
    f32                                 f32IdcOut;
    f32                                 f32IdcOutLimit;
    f32                                 f32Tdc;
    f32                                 f32UacUdcIn;
    f32                                 f32UbusP;
    f32                                 f32UbusN;
    f32                                 f32Tenv;
    f32                                 f32Ua;
    f32                                 f32Ub;
    f32                                 f32Uc;
    f32                                 f32Tpfc;
    f32                                 f32PdcOutRated;
    f32                                 f32IdcOutRated;
    stModDrvWinlineWarningCode_t        stWarningCode;
    u32                                 u32AddrBySwitch;
    u32                                 u32PowerInput;
    u32                                 u32Altitude;
    eModDrvWinlineInputMode_t           eInputMode;
    u64                                 u64SerialNo;
    u32                                 u32SwVerDcdc;
    u32                                 u32SwVerPfc;
    u32                                 u32FanS;
    u32                                 u32Group;
    
}stModDrvWinlineData_t;












//永联科技 NXR 系列充电模块电源驱动 数据缓存结构定义
typedef struct
{
    i32                                 i32Port;
    i32                                 i32Baudrate;
    
    //接收缓存和保护锁
    SemaphoreHandle_t                   xSemPkt;
    stModDrvWinlinePkt_t                stRxPkt;
    
    //命令应答信号量
    //永联模块的modbus协议比较简单，就不为每个寄存器地址配备信号量去做反馈了
    //全部的命令公用此信号量
    SemaphoreHandle_t                   xSemAck;
    
    
    //为每个地址的模块分配一个结构体
    stModDrvWinlineData_t               stData[cModDrvWinlineAddrNumMax];
    
    //模块存在状态更新时间戳
    //程序每隔一段时间会将所有地址轮询一次，不管该地址是否有模块在线
    //以便更新总线上所有在线的模块地址列表
    u32                                 u32ModExistStsUpdateTime;
    
}stModDrvWinlineCache_t;


























extern bool  sModDrvWinlineInit(i32 i32Port, i32 i32Baudrate);
extern bool  sModDrvWinlineSend(u8 u8Addr, eModDrvWinlineFun_t eFun, eModDrvWinlineReg_t eReg, unModDrvWinlinePayload_t unPayload, bool bWaitAckFlag);





#endif












