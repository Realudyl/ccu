/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_mme_op.c
* Description                           :   MME协议实现 之 MME底层操作接口实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-07-03
* notice                                :   
****************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "lwip/def.h"
#include "en_common.h"
#include "en_log.h"

#include "mse102x_drv.h"

#include "bms_ccs_mme.h"
#include "bms_ccs_mme_op.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme_op";





extern stMmeCache_t *pMmeCache[];






/**
 * Construct the FMI field of the MME header.
 * \param  nf_mi  number of fragments
 * \param  fn_mi  fragment number
 * \param  fmsn  fragmentation message sequence number
 * \return  the FMI field.
 */

static inline unsigned short mme_fmi_set(u16 nf_mi, u16 fn_mi, u16 fmsn)
{
    return (fmsn << 8) | (fn_mi << 4) | nf_mi;
}

/**
 * Get the FMI field uncompressed from the MME header.
 * \param  fmi  the compressed FMI
 * \param  nf_mi  number of fragments
 * \param  fn_mi  fragment number
 * \param  fmsn  fragmentation message sequence number
*/

static inline void mme_fmi_get(const unsigned short fmi, u16 *nf_mi, u16 *fn_mi, u16 *fmsn)
{
    *nf_mi = fmi & 0xF;
    *fn_mi = (fmi >> 4) & 0xF;
    *fmsn = (fmi >> 8) & 0xFF;
}


