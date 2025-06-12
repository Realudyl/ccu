/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mo_drv_en.h
* Description                           :   EN+模块电源CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-30
* notice                                :   
****************************************************************************************************/
#ifndef _mo_drv_en_h_
#define _mo_drv_en_h_
#include "mod_drv_en_data_def.h"

#include "can_drv_trans.h"











//收发任务周期 单位ms
#ifndef cSdkModDrvRecvTaskCycle
#define cModDrvEnRecvTaskCycle          (10)
#else
#define cModDrvEnRecvTaskCycle          (cSdkModDrvRecvTaskCycle)
#endif

#ifndef cSdkModDrvSendTaskCycle
#define cModDrvEnSendTaskCycle          (300)
#else
#define cModDrvEnSendTaskCycle          (cSdkModDrvSendTaskCycle)
#endif




//模块电源输入输出特性曲线定义
#ifndef cSdkModDrvUdcMax                                                        //最大输出电压
#define cModDrvEnUdcMax                 (1000.0f)
#else
#define cModDrvEnUdcMax                 (cSdkModDrvUdcMax)
#endif

#ifndef cSdkModDrvUdcMin                                                        //最小输出电压
#define cModDrvEnUdcMin                 (200.0f)
#else
#define cModDrvEnUdcMin                 (cSdkModDrvUdcMin)
#endif

#ifndef cSdkModDrvIdcMax                                                        //最大输出电流
#define cModDrvEnIdcMax                 (100.0f)
#else
#define cModDrvEnIdcMax                 (cSdkModDrvIdcMax)
#endif

#ifndef cSdkModDrvPdc                                                           //最大输出功率
#define cModDrvEnPdc                    (30000.0f)
#else
#define cModDrvEnPdc                    (cSdkModDrvPdc)
#endif

























//EN+ 模块电源CAN协议 地址相关定义
#define cModDrvEnAddrMaster             (0xf0)                                  //主机地址
#define cModDrvEnAddrBd                 (0x3f)                                  //广播地址
#define cModDrvEnAddrMin                (0x01)                                  //模块地址最小值
#define cModDrvEnAddrMax                (0x10)                                  //模块地址最大值






//定义模块在线标志清零时间 秒
#define cModDrvEnExistStsClearTime      (3)











