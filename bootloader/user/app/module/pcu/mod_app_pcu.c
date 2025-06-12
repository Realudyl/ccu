/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_pcu.c
* Description                           :   PCU通讯
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-18
* notice                                :   
****************************************************************************************************/
#include "mod_app_pcu.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_app_pcu";




stModAppPcuInfo_t *pPcuInfoCache[ePileGunIndexNum];









//初始化
bool sModAppPcuInit(u8 u8SwitchAddr, bool bIsSuperCharger);


//对外的api函数
bool sModAppPcuOpen(ePileGunIndex_t eGunIndex);
bool sModAppPcuClose(ePileGunIndex_t eGunIndex);
bool sModAppPcuCloseContactor(void);
bool sModAppPcuSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
bool sModAppPcuGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxVolt);
bool sModAppPcuGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxCurr);
bool sModAppPcuGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pCurVolt);


bool sModAppPcuSetReqVolt(ePileGunIndex_t eGunIndex, u16 u16ReqVolt);
bool sModAppPcuSetReqCurr(ePileGunIndex_t eGunIndex, u16 u16ReqCurr);
bool sModAppPcuGetCommSt(ePileGunIndex_t eGunIndex, ePcuCommState_t *pCommSt);
bool sModAppPcuGetEnergyErrSt(ePileGunIndex_t eGunIndex);
bool sModAppPcuGetReqVolt(ePileGunIndex_t eGunIndex, u16* u16ReqVolt);
bool sModAppPcuGetReqCurr(ePileGunIndex_t eGunIndex, u16* u16ReqCurr);





//主任务及收发处理
void sModAppPcuTask(void *pParam);

bool sModAppPcuMainLoop(ePileGunIndex_t eGunIndex);
bool sModAppPcuFlushCacheData(ePileGunIndex_t eGunIndex);
bool sModAppPcuRecvDecode(void);
u32  sModAppPcuGetMsgCanId(u8 u8GunNum, ePcuPgnCode_t ePgn);

bool sModAppPcuSendRegister(ePileGunIndex_t eGunIndex);//发送注册
bool sModAppPcuSendHeartPack(ePileGunIndex_t eGunIndex);//发送心跳
bool sModAppPcuSendEnergyReq(ePileGunIndex_t eGunIndex);//发送功率申请
bool sModAppPcuSendVoltChange(ePileGunIndex_t eGunIndex);//发送内侧电压变化
bool sModAppPcuRecvRegisterResp(ePileGunIndex_t eGunIndex, u8 *pMsg);//接收注册响应
bool sModAppPcuRecvHeartPackResp(ePileGunIndex_t eGunIndex, u8 *pMsg);//接收心跳响应

















/***************************************************************************************************
* Description                           :   初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuInit(u8 u8SwitchAddr, bool bIsSuperCharger)
{
    ePileGunIndex_t eGunIndex;
    stModAppPcuInfo_t *pCache = NULL;
    
    
    for(eGunIndex = 0; eGunIndex < ePileGunIndexNum; eGunIndex++)
    {
        EN_SLOGD(TAG, "CCU-%d枪与PCU通讯 协议初始化", eGunIndex);
        
        //首次初始化申请堆空间
        if(pPcuInfoCache[eGunIndex] == NULL)
        {
            pPcuInfoCache[eGunIndex] = (stModAppPcuInfo_t *)MALLOC(sizeof(stModAppPcuInfo_t));
        }
        
        
        //缓存数据初始化
        pCache = pPcuInfoCache[eGunIndex];
        memset(pCache, 0, sizeof(stModAppPcuInfo_t));
        
        pCache->i32Port = cPcuCanPort;
        pCache->u8GunAddr = u8SwitchAddr + eGunIndex * cGunAddrOffset;
        pCache->bIsSuperChgGun = bIsSuperCharger;
    }
    
    //初始化can节点
    sCanDrvOpen(cPcuCanPort, cPcuCanBaudrate);
    
    //创建任务
    xTaskCreate(sModAppPcuTask, "sModAppPcuTask", (512), NULL, 24, NULL);
    
    return true;
}







/***************************************************************************************************
* Description                           :   申请Pcu模块开机
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuOpen(ePileGunIndex_t eGunIndex)
{
    return true;
}






/***************************************************************************************************
* Description                           :   申请Pcu模块关机
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuClose(ePileGunIndex_t eGunIndex)
{
    sModAppPcuSetReqVolt(eGunIndex, 0);
    sModAppPcuSetReqCurr(eGunIndex, 0);
    
    //立即向Pcu发送需求，以达到快速停止输出的目的
    //sModAppPcuSendEnergyReq(eGunIndex);
    
    return true;
}







/***************************************************************************************************
* Description                           :   切断接触器
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   在PCU控模块的硬件方案下, 没有此动作的需求 直接返回
****************************************************************************************************/
bool sModAppPcuCloseContactor(void)
{
    return true;
}








