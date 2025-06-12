/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_common.c
* Description                           :   通用接口实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-20
* notice                                :   
****************************************************************************************************/
#include "en_common.h"
#include "en_log.h"

#include "rtc_drv_interface.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "en_common";











//Mac组播地址-----格式
const u8 u8MacMulticastAddrFormat[ETH_ALEN] = 
{0x33,0x33, 0x00, 0x00, 0x00, 0x00};

//Mac广播地址
const u8 u8MacLocalBroadcastAddr[ETH_ALEN] = 
{0xff,0xff, 0xff, 0xff, 0xff, 0xff};

//IPv6组播地址-----格式
const u8 u8Ipv6MulticastAddrFormat[cIpv6AddrLen] = 
{0xff,0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00};

//IPv6链路本地地址-----格式
const u8 u8Ipv6LinklocalAddrFormat[cIpv6AddrLen] = 
{0xfe,0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00};

//本地链路范围内所有节点
const u8 u8Ipv6LinklocalAddrAllNodes[cIpv6AddrLen] =
{0xff,0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

//本地链路范围内所有路由器
const u8 u8Ipv6LinklocalAddrAllRouter[cIpv6AddrLen] =
{0xff,0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};


u8 u8CrcTabHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

u8 u8CrcTabLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};






static const u16 u16CrcTabXModem[256] = 
{
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,
    0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,0x1231,0x0210,0x3273,0x2252,
    0x52b5,0x4294,0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,
    0xf3ff,0xe3de,0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,0x3653,0x2672,
    0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,0xb75b,0xa77a,0x9719,0x8738,
    0xf7df,0xe7fe,0xd79d,0xc7bc,0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,
    0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,0xdbfd,0xcbdc,
    0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,0x6ca6,0x7c87,0x4ce4,0x5cc5,
    0x2c22,0x3c03,0x0c60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,
    0x8d68,0x9d49,0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,0x9188,0x81a9,
    0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,0x1080,0x00a1,0x30c2,0x20e3,
    0x5004,0x4025,0x7046,0x6067,0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,
    0xe37f,0xf35e,0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,0x34e2,0x24c3,
    0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,0xa7db,0xb7fa,0x8799,0x97b8,
    0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,
    0x4615,0x5634,0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,0xcb7d,0xdb5c,
    0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,
    0x0af1,0x1ad0,0x2ab3,0x3a92,0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,
    0x9de8,0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,
    0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};









//字符处理
i32  sStrToHex(const char *pStr, u8 *pHexs, i32 i32MaxLen);
u8   sStr2Bcd(u8* bcd, const u8* str, u8 length);
bool sHexToStr(const u8 *pHexs, i32 i32HexLen, char *pStr, bool bCapitalFlag);
bool sIpIsString(const char *pIpIn);
bool sSnIsValid(const u8 *pSn, u8 u8Len);
bool sSequeIsString(const u8 *pSeque, u16 u16Len);

//HEX转换
u8   sHex2Bcd(u8 hex);
u16  sHex16ToBcd(u16 u16Hex);
int  sHexToDec(char c);

//DEC转换
char sDecToHex(short int c);

//BCD码转换
u8   sBcd2Hex(u8 bcd);
u8   sBcd2Str(u8* deststr, const u8* bcd, u8 length);


//版本号字符串转换十进制数
i32 sVersionToDec(char *pStr);



//校验算法
u8   sCheckBcc8(const u8 *pBuf , u16 u16Len);
u8   sCheckSum8(const u8 *pBuf , u16 u16Len);
u8   sCheckSum8Neg(const u8 *pBuf, u16 u16Len);
u32  sCheckSum16Adj(const char *pBuf, u32 u32Len);
u16  sCheckSumPseudoHead(struct in6_addr saddr, struct in6_addr daddr, u16 u16PayloadLen, u16 u16NextHead, u32 u32PkgCheckSum);
u16  sCrc16Modbus(const u8 *pBuf, u16 u16Len);
u16  sCrc16WithBeginPos(const u8 *data, i32 length, u16 BeginPos);
u16  sCrc16WithA001(const u8 *pData, u16 u16Len);
u16  sCrc16Xmodem(const u8 *pBuf, u16 u16Len);
bool sFileMd5Calc(const char *pFilePath, char *pMd5);
bool sFrameMd5Calc(const u8 *pBuf, u16 u16Len, u8 *pMd5);