//EN+ 模块电源 CAN协议 MsgId->命令号定义
typedef enum 
{
    eModDrvEnCmd01 = 0x01,                                                      //读取：输出电压Udc和输出总电流Idc 实际值
    eModDrvEnCmd02 = 0x02,                                                      //读取：模块数量
    eModDrvEnCmd03 = 0x03,                                                      //读取：输出电压Udc和输出电流Idc 实际值
    eModDrvEnCmd04 = 0x04,                                                      //读取：风扇转速 温度 错误码
    eModDrvEnCmd05 = 0x05,                                                      //读取：系列温度
    eModDrvEnCmd06 = 0x06,                                                      //读取：输入电压及模块状态
    eModDrvEnCmd07 = 0x07,                                                      //xxxx：模块并机广播报文-------上位机不可用
    eModDrvEnCmd08 = 0x08,                                                      //读取：总运行时间和本次运行时间
    eModDrvEnCmd09 = 0x09,                                                      //读取：LLC和PFC内部数据
    eModDrvEnCmd0A = 0x0A,                                                      //读取：模块组号
    eModDrvEnCmd0B = 0x0B,                                                      //读取：输入电流、输入功率
    eModDrvEnCmd0C = 0x0C,                                                      //读取：电量
    eModDrvEnCmd0D = 0x0D,                                                      //读取：LLC数据
    
    eModDrvEnCmd13 = 0x13,                                                      //设置：walking功能使能和禁止
    eModDrvEnCmd14 = 0x14,                                                      //控制：绿色LED灯闪烁
    eModDrvEnCmd16 = 0x16,                                                      //设置：模块分组
    eModDrvEnCmd19 = 0x19,                                                      //控制：进入或退出休眠
    eModDrvEnCmd1A = 0x1A,                                                      //控制：开机或关机
    eModDrvEnCmd1C = 0x1C,                                                      //设置：输出电压Udc和输出电流Idc
    eModDrvEnCmd1F = 0x1F,                                                      //设置：模块的地址模式
    eModDrvEnCmd20 = 0x20,                                                      //控制：总线上所有模块重新分配地址
    eModDrvEnCmd21 = 0x21,                                                      //设置：智能并机模式
    eModDrvEnCmd22 = 0x22,                                                      //设置：CAN广播报文使能或禁止
    eModDrvEnCmd23 = 0x23,                                                      //设置：模块的高低压模式----------------仅30kw机型有效
    eModDrvEnCmd24 = 0x24,                                                      //设置：MPPT曲线
    eModDrvEnCmd25 = 0x25,                                                      //设置：模块间并联均流功能
    eModDrvEnCmd26 = 0x26,                                                      //设置：模块间串联均压功能（要求地址相邻，如1-2,3-4,...）
    eModDrvEnCmd28 = 0x28,                                                      //设置：电压/电流变化率
    eModDrvEnCmd2A = 0x2A,                                                      //设置：老化模式
    eModDrvEnCmd2B = 0x2B,                                                      //校验：管理员密码
    
    eModDrvEnCmd30 = 0x30,                                                      //读取：模块固有信息1---软件料号、软件匹配码、机型
    eModDrvEnCmd31 = 0x31,                                                      //读取：模块固有信息2---模块特征字
    eModDrvEnCmd32 = 0x32,                                                      //读取：模块固有信息3---序列号      高8个字符
    eModDrvEnCmd33 = 0x33,                                                      //读取：模块固有信息4---序列号      低8个字符
    eModDrvEnCmd34 = 0x34,                                                      //读取：模块固有信息5---ModelName   高8个字符
    eModDrvEnCmd35 = 0x35,                                                      //读取：模块固有信息6---ModelName   低8个字符
    eModDrvEnCmd36 = 0x36,                                                      //读取：模块固有信息7---制造商ID    高8个字符
    eModDrvEnCmd37 = 0x37,                                                      //读取：模块固有信息8---制造商ID    低8个字符
    eModDrvEnCmd38 = 0x38,                                                      //读取：模块固有信息9---软件版本号  高8个字符
    eModDrvEnCmd39 = 0x39,                                                      //读取：模块固有信息A---软件版本号  低8个字符
    
    eModDrvEnCmdMax
}eModDrvEnCmd_t;




//EN+ 模块电源指令功能 封装结构体
typedef struct
{
    eModDrvEnCmd_t                      eCmd;                                   //指令类型
    const char                          *pString;                               //该指令的描述字符串
    i32                                 i32WaitTime;                            //该指令的等待时间 ms
    SemaphoreHandle_t                   xSemAck;                                //该指令的返回报文信号量
    bool                                bProcRst;                               //该指令的处理结果
    void                                (*pCmdPro)(const stCanMsg_t *pFrame);   //该指令的接收报文处理函数
}stModDrvEnCmdMap_t;











//EN+ 模块电源CAN协议 MsgId->设备号定义
//注意：在本协议实现中 不会使用 eModDrvEnDevNum01 和 eModDrvEnDevNum0B
typedef enum 
{
    eModDrvEnDevNum01 = 0x01,                                                   //充电模块与充电模块之间的通讯
    eModDrvEnDevNum0A = 0x0A,                                                   //监控单元与单个充电模块或系统所有充电模块之间的通讯
    eModDrvEnDevNum0B = 0x0B,                                                   //监控单元与同组充电模块之间的通讯
    
    eModDrvEnDevNumMax
}eModDrvEnDevNum_t;


//EN+ 模块电源CAN协议 MsgId->错误码定义
typedef enum 
{
    eModDrvEnErrCodeNone = 0x00,                                                //无错误
    eModDrvEnErrCodeAddr,                                                       //地址无效
    eModDrvEnErrCodeCmdNum,                                                     //命令号异常
    eModDrvEnErrCodeData,                                                       //数据信息异常
    eModDrvEnErrCodeEeprom,                                                     //EEPROM忙 无法写入数据
    eModDrvEnErrCodeAddrAlloc,                                                  //地址分配过程结束
    eModDrvEnErrCodeModInit,                                                    //模块尚未完成初始化
    eModDrvEnErrCodeStarting,                                                   //模块启动过程中
    
    //eModDrvEnErrCodeMax
}eModDrvEnErrCode_t;