/***************************************************************************************************
* Description                           :   设置模块输出电压电流
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit)
{
    sModAppPcuSetReqVolt(eGunIndex, (u16)f32Udc);
    sModAppPcuSetReqCurr(eGunIndex, (u16)f32IdcLimit);
    
    return true;
}










/***************************************************************************************************
* Description                           :   获取Pcu最大可提供的电压，单位：1V
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxVolt)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    if(pMaxVolt != NULL)
    {
        *pMaxVolt = pCache->u16AvailVolt;
        
        return true;
    }
    
    return false;
}










/***************************************************************************************************
* Description                           :   获取Pcu最大可提供的电流，单位：1A
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pMaxCurr)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    if(pMaxCurr != NULL)
    {
        *pMaxCurr = pCache->u16AvailCurr;
        
        return true;

    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   获取Pcu当前输出电压，单位：10V
* Author                                :   Dai
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pCurVolt)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    if(pCurVolt != NULL)
    {
        *pCurVolt = pCache->u16CurVolt;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   设置需求电压，单位：1V
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSetReqVolt(ePileGunIndex_t eGunIndex, u16 u16ReqVolt)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    pCache->u16ReqVolt = u16ReqVolt;
    return true;
}








/***************************************************************************************************
* Description                           :   设置需求电流，单位：1A
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSetReqCurr(ePileGunIndex_t eGunIndex, u16 u16ReqCurr)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    pCache->u16ReqCurr = u16ReqCurr;
    return true;
}









/***************************************************************************************************
* Description                           :   获取PCU通讯状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-24
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetCommSt(ePileGunIndex_t eGunIndex, ePcuCommState_t *pCommSt)
{
    if(pCommSt != NULL)
    {
        *pCommSt = pPcuInfoCache[eGunIndex]->ePcuCommState;
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   获取PCU能量传输状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetEnergyErrSt(ePileGunIndex_t eGunIndex)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    return (pCache->bPcuEnergyErr);
}











/***************************************************************************************************
* Description                           :   获取需求电压，单位：1V
* Author                                :   Dai
* Creat Date                            :   2024-11-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetReqVolt(ePileGunIndex_t eGunIndex, u16* u16ReqVolt)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    *u16ReqVolt = pCache->u16ReqVolt;
    return true;
}








/***************************************************************************************************
* Description                           :   获取需求电流，单位：1A
* Author                                :   Dai
* Creat Date                            :   2024-11-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuGetReqCurr(ePileGunIndex_t eGunIndex, u16* u16ReqCurr)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    *u16ReqCurr = pCache->u16ReqCurr;
    return true;
}













/***************************************************************************************************
* Description                           :   与pcu通讯主任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
void sModAppPcuTask(void *pParam)
{
    ePileGunIndex_t eGunIndex = 0;
    
    EN_SLOGI(TAG, "任务建立:模块电源管理用户程序,对PCU通讯主任务");
    while(1)
    {
        //与PCU通讯处理方法
        for(eGunIndex = 0; eGunIndex < cPrivDrvGunNumMax; eGunIndex++)
        {
            sModAppPcuRecvDecode();
            sModAppPcuMainLoop(eGunIndex);
        }
        
        
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}






/***************************************************************************************************
* Description                           :   与pcu通讯主循环
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuMainLoop(ePileGunIndex_t eGunIndex)
{
    static u16 u16VoltInOld[ePileGunIndexNum];
    static u16 u16OldVolt[ePileGunIndexNum] = {0};
    static u16 u16OldCurr[ePileGunIndexNum] = {0};
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    //1.刷新缓存区必要参数
    sModAppPcuFlushCacheData(eGunIndex);
    
    //2.上电注册
    if(pCache->eRegisterResult != eRegisterSuccess)
    {
        //如果注册失败原因是重复注册，则认为已经注册成功（用于掉电后又快速上电情况）
        if(pCache->eRegisterResult == eRegisterRepeat)
        {
            //心跳超时计数清零
            pCache->u16CommFailCnt = 0;
            pCache->eRegisterResult = eRegisterSuccess;
            return true;
        }
        else if(pCache->eRegisterResult == eRegisterOccup)
        {
            //通知TCU，枪编号冲突
            
        }
        else if(pCache->eRegisterResult == eRegisterPcuErr)
        {
            //心跳超时计数清零
            pCache->u16CommFailCnt = 0;
            pCache->ePcuCommState = ePcuCommStateNormal;
            memset(&pCache->stBcpReqParam, 0, sizeof(pCache->stBcpReqParam));
        }
        
        //1000ms周期发送注册指令
        if(xTaskGetTickCount() - (pCache->stTaskTick.u32InitTick) >= 1000)
        {
            pCache->stTaskTick.u32InitTick = xTaskGetTickCount();
            sModAppPcuSendRegister(eGunIndex);
            u16OldVolt[eGunIndex] = 0;
            u16OldCurr[eGunIndex] = 0;
        }
        
        return false;
    }
    
    //3.发送心跳 3000ms
    if(xTaskGetTickCount() - (pCache->stTaskTick.u32HeartTick) >= 3000)
    {
        pCache->stTaskTick.u32HeartTick = xTaskGetTickCount();
        sModAppPcuSendHeartPack(eGunIndex);
    }
    
    //4.需求有变化就发送功率申请，避免频繁发送总线占用率高
    if((u16OldVolt[eGunIndex] != pCache->u16ReqVolt) || (u16OldCurr[eGunIndex] != pCache->u16ReqCurr))
    {
        u16OldVolt[eGunIndex] = pCache->u16ReqVolt;
        u16OldCurr[eGunIndex] = pCache->u16ReqCurr;
        sModAppPcuSendEnergyReq(eGunIndex);
    }
    
    //5.内侧电压变化大于5V，发送当前内侧电压
    if(abs(pCache->u16VoltIn - u16VoltInOld[eGunIndex]) >= 5)
    {
        u16VoltInOld[eGunIndex] = pCache->u16VoltIn;
        sModAppPcuSendVoltChange(eGunIndex);
    }
    
    //5.PCU掉线检测
    if(pCache->u16CommFailCnt > cPcuHeartCommFaultCnt)
    {
        pCache->ePcuCommState = ePcuCommStateFailed;
        pCache->eRegisterResult = eRegistering;
        pCache->u16AvailVolt = 0;
        pCache->u16AvailCurr = 0;
        pCache->u16CurVolt = 0;
        memset(&pCache->stBcpReqParam, 0, sizeof(pCache->stBcpReqParam));
    }
    else
    {
        pCache->ePcuCommState = ePcuCommStateNormal;
    }
    
    return true;
}








/***************************************************************************************************
* Description                           :   刷新缓存区数据
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuFlushCacheData(ePileGunIndex_t eGunIndex)
{
    u16 u16Temp = 0;
    f32 f32Temp = 0;
    char u8Sn[cPrivDrvLenSn];
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    static stBcpReqParam_t stLastBcpReqParam[ePileGunIndexNum] = {0};
    
    //1.刷新枪编号
    //2.刷新需求电压，需求电流，这里的倍比需要转换一下，可能会存在精度丢失
    pCache->u32RatePower = 40000;
    
    
    //3.刷新Bcp报文的一些参数(充电结束阶段填0，供pcu投切模块)
    if((sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFinish) || (sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultAB))
    {
         pCache->stBcpReqParam.u16MaxChgVolt = 0;
         pCache->stBcpReqParam.u16MaxChgCurr = 0;
    }
    else
    {
        sBmsGbt15Get_BcpMaxChgVol(eGunIndex, &u16Temp);
        if(u16Temp != stLastBcpReqParam[eGunIndex].u16MaxChgVolt)
        {
            stLastBcpReqParam[eGunIndex].u16MaxChgVolt = u16Temp;
            //根据协议这里的倍比需要转换一下，可能会存在精度丢失
            pCache->stBcpReqParam.u16MaxChgVolt = u16Temp / 10;
        }
        
        sBmsGbt15Get_BcpMaxChgCur(eGunIndex, &u16Temp);
        if(u16Temp != stLastBcpReqParam[eGunIndex].u16MaxChgCurr)
        {
            stLastBcpReqParam[eGunIndex].u16MaxChgCurr = u16Temp;
            //根据协议这里的倍比需要转换一下，可能会存在精度丢失
            pCache->stBcpReqParam.u16MaxChgCurr = u16Temp / 10;
        }
    }
    
    //4.刷新直流母线输出电压
    if(eGunIndex == 0)
    {
        sAdcAppSampleGetData(eAdcPin_A_GUN_DC_P, &f32Temp);
    }
    else
    {
        sAdcAppSampleGetData(eAdcPin_B_GUN_DC_P, &f32Temp);
    }
    pCache->u16BusVolt = (u16)f32Temp;
    
    //5.刷新枪内测电压
    sMeterDrvOptGetUdcIdcPdc(eGunIndex, &f32Temp, NULL, NULL);
    pCache->u16VoltIn = (u16)f32Temp;
    
    //6.刷新Uid的值，来源于桩Sn码第3、4字段
    sEepromAppGetBlockFactorySn(u8Sn, sizeof(u8Sn));
    sStr2Bcd(pCache->u8Uid, (u8 *)(u8Sn + 5), cPcuUidLength);
    
    
    return true;
}










/***************************************************************************************************
* Description                           :   与pcu通讯主任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuRecvDecode(void)
{
    u8   i;
    u8   u8Pgn;
    u8   u8Src, u8Dst;
    bool bFind = false;
    ePileGunIndex_t eGunIndex;
    stCanMsg_t stRxMsg;
    
    
    //调用底层can-api读取报文
    if(sCanDrvRecv(cPcuCanPort, &stRxMsg) == true)
    {
        u8Pgn = (stRxMsg.u32Id >> 16) & 0xFF;
        u8Dst = (stRxMsg.u32Id >> 8) & 0xFF;
        u8Src = stRxMsg.u32Id & 0xFF;
        
        //不是pcu发出来的数据不解析
        if(u8Src != cPcuAddress)
        {
            return false;
        }
        
        //不是广播地址，也不是发给当前桩的数据不解析
        for(i = 0; i < ePileGunIndexNum; i++)
        {
            //遍历是否找到匹配的枪号
            if(pPcuInfoCache[i]->u8GunAddr == u8Dst)
            {
                eGunIndex = i;
                bFind = true;
                
                break;
            }
        }
        
        if((u8Dst != cBroadcast) && (bFind == false))
        {
            return false;
        }
        
        
        switch(u8Pgn)
        {
        case ePgnCodeRegisterResp:
            sModAppPcuRecvRegisterResp(eGunIndex, stRxMsg.u8Data);
            break;
        case ePgnCodeHeartBetResp:
            sModAppPcuRecvHeartPackResp(eGunIndex, stRxMsg.u8Data);
            break;
        case ePgnCodeEnergyResp:
            break;
        case ePgnCodePcuErrCode:
            break;
        default:
            break;
        }
    }
    
    return true;
}








/***************************************************************************************************
* Description                           :   获取发送给pcu报文的canId
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-22
* notice                                :   
****************************************************************************************************/
u32 sModAppPcuGetMsgCanId(u8 u8GunNum, ePcuPgnCode_t ePgn)
{
    unPcuCanId_t unCanId;
    
    unCanId.u32Value = 0;
    
    //枪编号
    unCanId.stValue.bAddSrc             = u8GunNum;
    //pcu地址
    unCanId.stValue.bAddDst             = cPcuAddress;
    //PGN码
    unCanId.stValue.bPgn                = ePgn;
    
    //bit29 数据帧
    unCanId.stValue.ebit29              = eBmsGbt15CanIdBit29DataFrame;
    //bit30 
    unCanId.stValue.ebit30              = eBmsGbt15CanIdBit30Data;
    //bit31 扩展帧
    unCanId.stValue.ebit31              = eBmsGbt15CanIdBit31Extended;
    
    
    switch(ePgn)
    {
    case ePgnCodeRegister:
        unCanId.stValue.bPri        = 6;
        
        break;
    case ePgnCodeHeartBeat:
        unCanId.stValue.bPri        = 4;
        
        break;
    case ePgnCodeEnergyReq:
        unCanId.stValue.bPri        = 5;
        
        break;
    default:
        break;
    }
    
    return unCanId.u32Value;
}








