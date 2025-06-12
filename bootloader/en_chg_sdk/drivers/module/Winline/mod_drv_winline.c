/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_drv_winline.c
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#include "mod_drv_winline.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_drv_winline";

//---------------------------------------------------------------------------------------------------------


stModDrvWinlineCache_t *pModDrvWinlineCache = NULL;






//---------------------------------------------------------------------------------------------------------
bool  sModDrvWinlineInit(i32 i32Port, i32 i32Baudrate);
void  sModDrvWinlineRecvTask(void *pParam);
void  sModDrvWinlineSendTask(void *pParam);


//协议中多指令合并的数据交互过程
bool  sModDrvWinlineReadData(void);
bool  sModDrvWinlineReadInfo(void);
bool  sModDrvWinlineReadAllAddr(void);


//配套处理函数
bool  sModDrvWinlineMsgIdCheck(unModDrvWinlineMsgId_t unMsgId);
bool  sModDrvWinlineMsgIdFill(stCanMsg_t *pFrame, u8 u8Addr);
bool  sModDrvWinlineSetModExistSts(void);


//接收和接收处理
bool  sModDrvWinlineRecv(void);
void  sModDrvWinlineProc(stCanMsg_t *pFrame);

//命令发送
bool  sModDrvWinlineSend(u8 u8Addr, eModDrvWinlineFun_t eFun, eModDrvWinlineReg_t eReg, unModDrvWinlinePayload_t unPayload, bool bWaitAckFlag);


//---------------------------------------------------------------------------------------------------------



