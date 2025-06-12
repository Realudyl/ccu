/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_hand.h
* Description                           :   V2G 应用层消息 handshake报文
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-11
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_hand_h_
#define _bms_ccs_v2g_hand_h_









//此处内容根据《DIN70121》——8.8.1.3——[V2G-DC-175]定义
#define APP_PROTOCOL_MAJOR_VERSION      2
#define APP_PROTOCOL_MINOR_VERSION      0











extern i32  sV2gHandshakeProc(i32 i32DevIndex, const u8 *pBuf, u32 u32Len);
extern bool sV2gHandshakeCheck(i32 i32DevIndex, bool bClearFlag);

extern bool sV2gHandshakeGetNegotiation(i32 i32DevIndex);





#endif




