//EN+ 模块电源CAN协议 MsgId->错误消息帧标志定义
typedef enum 
{
    eModDrvEnMsgIdBit29DataFrame = 0,                                           //数据帧
    eModDrvEnMsgIdBit29ErrMsg = 1,                                              //错误消息
    
    //eModDrvEnMsgIdBit29Max
}eModDrvEnMsgIdBit29_t;


//EN+ 模块电源CAN协议 MsgId->远程帧帧标志定义
typedef enum 
{
    eModDrvEnMsgIdBit30Data = 0,                                                //数据帧
    eModDrvEnMsgIdBit30Rtr = 1,                                                 //远程帧
    
    //eModDrvEnMsgIdBit30Max
}eModDrvEnMsgIdBit30_t;


//EN+ 模块电源CAN协议 MsgId->扩展帧帧标志定义
typedef enum 
{
    eModDrvEnMsgIdBit31Standard  = 0,                                           //标准帧 SFF
    eModDrvEnMsgIdBit31Extended  = 1,                                           //扩展帧 EFF
    
    //eModDrvEnMsgIdBit31Max
}eModDrvEnMsgIdBit31_t;






//EN+ 模块电源CAN协议 MsgId 定义
typedef struct
{
    u32                                 bAddSrc  : 8;                           //  源地址
    u32                                 bAddDst  : 8;                           //目的地址
    eModDrvEnCmd_t                      eCmdNum  : 6;                           //命令号
    eModDrvEnDevNum_t                   eDevNum  : 4;                           //设备号
    eModDrvEnErrCode_t                  eErrCode : 3;                           //错误码
    eModDrvEnMsgIdBit29_t               ebit29   : 1;
    eModDrvEnMsgIdBit30_t               ebit30   : 1;
    eModDrvEnMsgIdBit31_t               ebit31   : 1;
}__attribute__((packed)) stModDrvEnMsgId_t;


typedef union
{
    stModDrvEnMsgId_t                   stValue;                                //结构体访问
    u32                                 u32Value;                               //32位访问
}__attribute__((packed)) unModDrvEnMsgId_t;





















//EN+ 模块电源的一些数据
typedef struct
{
    bool                                bModExistSts;                           //模块存在状态
    i32                                 u32RxTimestamp;                         //最近一次收到该模块广播报文的时间戳
    
    //CMD01指令 输出电压，总电流
    f32                                 f32UdcTotal;
    f32                                 f32IdcTotal;
    
    //CMD02指令 模块数量
    u8                                  u8ModNum;
    
    //CMD03指令 输出电压，电流
    f32                                 f32Udc;
    f32                                 f32Idc;
    
    //CMD04指令 风扇转速、温度、错误码
    u8                                  u8FanSpeed;
    i8                                  i8Tenv;                                 //环境温度
    unModDrvEnErrCode_t                 unErrCode;
    
    //CMD05指令 系列温度
    i8                                  i8Tmos1;                                //MOS 管1 温度
    i8                                  i8Tmos2;                                //MOS 管2 温度
    i8                                  i8Tdiod1;                               //二极管1 温度
    i8                                  i8Tdiod2;                               //二极管2 温度
    i8                                  i8Ttsf;                                 //变压器  温度
    i8                                  i8Tcpu;                                 //CPU     温度
    i8                                  i8Trad1;                                //散热片1 温度
    i8                                  i8Trad2;                                //散热片2 温度
    
    //CMD06指令 输入电压、模块状态
    u16                                 u16Uab;
    u16                                 u16Ubc;
    u16                                 u16Uca;
    eModDrvEnWorkState_t                eState1;
    eModDrvEnSmartParaState_t           eState2;
    
    //CMD08指令 运行时间
    u32                                 u32Htotal;                              //总工作时间
    u32                                 u32Hthistime;                           //本次开机后的工作时间
    
    //CMD09指令 LLC和PFC数据
    u16                                 u16Fq;                                  //开关频率
    u16                                 u16Iq2;                                 //Q2关断电流
    u16                                 u16Fac;                                 //输入交流频率
    u16                                 u16Ubus;                                //PFC母线电压
    
    //CMD0A指令 模块组号
    u8                                  u8Group;                                //组号
    
    //CMD0B指令 输入电流、输入功率
    u16                                 u16Ia;                                  //A相电流
    u16                                 u16Ib;                                  //B相电流
    u16                                 u16Ic;                                  //C相电流
    u16                                 u16Pac;                                 //输入功率
    
    //CMD0C指令 模块总运行电量、本次电量
    u32                                 u32Etotal;                              //总电量
    u32                                 u32Ethistime;                           //本次电量
    
    //CMD0D指令 LLC数据
    u16                                 u16IrA;                                 //谐振腔电流A路
    u16                                 u16IrB;                                 //谐振腔电流B路
    u16                                 u16Pdc;                                 //输出功率（0.1w）
    
    //CMD1A指令 开机或关机返回状态
    unModDrvEnState3_t                  unState3;
    
    //CMD30指令 模块固有信息1
    u32                                 u32MaterialNum;                         //软件物料号码
    u16                                 u16MatchCode;                           //软件匹配码
    u16                                 u16ModelType;                           //机型
    
    //CMD31指令 模块固有信息2
    unModDrvEnFeatureWord_t             unFeatureWord;                          //模块特征字
    
    //CMD32指令 模块固有信息3
    //CMD33指令 模块固有信息4
    char                                u8SerialNum[cModDrvEnSerialNumLen];
    
    //CMD34指令 模块固有信息5
    //CMD35指令 模块固有信息6
    char                                u8ModelName[cModDrvEnModelNameLen];
    
    //CMD36指令 模块固有信息7
    //CMD37指令 模块固有信息8
    char                                u8ManufacturerId[cModDrvEnManufacturerIdLen];
    
    //CMD38指令 模块固有信息9
    //CMD39指令 模块固有信息A
    char                                u8SwVer[cModDrvEnSwVerLen];
    
}stModDrvEnData_t;







