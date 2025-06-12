/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate";




extern stPileCnCache_t *pPileCnCache;








void sPileCnGateLoop   (ePileGunIndex_t eGunIndex);
void sPileCnGateInit   (ePileGunIndex_t eGunIndex);
void sPileCnGateRun    (ePileGunIndex_t eGunIndex);
void sPileCnGateRun4Chg(ePileGunIndex_t eGunIndex);
void sPileCnGateTimeSyn(void);
void sPileCnGateLocal  (ePileGunIndex_t eGunIndex);
void sPileCnGateRmt    (ePileGunIndex_t eGunIndex);
void sPileCnGateRcdRpt (ePileGunIndex_t eGunIndex);
void sPileCnGateParam  (ePileGunIndex_t eGunIndex);
void sPileCnGateOta    (void);
void sPileCnGateBmsRpt (ePileGunIndex_t eGunIndex);
void sPileCnGateCoolRpt(ePileGunIndex_t eGunIndex);




ePrivDrvGunConnStatus_t sPileCnCc1StateToGunConnStatue(eAdcAppCc1State_t eState);





bool sPileCnGateSetGunStsReason(ePileGunIndex_t eGunIndex);

bool sPileCnGateClrStopReason(ePileGunIndex_t eGunIndex);

bool sPileCnGateMakeStopReason(ePileGunIndex_t eGunIndex, ePrivDrvStopReason_t eReason);
bool sPileCnGateMakeGunStsReason(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStsRsn_t eReason);











/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGateLoop(ePileGunIndex_t eGunIndex)
{
    //网关通讯数据初始化
    sPileCnGateInit(eGunIndex);
    
    //网关通讯数据持续更新
    sPileCnGateRun(eGunIndex);
    sPileCnGateRun4Chg(eGunIndex);
    
    //对时
    sPileCnGateTimeSyn();
    
    //远程控制命令交互
    sPileCnGateRmt(eGunIndex);
    
    //刷卡交互
    sPileCnGateCardFind(eGunIndex);
    sPileCnGateCardAuth(eGunIndex);
    sPileCnGateCardAuthAck(eGunIndex);
    
    //VIN码充电
    sPileCnGateVinAuth(eGunIndex);
    sPileCnGateVinAuthAck(eGunIndex);
    
    //充电记录交互
    sPileCnGateRcdRpt(eGunIndex);
    
    //参数管理
    sPileCnGateParam(eGunIndex);
    
    //OTA升级
    sPileCnGateOta();
    
    //bms数据交互
    sPileCnGateBmsRpt(eGunIndex);
    
    //液冷机信息上报
#if defined(DC480K_G2SF311_600)
    sPileCnGateCoolRpt(eGunIndex);
#endif
}







/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   部分（不可变）字段上电初始化赋值
****************************************************************************************************/
void sPileCnGateInit(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(pData->bInitFlag != true)
    {
        pData->bInitFlag = true;
        
        sPrivDrvSetChargerType(sEepromAppGetBlockFactoryChargerType());
        sPrivDrvSetPayAt(sEepromAppGetBlockFactoryPayAt());
        sPrivDrvSetMoneyCalcBy(sEepromAppGetBlockFactoryMoneyCalcBy());
        sPrivDrvSetStartPowerMode(sEepromAppGetBlockFactoryPowerMode());
        sPrivDrvSetWorkMode(sEepromAppGetBlockFactoryWorkMode());
        sPrivDrvSetBalancePhase(sEepromAppGetBlockFactoryBalancePhase());
        sPrivDrvSetPowerMax((i32)sEepromAppGetBlockFactoryPowerMax());
        sPrivDrvSetCurrMax((i32)(sEepromAppGetBlockFactoryCurrMax() * 10));
        sPrivDrvSetPowerRated((i32)sEepromAppGetBlockFactoryPowerRated());
        sPrivDrvSetCurrRated((i32)(sEepromAppGetBlockFactoryCurrRated() * 10));
        sPrivDrvSetBookingViaBle(sEepromAppGetBlockFactoryBookingViaBle());
        sPrivDrvSetRebootReason(sEepromAppGetBlockFactoryRebootReason());
        
        sPrivDrvSetHwVer(strlen("V1.0"), (u8 *)"V1.0");
        sPrivDrvSetSwVer(strlen(cAppVersion), (u8 *)cAppVersion);
        sPrivDrvSetGunNum(cPrivDrvGunNumMax);
        pData->eCc1StateOld[eGunIndex] = sAdcAppSampleGetCc1State(eGunIndex);
        sPrivDrvSetRtRptType(sPrivDrvGetGunId(eGunIndex), ePrivDrvRtDataRptTypeCharging);
    }
}







