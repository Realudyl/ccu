/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     enet_app.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-9-9
 * @Attention             :     
 * @Brief                 :     以太网用户实现程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-9-9
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "enet_app.h"

#include "en_log.h"
#include "en_mem.h"
#include "en_shell.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "enet_app";




struct netif *pEnetNetif[eEnetNumMax] = {NULL, NULL};








bool sEnetAppOpen(i32 i32EnetDevIndex, bool bDttFlag, void (*pPlugInFunc)(struct netif *pNetif), void (*pPlugOutFunc)(struct netif *pNetif));


void sEnetAppLinkCheckTask(void *pParam);
void sEnetAppDhcpTask(void *pParam);


void sEnetAppLwipNetifLinkCallback(struct netif *pNetif);
void sEnetAppLwipNetifStatusCallback(struct netif *pNetif);


// 对外提供可调用的API
i8 sEnetAppPhyLinkStatusGet(i32 i32EnetDevIndex);
const char* sEnetAppNetifIpGet(i32 i32EnetDevIndex);
bool sEnetAppNetifMacAddrSet(i32 i32EnetDevIndex, const u8* pMacAddr);


bool sEnlogShellEnetGetIpCmd(const stShellPkt_t *pkg);
bool sEnlogShellEnetSetIpv4Cmd(const stShellPkt_t *pkg);
bool sEnlogShellEnetSetMacAddrCmd(const stShellPkt_t *pkg);
bool sEnlogShellEnetSetDttFlagCmd(const stShellPkt_t *pkg);








stShellCmd_t stSellCmdEnetCmd[] = 
{
    {
        .pCmd       = "enetgetip",
        .pFormat    = "格式:enetgetip ENET设备号",
        .pFunction  = "功能:获取对应Enet的静态Ipv4地址操作命令",
        .pRemarks   = "备注:enetgetip 0",
        .pFunc      = sEnlogShellEnetGetIpCmd,
    },
    {
        .pCmd       = "enetsetipv4",
        .pFormat    = "格式:enetsetipv4 ENET设备号 Ipv4地址 子网掩码 默认网关",
        .pFunction  = "功能:设置对应Enet的静态Ipv4地址操作命令",
        .pRemarks   = "备注:enetsetipv4 0 192.168.1.168 255.255.255.0 192.168.1.255",
        .pFunc      = sEnlogShellEnetSetIpv4Cmd,
    },
    {
        .pCmd       = "enetsetmacaddr",
        .pFormat    = "格式:enetsetmacaddr ENET设备号 Mac地址",
        .pFunction  = "功能:设置对应Enet的Mac地址操作命令",
        .pRemarks   = "备注:enetsetmacaddr 0 04-0A-0D-0E-0D-06",
        .pFunc      = sEnlogShellEnetSetMacAddrCmd,
    },
    {
        .pCmd       = "enetsetdtt",
        .pFormat    = "格式:enetsetdtt ENET设备号 数据透传标志值",
        .pFunction  = "功能:设置对应Enet的数据透传标志操作命令",
        .pRemarks   = "备注:enetsetdtt 0 0",
        .pFunc      = sEnlogShellEnetSetDttFlagCmd,
    },
};





stEnetNetifData_t stNetifData[eEnetNumMax] = 
{
    //ENET0
    {
        .i32EnetDevIndex    = eEnetNum0,
        .u8Mac              = {0x04, 0x0A, 0x0D, 0x0E, 0x0D, 0x06},
        .u8Name             = "E0",
        
        .bDttFlag           = false,
        
        .pSocketList        = NULL,
        
        .pPlugInFunc        = NULL,
        .pPlugOutFunc       = NULL,
    },
    
    //ENET1
    {
        .i32EnetDevIndex    = eEnetNum1,
        .u8Mac              = {0x04, 0x0A, 0x0D, 0x0E, 0x0D, 0x07},
        .u8Name             = "E1",
        
        .bDttFlag           = false,
        
        .pSocketList        = NULL,
        
        .pPlugInFunc        = NULL,
        .pPlugOutFunc       = NULL,
    },
};