//EN+模块电源驱动 数据缓存结构定义
typedef struct
{
    i32                                 i32Port;
    i32                                 i32Baudrate;
    
    //为每个地址的模块分配一个结构体
    stModDrvEnData_t                    stData[cModDrvEnAddrMax];
    
}stModDrvEnCache_t;;



















extern bool  sModDrvEnInit(i32 i32Port, i32 i32Baudrate);

extern bool  sModDrvEnConfig(u8 u8Addr);

extern bool  sModDrvEnTxCmd01(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd02(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd03(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd04(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd05(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd06(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd08(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd09(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd0A(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd0B(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd0C(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd0D(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd13(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16WalkinTime);
extern bool  sModDrvEnTxCmd14(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32FlashTime);
extern bool  sModDrvEnTxCmd16(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 u8Group);
extern bool  sModDrvEnTxCmd19(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSleepValue_t eValue);
extern bool  sModDrvEnTxCmd1A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnTurnValue_t eValue);
extern bool  sModDrvEnTxCmd1C(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32Udc, u32 u32Idc);
extern bool  sModDrvEnTxCmd1F(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnAddrMode_t eMode);
extern bool  sModDrvEnTxCmd20(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd21(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSmartParaState_t eState);
extern bool  sModDrvEnTxCmd22(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnCanBroadcast_t eBdValue);
extern bool  sModDrvEnTxCmd23(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnVoltMode_t eMode);
extern bool  sModDrvEnTxCmd24(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16UpvOpen, u16 u16UpvMpp, u16 u16IpvMpp);
extern bool  sModDrvEnTxCmd25(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropU, u8 u8Coef);
extern bool  sModDrvEnTxCmd26(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropI, u8 u8Coef);
extern bool  sModDrvEnTxCmd28(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16Uspeed, u16 u16Ispeed);
extern bool  sModDrvEnTxCmd2A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, bool bAgingMode);
extern bool  sModDrvEnTxCmd2B(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 *pPassword, u8 u8Len);
extern bool  sModDrvEnTxCmd30(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd31(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd32(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd33(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd34(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd35(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd36(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd37(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd38(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
extern bool  sModDrvEnTxCmd39(u8 u8Addr, eModDrvEnDevNum_t eDevNum);





#endif