/***************************************************************************************************
* Description                           :   向PCU发送枪号注册
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSendRegister(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sModAppPcuGetMsgCanId(pCache->u8GunAddr, ePgnCodeRegister);
    stTxMsg.u8Len = 4;
    
    //桩出厂时间
    stTxMsg.u8Data[0] = pCache->u8Uid[0];
    stTxMsg.u8Data[1] = pCache->u8Uid[1];
    
    //生产批次号
    stTxMsg.u8Data[2] = pCache->u8Uid[2];
    stTxMsg.u8Data[3] = pCache->u8Uid[3];
    
    
    //2.将组好的包发出去
    sCanDrvSend(pCache->i32Port, &stTxMsg);
    
    return true;
}








/***************************************************************************************************
* Description                           :   向PCU发送枪号心跳
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSendHeartPack(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sModAppPcuGetMsgCanId(pCache->u8GunAddr, ePgnCodeHeartBeat);
    stTxMsg.u8Len = 6;
    
    //最大允许充电电压
    stTxMsg.u8Data[0] = (pCache->stBcpReqParam.u16MaxChgVolt) >> 8;
    stTxMsg.u8Data[1] = (pCache->stBcpReqParam.u16MaxChgVolt) & 0xFF;
    //最大允许充电电流
    stTxMsg.u8Data[2] = (pCache->stBcpReqParam.u16MaxChgCurr) >> 8;
    stTxMsg.u8Data[3] = (pCache->stBcpReqParam.u16MaxChgCurr) & 0xFF;
    
    //当前插枪状态
    stTxMsg.u8Data[4] = ((sAdcAppSampleGetCc1State(eGunIndex) == eAdcAppCc1StateU1c) ? 1 : 0);
    //枪类型 1-液冷枪
    stTxMsg.u8Data[5] = ((pCache->bIsSuperChgGun == true) ? 1 : 0);
    
    //2.将组好的包发出去
    sCanDrvSend(pCache->i32Port, &stTxMsg);
    (pCache->u16CommFailCnt)++;
    
    
    return true;
}







/***************************************************************************************************
* Description                           :   向PCU发送枪号能量请求
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSendEnergyReq(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sModAppPcuGetMsgCanId(pCache->u8GunAddr, ePgnCodeEnergyReq);
    stTxMsg.u8Len = 5;
    
    
    //需求电压
    stTxMsg.u8Data[0] = (pCache->u16ReqVolt) >> 8;
    stTxMsg.u8Data[1] = (pCache->u16ReqVolt) & 0xFF;
    //需求电流
    stTxMsg.u8Data[2] = (pCache->u16ReqCurr) >> 8;
    stTxMsg.u8Data[3] = (pCache->u16ReqCurr) & 0xFF;
    //充电优先级
    stTxMsg.u8Data[4] = pCache->u8ChgPrio;
    
    //2.将组好的包发出去
    sCanDrvSend(pCache->i32Port, &stTxMsg);
    EN_SLOGI(TAG, "向Pcu发送国标%d枪输出申请，电压：%04dV 电流：%04dA", eGunIndex, pCache->u16ReqVolt, pCache->u16ReqCurr);
    
    return true;
}






/***************************************************************************************************
* Description                           :   向PCU发送枪号内侧电压变化
* Author                                :   Dai
* Creat Date                            :   2024-10-14
* notice                                :   
****************************************************************************************************/
bool sModAppPcuSendVoltChange(ePileGunIndex_t eGunIndex)
{
    stCanMsg_t stTxMsg;
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    memset(&stTxMsg, 0, sizeof(stCanMsg_t));
    
    //1.组帧
    stTxMsg.u32Id = sModAppPcuGetMsgCanId(pCache->u8GunAddr, ePgnCodeVoltNotify);
    stTxMsg.u8Len = 3;
    
    //枪内侧电压
    stTxMsg.u8Data[0] = (pCache->u16VoltIn) >> 8;
    stTxMsg.u8Data[1] = (pCache->u16VoltIn) & 0xFF;
    
    //2.将组好的包发出去
    sCanDrvSend(pCache->i32Port, &stTxMsg);
    EN_SLOGI(TAG, "向Pcu发送国标%d枪内侧电压变化，电压：%04dV", eGunIndex, pCache->u16VoltIn);
    
    return true;
}







