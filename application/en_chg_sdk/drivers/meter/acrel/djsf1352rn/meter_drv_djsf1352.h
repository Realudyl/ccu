/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :     meter_drv_djsf1352.h
* Description                           :     安科瑞电表驱动
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2022-08-09
* notice                                :     具体型号为：DJSF1352-RN 导轨式直流电能表
*                                             使用modbus协议,而不是DLT645规约
****************************************************************************************************/
#ifndef _meter_drv_djsf1352_h_
#define _meter_drv_djsf1352_h_
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "usart_drv_trans.h"





//电表串口
#ifndef cSdkMeterComUartNum
#define cMeterComUartNum                (eUsartNum2)
#else
#define cMeterComUartNum                (cSdkMeterComUartNum)
#endif


//电表串口波特率
#ifndef cSdkMeterComBaudrate
#define cMeterComBaudrate               (9600)
#else
#define cMeterComBaudrate               (cSdkMeterComBaudrate)
#endif


//电表个数
#ifndef cSdkMeterNum
#define cMeterNum                       (1)
#else
#define cMeterNum                       (cSdkMeterNum)
#endif




//电表收发缓存区大小
#define cMeterDjsf1352rnBufSize         (256)


//判断电表通讯故障时间
#define cMeterDjsf1352rnFaultTimeCheck  (30000)





//安科瑞DJSF1352-RN电表协议 功能域 定义
typedef enum
{
    eMeterAcrelFunctionRead             = 0x03,                                 //读取保持寄存器---在一个或多个保持寄存器中取得当前的二进制值
    eMeterAcrelFunctionWrite            = 0x10,                                 //预置多寄存器-----把具体的二进制值装入一串连续的保持寄存器
    
    eMeterAcrelFunctionMax
}__attribute__((packed)) eMeterDjsf1352rnFunction_t;






//安科瑞DJSF1352-RN电表协议 帧头结构 定义
typedef struct
{
    u8                                  u8Addr;                                 //地址域
    eMeterDjsf1352rnFunction_t          eFunction;                              //功能域
}__attribute__((packed)) stMeterDjsf1352rnHead_t;

#define cMeterDjsf1352rnHeadSize        sizeof(stMeterDjsf1352rnHead_t)         //head长度
#define cMeterDjsf1352rnHeadTailSize    (cMeterDjsf1352rnHeadSize + 2)          //包含尾部校验和的head长度






//安科瑞DJSF1352-RN电表协议 最大寄存器个数 定义
//-5 是为了保证 unMeterDjsf1352rnPayload_t 里 stFun10 不会大于 u8Buf
#define cMeterDjsf1352rnRegNumMax       ((cMeterDjsf1352rnBufSize - cMeterDjsf1352rnHeadTailSize - 5) / 2)






//安科瑞DJSF1352-RN电表协议 Function 0x03 指令下发(主机->电表)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
}__attribute__((packed)) stMeterDjsf1352rnFun03_t;

//安科瑞DJSF1352-RN电表协议 Function 0x03 应答(电表->主机)
typedef struct
{
    u8                                  u8Len;
    u16                                 u16RegValue[cMeterDjsf1352rnRegNumMax];
}__attribute__((packed)) stMeterDjsf1352rnFun03Ack_t;






//安科瑞DJSF1352-RN电表协议 Function 0x10 指令下发(主机->电表)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
    u8                                  u8Len;
    u16                                 u16RegValue[cMeterDjsf1352rnRegNumMax];
}__attribute__((packed)) stMeterDjsf1352rnFun10_t;

//安科瑞DJSF1352-RN电表协议 Function 0x10 应答(电表->主机)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
}__attribute__((packed)) stMeterDjsf1352rnFun10Ack_t;






//安科瑞DJSF1352-RN电表协议 payload 定义
typedef union
{
    stMeterDjsf1352rnFun03_t            stFun03; 
    stMeterDjsf1352rnFun03Ack_t         stFun03Ack;
    
    stMeterDjsf1352rnFun10_t            stFun10;
    stMeterDjsf1352rnFun10Ack_t         stFun10Ack;
    
    u8                                  u8Buf[cMeterDjsf1352rnBufSize - cMeterDjsf1352rnHeadTailSize];
}__attribute__((packed)) unMeterDjsf1352rnPayload_t;






