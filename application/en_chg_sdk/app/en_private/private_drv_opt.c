/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_opt.c
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-01
* notice                                :     
****************************************************************************************************/
#include "private_drv_opt.h"







//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_opt";



stPrivDrvCache_t *pPrivDrvCache = NULL;


//初始化
bool sPrivDrvInit(ePrivDrvHwType_t eType);
bool sPrivDrvInitVar(void);




//map操作
i32  sPrivDrvCmdSearch30(ePrivDrvCmd_t eCmd);
i32  sPrivDrvCmdSearch(ePrivDrvCmd_t eCmd);

//发送处理相关函数
void sPrivDrvSetHead30(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len);
void sPrivDrvSetHead(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len);
void sPrivDrvSetCrc(u8 *pBuf, u16 u16Len);
bool sPrivDrvSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag);



//接收处理相关函数
void sPrivDrvPktCheck(const u8 *pBuf, i32 i32Len);
void sPrivDrvPktCheckTimeout(void);
void sPrivDrvPktRecv(const u8 *pBuf, i32 i32Len);
void sPrivDrvPktRecvRstSet(ePrivDrvCmd_t eCmd, bool bRst);
void sPrivDrvPktRecvReset(void);










stPrivDrvOptMap_t stPrivDrvOptMap[] =
{
    {
        ePrivDrvHwTypeCom,
        "串口通讯",
        sPrivComInit,
        sPrivComSend,
        
    },
    
    {
        ePrivDrvHwTypeCan,
        "CAN通讯",
        NULL,
        NULL,
        
    }
};
stPrivDrvOptMap_t *pPrivDrvOpt = &stPrivDrvOptMap[0];






//报文接收处理函数map
stPrivDrvCmdMap_t stPrivDrvCmdMap30[] = 
{
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    {ePrivDrvCmd02, sPrivDrvPktRecv0230,NULL,   false,                              0, "桩签到"              },
    {ePrivDrvCmd0B, sPrivDrvPktRecv0B30,NULL,   false,                              0, "远程升级应答"        },
#endif
};






