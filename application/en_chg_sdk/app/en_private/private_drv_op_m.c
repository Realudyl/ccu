/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     private_drv_op_m.c
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-05-27
* notice                                :     
****************************************************************************************************/
#include "private_drv_opt.h"




#if   (cSdkPrivDevType == cSdkPrivDevTypeM)                                     //Master 侧才有这些操作




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_op_m";



extern stPrivDrvCache_t *pPrivDrvCache;





void sPrivDrvInitLogLevel(void);                                                //esp32  日志等级设定
void sPrivDrvPktLogin(void);





/**********************************************************************************************
* Description       :     EN+ 内网私有协议驱动 日志等级 初始化
* Author            :     Hall
* modified Date     :     2023-11-11
* notice            :     
***********************************************************************************************/
void sPrivDrvInitLogLevel(void)
{
    esp_log_level_set("private_drv_02a2",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_02a2_30",    cPrivDrvLogLevel);
    esp_log_level_set("private_drv_03a3",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_04a4",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_05a5",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_06a6",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_07a7",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_08a8",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_09a9",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0aaa",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0bab",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0bab_30",    cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0bab_update",cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0cac",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0cac_update",cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0dad",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_0eae",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_11b1",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_13b3",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_14b4",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_15b5",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_16b6",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_api_m",      cPrivDrvLogLevel);
    esp_log_level_set("private_drv_api_s",      cPrivDrvLogLevel);
    esp_log_level_set("private_drv_basic",      cPrivDrvLogLevel);
    esp_log_level_set("private_drv_can",        cPrivDrvLogLevel);
    esp_log_level_set("private_drv_com",        cPrivDrvLogLevel);
    esp_log_level_set("private_drv_opt",        cPrivDrvLogLevel);
    esp_log_level_set("private_drv_op_m",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_op_s",       cPrivDrvLogLevel);
    esp_log_level_set("private_drv_shell",      cPrivDrvLogLevel);
    
}





/**********************************************************************************************
* Description       :     串口数据 签到
* Author            :     XRG
* modified Date     :     2024-03-18
* notice            :     1.上电或网关复位先发送签到到桩,快速同步信息
                          2.接收到任何数据之后,应该先让桩能签到,确保数据同步
***********************************************************************************************/
void sPrivDrvPktLogin(void)
{
    const u8   u8Temp = ePrivDrvParamGateOpReLogin;
    static u32 u32Time = 0;
    u32        u32WaitTime;
    stPrivDrvCmdMap_t *pMap = NULL;
    
    if((pPrivDrvCache->unData.stMaster.stChg.bCache02Flag == false)
    && ((u32Time == 0) || ((sGetTimestamp() > u32Time) && ((sGetTimestamp() - u32Time) > 10))))
    {
        u32Time = sGetTimestamp();
        EN_SLOGI(TAG, "请求slave端重新签到");
        
        pMap = sPrivDrvCmdGetMap(ePrivDrvCmd0A);
        if(pMap != NULL)
        {
            u32WaitTime       = pMap->u32WaitTime;
            pMap->u32WaitTime = 0;
            sPrivDrvSetCmdParam(true, cPrivDrvGunIdChg, ePrivDrvCfgCmdTypeSet, ePrivDrvCmdRstMax, ePrivDrvParamAddr018, 1, &u8Temp, 0);
            pMap->u32WaitTime = u32WaitTime;
        }
    }
}






#endif





