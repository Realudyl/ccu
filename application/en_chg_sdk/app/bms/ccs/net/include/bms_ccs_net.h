/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_net.h
* Description                           :   SECC 网络层实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-21
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_net_h_
#define _bms_ccs_net_h_
#include "en_common.h"
#include "en_common_eth.h"

#include "bms_ccs_mme.h"
#include "mse102x_drv.h"








//version field of IPv6 header
#define cIpv6Version                    6


//V2GTP 报文缓存区长度
#define cV2gtpBufSize                   2048













//UDP参数
typedef struct
{
    u16                                 u16Port;
    ip_addr_t                           *pAddr;
    struct udp_pcb                      *pPcb;
    struct pev_item                     *pPev;
}stUdpParm_t;
























typedef struct
{
    //v2gtp 协议参数
    u16                                 u16Port;                                //监听端口
    i32                                 i32Socket;                              //收发数据套接字
    i32                                 i32SocketListen;                        //listen  套接字
    
    bool                                bTcpConnStatus;                         //TCP连接状态
    SemaphoreHandle_t                   xSemTcpClose;                           //TCP连接关闭信号
    void                                (*pCbSwitchMmeToV2g)(i32 i32DevIndex);  //TCP连接建立后需要执行的状态切换回调函数
    
    //v2gtp   协议收发缓存区
    //u8TxBuf 不用锁保护, 因为同一个i32DevIndex下的发送函数不会被多线程并行调用
    u8                                  u8RxBuf[cV2gtpBufSize];                 //
    u8                                  u8TxBuf[cV2gtpBufSize];                 //
    
}stBmsCcsNetCache_t;









extern bool sBmsCcsNetInit(i32 i32DevIndex, void (*pCbSwitchMmeToV2g)(i32 i32DevIndex));

extern bool sBmsCcsNetSetTcpConnClose(i32 i32DevIndex);
extern bool sBmsCcsNetGetTcpConnStatus(i32 i32DevIndex);
























#endif