//报文接收处理函数map
stPrivDrvCmdMap_t stPrivDrvCmdMap[] = 
{
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    {ePrivDrvCmd01, NULL,               NULL,   false,                              0, "地址申请"           },
    {ePrivDrvCmd02, sPrivDrvPktRecv02,  NULL,   false,                              0, "桩签到"             },
    {ePrivDrvCmd03, sPrivDrvPktRecv03,  NULL,   false,                              0, "状态上报"           },
    {ePrivDrvCmd04, sPrivDrvPktRecv04,  NULL,   false,                              0, "桩心跳"             },
    {ePrivDrvCmd05, sPrivDrvPktRecv05,  NULL,   false,                              0, "卡认证"             },
    {ePrivDrvCmd06, sPrivDrvPktRecv06,  NULL,   false,                           5000, "远程操作应答"       },
    {ePrivDrvCmd07, sPrivDrvPktRecv07,  NULL,   false,                              0, "开始/结束充电"      },
    {ePrivDrvCmd08, sPrivDrvPktRecv08,  NULL,   false,                              0, "充电实时数据上报"   },
    {ePrivDrvCmd09, sPrivDrvPktRecv09,  NULL,   false,                              0, "结算报文上报"       },
    {ePrivDrvCmd0A, sPrivDrvPktRecv0A,  NULL,   false,                           3000, "桩参数管理应答"     },//有些指令先回去其他的才回复0A,因此该时间
    {ePrivDrvCmd0B, sPrivDrvPktRecv0B,  NULL,   false,                              0, "远程升级应答"       },
    {ePrivDrvCmd0C, sPrivDrvPktRecv0C,  NULL,   false,                              0, "模块升级应答"       },
    {ePrivDrvCmd0E, sPrivDrvPktRecv0E,  NULL,   false,                              0, "遥信数据上报"       },
    {ePrivDrvCmd11, sPrivDrvPktRecv11,  NULL,   false,                              0, "安全事件通知"       },
    {ePrivDrvCmd13, sPrivDrvPktRecv13,  NULL,   false,                              0, "BMS信息上报"        },
    {ePrivDrvCmd14, sPrivDrvPktRecv14,  NULL,   false,                           5000, "预约/定时充电应答"  },
    {ePrivDrvCmd15, sPrivDrvPktRecv15,  NULL,   false,                              0, "主柜信息上报"       },
    {ePrivDrvCmd16, sPrivDrvPktRecv16,  NULL,   false,                              0, "液冷机信息上报"     },
    
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    {ePrivDrvCmdA1, NULL,               NULL,   false,                              0, "地址申请应答"       },
    {ePrivDrvCmdA2, sPrivDrvPktRecvA2,  NULL,   false,                            100, "桩签到应答"         },
    {ePrivDrvCmdA3, sPrivDrvPktRecvA3,  NULL,   false,                            100, "状态上报应答"       },
    {ePrivDrvCmdA4, sPrivDrvPktRecvA4,  NULL,   false,                            100, "桩心跳应答"         },
    {ePrivDrvCmdA5, sPrivDrvPktRecvA5,  NULL,   false,     cPrivDrvCardAuthAckTimeout, "卡认证应答"         },
    {ePrivDrvCmdA6, sPrivDrvPktRecvA6,  NULL,   false,                              0, "远程操作"           },
    {ePrivDrvCmdA7, sPrivDrvPktRecvA7,  NULL,   false,                           5000, "开始/结束充电应答"  },
    {ePrivDrvCmdA8, sPrivDrvPktRecvA8,  NULL,   false,                            100, "充电实时数据应答"   },
    {ePrivDrvCmdA9, sPrivDrvPktRecvA9,  NULL,   false,                            100, "结算报文应答"       },
    {ePrivDrvCmdAA, sPrivDrvPktRecvAA,  NULL,   false,                           5000, "桩参数管理"         },
    {ePrivDrvCmdAB, sPrivDrvPktRecvAB,  NULL,   false,                              0, "远程升级命令"       },
    {ePrivDrvCmdAE, sPrivDrvPktRecvAE,  NULL,   false,                            100, "遥信数据上报应答"   },
    {ePrivDrvCmdB1, sPrivDrvPktRecvB1,  NULL,   false,                            100, "安全事件通知应答"   },
    {ePrivDrvCmdB3, sPrivDrvPktRecvB3,  NULL,   false,                            100, "BMS信息上报应答"    },
    {ePrivDrvCmdB4, sPrivDrvPktRecvB4,  NULL,   false,                              0, "预约/定时充电"      },
    {ePrivDrvCmdB5, sPrivDrvPktRecvB5,  NULL,   false,                              0, "主柜信息上报应答"   },
    {ePrivDrvCmdB6, sPrivDrvPktRecvB6,  NULL,   false,                              0, "液冷机信息上报应答" },
    
#endif
};











