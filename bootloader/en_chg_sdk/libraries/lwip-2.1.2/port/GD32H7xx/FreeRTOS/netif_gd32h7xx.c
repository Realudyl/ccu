/**
* @file
* Ethernet Interface Skeleton
*
*/

/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*
*/

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "err.h"
#include "netif_gd32h7xx.h"


#include "gd32h7xx_enet.h"
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "list.h"

#include "mse102x_drv.h"



#define NETIF_GD32H7XX_INPUT_TASK_STACK_SIZE        (512)
#define NETIF_GD32H7XX_INPUT_TASK_PRIO              (configMAX_PRIORITIES - 1)
#define LOWLEVEL_OUTPUT_WAITING_TIME                (250)
/* The time to block waiting for input */
#define LOWLEVEL_INPUT_WAITING_TIME               ((portTickType )100)

/* define those to better describe your network interface */
#define IFNAME0 'G'
#define IFNAME1 'D'

/* ENET RxDMA/TxDMA descriptor */
extern enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM], txdesc_tab[ENET_TXBUF_NUM];

/* ENET receive buffer  */
extern uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];

/* ENET transmit buffer */
extern uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];

/*global transmit and receive descriptors pointers */
extern enet_descriptors_struct  *dma_current_txdesc;
extern enet_descriptors_struct  *dma_current_rxdesc;

/* preserve another ENET RxDMA/TxDMA ptp descriptor for normal mode */
enet_descriptors_struct  ptp_txstructure[ENET_TXBUF_NUM];
enet_descriptors_struct  ptp_rxstructure[ENET_RXBUF_NUM];


static struct netif *low_netif = NULL;
xSemaphoreHandle gd32h7xx_rx_semaphore[2] = {NULL, NULL};

/**
* In this function, the hardware should be initialized.
* Called from netif_gd32h7xx_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void low_level_init(struct netif *netif)
{
    uint32_t i;
    stEnetNetifData_t *pNetifData = netif->state;
    i32 i32PeriphEnet = -1;
    
    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    
    /* set MAC hardware address */
    memcpy(netif->hwaddr, pNetifData->u8Mac,  sizeof(pNetifData->u8Mac ));
    memcpy(netif->name,   pNetifData->u8Name, sizeof(pNetifData->u8Name));
    
    /* set netif maximum transfer unit */
    netif->mtu = 1500;
    
    /* accept broadcast address and ARP traffic */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    
    /* create binary semaphore used for informing ethernetif of frame reception */
    if(gd32h7xx_rx_semaphore[pNetifData->i32EnetDevIndex] == NULL)
    {
        vSemaphoreCreateBinary(gd32h7xx_rx_semaphore[pNetifData->i32EnetDevIndex]);
        xSemaphoreTake(gd32h7xx_rx_semaphore[pNetifData->i32EnetDevIndex], 0);
    }
    
    switch(pNetifData->i32EnetDevIndex)
    {
    case 0:
        i32PeriphEnet = ENET0;
        
        /* initialize MAC address in ethernet MAC */
        enet_mac_address_set(i32PeriphEnet, ENET_MAC_ADDRESS0, netif->hwaddr);
        break;
    case 1:
        i32PeriphEnet = ENET1;
        
        /* initialize MAC address in ethernet MAC */
        enet_mac_address_set(i32PeriphEnet, ENET_MAC_ADDRESS1, netif->hwaddr);
        break;
    }
    
    /* initialize descriptors list: chain/ring mode */
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_ptp_enhanced_descriptors_chain_init(i32PeriphEnet, ENET_DMA_TX);
    enet_ptp_enhanced_descriptors_chain_init(i32PeriphEnet, ENET_DMA_RX);
#else
    enet_descriptors_chain_init(i32PeriphEnet, ENET_DMA_TX);
    enet_descriptors_chain_init(i32PeriphEnet, ENET_DMA_RX);
    
//    enet_descriptors_ring_init(ENET0, ENET_DMA_TX);
//    enet_descriptors_ring_init(ENET0, ENET_DMA_RX);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */
    
    /* enable ethernet Rx interrrupt */
    {
        int i;
        for(i = 0; i < ENET_RXBUF_NUM; i++)
        {
            enet_rx_desc_immediate_receive_complete_interrupt(&rxdesc_tab[i]);
        }
    }
    
#ifdef CHECKSUM_BY_HARDWARE
    /* enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for(i = 0; i < ENET_TXBUF_NUM; i++)
    {
        enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
    }
#endif /* CHECKSUM_BY_HARDWARE */
    
    /* create the task that handles the ETH_MAC */
    xTaskCreate(netif_gd32h7xx_input, "ETHERNETIF_INPUT", NETIF_GD32H7XX_INPUT_TASK_STACK_SIZE, netif, NETIF_GD32H7XX_INPUT_TASK_PRIO, NULL);
    
    /* enable MAC and DMA transmission and reception */
    enet_enable(i32PeriphEnet);
}


