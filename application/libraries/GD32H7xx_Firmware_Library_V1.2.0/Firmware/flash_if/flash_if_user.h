/*!
    \file    flash_if.h
    \brief   USB DFU device flash interface header file

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

#ifndef __FLASH_IF_USER_H
#define __FLASH_IF_USER_H

#include "gd32h7xx_libopt.h"
#include <stdlib.h>

#define FLASH_START_ADDR        0x08000000U
#define FLASH_END_ADDR          0x087FFFFFU

#define FMC_OB_START            (FMC_BASE + 0x1C)
#define FMC_OB_END              (FMC_BASE + 0x57)

#define OCRAM_START             0x24010000U
#define OCRAM_END               0x2407FFFFU

#define EFUSE_START             0x40022814U
#define EFUSE_END               0x40022843U
#define EFUSE_LEN               0x30U

#define AES_IV_START            0x52002068U
#define AES_IV_END              0x52002080U
#define AES_IV_LEN              0x18U

#define OB_RDPT0                0x1FFFC000U
#define OB_RDPT1                0x1FFEC000U

#define OB_RDPT                 0x1ffff800U

#define MAL_MASK_OB             0xFFFFFF00U

#define FLASH_PAGE_SIZE         0x1000U

//User Config
#define APPENTRYADDR            0x08080000U
#define APPENDADDR              0x083BFFFFU
#define ERSPAGESCNT             ((APPENDADDR + 1U - APPENTRYADDR) / FLASH_PAGE_SIZE)



typedef enum
{
    FLASH_OK = 0,
    FLASH_FAIL
} FLASH_Status;



/* function declarations */
extern uint8_t  flash_if_init       (void);
extern uint8_t  flash_if_deinit     (void);
extern uint8_t  flash_if_erase      (void);
extern uint8_t  flash_if_write      (uint8_t *buf, uint32_t addr, uint32_t len);
extern uint8_t* flash_if_read       (uint8_t *buf, uint32_t addr, uint32_t len);
extern uint8_t  flash_if_checkaddr  (uint32_t addr);

/* write the option byte */
extern fmc_state_enum Option_Byte_Write(uint32_t Mem_Add,uint8_t* data);

#endif /* __FLASH_IF_H */