/*******************************************************************************
 * @FunctionName   :      sEnetAppOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月21日  14:20:03
 * @Description    :      以太网用户实现程序 ———— 资源初始化
 * @Input          :      i32EnetDevIndex   以太网网口设备号
 * @Input          :      pPlugInFunc       传入的回调函数: 接入网线处理函数
 * @Input          :      pPlugOutFunc      传入的回调函数: 拔出网线处理函数
 * @Return         :      
*******************************************************************************/
bool sEnetAppOpen(i32 i32EnetDevIndex, bool bDttFlag, void (*pPlugInFunc)(struct netif *pNetif), void (*pPlugOutFunc)(struct netif *pNetif))
{
    bool bRst = true;
    static bool bCmdRst = false;
    
    ip_addr_t stIpAddr  = {0};
    ip_addr_t stNetMask = {0};
    ip_addr_t stGateWay = {0};
    
    if(i32EnetDevIndex >= eEnetNumMax)
    {
        //ENET接口超限
        bRst = false;
        return bRst;
    }
    
    // ENET外设初始化
    bRst = sEnetDrvInit(i32EnetDevIndex);
    
    if(bRst == true)
    {
        // 网卡申请内存
        pEnetNetif[i32EnetDevIndex] = (struct netif *)MALLOC(sizeof(struct netif));
        if(pEnetNetif[i32EnetDevIndex] == NULL)
        {
            EN_SLOGE(TAG, "ENET%d : 设备初始化 之 网卡 netif 初始化失败, 内存不足", i32EnetDevIndex);
            bRst = false;
            return bRst;
        }
        memset(pEnetNetif[i32EnetDevIndex], 0, sizeof(struct netif));
        
        stNetifData[i32EnetDevIndex].bDttFlag = bDttFlag;
        
        stNetifData[i32EnetDevIndex].pPlugInFunc  = pPlugInFunc;
        stNetifData[i32EnetDevIndex].pPlugOutFunc = pPlugOutFunc;
        
        memcpy(pEnetNetif[i32EnetDevIndex]->hwaddr, stNetifData[i32EnetDevIndex].u8Mac, sizeof(stNetifData[i32EnetDevIndex].u8Mac));
        pEnetNetif[i32EnetDevIndex]->hwaddr_len = sizeof(stNetifData[i32EnetDevIndex].u8Mac);
        memcpy(pEnetNetif[i32EnetDevIndex]->name, stNetifData[i32EnetDevIndex].u8Name, sizeof(stNetifData[i32EnetDevIndex].u8Name));
        
        if(NULL == netif_add(pEnetNetif[i32EnetDevIndex], &stIpAddr.u_addr.ip4, &stNetMask.u_addr.ip4, &stGateWay.u_addr.ip4, &stNetifData[i32EnetDevIndex], &netif_gd32h7xx_init, &tcpip_input))
        {
            EN_SLOGE(TAG, "ENET%d : 设备初始化 之 netif_add 失败", i32EnetDevIndex);
            if(pEnetNetif[i32EnetDevIndex] != NULL)
            {
                FREE(pEnetNetif[i32EnetDevIndex]);
            }
            bRst = false;
            return bRst;
        }
        
        netif_set_link_callback(pEnetNetif[i32EnetDevIndex], sEnetAppLwipNetifLinkCallback);
        netif_set_status_callback(pEnetNetif[i32EnetDevIndex], sEnetAppLwipNetifStatusCallback);
        
        netif_set_link_down(pEnetNetif[i32EnetDevIndex]);
        
        // 创建 EthLink 检测任务
        if(pdPASS != xTaskCreate(sEnetAppLinkCheckTask, "sEnetAppLinkCheckTask", 1024, pEnetNetif[i32EnetDevIndex], 15, NULL))
        {
            EN_SLOGE(TAG, "ENET%d : sEnetAppLinkCheckTask 创建失败!", i32EnetDevIndex);
            if(pEnetNetif[i32EnetDevIndex] != NULL)
            {
                FREE(pEnetNetif[i32EnetDevIndex]);
            }
            bRst = false;
            return bRst;
        }
        
        // Shell指令注册 --- 注册成功一次即可
        if(bCmdRst == false)
        {
            for(u8 i = 0; i < (sizeof(stSellCmdEnetCmd)/sizeof(stShellCmd_t)); i++)
            {
                bCmdRst = sShellCmdRegister(&stSellCmdEnetCmd[i]);
            }
        }
    }
    
    return bRst;
}