//时间处理
i32  sGetMinutesFromTime(i32 i32Hour, i32 i32Min);
u32  sGetTimestamp(void);
u32  sGetTimeByStructTime(stTime_t stTimeStart, stTime_t stTimeStop);
void sGetFullTimeString(char *pRtc, int maxlen);
bool sSetTime(const stTime_t *pTime);
bool sSetFullTime(const stFullTime_t *rtc);
bool sSetTimeByTimeStamp(u64 u64TimeStamp);
stTime_t sGetTime(void);
stFullTime_t sGetFullTime(void);
void sTimeStampConvertFullTime(u32 u32Timestamp, stFullTime_t *rtc);



//网络处理
void sMacUnicastToMulticast(u8 *pMac, struct in6_addr *pIp6Multicast);
void sIp6UnicastToMulticast(struct in6_addr *pUnicast);
struct in6_addr sMacToIp6Linklocal(const u8 *pMac);
bool sMacAddrCompare(const u8 *pMacDest, struct in6_addr *pIp6Dest, const u8 *pMacLocalUnicast);
bool sIp6AddrCompare(struct in6_addr *pIp6Dest, struct in6_addr *pIp6LocalUnicast);
bool sIn6_addr_2_Ip6_addr(struct in6_addr *pIn6, ip6_addr_t *pIp6);



bool sMakeUuid32(char* pDst, i32 i32MaxLen);



//值比较
f32 sGetMaxOfThree(f32 a, f32 b, f32 c);
f32 sGetMinOfThree(f32 a, f32 b, f32 c);



//屏幕显示转换格式
void Utf8ToOutUtf32(const char *Utf8Str, uint32_t *Utf32Out);
void DeleteIvaildData(u16 *u16Data, u16 u16Len,u16 *u16OutData);







i32 sStrToHex(const char *pStr, u8 *pHexs, i32 i32MaxLen)
{
    u8  u8Temp[3] = {0};
    i32 i32Size = 0;
    const char *pIn = pStr;
    
    while((*pIn) && *(pIn + 1) && IS_HEX_STR(*pIn) && (i32Size <= i32MaxLen))
    {
        strncpy((char*)u8Temp, pIn, 2);
        pHexs[i32Size++] = strtol((char*)u8Temp, 0, 16);
        pIn += 2;
    }
    
    return i32Size;
}




u8 sStr2Bcd(u8* bcd, const u8* str, u8 length)
{
    u8 i, j = 0;
    
    if (!str || !bcd)
    {
        return 0;
    }
    
    j = 0;
    for (i = 0; i < length; i++)
    {
        bcd[j++] = ((str[i * 2] - '0') << 4) | (str[i * 2 + 1] - '0');
    }
    
    return j;
}






/**********************************************************************************************
* Description       :     十六进制数组转字符串
* Author            :     XRG
* modified Date     :     2023-07-19
* notice            :     bCapitalFlaf ：true则默认大写       false为小写
***********************************************************************************************/
bool sHexToStr(const u8 *pHexs, i32 i32HexLen, char *pStr, bool bCapitalFlag)
{
    i32  i;
    char HEXS_CHAR[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};  
    
    if(!bCapitalFlag)//小写
    {
        HEXS_CHAR[10] = 'a';
        HEXS_CHAR[11] = 'b';
        HEXS_CHAR[12] = 'c';
        HEXS_CHAR[13] = 'd';
        HEXS_CHAR[14] = 'e';
        HEXS_CHAR[15] = 'f';
    }
    
    if((pHexs == NULL) || (pStr == NULL))
    {
        EN_SLOGE(TAG,"para is null.");
        return false;
    }
    
    char *p = pStr;
    for(i = 0; i < i32HexLen; i++)
    {
        *p++ = HEXS_CHAR[(pHexs[i] >> 4)& 0x0F];
        *p++ = HEXS_CHAR[pHexs[i] & 0x0F];
    }
    *p++ = '\0';
    
    return true;
}


u8 sHex2Bcd(u8 hex)
{
    u8 bcd;
    hex %= 100;
    bcd = hex / 10;
    bcd <<= 4;
    bcd += hex % 10;
    
    return bcd;
}



u16 sHex16ToBcd(u16 u16Hex)
{
    u16 u16Bcd;
    
    u16Bcd = (u16Hex / 1000) << 12;
    u16Hex %= 1000;
    u16Bcd += (u16Hex / 100) << 8;
    u16Hex %= 100;
    u16Bcd += (u16Hex / 10) << 4;
    u16Hex %= 10;
    u16Bcd += u16Hex;
    
    return u16Bcd;
}






