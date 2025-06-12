/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme.c
* Description                           :   MME协议实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   
****************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bms_ccs_mme.h"
#include "bms_ccs_mme_slac_param.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme";





stMmeCache_t *pMmeCache[cMse102xDevNum];










bool  sMmeInit(stMmeDev_t *pMmeDev, ePileGunIndex_t eGunIndex, void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));
bool  sMmeInitMap(void);
bool  sMmeInitCache(i32 i32MmeDevIndex, ePileGunIndex_t eGunIndex, void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));
bool  sMmeInitNetif(stMmeDev_t *pMmeDev);

void  sMmePacketPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);

bool  sMmeSetNetInit(i32 i32MmeDevIndex);
void  sMmeSetCpState(eChgCpState_t eCpState);
bool  sMmeGetSlacMatchFlag(i32 i32MmeDevIndex);
struct pev_item *sMmeGetPevHead(i32 i32MmeDevIndex);


void  sMmeRecvTask(void *pParam);
i32   sMmeCmdSearch(mmtype_t eType);

bool  sMmePacketSend(mmtype_t eType, bool bPlcNodeFlag, struct pev_item *pPev, mme_ctx_t *pCtx);
bool  sMmePacketRecv(stMmeDev_t *pMmeDev, MME_t *pEthPkt, i32 i32EthPktLen);





stMmeCmdTypeMap_t stMmeCmdTypeMap[] = 
{
    //EVCC主动发送的报文 无需设置等待时间
    //EVCC被动应答的报文 可以设置等待时间---要看报文重要程度
    { CM_SET_KEY_CNF,            "设置网络秘钥应答",   NULL,               1000, false,    sMmePktRecvCmSetKeyCnf           },
    { CM_SLAC_PARM_REQ,          "请求SLAC协议",       NULL,                  0, false,    sMmePktRecvCmSlacParmReq         },
    { CM_START_ATTEN_CHAR_IND,   "指示SLAC正在启动",   NULL,                  0, false,    sMmePktRecvCmStartAttenCharInd   },
    { CM_MNBC_SOUND_IND,         "指示衰减特性",       NULL,                  0, false,    sMmePktRecvCmMnbcSoundInd        },
    { CM_ATTEN_PROFILE_IND,      "指示测量衰减特性",   NULL,                  0, false,    sMmePktRecvCmAttenProfileInd     },
    { CM_ATTEN_CHAR_RSP,         "指示衰减特性应答",   NULL,  TT_match_response, false,    sMmePktRecvCmAttenCharRsp        },
    { CM_SLAC_MATCH_REQ,         "请求SLAC匹配成功",   NULL,                  0, false,    sMmePktRecvCmSlacMatchReq        },
    
    
    //以下是供应商自定义的MME帧---用于程序与载波芯片(MSE102x)通讯 而非EVCC
    { VS_GET_VERSION_CNF,        "请求PLC固件版本应答",NULL,                 20, false,    sMmePacketRecvVsGetVersionCnf    },
    { VS_RESET_CNF,              "请求PLC复位应答",    NULL,                 20, false,    sMmePacketRecvVsResetCnf         },
    { VS_GET_STATUS_CNF,         "请求PLC状态应答",    NULL,                 20, false,    sMmePacketRecvVsGetStatusCnf     },
    { VS_GET_LINK_STATS_CNF,     "链路流量信息应答",   NULL,                 20, false,    sMmePacketRecvVsGetLinkStatsCnf  },
    { VS_GET_NW_INFO_CNF,        "AVLN网络信息应答",   NULL,                 20, false,    sMmePacketRecvVsGetNwInfoCnf     },
    { VS_FILE_ACCESS_CNF,        "文件操作应答",       NULL,               3000, false,    sMmePacketRecvVsFileAccessCnf    },
    
};









