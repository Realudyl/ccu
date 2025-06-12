/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs.c
* Description                           :   ccs 通讯主程序
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-28
* notice                                :   
****************************************************************************************************/
#include "bms_ccs.h"

#include "io_pwm_app.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs";



stBmsCcsCache_t *pBmsCcsCache[cMse102xDevNum];




//ccs状态描述字符串
u8 u8CcsStateString[eBmsCcsStateMax][24] = 
{
    "Wait",
    "Slac匹配 & Sdp发现",
    "v2g消息收发",
    "预期中止",
    "紧急停机",
    "其他异常"
    
};











eBmsCcsState_t sBmsCcsStateGet(i32 i32DevIndex);
bool  sBmsCcsStateSet(i32 i32DevIndex, eBmsCcsState_t eState);


bool  sBmsCcsInit(stMmeDev_t *pDev, ePileGunIndex_t eGunIndex, void (*pCbCloseGunOutput)(ePileGunIndex_t eGunIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));

//状态处理任务和函数
void  sBmsCcsStateProcTask(void *pParam);
void  sBmsCcsStateProcWait(stMmeDev_t *pMmeDev);
void  sBmsCcsStateProcMmeNet(stMmeDev_t *pMmeDev);
void  sBmsCcsStateProcV2g(stMmeDev_t *pMmeDev);
void  sBmsCcsStateProcShutdownNormal(stMmeDev_t *pMmeDev);
void  sBmsCcsStateProcShutdownEmg(stMmeDev_t *pMmeDev);
void  sBmsCcsStateProcAbnormal(stMmeDev_t *pMmeDev);

//状态切换动作函数
void  sBmsCcsStateSwitchMmeToV2g(i32 i32DevIndex);
void  sBmsCcsStateSwitchV2gToShutdownEmg(i32 i32DevIndex);












/***************************************************************************************************
* Description                           :   SECC 状态获取函数
* Author                                :   Hall
* Creat Date                            :   2023-04-18
* notice                                :   
****************************************************************************************************/
eBmsCcsState_t sBmsCcsStateGet(i32 i32DevIndex)
{
    if((i32DevIndex >= cMse102xDevNum) || (pBmsCcsCache[i32DevIndex] == NULL))
    {
        return(eBmsCcsStateWait);
    }
    
    return(pBmsCcsCache[i32DevIndex]->eState);
}







/***************************************************************************************************
* Description                           :   SECC 状态设置函数
* Author                                :   Hall
* Creat Date                            :   2022-08-23
* notice                                :   
****************************************************************************************************/
bool sBmsCcsStateSet(i32 i32DevIndex, eBmsCcsState_t eState)
{
    if((i32DevIndex >= cMse102xDevNum) || (pBmsCcsCache[i32DevIndex] == NULL) || (eState >= eBmsCcsStateMax))
    {
        return(false);
    }
    
    if(pBmsCcsCache[i32DevIndex]->eState != eState)
    {
        //有变化才打印和赋值---避免无效的刷屏
        EN_SLOGI(TAG, "SECC[%d], 状态切换:%d, %s", i32DevIndex, eState, u8CcsStateString[eState]);
        pBmsCcsCache[i32DevIndex]->eState = eState;
    }
    
    return(true);
}


















