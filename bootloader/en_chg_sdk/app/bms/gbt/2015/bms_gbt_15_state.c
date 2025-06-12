/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_state.c
* Description                           :   国标2015通讯协议状态机管理
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-19
* notice                                :   车端紧急停充由驱动层负责，桩端紧急停充由用户层负责
****************************************************************************************************/

#include "bms_gbt_15_state.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_15_state";




//ccs状态描述字符串
u8 u8GbtStateString[eBmsGbt15StateMax][24] = 
{
    "待机",
    "握手辨识",
    "绝缘检测",
    "充电配置阶段",
    "预充电",
    "充电",
    "充电结束",
    "故障AB",
    "故障C",
};






extern stBmsGbt15Cache_t *pBmsGbt15Cache[ePileGunIndexNum];




eBmsGbt15ProtState_t sBmsGbt15StateGet(ePileGunIndex_t eGunIndex);
bool   sBmsGbt15SStateSet(ePileGunIndex_t eGunIndex, eBmsGbt15ProtState_t eState);




//gbt27930-15状态机处理方法
bool   sBmsGbt15StateBstCheck(ePileGunIndex_t eGunIndex);
bool   sBmsGbt15StateBemCheck(ePileGunIndex_t eGunIndex);
bool   sBmsGbt15StateAuthStopCheck(ePileGunIndex_t eGunIndex);
bool   sBmsGbt15StateStartTimer(ePileGunIndex_t eGunIndex, u8 u8TimerType, u32 u32Evt, u32 u32Timeout);