/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineInit(i32 i32Port, i32 i32Baudrate)
{
    i32  i;
    bool bRst;
    
    //初始化底层数据结构
    pModDrvWinlineCache = (stModDrvWinlineCache_t *)MALLOC(sizeof(stModDrvWinlineCache_t));
    memset(pModDrvWinlineCache, 0, sizeof(stModDrvWinlineCache_t));
    pModDrvWinlineCache->i32Port = i32Port;
    pModDrvWinlineCache->i32Baudrate = i32Baudrate;
    pModDrvWinlineCache->u32ModExistStsUpdateTime = 0;
    pModDrvWinlineCache->xSemPkt = xSemaphoreCreateBinary();
    pModDrvWinlineCache->xSemAck = xSemaphoreCreateBinary();
    xSemaphoreGive(pModDrvWinlineCache->xSemPkt);
    
    //打开CAN口
    bRst = sCanDrvOpen(i32Port, i32Baudrate);
    
    xTaskCreate(sModDrvWinlineRecvTask, "sModDrvWinlineRecvTask", (512), NULL, cModDrvWinlineRecvTaskPriority, NULL);
    xTaskCreate(sModDrvWinlineSendTask, "sModDrvWinlineSendTask", (512), NULL, cModDrvWinlineSendTaskPriority, NULL);
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 接收处理线程
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   
****************************************************************************************************/
void sModDrvWinlineRecvTask(void *pParam)
{
    i32 i32TimeoutCnt = 0;
    
    EN_SLOGI(TAG, "任务建立:永联科技 DC模块电源通讯接收任务");
    while(1)
    {
        i32TimeoutCnt = (sModDrvWinlineRecv() != true) ? (i32TimeoutCnt + 1) : 0;
        if(i32TimeoutCnt >= 1000)
        {
            i32TimeoutCnt = 0;
            EN_SLOGE(TAG, "错误:DC模块电源通讯接收报文超时!!!");
        }
        
        
        //检查掉线模块并清除其在线标志
        sModDrvWinlineSetModExistSts();
        
        vTaskDelay(cModDrvWinlineRecvTaskCycle / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}











/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 发送处理线程
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   
****************************************************************************************************/
void sModDrvWinlineSendTask(void *pParam)
{
    EN_SLOGI(TAG, "任务建立:永联科技 DC模块电源通讯发送任务");
    
    //初始化读取一次固有信息
    sModDrvWinlineReadInfo();
    
    while(1)
    {
        sModDrvWinlineReadData();
        sModDrvWinlineReadAllAddr();
        
        vTaskDelay(cModDrvWinlineSendTaskCycle / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 一些实时数据的读取
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   这些寄存器的字段 需要循环读取
****************************************************************************************************/
bool sModDrvWinlineReadData(void)
{
    i32  i;
    bool bRst = true;
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.u32Value = 0;
    for(i = cModDrvWinlineAddrMin; i < cModDrvWinlineAddrNumMax; i++)
    {
        if(pModDrvWinlineCache->stData[i].bModExistSts == true)
        {
            //以下顺序发送 都是带阻塞等待应答帧的, 因此发送一帧后无需主动延时等待
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUdcOut,          unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetIdcOut,          unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetIdcOutLimit,     unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetTdc,             unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUacUdcIn,        unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUbusP,           unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUbusN,           unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetTenv,            unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUa,              unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUb,              unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetUc,              unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetTpfc,            unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetWarningCode,     unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetAddrBySwitch,    unPayload, true);
            //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetPowerInput,      unPayload, true);
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetFanS,            unPayload, true);
        }
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 一些固有信息的读取
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   这些寄存器的字段一般不会变化，初始化读取一次就可以了
****************************************************************************************************/
bool sModDrvWinlineReadInfo(void)
{
    i32  i;
    bool bRst = true;
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.u32Value = 0;
    for(i = cModDrvWinlineAddrMin; i < cModDrvWinlineAddrNumMax; i++)
    {
        //固有信息时上电初始化读取一次的 此时无法判断哪些地址上有模块在线
        //只能全部查询一次 并且不能阻塞等待应答报文信号量
        //由于不能阻塞等待应答报文信号量 因此每发送一帧都需要等待几毫秒, 避免发送太快硬件外设来不及执行而丢包
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetPdcOutRated,     unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetIdcOutRated,     unPayload, false);
        //vTaskDelay(5 / portTICK_RATE_MS);
        //bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetAddrBySwitch,    unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetAltitude,        unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetInputMode,       unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetSerialNoLow,     unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetSerialNoHigh,    unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetSwVerDcdc,       unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
        bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetSwVerPfc,        unPayload, false);
        vTaskDelay(5 / portTICK_RATE_MS);
    }
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 轮询一次所有地址
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   eModDrvWinlineRegGetInputMode 对所有地址范围读取本寄存器 
*                                           主要目的是更新当前在线模块地址列表
****************************************************************************************************/
bool sModDrvWinlineReadAllAddr(void)
{
    i32  i;
    bool bRst = true;
    unModDrvWinlinePayload_t unValue;
    
    bool bUpdate;
    u32  u32Time;
    
    
    unValue.u32Value = 0;
    u32Time = sGetTimestamp();
    bUpdate = ((u32Time - pModDrvWinlineCache->u32ModExistStsUpdateTime) >= cModDrvWinlineReadAllAddrTime) ? true : false;
    if(bUpdate == true)
    {
        pModDrvWinlineCache->u32ModExistStsUpdateTime = u32Time;
        for(i = cModDrvWinlineAddrMin; i < cModDrvWinlineAddrNumMax; i++)
        {
            //对所有地址轮询时 大部分地址是没有回复的，所以不能阻塞等待应答报文
            bRst = bRst & sModDrvWinlineSend(i, eModDrvWinlineFunGet, eModDrvWinlineRegGetAddrBySwitch,       unValue, false);
        }
    }
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 接收报文的 MsgId 检查
* Author                                :   Hall
* Creat Date                            :   2024-04-03
* notice                                :   剔除不符合协议文本的报文
****************************************************************************************************/
bool sModDrvWinlineMsgIdCheck(unModDrvWinlineMsgId_t unMsgId)
{
    bool bRst;
    
    if((unMsgId.stValue.bAddSrc >  cModDrvWinlineAddrMax)                       //源地址超限
    || (unMsgId.stValue.ebit29 !=  eModDrvWinlineMsgIdBit29DataFrame)           //错误消息帧
    || (unMsgId.stValue.ebit30 !=  eModDrvWinlineMsgIdBit30Data)                //远程帧
    || (unMsgId.stValue.ebit31 !=  eModDrvWinlineMsgIdBit31Extended))           //标准帧------全部剔除
    {
        bRst = false;
    }
    else
    {
        if((unMsgId.stValue.bAddDst  == cModDrvWinlineAddrMaster)
        && (unMsgId.stValue.ePtp     == eModDrvWinlinePtpNormal)
        && (unMsgId.stValue.eProtNo  == eModDrvWinlineProtNoDefault))
        {
            //上位机利用 EN+模块电源 的CAN广播报文来标记地址在线的模块
            pModDrvWinlineCache->stData[unMsgId.stValue.bAddSrc].bModExistSts = true;
            pModDrvWinlineCache->stData[unMsgId.stValue.bAddSrc].u32RxTimestamp = sGetTimestamp();
            
            bRst = true;
        }
        else
        {
            bRst = false;
        }
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 MsgId 填充
* Author                                :   Hall
* Creat Date                            :   2024-04-03
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineMsgIdFill(stCanMsg_t *pFrame, u8 u8Addr)
{
    unModDrvWinlineMsgId_t unMsgId;
    
    memset(pFrame, 0, sizeof(stCanMsg_t));
    unMsgId.stValue.bGroup              = 0;
    unMsgId.stValue.bAddSrc             = cModDrvWinlineAddrMaster;
    unMsgId.stValue.bAddDst             = u8Addr;
    unMsgId.stValue.ePtp                = (u8Addr >= cModDrvWinlineAddrExternGroupMin) ? eModDrvWinlinePtpBd : eModDrvWinlinePtpNormal;
    unMsgId.stValue.eProtNo             = eModDrvWinlineProtNoDefault;
    unMsgId.stValue.ebit29              = eModDrvWinlineMsgIdBit29DataFrame;
    unMsgId.stValue.ebit30              = eModDrvWinlineMsgIdBit30Data;
    unMsgId.stValue.ebit31              = eModDrvWinlineMsgIdBit31Extended;
    pFrame->u32Id                       = unMsgId.u32Value;
    
    return(true);
}





/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 清除离线模块的 存在状态
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineSetModExistSts(void)
{
    u16 i;
    i32 i32Time;
    
    static i32 i32ModExistStsClearTime = 0;
    
    i32Time = sGetTimestamp();
    if((i32Time - i32ModExistStsClearTime) >= 1)
    {
        i32ModExistStsClearTime = i32Time;
        for(i = cModDrvWinlineAddrMin; i < cModDrvWinlineAddrNumMax; i++)
        {
            //持续收不到目标地址报文 就清除在线状态
            if((i32Time - pModDrvWinlineCache->stData[i].u32RxTimestamp) > cModDrvWinlineExistStsClearTime)
            {
                pModDrvWinlineCache->stData[i].bModExistSts = false;
            }
        }
    }
    
    return(true);
}





/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 获取底层模块的 存在状态
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
*                                           u8ModNum:调用者请求返回地址升序排列的前n个模块的 存在状态
*                                           pSts:是一个bool型数组 指向 u8ModNum * sizeof(bool) 大小的空间
****************************************************************************************************/
bool sModDrvWinlineGetModExistSts(bool *pSts, u8 u8ModNum)
{
    u16 i;
    
    if(u8ModNum > cModDrvWinlineAddrNumMax)
    {
        return(false);
    }
    
    //参数传递
    if(pSts != NULL)
    {
        for(i = 0; i < u8ModNum; i++)
        {
            pSts[i] = pModDrvWinlineCache->stData[i].bModExistSts;
        }
    }
    
    return(true);
}







/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 报文接收函数
* Author                                :   Hall
* Creat Date                            :   2024-04-03
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineRecv(void)
{
    i32  i;
    bool bRst;
    
    
    stCanMsg_t stFrame;
    unModDrvWinlineMsgId_t unMsgId;
    
    memset(&stFrame, 0, sizeof(stCanMsg_t));
    bRst = sCanDrvRecv(pModDrvWinlineCache->i32Port, &stFrame);
    if(bRst == true)
    {
        unMsgId.u32Value = stFrame.u32Id;
        if(sModDrvWinlineMsgIdCheck(unMsgId) == true)
        {
            sModDrvWinlineProc(&stFrame);
        }
    }
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 报文处理函数
* Author                                :   Hall
* Creat Date                            :   2024-04-03
* notice                                :   
****************************************************************************************************/
void sModDrvWinlineProc(stCanMsg_t *pFrame)
{
    u8  u8Addr;
    u64 u64Temp;
    unIeee754_t unValue;
    
    unModDrvWinlineMsgId_t unMsgId;
    stModDrvWinlinePkt_t *pRxPkt = NULL;
    
    unMsgId.u32Value = pFrame->u32Id;
    u8Addr = unMsgId.stValue.bAddSrc;                                           //bAddSrc 在 sModDrvWinlineMsgIdCheck() 已经限制过范围
    pRxPkt = (stModDrvWinlinePkt_t *)pFrame->u8Data;
    
    //有太多情况导致这里可能返回非 eModDrvWinlineErrNone 值
    //例如设置电压电流超范围, 为避免这里阻塞造成应用层时序异常，这里就改为收到报文不判值直接Give信号量
    xSemaphoreGive(pModDrvWinlineCache->xSemAck);
    if(pRxPkt->stHead.eErr != eModDrvWinlineErrNone)
    {
        //非 eModDrvWinlineErrNone 值时报文无需处理
        return;
    }
    
    unValue.u32Value = ntohl(pRxPkt->unPayload.unValue.u32Value);
    switch(ntohs(pRxPkt->stHead.eReg))
    {
    case eModDrvWinlineRegGetUdcOut:
        pModDrvWinlineCache->stData[u8Addr].f32UdcOut = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetIdcOut:
        pModDrvWinlineCache->stData[u8Addr].f32IdcOut = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetIdcOutLimit:
        pModDrvWinlineCache->stData[u8Addr].f32IdcOutLimit = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetTdc:
        pModDrvWinlineCache->stData[u8Addr].f32Tdc = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUacUdcIn:
        pModDrvWinlineCache->stData[u8Addr].f32UacUdcIn = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUbusP:
        pModDrvWinlineCache->stData[u8Addr].f32UbusP = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUbusN:
        pModDrvWinlineCache->stData[u8Addr].f32UbusN = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetTenv:
        pModDrvWinlineCache->stData[u8Addr].f32Tenv = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUa:
        pModDrvWinlineCache->stData[u8Addr].f32Ua = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUb:
        pModDrvWinlineCache->stData[u8Addr].f32Ub = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetUc:
        pModDrvWinlineCache->stData[u8Addr].f32Uc = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetTpfc:
        pModDrvWinlineCache->stData[u8Addr].f32Tpfc = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetPdcOutRated:
        pModDrvWinlineCache->stData[u8Addr].f32PdcOutRated = unValue.f32Value;
        break;
    case eModDrvWinlineRegGetIdcOutRated:
        pModDrvWinlineCache->stData[u8Addr].f32IdcOutRated = unValue.f32Value;
        break;
    case eModDrvWinlineRegSetGroup:
        pModDrvWinlineCache->stData[u8Addr].u32Group = unValue.u32Value;
        break;
    case eModDrvWinlineRegGetWarningCode:
        memcpy(&pModDrvWinlineCache->stData[u8Addr].stWarningCode, &unValue.u32Value, sizeof(unValue));
        break;
    case eModDrvWinlineRegGetAddrBySwitch:
        pModDrvWinlineCache->stData[u8Addr].u32AddrBySwitch = unValue.u32Value;
        break;
    case eModDrvWinlineRegGetPowerInput:
        pModDrvWinlineCache->stData[u8Addr].u32PowerInput = unValue.u32Value;
        break;
    case eModDrvWinlineRegGetAltitude:
        pModDrvWinlineCache->stData[u8Addr].u32Altitude = unValue.u32Value;
        break;
    case eModDrvWinlineRegGetInputMode:
        pModDrvWinlineCache->stData[u8Addr].eInputMode = unValue.u32Value;
        break;
    case eModDrvWinlineRegGetSerialNoLow:
        u64Temp = pModDrvWinlineCache->stData[u8Addr].u64SerialNo;
        u64Temp = (u64Temp & 0xffffffff00000000) + unValue.u32Value;
        pModDrvWinlineCache->stData[u8Addr].u64SerialNo = u64Temp;
        break;
    case eModDrvWinlineRegGetSerialNoHigh:
        u64Temp = pModDrvWinlineCache->stData[u8Addr].u64SerialNo;
        u64Temp = (u64Temp & 0xffffffff) + (((u64)unValue.u32Value) << 32);
        pModDrvWinlineCache->stData[u8Addr].u64SerialNo = u64Temp;
        break;
    case eModDrvWinlineRegGetSwVerDcdc:                                         //根据协议手册 取低16位值
        pModDrvWinlineCache->stData[u8Addr].u32SwVerDcdc = unValue.u32Value & 0xffff;
        break;
    case eModDrvWinlineRegGetSwVerPfc:                                          //根据协议手册 取低16位值
        pModDrvWinlineCache->stData[u8Addr].u32SwVerPfc = unValue.u32Value & 0xffff;
        break;
    case eModDrvWinlineRegGetFanS:                                              
        pModDrvWinlineCache->stData[u8Addr].u32FanS = unValue.u32Value;
        break;
    default:
        break;
    }
}








/***************************************************************************************************
* Description                           :   永联科技 NXR 系列充电模块电源驱动 报文发送函数
* Author                                :   Hall
* Creat Date                            :   2024-04-03
* notice                                :   
*                                           u8Addr      :地址域
*                                           eFun        :功能域
*                                           eReg        :寄存器地址
*                                           stPayload   :数据域部分
*                                           bWaitAckFlag:阻塞等待应答报文标志
****************************************************************************************************/
bool sModDrvWinlineSend(u8 u8Addr, eModDrvWinlineFun_t eFun, eModDrvWinlineReg_t eReg, unModDrvWinlinePayload_t unPayload, bool bWaitAckFlag)
{
    bool bRst;
    stCanMsg_t stFrame;
    stModDrvWinlinePkt_t *pTxPkt = NULL;
    
    //0:参数保护
    if(((eFun != eModDrvWinlineFunSet) && (eFun != eModDrvWinlineFunGet)))
    {
        return(false);
    }
    
    //1:访问保护---因为要共用 pModDrvWinlineCache->stRxPkt
    xSemaphoreTake(pModDrvWinlineCache->xSemPkt, portMAX_DELAY);
    
    
    //2:填充
    sModDrvWinlineMsgIdFill(&stFrame, u8Addr);
    stFrame.u8Len = sizeof(stModDrvWinlinePkt_t);
    pTxPkt = (stModDrvWinlinePkt_t *)stFrame.u8Data;
    pTxPkt->stHead.eFun = eFun;
    pTxPkt->stHead.eErr = eModDrvWinlineErrRsvd;
    pTxPkt->stHead.eReg = htons(eReg);
    pTxPkt->unPayload.u32Value = htonl(unPayload.u32Value);
    
    //3:发送
    xSemaphoreTake(pModDrvWinlineCache->xSemAck, 0);
    bRst = sCanDrvSend(pModDrvWinlineCache->i32Port, &stFrame);
    if((bRst == true) && (bWaitAckFlag == true))
    {
        //200ms足够收到回复报文了
        bRst = xSemaphoreTake(pModDrvWinlineCache->xSemAck, 200 / portTICK_RATE_MS);
    }
    
    
    //4:访问保护解除
    xSemaphoreGive(pModDrvWinlineCache->xSemPkt);
    
    return(bRst);
}