/***************************************************************************************************
* Description                           :   CCS通讯资源初始化
* Author                                :   Hall
* Creat Date                            :   2023-12-28
* notice                                :   
****************************************************************************************************/
bool sBmsCcsInit(stMmeDev_t *pDev, ePileGunIndex_t eGunIndex, void (*pCbCloseGunOutput)(ePileGunIndex_t eGunIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn))
{
    bool bRst;
    stMmeDev_t *pMmeDev = NULL;
    
    
    //1:参数限幅及参数拷贝到本地
    pMmeDev = (stMmeDev_t *)MALLOC(sizeof(stMmeDev_t));
    if((pMmeDev == NULL) || (pDev->i32Index >= cMse102xDevNum))
    {
        EN_SLOGI(TAG, "SECC(通道:%d) 资源初始化失败。", pDev->i32Index);
        return(false);
    }
    memcpy(pMmeDev, pDev, sizeof(stMmeDev_t));
    
    
    //2:cache 初始化
    pBmsCcsCache[pMmeDev->i32Index] = (stBmsCcsCache_t *)MALLOC(sizeof(stBmsCcsCache_t));
    pBmsCcsCache[pMmeDev->i32Index]->eState = eBmsCcsStateWait;
    pBmsCcsCache[pMmeDev->i32Index]->i32SlacEnable = 0;
    pBmsCcsCache[pMmeDev->i32Index]->eGunIndex = eGunIndex;
    pBmsCcsCache[pMmeDev->i32Index]->pCbCloseGunOutput = pCbCloseGunOutput;
    pBmsCcsCache[pMmeDev->i32Index]->pCpPwmControl = pCpPwmControl;
    
    
    //3:mme组件初始化
    bRst = sMmeInit(pMmeDev, eGunIndex, pCpPwmControl);
    
    //4:net组件初始化
    bRst = bRst & sBmsCcsNetInit(pDev->i32Index, sBmsCcsStateSwitchMmeToV2g);
    
    //5:v2g组件初始化
    bRst = bRst & sV2gOptInit(pDev->i32Index, eGunIndex, sBmsCcsStateSwitchV2gToShutdownEmg, pCpPwmControl);
    
    
    //6:创建ccs状态管理任务
    xTaskCreate(sBmsCcsStateProcTask, "sBmsCcsStateProcTask", (512), pMmeDev, 23, NULL);
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   SECC 状态管理线程
* Author                                :   Hall
* Creat Date                            :   2022-06-20
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcTask(void *pParam)
{
    stMmeDev_t *pMmeDev = (stMmeDev_t *)pParam;
    
    EN_SLOGI(TAG, "任务建立:SECC(通道:%d), 状态机管理主任务。", pMmeDev->i32Index);
    
    while((pMmeDev->i32Index >= 0) && (pMmeDev->i32Index < cMse102xDevNum))
    {
        switch(pBmsCcsCache[pMmeDev->i32Index]->eState)
        {
        case eBmsCcsStateWait:
            sBmsCcsStateProcWait(pMmeDev);
            break;
        case eBmsCcsStateMmeNet:
            sBmsCcsStateProcMmeNet(pMmeDev);
            break;
        case eBmsCcsStateV2g:
            sBmsCcsStateProcV2g(pMmeDev);
            break;
        case eBmsCcsStateShutdownNormal:
            sBmsCcsStateProcShutdownNormal(pMmeDev);
            break;
        case eBmsCcsStateShutdownEmg:
            sBmsCcsStateProcShutdownEmg(pMmeDev);
            break;
        case eBmsCcsStateAbnormal:
            sBmsCcsStateProcAbnormal(pMmeDev);
            break;
        default:
            break;
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/***************************************************************************************************
* Description                           :   SECC 状态管理 之 wait 状态
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcWait(stMmeDev_t *pMmeDev)
{
    if(pBmsCcsCache[pMmeDev->i32Index]->i32SlacEnable == 1)
    {
        //开启CP振荡器
        pBmsCcsCache[pMmeDev->i32Index]->pCpPwmControl(pBmsCcsCache[pMmeDev->i32Index]->eGunIndex, true);
        sIoPwmDrvSetPwmDuty(sIoPwmGetCp(pBmsCcsCache[pMmeDev->i32Index]->eGunIndex), 5);
        
        //一些初始化动作
        sV2gResetTimingLogic(pMmeDev->i32Index);
        sV2gOptCacheInit(pMmeDev->i32Index);
        sV2gOptHandCacheInit(pMmeDev->i32Index);
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateMmeNet);
    }
}






/***************************************************************************************************
* Description                           :   SECC 状态管理 之 MME 底层网络建立阶段
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcMmeNet(stMmeDev_t *pMmeDev)
{
    if(pBmsCcsCache[pMmeDev->i32Index]->i32SlacEnable == 0)
    {
        pBmsCcsCache[pMmeDev->i32Index]->bSlacMatchFlagOld = false;
        sMmeSetNetInit(pMmeDev->i32Index);
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateWait);
    }
    
    if((pBmsCcsCache[pMmeDev->i32Index]->bSlacMatchFlagOld == false)
    && (sMmeGetSlacMatchFlag(pMmeDev->i32Index) != pBmsCcsCache[pMmeDev->i32Index]->bSlacMatchFlagOld))
    {
        //<ISO15118-2:2014.pdf>@8.7.3.3 [V2G2-714]
        //sV2gSeccCommunicationSetupPerformanceTimerStart(pMmeDev->i32Index);
        
        pBmsCcsCache[pMmeDev->i32Index]->bSlacMatchFlagOld = true;
    }
    
    //有实时性要求，由TCP连接建立位置直接调用 sBmsCcsStateSwitchMmeToV2g 函数实现
    //if(sBmsCcsNetGetTcpConnStatus(pMmeDev->i32Index) == true)
    
}








/***************************************************************************************************
* Description                           :   SECC 状态管理 之 v2g消息收发 状态
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcV2g(stMmeDev_t *pMmeDev)
{
    //1:预期中止, SECC requirements for premature intentional termination
    //  检查 是否 SessionStop 会话阶段
    if(sV2gOptGetSessionStop(pMmeDev->i32Index) == true)
    {
        sV2gSeccSequenceTimerStop(pMmeDev->i32Index);
        sBmsCcsNetSetTcpConnClose(pMmeDev->i32Index);
        sMmeSetNetInit(pMmeDev->i32Index);
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateShutdownNormal);
    }
    
    
    //2:EVSE紧急停机, EVSE-initiated emergency shutdown
    //有实时性要求，由故障触发位置直接调用 sBmsCcsStateSwitchV2gToShutdownEmg 函数实现
    
    
    
    //3:其他异常
    if(sBmsCcsNetGetTcpConnStatus(pMmeDev->i32Index) != true)
    {
        //检查TCP连接--v2g通讯阶段 TCP连接意外断开
        pBmsCcsCache[pMmeDev->i32Index]->pCbCloseGunOutput(pMmeDev->i32Index);  //关闭枪输出
        sV2gSeccSequenceTimerStop(pMmeDev->i32Index);
        sV2gSeccCpStateDetectionTimerStop(pMmeDev->i32Index);
        sMmeSetNetInit(pMmeDev->i32Index);
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateAbnormal);
    }
    else if(sV2gHandshakeGetNegotiation(pMmeDev->i32Index) == false)
    {
        //握手协商失败
        sBmsCcsNetSetTcpConnClose(pMmeDev->i32Index);
        pBmsCcsCache[pMmeDev->i32Index]->pCpPwmControl(pBmsCcsCache[pMmeDev->i32Index]->eGunIndex, false);
        sMmeSetNetInit(pMmeDev->i32Index);
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateAbnormal);
    }
}





/***************************************************************************************************
* Description                           :   SECC 状态管理 之 预期中止 状态
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcShutdownNormal(stMmeDev_t *pMmeDev)
{
    if(sBmsCcsNetGetTcpConnStatus(pMmeDev->i32Index) != true)
    {
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateWait);
    }
}







/***************************************************************************************************
* Description                           :   SECC 状态管理 之 EVSE发起的紧急停机 状态
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcShutdownEmg(stMmeDev_t *pMmeDev)
{
    if(sBmsCcsNetGetTcpConnStatus(pMmeDev->i32Index) != true)
    {
        sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateWait);
    }
}









/***************************************************************************************************
* Description                           :   SECC 状态管理 之 其他异常 状态
* Author                                :   Hall
* Creat Date                            :   2023-07-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateProcAbnormal(stMmeDev_t *pMmeDev)
{
    sBmsCcsStateSet(pMmeDev->i32Index, eBmsCcsStateWait);
}






/***************************************************************************************************
* Description                           :   SECC 状态管理 之 V2G状态 切换动作函数
* Author                                :   Hall
* Creat Date                            :   2024-03-25
* notice                                :   
****************************************************************************************************/
void sBmsCcsStateSwitchMmeToV2g(i32 i32DevIndex)
{
    if(sBmsCcsStateGet(i32DevIndex) == eBmsCcsStateMmeNet)
    {
        //向v2g阶段切换之前一系列复位操作
        sV2gResetTimingLogic(i32DevIndex);
        pBmsCcsCache[i32DevIndex]->i32SlacEnable = 0;
        pBmsCcsCache[i32DevIndex]->bSlacMatchFlagOld = false;
        
        //[V2G-DC-432] 启动 V2G_SECC_Sequence_Timer 定时器
        sV2gSeccSequenceTimerStart(i32DevIndex);
        sBmsCcsStateSet(i32DevIndex, eBmsCcsStateV2g);
    }
}









/***************************************************************************************************
* Description                           :   SECC 状态管理 之 EVSE紧急停机 切换动作函数
* Author                                :   Hall
* Creat Date                            :   2024-02-19
* notice                                :   考虑到切换的实时性 eBmsCcsStateV2g--->eBmsCcsStateShutdownEmg 切换的动作执行函数
*                                           需要设计成回调函数传递 供din/iso协议内部直接执行
****************************************************************************************************/
void sBmsCcsStateSwitchV2gToShutdownEmg(i32 i32DevIndex)
{
    if(sBmsCcsStateGet(i32DevIndex) == eBmsCcsStateV2g)
    {
        //紧急停机的动作仅在从 eBmsCcsStateV2g 向 eBmsCcsStateShutdownEmg 切换的时刻执行
        //避免重复执行以下动作 造成时序问题
        pBmsCcsCache[i32DevIndex]->pCbCloseGunOutput(i32DevIndex);              //关闭枪输出
        sV2gSeccSequenceTimerStop(i32DevIndex);
        sV2gSeccCpStateDetectionTimerStop(i32DevIndex);
        sBmsCcsNetSetTcpConnClose(i32DevIndex);
        sMmeSetNetInit(i32DevIndex);
        sBmsCcsStateSet(i32DevIndex, eBmsCcsStateShutdownEmg);
    }
}