/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    static xSemaphoreHandle s_tx_semaphore = NULL;
    struct pbuf *q;
    uint8_t *buffer ;
    uint16_t framelength = 0;
    ErrStatus reval = ERROR;
    
    stEnetNetifData_t *pNetifData = netif->state;
    i32 i32PeriphEnet = -1;
    
    switch(pNetifData->i32EnetDevIndex)
    {
    case 0:
        i32PeriphEnet = ENET0;
        break;
    case 1:
        i32PeriphEnet = ENET1;
        break;
    }
    
    SYS_ARCH_DECL_PROTECT(sr);
    
    if(s_tx_semaphore == NULL)
    {
        vSemaphoreCreateBinary(s_tx_semaphore);
    }
    
    if(xSemaphoreTake(s_tx_semaphore, LOWLEVEL_OUTPUT_WAITING_TIME))
    {
        SYS_ARCH_PROTECT(sr);
        
        while((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_DAV))
        {
            ;
        }
        
        buffer = (uint8_t *)(enet_desc_information_get(i32PeriphEnet, dma_current_txdesc, TXDESC_BUFFER_1_ADDR));
        
        for(q = p; q != NULL; q = q->next)
        {
            memcpy((uint8_t *)&buffer[framelength], q->payload, q->len);
            framelength = framelength + q->len;
        }
        
        /* transmit descriptors to give to DMA */
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
        reval = ENET_NOCOPY_PTPFRAME_TRANSMIT_ENHANCED_MODE(i32PeriphEnet, framelength, NULL);
#else
        reval = ENET_NOCOPY_FRAME_TRANSMIT(i32PeriphEnet, framelength);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */
        SYS_ARCH_UNPROTECT(sr);
        
        /* give semaphore and exit */
        xSemaphoreGive(s_tx_semaphore);
    }
    
    if(SUCCESS == reval)
    {
        return ERR_OK;
    }
    else
    {
        while(1)
        {
            ;
        }
    }

}

/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL, *q;
    uint32_t l = 0;
    u16_t len;
    uint8_t *buffer;
    
    stEnetNetifData_t *pNetifData = netif->state;
    i32 i32PeriphEnet = -1;
    
    switch(pNetifData->i32EnetDevIndex)
    {
    case 0:
        i32PeriphEnet = ENET0;
        break;
    case 1:
        i32PeriphEnet = ENET1;
        break;
    }
    
    /* obtain the size of the packet and put it into the "len" variable. */
    len = enet_desc_information_get(i32PeriphEnet, dma_current_rxdesc, RXDESC_FRAME_LENGTH);
    buffer = (uint8_t *)(enet_desc_information_get(i32PeriphEnet, dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));
    
    if(pNetifData->bDttFlag == true)
    {
        // 将从上位机接收到的以太网报文通过一函数打包SPI帧头帧尾并发送给PLC
        sMse102xSend(pNetifData->i32EnetDevIndex, len, buffer);
    }
    else
    {
        if(len > 0)
        {
            /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
            p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        }
        
        if(p != NULL)
        {
            for(q = p; q != NULL; q = q->next)
            {
                memcpy((uint8_t *)q->payload, (u8_t *)&buffer[l], q->len);
                l = l + q->len;
            }
        }
    }
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    ENET_NOCOPY_PTPFRAME_RECEIVE_ENHANCED_MODE(i32PeriphEnet, NULL);
#else
    ENET_NOCOPY_FRAME_RECEIVE(i32PeriphEnet);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */
    
    return p;
}


/**
* This function is the netif_gd32h7xx_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function low_level_input()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/
void netif_gd32h7xx_input(void *pvParameters)
{
    struct pbuf *p;
    SYS_ARCH_DECL_PROTECT(sr);
    
    struct netif *pNetif = (struct netif *)pvParameters;
    stEnetNetifData_t *pNetifData = pNetif->state;
    
    for(;;)
    {
        if(pdTRUE == xSemaphoreTake(gd32h7xx_rx_semaphore[pNetifData->i32EnetDevIndex], LOWLEVEL_INPUT_WAITING_TIME))
        {
TRY_GET_NEXT_FRAME:
            SYS_ARCH_PROTECT(sr);
            p = low_level_input(pNetif);
            SYS_ARCH_UNPROTECT(sr);
            
            if(p != NULL)
            {
                if(ERR_OK != pNetif->input(p, pNetif))
                {
                    pbuf_free(p);
                }
                else
                {
                    goto TRY_GET_NEXT_FRAME;
                }
            }
        }
    }
    
    vTaskDelete(NULL);
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t netif_gd32h7xx_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    
#if LWIP_NETIF_HOSTNAME
    /* initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */
    
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;
    
    /* initialize the hardware */
    low_level_init(netif);
    
    return ERR_OK;
}
