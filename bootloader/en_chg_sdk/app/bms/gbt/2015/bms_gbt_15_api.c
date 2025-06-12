/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_api.c
* Description                           :   协议结构体变量的一些set和get方法
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-19
* notice                                :   
****************************************************************************************************/
#include "bms_gbt_15_api.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_15_api";



//桩和车的数据缓存
extern stBmsGbt15Cache_t *pBmsGbt15Cache[ePileGunIndexNum];











//用户层set
bool sBmsGbt15Set_ChgPointNumber(ePileGunIndex_t eGunIndex, u8 *pNum);                                  //设置充电桩编码
bool sBmsGbt15Set_ChgAreaNum(ePileGunIndex_t eGunIndex, u8 *pAreaNum);                                  //设置充电桩地区号
bool sBmsGbt15Set_ChgMaxOutVol(ePileGunIndex_t eGunIndex, u16 u16MaxOutVol);                            //设置充电桩最大输出电压
bool sBmsGbt15Set_ChgMinOutVol(ePileGunIndex_t eGunIndex, u16 u16MinOutVol);                            //设置充电桩最小输出电压
bool sBmsGbt15Set_ChgMaxOutCur(ePileGunIndex_t eGunIndex, u16 u16MaxOutCur);                            //设置充电桩最大输出电流
bool sBmsGbt15Set_ChgMinOutCur(ePileGunIndex_t eGunIndex, u16 u16MinOutCur);                            //设置充电桩最小输出电流
bool sBmsGbt15Set_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t eResult);        //设置绝缘检测前枪口的电压结果
bool sBmsGbt15Set_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t eResult);              //设置绝缘检测结果
bool sBmsGbt15Set_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t eResult);                //设置预充电结果
bool sBmsGbt15Set_ReChgCondition(ePileGunIndex_t eGunIndex, eBmsGbt15ReChgCondition_t eCondition);      //设置重新握手条件是否达成
bool sBmsGbt15Set_OutputVoltage(ePileGunIndex_t eGunIndex, u16 u16OutputVol);                           //设置当前枪输出电压
bool sBmsGbt15Set_OutputCurrent(ePileGunIndex_t eGunIndex, u16 u16OutputCur);                           //设置当前枪输出电流
bool sBmsGbt15Set_AuthorizeState(ePileGunIndex_t eGunIndex, bool bState);                               //设置是否鉴权通过，通信开始的关键
bool sBmsGbt15Set_ChargeTotalWh(ePileGunIndex_t eGunIndex, u16 u16TotalWh);                             //设置本次充电累计充电电量
bool sBmsGbt15Set_ChargeTime(ePileGunIndex_t eGunIndex, u16 u16ChgTime);                                //设置本次充电持续时间
bool sBmsGbt15Set_Return2Wait(ePileGunIndex_t eGunIndex);                                               //设置协议返回初始化阶段
bool sBmsGbt15Set_CstCode(ePileGunIndex_t eGunIndex, eBmsGbt15StopReason_t eReason, eBmsGbt15StopFault_t eFault, eBmsGbt15StopErr_t eErr);    //设置停止Cst报文充电原因
bool sBmsGbt15Set_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t eResult);             //设置充电中CC1异常情况







//用户层get
bool sBmsGbt15Get_EvPauseChargeState(ePileGunIndex_t eGunIndex);                                        //获取车端暂停充电状态
bool sBmsGbt15Get_AllowStopAuxPower(ePileGunIndex_t eGunIndex);                                         //获取辅助电源关闭条件是否达成

bool sBmsGbt15Get_EvccMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol);                            //获取车端最高允许充电电压（用于绝缘检测）
bool sBmsGbt15Get_BcpMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol);                             //获取Bcp报文最大允许充电电压
bool sBmsGbt15Get_BcpMaxChgCur(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur);                             //获取Bcp报文最大允许充电电流
bool sBmsGbt15Get_BcpCurBatVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur);                             //获取Bcp报文当前电池电压
bool sBmsGbt15Get_BclReqChgVol(ePileGunIndex_t eGunIndex, u16 *pReqChgVol);                             //获取Bcl报文充电需求电压
bool sBmsGbt15Get_BclReqChgCur(ePileGunIndex_t eGunIndex, u16 *pReqChgCur);                             //获取Bcl报文充电需求电流