/***************************************************************************************************
* Description                           :   MME 设备初始化函数
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmeInit(stMmeDev_t *pMmeDev, ePileGunIndex_t eGunIndex, void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn))
{
    bool bRst;
    
    //参数限制
    EN_SLOGI(TAG, "MME 设备初始化!!!");
    if(pMmeDev == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化失败, 传入参数为NULL");
        return(false);
    }
    
    
    //map 初始化
    bRst = sMmeInitMap();
    
    //cache 初始化
    bRst = bRst & sMmeInitCache(pMmeDev->i32Index, eGunIndex, pCpPwmControl);
    
    //netif 初始化
    bRst = bRst & sMmeInitNetif(pMmeDev);
    
    //MME接收任务创建
    xTaskCreate(sMmeRecvTask,     "sMmeRecvTask",     (512), pMmeDev, 26, NULL);
    xTaskCreate(sMmeSlacTaskSend, "sMmeSlacTaskSend", (512), pMmeDev, 25, &pMmeCache[pMmeDev->i32Index]->xTaskSlacSend);
    
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   MME 设备初始化 之 map初始化
* Author                                :   Hall
* Creat Date                            :   2023-12-26
* notice                                :   
****************************************************************************************************/
bool sMmeInitMap(void)
{
    i32  i;
    
    for(i = 0; i < sArraySize(stMmeCmdTypeMap); i++)
    {
        if(stMmeCmdTypeMap[i].i32WaitTime > 0)
        {
            //等待时间大于0的信号量才创建
            stMmeCmdTypeMap[i].xSemAck = xSemaphoreCreateBinary();
            xSemaphoreGive(stMmeCmdTypeMap[i].xSemAck);
        }
    }
    
    return(true);
}





/***************************************************************************************************
* Description                           :   MME 设备初始化 之 cache初始化
* Author                                :   Hall
* Creat Date                            :   2023-12-26
* notice                                :   
****************************************************************************************************/
bool sMmeInitCache(i32 i32MmeDevIndex, ePileGunIndex_t eGunIndex, void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn))
{
    //分配cache内存
    if(pMmeCache[i32MmeDevIndex] == NULL)
    {
        pMmeCache[i32MmeDevIndex] = (stMmeCache_t *)MALLOC(sizeof(stMmeCache_t));
        if(pMmeCache[i32MmeDevIndex] == NULL)
        {
            EN_SLOGE(TAG, "MME 设备初始化 之 cache初始化失败, 内存不足");
            return(false);
        }
    }
    
    //初始化cache成员
    memset(pMmeCache[i32MmeDevIndex], 0, sizeof(stMmeCache_t));
    pMmeCache[i32MmeDevIndex]->eGunIndex = eGunIndex;
    pMmeCache[i32MmeDevIndex]->stEvse.pev_item_head = (pev_item *)MALLOC(sizeof(pev_item));
    pMmeCache[i32MmeDevIndex]->stEvse.pev_item_head->next = NULL;
    pMmeCache[i32MmeDevIndex]->xSemTx = xSemaphoreCreateBinary();
    if(pMmeCache[i32MmeDevIndex]->xSemTx == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化 之 cache初始化失败, 信号量创建失败");
        return(false);
    }
    xSemaphoreGive(pMmeCache[i32MmeDevIndex]->xSemTx);
    
    pMmeCache[i32MmeDevIndex]->pCpPwmControl = pCpPwmControl;
    
    return(true);
}






/***************************************************************************************************
* Description                           :   MME 设备初始化 之 虚拟网卡 netif 初始化
* Author                                :   Hall
* Creat Date                            :   2023-12-26
* notice                                :   
****************************************************************************************************/
bool sMmeInitNetif(stMmeDev_t *pMmeDev)
{
    struct netif *pNetif = NULL;
    stMes102xNetifData_t *pNetifData = NULL;
    
    if(pMmeDev == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化 之 虚拟网卡 netif 初始化失败, 传入参数超限");
        return(false);
    }
    
    //虚拟网卡私有数据申请内存
    pNetifData = (stMes102xNetifData_t *)MALLOC(sizeof(stMes102xNetifData_t));
    if(pNetifData == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化 之 虚拟网卡 netif 初始化失败, 内存不足");
        return(false);
    }
    memcpy(pNetifData->u8Mac,  pMmeDev->u8MacEvseHsot,  sizeof(pMmeDev->u8MacEvseHsot));
    memcpy(pNetifData->u8Name, pMmeDev->u8Name, sizeof(pMmeDev->u8Name));
    pNetifData->i32PlcDevIndex = pMmeDev->i32Index;
    
    
    //虚拟网卡申请内存
    pNetif = (struct netif *)MALLOC(sizeof(struct netif));
    if(pNetif == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化 之 虚拟网卡 netif 初始化失败, 内存不足");
        FREE(pNetifData);
        pNetifData = NULL;
        
        return(false);
    }
    
    if(netif_add(pNetif, NULL, NULL, NULL, pNetifData, &netif_mse102x_init, &tcpip_input) == NULL)
    {
        EN_SLOGE(TAG, "MME 设备初始化 之 netif_add失败");
        FREE(pNetifData);
        pNetifData = NULL;
        
        return(false);
    }
    memcpy(pNetif->hwaddr, pMmeDev->u8MacEvseHsot, sizeof(pMmeDev->u8MacEvseHsot));
    netif_create_ip6_linklocal_address(pNetif, 1);
    netif_ip6_addr_set_state(pNetif, 0, IP6_ADDR_VALID);
    netif_set_up(pNetif);
    
    pMmeDev->pNetif = pNetif;
    
    return(true);
}