//安科瑞DJSF1352-RN电表协议 整个帧结构定义
typedef struct
{
    stMeterDjsf1352rnHead_t             stHead;
    unMeterDjsf1352rnPayload_t          unPayload;
}__attribute__((packed)) stMeterDjsf1352rnPkt_t;



//安科瑞DJSF1352-RN电表协议 整个帧结构定义---union 形式
typedef union 
{
    stMeterDjsf1352rnPkt_t              stPkt;
    u8                                  u8Buf[cMeterDjsf1352rnBufSize];
}__attribute__((packed)) unMeterDjsf1352rnPkt_t;







//安科瑞DJSF1352-RN电表协议 寄存器地址 定义
typedef enum
{
    eMeterDjsf1352rnRegUdc             = 0,                                     //直流电压值
    eMeterDjsf1352rnRegUdcDp           = 1,                                     //电压小数点(DPT)
    eMeterDjsf1352rnRegIdc             = 2,                                     //直流电流值
    eMeterDjsf1352rnRegIdcDp           = 3,                                     //电流小数点(DCT)
    eMeterDjsf1352rnRegBreaklineDetect = 4,                                     //断线检测指示
    eMeterDjsf1352rnRegTinter          = 5,                                     //内部温度
    
    eMeterDjsf1352rnRegPower           = 8,                                     //功率值
    eMeterDjsf1352rnRegPowerDp         = 9,                                     //功率小数点(DP)
    
    eMeterDjsf1352rnRegEposH           = 12,                                    //总正向有功电能 高16位
    eMeterDjsf1352rnRegEposL           = 13,                                    //  正           低
    eMeterDjsf1352rnRegEnegH           = 14,                                    //  反           高
    eMeterDjsf1352rnRegEnegL           = 15,                                    //  反           低
    eMeterDjsf1352rnRegUdcRatio        = 16,                                    //电压变比
    eMeterDjsf1352rnRegIdcRated1       = 17,                                    //额定一次电流值
    eMeterDjsf1352rnRegDiDoSts         = 18,                                    //开关量输入输出状态
    eMeterDjsf1352rnRegWarningSts      = 19,                                    //报警状态
    
    eMeterDjsf1352rnRegTime1           = 30,                                    //日期时间设置 年 月
    eMeterDjsf1352rnRegTime2           = 31,                                    //日期时间设置 日 时
    eMeterDjsf1352rnRegTime3           = 32,                                    //日期时间设置 分 秒 ---均为10进制
    eMeterDjsf1352rnRegMeterReadInfo   = 33,                                    //抄表信息 高字节:当前抄表日(1-31)  低字节:当前费率(0-3依次为尖峰平谷)
    
    eMeterDjsf1352rnRegSwVer           = 35,                                    //软件版本号
    
    //下面还有相当多的寄存器 略过,需要用的时候再增加
    //......
    
    eMeterDjsf1352rnRegMax
}eMeterDjsf1352rnReg_t;











//安科瑞DJSF1352-RN电表协议 断线检测指示 定义
typedef enum
{
    eMeterDjsf1352rnBreaklineSts0       = 0,                                    //未断线
    eMeterDjsf1352rnBreaklineSts1       = 1,                                    //  断线
    
    eMeterDjsf1352rnBreaklineStsMax
}eMeterDjsf1352rnBreaklineSts_t;



//安科瑞DJSF1352-RN电表协议 开关量输入输出状态 定义
typedef struct
{
    u16                                 bDo1   : 1;                             //输出1
    u16                                 bDo2   : 1;                             //输出2
    u16                                 bRsvd1 : 6;
    u16                                 bDi1   : 1;                             //输入1
    u16                                 bDi2   : 1;                             //输入2
    u16                                 bRsvd2 : 6;
}stMeterDjsf1352rnDiDoSts_t;
typedef union
{
    stMeterDjsf1352rnDiDoSts_t          stValue;                                //结构体访问
    u16                                 u16Value;                               //16位访问
}unMeterDjsf1352rnDiDoSts_t;