/***************************************************************************************************
* Description                           :   注册响应解包
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuRecvRegisterResp(ePileGunIndex_t eGunIndex, u8 *pMsg)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    if(pMsg == NULL || pCache == NULL)
    {
        return false;
    }
    
    
    pCache->eRegisterResult = pMsg[0];
    
    return true;
}








/***************************************************************************************************
* Description                           :   心跳响应解包
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
bool sModAppPcuRecvHeartPackResp(ePileGunIndex_t eGunIndex, u8 *pMsg)
{
    stModAppPcuInfo_t *pCache = pPcuInfoCache[eGunIndex];
    
    if(pMsg == NULL || pCache == NULL)
    {
        return false;
    }
    
    //可提供的电压
    pCache->u16AvailVolt = (u16)(pMsg[0] << 8) | (u16)(pMsg[1]);
    //可提供的电流
    pCache->u16AvailCurr = (u16)(pMsg[2] << 8) | (u16)(pMsg[3]);
    //可提供功率
    pCache->u16AvailPdc = (u16)(pMsg[4] << 8) | (u16)(pMsg[5]);
    //Pcu当前输出电压，单位：10V
    pCache->u16CurVolt = pMsg[6] * 10;
    //Pcu是否存在故障
    pCache->bPcuEnergyErr = ((pMsg[7] == 1) ? true: false);
    
    //心跳超时计数清零
    pCache->u16CommFailCnt = 0;
    
    return true;
}



































