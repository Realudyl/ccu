/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_common.h
* Description                           :   通用接口实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-20
* notice                                :   
****************************************************************************************************/
#ifndef _en_common_H_
#define _en_common_H_
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "lwip/dhcp.h"
#include "lwip/etharp.h"
#include "lwip/icmp6.h"
#include "lwip/inet.h"
#include "lwip/init.h"
#include "lwip/ip6.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "netif/ethernet.h"

#include "en_md5.h"
#include "en_common_eth.h"
#include "en_chg_sdk_cfg.h"






//内网私有协议--->通讯的方向
#define cSdkPrivDevTypeM                0                                       //内网协议作为网关端运行
#define cSdkPrivDevTypeS                1                                       //内网协议作为  桩端运行
#define cSdkPrivDevType                 cSdkPrivDevTypeS





//数据类型定义
typedef unsigned int                    size_t;
typedef unsigned char                   byte;


typedef signed char                     int8_t;
typedef short                           int16_t;
typedef int                             int32_t;
typedef long long                       int64_t;
typedef unsigned char                   uint8_t;
typedef unsigned short                  uint16_t;
typedef unsigned int                    uint32_t;
typedef unsigned long long              uint64_t;

typedef uint64_t                        u64;
typedef uint32_t                        u32;
typedef uint16_t                        u16;
typedef uint8_t                         u8;

typedef int64_t                         i64;
typedef int32_t                         i32;
typedef int16_t                         i16;
typedef int8_t                          i8;

typedef double                          f64;
typedef float                           f32;





//通用字符定义
#define cCr                             (0x0D)                                  // \r
#define cLf                             (0x0A)                                  // \n
#define cCrLf                           "\r\n"
#define cCrLfCrLf                       "\r\n\r\n"
#define cCtrlZ                          (0x1A)                                  // CTRL+Z

//通用长度定义
#define cIpv6AddrLen                    (16)
#define cMd5Len                         (16)                                    //16字节 MD5 校验值长度
#define cMd5StringLen                   (32)                                    //16字节 MD5 校验值长度--字符串形式




//通用运算定义
#define sArraySize(x)                   (sizeof(x) / sizeof(x[0]))

//计算是否是润年
#define is_leap_year(y)                 (((y) % 4  == 0 && (y) % 100 != 0) || (y) % 400 == 0)

#define IS_STRING(ch)                   (((ch) <= '~') ? true : false)
#define IS_DIGIT(ch)                    ((((ch) >= '0') && ((ch) <= '9')) ? true : false)
#define IS_CHAR(ch)                     (((((ch) >= 'A') && ((ch)<='Z')) || (((ch) >= 'a') && ((ch) <= 'z'))) ? true : false)
#define IS_HEX_STR(ch)                  ((IS_DIGIT(ch)) || (IS_CHAR(ch))) 

#define GET_BYTE(Byte,Bit)              ((Byte) & (0x0001 << (Bit)))
#define SET_BYTE(Byte,Bit)              ((Byte) |=  (0x0001 << (Bit)))
#define CLR_BYTE(Byte,Bit)              ((Byte) &= (~(0x0001 << (Bit))))

#define MAKE_UINT16(d1, d2)             (((d1) << 8) | ((d2)))
#define MAKE_UINT32(d1, d2, d3, d4)     (((u32)(d1) << 24) | ((u32)(d2) << 16) | ((u32)(d3) << 8) | ((u32)(d4)))


// 短整型大小端互换
#define BigLittleSwap16(A)              ((((u16)(A) & 0xff00) >> 8) | (((u16)(A) & 0x00ff) << 8))

// 长整型大小端互换
#define BigLittleSwap32(A)              ((((u32)(A) & 0xff000000) >> 24) | (((u32)(A) & 0x00ff0000) >>  8) | (((u32)(A) & 0x0000ff00) <<  8) | (((u32)(A) & 0x000000ff) << 24))








//--------------------------------------通用结构体定义--------------------------