//安科瑞DJSF1352-RN电表协议 报警状态 定义
typedef struct
{
    u16                                 bUdo1High : 1;                          //DO1过压
    u16                                 bUdo1Low  : 1;                          //DO1欠压
    u16                                 bIdo1High : 1;                          //DO1过流
    u16                                 bIdo1Low  : 1;                          //DO1欠流
    u16                                 bPdo1High : 1;                          //DO1过功率
    u16                                 bPdo1Low  : 1;                          //DO1欠功率
    u16                                 bI1do1    : 1;                          //第 1 路开 关量输入
    u16                                 bI2do1    : 1;                          //第 2 路开 关量输入
    
    u16                                 bUdo2High : 1;                          //DO2 同上
    u16                                 bUdo2Low  : 1;
    u16                                 bIdo2High : 1;
    u16                                 bIdo2Low  : 1;
    u16                                 bPdo2High : 1;
    u16                                 bPdo2Low  : 1;
    u16                                 bI1do2    : 1;
    u16                                 bI2do2    : 1;

}stMeterDjsf1352rnWarningSts_t;

typedef union
{
    stMeterDjsf1352rnWarningSts_t       stValue;                                //结构体访问
    u16                                 u16Value;                               //16位访问
}unMeterDjsf1352rnWarningSts_t;



//安科瑞DJSF1352-RN电表协议 当前费率 定义
typedef enum
{
    eMeterDjsf1352rnRateSharp           = 0,                                    //尖
    eMeterDjsf1352rnRatePeak            = 1,                                    //峰
    eMeterDjsf1352rnRateFlat            = 2,                                    //平
    eMeterDjsf1352rnRateValley          = 3,                                    //谷
    
    eMeterDjsf1352rnRateMax
}eMeterDjsf1352rnRate_t;





























//电表数据字段
typedef struct
{
    f32                                 f32Udc;                                 //直流电压值
    f32                                 f32Idc;                                 //直流电流值
    eMeterDjsf1352rnBreaklineSts_t      eBreaklineSts;                          //断线检测指示
    f32                                 f32Tmeter;                              //内部温度
    f32                                 f32Pdc;                                 //功率值
    u32                                 u32Epos;                                //总正向有功电能
    u32                                 u32Eneg;                                //总反向有功电能
    u16                                 u16UdcRatio;                            //电压变比
    u16                                 u16IdcRated1;                           //额定一次电流值
    unMeterDjsf1352rnDiDoSts_t          unDiDoSts;                              //开关量输入输出状态
    unMeterDjsf1352rnWarningSts_t       unWarningSts;                           //报警状态
    stTime_t                            stTime;                                 //日期时间
    u8                                  u8ReadingDay;                           //抄表信息:抄表日
    eMeterDjsf1352rnRate_t              eRate;                                  //抄表信息:当前费率(0-3依次为尖峰平谷)
    u16                                 u16SwVer;                               //软件版本号
}stMeterDjsf1352rnData_t;













//安科瑞DJSF1352-RN电表驱动层 数据结构
//缓存驱动层所需的一些数据并返回给上层
typedef struct
{
    //收发资源
    u8                                  u8Buf[cMeterDjsf1352rnBufSize];         //接收buf
    unMeterDjsf1352rnPkt_t              unTxPkt;                                //发送buf
    SemaphoreHandle_t                   xSemTxPkt;
    
    //断帧资源
    i32                                 i32RxIndex;
    unMeterDjsf1352rnPkt_t              unRxPkt;
    unMeterDjsf1352rnPkt_t              unRxPktTemp;
    
    //命令应答信号量
    SemaphoreHandle_t                   xSemAck;
    
    
    //电表数据
    stMeterDjsf1352rnData_t             stData[cMeterNum];
    
    //电表通信故障
    bool                                bMeterFault_MCF;
    
    //上一次接收电表数据时间戳
    u32                                 u32RecvTimeStamp;
}stMeterDjsf1352rnCache_t;












extern bool sMeterDrvDjsf1352rnInit(void);






#endif







