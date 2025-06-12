/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   hmi_drv_dwin.h
* Description                           :   dwin屏幕
* Version                               :   
* Author                                :   hh
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
#ifndef _hmi_drv_dwin_H_
#define _hmi_drv_dwin_H_

#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "usart_drv_trans.h"




//迪文屏串口
#ifndef cSdkHmiComUartNum
#define cHmiComUartNum                  (eUsartNum3)
#else
#define cHmiComUartNum                  (cSdkHmiComUartNum)
#endif


//迪文屏幕串口波特率
#ifndef cSdkHmiComBaudrate
#define cHmiComBaudrate                 (115200)
#else
#define cHmiComBaudrate                 (cSdkHmiComBaudrate)
#endif



//显示屏收发缓存区大小
#define cHmiDwinBufSize                 (512)
#define cHmiDwinDateBufSize             (256)



//迪文显示屏串口 协议头 定义
typedef enum
{
    eHmiDwinHead                        = 0x5AA5,                               //报文头固定 5A A5
    
    eHmiDwinHeadMax
}__attribute__((packed)) eHmiDwinHead_t;


//迪文显示屏串口协议 功能域 定义
typedef enum
{
    eHmiDwinFunWrReg                    = 0x80,                                 //指定地址开始写数据串到寄存器。
    eHmiDwinFunRdReg                    = 0x81,                                 //从指定寄存器开始读数据。
    eHmiDwinFunWrRam                    = 0x82,                                 //指定地址开始写数据串(字数据)到变量空间。系统保留的空间不要写。
    eHmiDwinFunRdRam                    = 0x83,                                 //从变量空间指定地址开始读指定长度字数据or屏幕返回所读数据以及触控按键返回到串口数据
    
    eHmiDwinFunMax,
}__attribute__((packed)) eHmiDwinFunction_t;






typedef enum
{
    eDwinTime                           = 0x0010,                               //返回屏幕时间
    eDwinPage                           = 0x0014,                               //屏幕返回当前页面
    eDwinBrightness                     = 0x0031,                               //读取屏幕亮度
    eDwinBright                         = 0x0082,                               //设置背光时间
    
}__attribute__((packed)) eHmiDwinAddr_t;





typedef struct
{
    eHmiDwinAddr_t                      eAddr;                                  //屏幕返回数据带的地址
    bool                                (*pRecv)(const u8 *pBuf, i32 i32Len);   //对应屏幕返回数据指令的接收处理函数
    const char                          *pString;                               //该报文类型描述字符串
}__attribute__((packed))stHmiDwinCmdMap_t;




//迪文显示屏串口 帧头结构 定义
typedef struct
{
    u16                                 u16Head;                                //协议头固定5A A5
    u8                                  u8len;                                  //整个协议长度（不包括头）
    eHmiDwinFunction_t                  eFunction;                              //功能域
}__attribute__((packed)) stHmiDwinrnHead_t;

#define cHmiDwinHeadSize                sizeof(stHmiDwinrnHead_t)               //head长度
#define cHmiDwinHeadTailSize            (cHmiDwinHeadSize + 3)                  //包含head长度+变量地址长度





//迪文屏串口协议 单帧结构定义 0x80 指定地址开始写数据串到寄存器。(主机->屏幕)
typedef struct
{
    u8                                  u8RegID;                                //寄存器页面 ID（0x00-0x08）
    u8                                  u8RegAddr;                              //寄存器地址(0x00-0xFF)
    u8                                  u8WrData[cHmiDwinDateBufSize];          //写入的数据
}__attribute__((packed))stHmiDwinFun80_t;



//迪文屏串口协议 单帧结构定义 0x80 指令应答(屏幕->主机)
typedef struct
{
    u16                                 u16Ack;                                 //应答 0x4F 0x4B 。
}__attribute__((packed))stHmiDwinFun80Ack_t;






//迪文屏串口协议 单帧结构定义 0x81 从指定寄存器开始读数据。。(主机->屏幕)
typedef struct
{
    u8                                  u8RegID;                                //寄存器页面 ID（0x00-0x08）
    u8                                  u8RegAddr;                              //寄存器地址(0x00-0xFF)
    u8                                  u8RdLen;                                //读取数据字节长度(0x01-0xFB)
}__attribute__((packed))stHmiDwinFun81_t;



//迪文屏串口协议 单帧结构定义 0x81 指令应答(屏幕->主机)
typedef struct
{
    u8                                  u8RegID;                                //寄存器页面 ID（0x00-0x08）
    u8                                  u8RegAddr;                              //寄存器地址(0x00-0xFF)
    u8                                  u8RdLen;                                //读取数据字节长度(0x01-0xFB)
    u8                                  u8RegData[cHmiDwinDateBufSize];         //返回的数据
}__attribute__((packed))stHmiDwinFun81Ack_t;