bool sBmsGbt15Get_ChgRemainTime(ePileGunIndex_t eGunIndex, u16 *pRemainTime);                           //获取本次充电剩余充电时间
u8   sBmsGbt15Get_EvccCurSoc(ePileGunIndex_t eGunIndex);                                                //获取当前电池Soc
bool sBmsGbt15Get_VinCode(ePileGunIndex_t eGunIndex, u8 *pVin);                                         //获取车端Vin码，17字节
bool sBmsGbt15Get_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t *eResult);             //获取绝缘检测结果
bool sBmsGbt15Get_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t *eResult);               //获取预充结果
bool sBmsGbt15Get_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t *eResult);       //获取绝缘检测结果
bool sBmsGbt15Get_EvccCacheData(ePileGunIndex_t eGunIndex, stBmsGbt15Evcc_t *pCache);                   //获取车端报文缓存结构体
bool sBmsGbt15Get_EvccFrameTimeout(ePileGunIndex_t eGunIndex, unBmsGbt15RecvTimeout *pTimeout);         //获取车端报文超时信息，用于告警
bool sBmsGbt15Get_BmsShakeHandTimes(ePileGunIndex_t eGunIndex, u16 *u16BmsShankhandTimes);              //获取重新握手次数
bool sBmsGbt15Get_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t * eResult);           //获取充电中CC1情况











