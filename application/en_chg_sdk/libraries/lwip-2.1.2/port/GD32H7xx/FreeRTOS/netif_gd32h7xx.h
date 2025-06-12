#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

#include "en_common.h"
#include "en_mem.h"





/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
typedef struct 
{
    /* Add whatever per-interface state that is needed here. */
    i32                                 i32EnetDevIndex;                        //ENET设备序号
    u8                                  u8Mac[ETH_ALEN];                        //为该网卡配置的MAC地址
    char                                u8Name[2];                              //网卡名字 长度为2 是受Lwip协议栈限制的
    
    bool                                bDttFlag;                               //数据透传标志 --- true:走数据透传  false:走lwip协议栈
    
    TaskHandle_t                        xTaskDtt;                               //数据透传任务句柄
    
    void*                               pSocketList;                            //指向用户定义的存有socket的结构体/链表
    
    void                                (*pPlugInFunc)(struct netif *pNetif);   //回调函数:网线物理链路接入处理函数
    void                                (*pPlugOutFunc)(struct netif *pNetif);  //回调函数:网线物理链路拔出处理函数
    
}stEnetNetifData_t;








err_t netif_gd32h7xx_init(struct netif *netif);
void netif_gd32h7xx_input( void * pvParameters );






#endif 
