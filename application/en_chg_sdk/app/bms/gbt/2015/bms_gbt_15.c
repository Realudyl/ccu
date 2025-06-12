/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15.c
* Description                           :   GBT27930 协议实现
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   
****************************************************************************************************/

#include "bms_gbt_15.h"
#include "bms_gbt_15_state.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_gbt_15";




//协议版本号 V1.1
const u8 protVer[cGbtChgProtVerLen] = {0x01, 0x01, 0x00};



//桩和车的数据缓存
stBmsGbt15Cache_t *pBmsGbt15Cache[ePileGunIndexNum];





extern st_mult_pkg_t mult_pkg_rx_info[ePileGunIndexNum];




//------------------------------------------------------------------------------------------------



//协议内容初始化
bool   sBmsGbt15Init(ePileGunIndex_t eGunIndex, i32 i32Port, i32 i32Baudrate, void (*pCbFunc)(ePileGunIndex_t eGunIndex));
bool   sBmsGbt15CacheInit(ePileGunIndex_t eGunIndex);




//gbt27930-2015 协议 桩端数据发送函数声明
i32   sBmsGbt15SendChm(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCrm(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCts(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCml(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCro(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCcs(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCst(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCsd(ePileGunIndex_t eGunIndex);
i32   sBmsGbt15SendCem(ePileGunIndex_t eGunIndex);



//gbt27930-2015 协议 车端数据接收函数声明
i32   sBmsGbt15RecvBhm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBrm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBcp(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBro(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBcl(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBcs(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBsm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBmv(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBmt(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBsp(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBst(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBsd(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);
i32   sBmsGbt15RecvBem(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len);


//数据解析函数
void  sBmsGbt15Decode(ePileGunIndex_t eGunIndex, u8 u8Pgn, u8 *pMsg, u16 u16Len);

//接收数据处理函数
i32   sBmsGbt15RecvProc(ePileGunIndex_t eGunIndex);











//GBT27930-15 接收到的消息  映射map
stBmsGbt15MsgIdMap_t stBmsGbt15MsgIdMap[] = 
{
    {eBmsGbt15PgnBhm,                   sBmsGbt15RecvBhm},
    {eBmsGbt15PgnBrm,                   sBmsGbt15RecvBrm},
    {eBmsGbt15PgnBcp,                   sBmsGbt15RecvBcp},
    {eBmsGbt15PgnBro,                   sBmsGbt15RecvBro},
    {eBmsGbt15PgnBcl,                   sBmsGbt15RecvBcl},
    {eBmsGbt15PgnBcs,                   sBmsGbt15RecvBcs},
    {eBmsGbt15PgnBsm,                   sBmsGbt15RecvBsm},
    {eBmsGbt15PgnBmv,                   sBmsGbt15RecvBmv},
    {eBmsGbt15PgnBmt,                   sBmsGbt15RecvBmt},
    {eBmsGbt15PgnBsp,                   sBmsGbt15RecvBsp},
    {eBmsGbt15PgnBst,                   sBmsGbt15RecvBst},
    {eBmsGbt15PgnBsd,                   sBmsGbt15RecvBsd},
    {eBmsGbt15PgnBem,                   sBmsGbt15RecvBem},
};







/***************************************************************************************************
* Description                           :   bms协议初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15Init(ePileGunIndex_t eGunIndex, i32 i32Port, i32 i32Baudrate, void (*pCbFunc)(ePileGunIndex_t eGunIndex))
{
    stBmsGbt15Cache_t *pCache = NULL;
    ePileGunIndex_t *pGunIndex = MALLOC(sizeof(ePileGunIndex_t));
    
    EN_SLOGD(TAG, "GBT27930(%d枪) 协议初始化", eGunIndex);
    
    //首次初始化申请堆空间
    if(pBmsGbt15Cache[eGunIndex] == NULL)
    {
        pBmsGbt15Cache[eGunIndex] = (stBmsGbt15Cache_t *)MALLOC(sizeof(stBmsGbt15Cache_t));
    }
    
    //缓存数据初始化
    pCache = pBmsGbt15Cache[eGunIndex];
    memset(pCache, 0, sizeof(stBmsGbt15Cache_t));
    pCache->i32Port = i32Port;
    pCache->pBmsGbt15ToShutdownEmg = pCbFunc;

    //定时器初始化
    sBmsGbtInitTiming(eGunIndex);
    
    //初始化can节点
    sCanDrvOpen(i32Port, i32Baudrate);
    
    *pGunIndex = eGunIndex;
    xTaskCreate(sBmsGbt15StateTask, "sBmsGbt15StateTask", (512),   pGunIndex, 30, NULL);
    
    return true;
}






/***************************************************************************************************
* Description                           :   bms协议缓存数据初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sBmsGbt15CacheInit(ePileGunIndex_t eGunIndex)
{
    //缓存数据初始化
    memset(&pBmsGbt15Cache[eGunIndex]->stSeccInfo, 0, sizeof(stBmsGbt15Secc_t));
    memset(&pBmsGbt15Cache[eGunIndex]->stEvccInfo, 0, sizeof(stBmsGbt15Evcc_t));
    
    return true;
}











/***************************************************************************************************
* Description                           :   发送充电握手报文CHM 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   握手阶段周期发送 
*                                       
****************************************************************************************************/
i32 sBmsGbt15SendChm(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnChm);
    stTxMsg.u8Len = 3;
    
    //充电机通信协议版本SC1 3byte
    stTxMsg.u8Data[0] = 0x01;
    stTxMsg.u8Data[1] = 0x01;
    stTxMsg.u8Data[2] = 0x00;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送充电辨识报文CRM 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCrm(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCrm);
    stTxMsg.u8Len = 8;
    
    //充电机辨识结果 1byte
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCrmData.u8RecogResult;
    
    //充电机编号和区域编码 4byte 3byte
    //memcpy(&stTxMsg.u8Data[1], pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCrmData.u8SeccNum, cSeccNumLen);
    //memcpy(&stTxMsg.u8Data[5], pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCrmData.u8AreaCode, cAreaCodeLen);
    stTxMsg.u8Data[1] = 00;
    stTxMsg.u8Data[2] = 00;
    stTxMsg.u8Data[3] = 00;
    stTxMsg.u8Data[4] = 00;
    stTxMsg.u8Data[5] = 0xFF;
    stTxMsg.u8Data[6] = 0xFF;
    stTxMsg.u8Data[7] = 0xFF;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送时间同步报文 周期：500ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCts(ePileGunIndex_t eGunIndex)
{
    u16 u16Bcd;
    stCanMsg_t stTxMsg;
    stFullTime_t stFullTime;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCts);
    stTxMsg.u8Len = 7;
    
    //2.获取系统Rtc时间
    stFullTime = sGetFullTime();
    u16Bcd = sHex16ToBcd(stFullTime.year);
    
    //秒
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[0] = sHex2Bcd(stFullTime.sec);
    //分
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[1] = sHex2Bcd(stFullTime.min);
    //时
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[2] = sHex2Bcd(stFullTime.hour);
    //日
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[3] = sHex2Bcd(stFullTime.day);
    //月
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[4] = sHex2Bcd(stFullTime.mon);
    //年L
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[5] = u16Bcd & 0xFF;
    //年H
    pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime[6] = (u16Bcd >> 8) & 0xFF;
    
    //充电机时间，压缩BCD码 7byte
    memcpy(stTxMsg.u8Data, pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCtsData.u8SeccTime, cSeccTimeLen);
    
    //3.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送充电机输出能力报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCml(ePileGunIndex_t eGunIndex)
{
    u16 u16TempData = 0;
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCml);
    stTxMsg.u8Len = 8;
    
    //电桩最高输出电压 单位：0.1v 2byte
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutVol & 0xFF;
    stTxMsg.u8Data[1] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutVol >> 8) & 0xFF;
    
    //电桩最低输出电压 单位：0.1v 2byte
    stTxMsg.u8Data[2] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MinOutVol & 0xFF;
    stTxMsg.u8Data[3] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MinOutVol >> 8) & 0xFF;
    
    //电桩最大输出电流 单位：0.1A 2byte
    if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
    {
        u16TempData = 30000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur;
    }
    else
    {
        if(pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur >= 4000)
        {
            pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur = 4000;
        }
        
        if(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16AllowMaxCur <= pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur)
        {
            pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur = pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16AllowMaxCur;
        }
        
        u16TempData = 4000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MaxOutCur;
    }
    
    stTxMsg.u8Data[4] = u16TempData & 0xFF;
    stTxMsg.u8Data[5] = (u16TempData >> 8) & 0xFF;
    
    //电桩最小输出电流 单位：0.1A 2byte
    if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
    {
        u16TempData = 30000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MinOutCur;
    }
    else
    {
        u16TempData = 4000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCmlData.u16MinOutCur;
    }
    
    stTxMsg.u8Data[6] = u16TempData & 0xFF;
    stTxMsg.u8Data[7] = (u16TempData >> 8) & 0xFF;
    
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送充电机准备就绪报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCro(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCro);
    stTxMsg.u8Len = 1;
    
    //充电机是否准备好供电
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCroData.eReady;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}





/***************************************************************************************************
* Description                           :   发送充电机充电状态报文 周期：50ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCcs(ePileGunIndex_t eGunIndex)
{
    u16 u16TempData = 0;
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCcs);
    stTxMsg.u8Len = 7;
    
    //电压输出值，单位：0.1v 2byte
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16OutVol;
    stTxMsg.u8Data[1] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16OutVol >> 8) & 0xFF;
    
    
    if(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bAllowChg == 0)
    {
        pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16OutCur = 0;
    }
    //电流输出值，单位：0.1A 2byte
    if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
    {
        u16TempData = 30000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16OutCur;
    }
    else
    {
        u16TempData = 4000 - pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16OutCur;
    }
    
    stTxMsg.u8Data[2] = u16TempData & 0xFF;
    stTxMsg.u8Data[3] = (u16TempData >> 8) & 0xFF;
    
    //累计充电时间，单位：min 2byte
    stTxMsg.u8Data[4] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16ChgTime & 0xFF;
    stTxMsg.u8Data[5] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.u16ChgTime >> 8) & 0xFF;
    
    //充电是否允许 2bit，未定义的bit需要填充1
    stTxMsg.u8Data[6] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCcsData.stAllowChg.bAllow | 0xFC;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送充电机中止充电报文 周期：10ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCst(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCst);
    stTxMsg.u8Len = 4;
    
    //充电中止原因
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCstData.unStopReason.u8Reason;
    
    //充电故障，未定义的bit需要填充1
    stTxMsg.u8Data[1] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCstData.unStopFault.u16Fault & 0xFF;
    stTxMsg.u8Data[2] = ((pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCstData.unStopFault.u16Fault >> 8) & 0xFF) | 0xF0;
    
    //充电错误，未定义的bit需要填充1
    stTxMsg.u8Data[3] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCstData.unStopErr.u8Err | 0xF0;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}




/***************************************************************************************************
* Description                           :   发送充电机统计报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCsd(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCsd);
    stTxMsg.u8Len = 8;
    
    //累计充电时间，单位：min 2byte
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCsdData.u16ChgTime & 0xFF;
    stTxMsg.u8Data[1] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCsdData.u16ChgTime >> 8) & 0xFF;
    
    //累计充电电量，单位：0.1 kw.h 2byte
    stTxMsg.u8Data[2] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCsdData.u16ChgKwh & 0xFF;
    stTxMsg.u8Data[3] = (pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCsdData.u16ChgKwh >> 8) & 0xFF;
    
    //充电机编号 4byte
    memcpy(&stTxMsg.u8Data[4], pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCsdData.u8SeccNum, cSeccNumLen);
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}





/***************************************************************************************************
* Description                           :   发送充电机错误报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15SendCem(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sBmsGbt15GetMsgCanId(eBmsGbt15PgnCem);
    stTxMsg.u8Len = 4;
    
    //未定义的bit需要填充1
    stTxMsg.u8Data[0] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCemData.unCode01.u8CemCode01 | 0xFC;
    stTxMsg.u8Data[1] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCemData.unCode02.u8CemCode02 | 0xF0;
    stTxMsg.u8Data[2] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCemData.unCode03.u8CemCode03 | 0xC0;
    stTxMsg.u8Data[3] = pBmsGbt15Cache[eGunIndex]->stSeccInfo.stCemData.unCode04.u8CemCode04 | 0xFC;
    
    //2.将组好的包发出去
    sCanDrvSend(pBmsGbt15Cache[eGunIndex]->i32Port, &stTxMsg);
    
    return 0;
}














/***************************************************************************************************
* Description                           :   收到车端握手报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBhm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //最高允许充电电压，单位0.1v 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBhmData.u16MaxPermitChgVol = (u16)pMsg[0] | ((u16)pMsg[1] << 8);
    
    //收到BHM标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBhm = true;
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端辨识报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBrm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    u8 u8Ver[cGbtBmsProtVerLen];
    
    //bms通信协议版本号 3byte
    memcpy(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8Protver, pMsg, cGbtBmsProtVerLen);
    memset(u8Ver, 0, cGbtBmsProtVerLen);
    memcpy(u8Ver, pMsg, cGbtBmsProtVerLen);
    
    if((u8Ver[0] == 0x31) && (u8Ver[1] == 0x43) && (u8Ver[2] == 0x53))
    {
        pBmsGbt15Cache[eGunIndex]->bSuperChgProt = true;
    }
    else
    {
        pBmsGbt15Cache[eGunIndex]->bSuperChgProt = false;
    }
    
    //电池类型 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.eBatType = pMsg[3];
    
    //电池额定容量，单位0.1Ah 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u16BatRateCap = (u16)pMsg[4] | ((u16)pMsg[5] << 8);
    
    //电池额定总电压，单位0.1v 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u16BatRateVol = (u16)pMsg[6] | ((u16)pMsg[7] << 8);
    
    //电池生产商名称 4byte
    memcpy(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BatFactName, &pMsg[8], 4);
    
    //电池组序号 4byte
    memcpy(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BatSerial, &pMsg[12], cBatSerialLen);
    
    //电池组生产日期 3byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BatYear = pMsg[16];
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BatMonth = pMsg[17];
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BatDay = pMsg[18];
    
    //电池组充电次数 3byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u32BatChgCount = ((u32)pMsg[19] | ((u32)pMsg[20] << 8) | ((u32)pMsg[21] << 16)) & 0xFFFFFF;
    
    //电池组产权标识 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.eBatProt = pMsg[22];
    
    //预留 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8Res = pMsg[23];
    
    //车辆识别码 17byte
    memcpy(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8Vin, &pMsg[24], cVinCodeLen);
    
    //bms软件版本号 8byte
    memcpy(pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrmData.u8BmsSoftVer, &pMsg[41], cBmsSoftVerLen);
    
    //收到BRM标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBrm = true;
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端电池充电参数报文 周期：500ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBcp(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    if(u16Len == 13)
    {
        //单体电池最高允许充电电压，单位：0.01v 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16CellMaxVol = pMsg[0] | (pMsg[1] << 8);
        
        //最高允许充电电流，单位：0.1A 2byte
        if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
        {
            pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16AllowMaxCur = 30000 - ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
        }
        else
        {
            pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16AllowMaxCur = 4000 - ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
        }
        
        //动力电池标称总能量，单位：0.1Kw.h 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16BatteryCap = (u16)pMsg[4] | ((u16)pMsg[5] << 8);
        
        //最高允许充电总电压，单位：0.1v 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16AllowMaxVol = (u16)pMsg[6] | ((u16)pMsg[7] << 8);
        
        //最高允许温度 1byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u8AllowMaxTemp = pMsg[8] - 50;
        
        //电池当前荷电状态，单位：0.1% 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16BatterySoc = (u16)pMsg[9] | ((u16)pMsg[10] << 8);
        
        //电池当前电压，单位：0.1v 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcpData.u16CurBatVol = (u16)pMsg[11] | ((u16)pMsg[12] << 8);
        
        //收到BCP标识置true
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBcp = true;
    }
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端充电准备就绪报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBro(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //bms是否准备就绪
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrodata.eReady = pMsg[0];
    
    //收到过Bro_AA的报文
    if(pMsg[0] == 0xAA)
    {
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBrodata.bBroAAFlag = true;
    }
    
    //收到BRO标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBro = true;
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端电池需求报文 周期：50ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBcl(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    u16 u16TempData;
    
    //电压需求，单位：0.1v 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBclData.u16ReqVol = (u16)pMsg[0] | ((u16)pMsg[1] << 8);
    
    //电流需求，单位：0.1A 2byte
    if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
    {
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBclData.u16ReqCur = 30000 - ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
    }
    else
    {
        u16TempData = ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
        
        if(u16TempData >= 4000)
        {
            u16TempData = 4000;
        }
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBclData.u16ReqCur = 4000 - u16TempData;
    }
    
    //充电模式 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBclData.u8ChgMode = pMsg[4];
    
    //收到Bcl标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBcl = true;
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端电池充电总状态报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBcs(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    u16 u16TempData = 0;
    
    if(u16Len == 9)
    {
        //充电电压测量值，单位：0.1v 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u16MeasureVol = (u16)pMsg[0] | ((u16)pMsg[1] << 8);
        
        //充电电流测量值，单位：0.1A 2byte
        if(pBmsGbt15Cache[eGunIndex]->bSuperChgProt == true)
        {
            pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u16MeasureCur = 30000 - ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
        }
        else
        {
            pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u16MeasureCur = 4000 - ((u16)pMsg[2] | ((u16)pMsg[3] << 8));
        }
        
        //单体电池最高电压及其组号 2byte
        u16TempData = (u16)pMsg[4] | ((u16)pMsg[5] << 8);
        
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.stMaxCellInfo.u16MaxCellVol = u16TempData & 0x0FFF;
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.stMaxCellInfo.u16OwnGroup   = (u16TempData & 0xF000) >> 12;
        
        //当前荷电状态 1byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u8CurSoc = pMsg[6];
        
        //估算剩余时间 2byte
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBcsData.u16RemainChgTime = (u16)pMsg[7] | ((u16)pMsg[8] << 8);
        
        //收到Bcs标识置true
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBcs = true;
    }
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端蓄电池状态信息报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBsm(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //最高单体电池电压所在位置编号 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.u8CellNum = pMsg[0];
    
    //最高电池温度 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.u8CellMaxTemp = pMsg[1] - 50;
    
    //最高温度检测点编号 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.u8MaxTempIdx = pMsg[2];
    
    //最低电池温度 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.u8CellMinTemp = pMsg[3] - 50;
    
    //最低温度检测点编号 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.u8MinTempIdx = pMsg[4];
    
    //故障码01
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatVolFault = pMsg[5] & 0x03;
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatSocFault = (pMsg[5] >> 2) & 0x03;
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatCurFault = (pMsg[5] >> 4) & 0x03;
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatTempFault = (pMsg[5] >> 6) & 0x03;
    
    //故障码02
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatIsulstatus = pMsg[6] & 0x03;
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bBatConnStatus = (pMsg[6] >> 2) & 0x03;
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsmData.stBatFaultCode.bAllowChg = (pMsg[6] >> 4) & 0x03;
    
    //收到BSM标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBsm = true;
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端单体电池电压信息报文 周期：不定
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBmv(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    u16 u16BatId;
    
    //长度不对不解析
    if(u16Len < (cBattUnitVoltNum * 2))
    {
        return (-1);
    }
    
    for(u16BatId = 0; u16BatId < cBattUnitVoltNum; u16BatId++)
    {
        //单体电池电压，12bit，单位：0.01V
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBmvData.u16CellVol[u16BatId] = (pMsg[u16BatId*2] | (pMsg[u16BatId*2+1] << 8)) & 0xFFF;
        //单体电池所属组号，4bit
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBmvData.u8CellGrp[u16BatId] = (pMsg[u16BatId*2+1] >> 4) & 0xF;
    }
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端电池温度信息报文 周期：不定
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBmt(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    u16 u16BatId;
    
    //长度不对不解析
    if(u16Len < cBattUnitTempNum)
    {
        return (-1);
    }
    
    for(u16BatId = 0; u16BatId < cBattUnitTempNum; u16BatId++)
    {
        //单体电池温度，单位：1°C
        pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBmtData.u8CellTemp[u16BatId] = pMsg[u16BatId] - 50;
    }
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端电池预留信息报文 周期：不定
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBsp(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //不解析
    
    return 0;
}




/***************************************************************************************************
* Description                           :   收到车端中止充电报文 周期：10ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBst(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //bms中止充电原因 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBstData.unStopReason.u8Reason = pMsg[0];
    
    //bms中止充电故障 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBstData.unStopFault.u16Fault = (u16)pMsg[1] | ((u16)pMsg[2] << 8);
    
    //bms中止充电错误 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBstData.unStopErr.u8Err = pMsg[3];
    
    //收到Bst标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBst = true;
    
    return true;
}




/***************************************************************************************************
* Description                           :   收到车端统计数据报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBsd(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //中止充电时的soc值 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsdData.u8StopSoc = pMsg[0];
    
    //单体电池最低电压，单位：0.01v 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsdData.u16MinCellVol = (u16)pMsg[1] | ((u16)pMsg[2] << 8);
    
    //单体电池最高电压，单位：0.01v 2byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsdData.u16MaxCellVol = (u16)pMsg[3] | ((u16)pMsg[4] << 8);
    
    //电池最低温度 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsdData.u8MinBatTemp = pMsg[5];
    
    //电池最高温度 1byte
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBsdData.u8MaxBatTemp = pMsg[6];
    
    //收到Bsd标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBsd = true;
    
    return true;
}




/***************************************************************************************************
* Description                           :   收到车端错误报文 周期：250ms
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvBem(ePileGunIndex_t eGunIndex, u8 *pMsg, u16 u16Len)
{
    //故障码01
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBemData.unCode01.u8BemCode01 = pMsg[0];
    
    //故障码02
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBemData.unCode02.u8BemCode02 = pMsg[1];
    
    //故障码03
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBemData.unCode03.u8BemCode03 = pMsg[2];
    
    //故障码04
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.stBemData.unCode04.u8BemCode04 = pMsg[3];
    
    //收到BEM标识置true
    pBmsGbt15Cache[eGunIndex]->stEvccInfo.unRecvFlag.stRecvFlag.bBem = true;
    
    return true;
}






/***************************************************************************************************
* Description                           :   数据解析
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-25
* notice                                :   
*                                           
****************************************************************************************************/
void  sBmsGbt15Decode(ePileGunIndex_t eGunIndex, u8 u8Pgn, u8 *pMsg, u16 u16Len)
{
    u16 u16GunIndex = 0;
    
    for(u16GunIndex=0; u16GunIndex < sArraySize(stBmsGbt15MsgIdMap); u16GunIndex++)
    {
        if(u8Pgn == stBmsGbt15MsgIdMap[u16GunIndex].ePgn)
        {
            stBmsGbt15MsgIdMap[u16GunIndex].pProc(eGunIndex, pMsg, u16Len);
            
            break;
        }
    }
}








/***************************************************************************************************
* Description                           :   车端数据接收处理函数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-19
* notice                                :   
*                                           
****************************************************************************************************/
i32 sBmsGbt15RecvProc(ePileGunIndex_t eGunIndex)
{
    u8  u8Pgn = 0;
    stCanMsg_t stRxMsg;
    u8 u8PS ;                                                                   //桩作为接收方:目标地址0x56
    u8 u8SA;                                                                    //桩作为接收方:源地址0xF4
    
    //调用底层can-api读取报文
    //if(sCanDrvRecv(pBmsGbt15Cache[eGunIndex]->i32Port, &stRxMsg) == true)
    //<<<一致性测试发现DP.3003过不了是因为多发了一帧CCS，根本原因在于缓存区数据未及时处理，不停读取缓存区直至为空这种做法有风险，总线负载率过高可能导致无法正常充电
    while(sCanDrvRecv(pBmsGbt15Cache[eGunIndex]->i32Port, &stRxMsg) == true)
    {
        //目的地址和源地址不对的情况下不处理
        u8PS = (stRxMsg.u32Id & 0x0000ff00) >> 8;
        u8SA = stRxMsg.u32Id & 0x000000ff;
        if(((u8PS != 0x56) && (u8PS != 0xFF)) || (u8SA !=0xF4))
        {
            continue;
        }
        
        u8Pgn = sBmsGbt15GetPgnCode(stRxMsg.u32Id);
        
        if(u8Pgn == eBmsGbt15PgnCm)
        {
            //长帧处理
            more_pkg_tranc_0xec_proc(eGunIndex, stRxMsg.u8Data);
        }
        else if((u8Pgn == eBmsGbt15PgnDt) && mult_pkg_rx_info[eGunIndex].pkg_rx_en)
        {
            //多帧数据接收缓存，=2表示接收完成
            if(more_pkg_tranc_0xeb_proc(eGunIndex, stRxMsg.u8Data) == 2)
            {
                //跳转到解析函数
                sBmsGbt15Decode(eGunIndex, mult_pkg_rx_info[eGunIndex].pgn[1], mult_pkg_rx_info[eGunIndex].buf, mult_pkg_rx_info[eGunIndex].received_len);
            }
        }
        else
        {
            //短帧，跳转到解析函数
            sBmsGbt15Decode(eGunIndex, u8Pgn, stRxMsg.u8Data, stRxMsg.u8Len);
        }
    }
    
    return 0;
}