//迪文屏串口协议 单帧结构定义 0x82 指定地址开始写数据串(字数据)到变量空间。系统保留的空间不要写(主机->屏幕)
typedef struct
{
    u16                                 u16RamAddr;                             //变量空间首地址(0x0000-0xFFFF)
    u8                                  u8WrData[cHmiDwinDateBufSize];          //写入数据字节长度
}__attribute__((packed))stHmiDwinFun82_t;


//迪文屏串口协议 单帧结构定义 0x82 指令应答(屏幕->主机)
typedef struct
{
    u16                                 u16Ack;                                 //应答 0x4F 0x4B 。
}__attribute__((packed))stHmiDwinFun82Ack_t;





//迪文屏串口协议 单帧结构定义 0x83 从变量空间指定地址开始读指定长度字数据。。系统保留的空间不要写(主机->屏幕)
typedef struct
{
    u16                                 u16RamAddr;                             //变量空间首地址(0x0000-0xFFFF)
    u8                                  u8RdLen;                                //读取数据字节长度
}__attribute__((packed))stHmiDwinFun83_t;


//迪文屏串口协议 单帧结构定义 0x83 读取数据指令应答(屏幕->主机)
//---触摸按键返回到串口数据也用此指令,ASCII录入如果开启输入状态返回，返回的数据字节长度为地址长度，具体数据长度再u8RamData[1]
// 一般5A A5 06 83 1001 01 00 02，设置SN：5A A5 18 83 51BD 0A 5A 10 53 4E 32 30 32 34 30 35 31 34 35 35 35 36 36 36 FF FF
typedef struct
{
    u16                                 u16Addr;                                //变量空间首地址(0x0000-0xFFFF)
    u8                                  u8RdLen;                                //读取数据字节长度
    u8                                  u8RamData[cHmiDwinDateBufSize];         //读取数据
}__attribute__((packed))stHmiDwinFun83Ack_t;



//读取页面ID后返回的数据
typedef struct
{
    u16                                 u16addr;                                //返回读取页面操作的地址
    u8                                  u8len;                                  //屏幕返回的字长度-一个字有2个字节数据
    u16                                 u16page;                                //切换到的页面
}__attribute__((packed))stHmiDwinRedPage_t;




//读取亮度后返回的数据
typedef struct
{
    u16                                 u16addr;                                //地址
    u8                                  u8len;                                  //屏幕返回的字长度-一个字有2个字节数据
    u8                                  u8D1;                                   //5A表示背光亮度值
    u8                                  u8D0;                                   //0x00-0x64当前背光亮度值
}__attribute__((packed))stHmiDwinBrigtness_t;




//读取屏幕时间返回的数据5A A5 0C 83 0010 04 13 0A 01 00 0B 0C 0D 00
typedef struct
{
    u16                                 u16addr;                                //地址
    u8                                  u8len;                                  //屏幕返回的字长度-一个字有2个字节数据
    u8                                  u8D7;                                   //年+2000
    u8                                  u8D6;                                   //月
    u8                                  u8D5;                                   //日
    u8                                  u8D4;                                   //星期
    u8                                  u8D3;                                   //时
    u8                                  u8D2;                                   //分
    u8                                  u8D1;                                   //秒
    u8                                  u8D0;                                   //未定义
}__attribute__((packed))stHmiDwinRtc_t;



typedef struct
{
    u8                                  u8Rtc[10];                              //设置的时间存入缓存buf
}__attribute__((packed))stHmiDwinSetRtc_t;




typedef struct
{
    u8                                  u8D3;                                   //0x5A 表示启动一次页面处理， CPU 处理完清零
    u8                                  u8D2;                                   //处理模式,0x01=页面切换
    u8                                  u8D1;                                   //
    u8                                  u8D0;                                   //D1:D0,页面ID 如：0001
}__attribute__((packed))stHmiDwinSetPage_t;




