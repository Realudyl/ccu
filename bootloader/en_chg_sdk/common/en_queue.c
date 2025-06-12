#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"
#include "en_queue.h"







#define GET_QUEUE_HDR_ADDR(queue, idx) (sQueueHdr_t *)(queue->qValues + ((u16)(queue->qNodeSpace+sizeof(sQueueHdr_t))*(idx)))






/*
queue:操作队列
bufSize: 存储数据的缓存总大小
queueSize: 队列存入最大各数
qNodeSpace: 队列信息包含的字节大小
*/
bool queueInit(stQueue_t *queue, u32 bufSize, u8 qNodeSize, u8 qNodeSpace)
{
    u32 totalSize = 0;
    
    if(!queue)
    {
        return false;
    }
    
    if(queue->initFin)
    {
        return true;
    }
    
    memset(queue, 0, sizeof(stQueue_t));
    
    if(!queue->buffer)
    {
        queue->buffer =  (u8*)MALLOC(bufSize);
        if(queue->buffer == false)
        {
            return(false);
        }
        
        memset(queue->buffer, 0, bufSize);
        queue->bufMaxSize = bufSize;
    }
    
    if(!queue->qValues)
    {
        totalSize = (u16)(qNodeSpace+sizeof(sQueueHdr_t))*qNodeSize;
        queue->qValues =   (u8*)MALLOC(totalSize);
        if(queue->qValues == false)
        {
            return(false);
        }
        
        memset(queue->qValues, 0, totalSize);
        queue->qNodeSize = qNodeSize;
        queue->qNodeSpace = qNodeSpace;
    }
    
    queue->initFin = true;
    {
        return true;
    }
}






u8 queueSize(const stQueue_t *queue)
{
    if(!queue || !queue->initFin)
    {
        return 0;
    }
    
    return queue->qSize;
}






bool queuePop(stQueue_t *queue)
{
    sQueueHdr_t *hdr;
    u32 buflen, bufhead, len;
    
    if(!queue || !queue->initFin)
    {
        return false;
    }
    
    if(queue->qSize == 0)
    {
        return true;
    }
    
    hdr = GET_QUEUE_HDR_ADDR(queue, queue->qHead);
    buflen = hdr->len;
    bufhead = hdr->head;
    if(bufhead+buflen < queue->bufMaxSize)
    {
        memset(queue->buffer + bufhead, 0, buflen);
    }
    else
    {
        len = queue->bufMaxSize - bufhead;
        memset(queue->buffer + bufhead, 0, len);
        memset(queue->buffer, 0, buflen-len);
    }
    queue->bufTotallen -= buflen;
    
    memset((u8*)hdr, 0, (queue->qNodeSpace + sizeof(sQueueHdr_t)));
    queue->qHead = (queue->qHead+1) % queue->qNodeSize;
    queue->qSize --;
    
    return true;
}







u16 queueHead(const stQueue_t *queue, QUEUE_TYPE *nodeHdr , u8 *data, u32 maxLen)
{
    sQueueHdr_t *hdr;
    u32 buflen, bufhead, len;
    
    if(!queue || !queue->initFin || !data)
    {
        return 0;
    }
    
    if(queue->qSize == 0)
    {
        return 0;
    }
    
    
    memset(data, 0, maxLen);
    
    //读取内容缓存
    hdr = GET_QUEUE_HDR_ADDR(queue, queue->qHead);
    buflen = hdr->len;
    bufhead = hdr->head;
    
    if(buflen > maxLen)
    {
        return 0;
    }
    
    if(bufhead+buflen < queue->bufMaxSize)
    {
        memcpy(data, queue->buffer + bufhead, buflen);
    }
    else 
    {
        len = queue->bufMaxSize - bufhead;
        memcpy(data, queue->buffer + bufhead, len);
        memcpy(data+len, queue->buffer, buflen-len);
    }
    
    //读取队列缓存
    if(queue->qNodeSpace && nodeHdr)
    {
        memcpy(nodeHdr, (u8*)hdr + sizeof(sQueueHdr_t), queue->qNodeSpace);
    }
    
    return buflen;
}







bool  queuePush(stQueue_t *queue, const QUEUE_TYPE *nodeHdr , const u8 *data, u32 datalen, bool force)
{
    sQueueHdr_t *hdr;
    u32  spanlen, tail;
    
    if(!queue || !queue->initFin || !data)
    {
        return false;
    }
    
    if(queue->qSize >= queue->qNodeSize) //full
    {
        if(!force)
        {
            return false;
        }
        queuePop(queue);
    }
    
    while(queue->bufTotallen + datalen > queue->bufMaxSize)
    {
        if(!force)
        {
            return false;
        }       
        queuePop(queue);
    }
    
    tail = (queue->bufTail + datalen) % queue->bufMaxSize;
    queue->bufHead = queue->bufTail;
    queue->bufTail = tail;
    queue->bufTotallen += datalen;
    
    if(queue->bufTail < queue->bufHead)
    {
        spanlen = queue->bufMaxSize - queue->bufHead ;
        memcpy(queue->buffer + queue->bufHead, data, spanlen);
        memcpy(queue->buffer , data+spanlen, datalen-spanlen);
    }
    else
    {
        memcpy(queue->buffer + queue->bufHead, data, datalen);
    } 
    
    hdr = GET_QUEUE_HDR_ADDR(queue, queue->qTail);
    hdr->head = queue->bufHead;
    hdr->len  = datalen;
    
    if(queue->qNodeSpace && nodeHdr)
    {
        memcpy((u8*)hdr + sizeof(sQueueHdr_t), nodeHdr, queue->qNodeSpace);
    }
    
    queue->qTail = (queue->qTail+1) % queue->qNodeSize; 
    queue->qSize ++;
    
    
    return true;
}












//以下几个函数 为整个工程提供队列操作的接口函数
bool en_queueInit(stQueue_t *queue, u32 bufSize, u8 qNodeSize, u8 qNodeSpace)
{
    if(queue)
    {
        return queueInit(queue, bufSize, qNodeSize, qNodeSpace);
    }
    return false;
}





u8  en_queueSize(const stQueue_t *queue)
{   
    return queueSize(queue);
}





bool en_queuePush(stQueue_t *queue, u8 u8Chan, const u8* data, u16 len)
{
    return queuePush(queue, (const QUEUE_TYPE*)&u8Chan, data, len, true);
}





u16  en_queuePop(stQueue_t *queue, u8 *pChan, u8* data, u16 maxlen)
{
    u16 len = 0;
    
    if (queueSize(queue) == 0)
    {
        return 0;
    }
    
    if ((len = queueHead(queue, (QUEUE_TYPE*)pChan, data, maxlen)) > 0)
    {
        queuePop(queue);
        return len;
    }
    
    return 0;
}