/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 持续运行
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   部分（可变）字段需要持续更新并赋值
****************************************************************************************************/
void sPileCnGateRun(ePileGunIndex_t eGunIndex)
{
    u8  u8GunId;                                                                //内网驱动api函数的枪id参数 u8GunId
    u8  u8Buf[48];
    f32 f32Value1, f32Value2, f32Value3;
    stPrivDrvPhaseData_t   stPhaseData;
    stPrivDrvPhaseDataI_t  stPhaseDataI;
    stPrivDrvGunTempData_t stGunTemp;
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcd_t *pRcd = NULL;
    stPrivDrvFaultCode_t stFault;
    
    sPrivDrvSetDisableFlag(sEepromAppGetBlockFactoryDisableFlag());             //禁用标志
    sPrivDrvSetLoginedFlag(pData->eLoginedFlag);                                //登录标志
    
    //序列号
    sEepromAppGetBlockFactorySn((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetSn(strlen((char *)u8Buf), u8Buf);
    
    //DevCode
    sEepromAppGetBlockFactoryDevCode((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetDevCode(strlen((char *)u8Buf), u8Buf);
    
    //DevName
    sEepromAppGetBlockFactoryDevName((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetDevName(strlen((char *)u8Buf), u8Buf);
    
    
    //枪信息
    u8GunId = sPrivDrvGetGunId(eGunIndex);
    
    //故障码
    sFaultCodeGet(u8GunId, &stFault);
    sPrivDrvSetFaultCode(u8GunId, &stFault);
    
    
    //------------------------------cmd_03_04-------------------------------
    sPrivDrvSetGunWorkStatus(u8GunId, sPileCnStateGet(eGunIndex));
    sPrivDrvSetGunConnStatus(u8GunId, sPileCnCc1StateToGunConnStatue(sAdcAppSampleGetCc1State(eGunIndex)));
    sPrivDrvSetGunLockStatus(u8GunId, pPileCnCache->stChgInfo[eGunIndex].eElockState);
    
    //枪事件
    if((pData->eCc1StateOld[eGunIndex] == eAdcAppCc1StateU1c) && (sAdcAppSampleGetCc1State(eGunIndex) != eAdcAppCc1StateU1c))
    {
        sPrivDrvSetGunEvent(u8GunId, ePrivDrvGunEventPlugOut);
    }
    else if((sAdcAppSampleGetCc1State(eGunIndex) == eAdcAppCc1StateU1c) && (pData->eCc1StateOld[eGunIndex] != eAdcAppCc1StateU1c))
    {
        sPrivDrvSetGunEvent(u8GunId, ePrivDrvGunEventPlugIn);
    }
    pData->eCc1StateOld[eGunIndex] = sAdcAppSampleGetCc1State(eGunIndex);
    
    
    //直流输出电压
    sMeterDrvOptGetUdcIdcPdc(eGunIndex, &f32Value1, &f32Value2, &f32Value3);
    stPhaseData.i16DataA = (i16)(f32Value1 * 10);
    stPhaseData.i16DataB = 0;
    stPhaseData.i16DataC = 0;
    sPrivDrvSetUacOut(u8GunId, &stPhaseData);
    
    //直流输出电流
    stPhaseDataI.i32DataA = (i16)(f32Value2 * 100);
    stPhaseDataI.i32DataB = 0;
    stPhaseDataI.i32DataC = 0;
    sPrivDrvSetIacOut(u8GunId, &stPhaseDataI);
    
    //CP/CC1电压
    sPrivDrvSetUcp(u8GunId, (i16)(sAdcAppSampleGetUcp (eGunIndex) * 100));
    sPrivDrvSetUcc(u8GunId, (i16)(sAdcAppSampleGetUcc1(eGunIndex) * 100));
    
    //枪温度
    memset(&stGunTemp, 0, sizeof(stGunTemp));
    sAdcAppSampleGetGunTemp(eGunIndex, &f32Value1, &f32Value2);
    stGunTemp.i16Gun = (i16)(f32Value1 * 10);
    stGunTemp.i16Data1 = (i16)(f32Value2 * 10);
    sPrivDrvSetGunTemp(u8GunId, &stGunTemp);
    
    //------------------------------cmd_08_0E----------------------------------
    if(sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusCharging)
    {
        pRcd = &pData->stNewRcd[eGunIndex];
        sPrivDrvSetChargingPower(u8GunId, (u32)f32Value3);
        sPrivDrvSetRtRpt(u8GunId, &pRcd->stRcdData);
    }
    sPrivDrvSetYcRpt(u8GunId);
}








/***************************************************************************************************
* Description                           :   枪数据持续填充时 顺带填充一下桩的数据
* Author                                :   Hall
* Creat Date                            :   2024-07-04
* notice                                :   桩需要上报03报文，需要填充里面的故障告警码和桩工作状态字段
*
*                                           根据约定：
*                                           故障告警码----仅填充关于整桩的故障告警码
*                                           桩工作状态----仅填充 Available，Unavailable，Faulted 这三个状态
****************************************************************************************************/
void sPileCnGateRun4Chg(ePileGunIndex_t eGunIndex)
{
    u8  i;
    ePrivDrvGunWorkStatus_t eStatus;
    stPrivDrvFaultCode_t stFault;
    
    //0:桩工作状态先赋初值---可用状态
    eStatus = ePrivDrvGunWorkStatusAvailable;
    
    
    //1:只要有一把枪是禁用状态，就改为禁用状态
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        if(sPileCnStateGet(i) == ePrivDrvGunWorkStatusUnavailable)
        {
            break;
        }
    }
    if(i != ePileGunIndexNum)
    {
        eStatus = ePrivDrvGunWorkStatusUnavailable;
    }
    
    
    //2:所有枪都是故障态，再改为故障态---故障态要优先显示
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        //只要枪在充电中或者进入充电中的状态不能给整桩可用状态
        if((sPileCnStateGet(i) == ePrivDrvGunWorkStatusCharging) || 
        (sPileCnStateGet(i) == ePrivDrvGunWorkStatusSuspendedByEvse) ||
        (sPileCnStateGet(i) == ePrivDrvGunWorkStatusSuspendedByEv))
        {

            return;
        }

        if(sPileCnStateGet(i) != ePrivDrvGunWorkStatusFaulted)
        {
            break;
        }
    }
    if(i == ePileGunIndexNum)
    {
        eStatus = ePrivDrvGunWorkStatusFaulted;
    }
    sPrivDrvSetGunWorkStatus(cPrivDrvGunIdChg, eStatus);
    
    
    //3:填充桩故障码
    sFaultCodeGet(cPrivDrvGunIdChg, &stFault);//顺便把桩的故障告警码填充一下
    sPrivDrvSetFaultCode(cPrivDrvGunIdChg, &stFault);
}







/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 对时处理
* Author                                :   Hall
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
void sPileCnGateTimeSyn(void)
{
    bool bRst;
    i32  i32DeltaTime;
    stTime_t stGateTime, stChgTime;
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(sPrivDrvGetMatserTime(&stGateTime) == true)
    {
        stChgTime = sGetTime();
        i32DeltaTime = stGateTime.sec;
        i32DeltaTime = i32DeltaTime - stGateTime.sec;
        if((abs(i32DeltaTime) >= 5)                                             //误差超过5秒就对时
        || (stGateTime.year != stChgTime.year)
        || (stGateTime.mon  != stChgTime.mon )
        || (stGateTime.day  != stChgTime.day )
        || (stGateTime.hour != stChgTime.hour)
        || (stGateTime.min  != stChgTime.min ))
        {
            EN_SLOGI(TAG, "桩端时间:%d-%02d-%02d,%02d:%02d:%02d", stChgTime.year,  stChgTime.mon,  stChgTime.day,  stChgTime.hour,  stChgTime.min,  stChgTime.sec );
            EN_SLOGI(TAG, "网关时间:%d-%02d-%02d,%02d:%02d:%02d", stGateTime.year, stGateTime.mon, stGateTime.day, stGateTime.hour, stGateTime.min, stGateTime.sec);
            
            sSetTime(&stGateTime);
        }
    }
}








/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 本地启动逻辑
* Author                                :   Hall
* Creat Date                            :   2024-06-08
* notice                                :   刷卡启动、VIN码启动...等等
****************************************************************************************************/
void sPileCnGateLocal(ePileGunIndex_t eGunIndex)
{
    stCardData_t *pCard = NULL;
    if((pCard = sCardAppGetData()) != NULL)
    {
        
    }
}








/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 远程操作检查及执行
* Author                                :   Hall
* Creat Date                            :   2024-05-29
* notice                                :   检查是否收到网关的远程操作命令 并执行
****************************************************************************************************/
void sPileCnGateRmt(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    
    if(sPrivDrvGetRmt(u8GunId, &pData->pRmt[eGunIndex]) == true)
    {
        switch(pData->pRmt[eGunIndex]->eCmd)
        {
        case ePrivDrvRmtCmdNone:
            break;
        case ePrivDrvRmtCmdUnlock:
             sPileCnGateRmtUnlock(eGunIndex);
            break;
        case ePrivDrvRmtCmdLock:
            break;
        case ePrivDrvRmtCmdStart:
            sPileCnGateRmtStart(eGunIndex);
            break;
        case ePrivDrvRmtCmdStop:
            sPileCnGateRmtStop(eGunIndex);
            break;
        case ePrivDrvRmtCmdReserve:
            break;
        case ePrivDrvRmtCmdUnreserve:
            break;
        default:
            break;
        }
    }
}








/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 充电记录对网关上报
* Author                                :   Hall
* Creat Date                            :   2024-05-29
* notice                                :   
****************************************************************************************************/
void sPileCnGateRcdRpt(ePileGunIndex_t eGunIndex)
{
    sPileCnGateNewRcdRpt(eGunIndex);
    sPileCnGateOldRcdRpt(eGunIndex);
    sPileCnGateRptAck(eGunIndex);
}









/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 参数管理命令处理
* Author                                :   Hall
* Creat Date                            :   2024-05-29
* notice                                :   检查是否收到网关的参数管理命令 并执行
****************************************************************************************************/
void sPileCnGateParam(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    static bool bIsoFlag[ePileGunIndexNum];
    f32 f32IsoOhm;
    u32 u32IsoOhm;
    
    stPrivDrvCmdAA_t *pCmdAaRx = NULL;
    stPrivDrvCmdAA_t *pCmdAa   = &pData->stCmdAa[eGunIndex];
    
    sIsoGetOhm(eGunIndex, &f32IsoOhm);
    u32IsoOhm = (u32)f32IsoOhm;
    
    if(sPrivDrvGetCmdParam(true, &pCmdAaRx) == true)
    {
        memset(pCmdAa, 0,        sizeof(stPrivDrvCmdAA_t));
        memcpy(pCmdAa, pCmdAaRx, sizeof(stPrivDrvCmdAA_t));
        
        sPileCnGateParamProc(pCmdAa);
    }
    
    //新增桩端主动上报绝缘电阻
    if((sBmsGbt15StateGet(eGunIndex) > eBmsGbt15StateCableCheck) && (bIsoFlag[eGunIndex] == true))
    {
       bIsoFlag[eGunIndex] = false;
       sPrivDrvSetCmdParam(true, u8GunId, ePrivDrvCfgCmdTypeChargSet, ePrivDrvCmdRstSuccess, ePrivDrvParamAddr202, 4, (u8*)&u32IsoOhm, 0);
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateWait)
    {
        bIsoFlag[eGunIndex] = true;
    }
}







/*******************************************************************************
 * @FunctionName   :      sPileCnGateOta
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  15:04:05
 * @Description    :      国标充电桩用户程序 之 网关用户程序 之 CCU OTA升级
 * @Input          :      void        
*******************************************************************************/
void sPileCnGateOta(void)
{
    bool bRst;
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(sPrivDrvGetChgOta(&pData->pOta) == true)
    {
        if(pData->pOta->eCmd == ePrivDrvUpdateCmdRebootBoot)
        {
            EN_SLOGI(TAG, "远程升级命令, 进入boot模式!!!");
            sPileCnGateOtaGetInBoot();
        }
    }
}








/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 bms数据上报
* Author                                :   Hall
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
void sPileCnGateBmsRpt(ePileGunIndex_t eGunIndex)
{
    i32 i;
    u8  u8GunId;                                                                //内网驱动api函数的枪id参数 u8GunId
    
    
    stPileCnGateData_t  *pData      = &pPileCnCache->stGateData;
    stPrivDrvCmd13_t    *pBmsData   = &pData->stBmsData[eGunIndex];
    stBmsGbt15Evcc_t    *pEvccData  = &pData->stEvccData[eGunIndex];
    
    if(sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusCharging)
    {
        memset(pBmsData, 0,sizeof(stPrivDrvCmd13_t));
        sBmsGbt15Get_EvccCacheData(eGunIndex, pEvccData);
        
        //常规数据
        u8GunId = sPrivDrvGetGunId(eGunIndex);
        pBmsData->u16Seq = pData->u16Seq[eGunIndex]++;
        pBmsData->u8GunId = u8GunId;
        sEepromAppGetBlockFactorySn((char *)pBmsData->u8Sn, sizeof(pBmsData->u8Sn));
        pBmsData->eWorkStatus = sPileCnStateGet(eGunIndex);
        pBmsData->eConnStatus = sPileCnCc1StateToGunConnStatue(sAdcAppSampleGetCc1State(eGunIndex));
        
        //bms数据--->brm
        memcpy(pBmsData->stBmsInfo.u8BmsVersion, pEvccData->stBrmData.u8Protver, sizeof(pEvccData->stBrmData.u8Protver));
        pBmsData->stBmsInfo.eBatteryType = (ePrioDrvBatType_t)pEvccData->stBrmData.eBatType;
        pBmsData->stBmsInfo.u32BatteryCap = pEvccData->stBrmData.u16BatRateCap;
        pBmsData->stBmsInfo.u32BatteryVolt = pEvccData->stBrmData.u16BatRateVol;
        memcpy(pBmsData->stBmsInfo.u8ManuName, pEvccData->stBrmData.u8BatFactName, sizeof(pEvccData->stBrmData.u8BatFactName));
        memcpy(pBmsData->stBmsInfo.u8BatteryIndex, pEvccData->stBrmData.u8BatSerial, sizeof(pEvccData->stBrmData.u8BatSerial));
        pBmsData->stBmsInfo.u16ManuYear = pEvccData->stBrmData.u8BatYear + cBatYearBase;
        pBmsData->stBmsInfo.u8ManuMonth = pEvccData->stBrmData.u8BatMonth;
        pBmsData->stBmsInfo.u8ManuDay = pEvccData->stBrmData.u8BatDay;
        pBmsData->stBmsInfo.u32ChargeCnt = pEvccData->stBrmData.u32BatChgCount;
        pBmsData->stBmsInfo.u8BatteryProperty = pEvccData->stBrmData.eBatProt;
        memcpy(pBmsData->stBmsInfo.u8CarVin, pEvccData->stBrmData.u8Vin, sizeof(pEvccData->stBrmData.u8Vin));
        memcpy(pBmsData->stBmsInfo.u8BmsSwVer, pEvccData->stBrmData.u8BmsSoftVer, sizeof(pEvccData->stBrmData.u8BmsSoftVer));
        for(i = 0; i < sizeof(pBmsData->stBmsInfo.u8CarVin); i++)
        {
            //一些车子上报的VIN码字段是全FF，这种值上报给网关再转成JSON发给平台会造成TCU被踢下线
            //这里对这种值做一下屏蔽
            if(pBmsData->stBmsInfo.u8CarVin[i] == 0xff)
            {
                memset(pBmsData->stBmsInfo.u8CarVin, 0, sizeof(pBmsData->stBmsInfo.u8CarVin));
                break;
            }
        }
        
        //bms数据--->bcp
        pBmsData->stBmsInfo.u32CellVoltMax = pEvccData->stBcpData.u16CellMaxVol;
        pBmsData->stBmsInfo.u32BatteryCurrMax = pEvccData->stBcpData.u16AllowMaxCur;
        pBmsData->stBmsInfo.u32BcpBatteryCap = pEvccData->stBcpData.u16BatteryCap;
        pBmsData->stBmsInfo.u32BatteryVoltMax = pEvccData->stBcpData.u16AllowMaxVol;
        pBmsData->stBmsInfo.i8BatteryTempMax = pEvccData->stBcpData.u8AllowMaxTemp;
        pBmsData->stBmsInfo.u16BatteryCapStatus = pEvccData->stBcpData.u16BatterySoc;
        pBmsData->stBmsInfo.u32BcpBatteryVolt = pEvccData->stBcpData.u16CurBatVol;
        
        //bms数据--->bro
        pBmsData->stBmsInfo.u8Ready = pEvccData->stBrodata.eReady;
        
        //bms数据--->bcl
        pBmsData->stBmsInfo.u32BmsVolt = pEvccData->stBclData.u16ReqVol;
        pBmsData->stBmsInfo.u32BmsCurr = pEvccData->stBclData.u16ReqCur;
        pBmsData->stBmsInfo.u8BmsMode = pEvccData->stBclData.u8ChgMode;
        
        //bms数据--->bcs
        pBmsData->stBmsInfo.u32BcsBmsVolt = pEvccData->stBcsData.u16MeasureVol;
        pBmsData->stBmsInfo.u32BcsBmsCurr = pEvccData->stBcsData.u16MeasureCur;
        pBmsData->stBmsInfo.u32CellMax = pEvccData->stBcsData.stMaxCellInfo.u16MaxCellVol;
        pBmsData->stBmsInfo.u32CellCnt = pEvccData->stBcsData.stMaxCellInfo.u16OwnGroup;
        pBmsData->stBmsInfo.u16BmsSoc = pEvccData->stBcsData.u8CurSoc;
        pBmsData->stBmsInfo.u32TimeRemain = pEvccData->stBcsData.u16RemainChgTime;
        
        //bms数据--->bsm
        pBmsData->stBmsInfo.u8CellVoltMaxIndex = pEvccData->stBsmData.u8CellNum;
        pBmsData->stBmsInfo.u8BsmBatteryTempMax = pEvccData->stBsmData.u8CellMaxTemp;
        pBmsData->stBmsInfo.u8BatteryTempMaxIdx = pEvccData->stBsmData.u8MaxTempIdx;
        pBmsData->stBmsInfo.u8BatteryTempMin = pEvccData->stBsmData.u8CellMinTemp;
        pBmsData->stBmsInfo.u8BatteryTempMinIdx = pEvccData->stBsmData.u8MinTempIdx;
        pBmsData->stBmsInfo.u8BatteryOverHighLow = pEvccData->stBsmData.stBatFaultCode.bBatVolFault;
        pBmsData->stBmsInfo.u8SocOverHighLow = pEvccData->stBsmData.stBatFaultCode.bBatSocFault;
        pBmsData->stBmsInfo.u8BatteryChargOverCurr = pEvccData->stBsmData.stBatFaultCode.bBatCurFault;
        pBmsData->stBmsInfo.u8BatteryTempOverHigh = pEvccData->stBsmData.stBatFaultCode.bBatTempFault;
        pBmsData->stBmsInfo.u8BatteryInsSts = pEvccData->stBsmData.stBatFaultCode.bBatIsulstatus;
        pBmsData->stBmsInfo.u8BatteryConnectorSts = pEvccData->stBsmData.stBatFaultCode.bBatConnStatus;
        pBmsData->stBmsInfo.u8AllowCharg = pEvccData->stBsmData.stBatFaultCode.bAllowChg;
        
        //bms数据--->bst
        pBmsData->stBmsInfo.u8BmsStopSoc = pEvccData->stBstData.unStopReason.stReason.bSocFull;
        pBmsData->stBmsInfo.u8BmsStopTotalVolt = pEvccData->stBstData.unStopReason.stReason.bBatVolTarget;
        pBmsData->stBmsInfo.u8StopUnitVolt = pEvccData->stBstData.unStopReason.stReason.bCellVolTarget;
        pBmsData->stBmsInfo.u8StopChargerActively = pEvccData->stBstData.unStopReason.stReason.bSeccStop;
        pBmsData->stBmsInfo.u8FaultIns = pEvccData->stBstData.unStopFault.stFault.bInsulFault;
        pBmsData->stBmsInfo.u8FaultConnector = pEvccData->stBstData.unStopFault.stFault.bOutConntFault;
        pBmsData->stBmsInfo.u8FaultBmsConnector = pEvccData->stBstData.unStopFault.stFault.bComponConntFault;
        pBmsData->stBmsInfo.u8FaultChargConnecto = pEvccData->stBstData.unStopFault.stFault.bChgConntFault;
        pBmsData->stBmsInfo.u8FaultBatteryOverTemp = pEvccData->stBstData.unStopFault.stFault.bBatOverTemp;
        pBmsData->stBmsInfo.u8FaultHigtRelay = pEvccData->stBstData.unStopFault.stFault.bHighVolRelay;
        pBmsData->stBmsInfo.u8FaultCheck2 = pEvccData->stBstData.unStopFault.stFault.bCC2CheckFault;
        pBmsData->stBmsInfo.u8FaultOther = pEvccData->stBstData.unStopFault.stFault.bOtherFault;
        pBmsData->stBmsInfo.u8FaultCurr = pEvccData->stBstData.unStopErr.stErr.bOverCur;
        pBmsData->stBmsInfo.u8FaultVolt = pEvccData->stBstData.unStopErr.stErr.bVolAbnormal;
        
        //bms数据--->bsd
        pBmsData->stBmsInfo.u16StatisticSoc = pEvccData->stBsdData.u8StopSoc;
        pBmsData->stBmsInfo.u32StatisticVoltMin = pEvccData->stBsdData.u16MinCellVol;
        pBmsData->stBmsInfo.u32StatisticVoltMax = pEvccData->stBsdData.u16MaxCellVol;
        pBmsData->stBmsInfo.u8StatisticTempMin = pEvccData->stBsdData.u8MinBatTemp;
        pBmsData->stBmsInfo.u8StatisticTempMax = pEvccData->stBsdData.u8MaxBatTemp;
        
        //bms数据--->bem
        pBmsData->stBmsInfo.u8BemTimeOut00 = pEvccData->stBemData.unCode01.stBemCode01.bCrm00Timeout;
        pBmsData->stBmsInfo.u8BemTimeOutAA = pEvccData->stBemData.unCode01.stBemCode01.bCrmAATimeout;
        pBmsData->stBmsInfo.u8BemTimeOutSync = pEvccData->stBemData.unCode02.stBemCode02.bCmlTimeout;
        pBmsData->stBmsInfo.u8BemTimeOutCharging = pEvccData->stBemData.unCode02.stBemCode02.bCroTimeout;
        pBmsData->stBmsInfo.u8BemTimeOutChargSts = pEvccData->stBemData.unCode03.stBemCode03.bCcsTimeout;
        pBmsData->stBmsInfo.u8BemTimeOutChargEnd = pEvccData->stBemData.unCode03.stBemCode03.bCstTimeout;
        pBmsData->stBmsInfo.u8BemTimeOutChargStatistics = pEvccData->stBemData.unCode04.stBemCode04.bCsdTimeout;
        pBmsData->stBmsInfo.u8BemOther = 0;
        
        //模块电源数据---略
        
        
        
        sPrivDrvSetBmsRpt(u8GunId, pBmsData);
    }
}






#if defined(DC480K_G2SF311_600)
/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 液冷机数据上报
* Author                                :   Hall
* Creat Date                            :   2024-08-21
* notice                                :   
****************************************************************************************************/
void sPileCnGateCoolRpt(ePileGunIndex_t eGunIndex)
{
    u8  u8GunId;                                                                //内网驱动api函数的枪id参数 u8GunId
    f32 f32Value1, f32Value2, f32Value3;
    
    stPrivDrvCmd16_t stCoolingData;
    ePrivDrvCmdRst_t eRst = ePrivDrvCmdRstMax;
    stCoolingAppRealInfo_t *pInfo = NULL;
    stCoolingAppErrCode_t  stCode;
    
    static u32 u32TimeOld = 0;
    
    sPrivDrvGetLoginAck(&eRst);
    if((eRst == ePrivDrvCmdRstSuccess) && ((sGetTimestamp() - u32TimeOld) >= 2))
    {
        u32TimeOld = sGetTimestamp();
        if(sCoolingAppGetInfo(&pInfo) == true)
        {
            memset(&stCoolingData, 0, sizeof(stCoolingData));
            stCoolingData.u8GunId = sPrivDrvGetGunId(eGunIndex);
            sMeterDrvOptGetUdcIdcPdc(eGunIndex, &f32Value1, &f32Value2, &f32Value3);
            stCoolingData.i32IdcOut = f32Value2 * 100;
            sAdcAppSampleGetGunTemp(eGunIndex, &f32Value1, &f32Value2);
            stCoolingData.i16Tgun = f32Value1 * 10;
            stCoolingData.u16Qliquid = pInfo->f32SysFlowRate * 100;
            stCoolingData.u16Pliquid = pInfo->f32SysPressure * 100;
            stCoolingData.u16TliquidIn = pInfo->f32InTemper * 10;
            stCoolingData.u16TliquidOut = pInfo->f32OutTemper * 10;
            stCoolingData.u16PumpSpeed = pInfo->u16PumpsSpeed;
            stCoolingData.u16FanSpeed = pInfo->u16FanSpeed;
            
            //告警状态
            stCode= sCoolingAppGetErrCode();
            stCoolingData.stCoolingSts.bPressHigh  = stCode.bPressHigh;
            stCoolingData.stCoolingSts.bPressLow   = stCode.bPressLow;
            stCoolingData.stCoolingSts.bLiquidLow  = stCode.bLiquidLow;
            stCoolingData.stCoolingSts.bIpumpHigh  = stCode.bIpumpHigh;
            stCoolingData.stCoolingSts.bPpumpLow   = stCode.bPpumpLow;
            stCoolingData.stCoolingSts.bPpumpHigh  = stCode.bPpumpHigh;
            stCoolingData.stCoolingSts.bTempHigh   = stCode.bTempHigh;
            stCoolingData.stCoolingSts.bPpumpStall = stCode.bPpumpStall;
            stCoolingData.stCoolingSts.bIfanHigh   = stCode.bIfanHigh;
            
            sPrivDrvSetCoolingData(stCoolingData.u8GunId, &stCoolingData);
        }
    }
}
#endif

















/***************************************************************************************************
* Description                           :   国标充电桩用户程序 之 网关用户程序 之 一般数据处理
* Author                                :   Hall
* Creat Date                            :   2024-05-27
* notice                                :   国标CC1状态转换到内网协议的枪连接状态
****************************************************************************************************/
ePrivDrvGunConnStatus_t sPileCnCc1StateToGunConnStatue(eAdcAppCc1State_t eState)
{
    if((eState == eAdcAppCc1StateU1a) || (eState == eAdcAppCc1StateU1b))
    {
        return ePrivDrvGunConnStatusDisconn;
    }
    else if(eState == eAdcAppCc1StateU1c)
    {
        return ePrivDrvGunConnStatusConn;
    }
    else
    {
        return ePrivDrvGunConnStatusUnknow;
    }
}






/***************************************************************************************************
* Description                           :   将 枪工作状态变化原因 设置到内网协议
* Author                                :   Hall
* Creat Date                            :   2024-05-30
* notice                                :   eGunStsReason[]字段值填充至内网协议缓存
****************************************************************************************************/
bool sPileCnGateSetGunStsReason(ePileGunIndex_t eGunIndex)
{
    bool bRst = false;
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(pData->eGunStsReason[eGunIndex] != ePrivDrvGunWorkStsRsnMax)
    {
        bRst = sPrivDrvSetGunStsReason(sPrivDrvGetGunId(eGunIndex), pData->eGunStsReason[eGunIndex]);
        pData->eGunStsReason[eGunIndex] = ePrivDrvGunWorkStsRsnMax;
    }
    
    return bRst;
}







/***************************************************************************************************
* Description                           :   清除 停充原因
* Author                                :   Hall
* Creat Date                            :   2024-07-25
* notice                                :   
****************************************************************************************************/
bool sPileCnGateClrStopReason(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    pData->eStopReason[eGunIndex] = ePrivDrvStopReasonDefault;
    
    return true;
}












/***************************************************************************************************
* Description                           :   生成 停充原因
* Author                                :   Hall
* Creat Date                            :   2024-07-24
* notice                                :   
****************************************************************************************************/
bool sPileCnGateMakeStopReason(ePileGunIndex_t eGunIndex, ePrivDrvStopReason_t eReason)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pData->eStopReason[eGunIndex] == ePrivDrvStopReasonDefault)
    {
        //只有默认值的情况下，才给赋值，避免前面已经赋过的值被覆盖
        pData->eStopReason[eGunIndex] = eReason;
    }
    
    return true;
}








/***************************************************************************************************
* Description                           :   生成 枪工作状态变化原因
* Author                                :   Hall
* Creat Date                            :   2024-05-30
* notice                                :   eGunStsReason[]字段赋 传入值 eReason
****************************************************************************************************/
bool sPileCnGateMakeGunStsReason(ePileGunIndex_t eGunIndex, ePrivDrvGunWorkStsRsn_t eReason)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    pData->eGunStsReason[eGunIndex] = eReason;
    
    return true;
}
















