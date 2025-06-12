/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_eu.c
* Description                           :   欧标充电桩实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
#include "pile_eu.h"
#include "pile_eu_din.h"
#include "pile_eu_iso1.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_eu";




stPileEuCache_t stPileEuCache;







bool sPileEuInit(void);
void sPileEuTask(void *pParam);


void sPileEuClostGunOutput(ePileGunIndex_t eGunIndex);


bool sPileEuShellShutdown(const stShellPkt_t *pkg);







//TEST
stShellCmd_t stSellCmdShutdownCmd = 
{
    .pCmd       = "shutdown",
    .pFormat    = "格式: shutdown 0",
    .pFunction  = "功能: 桩端停充",
    .pRemarks   = "备注: 第0号枪 : 0  第1号枪 : 1",
    .pFunc      = sPileEuShellShutdown,
};












/***************************************************************************************************
* Description                           :   欧标充电桩 初始化
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
bool sPileEuInit(void)
{
    ePileGunIndex_t *pGunIndex = NULL;
    stMmeDev_t stMmeDev = 
    {
        //整个CCS协议下所有 i32Index 都是PLC设备序号,而不是枪序号
        //在单枪或多枪CCS桩里, 每把枪的PLC设备序号与枪序号都相同
        //但在ccs枪与国标或日标枪组合的多枪桩里，这俩序号不一定相同
        .i32Index           = ePileGunIndex0 - ePileGunIndex0,
        .u8MacEvseHsot      = {0x00, 0x13, 0xD7, 0x78, 0x79, 0x80},
        .u8MacEvsePlcNode   = {0x00, 0x13, 0xD7, 0x00, 0x00, 0x01},
        .u8Name             = "P0",
    };
    
    stMmeDev_t *pMmeDev = (stMmeDev_t *)MALLOC(sizeof(stMmeDev_t));
    memcpy(pMmeDev, &stMmeDev, sizeof(stMmeDev));
    
    //协议栈初始化
    tcpip_init(NULL, NULL);
    
    //bms初始化
    sBmsCcsInit(pMmeDev, ePileGunIndex0, sPileEuClostGunOutput, sIoPwmSetCpOnOff);
    
    //注册shell命令
    sShellCmdRegister(&stSellCmdShutdownCmd);
    
    //桩缓存资源初始化
    memset(&stPileEuCache, 0, sizeof(stPileEuCache));
    stPileEuCache.eGunIndexBaseCcs = ePileGunIndex0;
    sPileEuStateSet(ePileGunIndex0, ePileEuStateMax);                                //上电赋初值
    
    pGunIndex = MALLOC(sizeof(ePileGunIndex_t));
    (*pGunIndex) = ePileGunIndex0;
    xTaskCreate(sPileEuTask,      "sPileEuTask",      (1024), pGunIndex, 19, NULL);
    xTaskCreate(sPileEuStateTask, "sPileEuStateTask", (512),  pGunIndex, 18, NULL);
    
    return(true);
}









/***************************************************************************************************
* Description                           :   欧标充电桩 主任务
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
****************************************************************************************************/
void sPileEuTask(void *pParam)
{
    ePileGunIndex_t eGunIndex = (*((ePileGunIndex_t *)pParam));
    
    eChgCpState_t eCpState;
    eChgCpState_t eCpStateOld = eChgCpStateMax;
    
    EN_SLOGI(TAG, "任务建立:欧标充电桩 主任务, 对应枪序号:%d", eGunIndex);
    
    while(1)
    {
        sPileEuDinMainLoop(eGunIndex);
        sPileEuIso1MainLoop(eGunIndex);
        
        //更新底层各组件的cp状态---参见SECC状态机设计图
        eCpState = sAdcAppSampleGetCpState(eGunIndex);
        if(eCpStateOld != eCpState)
        {
            eCpStateOld = eCpState;
            sMmeSetCpState(eCpState);
            sV2gOptSetCpState(eGunIndex, eCpState);
            
            if(eCpState == eChgCpStateA)
            {
                //拔枪状态
            }
            else if((eCpState == eChgCpStateB) || (eCpState == eChgCpStateC) || (eCpState == eChgCpStateD))
            {
                //插枪状态
            }
        }
        
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}








/***************************************************************************************************
* Description                           :   关闭指定的枪 
* Author                                :   Hall
* Creat Date                            :   2024-02-23
* notice                                :   
*
*                                           供给bms驱动层紧急停机或其他异常停机时调用 关闭输出
****************************************************************************************************/
void sPileEuClostGunOutput(ePileGunIndex_t eGunIndex)
{
    if(sAdcAppSampleGetFbStateK(eGunIndex) == eAdcAppFbState11)
    {
        sModAppAvgClose(eGunIndex);
        sIoSetDrK(eGunIndex, false);
    }
}












bool sPileEuShellShutdown(const stShellPkt_t *pkg)
{
    struct dinDC_EVSEStatusType stDC_EVSEStatus =
    {
        .EVSEIsolationStatus = dinisolationLevelType_Invalid,
        .EVSEIsolationStatus_isUsed = 1u,
        .EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Shutdown,
        .NotificationMaxDelay = 0,
        .EVSENotification = dinEVSENotificationType_StopCharging,
    };
    
    sV2gDinSet_DC_EVSEStatus(atoi(pkg->para[0] - stPileEuCache.eGunIndexBaseCcs), eV2gDinMsgIdMax, &stDC_EVSEStatus);
    
    return(true);
}


















