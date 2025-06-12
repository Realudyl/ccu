/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   meter_drv_djsf2669.c
* Description                           :   青岛英利达电表驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   具体型号为:DJSF2669D 型双回路导轨式直流电能表
*                                           使用modbus-rtu协议,而不是DLT645规约
****************************************************************************************************/
#ifndef _meter_drv_djsf2669_h_
#define _meter_drv_djsf2669_h_
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
#define cMeterComBaudrate               (2400)
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
#define cMeterDjsf2669dBufSize          (256)


//判断电表通讯故障时间
#define cMeterDjsf2669dFaultTimeCheck   (30000)





//英利达DJSF2669D电表协议 功能域 定义
typedef enum
{
    eMeterDjsf2669dFunCodeRead          = 0x03,                                 //读多个寄存器
    eMeterDjsf2669dFunCodeWriteM        = 0x10,                                 //写多个寄存器
    
    eMeterDjsf2669dFunCodeMax
}__attribute__((packed)) eMeterDjsf2669dFunCode_t;






//英利达DJSF2669D电表协议 帧头结构 定义
typedef struct
{
    u8                                  u8Addr;                                 //地址
    eMeterDjsf2669dFunCode_t            eCode;                                  //功能码
}__attribute__((packed)) stMeterDjsf2669dHead_t;

#define cMeterDjsf2669dHeadSize         sizeof(stMeterDjsf2669dHead_t)         //head长度
#define cMeterDjsf2669dHeadTailSize     (cMeterDjsf2669dHeadSize + sizeof(u16))//包含尾部校验和的head长度






//英利达DJSF2669D电表协议 最大寄存器个数 定义
//-5 是为了保证 unMeterDjsf2669dPayload_t 里 stFun10 不会大于 u8Buf
#define cMeterDjsf2669dRegNumMax        ((cMeterDjsf2669dBufSize - cMeterDjsf2669dHeadTailSize - 5) / 2)






//英利达DJSF2669D电表协议 Function 0x03 指令下发(主机->电表)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
}__attribute__((packed)) stMeterDjsf2669dFun03_t;

//英利达DJSF2669D电表协议 Function 0x03 应答(电表->主机)
typedef struct
{
    u8                                  u8Len;
    u16                                 u16RegValue[cMeterDjsf2669dRegNumMax];
}__attribute__((packed)) stMeterDjsf2669dFun03Ack_t;






//英利达DJSF2669D电表协议 Function 0x10 指令下发(主机->电表)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
    u8                                  u8Len;
    u16                                 u16RegValue[cMeterDjsf2669dRegNumMax];
}__attribute__((packed)) stMeterDjsf2669dFun10_t;

//英利达DJSF2669D电表协议 Function 0x10 应答(电表->主机)
typedef struct
{
    u16                                 u16RegAddr;
    u16                                 u16RegNum;
}__attribute__((packed)) stMeterDjsf2669dFun10Ack_t;






//英利达DJSF2669D电表协议 payload 定义
typedef union
{
    stMeterDjsf2669dFun03_t             stFun03; 
    stMeterDjsf2669dFun03Ack_t          stFun03Ack;
    
    stMeterDjsf2669dFun10_t             stFun10;
    stMeterDjsf2669dFun10Ack_t          stFun10Ack;
    
    u8                                  u8Buf[cMeterDjsf2669dBufSize - cMeterDjsf2669dHeadTailSize];
}__attribute__((packed)) unMeterDjsf2669dPayload_t;






//英利达DJSF2669D电表协议 整个帧结构定义
typedef struct
{
    stMeterDjsf2669dHead_t              stHead;
    unMeterDjsf2669dPayload_t           unPayload;
}__attribute__((packed)) stMeterDjsf2669dPkt_t;



//英利达DJSF2669D电表协议 整个帧结构定义---union 形式
typedef union 
{
    stMeterDjsf2669dPkt_t               stPkt;
    u8                                  u8Buf[cMeterDjsf2669dBufSize];
}__attribute__((packed)) unMeterDjsf2669dPkt_t;