/**********************************************************************************************
* Description                           :   MME 协议 接收和发送数据打印函数
* Author                                :   Hall
* modified Date                         :   2023-10-18
* notice                                :   
***********************************************************************************************/
void sMmePacketPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len)
{
    char u8HeadStr[48] = { 0 };
    char u8SendRecvStr[3][8] = { "RX", "TX"};
    
    return;
    
    memset(u8HeadStr, 0, sizeof(u8HeadStr));
    snprintf(u8HeadStr, sizeof(u8HeadStr), "PLC %s(以太网帧 len:%04d)%s", u8SendRecvStr[bSendFlag], u16Len, bSendFlag ? "--->" : "<---");
    EN_HLOGD(TAG, u8HeadStr, pBuf, u16Len);
}







/**********************************************************************************************
* Description                           :   MME homeplug网络参数初始化
* Author                                :   Hall
* modified Date                         :   2024-03-11
* notice                                :   
*                                           只要对其中一个pev节点执行网络参数初始化，所有节点都能起作用，
*                                           因为是pev节点挂在同一个plc载波芯片上
*                                           
*                                           所以这里只取链表中的一个节点设置事件标志就OK了
***********************************************************************************************/
bool sMmeSetNetInit(i32 i32MmeDevIndex)
{
    pev_item *pPev = NULL;
    
    pPev = search_pev_item_oldest(pMmeCache[i32MmeDevIndex]->stEvse.pev_item_head);
    if(pPev != NULL)
    {
        xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacNetInit);
        pPev->bSlacMatchFlag = false;
        return(true);
    }
    
    return(false);
}






/**********************************************************************************************
* Description                           :   MME 从外部输入CP状态
* Author                                :   Hall
* modified Date                         :   2024-01-30
* notice                                :   
***********************************************************************************************/
void sMmeSetCpState(eChgCpState_t eCpState)
{
    i32  i;
    eChgCpState_t eCpStateOld;
    
    for(i = 0; i < cMse102xDevNum; i++)
    {
        if(pMmeCache[i] != NULL)
        {
            eCpStateOld = pMmeCache[i]->eCpState;
            if(((eCpStateOld == eChgCpStateB) || (eCpStateOld == eChgCpStateC) || (eCpStateOld == eChgCpStateD))
            && ((eCpState    == eChgCpStateA) || (eCpState    == eChgCpStateE) || (eCpState    == eChgCpStateF) || (eCpState == eChgCpStateMax)))
            {
                //CP从状态B/C/D向其他状态切换 均认为是CP断开
                sMmeSlacSetEvtCpDisconn(pMmeCache[i]->stEvse.pev_item_head);
            }
            pMmeCache[i]->eCpState = eCpState;
        }
    }
}








/*******************************************************************************
 * @FunctionName   :      sMmeGetSlacMatchFlag
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月3日  13:37:21
 * @Description    :      MME 获取指定 i32MmeDevIndex 的Pev节点 的SLAC连接状态
 * @Input          :      i32MmeDevIndex
 * @Return         :      
*******************************************************************************/
bool sMmeGetSlacMatchFlag(i32 i32MmeDevIndex)
{
    pev_item *pPev = NULL;
    
    pPev = pMmeCache[i32MmeDevIndex]->stEvse.pev_item_head;
    
    while(pPev != NULL)
    {
        if(pPev->bSlacMatchFlag == true)
        {
            return(true);
        }
        
        pPev = pPev->next;
    }
    
    return(false);
}








/**********************************************************************************************
* Description                           :   MME 获取指定 i32MmeDevIndex 的Pev节点头
* Author                                :   Hall
* modified Date                         :   2024-01-16
* notice                                :   
***********************************************************************************************/
struct pev_item *sMmeGetPevHead(i32 i32MmeDevIndex)
{
    if(i32MmeDevIndex < cMse102xDevNum)
    {
        return pMmeCache[i32MmeDevIndex]->stEvse.pev_item_head;
    }
    
    return(NULL);
}









