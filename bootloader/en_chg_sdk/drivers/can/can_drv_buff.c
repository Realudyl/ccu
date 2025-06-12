/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     can_drv_buff.c
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
#include "can_drv_buff.h"







//Can消息缓存使用的环形缓存
stCanMsgBuf_t stCanMsgBuf[eCanNumMax];







bool sCanMsgBufInit(eCanNum_t eCanNum, i32 i32RxSize, i32 i32TxSize);
bool sCanMsgBufDeinit(eCanNum_t eCanNum);
bool sCanMsgBufReset(eCanNum_t eCanNum);


bool sCanMsgBufRead(eCanNum_t eCanNum, eCanMsgOpType_t eType, stCanMsg_t* pMsg);


bool sCanMsgBufWrite(eCanNum_t eCanNum, eCanMsgOpType_t eType, const stCanMsg_t* pMsg);
















/*******************************************************************************
 * @FunctionName   :      sCanMsgBufInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:42:49
 * @Description    :      Can驱动 环形缓存 ring buffer 操作接口 之 初始化函数
 * @Input          :      eCanNum     Can外设号
 * @Input          :      i32RxSize   接收环形缓存大小
 * @Input          :      i32TxSize   发送环形缓存大小
 * @Return         :      
*******************************************************************************/
bool sCanMsgBufInit(eCanNum_t eCanNum, i32 i32RxSize, i32 i32TxSize)
{
    //0:参数限幅
    if(eCanNum >= eCanNumMax)
    {
        return(false);
    }
    i32RxSize = ((i32RxSize < 0) || (i32RxSize > cCanMsgBufSizeMax)) ? cCanMsgBufSizeMax : i32RxSize;
    i32TxSize = ((i32TxSize < 0) || (i32TxSize > cCanMsgBufSizeMax)) ? cCanMsgBufSizeMax : i32TxSize;
    
    
    //1:接收缓存  初始化
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf = (stCanMsg_t *)MALLOC(sizeof(stCanMsg_t) * i32RxSize);
    if(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf == NULL)
    {
        return(false);
    }
    memset(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf, 0, i32RxSize);
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32Size = i32RxSize;
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32ReadIndex = 0;
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32WriteIndex = 0;
    
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sCanMsgBufDeinit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:42:51
 * @Description    :      Can驱动 环形缓存 ring buffer 操作接口 之 取消初始化函数
 * @Input          :      eCanNum     Can外设号
 * @Return         :      
*******************************************************************************/
bool sCanMsgBufDeinit(eCanNum_t eCanNum)
{
    //0:参数限幅
    if(eCanNum >= eCanNumMax)
    {
        return(false);
    }
    
    
    //1:接收缓存  资源释放
    if(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf != NULL)
    {
        FREE(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf);
        stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf = NULL;
    }
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32Size = 0;
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32ReadIndex = 0;
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32WriteIndex = 0;
    
    
    return(true);
}










/*******************************************************************************
 * @FunctionName   :      sCanMsgBufReset
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:42:53
 * @Description    :      Can驱动 环形缓存 ring buffer 操作接口 之 缓存区复位函数
 * @Input          :      eCanNum     Can外设号
 * @Return         :      
*******************************************************************************/
bool sCanMsgBufReset(eCanNum_t eCanNum)
{
    //0:参数限幅
    if(eCanNum >= eCanNumMax)
    {
        return(false);
    }
    
    
    //1:接收缓存  复位
    if(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf != NULL)
    {
        memset(stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].pBuf, 0, stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32Size);
    }
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32ReadIndex = 0;
    stCanMsgBuf[eCanNum].stRing[eCanMsgOpTypeRx].i32WriteIndex = 0;
    
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sCanMsgBufRead
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:42:56
 * @Description    :      Can驱动 环形缓存 ring buffer 操作接口 之 从环形缓存中读取Can结构体数据
 * @Input          :      eCanNum       Can外设号
 * @Input          :      eType         环形缓存操作类型
 * @Input          :      pMsg          指向储存Can结构体数据的指针
 * @Return         :      
*******************************************************************************/
bool sCanMsgBufRead(eCanNum_t eCanNum, eCanMsgOpType_t eType, stCanMsg_t* pMsg)
{
    //0:输入参数限幅
    if((eCanNum >= eCanNumMax) || (eType >= eCanMsgOpTypeMax) || (pMsg == NULL))
    {
        return(false);
    }
    
    
    //1:无可读数据
    if((stCanMsgBuf[eCanNum].stRing[eType].i32Size == 0)
    || (stCanMsgBuf[eCanNum].stRing[eType].pBuf == NULL)
    || (stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex == stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex))
    {
        return(false);
    }
    
    
    //2:输出数据
    (*pMsg) = stCanMsgBuf[eCanNum].stRing[eType].pBuf[stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex];
    
    stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex++;
    if(stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex >= stCanMsgBuf[eCanNum].stRing[eType].i32Size)
    {
        stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex = 0;
    }
    
    return(true);
}








/*******************************************************************************
 * @FunctionName   :      sCanMsgBufWrite
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月28日  21:42:58
 * @Description    :      Can驱动 环形缓存 ring buffer 操作接口 之 向环形缓存中写入Can结构体数据
 * @Input          :      eCanNum       Can外设号
 * @Input          :      eType         环形缓存操作类型
 * @Input          :      pMsg          指向要写入环形缓存的Can结构体数据的指针
 * @Return         :      
*******************************************************************************/
bool sCanMsgBufWrite(eCanNum_t eCanNum, eCanMsgOpType_t eType, const stCanMsg_t* pMsg)
{
    i32 i32LeftSize;                                                            //剩余的可写空间
    
    //0:输入参数限幅
    if((eCanNum >= eCanNumMax) || (eType >= eCanMsgOpTypeMax))
    {
        return(false);
    }
    
    
    //1:存储空间不够
    i32LeftSize  = stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex + 1;
    i32LeftSize %= stCanMsgBuf[eCanNum].stRing[eType].i32Size;
    if((i32LeftSize == stCanMsgBuf[eCanNum].stRing[eType].i32ReadIndex) || (stCanMsgBuf[eCanNum].stRing[eType].pBuf == NULL))
    {
        return(false);
    }
    
    
    //2:写入数据
    stCanMsgBuf[eCanNum].stRing[eType].pBuf[stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex] = (*pMsg);
    
    stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex++;
    if(stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex >= stCanMsgBuf[eCanNum].stRing[eType].i32Size)
    {
        stCanMsgBuf[eCanNum].stRing[eType].i32WriteIndex = 0;
    }
    
    
    return(true);
}


















