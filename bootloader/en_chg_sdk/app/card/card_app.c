/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_app.c
* Description                           :   刷卡数据管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :   
****************************************************************************************************/
#include "card_app.h"








//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "card_app";



stCardAppCache_t stCardAppCache;






bool sCardAppInit(void);
void sCardAppSendTask(void *pParam);

void sCardAppStop(void);
void sCardAppStartForStop(void);
void sCardAppStartForStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param);


stCardData_t *sCardAppGetData(void);
void sCardAppSetDataClear(void);

void sCardReaderBreaklineCheck(void);







/**********************************************************************************************
* Description                           :   刷卡数据管理--->初始化
* Author                                :   Hall
* modified Date                         :   2024-06-04
* notice                                :   
***********************************************************************************************/
bool sCardAppInit(void)
{
    bool bRst;
    
    bRst = sCardDrvMt626Init();
    memset(&stCardAppCache, 0, sizeof(stCardAppCache));
    
    //建立发送线程
    xTaskCreate(sCardAppSendTask, "sCardAppSendTask", (512), NULL, 16, NULL);
    
    return bRst;
}







/***************************************************************************************************
* Description                           :   刷卡数据管理 读数据任务
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :     
****************************************************************************************************/
void sCardAppSendTask(void *pParam)
{
    EN_SLOGD(TAG, "任务建立:刷卡数据管理 读数据任务");
    while(1)
    {
        //有错误就会一直寻卡直至回复
        if((stCardAppCache.bReadFlag == true) || (stCardAppCache.bCardReaderBreakline == true))
        {
            if(sCardAppEnRead(&stCardAppCache.stData) == true)
            {
                //读取到完整卡数据以后就停止读取
                sCardAppStop();
                stCardAppCache.bReadedFlag = true;
                
                //每次寻卡成功以后 延时一段时间 避免用户没有及时拿走卡 导致重新寻卡
                vTaskDelay(10000 / portTICK_RATE_MS);
            }
        }
        else
        {
            //不寻卡的话时间戳更新
            stCardAppCache.u32ReadTimeStamp = sGetTimestamp();
        }
        
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}





/***************************************************************************************************
* Description                           :   刷卡数据管理 停止读数据
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :     
****************************************************************************************************/
void sCardAppStop(void)
{
    stCardAppCache.bReadFlag = false;
}






/***************************************************************************************************
* Description                           :   启动寻卡
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :   为  停止充电  启动寻卡
****************************************************************************************************/
void sCardAppStartForStop(void)
{
    stCardAppCache.bReadFlag = true;
}






/***************************************************************************************************
* Description                           :   启动寻卡
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :   为  启动充电  启动寻卡
****************************************************************************************************/
void sCardAppStartForStart(ePileGunIndex_t eGunIndex, ePrivDrvChargingMode_t eMode, u32 u32Param)
{
    stCardAppCache.bReadFlag = true;
    stCardAppCache.bReadedFlag = false;
    stCardAppCache.stData.eGunIndex = eGunIndex;
    stCardAppCache.stData.eStartType = ePrivDrvStartTransTypeCard;
    stCardAppCache.stData.eMode = eMode;
    stCardAppCache.stData.unParam.u32Value = u32Param;
}







/***************************************************************************************************
* Description                           :   刷卡数据管理 获取卡数据
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :     
****************************************************************************************************/
stCardData_t *sCardAppGetData(void)
{
    return (stCardAppCache.bReadedFlag == false) ? NULL : &stCardAppCache.stData;
}





/***************************************************************************************************
* Description                           :   刷卡数据管理 清除卡数据
* Author                                :   Hall
* Creat Date                            :   2024-06-10
* notice                                :     
****************************************************************************************************/
void sCardAppSetDataClear(void)
{
    stCardAppCache.bReadedFlag = false;
}






/***************************************************************************************************
* Description                           :   刷卡数据管理读卡器故障判断
* Author                                :   Dai
* Creat Date                            :   2024-06-18
* notice                                :     
****************************************************************************************************/
void sCardReaderBreaklineCheck(void)
{
    //寻卡超过30秒无回复认为断线
    if((stCardAppCache.bReadFlag == true) && (sGetTimestamp() - stCardAppCache.u32ReadTimeStamp >= cCardReaderErrTimeCheck) && (sGetTimestamp() >= stCardAppCache.u32ReadTimeStamp) && (sCardDrvMt626GetOnLineState() == false))
    {
        stCardAppCache.bCardReaderBreakline = true;
    }
    else if((sCardDrvMt626GetOnLineState() == true))
    {
        stCardAppCache.u32ReadTimeStamp = sGetTimestamp();
        stCardAppCache.bCardReaderBreakline = false;
    }
}






