//英利达DJSF2669D电表协议 寄存器地址 定义
typedef enum
{
    eMeterDjsf2669dRegU1H               = 0x203C,                               //1 路电压高位字---32bit 有符号数,单位：V,4 位小数
    eMeterDjsf2669dRegU1L               = 0x203D,                               //1       低    
    eMeterDjsf2669dRegU2H               = 0x203E,                               //2       高    
    eMeterDjsf2669dRegU2L               = 0x203F,                               //2       低    
    eMeterDjsf2669dRegI1H               = 0x2040,                               //1 路电流高    ---32bit 有符号数,单位：A,4 位小数
    eMeterDjsf2669dRegI1L               = 0x2041,                               //1       低    
    eMeterDjsf2669dRegI2H               = 0x2042,                               //2       高    
    eMeterDjsf2669dRegI2L               = 0x2043,                               //2       低    
    eMeterDjsf2669dRegP1H               = 0x2044,                               //1 路功率高    ---32bit 有符号数,单位：kW,4 位小数
    eMeterDjsf2669dRegP1L               = 0x2045,                               //1       低    
    eMeterDjsf2669dRegP2H               = 0x2046,                               //2       高    
    eMeterDjsf2669dRegP2L               = 0x2047,                               //2       低    
    
    //t:total---总   s:sharp---尖   p:peak---峰   f:flat---平   v:valley---谷
    eMeterDjsf2669dRegEtH               = 0x9A00,                               //组合总电能高位字
    eMeterDjsf2669dRegEtL               = 0x9A01,                               //    总    低    
    eMeterDjsf2669dRegEsH               = 0x9A02,                               //    尖    高    
    eMeterDjsf2669dRegEsL               = 0x9A03,                               //    尖    低    
    eMeterDjsf2669dRegEpH               = 0x9A04,                               //    峰    高    
    eMeterDjsf2669dRegEpL               = 0x9A05,                               //    峰    低    
    eMeterDjsf2669dRegEfH               = 0x9A06,                               //    平    高    
    eMeterDjsf2669dRegEfL               = 0x9A07,                               //    平    低    
    eMeterDjsf2669dRegEvH               = 0x9A08,                               //    谷    高    
    eMeterDjsf2669dRegEvL               = 0x9A09,                               //    谷    低    
    
    eMeterDjsf2669dRegEptH              = 0x9A0A,                               //正向总    高    p:positive---正向
    eMeterDjsf2669dRegEptL              = 0x9A0B,                               //    总    低    
    eMeterDjsf2669dRegEpsH              = 0x9A0C,                               //    尖    高    
    eMeterDjsf2669dRegEpsL              = 0x9A0D,                               //    尖    低    
    eMeterDjsf2669dRegEppH              = 0x9A0E,                               //    峰    高    
    eMeterDjsf2669dRegEppL              = 0x9A0F,                               //    峰    低    
    eMeterDjsf2669dRegEpfH              = 0x9A10,                               //    平    高    
    eMeterDjsf2669dRegEpfL              = 0x9A11,                               //    平    低    
    eMeterDjsf2669dRegEpvH              = 0x9A12,                               //    谷    高    
    eMeterDjsf2669dRegEpvL              = 0x9A13,                               //    谷    低    
    
    eMeterDjsf2669dRegEntH              = 0x9A14,                               //反向总    高    n:negative---反向
    eMeterDjsf2669dRegEntL              = 0x9A15,                               //    总    低    
    eMeterDjsf2669dRegEnsH              = 0x9A16,                               //    尖    高    
    eMeterDjsf2669dRegEnsL              = 0x9A17,                               //    尖    低    
    eMeterDjsf2669dRegEnpH              = 0x9A18,                               //    峰    高    
    eMeterDjsf2669dRegEnpL              = 0x9A19,                               //    峰    低    
    eMeterDjsf2669dRegEnfH              = 0x9A1A,                               //    平    高    
    eMeterDjsf2669dRegEnfL              = 0x9A1B,                               //    平    低    
    eMeterDjsf2669dRegEnvH              = 0x9A1C,                               //    谷    高    
    eMeterDjsf2669dRegEnvL              = 0x9A1D,                               //    谷    低    
    
    
    eMeterDjsf2669dRegClear             = 0xB000,                               //电能清零
    eMeterDjsf2669dRegTime1             = 0xB001,                               //日期时间设置 年 月---BCD码
    eMeterDjsf2669dRegTime2             = 0xB002,                               //日期时间设置 日 时
    eMeterDjsf2669dRegTime3             = 0xB003,                               //日期时间设置 分 秒
    
    
    //还有相当多的寄存器 略过,需要用的时候再增加
    //......
    
    eMeterDjsf2669dRegMax
}__attribute__((packed)) eMeterDjsf2669dReg_t;






//一些宏定义
#define cMeterDjsf2669dRegValueClearE   (0x5A01)                                //电能清零动作的写入值














//电表数据字段
typedef struct
{
    stTime_t                            stTime;                                 //电表时间
    f32                                 f32Udc;                                 //电压值
    f32                                 f32Idc;                                 //电流值
    f32                                 f32Pdc;                                 //功率值
    
    //t:total---总   s:sharp---尖   p:peak---峰   f:flat---平   v:valley---谷
    f32                                 f32Et;                                  //组合电能  总
    f32                                 f32Es;                                  //          尖
    f32                                 f32Ep;                                  //          峰
    f32                                 f32Ef;                                  //          平
    f32                                 f32Ev;                                  //          谷
    
    f32                                 f32Ept;                                 //正向电能  总----p:positive---正向
    f32                                 f32Eps;                                 //          尖
    f32                                 f32Epp;                                 //          峰
    f32                                 f32Epf;                                 //          平
    f32                                 f32Epv;                                 //          谷
    
    f32                                 f32Ent;                                 //反向电能  总----p:positive---正向
    f32                                 f32Ens;                                 //          尖
    f32                                 f32Enp;                                 //          峰
    f32                                 f32Enf;                                 //          平
    f32                                 f32Env;                                 //          谷
    
}stMeterDjsf2669dData_t;













//安科瑞DJSF1352-RN电表驱动层 数据结构
//缓存驱动层所需的一些数据并返回给上层
typedef struct
{
    //收发资源
    u8                                  u8Buf[cMeterDjsf2669dBufSize];          //接收buf
    unMeterDjsf2669dPkt_t               unTxPkt;                                //发送buf
    SemaphoreHandle_t                   xSemTxPkt;
    
    //断帧资源
    i32                                 i32RxIndex;
    unMeterDjsf2669dPkt_t               unRxPkt;
    unMeterDjsf2669dPkt_t               unRxPktTemp;
    
    //命令应答信号量
    SemaphoreHandle_t                   xSemAck;
    
    
    //电表数据---此电表是双路电表, 监控2把枪的数据
    stMeterDjsf2669dData_t              stData[cMeterNum * 2];
    
    //电表通信故障
    bool                                bMeterFault_MCF;
    
    //上一次接收电表数据时间戳
    u32                                 u32RecvTimeStamp;
}stMeterDjsf2669dCache_t;

























extern bool sMeterDrvDjsf2669dInit(void);
extern bool sMeterDrvDjsf2669dWriteRegClear(u8 u8Addr);









#endif



