/***************************************************************************************************
* Description                           :   用户层设置 bms协议版本
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgProtocolVer(ePileGunIndex_t eGunIndex, u8 *pVersion)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pVersion != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        memcpy(pCache->stSeccInfo.stChmData.u8ProtVer, pVersion, cGbtChgProtVerLen);
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机编号
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgPointNumber(ePileGunIndex_t eGunIndex, u8 *pNum)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pNum != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        memcpy(pCache->stSeccInfo.stCrmData.u8SeccNum, pNum, cSeccNumLen);
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机所在区域编码
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgAreaNum(ePileGunIndex_t eGunIndex, u8 *pAreaNum)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pAreaNum != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        memcpy(pCache->stSeccInfo.stCrmData.u8AreaCode, pAreaNum, cAreaCodeLen);
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机最大输出电压，单位：0.1V
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgMaxOutVol(ePileGunIndex_t eGunIndex, u16 u16MaxOutVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCmlData.u16MaxOutVol = u16MaxOutVol;
        
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机最小输出电压，单位：0.1V
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgMinOutVol(ePileGunIndex_t eGunIndex, u16 u16MinOutVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCmlData.u16MinOutVol = u16MinOutVol;
        
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机最大输出电流，单位：0.1A
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgMaxOutCur(ePileGunIndex_t eGunIndex, u16 u16MaxOutCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCmlData.u16MaxOutCur = u16MaxOutCur;
        
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 充电机最小输出电流，单位：0.1A
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChgMinOutCur(ePileGunIndex_t eGunIndex, u16 u16MinOutCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCmlData.u16MinOutCur = u16MinOutCur;
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 绝缘检测前枪口电压检测结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t eResult)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.ePreIsoCheckResult = eResult;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层设置 绝缘检测结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t eResult)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCrmIsoCheckResult = eResult;
        
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 预充电结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t eResult)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCroPreChgSuccess = eResult;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层设置 重新握手条件是否满足
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ReChgCondition(ePileGunIndex_t eGunIndex, eBmsGbt15ReChgCondition_t eCondition)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCemReChgCondition = eCondition;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层设置 当前输出电压，单位：0.1V
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_OutputVoltage(ePileGunIndex_t eGunIndex, u16 u16OutputVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCcsData.u16OutVol = u16OutputVol;
        
        return true;
    }
    
    return false;
}




/***************************************************************************************************
* Description                           :   用户层设置 当前输出电流，单位：0.1A
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_OutputCurrent(ePileGunIndex_t eGunIndex, u16 u16OutputCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCcsData.u16OutCur = u16OutputCur;
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 授权状态，通知驱动层开始通讯
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_AuthorizeState(ePileGunIndex_t eGunIndex, bool bState)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stPacketSwitchCdt.bAuthorize = bState;
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 本次充电消耗电量，单位：1Wh
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChargeTotalWh(ePileGunIndex_t eGunIndex, u16 u16TotalWh)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCsdData.u16ChgKwh = ((u16TotalWh * 10) / 1000);
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 当前充电累计时间（该数据来源于订单计费组件），单位：1min
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_ChargeTime(ePileGunIndex_t eGunIndex, u16 u16ChgTime)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stSeccInfo.stCcsData.u16ChgTime = u16ChgTime;
        pCache->stSeccInfo.stCsdData.u16ChgTime = u16ChgTime;
        
        return true;
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 从充电结束/故障 返回待机
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_Return2Wait(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pCache->stPacketSwitchCdt.bReturn = true;
        
        return true;
    }
    
    
    return false;
}






/***************************************************************************************************
* Description                           :   用户层设置 CstCode
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_CstCode(ePileGunIndex_t eGunIndex, eBmsGbt15StopReason_t eReason, eBmsGbt15StopFault_t eFault, eBmsGbt15StopErr_t eErr)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    
    if((pCache != NULL) && (pCache->stSeccInfo.stCstData.unStopReason.u8Reason == 00))
    {
        pCache->stSeccInfo.stCstData.unStopReason.u8Reason = eReason;
        pCache->stSeccInfo.stCstData.unStopFault.u16Fault = eFault;
        pCache->stSeccInfo.stCstData.unStopErr.u8Err = eErr;
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   用户层设置 Cc1在充电中状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Set_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t eResult)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCc1stateInChg = eResult;
        
        return true;
    }
    
    return false;
}


































/***************************************************************************************************
* Description                           :   用户层获取 车端是否暂停了充电
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-29
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_EvPauseChargeState(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        return ((pCache->stEvccInfo.stBsmData.stBatFaultCode.bAllowChg == 0) ? true : false);
    }
    
    return false;
}









/***************************************************************************************************
* Description                           :   用户层获取 是否达成断开辅助电源的条件
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-29
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_AllowStopAuxPower(ePileGunIndex_t eGunIndex)
{
    stBmsGbt15Cache_t *pCache = pBmsGbt15Cache[eGunIndex];
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        return (pCache->bAllowStopAuxPower);
    }
    
    return false;
}











/***************************************************************************************************
* Description                           :   用户层获取 车端最高允许充电电压（用于绝缘检测）
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_EvccMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pMaxChgVol != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        
        if(pCache->stEvccInfo.stBhmData.u16MaxPermitChgVol != 0)
        {
            *pMaxChgVol = pCache->stEvccInfo.stBhmData.u16MaxPermitChgVol;
        }
        else
        {
            //兼容老版本车，如果没有Bhm报文，默认500v做绝缘检测
            *pMaxChgVol = 5000;
        }
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   用户层获取 车端Bcp报文中最大允许充电电压
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_BcpMaxChgVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pMaxChgVol != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pMaxChgVol = pCache->stEvccInfo.stBcpData.u16AllowMaxVol;
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   用户层获取 车端Bcp报文中最大允许充电电流
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_BcpMaxChgCur(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pMaxChgCur != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pMaxChgCur = pCache->stEvccInfo.stBcpData.u16AllowMaxCur;
        return true;
    }
    
    return false;
}








/*******************************************************************************
 * @FunctionName   :      sBmsGbt15Get_BcpCurBatVol
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月15日  13:46:06
 * @Description    :      用户层获取 车端Bcp报文中当前电池组的电压
 * @Input          :      eGunIndex         枪号
 * @Input          :      pMaxChgCur        当前电池组的电压(分辨率0.1)
 * @Return         :      
*******************************************************************************/
bool sBmsGbt15Get_BcpCurBatVol(ePileGunIndex_t eGunIndex, u16 *pMaxChgCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pMaxChgCur != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pMaxChgCur = pCache->stEvccInfo.stBcpData.u16CurBatVol;
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   用户层获取 车端Bcl需求电压
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_BclReqChgVol(ePileGunIndex_t eGunIndex, u16 *pReqChgVol)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pReqChgVol != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pReqChgVol = pCache->stEvccInfo.stBclData.u16ReqVol;
        return true;
    }
    
    return false;
}











