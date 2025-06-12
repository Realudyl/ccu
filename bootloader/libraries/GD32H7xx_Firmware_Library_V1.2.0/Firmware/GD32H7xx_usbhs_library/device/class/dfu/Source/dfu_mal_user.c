/*!
    \file    dfu_mal.c
    \brief   USB DFU device media access layer functions

    \version 2024-01-05, V1.2.0, firmware for GD32H7xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "dfu_mal_user.h"



dfu_mal_prop DFU_Flash_cb =
{
    flash_if_init,
    flash_if_deinit,
    flash_if_erase,
    flash_if_write,
    flash_if_read,
    flash_if_checkaddr,
    60U, /* flash erase timeout in ms */
    80U  /* flash programming timeout in ms (80us * RAM Buffer size (1024 Bytes) */
};


uint8_t dfu_mal_init(void);
uint8_t dfu_mal_deinit(void);
uint8_t dfu_mal_erase(void);
uint8_t dfu_mal_write(uint8_t *buf, uint32_t addr, uint32_t len);
uint8_t* dfu_mal_read(uint8_t *buf, uint32_t addr, uint32_t len);
uint8_t dfu_mal_getstatus(uint32_t addr, uint8_t cmd, uint8_t *buffer);


static uint8_t dfu_mal_checkaddr(uint32_t addr);






/*!
    \brief      initialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t dfu_mal_init(void)
{
    /* initialize all supported memory medias */
    /* check if the memory media exists */
    if(NULL != DFU_Flash_cb.mal_init)
    {
        DFU_Flash_cb.mal_init();
    }
    
    return MAL_OK;
}

/*!
    \brief      deinitialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t dfu_mal_deinit(void)
{
    /* deinitializes all supported memory medias */
    /* check if the memory media exists */
    if(NULL != DFU_Flash_cb.mal_deinit)
    {
        DFU_Flash_cb.mal_deinit();
    }
    
    return MAL_OK;
}

/*!
    \brief      erase a memory sector
    \param[in]  addr: memory sector address/code
    \param[out] none
    \retval     MAL_OK
*/
uint8_t dfu_mal_erase(void)
{
    /* check if the operation is supported */
    if(NULL != DFU_Flash_cb.mal_erase)
    {
        return DFU_Flash_cb.mal_erase();
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      write data to sectors of memory
    \param[in]  buf: the data buffer to be write
    \param[in]  addr: memory sector address/code
    \param[in]  len: data length
    \param[out] none
    \retval     MAL_OK
*/
uint8_t dfu_mal_write(uint8_t *buf, uint32_t addr, uint32_t len)
{
    MAL_Status u8Status = dfu_mal_checkaddr(addr);
    
    if(u8Status == MAL_OK)
    {
        /* check if the operation is supported */
        if(NULL != DFU_Flash_cb.mal_write)
        {
            return DFU_Flash_cb.mal_write(buf, addr, len);
        }
        else
        {
            return MAL_FAIL;
        }
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      read data from sectors of memory
    \param[in]  buf: the data buffer to be write
    \param[in]  addr: memory sector address/code
    \param[in]  len: data length
    \param[out] none
    \retval     pointer to buffer
*/
uint8_t *dfu_mal_read(uint8_t *buf, uint32_t addr, uint32_t len)
{
    /* check if the operation is supported */
    if(NULL != DFU_Flash_cb.mal_read)
    {
        return DFU_Flash_cb.mal_read(buf, addr, len);
    }
    else
    {
        return buf;
    }
}

/*!
    \brief      get the status of a given memory and store in buffer
    \param[in]  addr: memory sector address/code
    \param[in]  cmd: 0 for erase and 1 for write
    \param[in]  buffer: pointer to the buffer where the status data will be stored
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t dfu_mal_getstatus(uint32_t addr, uint8_t cmd, uint8_t *buffer)
{
    MAL_Status u8Status = dfu_mal_checkaddr(addr);
    
    if(u8Status == MAL_OK)
    {
        if(cmd & 0x01U)
        {
            SET_POLLING_TIMEOUT(DFU_Flash_cb.write_timeout);
        }
        else
        {
            SET_POLLING_TIMEOUT(DFU_Flash_cb.erase_timeout);
        }
        return MAL_OK;
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      check the address is supported
    \param[in]  addr: memory sector address/code
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
static uint8_t dfu_mal_checkaddr(uint32_t addr)
{
    return DFU_Flash_cb.mal_checkaddr(addr);
}
