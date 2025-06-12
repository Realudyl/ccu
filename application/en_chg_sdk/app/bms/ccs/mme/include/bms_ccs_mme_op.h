/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     bms_ccs_mme_op.h
* Description                           :     MME协议实现 之 MME底层操作接口实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-07-03
* notice                                :     
****************************************************************************************************/
#ifndef _bms_ccs_mme_op_h
#define _bms_ccs_mme_op_h

#include "en_common_eth.h"

#include "bms_ccs_mme_data_def.h"


















//MME设备结构定义
//这里的MME设备是指本地的PLC载波芯片
typedef struct
{
    i32                                 i32Index;                               //MME设备的序号----当MCU外挂多路PLC载波芯片时会有效使用此字段
    struct netif                        *pNetif;                                //MME设备的虚拟网卡地址
    u8                                  u8MacEvseHsot[ETH_ALEN];                //MME设备的Mac地址:macEvseHost
    u8                                  u8MacEvsePlcNode[ETH_ALEN];             //MME设备的Mac地址:macEvsePlcNode
    u8                                  u8Name[2];                              //MME设备的网卡名, 2字节长度是被Lwip协议栈限定的
    
}stMmeDev_t;











/* function prototypes */
/**
 * Create a MME message context.
 * This context is used to build the MME message step by step.<br>
 * The provided buffer must be enough large to contain all the final payload.<br>
 * The MME payload can be bigger than an ethernet payload,
 * as the fragmentation is managed by the 'send' function.
 *
 * \param  ctx MME context to fill with init value
 * \param  mmtype type of MME message (must be in official type list)
 * \param  buffer the buffer to put the payload
 * \param  length  the buffer length
 * \return error type (MME_SUCCESS if success)
 */
mme_error_t mme_init(mme_ctx_t *ctx, u16 u16MmType , unsigned char *buffer, const unsigned int length);





/** Get the current MME payload length
 *
 * \param ctx MME context to get the payload length
 * \param length the returned length
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 */
mme_error_t mme_get_length(mme_ctx_t *ctx, unsigned int *length);





/** Get the remaining free space to add payload
 *
 * \param ctx MME context to get the remaining space
 * \param length the remaining space
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 */
mme_error_t mme_get_free_space(mme_ctx_t *ctx, unsigned int *length);





/**
 * Push data at the beginning of the MME payload.
 * Data are inserted between the start of buffer
 * and the current payload data.<br>
 * MME data tail and length are updated.<br>
 * If there is not enough free place to push data,
 * an error is returned and the remaining free space length is returned.
 *
 * \param  ctx MME context where to push data
 * \param  data data to be pushed into payload
 * \param  length length of data to push
 * \param  result_length length of data really pushed
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 * \return MME_ERROR_SPACE: not enough available space
 */
mme_error_t mme_push(mme_ctx_t *ctx, const void *data, unsigned int length, unsigned int *result_length);





/**
 * Put data at the end of MME payload. MME data tail and length are updated<br>
 * If there is not enough free place to put data,
 * an error is returned and the remaining free space length is returned.
 *
 * \param  ctx MME context where to put data
 * \param  data data to put at the end of MME payload
 * \param  length length of data to put
 * \param  result_length length of data really put
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 * \return MME_ERROR_SPACE: not enough available space
 */
mme_error_t mme_put(mme_ctx_t *ctx, const void *data, unsigned int length, unsigned int *result_length);





/**
 * Pull (get) data from beginning of MME payload.
 * MME data head and length are updated<br>
 * If there is not enough data to pull,
 * an error is returned and the remaining payload length is returned.
 *
 * \param  ctx MME context where to get data
 * \param  data buffer where to get data from the beginning of MME payload
 * \param  length length of data to pull
 * \param result_length length of data pulled;
 * if there is not enough data into the MME to fit the length,
 * the remaining data length is returned
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 */
mme_error_t mme_pull(mme_ctx_t *ctx, void *data, unsigned int length, unsigned int *result_length);





/**
 * Pop (get) data from the end of MME payload.
 * MME data tail and length are updated<br>
 * If there is not enough data to pull,
 * an error is returned and the remaining payload length is returned.
 *
 * \param  ctx MME context where to get data
 * \param  data buffer where to get data from the end of MME payload
 * \param  length length of data to pull
 * \param result_length length of data gotten;
 * if there is not enough data into the MME to fit the length,
 * the remaining data length is returned
 * \return error type (MME_SUCCESS if success)
 * \return MME_ERROR_NOT_INIT: context not initialized
 * \return MME_ERROR_SPACE: not enough available space
 */
mme_error_t mme_pop(mme_ctx_t *ctx, void *data, unsigned int length, unsigned int *result_length);





/**
* Build the MME header. It calculates the number of packet and returns it.
* \param mh MME header, filled by this function
* \param ctx MME context to send
* \param src_mac MAC Address of the source
* \param dest MAC Address of the destination
* \return int The number of packets required to send the MME
*/
//int mme_header_prepare(MME_t *mh, mme_ctx_t *ctx, unsigned char *src_mac, const unsigned char *dest);





/**
* Build a packet from the context. This function will be called by mme_send
* before sending each fragment of the packet. The function will update the
* fragment offset.
* This function is for internal use, and should not be used outside the
* library.
* \param mh MME header, will be updated with the correct fragment number
* \param ctx MME context to send
* \param fo Fragment offset, from which data must be sent
* \param index Fragment number
* \param pkt Pointer to the head of the data buffer
* \return int Size of the packet (including header)
*/
//int mme_prepare(MME_t *mh, mme_ctx_t *ctx, int *fo, int index, unsigned char *pkt);





/**
 * Send MME to destination MAC address
 * \param  iface interface where to listen to the MME; if NULL,
 *                          iface is guessed according to source
 * \param  ctx context to receive the MME data
 * \parm type type of MME
 * \parm dest destination MAC address
*/
mme_error_t mme_send(bool bPlcNodeFlag, void *pPev, mme_ctx_t *pCtx);











/**
 * Remove MME Header from the raw packet
 *
 *
 * \param  mme_type   mmtype of the raw packet
 * \param  recv_pkt  raw packet that you want to remove header
 * \param  pkt_len  data length of raw packet
 * \param  ctx  the packet without mme header
 */
void mme_remove_header(unsigned short mme_type, unsigned char *recv_pkt, int pkt_len, mme_ctx_t *ctx);










#endif





















