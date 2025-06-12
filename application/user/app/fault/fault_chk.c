/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   fault_chk.c
* Description                           :   fault check 故障告警检查实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#include "fault_chk.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "fault_chk";





stFaultChkMap_t stFaultChkMap;





bool sFaultChkInit(void);
void sFaultChkTask(void *pvParam);



//故障检测注册函数供外部调用
void sFaultChkRegister(void (*pFunc)(void));











/**********************************************************************************************
* Description                           :   故障告警码系统初始化
* Author                                :   Dai
* modified Date                         :   2024-08-12
* notice                                :   
***********************************************************************************************/
bool sFaultChkInit(void)
{
    bool bRst;
    
    sFaultInit();
    
    bRst = true;
    if(pdPASS != xTaskCreate(sFaultChkTask, "sFaultChkTask", 512, NULL, 28, NULL))
    {
        bRst = false;
        EN_SLOGE(TAG, "故障告警检查任务创建出错!!!");
    }
    
    return(bRst);
}














/**********************************************************************************************
* Description                           :   故障告警码检查任务
* Author                                :   Hall
* modified Date                         :   2024-06-12
* notice                                :   
***********************************************************************************************/
void sFaultChkTask(void *pvParam)
{
    u8 i;
    
    EN_SLOGI(TAG, "任务建立:故障告警检查任务");
    vTaskDelay(15000 / portTICK_RATE_MS);                                       //延时15秒等待状态就绪再检测故障
    
    while(1)
    {
        for(i = 0; i < stFaultChkMap.i32CmdNum; i++)
        {
            if(stFaultChkMap.pFunc[i] != NULL)
            {
                stFaultChkMap.pFunc[i]();
            }
            else
            {
                //遍历到第一个未注册的指针直接跳出循环
                break;
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}











/**********************************************************************************************
* Description                           :   故障告警码注册函数
* Author                                :   Dai
* modified Date                         :   2024-08-12
* notice                                :   
***********************************************************************************************/
void sFaultChkRegister(void (*pFunc)(void))
{
    if((stFaultChkMap.i32CmdNum < cPileFaultChkNumMax) && (pFunc != NULL))
    {
        stFaultChkMap.pFunc[stFaultChkMap.i32CmdNum] = pFunc;
        stFaultChkMap.i32CmdNum++;
    }
}





