void   sBmsGbt15StateChgInit(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateHandshake(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateConfig(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateCharging(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateFinish(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateFault(ePileGunIndex_t eGunIndex);
void   sBmsGbt15StateTimeout(ePileGunIndex_t eGunIndex);


//发送数据帧函数
void   sBmsGbt15SendFrame(ePileGunIndex_t eGunIndex);


//状态机管理线程
void   sBmsGbt15StateTask(void *pParam);














/***************************************************************************************************
* Description                           :   GBT/BMS状态获取函数
* Author                                :   Hall
* Creat Date                            :   2024-07-18
* notice                                :   
****************************************************************************************************/
eBmsGbt15ProtState_t sBmsGbt15StateGet(ePileGunIndex_t eGunIndex)
{
    if(pBmsGbt15Cache[eGunIndex] == NULL)
    {
        return(eBmsGbt15StateMax);
    }
    
    return(pBmsGbt15Cache[eGunIndex]->eState);
}









/***************************************************************************************************
* Description                           :   GBT/BMS状态设置函数
* Author                                :   Hall
* Creat Date                            :   2024-07-18
* notice                                :   
****************************************************************************************************/
bool sBmsGbt15SStateSet(ePileGunIndex_t eGunIndex, eBmsGbt15ProtState_t eState)
{
    if((pBmsGbt15Cache[eGunIndex] == NULL) || (eState >= eBmsGbt15StateMax))
    {
        return(false);
    }
    
    if(pBmsGbt15Cache[eGunIndex]->eState != eState)
    {
        //有变化才打印和赋值---避免无效的刷屏
        EN_SLOGI(TAG, "SECC[%d], 状态切换:%d, %s", eGunIndex, eState, u8GbtStateString[eState]);
        pBmsGbt15Cache[eGunIndex]->eState = eState;
    }
    
    return(true);
}












/***************************************************************************************************
* Description                           :   gbt27930-15 判断是否收到Bst报文
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15StateBstCheck(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    eBmsGbt15StopReasonType_t eReason;
    
    if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBst == true)
    {
        eReason = sBmsGbt15CheckBstFrameIsNormal(eGunIndex);
        
        if((eReason == eBmsGbt15StopReasonDanger) || (eReason == eBmsGbt15StopReasonNormal))
        {
            if(eReason == eBmsGbt15StopReasonDanger)
            {
                //调用用户层紧急停机接口
                pCache->pBmsGbt15ToShutdownEmg(eGunIndex);
            }
            
            //停止充电，车端主动结束
            EN_SLOGI(TAG, "bmsDrv:枪%d收到BST报文，停止充电！！！", eGunIndex);
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCst;
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFinish);
            sBmsGbt15Set_CstCode(eGunIndex, eReasonBstFrame, eFaultNone, eErrNone);
            
            //启动Bst超时检测定时器
            sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bst);
            
            //启动Bsd超时检测定时器
            sBmsGbt15StateStartTimer(eGunIndex, 1, cGbtEvt_Pkt_Timer2, cTimeout_Gbt15_Bsd);
            
            //每个阶段切换前都清零
            pCache->u32TaskTick = 0;
            
            return true;
        }
        else
        {
            //忽略不可信状态报文
            pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBst = false;
            return false;
        }
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   gbt27930-15 判断是否收到Bem报文
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15StateBemCheck(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBem == true)
    {
        //停止充电，处理方式c，重新握手
        EN_SLOGI(TAG, "bmsDrv:枪%d收到BEM报文，等待重新握手", eGunIndex);
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
        
        //每个阶段切换前都清零
        pCache->u32TaskTick = 0;
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   gbt27930-15 判断授权是否中止
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15StateAuthStopCheck(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    if(pCache->stPacketSwitchCdt.bAuthorize == false)
    {
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCst;
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFinish);
        
        //桩原因停掉的发cst，赋值统一给正常，故障停会在状态机那边赋值，赋值后不会再被覆盖掉
        sBmsGbt15Set_CstCode(eGunIndex, eReasonManual, eFaultNone, eErrNone);
        
        //启动Bst超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bst);
        
        //启动Bsd超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 1, cGbtEvt_Pkt_Timer2, cTimeout_Gbt15_Bsd);
        
        //每个阶段切换前都清零
        pCache->u32TaskTick = 0;
        
        return true;
    }
    
    return false;
}










/***************************************************************************************************
* Description                           :   gbt27930-15 启动超时定时器
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-22
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15StateStartTimer(ePileGunIndex_t eGunIndex, u8 u8TimerType, u32 u32Evt, u32 u32Timeout)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //启动超时判断定时器
    if(u8TimerType == 0)
    {
        pCache->u32Timer1Event = u32Evt;
        pCache->u32Timer1Time = u32Timeout;
        sBmsGbtTimer1Start(eGunIndex);
    }
    else
    {
        pCache->u32Timer2Event = u32Evt;
        pCache->u32Timer2Time = u32Timeout;
        sBmsGbtTimer2Start(eGunIndex);
    }
    
    return true;
}












/***************************************************************************************************
* Description                           :   gbt27930-15 协议初始化阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-23
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateChgInit(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //1.初始化发送缓存数据
    if(pCache->bInitOk == false)
    {
        pCache->bInitOk = true;
        pCache->bAllowStopAuxPower = false;
        memset(&pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt, 0, sizeof(pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt));
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
        EN_SLOGI(TAG, "bmsDrv:枪%d-bms驱动协议初始化完成", eGunIndex);
    }
    //此处需要一直清理防止停止充电后依然有通讯数据
    memset(&pBmsGbt15Cache[eGunIndex]->stSeccInfo, 0, sizeof(pBmsGbt15Cache[eGunIndex]->stSeccInfo));
    memset(&pBmsGbt15Cache[eGunIndex]->stEvccInfo, 0, sizeof(pBmsGbt15Cache[eGunIndex]->stEvccInfo));
    
    //防止因为第三次重新握手收到Bst报文或故障态清除鉴权直接跳到eBmsGbt15StateFinish未完成握手次数清除
    pCache->u16TryAgainChgCount = 0;
    
    //2.判断用户层是否授权
    if(pCache->stPacketSwitchCdt.bAuthorize == true)
    {
        //3.1跳转到充电握手阶段
        EN_SLOGI(TAG, "bmsDrv:枪%d进入充电握手流程，开始发送CHM报文", eGunIndex);
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateHandshake);
        
        //3.2发送Chm报文
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeChm;
        
        //3.3启动Bhm超时判断定时器
        sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bhm);
        
        //3.4防止待机阶段收到车端的报文，重新初始化
        memset(&pBmsGbt15Cache[eGunIndex]->stEvccInfo, 0, sizeof(pBmsGbt15Cache[eGunIndex]->stEvccInfo));
        
        //3.5每个阶段切换前都清零
        pCache->u32TaskTick = 0;
    }
}






/***************************************************************************************************
* Description                           :   gbt27930-15 协议握手阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateHandshake(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //是否有收到Bem/Bst或用户层停止充电指令
    if((sBmsGbt15StateBemCheck(eGunIndex) == true)
    || (sBmsGbt15StateBstCheck(eGunIndex) == true)
    || (sBmsGbt15StateAuthStopCheck(eGunIndex) == true))
    {
        return;
    }
    
    
    if(pCache->stSeccInfo.stCrmData.u8RecogResult == 0xAA)
    {
        //判断Bcp超时
        if(pCache->bTimer1Timeout == true)
        {
            //停止充电，处理方式c，重新握手
            EN_SLOGI(TAG, "bmsDrv:枪%d检测BCP报文超时，开始发送CEM", eGunIndex);
            pCache->stSeccInfo.stCemData.unCode02.stCemCode02.bBcpTimeout = true;
            pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBcpTimeout = true;
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
            
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
            pCache->u32FirstSendTick = xTaskGetTickCount();
            
            //每个阶段切换前都清零
            pCache->u32TaskTick = 0;
        }
        else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcp == true)
        {
            //检查车辆参数是否匹配充电桩
            if(sBmsGbt15EvParamMatchCheck(eGunIndex) == eBmsGbt15EvParamCheckNormal)
            {
                //进入充电配置阶段
                EN_SLOGI(TAG, "bmsDrv:枪%d进入充电配置流程", eGunIndex);
                pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCtsAndCml;
                sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateConfig);
                
                //启动Bro超时判断定时器
                sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bro);
                
                //每个阶段切换前都清零
                pCache->u32TaskTick = 0;
            }
            else
            {
                //停止充电，处理方式c，重新握手
                EN_SLOGI(TAG, "bmsDrv:枪%d的Bcp参数不匹配，进入重新握手阶段，开始发送CEM", eGunIndex);
                pCache->stSeccInfo.stCemData.unCode04.stCemCode04.bStandby = true;
                pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
                
                sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
                pCache->u32FirstSendTick = xTaskGetTickCount();
            }
        }
    }
    else if(pCache->stPacketSwitchCdt.eCrmIsoCheckResult == eBmsGbt15IsoCheckNormal || pCache->stPacketSwitchCdt.eCrmIsoCheckResult == eBmsGbt15IsoCheckWarn)
    {
        //绝缘检测通过,发送Crm报文
        if(pCache->stSeccInfo.eSendFrameType != eSendFrameTypeCrm)
        {
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCrm;
            pCache->u32TaskTick = 0;
            
            //启动Brm超时判断定时器
            sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Brm);
        }
        
        
        //判断Brm是否超时
        if(pCache->bTimer1Timeout == true)
        {
            //停止充电，处理方式c，重新握手
            EN_SLOGI(TAG, "bmsDrv:枪%d检测BRM超时，开始发送CEM", eGunIndex);
            pCache->stSeccInfo.stCemData.unCode01.stCemCode01.bBrmTimeout = true;
            pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBrmTimeout = true;
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
            
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
            pCache->u32FirstSendTick = xTaskGetTickCount();
            
            //每个阶段切换前都清零
            pCache->u32TaskTick = 0;
        }
        else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBrm == true)
        {
            //发送Crm_AA
            pCache->stSeccInfo.stCrmData.u8RecogResult = 0xAA;
            
            //启动Bcp超时判断定时器
            sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bcp);
            
            pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBrm = false;
        }
    }
    else if(pCache->stPacketSwitchCdt.eCrmIsoCheckResult == eBmsGbt15IsoCheckFail)
    {
        //停止充电
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCst;
        sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultOther, eErrNone);
        
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFinish);
        pCache->pBmsGbt15ToShutdownEmg(eGunIndex);
    }
    else
    {
        //接收到Bhm报文或Bhm接收超时进入绝缘检测
        if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBhm == true)
        {
            //接收到数据，复位定时器
            sBmsGbtTimer1Rst(eGunIndex);
            
            //清除Bhm接收标志(发送接收处于同一个task，不需要操心会多任务同时操作同一个变量)
            pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBhm = false;
            
            //绝缘检测阶段
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateCableCheck);
        }
        else if(pCache->bTimer1Timeout == true)
        {
            //Bhm接收超时，超时直接做绝缘检测，兼容旧协议
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateCableCheck);
            pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBhmTimeout = true;
        }
    }
}






/***************************************************************************************************
* Description                           :   gbt27930-15 协议配置阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateConfig(ePileGunIndex_t eGunIndex)
{
    u32 u32TempTick = 0;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //是否有收到Bem/Bst或用户层停止充电指令
    if((sBmsGbt15StateBemCheck(eGunIndex) == true)
    || (sBmsGbt15StateBstCheck(eGunIndex) == true)
    || (sBmsGbt15StateAuthStopCheck(eGunIndex) == true))
    {
        return;
    }
    
    
    if(pCache->stSeccInfo.stCroData.eReady == 0xAA)
    {
        
        //Bcl或Bcs接收超时
        if((pCache->bTimer1Timeout == true) || (pCache->bTimer2Timeout == true))
        {
            if(pCache->bTimer1Timeout == true)
            {
                pCache->stSeccInfo.stCemData.unCode03.stCemCode03.bBclTimeout = true;
                pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBclTimeout = true;
                EN_SLOGI(TAG, "bmsDrv:枪%d接收Bcl超时，重新握手", eGunIndex);
            }
            else
            {
                pCache->stSeccInfo.stCemData.unCode03.stCemCode03.bBcsTimeout = true;
                pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBcsTimeout = true;
                EN_SLOGI(TAG, "bmsDrv:枪%d接收Bcs超时，重新握手", eGunIndex);
            }
            
            //停止充电，处理方式c，重新握手
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
            
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
            pCache->u32TaskTick = 0;
            pCache->u32FirstSendTick = xTaskGetTickCount();
        }
        else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcl == true && pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcs == true)
        {
            //收到了Bcl和Bcs报文
            EN_SLOGI(TAG, "bmsDrv:枪%d进入正常充电流程", eGunIndex);
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCcs;
            pCache->stSeccInfo.stCcsData.stAllowChg.bAllow = eBmsGbt15AllowChg;
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateCharging);
            
            //先默认车端允许充电，否则在未收到Bsm报文前用户层会认为车端暂停充电
            pCache->stEvccInfo.stBsmData.stBatFaultCode.bAllowChg = eBmsGbt15AllowChg;
            
            //接收到数据，复位定时器
            sBmsGbtTimer1Rst(eGunIndex);
            sBmsGbtTimer2Rst(eGunIndex);
            
            //每个阶段切换前都清零
            pCache->u32TaskTick = 0;
        }
        else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcl == true)
        {
            //接收到数据，复位定时器，恢复原来的超时时间
            pCache->u32Timer1Time = cTimeout_Gbt15_Bcl;
            sBmsGbtTimer1Rst(eGunIndex);
        }
        else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcs == true)
        {
            //接收到数据，复位定时器，恢复原来的超时时间
            pCache->u32Timer2Time = cTimeout_Gbt15_Bcs;
            sBmsGbtTimer2Rst(eGunIndex);
        }
    }
    else if((pCache->stPacketSwitchCdt.eCroPreChgSuccess == eBmsGbt15PreChgNormal)
         && (pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBro == true)
         && (pCache->stEvccInfo.stBrodata.eReady == eBmsGbt15ChgReadyStatusGetReady))
    {
        //预充成功，发送Cro_AA
        pCache->stSeccInfo.stCroData.eReady = 0xAA;
        
        u32TempTick = c250msTime - (xTaskGetTickCount() - pCache->u32TaskTick);
        
        //启动Bcl超时检测定时器，补充下一次Cro_AA的发送时间差
        sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, (cTimeout_Gbt15_Bcl + u32TempTick));
        
        //启动Bcs超时检测定时器，补充下一次Cro_AA的发送时间差
        sBmsGbt15StateStartTimer(eGunIndex, 1, cGbtEvt_Pkt_Timer2, (cTimeout_Gbt15_Bcs + u32TempTick));
    }
    else if(pCache->stPacketSwitchCdt.eCroPreChgSuccess == eBmsGbt15PreChgFail)
    {
        //预充失败，停止充电
        EN_SLOGI(TAG, "bmsDrv:枪%d预充条件不满足，停止充电", eGunIndex);
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCst;
        sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultOther, eErrNone);
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFinish);
        
        //启动Bst超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bst);
        
        //启动Bsd超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 1, cGbtEvt_Pkt_Timer2, cTimeout_Gbt15_Bsd);
        
        //每个阶段切换前都清零
        pCache->u32TaskTick = 0;
    }
    else
    {
        if((pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBro == true) && (pCache->stEvccInfo.stBrodata.eReady == eBmsGbt15ChgReadyStatusGetReady))
        {
            //接收到Bro_AA报文，开始发送Cro报文
            if(pCache->stSeccInfo.eSendFrameType != eSendFrameTypeCro)
            {
                pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCro;
                pCache->u32TaskTick = 0;
                
                //收到Bro_AA后，超时时间变回5s
                sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bro);
            }
            else
            {
                //接收到数据，复位定时器
                sBmsGbtTimer1Rst(eGunIndex);
            }
            
            //进入预充阶段，清除Bro接收标志
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StatePreCharge);
            pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBro = false;
        }
        else if((pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBro == true) && (pCache->stEvccInfo.stBrodata.eReady == eBmsGbt15ChgReadyStatusNotReady))
        {
            //接收到Bro_00报文，需确认是否已经接收过Bro_AA报文 (收到Bro_AA后要持续收到Bro_AA，否则进入故障级别3)
            if(pCache->stEvccInfo.stBrodata.bBroAAFlag == true)
            {
                //停止充电，处理方式c，重新握手
                EN_SLOGI(TAG, "bmsDrv:枪%d收到Bro_AA后又收到Bro_00，停止充电", eGunIndex);
                pCache->stSeccInfo.stCemData.unCode02.stCemCode02.bBroTimeout = true;
                pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
                
                sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultAB);
                pCache->u32FirstSendTick = xTaskGetTickCount();
            }
            
            //收到Bro_00后，更新一次超时时间
            if(pCache->bInitOk == true)
            {
                pCache->bInitOk = false;
                
                //启动Bro_AA超时检测定时器
                sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_BroAA);
            }
            
            //清除Bro接收标志
            pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBro = false;
        }
        else
        {
            //Bro接收超时
            if(pCache->bTimer1Timeout == true)
            {
                //停止充电，处理方式c，重新握手
                EN_SLOGI(TAG, "bmsDrv:枪%d接收Bro报文超时", eGunIndex);
                pCache->stSeccInfo.stCemData.unCode02.stCemCode02.bBroTimeout = true;
                pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBroTimeout = true;
                pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
                
                sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
                pCache->u32FirstSendTick = xTaskGetTickCount();
            }
        }
    }
}






/***************************************************************************************************
* Description                           :   gbt27930-15 协议充电阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateCharging(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //1.是否有收到Bem/Bst或用户层停止充电指令
    if((sBmsGbt15StateBemCheck(eGunIndex) == true)
    || (sBmsGbt15StateBstCheck(eGunIndex) == true)
    || (sBmsGbt15StateAuthStopCheck(eGunIndex) == true))
    {
        return;
    }
    
    
    //2.检查Bcl是否超时
    if(pCache->bTimer1Timeout == true)
    {
        //停止充电，处理方式c，重新握手
        EN_SLOGI(TAG, "bmsDrv:枪%d充电过程中Bcl接收超时，重新握手", eGunIndex);
        pCache->stSeccInfo.stCemData.unCode03.stCemCode03.bBclTimeout = true;
        pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBclTimeout = true;
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
        
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
        pCache->u32TaskTick = 0;
        pCache->u32FirstSendTick = xTaskGetTickCount();
        
        return;
    }
    else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcl == true)
    {
        //接收到数据，复位定时器
        sBmsGbtTimer1Rst(eGunIndex);
        pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcl = false;
    }
    
    //3.检查Bcs是否超时
    if(pCache->bTimer2Timeout == true)
    {
        //停止充电，处理方式c，重新握手
        EN_SLOGI(TAG, "bmsDrv:枪%d充电过程中Bcs接收超时，重新握手", eGunIndex);
        pCache->stSeccInfo.stCemData.unCode03.stCemCode03.bBcsTimeout = true;
        pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBcsTimeout = true;
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
        
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
        pCache->u32TaskTick = 0;
        pCache->u32FirstSendTick = xTaskGetTickCount();
        
        return;
    }
    else if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcs == true)
    {
        //接收到数据，复位定时器
        sBmsGbtTimer2Rst(eGunIndex);
        pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBcs = false;
    }
    
    
    //4.车端是否暂停了充电
    if(pCache->stEvccInfo.stBsmData.stBatFaultCode.bAllowChg == eBmsGbt15ForbidChg)
    {
        pCache->stSeccInfo.stCcsData.stAllowChg.bAllow = eBmsGbt15ForbidChg;
    }
    else
    {
        pCache->stSeccInfo.stCcsData.stAllowChg.bAllow = eBmsGbt15AllowChg;
    }
    
    
    //5.确认Bsm数据报文是否正常
    if(sBmsGbt15CheckBsmFrameIsNormal(eGunIndex) == false)
    {
        //车端故障停止充电，发送Cst报文，处理方式b
        EN_SLOGI(TAG, "bmsDrv:枪%d收到异常BSM报文，开始发送CST", eGunIndex);
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCst;
        sBmsGbt15Set_CstCode(eGunIndex, eReasonBstFrame, eFaultOther, eErrNone);
        pCache->stSeccInfo.stCstData.unStopFault.stFault.bOthers = true;
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFinish);
        
        //启动Bst超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 0, cGbtEvt_Pkt_Timer1, cTimeout_Gbt15_Bst);
        
        //启动Bsd超时检测定时器
        sBmsGbt15StateStartTimer(eGunIndex, 1, cGbtEvt_Pkt_Timer2, cTimeout_Gbt15_Bsd);
        
        pCache->pBmsGbt15ToShutdownEmg(eGunIndex);
        
        //每个阶段切换前都清零
        pCache->u32TaskTick = 0;
    }
}






/***************************************************************************************************
* Description                           :   gbt27930-15 协议结束充电阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateFinish(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //1.检测Bst报文是否超时
    if((pCache->bTimer1Timeout == true) && (pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBst == false))
    {
        //超时发送CEM，然后直接停止充电，不再重新握手
        pCache->stSeccInfo.stCemData.unCode03.stCemCode03.bBstTimeout = true;
        pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBstTimeout = true;
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
        
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
        pCache->u32FirstSendTick = xTaskGetTickCount();
        pCache->u16TryAgainChgCount = cReShakehandMaxCount;
        return;
    }
    
    //2.检测Bsd报文是否超时
    if((pCache->bTimer2Timeout == true) && (pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBsd == false))
    {
        //超时发送CEM，然后直接停止充电，不再重新握手
        pCache->stSeccInfo.stCemData.unCode04.stCemCode04.bBsdTimeout = true;
        pCache->stEvccInfo.unRecvTimeout.stRxTimeout.bBsdTimeout = true;
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCem;
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultC);
        pCache->u32FirstSendTick = xTaskGetTickCount();
        pCache->u16TryAgainChgCount = cReShakehandMaxCount;
        return;
    }
    
    //3.是否收到Bsd报文
    if(pCache->stEvccInfo.unRecvFlag.stRecvFlag.bBsd == true)
    {
        if(pCache->stSeccInfo.eSendFrameType == eSendFrameTypeCst)
        {
            pCache->u32FirstSendTick = xTaskGetTickCount();
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeCsd;
            
            //根据18487.1-2023的说法，要等收到Bsd才能断开辅助电源
            pCache->bAllowStopAuxPower = true;
        }
        
        
        //停止发送Csd报文的条件：发送Crm或者辅源断开。 临时处理：发送5s-Csd就停止
        if((xTaskGetTickCount() - pCache->u32FirstSendTick) > cKeepSendTime)
        {
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
            
            //等待拔枪
            if(pCache->stPacketSwitchCdt.bReturn == true)
            {
                sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateWait);
                pCache->stPacketSwitchCdt.bAuthorize = false;
                pCache->stPacketSwitchCdt.bReturn = false;
                pCache->bInitOk = false;
            }
        }
    }
}





/***************************************************************************************************
* Description                           :   gbt27930-15 协议充电故障阶段
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateFault(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //根据18487.1-2023的说法，要等收到Bsd才能断开辅助电源，严重故障时不判断Bsd
    pCache->bAllowStopAuxPower = true;
    
    //充电枪拔出后返回待机
    if(pCache->stPacketSwitchCdt.bReturn == true)
    {
        pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
        
        sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateWait);
        pCache->stPacketSwitchCdt.bAuthorize = false;
        pCache->stPacketSwitchCdt.bReturn = false;
        pCache->bInitOk = false;
    }
}






/***************************************************************************************************
* Description                           :   gbt27930-15 协议通讯超时
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateTimeout(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    //是否超过最大重新握手次数
    if(pCache->u16TryAgainChgCount < cReShakehandMaxCount)
    {
        //重新握手条件已经满足（条件：输出电流<5A，K1K2继电器断开，等待5s后再重新握手避免车端没有及时切换到重新握手状态）
        if((pCache->stPacketSwitchCdt.eCemReChgCondition == eBmsGbt15ReChgOk) && ((xTaskGetTickCount() - pCache->u32FirstSendTick) > cKeepSendTime))
        {
            //初始化一些关键的变量
            EN_SLOGI(TAG, "错误：国标%d枪开始重新第%d次握手,发送Crm_00", eGunIndex, pCache->u16TryAgainChgCount);
            memset(&pCache->stEvccInfo.unRecvFlag.u16RecvFlag, 0, sizeof(pCache->stEvccInfo.unRecvFlag.u16RecvFlag));
            memset(&pCache->stEvccInfo.stBemData, 0, sizeof(pCache->stEvccInfo.stBemData));
            memset(&pBmsGbt15Cache[eGunIndex]->stSeccInfo, 0, sizeof(pBmsGbt15Cache[eGunIndex]->stSeccInfo));
            
            pCache->stPacketSwitchCdt.eCroPreChgSuccess = eBmsGbt15PreChging;
            pCache->stPacketSwitchCdt.eCemReChgCondition = eBmsGbt15ReChgPrepare;
            pCache->bAllowStopAuxPower = false;
            
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateHandshake);
            
            //每个阶段切换前都清零
            pCache->u32TaskTick = 0;
            
            //重新握手次数+1
            (pCache->u16TryAgainChgCount)++;
        }
        else if((pCache->stPacketSwitchCdt.eCemReChgCondition != eBmsGbt15ReChgOk) && ((xTaskGetTickCount() - pCache->u32FirstSendTick) > 10000))
        {
            //当超过10s未达到重新握手条件则按通讯故障处理并停冲
            pCache->u16TryAgainChgCount = cReShakehandMaxCount;
            EN_SLOGI(TAG, "错误：国标%d枪超过10s未达到握手条件，按超时%次通讯故障处理", eGunIndex, pCache->u16TryAgainChgCount);
        }
    }
    else
    {
        //根据18487.1-2023的说法，要等收到Bsd才能断开辅助电源，超时跳转也认为条件满足
        //重试3次握手依旧失败跳转到Fault状态等待拔枪
        if((xTaskGetTickCount() - pCache->u32FirstSendTick) > cKeepSendTime)
        {
            sBmsGbt15SStateSet(eGunIndex, eBmsGbt15StateFaultAB);
            pCache->stSeccInfo.eSendFrameType = eSendFrameTypeIdle;
            pCache->bAllowStopAuxPower = true;
            pCache->u16TryAgainChgCount = 0;
        }
    }
}








/***************************************************************************************************
* Description                           :   gbt27930-15发送数据帧函数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-24
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15SendFrame(ePileGunIndex_t eGunIndex)
{
    static u8 u8CtsCnt = 0;
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    
    switch(pBmsGbt15Cache[eGunIndex]->stSeccInfo.eSendFrameType)
    {
    case eSendFrameTypeChm:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Chm
            sBmsGbt15SendChm(eGunIndex);
        }
        break;
    case eSendFrameTypeCrm:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Crm
            sBmsGbt15SendCrm(eGunIndex);
        }
        break;
    case eSendFrameTypeCtsAndCml:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Cml
            sBmsGbt15SendCml(eGunIndex);
            
            if((u8CtsCnt % 2) == 0)
            {
                //500ms周期发送Cts
                sBmsGbt15SendCts(eGunIndex);
            }
            u8CtsCnt++;
        }
        break;
    case eSendFrameTypeCro:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Cro
            sBmsGbt15SendCro(eGunIndex);
        }
        break;
    case eSendFrameTypeCcs:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c50msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //50ms周期发送Ccs
            sBmsGbt15SendCcs(eGunIndex);
        }
        break;
    case eSendFrameTypeCst:
        //10ms周期发送Cst
        sBmsGbt15SendCst(eGunIndex);
        break;
    case eSendFrameTypeCsd:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Csd
            sBmsGbt15SendCsd(eGunIndex);
        }
        break;
    case eSendFrameTypeCem:
        if((xTaskGetTickCount() - pCache->u32TaskTick) >= c250msTime)
        {
            pCache->u32TaskTick = xTaskGetTickCount();
            
            //250ms周期发送Cem
            sBmsGbt15SendCem(eGunIndex);
        }
        break;
    default:
        break;
    }
}








/***************************************************************************************************
* Description                           :   gbt27930-15状态机管理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                       
****************************************************************************************************/
void sBmsGbt15StateTask(void *pParam)
{
    ePileGunIndex_t eGunIndex = *((ePileGunIndex_t *)pParam);
    
    while(1)
    {
        //接收处理方法
        sBmsGbt15RecvProc(eGunIndex);
        
        switch(pBmsGbt15Cache[eGunIndex]->eState)
        {
        case eBmsGbt15StateWait:
            sBmsGbt15StateChgInit(eGunIndex);
            break;
        case eBmsGbt15StateHandshake:
        case eBmsGbt15StateCableCheck:
            sBmsGbt15StateHandshake(eGunIndex);
            break;
        case eBmsGbt15StateConfig:
        case eBmsGbt15StatePreCharge:
            sBmsGbt15StateConfig(eGunIndex);
            break;
        case eBmsGbt15StateCharging:
            sBmsGbt15StateCharging(eGunIndex);
            break;
        case eBmsGbt15StateFinish:
            sBmsGbt15StateFinish(eGunIndex);
            break;
        case eBmsGbt15StateFaultAB:
            sBmsGbt15StateFault(eGunIndex);
            break;
        case eBmsGbt15StateFaultC:
            sBmsGbt15StateTimeout(eGunIndex);
            break;
        default:
            break;
        }
        
        //发送数据帧
        sBmsGbt15SendFrame(eGunIndex);
        
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}



