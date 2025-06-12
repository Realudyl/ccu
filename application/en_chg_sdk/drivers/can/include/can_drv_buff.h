/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv_buff.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-28
 * @Attention             :     
 * @Brief                 :     Can驱动实现 之 环形缓存 ring buffer 操作接口
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-28
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _can_drv_buff_H
#define _can_drv_buff_H
#include "can_drv.h"





//定义CAN消息缓存允许的最大环形缓存长度
#define cCanMsgBufSizeMax                 128






//接口操作类型定义
typedef enum
{
    eCanMsgOpTypeRx                   = 0,                                    //对接收缓存操作
    
    eCanMsgOpTypeMax,
}eCanMsgOpType_t;








//典型的环形缓存 ring buffer 结构定义
typedef struct
{
    i32                                 i32Size;                                //缓存区总大小
    i32                                 i32ReadIndex;                           //当前读位置
    i32                                 i32WriteIndex;                          //当前写位置
    stCanMsg_t                          *pBuf;                                  //缓存区
    
}stCanRingBuf_t;




//Can消息专用的环形缓存的结构定义---要配置接收和发送两组
typedef struct
{
    stCanRingBuf_t                      stRing[cCanMsgBufSizeMax];             //接收和发送用环形缓存 ring buffer
    
}stCanMsgBuf_t;










extern bool sCanMsgBufInit(eCanNum_t eCanNum, i32 i32RxSize, i32 i32TxSize);
extern bool sCanMsgBufDeinit(eCanNum_t eCanNum);
extern bool sCanMsgBufReset(eCanNum_t eCanNum);



extern bool sCanMsgBufRead(eCanNum_t eCanNum, eCanMsgOpType_t eType, stCanMsg_t* pMsg);



extern bool sCanMsgBufWrite(eCanNum_t eCanNum, eCanMsgOpType_t eType, const stCanMsg_t* pMsg);











#endif