//年份取缩略值(十位+个位)的日期定义
typedef struct
{
    u8                                  year;
    u8                                  month;
    u8                                  day;
}__attribute__((packed)) stDate_t;
//年份取完整值的日期定义
typedef struct
{
    u16                                 year;
    u16                                 month;
    u16                                 day;
}__attribute__((packed)) stFullDate_t;


//日期相关长度定义
#define cDateLen                        3                                       //年份取缩略值的时间结构体长度
#define cFullDateLen                    4                                       //年份取完整值的时间结构体长度
#define cDateStrLen                     6                                       //字符串YYMMDD长度   例如210830
#define cFullDateStrLen                 8                                       //字符串YYYYMMDD长度 例如20210830


//年份取缩略值(十位+个位)的时间结构体定义
typedef struct
{
    u8                                  year;
    u8                                  mon;
    u8                                  day;
    u8                                  hour;
    u8                                  min;
    u8                                  sec;
}__attribute__((packed)) stTime_t;

//年份取完整值的时间结构体定义
typedef struct
{
    u16                                 year;
    u8                                  mon;
    u8                                  day;
    u8                                  hour;
    u8                                  min;
    u8                                  sec;
}__attribute__((packed)) stFullTime_t;


//时间相关长度定义
#define cTimeLen                        6                                       //年份取缩略值的时间结构体长度
#define cFullTimeLen                    7                                       //年份取完整值的时间结构体长度
#define cTimeStrLen                     12                                      //字符串YYMMDDhhmmss长度   例如210830144600
#define cFullTimeStrLen                 14                                      //字符串YYYYMMDDhhmmss长度 例如20210830144600


//年份取缩略值(十位+个位)的时间 联合结构体定义
typedef union
{
    stTime_t                            rtc;
    u8                                  rtc_buf[cTimeLen];
}__attribute__((packed)) unTime_t;

//年份取完整值的时间 联合结构体定义
typedef union
{
    stFullTime_t                        rtc;
    u8                                  rtc_buf[cFullTimeLen];
}__attribute__((packed)) unFullTime_t;



//世界协调时间(UTC)/格林尼治时间(GMT) 时区枚举
typedef enum
{
    eTimeZoneUtcMin                     = (-13),
    
    eTimeZoneUtcNeg12                   = (-12),
    eTimeZoneUtcNeg11                   = (-11),
    eTimeZoneUtcNeg10                   = (-10),
    eTimeZoneUtcNeg9                    = (-9 ),
    eTimeZoneUtcNeg8                    = (-8 ),
    eTimeZoneUtcNeg7                    = (-7 ),
    eTimeZoneUtcNeg6                    = (-6 ),
    eTimeZoneUtcNeg5                    = (-5 ),
    eTimeZoneUtcNeg4                    = (-4 ),
    eTimeZoneUtcNeg3                    = (-3 ),
    eTimeZoneUtcNeg2                    = (-2 ),
    eTimeZoneUtcNeg1                    = (-1 ),
    eTimeZoneUtcZero                    = ( 0 ),
    eTimeZoneUtcPos1                    = ( 1 ),
    eTimeZoneUtcPos2                    = ( 2 ),
    eTimeZoneUtcPos3                    = ( 3 ),
    eTimeZoneUtcPos4                    = ( 4 ),
    eTimeZoneUtcPos5                    = ( 5 ),
    eTimeZoneUtcPos6                    = ( 6 ),
    eTimeZoneUtcPos7                    = ( 7 ),
    eTimeZoneUtcPos8                    = ( 8 ),
    eTimeZoneUtcPos9                    = ( 9 ),
    eTimeZoneUtcPos10                   = ( 10),
    eTimeZoneUtcPos11                   = ( 11),
    eTimeZoneUtcPos12                   = ( 12),
    
    eTimeZoneUtcMax                     = ( 13)
    
}__attribute__((packed)) eTimeZone_t;





/*IEEE754浮点数*/
typedef union
{
   f32                                  f32Value;                               //以浮点数访问
   u32                                  u32Value;                               //以整型数访问
   u8                                   u8Buf[4];                               //以4字节8位序列访问
}unIeee754_t; 










