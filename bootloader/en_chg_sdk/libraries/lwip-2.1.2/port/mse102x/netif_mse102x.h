#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

#include "en_common.h"







/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
typedef struct 
{
    /* Add whatever per-interface state that is needed here. */
    i32                                 i32PlcDevIndex;                         //PLC设备序号
    u8                                  u8Mac[ETH_ALEN];                        //为该网卡配置的MAC地址
    char                                u8Name[2];                              //网卡名字 长度为2 是受Lwip协议栈限制的
}stMes102xNetifData_t;














err_t netif_mse102x_init(struct netif *netif);
void netif_mse102x_input(struct netif *netif, u16 u16Len, u8 *pData);






#endif 