/***************************************************************************************************
* Description                           :   MME 协议 接收任务
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
void sMmeRecvTask(void *pParam)
{
    stMmeDev_t   *pMmeDev = NULL;
    stMmeCache_t *pCache  = NULL;
    
    i32 i32RecvLen = 0;
    MME_t *pEthPkt = NULL;                                                      //指向接收buf的以太网帧
    
    
    pMmeDev = (stMmeDev_t *)pParam;
    EN_SLOGI(TAG, "SECC[%d], MME主任务开始运行!!!", pMmeDev->i32Index);
    
    if(pMmeDev->i32Index < cMse102xDevNum)
    {
        pCache  = pMmeCache[pMmeDev->i32Index];
    }
    
    while(pMmeDev->i32Index < cMse102xDevNum)
    {
        i32RecvLen = sMse102xReadHpav(pMmeDev->i32Index, sizeof(pCache->u8RxBuf), pCache->u8RxBuf);
        if(i32RecvLen > 0)
        {
            sMmePacketPrint(false, pCache->u8RxBuf, i32RecvLen);
            
            //检查数据是Mac目的地址是否匹配本机
            pEthPkt = (MME_t *)pCache->u8RxBuf;
            if(sMacAddrCompare(pEthPkt->mme_dest, NULL, pMmeDev->u8MacEvseHsot) == true)
            {
                sMmePacketRecv(pMmeDev, pEthPkt, i32RecvLen);
            }
        }
        
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    
    
    EN_SLOGI(TAG, "SECC[%d], MME主任务退出运行!!!", pMmeDev->i32Index);
    vTaskDelete(NULL);
}









/**********************************************************************************************
* Description                           :   MME 协议 消息类型 搜索
* Author                                :   Hall
* modified Date                         :   2023-07-05
* notice                                :   此函数用于通过指定的 eType 去匹配到对应的序号 以便引用 stMmeCmdTypeMap[] 成员
***********************************************************************************************/
i32 sMmeCmdSearch(mmtype_t eType)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stMmeCmdTypeMap); i++)
    {
        if(eType == stMmeCmdTypeMap[i].eType)
        {
            if(stMmeCmdTypeMap[i].pRecv != NULL)
            {
                return(i);
            }
        }
    }
    
    return(-1);
}









/***************************************************************************************************
* Description                           :   MME 帧发送函数
* Author                                :   Hall
* Creat Date                            :   2023-07-05
* notice                                :   eType       :调用者在调用本函数发送MME帧时 应填写该MME帧对应的接收帧类型
*                                           bPlcNodeFlag:该MME帧发送目标是否为 evse plc node
****************************************************************************************************/
bool sMmePacketSend(mmtype_t eType, bool bPlcNodeFlag, struct pev_item *pPev, mme_ctx_t *pCtx)
{
    bool bRst;
    i32  i32Index;
    mme_error_t eError;
    
    //0:搜索索引号
    bRst = false;
    i32Index = sMmeCmdSearch(eType);
    if(i32Index < 0)
    {
        return(bRst);
    }
    
    //1:对于 u32WaitTime 时间不为零的消息类型
    //需要重置相关信号量 以便发送后去等待信号量
    if(stMmeCmdTypeMap[i32Index].i32WaitTime > 0)
    {
        stMmeCmdTypeMap[i32Index].bProcRst = false;
        xSemaphoreTake(stMmeCmdTypeMap[i32Index].xSemAck, 0);
    }
    
    //3:发送
    eError = mme_send(bPlcNodeFlag, pPev, pCtx);
    
    
    //4:等待信号量
    if(stMmeCmdTypeMap[i32Index].i32WaitTime > 0)
    {
        bRst = xSemaphoreTake(stMmeCmdTypeMap[i32Index].xSemAck, stMmeCmdTypeMap[i32Index].i32WaitTime / portTICK_PERIOD_MS);
        if(bRst == true)
        {
            bRst = stMmeCmdTypeMap[i32Index].bProcRst;
        }
    }
    else
    {
        //u32WaitTime 为0 表示报文不重要 无需等待反馈
        //则直接使用发送结果 作为结果返回
        bRst = (eError == MME_SUCCESS) ? true : false;
    }
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   MME 帧处理函数
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePacketRecv(stMmeDev_t *pMmeDev, MME_t *pEthPkt, i32 i32EthPktLen)
{
    u16  i;
    bool bRst;
    
    bRst = false;
    for(i = 0; i < sArraySize(stMmeCmdTypeMap); i++)
    {
        if(pEthPkt->mmtype == stMmeCmdTypeMap[i].eType)
        {
            if(stMmeCmdTypeMap[i].pRecv != NULL)
            {
                bRst = stMmeCmdTypeMap[i].pRecv(i, pMmeDev, pEthPkt, i32EthPktLen);
                break;
            }
        }
    }
    
    return(bRst);
}


