//迪文串口屏协议 payload 定义
typedef union
{
    //迪文公共命名格式
    stHmiDwinFun80_t                    stFun80;
    stHmiDwinFun80Ack_t                 stFun80Ack;                             //应答指令不是按照标准发送，没包头只有0x4F 0x4B 2字节，不确定是否放此
    
    stHmiDwinFun81_t                    stFun81; 
    stHmiDwinFun81Ack_t                 stFun81Ack;
    
    stHmiDwinFun82_t                    stFun82;
    stHmiDwinFun82Ack_t                 stFun82Ack;                             //应答指令不是按照标准发送，没包头只有0x4F 0x4B 2字节，不确定是否放此
    
    stHmiDwinFun83_t                    stFun83;
    stHmiDwinFun83Ack_t                 stFun83Ack;
    
    //应用到的各种参数发送指令
    stHmiDwinRedPage_t                  stHmiDwinRedPage;                       //当前页面ID
    stHmiDwinBrigtness_t                stHmiDwinBrigtness;                     //屏幕亮度返回
    stHmiDwinRtc_t                      stHmiDwinRtc;                           //屏幕当前时间

    //0x82设置系统参数
    stHmiDwinSetPage_t                  stHmiDwinSetPage;                       //设置的页面
    stHmiDwinSetRtc_t                   stHmiDwinSetRtc;                        //设置的时间
    u8                                  u8Buf[cHmiDwinDateBufSize+3];           //256长度够整个包的+变量地址+长度
}__attribute__((packed)) unHmiDwinPayload_t;







//迪文屏串口协议 整个帧结构定义
typedef struct
{
    stHmiDwinrnHead_t                   stHead;
    unHmiDwinPayload_t                  unPayload;
}__attribute__((packed)) stHmiDwinPkt_t;


//迪文屏串口协议 整个帧结构定义---union 形式
typedef union 
{
    stHmiDwinPkt_t                      stPkt;
    u8                                  u8Buf[cHmiDwinDateBufSize+cHmiDwinHeadTailSize];
}__attribute__((packed)) unHmiDwinPkt_t;






//屏幕数据字段
typedef struct
{
    u16                                 u16nowpage;                             //当前页面
    u8                                  u8nextpage;                             //当前页面
    u8                                  u8prepage;                              //当前页面
    stTime_t                            stTime;                                 //日期时间
    u8                                  u8Brightness;                           //当前亮度
    u16                                 u16SwVer;                               //软件版本号
}stHmiDwinrnData_t;





//迪文显示屏驱动层 数据结构
//缓存驱动层所需的一些数据并返回给上层
typedef struct
{
    //收发资源
    u8                                  u8Buf[cHmiDwinDateBufSize];             //接收buf
    unHmiDwinPkt_t                      unTxPkt;                                //发送buf
    SemaphoreHandle_t                   xSemDrvTxPkt;
    SemaphoreHandle_t                   xSemTxPkt;
    
    //断帧资源
    i32                                 i32RxIndex;
    unHmiDwinPkt_t                      unRxPkt;
    unHmiDwinPkt_t                      unRxPktTemp;
    
    //命令应答信号量
    SemaphoreHandle_t                   xSemAck;
    
    //屏幕相关数据
    stHmiDwinrnData_t                   stData;
}stHmiDwinrnCache_t;





//函数指针声明
typedef void (*DwinFuncCmd)(u8 cmd, u8 type,u8 len,u8 *data); 




typedef enum
{
    eHmiDwinSdStep0                     = 0,                                    //
    eHmiDwinSdStep1                     = 1,                                    //第一步查询页面
    eHmiDwinSdStep2                     = 2,                                    //第二步设置页面--页面设置单独一个线程快速切换
    eHmiDwinSdStep3                     = 3,                                    //第三步读取屏幕时间
    eHmiDwinSdStep4                     = 4,                                    //第四步当前页面需要显示的内容
    eHmiDwinSdStep5                     = 5,                                    //第五步发送公用地址显示的数据
    eHmiDwinSdStep6                     = 6,                                    //第六步发送校时
    
    eHmiDwinSdStepMax,
}__attribute__((packed)) eHmiDwinSdStep_t;




//迪文屏幕发生一个事件（按键、录入等）时，将数据按照此格式组合入列提供给应用层去做对应的处理
typedef struct
{
    u16                                 u16page;                                //当前屏幕所处的页面
    u16                                 u16addr;                                //当前接收到数据的地址
    u8                                  u8len;                                  //接收到的数据的长度
    u8                                  u8Buf[cHmiDwinDateBufSize];             //屏幕发送的数据
}stHmiDwinEvent_t;



typedef struct
{
    //迪文串口返回数据缓存队列--非系统变量地址返回数据，需要用户层去处理
    stQueue_t                           stEventQueue;                         //日志队列
    SemaphoreHandle_t                   xSemEventQueue;                       //日志队列访问信号量
    stHmiDwinEvent_t                    stEvent;                              //日志生成buf---此buf同样由 xSemLogQueue 保护
    
}stHmiDwinRcvCache_t;







extern bool sHmiDwinWrData(u16 u16addr, const u8 *pBuf, u8 u8len);
extern bool sHmiDrvDwinInit(void);
extern bool sHmiDwinSendCmd(u16 u16Len, const u8 *pBuf);



#endif
