/**********************************************************************************************
* Description       :     EN+ 内网私有协议驱动 初始化函数
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
bool sPrivDrvInit(ePrivDrvHwType_t eType)
{
    i32 i;
    bool bRst;
    
    
    //0:注册内网协议在shell上的命令
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    sPrivDrvInitLogLevel();
#endif
    sPrivDrvShellRegister();
    
    //1:缓存资源初始化
    if(sPrivDrvInitVar() == false)
    {
        return(false);
    }
    
    //2:map初始化
    for(i = 0; i < sArraySize(stPrivDrvCmdMap); i++)
    {
        if(stPrivDrvCmdMap[i].u32WaitTime != 0)
        {
            //等待时间不为0的信号量才创建
            stPrivDrvCmdMap[i].hAckMutex = xSemaphoreCreateBinary();
            xSemaphoreGive(stPrivDrvCmdMap[i].hAckMutex);
        }
    }
    
    //3:串口硬件初始化
    bRst = false;
    if(eType < ePrivDrvHwTypeMax)
    {
        pPrivDrvOpt = &stPrivDrvOptMap[eType];
    }
    if(pPrivDrvOpt->pInit != NULL)
    {
        bRst = pPrivDrvOpt->pInit();
    }
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    if(bRst)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        sPrivDrvPktLogin();
    }
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    xTaskCreate(sPrivDrvSendTask, "sPrivDrvSendTask", cPrivDrvSendTaskStackSize, NULL, cPrivDrvSendTaskPrio, NULL);
#endif
    
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     EN+ 内网私有协议驱动 缓存资源 初始化
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
bool sPrivDrvInitVar(void)
{
    EN_SLOGE(TAG, "EN+ 内网协议-最大枪数%d,缓存大小:%d,%s接收", cPrivDrvGunNumMax, sizeof(stPrivDrvCache_t), (cPrivDrvRxScanEn) ? "扫描" : "事件");
    pPrivDrvCache = (stPrivDrvCache_t *)MALLOC(sizeof(stPrivDrvCache_t));
    if(pPrivDrvCache == NULL)
    {
        EN_SLOGE(TAG, "EN+ 内网协议:数据初始化失败, 内存分配出错!!!");
        return(false);
    }
    
    memset(pPrivDrvCache, 0, sizeof(stPrivDrvCache_t));
    pPrivDrvCache->hUartQueue           = NULL;
    pPrivDrvCache->hAckTxBufMutex       = xSemaphoreCreateBinary();
    pPrivDrvCache->hRptTxBufMutex       = xSemaphoreCreateBinary();
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    pPrivDrvCache->i32PktIndex          = 0;
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    sPrivDrvVarInitM();
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    sPrivDrvVarInitS();
#endif
    
    
    return(true);
}








/**********************************************************************************************
* Description       :     报文类型 搜索
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     此函数用于通过指定的 eCmd 去匹配到对应的序号 以便引用 stPrivDrvCmdMap[] 成员
***********************************************************************************************/
i32 sPrivDrvCmdSearch30(ePrivDrvCmd_t eCmd)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stPrivDrvCmdMap30); i++)
    {
        if(eCmd == stPrivDrvCmdMap30[i].eCmd)
        {
            if(stPrivDrvCmdMap30[i].pRecv != NULL)
            {
                return(i);
            }
        }
    }
    
    return(-1);
}








/**********************************************************************************************
* Description       :     报文类型 搜索
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     此函数用于通过指定的 eCmd 去匹配到对应的序号 以便引用 stPrivDrvCmdMap[] 成员
***********************************************************************************************/
i32 sPrivDrvCmdSearch(ePrivDrvCmd_t eCmd)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stPrivDrvCmdMap); i++)
    {
        if(eCmd == stPrivDrvCmdMap[i].eCmd)
        {
            if(stPrivDrvCmdMap[i].pRecv != NULL)
            {
                return(i);
            }
        }
    }
    
    return(-1);
}








/**********************************************************************************************
* Description       :     报文类型 获取MAP
* Author            :     XRG
* modified Date     :     2024-12-19
* notice            :     此函获取MAP的所有参数
***********************************************************************************************/
stPrivDrvCmdMap_t *sPrivDrvCmdGetMap(ePrivDrvCmd_t eCmd)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stPrivDrvCmdMap); i++)
    {
        if(eCmd == stPrivDrvCmdMap[i].eCmd)
        {
            return(&stPrivDrvCmdMap[i]);
        }
    }
    
    return(NULL);
}








/**********************************************************************************************
* Description       :     EN+ 内网私有协议 发送报文的head填充
* Author            :     XRG
* modified Date     :     2024-01-24
* notice            :     针对内容3.0协议填充
***********************************************************************************************/
void sPrivDrvSetHead30(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len)
{
    pHead->eHeader                      = ePrivDrvHeader;
    pHead->eVersion                     = ePrivDrvVersion30;
    pHead->u16Addr                      = u16Addr;
    pHead->eCmd                         = eCmd;
    pHead->u8Seqno                      = u8Seq;
    pHead->u16Len                       = u16Len;
}








/**********************************************************************************************
* Description       :     EN+ 内网私有协议 发送报文的head填充
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     
***********************************************************************************************/
void sPrivDrvSetHead(stPrivDrvHead_t *pHead, ePrivDrvCmd_t eCmd, u16 u16Addr, u8 u8Seq, u16 u16Len)
{
    pHead->eHeader                      = ePrivDrvHeader;
    pHead->eVersion                     = ePrivDrvVersion;
    pHead->u16Addr                      = u16Addr;
    pHead->eCmd                         = eCmd;
    pHead->u8Seqno                      = u8Seq;
    pHead->u16Len                       = u16Len;
}








