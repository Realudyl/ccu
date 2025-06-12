/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_timing.c
* Description                           :   GBT27930-15 协议实现 之时序部分
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-19
* notice                                :   
****************************************************************************************************/

#include "bms_gbt_timing.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_timing";



extern stBmsGbt15Cache_t *pBmsGbt15Cache[ePileGunIndexNum];









//------------------------------------------------------------------------------------------------



//定时器任务初始化
void sBmsGbtInitTiming(ePileGunIndex_t eGunIndex);


//定时器清零
void sBmsGbtTimer1Rst(ePileGunIndex_t eGunIndex);
void sBmsGbtTimer2Rst(ePileGunIndex_t eGunIndex);


//定时器停止
void sBmsGbtTimer1Stop(ePileGunIndex_t eGunIndex);
void sBmsGbtTimer2Stop(ePileGunIndex_t eGunIndex);



//定时器启动
void sBmsGbtTimer1Start(ePileGunIndex_t eGunIndex);
void sBmsGbtTimer2Start(ePileGunIndex_t eGunIndex);



//定时器任务
void sBmsGbtTimer1Task(void *pParam);
void sBmsGbtTimer2Task(void *pParam);







/***************************************************************************************************
* Description                           :   gbt27930 协议时序初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
void sBmsGbtInitTiming(ePileGunIndex_t eGunIndex)
{
    u16 *pGunId = (u16 *)MALLOC(sizeof(u16));
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    pCache->xEvtTiming = xEventGroupCreate();
    xEventGroupClearBits(pCache->xEvtTiming, cGbtEvt_All);
    
    *pGunId = eGunIndex;
    
    //获取时序任务的句柄，用于给指定的任务发送消息（备注：无法给多个任务发送）
    xTaskCreate(sBmsGbtTimer1Task,  "sBmsGbtTimer1Task",    (512),    pGunId,    31,    &pCache->xTaskTimer1);
    xTaskCreate(sBmsGbtTimer2Task,  "sBmsGbtTimer2Task",    (512),    pGunId,    31,    &pCache->xTaskTimer2);
}



/***************************************************************************************************
* Description                           :   gbt27930 定时器1复位
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer1Rst(ePileGunIndex_t eGunIndex)
{
    i32 i32Cmd;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];

    //停止定时器
    xEventGroupSetBits(pBmsGbt15Cache[eGunIndex]->xEvtTiming, cGbtEvt_Pkt_Timer1);
    
    while(i32Cmd <= 100)
    {
        i32Cmd++;
        if(pCache->bTimer1IsRunning == false)
        {
            break;
        }
    }
    
    //向Timer1任务发送消息，开启定时器----这是开始计时等待下一次接收该报文
    xTaskNotify(pBmsGbt15Cache[eGunIndex]->xTaskTimer1, cNotify_TimeoutCheck, eSetBits);
}





/***************************************************************************************************
* Description                           :   gbt27930 定时器2复位
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer2Rst(ePileGunIndex_t eGunIndex)
{
    i32 i32Cmd;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    xEventGroupSetBits(pBmsGbt15Cache[eGunIndex]->xEvtTiming, cGbtEvt_Pkt_Timer2);
    
    while(i32Cmd <= 100)
    {
        i32Cmd++;
        if(pCache->bTimer2IsRunning == false)
        {
            break;
        }
    }
    
    //向Timer2任务发送消息，开启定时器----这是开始计时等待下一次接收该报文
    xTaskNotify(pBmsGbt15Cache[eGunIndex]->xTaskTimer2, cNotify_TimeoutCheck, eSetBits);
}





/***************************************************************************************************
* Description                           :   gbt27930 定时器1停止
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer1Stop(ePileGunIndex_t eGunIndex)
{
    xEventGroupSetBits(pBmsGbt15Cache[eGunIndex]->xEvtTiming, cGbtEvt_Pkt_Timer1_s);
}





/***************************************************************************************************
* Description                           :   gbt27930 定时器2停止
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer2Stop(ePileGunIndex_t eGunIndex)
{
    xEventGroupSetBits(pBmsGbt15Cache[eGunIndex]->xEvtTiming, cGbtEvt_Pkt_Timer2_s);
}





/***************************************************************************************************
* Description                           :   gbt27930 定时器1启动
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer1Start(ePileGunIndex_t eGunIndex)
{
    i32 i32Cmd;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    pBmsGbt15Cache[eGunIndex]->bTimer1Timeout = false;
    
    //启动定时器前先判断定时器是否在运行态
    if(pCache->bTimer1IsRunning == true)
    {
        //关闭定时器
        sBmsGbtTimer1Stop(eGunIndex);
        
        while(i32Cmd <= 100)
        {
            i32Cmd++;
            if(pCache->bTimer1IsRunning == false)
            {
                break;
            }
        }
    }
    
    //向xTaskSequence任务发送消息，重新开启定时器
    xTaskNotify(pBmsGbt15Cache[eGunIndex]->xTaskTimer1, cNotify_TimeoutCheck, eSetBits);
}







/***************************************************************************************************
* Description                           :   gbt27930 定时器2启动
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer2Start(ePileGunIndex_t eGunIndex)
{
    i32 i32Cmd;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    pBmsGbt15Cache[eGunIndex]->bTimer2Timeout = false;
    
    //启动定时器前先判断定时器是否处于运行态
    if(pCache->bTimer2IsRunning == true)
    {
        //关闭定时器
        sBmsGbtTimer2Stop(eGunIndex);
        
        while(i32Cmd <= 100)
        {
            i32Cmd++;
            if(pCache->bTimer2IsRunning == false)
            {
                break;
            }
        }
        
    }
    
    //向xTaskSequence任务发送消息，重新开启定时器
    xTaskNotify(pBmsGbt15Cache[eGunIndex]->xTaskTimer2, cNotify_TimeoutCheck, eSetBits);
}





/***************************************************************************************************
* Description                           :   国标协议 Timer1 任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer1Task(void *pParam)
{
    EventBits_t xEvtBitWait;
    u32  u32Notify;
    i32  i32Ret;
    u16  *pGunId = pParam;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[*pGunId];
    
    EN_SLOGI(TAG, "任务建立:SECC[%d], Timer1 时序任务", (*pGunId));
    
    while(1)
    {
        //等待定时器启动
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        
        //收到需要超时判断的消息
        if((u32Notify & (cNotify_TimeoutCheck)) != 0)
        {
            //等待事件设置为：事件超时或者收到数据后停止计时
            xEvtBitWait = pCache->u32Timer1Event | cGbtEvt_Pkt_Timer1_s;
            pCache->bTimer1IsRunning = true;
                
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, (pCache->u32Timer1Time) / portTICK_RATE_MS);
            if((i32Ret & cGbtEvt_Pkt_Timer1) != 0)
            {
                //收到了等待的报文，
                pCache->bTimer1Timeout = false;
            }
            else if((i32Ret & (cGbtEvt_Pkt_Timer1_s)) != 0)
            {
                //提前停止计时事件
                pCache->bTimer1Timeout = false;
            }
            else
            {
                //超时
                pCache->bTimer1Timeout = true;
            }
            
            pCache->bTimer1IsRunning = false;
        }
    }
    
    vTaskDelete(NULL);
    
}






/***************************************************************************************************
* Description                           :   国标协议 Timer2 任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
****************************************************************************************************/
void sBmsGbtTimer2Task(void *pParam)
{
    EventBits_t xEvtBitWait;
    u32  u32Notify;
    i32  i32Ret;
    u16  *pGunId = pParam;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[*pGunId];
    
    EN_SLOGI(TAG, "任务建立:SECC[%d], Timer2 时序任务", (*pGunId));
    
    while(1)
    {
        //等待定时器启动
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        
        //收到需要超时判断的消息
        if((u32Notify & (cNotify_TimeoutCheck)) != 0)
        {
            //等待事件设置为：事件超时或者外部状态变化后停止计时
            xEvtBitWait = pCache->u32Timer2Event | cGbtEvt_Pkt_Timer2_s;
            pCache->bTimer2IsRunning = true;
            
            i32Ret = xEventGroupWaitBits(pCache->xEvtTiming, xEvtBitWait, pdTRUE, pdFALSE, (pCache->u32Timer2Time) / portTICK_RATE_MS);
            if((i32Ret & cGbtEvt_Pkt_Timer2) != 0)
            {
                //收到了等待的报文，
                pCache->bTimer2Timeout = false;
            }
            else if((i32Ret & (cGbtEvt_Pkt_Timer2_s)) != 0)
            {
                //提前停止计时事件
                pCache->bTimer2Timeout = false;
            }
            else
            {
                //超时
                pCache->bTimer2Timeout = true;
            }
            
            pCache->bTimer2IsRunning = false;
        }
        
    }
    
    vTaskDelete(NULL);
    
}





