/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_tsf.h
* Description                           :   SECC 网络层实现 之报文传输部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_net_tsf_h_
#define _bms_ccs_net_tsf_h_
#include "en_common.h"








//version field of IPv6 header
#define cIpv6Version                    6




















//网络报文发送时 各个层次涉及到 IP地址 端口 Mac地址 
//定义本结构体 用于集中传递以上参数
typedef struct
{
    u8                                  u8Smac[ETH_ALEN];                       //  源Mac地址
    u8                                  u8Dmac[ETH_ALEN];                       //目的Mac地址
    u16                                 u16Sport;                               //  源   端口
    u16                                 u16Dport;                               //目的   端口
    struct in6_addr                     stSaddr;                                //  源 IP地址
    struct in6_addr                     stDaddr;                                //目的 IP地址
    
    i32                                 i32Index;                               //设备序号
}stBmsCcsNetTsfParam_t;
























extern bool sBmsCcsNetTsfSendIcmpv6(stBmsCcsNetTsfParam_t stParam, const u8 *pBuf, u16 u16Len, u8 u8Icmpv6Type, u8 u8Icmpv6Code);






















#endif




