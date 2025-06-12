/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv_buff.h
* Description                           :   串口驱动实现 之 环形缓存 ring buffer 操作接口
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-30
* notice                                :   
****************************************************************************************************/
#ifndef _usart_drv_buff_H
#define _usart_drv_buff_H
#include "usart_drv.h"











//定义串口驱动允许的最大环形缓存长度
#define cUsartDrvBufSizeMax             2048






//接口操作类型定义
typedef enum
{
    eUsartDrvOpTypeRx                   = 0,                                    //对接收缓存操作
    eUsartDrvOpTypeTx                   = 1,                                    //对发送缓存操作
    
    eUsartDrvOpTypeMax,
}eUsartDrvOpType_t;








//典型的环形缓存 ring buffer 结构定义
typedef struct
{
    i32                                 i32Size;                                //缓存区总大小
    i32                                 i32ReadIndex;                           //当前读位置
    i32                                 i32WriteIndex;                          //当前写位置
    u8                                  *pBuf;                                  //缓存区
    SemaphoreHandle_t                   xSemBuf;                                //缓存区保护信号量
}stRingBuf_t;



//串口驱动专用的环形缓存的结构定义---要配置接收和发送两组
typedef struct
{
    stRingBuf_t                         stRing[eUsartDrvOpTypeMax];             //接收和发送用环形缓存 ring buffer
    
}stUsartDrvBuf_t;

































extern bool sUsartDrvBufInit(eUsartNum_t eUsartNum, i32 i32RxSize, i32 i32TxSize);
extern bool sUsartDrvBufDeinit(eUsartNum_t eUsartNum);
extern bool sUsartDrvBufReset(eUsartNum_t eUsartNum);
extern i32  sUsartDrvBufReadByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 *pByte);
extern i32  sUsartDrvBufRead(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32MaxLen, u8 *pBuf);
extern bool sUsartDrvBufWriteByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 u8Byte);
extern bool sUsartDrvBufWrite(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32Len, const u8 *pBuf);
























#endif
