/**********************************************************************************************
* Description       :     EN+ 内网私有协议 CheckCrc计算和填充
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     
***********************************************************************************************/
void sPrivDrvSetCrc(u8 *pBuf, u16 u16Len)
{
    u16 u16CheckCrc;
    
    u16CheckCrc       = sCrc16WithA001((const u8 *)pBuf, u16Len);
    pBuf[u16Len + 0]  = ((u16CheckCrc >> 0) & 0xff);
    pBuf[u16Len + 1]  = ((u16CheckCrc >> 8) & 0xff);
}








/**********************************************************************************************
* Description       :     EN+ 内网私有协议 数据发送函数
* Author            :     Hall
* modified Date     :     2023-11-01
* notice            :     
***********************************************************************************************/
bool sPrivDrvSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag)
{
    bool bRst = false;
    
    if(pPrivDrvOpt->pSend != NULL)
    {
        bRst = pPrivDrvOpt->pSend(eCmd, pData, u16Len, bRptFlag);
    }
    
    return(bRst);
}






/**********************************************************************************************
* Description       :     串口数据 断帧函数
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     负责对收到的数据做帧检查, 提取其中符合内网协议的帧 并处理
***********************************************************************************************/
void sPrivDrvPktCheck(const u8 *pBuf, i32 i32Len)
{
    i32  i;
    stPrivDrvHead_t *pHead = NULL;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pPrivDrvCache->i32PktIndex)
        {
        case 0:
            if(pBuf[i] == ePrivDrvHeader)
            {
                pPrivDrvCache->u8Pkt[pPrivDrvCache->i32PktIndex] = pBuf[i];
                pPrivDrvCache->i32PktIndex++;
                pPrivDrvCache->u32PktHeadTime = sGetTimestamp();                //收到head时记录时间戳
            }
            break;
        case 1:
            if((pBuf[i] == ePrivDrvVersion) || (pBuf[i] == ePrivDrvVersion30))
            {
                pPrivDrvCache->u8Pkt[pPrivDrvCache->i32PktIndex] = pBuf[i];
                pPrivDrvCache->i32PktIndex++;
            }
            else
            {
                EN_SLOGE(TAG, "协议版本异常:%d,%02X", pPrivDrvCache->i32PktIndex, pBuf[i]);
                memset(pPrivDrvCache->u8Pkt, 0, sizeof(pPrivDrvCache->u8Pkt));
                pPrivDrvCache->i32PktIndex = 0;
            }
            break;
        default:
            if(pPrivDrvCache->i32PktIndex <sizeof(pPrivDrvCache->u8Pkt))
            {
                pPrivDrvCache->u8Pkt[pPrivDrvCache->i32PktIndex] = pBuf[i];
                pPrivDrvCache->i32PktIndex++;
                
                if(pPrivDrvCache->i32PktIndex >= cPrivDrvHeadSize)
                {
                    pHead = (stPrivDrvHead_t *)pPrivDrvCache->u8Pkt;
                    
                    if(pPrivDrvCache->i32PktIndex >= (cPrivDrvHeadSize + pHead->u16Len + 2))//2字节的校验值
                    {
                        //收到完整帧的时候 也要更新本时间戳 避免帧被清除
                        pPrivDrvCache->u32PktHeadTime = sGetTimestamp();
                        if(sPrivDrvCheckCrc(pPrivDrvCache->u8Pkt, pPrivDrvCache->i32PktIndex) == true)
                        {
                            sPrivDrvPktRecv(pPrivDrvCache->u8Pkt, pPrivDrvCache->i32PktIndex);
                        }
                        
                        memset(pPrivDrvCache->u8Pkt, 0, sizeof(pPrivDrvCache->u8Pkt));
                        pPrivDrvCache->i32PktIndex = 0;
                        
                    }
                }
            }
            else
            {
                EN_SLOGE(TAG, "接收缓冲区超出:%d", pPrivDrvCache->i32PktIndex);
                memset(pPrivDrvCache->u8Pkt, 0, sizeof(pPrivDrvCache->u8Pkt));
                pPrivDrvCache->i32PktIndex = 0;
            }
            break;
        }
    }
}








