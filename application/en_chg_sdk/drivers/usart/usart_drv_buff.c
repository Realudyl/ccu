/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   usart_drv_buff.c
* Description                           :   串口驱动实现 之 环形缓存 ring buffer 操作接口
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-11-30
* notice                                :   根据EN+老代码的架构 重构，
*                                           重构思想:
*                                           1:ring buffer 操作接口属于串口专用，而非项目通用，因此与串口驱动聚合
*                                           2:由于属于串口专用，因此操作接口要基于串口号执行
****************************************************************************************************/
#include "usart_drv_buff.h"





//串口驱动使用的环形缓存
stUsartDrvBuf_t stUsartDrvBuf[eUsartNumMax];







bool sUsartDrvBufInit(eUsartNum_t eUsartNum, i32 i32RxSize, i32 i32TxSize);
bool sUsartDrvBufDeinit(eUsartNum_t eUsartNum);
bool sUsartDrvBufReset(eUsartNum_t eUsartNum);

i32  sUsartDrvBufReadByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 *pByte);
i32  sUsartDrvBufRead(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32MaxLen, u8 *pBuf);

bool sUsartDrvBufWriteByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 u8Byte);
bool sUsartDrvBufWrite(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32Len, const u8 *pBuf);










/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 初始化函数
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
***********************************************************************************************/
bool sUsartDrvBufInit(eUsartNum_t eUsartNum, i32 i32RxSize, i32 i32TxSize)
{
    
    //0:参数限幅
    if(eUsartNum >= eUsartNumMax)
    {
        return(false);
    }
    i32RxSize = ((i32RxSize < 0) || (i32RxSize > cUsartDrvBufSizeMax)) ? cUsartDrvBufSizeMax : i32RxSize;
    i32TxSize = ((i32TxSize < 0) || (i32TxSize > cUsartDrvBufSizeMax)) ? cUsartDrvBufSizeMax : i32TxSize;
    
    
    //1:接收缓存  初始化
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf = (u8 *)MALLOC(i32RxSize);
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf == NULL)
    {
        return(false);
    }
    memset(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf, 0, i32RxSize);
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32Size = i32RxSize;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32WriteIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf = xSemaphoreCreateBinary();
    xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf);
    
    
    //2:发送缓存  初始化
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf = (u8 *)MALLOC(i32TxSize);
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf == NULL)
    {
        FREE(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf);          //初始化失败 提前返回需要释放已分配的资源
        stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf = NULL;
        vSemaphoreDelete(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf);
        return(false);
    }
    memset(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf, 0, i32TxSize);
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32Size = i32TxSize;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32WriteIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].xSemBuf = xSemaphoreCreateBinary();
    xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].xSemBuf);
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 取消初始化函数
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
***********************************************************************************************/
bool sUsartDrvBufDeinit(eUsartNum_t eUsartNum)
{
    //0:参数限幅
    if(eUsartNum >= eUsartNumMax)
    {
        return(false);
    }
    
    
    //1:接收缓存  资源释放
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf != NULL)
    {
        FREE(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf);
        stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf = NULL;
        vSemaphoreDelete(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf);
    }
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32Size = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32WriteIndex = 0;
    
    
    //2:发送缓存  资源释放
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf != NULL)
    {
        FREE(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf);
        stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf = NULL;
        vSemaphoreDelete(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].xSemBuf);
    }
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32Size = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32WriteIndex = 0;
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 缓存区复位函数
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
***********************************************************************************************/
bool sUsartDrvBufReset(eUsartNum_t eUsartNum)
{
    //0:参数限幅
    if(eUsartNum >= eUsartNumMax)
    {
        return(false);
    }
    
    
    //1:接收缓存  复位
    xSemaphoreTake(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf, portMAX_DELAY);
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf != NULL)
    {
        memset(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].pBuf, 0, stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32Size);
    }
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].i32WriteIndex = 0;
    xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeRx].xSemBuf);
    
    
    //2:发送缓存  复位
    xSemaphoreTake(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].xSemBuf, portMAX_DELAY);
    if(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf != NULL)
    {
        memset(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].pBuf, 0, stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32Size);
    }
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32ReadIndex = 0;
    stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].i32WriteIndex = 0;
    xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eUsartDrvOpTypeTx].xSemBuf);
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 读取一个字节
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eUsartNum:串口通道
*                                           eType    :操作类型 接收缓存或发送缓存
*                                           pByte    :指向一个地址 用于保存读取的数据
***********************************************************************************************/
i32 sUsartDrvBufReadByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 *pByte)
{
    i32 i32ReadCnt = (-1);
    
    //0:输入参数限幅
    if((eUsartNum >= eUsartNumMax) || (eType >= eUsartDrvOpTypeMax) || (pByte == NULL))
    {
        return(i32ReadCnt);
    }
    
    
    //1:无可读数据
    i32ReadCnt = 0;
    if((stUsartDrvBuf[eUsartNum].stRing[eType].i32Size == 0)
    || (stUsartDrvBuf[eUsartNum].stRing[eType].pBuf == NULL)
    || (stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex == stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex))
    {
        return(i32ReadCnt);
    }
    
    
    //2:输出数据
    (*pByte) = stUsartDrvBuf[eUsartNum].stRing[eType].pBuf[stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex];
    i32ReadCnt++;
    
    stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex++;
    if(stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex >= stUsartDrvBuf[eUsartNum].stRing[eType].i32Size)
    {
        stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex = 0;
    }
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 读取序列
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eUsartNum:串口通道
*                                           eType    :操作类型 接收缓存或发送缓存
*                                           i32MaxLen:pBuf指向的区域最大可用长度
*                                           pBuf     :指向一个地址 用于保存读取的数据
***********************************************************************************************/
i32 sUsartDrvBufRead(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32MaxLen, u8 *pBuf)
{
    i32 i32ReadCnt = (-1);
    
    //0:输入参数限幅
    if((eUsartNum >= eUsartNumMax) || (eType >= eUsartDrvOpTypeMax) || (i32MaxLen < 0) || (pBuf == NULL))
    {
        return(i32ReadCnt);
    }
    
    
    //1:无可读数据
    i32ReadCnt = 0;
    if((stUsartDrvBuf[eUsartNum].stRing[eType].i32Size == 0)
    || (stUsartDrvBuf[eUsartNum].stRing[eType].pBuf == NULL)
    || (stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex == stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex))
    {
        return(i32ReadCnt);
    }
    
    
    //2:输出数据
    while(1)
    {
        if((i32ReadCnt >= i32MaxLen)
        || (stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex == stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex))
        {
            break;
        }
        
        pBuf[i32ReadCnt] = stUsartDrvBuf[eUsartNum].stRing[eType].pBuf[stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex];
        i32ReadCnt++;
        
        stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex++;
        if(stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex >= stUsartDrvBuf[eUsartNum].stRing[eType].i32Size)
        {
            stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex = 0;
        }
    }
    
    return(i32ReadCnt);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 写入一个字节
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eUsartNum:串口通道
*                                           eType    :操作类型 接收缓存或发送缓存
*                                           u8Byte   :要写入的字节
***********************************************************************************************/
bool sUsartDrvBufWriteByte(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, u8 u8Byte)
{
    i32 i32LeftSize;                                                            //剩余的可写空间
    
    
    //0:输入参数限幅
    if((eUsartNum >= eUsartNumMax) || (eType >= eUsartDrvOpTypeMax))
    {
        return(false);
    }
    
    
    //1:存储空间不够
    i32LeftSize  = stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex;
    i32LeftSize += stUsartDrvBuf[eUsartNum].stRing[eType].i32Size;
    i32LeftSize -= stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex;
    if((i32LeftSize < sizeof(u8))
    || (stUsartDrvBuf[eUsartNum].stRing[eType].pBuf == NULL))
    {
        return(false);
    }
    
    
    //2:上锁
    if(eType == eUsartDrvOpTypeTx)
    {
        xSemaphoreTake(stUsartDrvBuf[eUsartNum].stRing[eType].xSemBuf, portMAX_DELAY);
    }
    
    
    //3:写入数据
    stUsartDrvBuf[eUsartNum].stRing[eType].pBuf[stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex] = u8Byte;
    stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex++;
    if(stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex >= stUsartDrvBuf[eUsartNum].stRing[eType].i32Size)
    {
        stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex = 0;
    }
    
    
    //4:解锁
    if(eType == eUsartDrvOpTypeTx)
    {
        xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eType].xSemBuf);
    }
    
    
    return(true);
}








