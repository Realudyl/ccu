#ifndef _EN_QUEUE_H_
#define _EN_QUEUE_H_

#include <stdbool.h>
#include "en_common.h"









/////////////////////////////queue
typedef struct
{
    u16 head;
    u16 len ;
}sQueueHdr_t;






#define QUEUE_TYPE                      u8
#define QUEUE_SIZE                      20



typedef struct
{
    //队列结构
    u8                                  *qValues;
    u8                                  qNodeSize;                              //队列可以放多少个
    u8                                  qNodeSpace;                             //一个队列头所包含的空间
    u8                                  qSize;                                  //当前队列个数
    u8                                  qHead;                                  //当前队列头
    u8                                  qTail;                                  //当前队列尾 
    
    //内容存放点
    u8                                  *buffer;                                //存放真实数据的地方
    u32                                 bufMaxSize;                             //上面buffer的大小
    u32                                 bufHead ;                               //有内容的缓存的起址
    u32                                 bufTail ;                               //有内容的缓存的终址
    u32                                 bufTotallen;                            //所有缓存内容
    
    bool                                initFin;
}stQueue_t;









bool queueInit(stQueue_t *queue, u32 bufSize, u8 qNodeSize, u8 qNodeSpace);
u8   queueSize(const stQueue_t *queue);
bool queuePop(stQueue_t *queue);
bool queuePush(stQueue_t *queue, const QUEUE_TYPE *nodeHdr , const u8 *data, u32 datalen, bool force);
u16  queueHead(const stQueue_t *queue, QUEUE_TYPE *nodeHdr , u8 *data, u32 maxLen);








extern bool en_queueInit(stQueue_t *queue, u32 bufSize, u8 qNodeSize, u8 qNodeSpace);
extern u8   en_queueSize(const stQueue_t *queue);
extern bool en_queuePush(stQueue_t *queue, u8 u8Chan, const u8* data, u16 len);
extern u16  en_queuePop(stQueue_t *queue, u8 *pChan, u8* data, u16 maxlen);




#endif



