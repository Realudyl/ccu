/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net_ndp.h
* Description                           :   SECC 网络层实现之 邻居发现协议(ndp)部分
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#ifndef __bms_ccs_net_ndp_h_
#define __bms_ccs_net_ndp_h_

#include "en_common.h"

#include "bms_ccs_net_tsf.h"






//ICMP codes for neighbour discovery messages
//本来属于 net/ndisc.h 内容 但包含该文件无法编译 拷贝过来
#define NDISC_ROUTER_SOLICITATION       133
#define NDISC_ROUTER_ADVERTISEMENT      134
#define NDISC_NEIGHBOUR_SOLICITATION    135
#define NDISC_NEIGHBOUR_ADVERTISEMENT   136
#define NDISC_REDIRECT                  137




//ndisc options
//本来属于 net/ndisc.h 内容 但包含该文件无法编译 拷贝过来
enum 
{
    __ND_OPT_PREFIX_INFO_END            = 0,
    ND_OPT_SOURCE_LL_ADDR               = 1,                                    // RFC2461
    ND_OPT_TARGET_LL_ADDR               = 2,                                    // RFC2461
    ND_OPT_PREFIX_INFO                  = 3,                                    // RFC2461
    ND_OPT_REDIRECT_HDR                 = 4,                                    // RFC2461
    ND_OPT_MTU                          = 5,                                    // RFC2461
    __ND_OPT_ARRAY_MAX,
    
    ND_OPT_ROUTE_INFO                   = 24,                                   // RFC4191
    ND_OPT_RDNSS                        = 25,                                   // RFC5006
    __ND_OPT_MAX
};






//NDP 协议下 RS、NS、NA等报文附带的 option 参数格式定义
typedef struct
{
    u8                                  u8Type;
    u8                                  u8len;                                  //单位为8字节 即1表示8字节，2表示16字节
    u8                                  u8Data[0];                              //用于指向 option 附加数据首地址
}__attribute__((__packed__)) stNdpOpt_t;

//定义 u8len 字段的单位
#define cOptLenUnit                     8                                       //字节数


//NDP 协议下 NS、NA消息数据部分格式定义
typedef struct
{
    struct in6_addr                     stTargetAddr;                           //目标的 IPv6地址
    u8                                  u8Opt[0];                               //用于指向 option 参数首地址
}__attribute__((__packed__)) stNdMsg_t;














extern bool sBmsCcsNetNdpNsSend(i32 i32DevIndex, const u8 *pBuf, const u8 *pMacSrc, const struct in6_addr *pAddrSrc);















#endif 