extern const u8 u8MacLocalBroadcastAddr[ETH_ALEN];                                    //Mac广播地址








//字符处理
extern i32  sStrToHex(const char *pStr, u8 *pHexs, i32 i32MaxLen);
extern u8   sStr2Bcd(u8* bcd, const u8* str, u8 length);
extern bool sHexToStr(const u8 *pHexs, i32 i32HexLen, char *pStr, bool bCapitalFlag);
extern bool sIpIsString(const char *pIpIn);
extern bool sSnIsValid(const u8 *pSn, u8 u8Len);
extern bool sSequeIsString(const u8 *pSeque, u16 u16Len);

//HEX转换
extern u8   sHex2Bcd(u8 hex);
extern u16  sHex16ToBcd(u16 u16Hex);

extern int  sHexToDec(char c);

//DEC转换
extern char sDecToHex(short int c);

//BCD码转换
extern u8   sBcd2Hex(u8 bcd);
extern u8   sBcd2Str(u8* deststr, const u8* bcd, u8 length);


//版本号字符串转换十进制数
extern i32 sVersionToDec(char *pStr);

//校验算法
extern u8   sCheckBcc8(const u8 *pBuf , u16 u16Len);
extern u8   sCheckSum8(const u8 *pBuf , u16 u16Len);
extern u8   sCheckSum8Neg(const u8 *pBuf, u16 u16Len);
extern u32  sCheckSum16Adj(const char *pBuf, u32 u32Len);
extern u16  sCheckSumPseudoHead(struct in6_addr saddr, struct in6_addr daddr, u16 u16PayloadLen, u16 u16NextHead, u32 u32PkgCheckSum);
extern u16  sCrc16Modbus(const u8 *pBuf, u16 u16Len);
extern u16  sCrc16WithBeginPos(const u8 *data, i32 length, u16 BeginPos);
extern u16  sCrc16WithA001(const u8 *pData, u16 u16Len);
extern bool sFileMd5Calc(const char *pFilePath, char *pMd5);
extern bool sFrameMd5Calc(const u8 *pBuf, u16 u16Len, u8 *pMd5);


//时间处理
extern i32  sGetMinutesFromTime(i32 i32Hour, i32 i32Min);
extern u32  sGetTimestamp(void);
extern u32  sGetTimeByStructTime(stTime_t stTimeStart, stTime_t stTimeStop);
extern void sGetFullTimeString(char *pRtc, int maxlen);
extern bool sSetTime(const stTime_t *pTime);
extern bool sSetFullTime(const stFullTime_t *rtc);
extern bool sSetTimeByTimeStamp(u64 u64TimeStamp);
extern stTime_t sGetTime(void);
extern stFullTime_t sGetFullTime(void);
extern void sTimeStampConvertFullTime(u32 u32Timestamp, stFullTime_t *rtc);



//网络处理
extern void sMacUnicastToMulticast(u8 *pMac, struct in6_addr *pIp6Multicast);
extern void sIp6UnicastToMulticast(struct in6_addr *pUnicast);
extern struct in6_addr sMacToIp6Linklocal(const u8 *pMac);
extern bool sMacAddrCompare(const u8 *pMacDest, struct in6_addr *pIp6Dest, const u8 *pMacLocalUnicast);
extern bool sIp6AddrCompare(struct in6_addr *pIp6Dest, struct in6_addr *pIp6LocalUnicast);
extern bool sIn6_addr_2_Ip6_addr(struct in6_addr *pIn6, ip6_addr_t *pIp6);


extern bool sMakeUuid32(char* pDst, i32 i32MaxLen);



//值比较
extern f32 sGetMaxOfThree(f32 a, f32 b, f32 c);
extern f32 sGetMinOfThree(f32 a, f32 b, f32 c);



//屏幕显示转换格式
void Utf8ToOutUtf32(const char *Utf8Str, uint32_t *Utf32Out);
void DeleteIvaildData(u16 *u16Data, u16 u16Len,u16 *u16OutData);
















#endif /* COMMON_H_ */