/***************************************************************************************************
* Description                           :   用户层获取 车端Bcl需求电流
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_BclReqChgCur(ePileGunIndex_t eGunIndex, u16 *pReqChgCur)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pReqChgCur != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pReqChgCur = pCache->stEvccInfo.stBclData.u16ReqCur;
        return true;
    }
    
    return false;
}










/***************************************************************************************************
* Description                           :   用户层获取 剩余充电时间，min
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_ChgRemainTime(ePileGunIndex_t eGunIndex, u16 *pRemainTime)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pRemainTime != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        *pRemainTime = pCache->stEvccInfo.stBcsData.u16RemainChgTime;
        
        return true;
    }
    
    return false;
}










/***************************************************************************************************
* Description                           :   用户层获取 车端当前soc
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-11
* notice                                :   
*                                       
****************************************************************************************************/
u8 sBmsGbt15Get_EvccCurSoc(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) || (pBmsGbt15Cache[eGunIndex] == NULL)) ? 0 : pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u8CurSoc;
}










/***************************************************************************************************
* Description                           :   用户层获取 车端Vin码
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-12
* notice                                :   需注意pVin可用长度不可小于17字节
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_VinCode(ePileGunIndex_t eGunIndex, u8 *pVin)
{
    stBmsGbt15Cache_t *pCache = NULL;
    
    if(pVin != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        pCache = pBmsGbt15Cache[eGunIndex];
        memcpy(pVin, pCache->stEvccInfo.stBrmData.u8Vin, cVinCodeLen);
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   用户层获取 绝缘检测结果
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_IsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15IsoResult_t *eResult)
{
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        *eResult = pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCrmIsoCheckResult;
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   用户层获取 预充电结果
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_PreChgResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreChgRst_t *eResult)
{
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        *eResult = pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCroPreChgSuccess;
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   用户层获取 绝缘检测结果
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_PreIsoCheckResult(ePileGunIndex_t eGunIndex, eBmsGbt15PreIsoResult_t * eResult)
{
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        *eResult = pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.ePreIsoCheckResult;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层获取 车端报文缓存结构体
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_EvccCacheData(ePileGunIndex_t eGunIndex, stBmsGbt15Evcc_t *pCache)
{
    if(pCache != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        memcpy(pCache, &pBmsGbt15Cache[eGunIndex]->stEvccInfo, sizeof(stBmsGbt15Evcc_t));
        
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   用户层获取 车端报文超时故障
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_EvccFrameTimeout(ePileGunIndex_t eGunIndex, unBmsGbt15RecvTimeout *pTimeout)
{
    if(pTimeout != NULL && pBmsGbt15Cache[eGunIndex] != NULL)
    {
        memcpy(pTimeout, &pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvTimeout, sizeof(unBmsGbt15RecvTimeout));
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层获取 当前握手次数（用于判断故障）
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_BmsShakeHandTimes(ePileGunIndex_t eGunIndex, u16 *u16BmsShankhandTimes)
{
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        *u16BmsShankhandTimes = pBmsGbt15Cache[eGunIndex]->u16TryAgainChgCount;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   用户层获取 充电中CCI状态（故障判断）
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Get_Cc1StateInChg(ePileGunIndex_t eGunIndex, eBmsGbt15ChgCc1State_t * eResult)
{
    if(pBmsGbt15Cache[eGunIndex] != NULL)
    {
        *eResult = pBmsGbt15Cache[eGunIndex]->stPacketSwitchCdt.eCc1stateInChg;
        
        return true;
    }
    
    return false;
}