mme_error_t mme_init(mme_ctx_t *ctx, u16 u16MmType , unsigned char *buffer, const unsigned int length)
{
    /* protect from null pointers */
    if(ctx == NULL || buffer == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    ctx->buffer     =   buffer;
    ctx->mmtype     =   u16MmType;
    ctx->length     =   length;
    ctx->head       =   0;
    ctx->tail       =   0;
    
    /* By default the MME is not VS */
    ctx->oui        =   MME_OUI_NOT_PRESENT;
    ctx->status     =   MME_STATUS_OK;
    
    return MME_SUCCESS;
}


mme_error_t mme_get_length(mme_ctx_t *ctx, unsigned int *length)
{
    /* protect from null pointers */
    if(ctx == NULL || length == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(ctx->status == MME_STATUS_INIT)
    {
        return MME_ERROR_NOT_INIT;
    }
    
    *length = ctx->tail - ctx->head;
    
    return MME_SUCCESS;
}


mme_error_t mme_get_free_space(mme_ctx_t *ctx, unsigned int *length)
{
    /* protect from null pointers */
    if(ctx == NULL || length == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(ctx->status == MME_STATUS_INIT)
    {
        return MME_ERROR_NOT_INIT;
    }
    
    *length = ctx->length - (ctx->tail - ctx->head);
    
    return MME_SUCCESS;
}


mme_error_t mme_push(mme_ctx_t *ctx, const void *data, unsigned int length, unsigned int *result_length)
{
    unsigned int free = 0;
    int delta = 0;
    
    /* protect from null pointers */
    if(ctx == NULL || data == NULL || result_length == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(ctx->status == MME_STATUS_INIT)
    {
        return MME_ERROR_NOT_INIT;
    }
    
    free = ctx->length - (ctx->tail - ctx->head);
    if(length > free)
    {
        *result_length = free;
        return MME_ERROR_SPACE;
    }
    
    *result_length = length;
    
    /* make place in front, if needed */
    if(length > ctx->head)
    {
        /*
         *             *length
         *  .-----------^-----------.
         *  |---------|xxxxxxxxxxxxx|--------------------|---------------------|
         * buff      head                              tail                 ctx->length
         *             \_______________  _______________/
         *                             \/
         *                           pyload
         *
         * we have to shift right our payload for this difference delta marked with 'x'
         * in order for *length bytes to fit in from beginning of the buffer
         */
        delta = length - ctx->head;
        memmove(ctx->buffer + ctx->head + delta, ctx->buffer + ctx->head, ctx->tail - ctx->head);

        /* update head and tail pointers (offsets) */
        ctx->head += delta;
        ctx->tail += delta;

/*当 head 前面部分不足 length 时 payload后移 delta 部分 腾出足够 length 的空间供后面拷贝数据进来
    之前
         *             *length
         *  .-----------^-----------.
         *  |---------|xxxxxxxxxxxxx|--------------------|---------------------|
         * buff      head                              tail                 ctx->length
         *             \_______________  _______________/
         *                             \/
         *                           pyload
    
    
    
    之后 
    
         *             *length
         *  .-----------^-----------.
         *  |-----------------------|xxxxxxxxxxxxx-------|---------------------|
         * buff                     head                              tail  ctx->length
         *                          \_______________  _______________/
         *                                          \/
         *                                        pyload
*/
    }
    
    /* place head pointer to where copy chould start from */
    ctx->head -= length;
    memcpy(ctx->buffer + ctx->head, data, length);
    
    return MME_SUCCESS;
}


mme_error_t mme_put(mme_ctx_t *ctx, const void *data, unsigned int length, unsigned int *result_length)
{
    unsigned int free = 0;
    int delta = 0;
    
    /* protect from null pointers */
    if(ctx == NULL || data == NULL || result_length == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(ctx->status == MME_STATUS_INIT)
    {
        return MME_ERROR_NOT_INIT;
    }
    
    free = ctx->length - (ctx->tail - ctx->head);
    if(length > free)
    {
        *result_length = free;
        return MME_ERROR_SPACE;
    }
    
    *result_length = length;
    
    /* make place after payload, if needed */
    if(length > ctx->length - ctx->tail)
    {
        /*
         *                                       *length
         *                           .---------------^-----------------.
         *  |-----------|------------|xxxxxxx|-------------------------|
         * buff        head                 tail                    ctx->length
         *               \________  ________/
         *                        \/
         *                      payload
         *
         * we have to shift left our payload for this difference delta marked with 'x'
         * in order for *length bytes to fit in from beginning of the buffer
         */
        delta = length - (ctx->length - ctx->tail);
        memmove((unsigned char *) (ctx->buffer + ctx->head - delta), (unsigned char *) (ctx->buffer + ctx->head), ctx->tail - ctx->head);

        /* update head and tail pointers (offsets) */
        ctx->head -= delta;
        ctx->tail -= delta;
        
/*当 tail 后面部分不足 length 时，payload前移 delta 部分 腾出足够 length 的空间 供后面拷贝数据进来
    之前 
    *                                       *length
    *                           .---------------^-----------------.
    *  |-----------|------------|xxxxxxx|-------------------------|
    * buff        head                 tail                    ctx->length
    *              |\________  ________/
    *              |         \/
    *              |       payload
                   |
                   |
                   |
    之后           |
                   |
    *              |                        *length
    *      | delta |            .---------------^-----------------.
    *  |---|------------|xxxxxxx|---------------------------------|
    * buff head                tail                           ctx->length
    *       \________  ________/
    *                  \/
    *                payload
*/
    }
    
    memcpy((unsigned char *) (ctx->buffer + ctx->tail), (unsigned char *) data, length);
    ctx->tail += length;
    
    return MME_SUCCESS;
}


mme_error_t mme_pull(mme_ctx_t *ctx, void *data, unsigned int length, unsigned int *result_length)
{
    //参数空指针保护
    if(ctx == NULL || data == NULL || result_length == NULL)
    {
        EN_SLOGE(TAG, "有空指针");
        return MME_ERROR_GEN;
    }
    
    //检查 ctx 有没有初始化
    if(ctx->status == MME_STATUS_INIT)
    {
        EN_SLOGE(TAG, "ctx 未初始化");
        return MME_ERROR_NOT_INIT;
    }

    /* check if it is demanded more data than we have in payload */
    if(length > ctx->tail - ctx->head)
    {
        *result_length = ctx->tail - ctx->head;
        
        EN_SLOGE(TAG, "MME_ERROR_ENOUGH!!length= %d, ctx->tail= %d, ctx->head= %d", length, ctx->tail, ctx->head);
        return MME_ERROR_ENOUGH;
    }

    *result_length = length;

    memcpy(data, (unsigned char *) (ctx->buffer + ctx->head), length);
    ctx->head += length;

    return MME_SUCCESS;
}


mme_error_t mme_pop(mme_ctx_t *ctx, void *data, unsigned int length, unsigned int *result_length)
{
    /* protect from null pointers */
    if(ctx == NULL || data == NULL || result_length == NULL)
    {
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(ctx->status == MME_STATUS_INIT)
    {
        return MME_ERROR_NOT_INIT;
    }
    
    /* check if it is demanded more data than we have in payload */
    if(length > ctx->tail - ctx->head)
    {
        *result_length = ctx->tail - ctx->head;
        return MME_ERROR_ENOUGH;
    }
    
    *result_length = length;
    
    memcpy(data, (unsigned char *)(ctx->buffer + ctx->tail - length), length);
    ctx->tail -= length;
    
    return MME_SUCCESS;
}





int mme_header_prepare(MME_t *mh, mme_ctx_t *ctx, unsigned char *src_mac, const unsigned char *dest)
{
    int   nbpkt = 0;
    u16  fmi_nf_mi = 0; /* Number of fragments */
    u16  fmi_fmsn = 0; /* SSN of the fragmented mme */
    u16  payload_size;
    /*
     * --- Create MME header ---
     */
    
    /* copy the Src mac addr */
    memcpy (mh->mme_src, (void *)src_mac, ETH_ALEN);
    /* copy the Dst mac addr */
    memcpy (mh->mme_dest, (void *)dest, ETH_ALEN);
    /* copy the protocol */
    mh->mtype = htons (MME_TYPE);
    /* set default mme version */
    mh->mmv = MME_VERSION;
    mh->mmtype = ctx->mmtype;
    
    /* Set the max payload size */
    SET_MME_PAYLOAD_MAX_SIZE(mh->mmtype, payload_size);
    
    /* calculate number of mme packets needed */
    nbpkt = (ctx->tail - ctx->head) / payload_size;
    if(nbpkt == 0 || ((ctx->tail - ctx->head) % payload_size) > 0)
    {
        nbpkt++;
    }
    
    fmi_nf_mi  = nbpkt - 1;
    /* Set the sequence number to 1 if the message is fragmented.
     * This number correspond to a session number, all fragmented MME
     *  of this session must have the same fmsn number. */
    fmi_fmsn = fmi_nf_mi ? 1 : 0;
    mh->fmi = mme_fmi_set(fmi_nf_mi, 0, fmi_fmsn);
    
    return nbpkt;
}


int mme_prepare(MME_t *mh, mme_ctx_t *ctx, int *fo, int index, unsigned char *pkt)
{
    u16  fs = 0;
    u16  fmi_nf_mi = 0; /* Number of fragments */
    u16  fmi_fn_mi = 0; /* Fragment number */
    u16  fmi_fmsn = 0; /* SSN of the fragmented mme */
    u16  payload_size, payload_min_size;
    
    SET_MME_PAYLOAD_MAX_SIZE(mh->mmtype, payload_size);
    SET_MME_PAYLOAD_MIN_SIZE(mh->mmtype, payload_min_size);
    
    /*
     * --- Set per-message fragmentation info, current fragment number ---
     */
    mme_fmi_get(mh->fmi, &fmi_nf_mi, &fmi_fn_mi, &fmi_fmsn);
    mh->fmi = mme_fmi_set(fmi_nf_mi, index, fmi_fmsn);
    /*
     * --- Append payload ---
     */
    
    /* In case of VS MME, append the OUI first */
    if(MMTYPE_IS_VS(mh->mmtype))
    {
        memcpy (pkt, OUI_MSTAR, MSTAR_OUI_SIZE);
        pkt += MSTAR_OUI_SIZE;
    }
    
    /* calculate the size of the current fragment */
    fs = ((ctx->tail - *fo) < payload_size) ? (ctx->tail - *fo) : payload_size;
    /* copy the content into packet buffer */
    memcpy(pkt, ctx->buffer + *fo, fs);
    /* update fo */
    *fo += fs;
    
    /* zero-pad the rest if last packet fs < MME_MIN_SIZE,
     * which is minimum size for mme packet */
    if(fs < payload_min_size)
    {
        memset(pkt + fs, 0x0, payload_min_size - fs);
        fs = payload_min_size;
    }
    
    /* In case of VS MME, add SPIDCOM_OUI_SIZE to the payload */
    if(MMTYPE_IS_VS(mh->mmtype))
    {
        fs += MSTAR_OUI_SIZE;
    }
    
    return (fs + sizeof(MME_t));
}











mme_error_t mme_send(bool bPlcNodeFlag, void *pPev, mme_ctx_t *pCtx)
{
    MME_t *mh;
    u8  *pkt;
    u8  *pBuf = NULL;
    
    i32 pkt_len;
    i32 sent = -1;
    u16 nbpkt = 0;
    i32 fo;                                                                     // current fragment offset
    
    u16 fmi_nf_mi = 0;                                                          // Number of fragments
    u16 fmi_fn_mi = 0;                                                          // Fragment number
    u16 fmi_fmsn = 0;                                                           // SSN of the fragmented mme
    i32 i;
    
    struct pev_item *pPevTemp = NULL;
    
    /* protect from null pointers */
    if(pPev == NULL || pCtx == NULL)
    {
        EN_SLOGE(TAG, "pPev/pCtx is null");
        return MME_ERROR_GEN;
    }
    
    /* check if ctx has been inititalized */
    if(pCtx->status == MME_STATUS_INIT)
    {
        EN_SLOGE(TAG, "ctx has not been inititalized");
        return MME_ERROR_NOT_INIT;
    }
    
    
    /* protect from null pointers */
    pPevTemp = (struct pev_item *)pPev;
    if(pPevTemp->stMmeDev.i32Index >= cMse102xDevNum)
    {
        EN_SLOGE(TAG, "i32Index 超限");
        return MME_ERROR_GEN;
    }
    pBuf = pMmeCache[pPevTemp->stMmeDev.i32Index]->u8TxBuf;
    mh = (MME_t *)pBuf;
    
    
    /* nbpkt = the number of packet */
    if(bPlcNodeFlag == true)
    {
        //一些MME帧发送目的地址是 macEvsePlcNode
        nbpkt = mme_header_prepare(mh, pCtx, pPevTemp->stMmeDev.u8MacEvseHsot, pPevTemp->stMmeDev.u8MacEvsePlcNode);
    }
    else
    {
        nbpkt = mme_header_prepare(mh, pCtx, pPevTemp->stMmeDev.u8MacEvseHsot, pPevTemp->u8MacEvHost);
    }
    mme_fmi_get(mh->fmi, &fmi_nf_mi, &fmi_fn_mi, &fmi_fmsn);
    
    
    /* Security check, if nbpkt is more than 15 don't send anything
     * and inform the user. */
    if(fmi_nf_mi > 0xf)
    {
        EN_SLOGE(TAG, "The MME to send is too long and cannot be fragmented");
        return MME_ERROR_SPACE;
    }
    
    
    /* initialize fragment offset to the begining of the payload*/
    fo = pCtx->head;
    
    
    /* prepare and send out packets */
    for(i = 0; i < nbpkt; i++)
    {
        /*
         * --- Append payload ---
         */
        /* skip the MME header */
        pkt = pBuf + sizeof(MME_t);
        
        
        /* determine the size of whole packet (header + data) */
        pkt_len = mme_prepare(mh, pCtx, &fo, i, pkt);
        
        
        /*
         * --- Send raw packet ---
         */
        /* initialize pkt pointer to the start of the send buffer */
        pkt = pBuf;
        sMmePacketPrint(true, pkt, pkt_len);
        if(sMse102xSend(pPevTemp->stMmeDev.i32Index, pkt_len, pkt) != true)
        {
            EN_SLOGE(TAG, "帧长:%d,发送:%d,发送失败",pkt_len, sent);
            return MME_ERROR_TXRX;
        }
    
    } //for 
    
    return MME_SUCCESS;
}












void mme_remove_header(unsigned short mme_type, unsigned char *recv_pkt, int pkt_len, mme_ctx_t *ctx)
{
    unsigned int result_len;
    mme_error_t ret;
    /* Get the MME Header size */
    u16 mme_h_size; /* Size of MME header */
    SET_MME_HEADER_SIZE(mme_type, mme_h_size);
    
    recv_pkt += mme_h_size;
    pkt_len -= mme_h_size;
    
    /* copy the packet into the receive buffer */
    ret = mme_put(ctx, recv_pkt, pkt_len, &result_len);
    
    if(ret != MME_SUCCESS)
    {
        EN_SLOGD(TAG, "mme_put() error. ret = %d", ret);
    }
}