/**********************************************************************************************
* Description                           :   串口驱动 环形缓存 ring buffer 操作接口 之 写入序列
* Author                                :   Hall
* modified Date                         :   2023-11-30
* notice                                :   
*                                           eUsartNum:串口通道
*                                           eType    :操作类型 接收缓存或发送缓存
*                                           i32Len   :写入的数据长度
*                                           pBuf     :指向一个地址 用于保存读取的数据
***********************************************************************************************/
bool sUsartDrvBufWrite(eUsartNum_t eUsartNum, eUsartDrvOpType_t eType, i32 i32Len, const u8 *pBuf)
{
    i32 i32LeftSize;                                                            //剩余的可写空间
    i32 i32WriteCnt;
    
    //0:输入参数限幅
    if((eUsartNum >= eUsartNumMax) || (eType >= eUsartDrvOpTypeMax) || (i32Len < 0) || (pBuf == NULL))
    {
        return(false);
    }
    
    
    //1:存储空间不够
    i32LeftSize  = stUsartDrvBuf[eUsartNum].stRing[eType].i32ReadIndex;
    i32LeftSize += stUsartDrvBuf[eUsartNum].stRing[eType].i32Size;
    i32LeftSize -= stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex;
    i32LeftSize -= 1;                                                           //防止读写index相等导致异常
    if((i32LeftSize < i32Len)
    || (stUsartDrvBuf[eUsartNum].stRing[eType].pBuf == NULL))
    {
        return(false);
    }
    
    
    //2:上锁
    if(eType == eUsartDrvOpTypeTx)
    {
        xSemaphoreTake(stUsartDrvBuf[eUsartNum].stRing[eType].xSemBuf, portMAX_DELAY);
    }
    
    
    //3:写入数据
    i32WriteCnt = 0;
    while(i32WriteCnt < i32Len)
    {
        stUsartDrvBuf[eUsartNum].stRing[eType].pBuf[stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex] = pBuf[i32WriteCnt];
        i32WriteCnt++;
        
        stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex++;
        if(stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex >= stUsartDrvBuf[eUsartNum].stRing[eType].i32Size)
        {
            stUsartDrvBuf[eUsartNum].stRing[eType].i32WriteIndex = 0;
        }
    }
    
    //4:解锁
    if(eType == eUsartDrvOpTypeTx)
    {
        xSemaphoreGive(stUsartDrvBuf[eUsartNum].stRing[eType].xSemBuf);
    }
    
    
    return(true);
}






