int sHexToDec(char c)
{
    if('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}



char sDecToHex(short int c)
{
    if (0 <= c && c <= 9)
    {
        return c + '0';
    }
    else if (10 <= c && c <= 15)
    {
        return c + 'A' - 10;
    }
    else
    {
        return -1;
    }
}




u8 sBcd2Hex(u8 bcd)
{
    u8 hex;
    hex = (bcd >> 4) * 10;
    hex += bcd & 0x0F;
    
    return hex;
}




u8 sBcd2Str(u8* deststr, const u8* bcd, u8 length)
{
    u8 i, j = 0;
    if (!deststr || !bcd)return 0;
    
    for (i = 0; i < length; i++)
    {
        deststr[j++]= (u8)(((bcd[i] >> 4) & 0x0f) + '0');
        deststr[j++]= (u8)((bcd[i] & 0x0f) + '0');
    }
    deststr[j++] = 0;    // 字符串结束符
    
    return j;
}





/*******************************************************************************
 * @FunctionName   :      sVersionToDec
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月20日  14:34:49
 * @Description    :      版本号字符串转换为十进制数    e.g.:("1.0.1" = 101)
 * @Input          :      pStr        
 * @Return         :      
*******************************************************************************/
i32 sVersionToDec(char *pStr)
{
    i32 i32Dec = 0;
    char *pToken = NULL;
    char *pDelimiter = ".";
    
    // 使用strtok函数按照"."分割字符串
    pToken = strtok(pStr, pDelimiter);
    
    while (pToken != NULL)
    {
        // 将分割出的字符串转换为整数
        int num = atoi(pToken);
        
        // 将转换后的数字累加到result中
        i32Dec = i32Dec * 10 + num;
        
        // 继续分割下一个部分
        pToken = strtok(NULL, pDelimiter);
    }
    
    return i32Dec;
}





/***************************************************************************************************
* Description                           :   8位 BCC校验(异或校验)
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :   
****************************************************************************************************/
u8 sCheckBcc8(const u8 *pBuf , u16 u16Len)
{
    u16 i;
    u8  u8CheckBcc = 0;
    
    for(i = 0; i < u16Len; i++)
    {
        u8CheckBcc ^= pBuf[i];
    }
    
    return(u8CheckBcc);
}






/***************************************************************************************************
* Description                           :   8位校验和
* Author                                :   Hall
* Creat Date                            :   2023-07-21
* notice                                :   
****************************************************************************************************/
u8 sCheckSum8(const u8 *pBuf , u16 u16Len)
{
    u16 i;
    u8  u8CheckSum = 0;
    
    for(i = 0; i < u16Len; i++)
    {
        u8CheckSum += pBuf[i];
    }
    
    return(u8CheckSum);
}




/***************************************************************************************************
* Description                           :   结果取反的 8位校验和
* Author                                :   Hall
* Creat Date                            :   2022-07-20
* notice                                :   算法有特殊，结果取负数才返回的  给HMI串口通讯专用
****************************************************************************************************/
u8 sCheckSum8Neg(const u8 *pBuf, u16 u16Len)
{
    u16 i;
    u8 u8Chksum;
    
    u8Chksum = 0;
    for(i=0; i<u16Len; i++)
    {
        u8Chksum = u8Chksum + *(pBuf + i);
    }
    u8Chksum = (0 - u8Chksum) & 0xff;
    
    return u8Chksum;
}






/***************************************************************************************************
* Description                           :   带长度调整的16位和校验计算---网络帧checksum计算专用
* Author                                :   Hall
* Creat Date                            :   2022-06-23
* notice                                :   应该按照word求和，如果外部输入的byte数为奇数,应补齐偶数
*                                           外部调用应保证补齐的地址 值为0---这种情况尚未验证
****************************************************************************************************/
u32 sCheckSum16Adj(const char *pBuf, u32 u32Len)
{
    i32 i;
    u16 *pWord;
    u32 u32LenWord = 0;
    u16 u16Temp = 0;
    u32 u32Sum = 0;
    
    u32LenWord = (u32Len >> 1) + (u32Len & 1); //byte长度转换为word长度
    pWord = (u16*)(pBuf);
    
    for(i=0; i<u32LenWord; i++)
    {
        //由于 pBuf 内的16位值已经按照网络字节序填充 
        //所以 应该转换求和
        u16Temp = ntohs(*(pWord + i));
        u32Sum += u16Temp;
    }
    
    return(u32Sum);
}




/***************************************************************************************************
* Description                           :   包含伪首部的和校验计算---网络帧checksum计算专用
* Author                                :   Hall
* Creat Date                            :   2022-06-23
* notice                                :   u32PkgCheckSum： 报文部分校验和
*
*                                           以下Ipv6报文头的伪首部部分：
*                                           saddr         ：  源地址
*                                           daddr         ： 目的地址
*                                           u16PayloadLen ： payload length
*                                           u16NextHead   ：  NextHead
****************************************************************************************************/
u16 sCheckSumPseudoHead(struct in6_addr saddr, struct in6_addr daddr, u16 u16PayloadLen, u16 u16NextHead, u32 u32PkgCheckSum)
{
    i32 i;
    u16 u16Sum;
    u32 u32Sum = 0;
    
    u32Sum = u32PkgCheckSum + u16PayloadLen + u16NextHead;
    
    for(i=0; i<(cIpv6AddrLen / 2); i++)
    {
        //由于 pBuf 内的16位值已经按照网络字节序填充 
        //所以 应该转换求和
        u32Sum = u32Sum + ntohs(saddr.s6_addr16[i]) + ntohs(daddr.s6_addr16[i]);
    }
    
    //疑点标注：这里没理解错误吧
    while(u32Sum > 0xffff)
    {
        u32Sum = (u32Sum >> 16) + (u32Sum & 0xffff);
    }
    u16Sum = u32Sum;
    u16Sum = ~u16Sum;
    
    return(u16Sum);
}






/***************************************************************************************************
* Description                           :   16位CRC计算----for modbus协议
* Author                                :   Hall
* Creat Date                            :   2022-08-10
* notice                                :   查表法 16位CRC
*                                           与 CRC计算工具V3.4.0-64选择 CRC16-(Modbus) 方式 一致
****************************************************************************************************/
u16 sCrc16Modbus(const u8 *pBuf, u16 u16Len)
{
    u8 u8Hi = 0xFF;  //高字节初始化值
    u8 u8Lo = 0xFF;  //低字节初始化值
    u16 u16Index;
    
    while(u16Len--) 
    {
        u16Index = u8Lo ^ (*pBuf++);
        u8Lo = u8Hi ^ u8CrcTabHi[u16Index];
        u8Hi = u8CrcTabLo[u16Index];
    }
    return (u8Hi << 8 | u8Lo);
}





u16 sCrc16WithBeginPos(const u8 *data, i32 length, u16 BeginPos)
{
    unsigned  short  reg_crc;
    unsigned  short  s_crcchk;
    const u8 *pData = NULL;
    
    pData = data + BeginPos;
    s_crcchk  =  0;
    reg_crc  =  0x0;
    while (length--)
    {
        //reg_crc  ^=  *data++;
        reg_crc  ^=  *pData++;
        for (s_crcchk = 0; s_crcchk < 8; s_crcchk++)
        {
            //reg_crc=reg_crc>>1;
            if (reg_crc  &  0x01)
            {
                reg_crc  =  (reg_crc  >>  1) ^ 0xa001;
            }
            else
            {
                reg_crc  =  reg_crc  >>  1;
            }
        }
    }
    return  reg_crc;
}

/***************************************************************************************************
* Description                           :   16位CRC校验  
* Author                                :   Hall
* Creat Date                            :   2023-06-08
* notice                                :   从桩端代码移植过来，他们标准的CRC计算函数，
*                                           与 CRC计算工具V3.4.0-64选择 CRC16-(0xA001) 方式 一致
****************************************************************************************************/
u16 sCrc16WithA001(const u8 *pData, u16 u16Len)
{
    return sCrc16WithBeginPos(pData, u16Len, 0);
}







/***************************************************************************************************
* Description                           :   16位CRC校验  
* Author                                :   Hall
* Creat Date                            :   2023-06-08
* notice                                :   CRC16 算法（CRC - 16 / XMODEM x16 + x12 + x5 + 1）：
*                                           与 CRC计算工具V3.4.0-64选择 CRC-CCITT(XModem) 方式 一致
****************************************************************************************************/
u16 sCrc16Xmodem(const u8 *pBuf, u16 u16Len)//CRC沉余校验 
{
    u8  da;
    u16 crc = 0;
    
    while(u16Len--) 
    {
        da = crc >> 8;
        crc <<= 8;
        crc ^= u16CrcTabXModem[da ^ *pBuf++];
    }
    
    return crc;
}





/**********************************************************************************************
* Description                           :   计算指定文件的MD5值
* Author                                :   Hall
* modified Date                         :   2022-11-11
* notice                                :   本函数内 读取文件内容时 一次读取4096字节
***********************************************************************************************/
bool sFileMd5Calc(const char *pFilePath, char *pMd5)
{
/*
    FILE *pFile = NULL;
    u32 u32Temp = 0;
    u32 u32ReadLen = 0;
    i32 i32FileSize = 0;
    u32 u32ReadTotalLen = 0;
    
    md5_context md5;
    u8 u8Code[cMd5Len];
    
    static char *pMd5Buf = NULL;
    
    if(!pFilePath)
    {
        return false;
    }
    EN_SLOGI(TAG, "计算文件 %s 的MD5值", pFilePath);
    
    if(pMd5Buf == NULL)
    {
        pMd5Buf = MALLOC(4096);
    }
    
    i32FileSize = sGetFileSize(pFilePath);
    if(i32FileSize <= 0)
    {
        EN_SLOGE(TAG, "u32FileSize error!");
        return false;
    }
    
    pFile = fopen(pFilePath, "rb");
    if(pFile == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", pFilePath);
        return false;
    }
    
    memset(&md5, 0, sizeof(md5));
    
    md5_starts(&md5);
    u32ReadTotalLen = 0;
    while(u32ReadTotalLen < i32FileSize)
    {
        u32ReadLen = (4096 < (i32FileSize - u32ReadTotalLen)) ? 4096 : (i32FileSize - u32ReadTotalLen);
        u32Temp = fread(pMd5Buf, 1, u32ReadLen, pFile);
        if(u32Temp == u32ReadLen)
        {
            md5_update(&md5, (u8*)pMd5Buf, u32ReadLen); 
            memset(pMd5Buf, 0, 4096);
            u32ReadTotalLen = u32ReadTotalLen + u32ReadLen;
        }
        else
        {
            EN_SLOGE(TAG, "fread file:%s error!", pFilePath);
            break;
        }
    }
    
    md5_finish(&md5, u8Code);
    EN_HLOGI(TAG, "count md5: ", u8Code, cMd5Len);
    if(pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    
    sHexsToString(u8Code, cMd5Len, pMd5, false);
    */
    return true;
}





/**********************************************************************************************
* Description                           :   计算一帧数据的MD5值
* Author                                :   Hall
* modified Date                         :   2022-11-11
* notice                                :   
***********************************************************************************************/
bool sFrameMd5Calc(const u8 *pBuf, u16 u16Len, u8 *pMd5)
{
    md5_context md5;
    u8 u8Code[cMd5Len];
    
    memset(&md5, 0, sizeof(md5));
    
    md5_starts(&md5);
    md5_update(&md5, (u8*)pBuf, u16Len); 
    
    md5_finish(&md5, u8Code);
    EN_HLOGI(TAG, "count md5: ", u8Code, cMd5Len);
    
    
    memcpy(pMd5, u8Code, sizeof(u8Code));
    
    return true;
}







/***************************************************************************************************
* Description                           :   将小时+分钟转换为分钟数
* Author                                :   Hall
* Creat Date                            :   2024-05-23
* notice                                :   
****************************************************************************************************/
i32 sGetMinutesFromTime(i32 i32Hour, i32 i32Min)
{
    return (i32Hour * 60 + i32Min);
}









u32 sGetTimestamp(void)
{
    return sRtcGetTimeStamp();
}






/**********************************************************************************************
* Description       :     计算2个时间结构体之间的秒数
* Author            :     Hall
* modified Date     :     2024-05-30
* notice            :     
***********************************************************************************************/
u32 sGetTimeByStructTime(stTime_t stTimeStart, stTime_t stTimeStop)
{
    struct tm t1 =
    {
        .tm_year    = stTimeStart.year + (2000  - 1900),
        .tm_mon     = stTimeStart.mon - 1,
        .tm_mday    = stTimeStart.day,
        .tm_hour    = stTimeStart.hour,
        .tm_min     = stTimeStart.min,
        .tm_sec     = stTimeStart.sec
    };
    struct tm t2 =
    {
        .tm_year    = stTimeStop.year + (2000  - 1900),
        .tm_mon     = stTimeStop.mon - 1,
        .tm_mday    = stTimeStop.day,
        .tm_hour    = stTimeStop.hour,
        .tm_min     = stTimeStop.min,
        .tm_sec     = stTimeStop.sec
    };
    
    
    //将N时区时间戳还原为零时区时间戳
    return (mktime(&t2) > mktime(&t1)) ? (mktime(&t2) - mktime(&t1)) : (mktime(&t1) - mktime(&t2));
}






//获取年月日时分秒的字符串
void sGetFullTimeString(char *pRtc, int maxlen)
{
    stFullTime_t p = sGetFullTime();
    
    if(p.sec < 59)//避免秒值满60进位 因为进位逻辑太麻烦
    {
        p.sec = p.sec + 1;//将秒值补偿1,弥补无毫秒值可能造成的误差
    }
    snprintf(pRtc, maxlen, "%04d%02d%02d%02d%02d%02d", p.year, p.mon, p.day, p.hour, p.min, p.sec);
}





//利用 年份取时间结构体 对时
bool sSetTime(const stTime_t *pTime)
{
    i64  i64Time;
    struct tm t;
    
    
    t.tm_year = pTime->year + 2000 - 1900;
    t.tm_mon  = pTime->mon - 1;
    t.tm_mday = pTime->day;
    t.tm_hour = pTime->hour;
    t.tm_min  = pTime->min;
    t.tm_sec  = pTime->sec;
    
    //将 pTime 调整到格林尼治时间
    i64Time   = mktime(&t) - (sRtcGetTimeZone() * 60 * 60);
    if(i64Time < 0)
    {
        i64Time = 0;
    }
    
    EN_SLOGI(TAG, "%d-%02d-%02d,%02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    EN_SLOGI(TAG, "时间戳:%d", (u32)i64Time);
    
    
    if(sRtcSetByTimeStamp(i64Time) != true)
    {
        EN_SLOGE(TAG, "Set system datatime error!/n");
        return false;
    }
    
    return true;
}






//利用 年份取完整值的时间结构体 对时
bool sSetFullTime(const stFullTime_t *rtc)
{
    i64  i64Time;
    struct tm t;
    
    
    t.tm_year = rtc->year - 1900;
    t.tm_mon  = rtc->mon  - 1;
    t.tm_mday = rtc->day;
    t.tm_hour = rtc->hour;
    t.tm_min  = rtc->min;
    t.tm_sec  = rtc->sec;
    
    //将 pTime 调整到格林尼治时间
    i64Time   = mktime(&t) - (sRtcGetTimeZone() * 60 * 60);
    if(i64Time < 0)
    {
        i64Time = 0;
    }
    
    EN_SLOGI(TAG, "%d-%02d-%02d,%02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    EN_SLOGI(TAG, "时间戳:%d", (u32)i64Time);
    
    
    if(sRtcSetByTimeStamp(i64Time) != true)
    {
        EN_SLOGE(TAG, "Set system datatime error!/n");
        return false;
    }
    
    return true;
}







/**********************************************************************************************
* Description       :     利用时间戳对时
* Author            :     Hall
* modified Date     :     2022-11-14
* notice            :     
***********************************************************************************************/
bool sSetTimeByTimeStamp(u64 u64TimeStamp)
{
    return sRtcSetByTimeStamp(u64TimeStamp);
}



//获取 年份取缩略值(十位+个位)的时间结构体
stTime_t sGetTime(void)
{
    stTime_t stTime;
    unFullTime_t unFullTime;
    
    sRtcGetCalendarTime(&unFullTime);
    
    stTime.year = unFullTime.rtc.year - 2000;//stTime.year 等于实际年份减去2000
    stTime.mon  = unFullTime.rtc.mon ;
    stTime.day  = unFullTime.rtc.day;
    stTime.hour = unFullTime.rtc.hour;
    stTime.min  = unFullTime.rtc.min;
    stTime.sec  = unFullTime.rtc.sec;
    
    return stTime;
}






//获取 年份取完整值的时间结构体
stFullTime_t sGetFullTime(void)
{
    unFullTime_t unFullTime;
    
    sRtcGetCalendarTime(&unFullTime);
    
    return unFullTime.rtc;
}



/**********************************************************************************************
* Description       :     公共组件-时间戳转标准时间
* Author            :     XRG
* modified Date     :     2024-03-12
* notice            :     时间戳转标准时间1710237677-->2024-03-12 18:01:17
***********************************************************************************************/
void sTimeStampConvertFullTime(u32 u32Timestamp, stFullTime_t *rtc)
{
    struct tm *tm_now;
    
    tm_now = localtime((const time_t *)&u32Timestamp);
    
    rtc->year = tm_now->tm_year + 1900;
    rtc->mon  = tm_now->tm_mon + 1;
    rtc->day  = tm_now->tm_mday;
    rtc->hour = tm_now->tm_hour;
    rtc->min  = tm_now->tm_min;
    rtc->sec  = tm_now->tm_sec;
}











/***************************************************************************************************
* Description                           :   Mac 组播地址生成
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
void sMacUnicastToMulticast(u8 *pMac, struct in6_addr *pIp6Multicast)
{
    // IPv6 下 Mac组播地址计算 需要用到对应于该Mac地址的 IPv6组播地址 pIp6Multicast
    *(pMac + 0) = u8MacMulticastAddrFormat[0];
    *(pMac + 1) = u8MacMulticastAddrFormat[1];
    *(pMac + 2) = pIp6Multicast->s6_addr[12];
    *(pMac + 3) = pIp6Multicast->s6_addr[13];
    *(pMac + 4) = pIp6Multicast->s6_addr[14];
    *(pMac + 5) = pIp6Multicast->s6_addr[15];
}




/***************************************************************************************************
* Description                           :   Ipv6 组播地址生成
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
void sIp6UnicastToMulticast(struct in6_addr *pUnicast)
{
    i32 i;
    
    for(i=0; i<(cIpv6AddrLen - 3); i++)
    {
        pUnicast->s6_addr[i] = u8Ipv6MulticastAddrFormat[i];
    }
}




/***************************************************************************************************
* Description                           :   Mac地址转换IPv6本地链路地址
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
struct in6_addr sMacToIp6Linklocal(const u8 *pMac)
{
    i32 i;
    struct in6_addr addr;
    
    for(i=0; i<cIpv6AddrLen; i++)
    {
        addr.s6_addr[i] = u8Ipv6LinklocalAddrFormat[i];
    }
    addr.s6_addr[8]  = (((*pMac) & 0x02) == 0x02) ? ((*pMac) - 0x02) : ((*pMac) + 0x02);
    addr.s6_addr[9]  = *(pMac + 1);
    addr.s6_addr[10] = *(pMac + 2);
    addr.s6_addr[13] = *(pMac + 3);
    addr.s6_addr[14] = *(pMac + 4);
    addr.s6_addr[15] = *(pMac + 5);
    
    return(addr);
}



/***************************************************************************************************
* Description                           :   比较Mac目的地址与本机Mac地址是否匹配
* Author                                :   Hall
* Creat Date                            :   2022-07-02
* notice                                :   pMacLocalUnicast:需传入本机Mac单播地址
*                                           pIp6Dest  需传入与pMacDest一起的 IPv6目的地址
****************************************************************************************************/
bool sMacAddrCompare(const u8 *pMacDest, struct in6_addr *pIp6Dest, const u8 *pMacLocalUnicast)
{
    bool bRst = false;
    u8   u8MacLocalMulticast[ETH_ALEN] = {0};                                   //Mac组播地址
    
    //根据 pIp6Dest 计算 Mac组播地址
    memset(u8MacLocalMulticast, 0, sizeof(u8MacLocalMulticast));
    if(pIp6Dest != NULL)
    {
        sMacUnicastToMulticast(u8MacLocalMulticast, pIp6Dest);
    }
    else
    {
        //pIp6Dest 为NULL时 输出 Mac广播地址
        memcpy(u8MacLocalMulticast, u8MacLocalBroadcastAddr, ETH_ALEN);
    }
    
    bRst = false;
    if((memcmp(pMacDest, pMacLocalUnicast, ETH_ALEN) == 0)                      //本机单播Mac地址
    || (memcmp(pMacDest, u8MacLocalMulticast, ETH_ALEN) == 0)                   //    组播
    || (memcmp(pMacDest, u8MacLocalBroadcastAddr, ETH_ALEN) == 0))              //    广播
    {
        bRst = true;
    }
    
    return(bRst);
}




/***************************************************************************************************
* Description                           :   比较IPv6目的地址与本机IPv6地址是否匹配
* Author                                :   Hall
* Creat Date                            :   2022-07-02
* notice                                :   pIp6LocalUnicast:需传入本机IPv6单播地址
****************************************************************************************************/
bool sIp6AddrCompare(struct in6_addr *pIp6Dest, struct in6_addr *pIp6LocalUnicast)
{
    u8 u8Len= 0;
    bool bRst = false;
    struct in6_addr stIp6LocalMulticast;
    
    memset(&stIp6LocalMulticast, 0, sizeof(stIp6LocalMulticast));
    memcpy(&stIp6LocalMulticast, pIp6LocalUnicast, sizeof(struct in6_addr));
    sIp6UnicastToMulticast(&stIp6LocalMulticast);
    
    u8Len = sizeof(struct in6_addr);
    bRst = false;
    
    if((memcmp(pIp6Dest->s6_addr, pIp6LocalUnicast->s6_addr, u8Len) == 0)       //本机单播Mac地址
    || (memcmp(pIp6Dest->s6_addr, stIp6LocalMulticast.s6_addr, u8Len) == 0)     //    组播
    || (memcmp(pIp6Dest->s6_addr, u8Ipv6LinklocalAddrAllNodes, u8Len) == 0))    //本地链路范围所有节点
    {
        bRst = true;
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   比较IPv6目的地址与本机IPv6地址是否匹配
* Author                                :   Hall
* Creat Date                            :   2022-07-02
* notice                                :   pIp6LocalUnicast:需传入本机IPv6单播地址
****************************************************************************************************/
bool sIn6_addr_2_Ip6_addr(struct in6_addr *pIn6, ip6_addr_t *pIp6)
{
    // 使用 inet_ntop 函数将 struct in6_addr 转换为字符串表示形式
    char buf[INET6_ADDRSTRLEN];
    
    inet_ntop(AF_INET6, pIn6, buf, sizeof(buf));
    
    // 使用 ip6addr_aton 函数将字符串表示形式转换为 ip6_addr_t 结构体
    ip6addr_aton(buf, pIp6);
    
    return(true);
}






bool sMakeUuid32(char* pDst, i32 i32MaxLen)
{
    u32 i;
    u32 u32Uuid[4];
    
    if((NULL == pDst) || (i32MaxLen < 32))
    {
        return false;
    }
    
    
    for(i = 0;i < 4;i++)
    {
        u32Uuid[i] = rand();
    }
    snprintf(pDst, i32MaxLen,"%08x%08x%08x%08x", u32Uuid[0], u32Uuid[1], u32Uuid[2], u32Uuid[3]);
    
    return true;
}






/*******************************************************************************
 * @FunctionName   :      sGetMaxOfThree
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月8日  15:26:34
 * @Description    :      获取三个数中的最大值
 * @Input          :      a           参数1
 * @Input          :      b           参数2
 * @Input          :      c           参数3
 * @Return         :      
*******************************************************************************/
f32 sGetMaxOfThree(f32 a, f32 b, f32 c)
{
    return ((a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c));
}




/*******************************************************************************
 * @FunctionName   :      sGetMinOfThree
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月8日  15:26:37
 * @Description    :      获取三个数中的最小值
 * @Input          :      a           参数1
 * @Input          :      b           参数2
 * @Input          :      c           参数3
 * @Return         :      
*******************************************************************************/
f32 sGetMinOfThree(f32 a, f32 b, f32 c)
{
    return ((a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c));
}





/***************************************************************************************************
* Description                           :   功能函数-将UTF-8 编码的字符串转换为 Unicode(即 UTF-32)
* Author                                :   HH
* Creat Date                            :   2022-08-10
* notice                                :   以下是一个在 C 语言中将 UTF-8 编码的字符串转换为 Unicode 代码点(即 UTF-32)
的示例代码。这个示例代码使用了标准库函数和一些基本的位运算来进行转换
转换过程中
单字节 UTF-8 字符（ASCII 范围）。
双字节 UTF-8 字符（范围 U+0080 到 U+07FF）。
三字节 UTF-8 字符（范围 U+0800 到 U+FFFF）
因此转换过程中可能遇到0000的数据，需要特殊处理掉
示例：“急停按键被按下”正常转换成Unicode去显示汉字，需要给屏幕传输 6025 505c 6309 952e 88ab 6309 4e0b
但是通过此功能函数转换结果是:6025 0000 505c 0000 6309 0000 952e 0000 88ab 0000 6309 0000 4e0b 0000 
转换后需要将0000剔除掉
****************************************************************************************************/
void Utf8ToOutUtf32(const char *Utf8Str, uint32_t *Utf32Out) 
{
    const unsigned char *ptr = (const unsigned char *)Utf8Str;
    uint32_t *out = Utf32Out;
    
    while (*ptr) 
    {
        uint32_t codepoint = 0;
        if (*ptr <= 0x7F) 
        {
            codepoint = *ptr;
            ptr++;
        } 
        else if ((*ptr & 0xE0) == 0xC0)
        {
            codepoint = *ptr & 0x1F;
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
        }
        else if ((*ptr & 0xF0) == 0xE0)
        {
            codepoint = *ptr & 0x0F;
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
        }
        else if ((*ptr & 0xF8) == 0xF0)
        {
            codepoint = *ptr & 0x07;
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
            codepoint = (codepoint << 6) | (*ptr & 0x3F);
            ptr++;
        }

        *out++ = codepoint;
    }

    *out = 0; // Null-terminate the output
}



/***************************************************************************************************
* Description                           :   功能函数-
* Author                                :   HH
* Creat Date                            :   2022-08-10
* notice                                :   将UTF-8 编码的字符串转换为 Unicode(即 UTF-32)中多余的
0000 字节删掉并且提前将高低位翻转后可以直接发给屏幕去显示
****************************************************************************************************/
void DeleteIvaildData(u16 *u16Data, u16 u16Len,u16 *u16OutData)
{
    while(u16Len--)
    {
        if(*u16Data != 0)
        {
            *u16OutData++ = htons(*u16Data);
        }
        u16Data++;
    }
}