/*******************************************************************************
 * @FunctionName   :      sEnetAppLinkCheckTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月18日  20:12:26
 * @Description    :      以太网用户实现程序 ———— 网线接入检查 任务
 * @Input          :      pParam      
*******************************************************************************/
void sEnetAppLinkCheckTask(void *pParam)
{
    struct netif *pNetif = NULL;
    stEnetNetifData_t *pData = NULL;
    i8 i8PhyLinkStatus = -1;
    bool bNetGetInFlag = false;
    
    if(pParam == NULL)
    {
        EN_SLOGE(TAG, "EthLink 检测任务创建失败! 失败原因：传入参数pParam为NULL!");
        vTaskDelete(NULL);
        return;
    }
    
    pNetif = (struct netif *)pParam;
    pData = (stEnetNetifData_t *)pNetif->state;
    
    EN_SLOGI(TAG, "网卡'%s' : EthLink 检测任务创建成功!", pNetif->name);
    
    vTaskDelay(5000 / portTICK_RATE_MS);
    
    while(1)
    {
        // 获取网线物理连接状态
        i8PhyLinkStatus = sEnetAppPhyLinkStatusGet(pData->i32EnetDevIndex);
        
        if((i8PhyLinkStatus > 0) && (bNetGetInFlag != true))
        {
            bNetGetInFlag = true;
            netif_set_link_up(pNetif);
        }
        else if((i8PhyLinkStatus <= 0) && (bNetGetInFlag == true))
        {
            bNetGetInFlag = false;
            netif_set_link_down(pNetif);
        }
        
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    
    EN_SLOGI(TAG, "网卡'%s' : EthLink 检测任务已删除!", pNetif->name);
    vTaskDelete(NULL);
}




/*******************************************************************************
 * @FunctionName   :      sEnetAppDhcpTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月9日  15:50:32
 * @Description    :      以太网用户实现程序 ———— 动态获取Ipv4地址 任务
 * @Input          :      pParam      
*******************************************************************************/
void sEnetAppDhcpTask(void *pParam)
{
    ip_addr_t stIpAddr  = {0};
    ip_addr_t stNetMask = {0};
    ip_addr_t stGateWay = {0};
    
    eDhcpState_t eDhcpState = eDhcpStart;
    
    struct netif *pNetif = NULL;
    struct dhcp *pDhcpClient = NULL;
    
    if(pParam == NULL)
    {
        EN_SLOGE(TAG, "DHCP 动态获取地址 任务创建失败! 失败原因：传入参数pParam为NULL!");
        vTaskDelete(NULL);
        return;
    }
    
    pNetif = (struct netif *)pParam;
    
    EN_SLOGI(TAG, "网卡'%s' : DHCP 动态获取地址 任务创建成功!", pNetif->name);
    
    while(1)
    {
        switch(eDhcpState)
        {
        case eDhcpStart:
            dhcp_start(pNetif);
            eDhcpState = eDhcpWaitAddress;
            break;
            
        case eDhcpWaitAddress:
            // 获取最新的IP地址, 子网掩码, 默认网关
            stIpAddr.u_addr.ip4.addr = pNetif->ip_addr.u_addr.ip4.addr;
            stNetMask.u_addr.ip4.addr = pNetif->netmask.u_addr.ip4.addr;
            stGateWay.u_addr.ip4.addr = pNetif->gw.u_addr.ip4.addr;
            
            if((0 != stIpAddr.u_addr.ip4.addr) && (0 != stNetMask.u_addr.ip4.addr) && (0 != stGateWay.u_addr.ip4.addr))
            {
                eDhcpState = eDhcpAddressAssigned;
                EN_SLOGI(TAG, "网卡'%s' : DHCP 动态获取  Ipv4地址: %d.%d.%d.%d  子网掩码: %d.%d.%d.%d  默认网关: %d.%d.%d.%d", pNetif->name,\
                    ip4_addr1_16(&stIpAddr.u_addr.ip4), ip4_addr2_16(&stIpAddr.u_addr.ip4), ip4_addr3_16(&stIpAddr.u_addr.ip4), ip4_addr4_16(&stIpAddr.u_addr.ip4),\
                    ip4_addr1_16(&stNetMask.u_addr.ip4), ip4_addr2_16(&stNetMask.u_addr.ip4), ip4_addr3_16(&stNetMask.u_addr.ip4), ip4_addr4_16(&stNetMask.u_addr.ip4),\
                    ip4_addr1_16(&stGateWay.u_addr.ip4), ip4_addr2_16(&stGateWay.u_addr.ip4), ip4_addr3_16(&stGateWay.u_addr.ip4), ip4_addr4_16(&stGateWay.u_addr.ip4));
            }
            else
            {
                pDhcpClient = netif_dhcp_data(pNetif);
                if(pDhcpClient->tries > cDhcpRetryMax)
                {
                    // DHCP 超时
                    eDhcpState = eDhcpTimeout;
                    
                    // 停止 DHCP
                    dhcp_stop(pNetif);
                    
                    EN_SLOGI(TAG, "网卡'%s' : DHCP 动态获取失败! 请用户通过shell指令进行手动设置!", pNetif->name);
                }
            }
            break;
            
        default:
            EN_SLOGI(TAG, "网卡'%s' : DHCP 动态获取地址 任务已删除!", pNetif->name);
            vTaskDelete(NULL);
            break;
        }
        
        vTaskDelay(50 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}




/*******************************************************************************
 * @FunctionName   :      sEnetAppLwipNetifLinkCallback
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月23日  20:39:27
 * @Description    :      以太网用户实现程序 ———— 网线物理链路状态回调函数
 * @Input          :      pNetif      
*******************************************************************************/
void sEnetAppLwipNetifLinkCallback(struct netif *pNetif)
{
    stEnetNetifData_t *pData = (stEnetNetifData_t *)pNetif->state;
    ip_addr_t stZeroAddr = {0};
    
    
    if((pNetif->flags & NETIF_FLAG_LINK_UP) != 0)
    {
        EN_SLOGD(TAG, "网卡'%s' : ENET%d 网线已接入! 开始初始化外设!", pNetif->name, pData->i32EnetDevIndex);
        
        // ENET外设初始化
        if(sEnetDrvInitDevInit(pData->i32EnetDevIndex))
        {
            EN_SLOGI(TAG, "网卡'%s' : 外设初始化成功!", pNetif->name);
            netif_set_up(pNetif);
            
            if(pData->pPlugInFunc != NULL)
            {
                pData->pPlugInFunc(pNetif);
            }
        }
        else
        {
            EN_SLOGE(TAG, "网卡'%s' : 外设初始化失败!", pNetif->name);
        }
    }
    else if((pNetif->flags & NETIF_FLAG_LINK_UP) == 0)
    {
        EN_SLOGD(TAG, "网卡'%s' : ENET%d 网线已拔出!", pNetif->name, pData->i32EnetDevIndex);
        
        netif_set_down(pNetif);
        netif_set_addr(pNetif, &stZeroAddr.u_addr.ip4, &stZeroAddr.u_addr.ip4, &stZeroAddr.u_addr.ip4);
        
        if(pData->pPlugOutFunc != NULL)
        {
            pData->pPlugOutFunc(pNetif);
        }
    }
}





/*******************************************************************************
 * @FunctionName   :      sEnetAppLwipNetifStatusCallback
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月13日  9:30:29
 * @Description    :      以太网用户实现程序 ———— 网络接口状态回调函数
 * @Input          :      pNetif      
*******************************************************************************/
void sEnetAppLwipNetifStatusCallback(struct netif *pNetif)
{
    stEnetNetifData_t *pData = (stEnetNetifData_t *)pNetif->state;
    
    if((pNetif->flags & NETIF_FLAG_UP) != 0)
    {
        if((pNetif->ip_addr.u_addr.ip4.addr == 0) && (pData->bDttFlag == false))
        {
            // 创建 DHCP 客户端 任务
            xTaskCreate(sEnetAppDhcpTask, "sEnetAppDhcpTask", 1024, pNetif, 16, NULL);
            vTaskDelay(500 / portTICK_RATE_MS);
        }
    }
    else if((pNetif->flags & NETIF_FLAG_UP) == 0)
    {
        if((pNetif->ip_addr.u_addr.ip4.addr != 0) && (pData->bDttFlag == false))
        {
            dhcp_release(pNetif);
        }
    }
}




/*******************************************************************************
 * @FunctionName   :      sEnetAppPhyLinkStatusGet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月21日  14:49:47
 * @Description    :      以太网用户实现程序 ———— 网线插拔状态获取 (0:未连接  1:已连接)
 * @Input          :      i32EnetDevIndex
 * @Return         :      
*******************************************************************************/
i8 sEnetAppPhyLinkStatusGet(i32 i32EnetDevIndex)
{
    u16 u16StatusValue = 0;
    
    if(i32EnetDevIndex >= eEnetNumMax)
    {
        return(-1);
    }
    
    if(sEnetDrvStatusRegGet(i32EnetDevIndex, &u16StatusValue))
    {
        return (u16StatusValue & cEnetPhyLinkStatus);
    }
    
    return(-1);
}




/*******************************************************************************
 * @FunctionName   :      sEnetAppNetifIpGet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月23日  14:35:09
 * @Description    :      以太网用户实现程序 ———— 获取IP地址
 * @Input          :      i32EnetDevIndex
 * @Return         :      
*******************************************************************************/
const char* sEnetAppNetifIpGet(i32 i32EnetDevIndex)
{
    static char ipBuff[50];             //IPV4地址需要16字节存放, IPV6地址需要46字节存放
    struct netif *pNetif = NULL;
    
    if(i32EnetDevIndex >= eEnetNumMax)
    {
        return(NULL);
    }
    pNetif = pEnetNetif[i32EnetDevIndex];
    
    memset(ipBuff, 0, sizeof(ipBuff));
    
    return ipaddr_ntoa_r(&pNetif->ip_addr, ipBuff, sizeof(ipBuff));
}




/*******************************************************************************
 * @FunctionName   :      sEnetAppNetifMacAddrSet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月24日  10:46:59
 * @Description    :      以太网用户实现程序 ———— 设置MAC地址
 * @Input          :      i32EnetDevIndex
 * @Input          :      pMacAddr    
 * @Return         :      
*******************************************************************************/
bool sEnetAppNetifMacAddrSet(i32 i32EnetDevIndex, const u8* pMacAddr)
{
    i8 i8PhyLinkStatus = -1;
    struct netif *pNetif = NULL;
    
    if((i32EnetDevIndex >= eEnetNumMax) || (pMacAddr == NULL))
    {
        return(false);
    }
    pNetif = pEnetNetif[i32EnetDevIndex];
    
    // 获取网线物理连接状态
    i8PhyLinkStatus = sEnetAppPhyLinkStatusGet(i32EnetDevIndex);
    
    if(i8PhyLinkStatus > 0)
    {
        netif_set_down(pNetif);
    }
    
    memcpy(pNetif->hwaddr, pMacAddr, pNetif->hwaddr_len);
    sEnetDrvMacAddrSet(i32EnetDevIndex, pNetif->hwaddr);
    
    etharp_cleanup_netif(pNetif);
    
    if(i8PhyLinkStatus > 0)
    {
        netif_set_up(pNetif);
    }
    
    return(true);
}









/*******************************************************************************
 * @FunctionName   :      sEnlogShellEnetGetIpCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月23日  15:30:15
 * @Description    :      以太网用户实现程序 ———— Shell命令获取Ip地址
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellEnetGetIpCmd(const stShellPkt_t *pkg)
{
    u8 u8EnetIndex;
    
    
    if(pkg->paraNum == 1)
    {
        u8EnetIndex = atoi(pkg->para[0]);
        if(u8EnetIndex >= eEnetNumMax)
        {
            return(false);
        }
        
        if(sEnetAppPhyLinkStatusGet(u8EnetIndex) > 0)
        {
            EN_SLOGD(TAG, "ENET%s 查询Ip地址成功! Ip地址: %s", pkg->para[0], sEnetAppNetifIpGet(u8EnetIndex));
        }
        else
        {
            EN_SLOGE(TAG, "ENET%s 查询Ip地址失败! 请检查网线是否已接入!", pkg->para[0]);
        }
        
        return(true);
    }
    else
    {
        EN_SLOGE(TAG, "Shell命令操作失败! 输入参数个数不符规范, 请确认后再次操作!");
        EN_SLOGE(TAG, "命令格式: enetgetip ENET设备号");
        EN_SLOGE(TAG, "示例:     enetgetip 0");
    }
    
    return(false);
}





/*******************************************************************************
 * @FunctionName   :      sEnlogShellEnetSetIpv4Cmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月12日  11:02:56
 * @Description    :      以太网用户实现程序 ———— Shell命令设置静态Ipv4地址
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellEnetSetIpv4Cmd(const stShellPkt_t *pkg)
{
    ip_addr_t stIpAddr  = {0};
    ip_addr_t stNetMask = {0};
    ip_addr_t stGateWay = {0};
    
    u8 u8EnetIndex;
    u8 u8Ip0, u8Ip1, u8Ip2, u8Ip3;
    u8 u8Mask0, u8Mask1, u8Mask2, u8Mask3;
    u8 u8Gw0, u8Gw1, u8Gw2, u8Gw3;
    
    
    if(pkg->paraNum == 4)
    {
        u8EnetIndex = atoi(pkg->para[0]);
        if(u8EnetIndex >= eEnetNumMax)
        {
            return(false);
        }
        
        if(sEnetAppPhyLinkStatusGet(u8EnetIndex) > 0)
        {
            if(0 == sscanf((const char *)pkg->para[1], "%hhu.%hhu.%hhu.%hhu", &u8Ip0, &u8Ip1, &u8Ip2, &u8Ip3))
            {
                return(false);
            }
            if(0 == sscanf((const char *)pkg->para[2], "%hhu.%hhu.%hhu.%hhu", &u8Mask0, &u8Mask1, &u8Mask2, &u8Mask3))
            {
                return(false);
            }
            if(0 == sscanf((const char *)pkg->para[3], "%hhu.%hhu.%hhu.%hhu", &u8Gw0, &u8Gw1, &u8Gw2, &u8Gw3))
            {
                return(false);
            }
            
            IP4_ADDR(&stIpAddr.u_addr.ip4, u8Ip0, u8Ip1, u8Ip2, u8Ip3);
            IP4_ADDR(&stNetMask.u_addr.ip4, u8Mask0, u8Mask1, u8Mask2, u8Mask3);
            IP4_ADDR(&stGateWay.u_addr.ip4, u8Gw0, u8Gw1, u8Gw2, u8Gw3);
            netif_set_addr(pEnetNetif[u8EnetIndex], &stIpAddr.u_addr.ip4, &stNetMask.u_addr.ip4, &stGateWay.u_addr.ip4);
            
            EN_SLOGD(TAG, "ENET%s 设置成功! Ipv4地址: %s  子网掩码: %s  默认网关: %s", pkg->para[0], pkg->para[1], pkg->para[2], pkg->para[3]);
        }
        else
        {
            EN_SLOGE(TAG, "ENET%s 设置失败! 请检查网线是否已接入!", pkg->para[0]);
        }
        
        return(true);
    }
    else
    {
        EN_SLOGE(TAG, "Shell命令操作失败! 输入参数个数不符规范, 请确认后再次操作!");
        EN_SLOGE(TAG, "命令格式: enetsetipv4 ENET设备号 Ipv4地址 子网掩码 默认网关");
        EN_SLOGE(TAG, "示例:     enetsetipv4 0 192.168.1.168 255.255.255.0 192.168.1.255");
    }
    
    return(false);
}




/*******************************************************************************
 * @FunctionName   :      sEnlogShellEnetSetMacAddrCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月24日  11:37:12
 * @Description    :      以太网用户实现程序 ———— Shell命令设置Mac地址
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellEnetSetMacAddrCmd(const stShellPkt_t *pkg)
{
    bool bRst;
    u8 u8EnetIndex;
    u8 u8Mac[ETH_ALEN];
    
    
    if(pkg->paraNum == 2)
    {
        u8EnetIndex = atoi(pkg->para[0]);
        if(u8EnetIndex >= eEnetNumMax)
        {
            return(false);
        }
        
        if(0 == sscanf((const char *)pkg->para[1], "%hhx-%hhx-%hhx-%hhx-%hhx-%hhx", &u8Mac[0], &u8Mac[1], &u8Mac[2], &u8Mac[3], &u8Mac[4], &u8Mac[5]))
        {
            return(false);
        }
        
        bRst = sEnetAppNetifMacAddrSet(u8EnetIndex, u8Mac);
        if(bRst == true)
        {
            EN_SLOGD(TAG, "ENET%s 设置成功! Mac地址: %s", pkg->para[0], pkg->para[1]);
        }
        else
        {
            EN_SLOGE(TAG, "ENET%s 设置失败! 请检查传入参数是否正确!", pkg->para[0]);
        }
        
        return(true);
    }
    else
    {
        EN_SLOGE(TAG, "Shell命令操作失败! 输入参数个数不符规范, 请确认后再次操作!");
        EN_SLOGE(TAG, "命令格式: enetsetmacaddr ENET设备号 Mac地址");
        EN_SLOGE(TAG, "示例:     enetsetmacaddr 0 04-0A-0D-0E-0D-06");
    }
    
    return(false);
}





/*******************************************************************************
 * @FunctionName   :      sEnlogShellEnetSetDttFlagCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年10月11日  9:45:15
 * @Description    :      以太网用户实现程序 ———— Shell命令设置数据透传标志
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellEnetSetDttFlagCmd(const stShellPkt_t *pkg)
{
    u8 u8EnetIndex;
    stEnetNetifData_t *pData = NULL;
    
    
    if(pkg->paraNum == 2)
    {
        u8EnetIndex = atoi(pkg->para[0]);
        if(u8EnetIndex >= eEnetNumMax)
        {
            return(false);
        }
        
        pData = pEnetNetif[u8EnetIndex]->state;
        
        if(sEnetAppPhyLinkStatusGet(u8EnetIndex) > 0)
        {
            EN_SLOGE(TAG, "ENET%s 设置数据透传标志失败! 请先将网线拔出再设置!", pkg->para[0]);
        }
        else
        {
            pData->bDttFlag = atoi(pkg->para[1]);
            EN_SLOGD(TAG, "ENET%s 设置数据透传标志成功! 数据透传标志: %d", pkg->para[0], pData->bDttFlag);
        }
        
        return(true);
    }
    else
    {
        EN_SLOGE(TAG, "Shell命令操作失败! 输入参数个数不符规范, 请确认后再次操作!");
        EN_SLOGE(TAG, "命令格式: enetsetdtt ENET设备号 数据透传标志值(0 - 不透传 / 1 - 透传)");
        EN_SLOGE(TAG, "示例:     enetgetip 0 0");
    }
    
    return(false);
}