/**********************************************************************************************
* Description       :     串口数据 帧超时检测函数
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     对于接收到head 但还没接收完整的帧, 检查其是否超时
***********************************************************************************************/
void sPrivDrvPktCheckTimeout(void)
{
#if (cPrivDrvPktTimeOutEn == 1)
    u32  u32Time;
    
    if(pPrivDrvCache->i32PktIndex > 0)
    {
        u32Time = sGetTimestamp();
        if((u32Time - pPrivDrvCache->u32PktHeadTime) > cPrivDrvPktTimeOutSec)
        {
            EN_SLOGE(TAG, "帧接收超时:帧长:%d, 时间戳:%u, %u, ", pPrivDrvCache->i32PktIndex, u32Time, pPrivDrvCache->u32PktHeadTime);
            EN_HLOGI(TAG, "帧接收内容:", pPrivDrvCache->u8Pkt, pPrivDrvCache->i32PktIndex);
            memset(pPrivDrvCache->u8Pkt, 0, sizeof(pPrivDrvCache->u8Pkt));
            pPrivDrvCache->i32PktIndex = 0;
        }
    }
#endif
}








/**********************************************************************************************
* Description       :     串口数据 帧处理函数
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     
***********************************************************************************************/
void sPrivDrvPktRecv(const u8 *pBuf, i32 i32Len)
{
    i32                i;
    stPrivDrvHead_t   *pHead = (stPrivDrvHead_t *)pBuf;
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    pPrivDrvCache->unData.stMaster.stChg.eVersion = pHead->eVersion;
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    if(pHead->u16Addr != pPrivDrvCache->unData.stSlave.stChg.u16Addr)
    {
        EN_SLOGE(TAG, "EN+ 内网协议:帧地址不匹配, 本机地址:%d, 报文目的地址:%d", pPrivDrvCache->unData.stSlave.stChg.u16Addr, pHead->u16Addr);
        return;
    }
#endif
    
    
    //2.帧处理
    if(pHead->eVersion == ePrivDrvVersion30)
    {
        //内网3.0协议版本帧处理
        i = sPrivDrvCmdSearch30(pHead->eCmd);
        if(i >= 0)
        {
            stPrivDrvCmdMap30[i].pRecv(pBuf, i32Len);
        }
        return;
    }
    
    //内网3.6协议版本帧处理
    i = sPrivDrvCmdSearch(pHead->eCmd);
    if(i >= 0)
    {
        stPrivDrvCmdMap[i].pRecv(pBuf, i32Len);
    }
    
    //接收到任何数据之后,应该先让桩能签到,确保数据同步
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    sPrivDrvPktLogin();
#endif
}





/**********************************************************************************************
* Description       :     接收帧处理结果填充
* Author            :     Hall
* modified Date     :     2023-11-02
* notice            :     
***********************************************************************************************/
void sPrivDrvPktRecvRstSet(ePrivDrvCmd_t eCmd, bool bRst)
{
    i32 i;
    
    i = sPrivDrvCmdSearch(eCmd);
    if(i >= 0)
    {
        stPrivDrvCmdMap[i].bProcRst = bRst;
        if(stPrivDrvCmdMap[i].hAckMutex != NULL)
        {
            xSemaphoreGive(stPrivDrvCmdMap[i].hAckMutex);
        }
    }
    else
    {
        EN_SLOGE(TAG, "报文类型搜索id异常 :%d", i);
    }
}











/**********************************************************************************************
* Description       :     接收帧重置
* Author            :     XRG
* modified Date     :     2024-02-22
* notice            :     用于异常情况清除所有缓冲区
***********************************************************************************************/
void sPrivDrvPktRecvReset(void)
{
#if(cPrivDrvRecvErrResetEn == 1)
    memset(pPrivDrvCache->u8RxBuf,    0, sizeof(pPrivDrvCache->u8RxBuf));
    memset(pPrivDrvCache->u8AckTxBuf, 0, sizeof(pPrivDrvCache->u8AckTxBuf));
    memset(pPrivDrvCache->u8RptTxBuf, 0, sizeof(pPrivDrvCache->u8RptTxBuf));
    memset(pPrivDrvCache->u8Pkt,      0, sizeof(pPrivDrvCache->u8Pkt));
    
    xSemaphoreGive(pPrivDrvCache->hAckTxBufMutex);
    xSemaphoreGive(pPrivDrvCache->hRptTxBufMutex);
    
    pPrivDrvCache->i32PktIndex          = 0;
#endif
}











































