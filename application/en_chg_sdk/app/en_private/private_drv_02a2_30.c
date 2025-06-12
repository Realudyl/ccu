/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_02a2_30.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     XRG
* Creat Date                            :     2024-01-24
* notice                                :     主要兼容桩BOOT内网3.0帧格式 02/A2 报文 
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_02a2_30.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_02a2_30";






extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap30[];














#if (cSdkPrivDevType == cSdkPrivDevTypeM)





/**********************************************************************************************
* Description       :     串口数据 帧接收处理函数 之 02报文
* Author            :     XRG
* modified Date     :     2024-01-24
* notice            :     供 Master 端使用
***********************************************************************************************/
bool sPrivDrvPktRecv0230(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvCmd0230_t  *pCmd02 = NULL;
    stPrivDrvDataMaster_t   *pData  = NULL;
    
    pPkt    = (unPrivDrvPkt_t *)pBuf;
    pCmd02  = &pPkt->stPkt.unPayload.stCmd0230;
    pData   = &pPrivDrvCache->unData.stMaster;
    
    
    //1:SN号校验
    bRst = false;
    if(!sSnIsValid((const u8 *)pCmd02->u8Sn, sizeof(pCmd02->u8Sn)))
    {
        EN_SLOGE(TAG, "桩端签到:SN号格式错误!!!");
    }
    else
    {
        bRst = true;
    }
    
    
    //2:数据解析及缓存
    if(bRst == true)
    {
        //桩boot上报cmd02仅需要这几个参数
        memset(pData->stChg.stCache02.u8Sn, 0, sizeof(pData->stChg.stCache02.u8Sn));
        memcpy(pData->stChg.stCache02.u8Sn, pCmd02->u8Sn, sizeof(pCmd02->u8Sn));
        memset(pData->stChg.stCache02.u8DevName, 0, sizeof(pData->stChg.stCache02.u8DevName));
        memcpy(pData->stChg.stCache02.u8DevName, pCmd02->u8DevName, sizeof(pCmd02->u8DevName));
        memset(pData->stChg.stCache02.u8DevCode, 0, sizeof(pData->stChg.stCache02.u8DevCode));
        snprintf((char *)pData->stChg.stCache02.u8DevCode, sizeof(pData->stChg.stCache02.u8DevCode), "%d", 0xff);//新平台默认0xff//pCmd02->u8DevCode);
        memset(pData->stChg.stCache02.u8SwVer, 0, sizeof(pData->stChg.stCache02.u8SwVer));
        if((pCmd02->u8SwVer[0]==0) || (pCmd02->u8SwVer[0]==0xFF))
        {
            memcpy(pData->stChg.stCache02.u8SwVer, cChgSwVerBootloader, strlen(cChgSwVerBootloader));
        }
        else
        {
            memcpy(pData->stChg.stCache02.u8SwVer, pCmd02->u8SwVer, sizeof(pCmd02->u8SwVer));
        }
        
        pData->stChg.bCache02Flag = true;
        
    }
    
    
    
    return(true);
}










#endif




























