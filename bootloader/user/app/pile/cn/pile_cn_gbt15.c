/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gbt15.c
* Description                           :   国标27930-2015用户程序
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-28
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"
#include "pile_cn_check.h"



//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gbt15";



extern stPileCnCache_t *pPileCnCache;





void sPileCnGbt15MainLoop(ePileGunIndex_t eGunIndex);
void sPileCnGbt15InitChgData(ePileGunIndex_t eGunIndex);

void sPileCnGbt15Ptc(ePileGunIndex_t eGunIndex);
void sPileCnGbt15ChgAuthor(ePileGunIndex_t eGunIndex);

void sPileCnGbt15IsoCheck(ePileGunIndex_t eGunIndex);
f32  sPileCnGbt15IsoCheckVoltCalc(ePileGunIndex_t eGunIndex);
eBmsGbt15IsoResult_t sPileCnGbt15IsoCheckRstClac(ePileGunIndex_t eGunIndex, eIsoMechinaStatus_t *pIsoRst);

void sPileCnGbt15PreCharge(ePileGunIndex_t eGunIndex);
bool sPileCnGbt15PreChgVoltProc(ePileGunIndex_t eGunIndex);
bool sPileCnGbt15PreChgParamCheck(ePileGunIndex_t eGunIndex);

void sPileCnGbt15Charging(ePileGunIndex_t eGunIndex);
void sPileCnGbt15ChargingStop(ePileGunIndex_t eGunIndex);

void sPileCnGbt15SetVoltCurr(ePileGunIndex_t eGunIndex, f32 f32Volt, f32 f32Curr);
void sPileCnGbt15SetDrvData(ePileGunIndex_t eGunIndex);

ePileCnVinChgCheckState_t sPileCnGbt15VinChgProc(ePileGunIndex_t eGunIndex);











/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 主循环
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15MainLoop(ePileGunIndex_t eGunIndex)
{
    //1.gbt15协议底层数据初始化
    sPileCnGbt15InitChgData(eGunIndex);
    
    //2.物理连接和授权充电
    //sPileCnGbt15Ptc(eGunIndex);
    sPileCnGbt15ChgAuthor(eGunIndex);
    
    //3.绝缘检测
    sPileCnGbt15IsoCheck(eGunIndex);
    
    //4.预充电
    sPileCnGbt15PreCharge(eGunIndex);
    
    //5.充电过程数据实时刷新
    sPileCnGbt15Charging(eGunIndex);
    
    //6.停充检查
    sPileCnGbt15ChargingStop(eGunIndex);
    
    //7.电子锁控制
    sPileCnElockCtrl(eGunIndex);
}







/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 ---初始化缓存数据
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   待机状态需要重新初始化缓存数据
*                                       
****************************************************************************************************/
void sPileCnGbt15InitChgData(ePileGunIndex_t eGunIndex)
{
    i32 i32UdcMax, i32IdcMax;
    
    //获取当前驱动层执行的阶段
    if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateWait)
    {
        //数据初始化，等驱动层初始化后再执行初始化
        if(pPileCnCache->stChgInfo[eGunIndex].bInit == false)
        {
            pPileCnCache->stChgInfo[eGunIndex].bInit = true;
            
            //通知ModApp组件停止输出
            sModAppOptClose(eGunIndex);
            
            //断开k1k2和辅源供电
            sIoSetDrK(eGunIndex, false);
            sPileCnAuxPowerCtrl(eGunIndex, eRelayCtrlOff);
            sPileCnSetElockState(eGunIndex, eELockCtrlOff);
            
            
            
            EN_SLOGI(TAG, "国标%d枪数据初始化完成", eGunIndex);
        }
    }
    else
    {
        pPileCnCache->stChgInfo[eGunIndex].bInit = false;
    }
    
    //获取ModApp组件可提供的电压电流
    sModAppOptGetUdcMax(eGunIndex, &i32UdcMax);
    sModAppOptGetIdcMax(eGunIndex, &i32IdcMax);
    
    //获取eeprom设置的电压电流上限值与ModApp可提供的电压电流取较小值
    i32UdcMax = (i32UdcMax < sEepromAppGetBlockFactoryUdcOutMax()) ? i32UdcMax : sEepromAppGetBlockFactoryUdcOutMax();
    i32IdcMax = (i32IdcMax < sEepromAppGetBlockFactoryIdcOutMax()) ? i32UdcMax : sEepromAppGetBlockFactoryIdcOutMax();
    
    i32IdcMax = (i32IdcMax < ((u16)pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun)) ? i32IdcMax : ((u16)pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun);
    sBmsGbt15Set_ChgMaxOutVol(eGunIndex, (u16)(i32UdcMax * 10));        //充电机最大输出电压
    sBmsGbt15Set_ChgMinOutVol(eGunIndex, 0);                            //充电机最小输出电压
    sBmsGbt15Set_ChgMaxOutCur(eGunIndex, (u16)(i32IdcMax * 10));        //充电机最大输出电流
    sBmsGbt15Set_ChgMinOutCur(eGunIndex, 0);                            //充电机最小输出电流
}









/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 Ptc充电
* Author                                :   haisheng.dang
* Creat Date                            :   2024-05-23
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15Ptc(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t          *pData = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcd_t    *pRcd = &pData->stNewRcd[eGunIndex];
    
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    
    if((sPileCnStateGet(eGunIndex) == ePrivDrvGunWorkStatusPreparing) && (sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeNone))
    {
        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeVin);
        
        //初始化并创建订单
        sPileCnGateNewRcdInitForPtc(eGunIndex);
        sEepromAppGetBlockOperatorRate(&pData->unRate);
        
        //启动计量
        sOrderStart(eGunIndex, sEepromAppNewBlockChgRcd(pRcd), pRcd, &pData->unRate, sPileCnGateGetSoc);
        
    }
}









/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 充电鉴权
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   识别插枪ok并且有用户授权的情况，开始充电
*                                       
****************************************************************************************************/
void sPileCnGbt15ChgAuthor(ePileGunIndex_t eGunIndex)
{
    i32 i32CurMaxAvailVolt, i32CurMaxAvailCurr;
    f32 f32GunDcVolt;
    
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32GunDcVolt);
    
    //获取当前驱动层执行的阶段
    if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateWait)
    {
        
        //判断车端是否熄火
        if(f32GunDcVolt <= cJudgeSelfCheckVolt)
        {
            sBmsGbt15Set_PreIsoCheckResult(eGunIndex, eBmsGbt15PreIsoCheckNormal);
        }
        else
        {
            sBmsGbt15Set_PreIsoCheckResult(eGunIndex, eBmsGbt15PreIsoCheckFail);
            return;
        }
        
        //获取ModApp组件可提供的电压电流
        sModAppOptGetUdcMax(eGunIndex, &i32CurMaxAvailVolt);
        sModAppOptGetIdcMax(eGunIndex, &i32CurMaxAvailCurr);
        
        pPileCnCache->stChgInfo[eGunIndex].eVinCheckState = eVinChgCheckIdle;
        
        //用户授权充电，等待用户数据初始化完成再走流程，避免某些数据未复位
        if((sPileCnGet_Authorize(eGunIndex) != eAuthorizeTypeNone) && (pPileCnCache->stChgInfo[eGunIndex].bInit == true))
        {
            
            //判断Cc1是否连接ok
            if(sAdcAppSampleGetCc1State(eGunIndex) != eAdcAppCc1StateU1c)
            {
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                EN_SLOGI(TAG, "错误：国标%d枪未插枪，停止充电", eGunIndex);
                return;
            }
            
            //判断Pcu是否能够提供电压电流
            if((i32CurMaxAvailVolt == 0) || (i32CurMaxAvailCurr == 0))
            {
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                EN_SLOGI(TAG, "错误：国标%d枪接收到授权充电指令，但Pcu无法提供能量，停止充电", eGunIndex);
                
                return;
            }
            
            //闭合电子锁
            sPileCnSetElockState(eGunIndex, eELockCtrlOn);
            
            //辅助电源供电，通知驱动层，开始握手充电
            sBmsGbt15Set_AuthorizeState(eGunIndex, true);
            sPileCnAuxPowerCtrl(eGunIndex, eRelayCtrlOn);
            pPileCnCache->stChgInfo[eGunIndex].u32StageTime = xTaskGetTickCount();
            EN_SLOGI(TAG, "国标%d枪电子锁闭合成功，开始充电握手", eGunIndex);
        }
        else
        {
            //刷新时间戳
            pPileCnCache->stChgInfo[eGunIndex].u32StageTime = xTaskGetTickCount();
            sBmsGbt15Set_AuthorizeState(eGunIndex, false);
        }
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateConfig)
    {
        //Vin码在收到Brm报文后才能拿到，所以在此阶段再给平台上送Vin码鉴权
        if((sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeVin) && (pPileCnCache->stChgInfo[eGunIndex].eVinCheckState == eVinChgCheckIdle))
        {
            //等待平台鉴权完成
            pPileCnCache->stChgInfo[eGunIndex].eVinCheckState = eVinChgCheckWait;
        }
    }
}








/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 iso绝缘检测
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   国标启动充电前桩端需要进行绝缘自检，充电中由车端自检
*                                       
****************************************************************************************************/
void sPileCnGbt15IsoCheck(ePileGunIndex_t eGunIndex)
{
    f32  f32GunDcVolt;
    
    eBmsGbt15IsoResult_t eRst;
    eIsoMechinaStatus_t *pIsoRst = NULL;
    
    static bool bIsoCheckFlag[ePileGunIndexNum];                                //可以不用赋初值, 下面的赋值逻辑会给好初值
    
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32GunDcVolt);
    
    
    //获取当前驱动层执行的阶段
    if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateCableCheck)
    {
        if(sEepromAppGetBlockFactoryIsoChkFlag() == true)
        {
            //1.启动绝缘检测，吸合K1K2
            if(bIsoCheckFlag[eGunIndex] == false)
            {
                if(f32GunDcVolt <= cJudgeSelfCheckVolt)
                {
                    sBmsGbt15Set_PreIsoCheckResult(eGunIndex, eBmsGbt15PreIsoCheckNormal);
                    
                    bIsoCheckFlag[eGunIndex] = true;
                    sIsoStart(eGunIndex, sPileCnGbt15IsoCheckVoltCalc(eGunIndex));
                    EN_SLOGI(TAG, "国标%d枪开始绝缘检测", eGunIndex);
                }
                else
                {
                    sBmsGbt15Set_PreIsoCheckResult(eGunIndex, eBmsGbt15PreIsoCheckFail);
                }
                //刷新时间戳
                pPileCnCache->stChgInfo[eGunIndex].u32StageTime = xTaskGetTickCount();
            }
            
            //2.周期调用绝缘检测主循环函数，待绝缘检测完成
            pIsoRst = sIosCheck(eGunIndex, eIsoCheckBeforeCharge);
            eRst = sPileCnGbt15IsoCheckRstClac(eGunIndex, pIsoRst);
        }
        else
        {
            //电阻载直接越过ISO检测
            eRst = eBmsGbt15IsoCheckNormal;
        }
        
        sBmsGbt15Set_IsoCheckResult(eGunIndex, eRst);
        if(eRst == eBmsGbt15IsoCheckFail)
        {
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
            sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultOther, eErrNone);
            EN_SLOGI(TAG, "错误：国标%d枪绝缘检测故障，停止充电", eGunIndex);
        }
    }
    else
    {
        //上电后程序就会执行这里 相当于赋初值了
        bIsoCheckFlag[eGunIndex] = false;
    }
}







/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 获取绝缘检测电压
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-29
* notice                                :   
*                                       
****************************************************************************************************/
f32 sPileCnGbt15IsoCheckVoltCalc(ePileGunIndex_t eGunIndex)
{
    f32 f32TempVolt;
    
    u16 u16EvMaxVol;
    i32 i32SeMaxVol;
    
    
    //1.从ModApp组件获取当前最大输出电压
    sModAppOptGetUdcMax(eGunIndex, &i32SeMaxVol);
    i32SeMaxVol *= 10;
    
    //2.获取车端最大允许充电电压
    sBmsGbt15Get_EvccMaxChgVol(eGunIndex, &u16EvMaxVol);
    
    //3.绝缘检测电压取车端最高允许充电电压和充电桩额定电压两者中的较小值
    f32TempVolt = ((f32)((u16EvMaxVol >= i32SeMaxVol) ? i32SeMaxVol : u16EvMaxVol)) * 0.1f;
    EN_SLOGI(TAG, "国标%d枪绝缘检测电压:%.1fV", eGunIndex, f32TempVolt);
    
    return f32TempVolt;
}






/***************************************************************************************************
* Description                           :   将ISO检测模块输出的结果转换为 国标bms协议 定义的类型
* Author                                :   Hall
* Creat Date                            :   2024-07-13
* notice                                :   
****************************************************************************************************/
eBmsGbt15IsoResult_t sPileCnGbt15IsoCheckRstClac(ePileGunIndex_t eGunIndex, eIsoMechinaStatus_t *pIsoRst)
{
    stPileCnChargeInfo_t *pInfo = &pPileCnCache->stChgInfo[eGunIndex];
    
    if(pIsoRst->eIsoCheckStatus == eIsoCheckIdel)
    {
        if(pIsoRst->eIsoRst == eIsoOk)
        {
            return eBmsGbt15IsoCheckNormal;
        }
        else if(pIsoRst->eIsoRst == eIsoIa)
        {
            return eBmsGbt15IsoCheckWarn;
        }
        else
        {
            return eBmsGbt15IsoCheckFail;
        }
    }
    else if((xTaskGetTickCount() - pInfo->u32StageTime) >= cTimeoutIsoCheck)
    {
        return eBmsGbt15IsoCheckFail;                                           //超时也是失败
    }
    else
    {
        return eBmsGbt15IsoChecking;
    }
}








/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 预充电
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15PreCharge(ePileGunIndex_t eGunIndex)
{
    static bool bParamVailedFlag[ePileGunIndexNum];                             //可以不用赋初值, 下面的赋值逻辑会给好初值
    
    
    if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StatePreCharge)
    {
        //开始预充
        if(bParamVailedFlag[eGunIndex] == true)
        {
            //预充超时判断
            if((xTaskGetTickCount() - pPileCnCache->stChgInfo[eGunIndex].u32StageTime) >= cTimeoutPreChg)
            {
                if(sPileCnGbt15VinChgProc(eGunIndex) == eVinChgCheckWait || (sPileCnGbt15VinChgProc(eGunIndex) == eVinChgCheckFailed))
                {
                    sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                    EN_SLOGI(TAG, "错误：国标%d枪Vin码平台校验超时，停止充电", eGunIndex);
                }
                else
                {
                    sBmsGbt15Set_PreChgResult(eGunIndex, eBmsGbt15PreChgFail);
                    sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                    EN_SLOGI(TAG, "错误：国标%d枪预充超时", eGunIndex);
                }
                sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultPowerForbid, eErrNone);
            }
            else
            {
                //等待预充完成
                if(sPileCnGbt15PreChgVoltProc(eGunIndex) == true)
                {
                    //确认平台校验结果
                    if(sPileCnGbt15VinChgProc(eGunIndex) == eVinChgCheckSuccess)
                    {
                        sModAppOptCloseContactor();                             //先断开接触器（XYZ）
                        sIoSetDrK(eGunIndex, true);                             //再闭合枪接触器Ka/Kb
                        pPileCnCache->stChgInfo[eGunIndex].bIsPauseChg = false;
                        sBmsGbt15Set_PreChgResult(eGunIndex, eBmsGbt15PreChgNormal);
                    }
                    else if(sPileCnGbt15VinChgProc(eGunIndex) == eVinChgCheckFailed)
                    {
                        //平台回复车端Vin校验不通过
                        sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                        sBmsGbt15Set_CstCode(eGunIndex, eReasonManual, eFaultNone, eErrNone);
                        EN_SLOGI(TAG, "错误：国标%d枪Vin码未通过平台校验，停止充电", eGunIndex);
                    }
                }
            }
        }
        else
        {
            //预充前需要做参数检查，电阻载模式下不检查
            if((sPileCnGbt15PreChgParamCheck(eGunIndex) == true) || (sEepromAppGetBlockFactoryResFlag() == true))
            {
                bParamVailedFlag[eGunIndex] = true;
                //刷新时间戳
                pPileCnCache->stChgInfo[eGunIndex].u32StageTime = xTaskGetTickCount();
                EN_SLOGI(TAG, "国标%d枪进入预充阶段", eGunIndex);
            }
            else
            {
                //条件不满足
                sModAppOptClose(eGunIndex);
                sBmsGbt15Set_PreChgResult(eGunIndex, eBmsGbt15PreChgFail);
                sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultPowerForbid, eErrNone);
                EN_SLOGI(TAG, "错误：国标%d枪预充条件不满足", eGunIndex);
            }
        }
    }
    else
    {
        //上电后程序就会执行这里 相当于赋初值了
        bParamVailedFlag[eGunIndex] = false;
    }
}







/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 预充处理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnGbt15PreChgVoltProc(ePileGunIndex_t eGunIndex)
{
    u16 u16Ubat;
    f32 f32CurOutputVol, f32SampleVol;
    
    //获取bcp电池当前电压
    sBmsGbt15Get_BcpCurBatVol(eGunIndex, &u16Ubat);
    f32SampleVol = ((f32)u16Ubat) / 10;
    
    //向ModApp组件申请电压输出，比电池电压小1~10v左右
    sModAppOptSetOutput(eGunIndex, (f32SampleVol - 5), 0);
    sModAppOptOpen(eGunIndex);
    if(sEepromAppGetBlockFactoryResFlag() == false)
    {
        //从电表获取当前输出电压
        sMeterDrvOptGetUdcIdcPdc((u8)eGunIndex, &f32CurOutputVol, NULL, NULL);
        
        //输出电压满足比外侧电池电压小，闭合k1k2继电器
        if((f32CurOutputVol >= (f32SampleVol - 10)) && (f32CurOutputVol <= (f32SampleVol - 1)))
        {
            //为兼容Vin码充电流程，直流接触器不放在这里闭合
            
            return true;
        }
    }
    else
    {
        //电阻载不做预充电 直接返回true
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 预充电前的参数检查
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
bool sPileCnGbt15PreChgParamCheck(ePileGunIndex_t eGunIndex)
{
    //预充前需要检查以下几个参数
    //1.采样外侧电压是否大于充电机最大输出电压
    //2.采样外侧电压是否小于充电机最小输出电压
    //3.采样外侧电压是否大于电池允许充电最高电压
    //4.采样外侧电压是否与报文电压相差超过5% ———— 《GB/T 18487.1-2015》B.3.4
    
    bool bRst = true;
    f32  f32SampleVol;
    i32  i32ChgMaxOutVol;
    u16  u16BcpAllowMaxVol, u16BcpCurBatVol;
    u16  u16FactoryUdcOutMax;
    
    //获取外侧采样电压
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32SampleVol);
    
    //获取ModApp组件最大输出电压
    sModAppOptGetUdcMax(eGunIndex, &i32ChgMaxOutVol);
    
    //获取电池允许充电最高电压
    sBmsGbt15Get_BcpMaxChgVol(eGunIndex, &u16BcpAllowMaxVol);
    u16BcpAllowMaxVol = u16BcpAllowMaxVol / 10;
    
    //获取当前电池组的电压
    sBmsGbt15Get_BcpCurBatVol(eGunIndex, &u16BcpCurBatVol);
    u16BcpCurBatVol = u16BcpCurBatVol / 10;
    
    //获取设置的电压上限
    u16FactoryUdcOutMax = sEepromAppGetBlockFactoryUdcOutMax();
    
    //确定桩端输出最大电压
    i32ChgMaxOutVol = (i32ChgMaxOutVol < u16FactoryUdcOutMax)? i32ChgMaxOutVol : u16FactoryUdcOutMax;
    
    if(f32SampleVol - 50 > (f32)i32ChgMaxOutVol)
    {
        EN_SLOGI(TAG, "错误：国标%d枪预充前外侧电压%f大于最大输出电压%f，停止充电", eGunIndex, f32SampleVol, (f32)i32ChgMaxOutVol);
        bRst = false;
    }
    else if(f32SampleVol < 0)
    {
        EN_SLOGI(TAG, "错误：国标%d枪预充前外侧电压%f小于0，停止充电", eGunIndex, f32SampleVol);
        bRst = false;
    }
    else if(f32SampleVol - 50 >= (f32)u16BcpAllowMaxVol)
    {
        EN_SLOGI(TAG, "错误：国标%d枪预充前外侧电压%f大于最大允许充电电压%f，停止充电", eGunIndex, f32SampleVol, (f32)u16BcpAllowMaxVol);
        bRst = false;
    }
    else if((fabs(f32SampleVol - (f32)u16BcpCurBatVol) * 20) > fmin(f32SampleVol, (f32)u16BcpCurBatVol))
    {
        EN_SLOGI(TAG, "错误：国标%d枪预充前外侧电压%f和电池电压%f之比相差%%5，停止充电", eGunIndex, f32SampleVol, (f32)u16BcpCurBatVol);
        bRst = false;
    }
    
    
    return bRst;
}







/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 正常充电
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15Charging(ePileGunIndex_t eGunIndex)
{
    f32 f32TempVolt, f32TempCurr;
    static u32 u32PauseTime[ePileGunIndexNum] = {0, 0};
    static u32 u32FullTime[ePileGunIndexNum] = {0, 0};
    
    if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateCharging)
    {
        //获取车端需求电压
        sBmsGbt15Get_BclReqChgVol(eGunIndex, &pPileCnCache->stChgInfo[eGunIndex].u16SetVol);
        sBmsGbt15Get_BclReqChgCur(eGunIndex, &pPileCnCache->stChgInfo[eGunIndex].u16SetCur);
        
        //确认是否暂停了充电
        if(sBmsGbt15Get_EvPauseChargeState(eGunIndex) == true)
        {
            sModAppOptClose(eGunIndex);
            pPileCnCache->stChgInfo[eGunIndex].bIsPauseChg = true;
            u32PauseTime[eGunIndex] = xTaskGetTickCount();
            
            sMeterDrvOptGetUdcIdcPdc((u8)eGunIndex, &f32TempVolt, &f32TempCurr, NULL);
            
            if((xTaskGetTickCount() - pPileCnCache->stChgInfo[eGunIndex].u32StageTime) >= cTimeoutPauseChg)
            {
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultPowerForbid, eErrNone);
                EN_SLOGI(TAG, "错误：国标%d枪充电暂停时间超过10min", eGunIndex);
            }
            else if(f32TempCurr <= cJudgeSafeCurr)
            {
                //断开直流接触器K1K2
                sIoSetDrK(eGunIndex, false);
            }
        }
        else if(pPileCnCache->stChgInfo[eGunIndex].bIsPauseChg == true)
        {
            //暂停充电恢复正常充电，需要重新预充
            if((xTaskGetTickCount() - u32PauseTime[eGunIndex]) > cTimeoutResumeChg)
            {
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                sBmsGbt15Set_CstCode(eGunIndex, eReasonFaulted, eFaultOther, eErrNone);
                EN_SLOGI(TAG, "错误：国标%d枪充电暂停恢复到正常充电阶段，输出超时", eGunIndex);
            }
            else if(sPileCnGbt15PreChgVoltProc(eGunIndex) == true)
            {
                pPileCnCache->stChgInfo[eGunIndex].bIsPauseChg = false;
                EN_SLOGI(TAG, "国标%d枪暂停充电已恢复：正常充电", eGunIndex);
            }
        }
        else
        {
            //通知Pcu按照Bcl需求输出能量
            sPileCnGbt15SetVoltCurr(eGunIndex, ((f32)pPileCnCache->stChgInfo[eGunIndex].u16SetVol) / 10, ((f32)pPileCnCache->stChgInfo[eGunIndex].u16SetCur) / 10);
            
            //刷新时间戳
            pPileCnCache->stChgInfo[eGunIndex].u32StageTime = xTaskGetTickCount();
        }
        
        
        //获取充电剩余时间，min
        sBmsGbt15Get_ChgRemainTime(eGunIndex, &pPileCnCache->stChgInfo[eGunIndex].u16ReaminTime);
        
        //获取车端当前soc
        pPileCnCache->stChgInfo[eGunIndex].u8CurSoc = sBmsGbt15Get_EvccCurSoc(eGunIndex);
        if(pPileCnCache->stChgInfo[eGunIndex].u8CurSoc >= sEepromAppGetBlockFactorySocMax())
        {
            u32FullTime[eGunIndex]++;
            if(u32FullTime[eGunIndex] >= 200)
            {
                EN_SLOGI(TAG, "国标%d枪电池容量已达桩端设置值，停止充电", eGunIndex);
                sPileCnGateMakeStopReason(eGunIndex, ePrivDrvStopReasonFull);
                sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
                sBmsGbt15Set_CstCode(eGunIndex, eReasonFinished, eFaultNone, eErrNone);
            }
        }
        else
        {
            u32FullTime[eGunIndex] = 0;
        }
        
        
        //刷新驱动层实时电压电流
        sPileCnGbt15SetDrvData(eGunIndex);
    }
}







/***************************************************************************************************
* Description                           :   基于gbt27930-2015的国标充电桩用户代码 停充检查
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-28
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15ChargingStop(ePileGunIndex_t eGunIndex)
{
    f32 f32OutVolt, f32OutCurr;
    f32 f32GunOutVolt;
    
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32GunOutVolt);
    sMeterDrvOptGetUdcIdcPdc((u8)eGunIndex, &f32OutVolt, &f32OutCurr, NULL);
    
    //1.检测CC1电平状态，确认充电枪连接是正常的
    if(sAdcAppSampleGetCc1State(eGunIndex) != eAdcAppCc1StateU1c)
    {
        if(sPileCnGet_Authorize(eGunIndex) != eAuthorizeTypeNone)
        {
            sBmsGbt15Set_Cc1StateInChg(eGunIndex, eCc1stateChgFail);
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
            sIoSetDrK(eGunIndex, false);
            sPileCnAuxPowerCtrl(eGunIndex, eRelayCtrlOff);
            EN_SLOGI(TAG, "错误：国标%d枪充电过程中检测到CC1电压异常，紧急停止充电", eGunIndex);
        }
    }
    
    //国标急停和门禁故障，K1,K2故障立刻断开接触器
    if((sFaultCodeChkCode(cPrivDrvGunIdChg,ePrivDrvFaultCode_ccuEstopErr) == true) 
    || (sFaultCodeChkCode(cPrivDrvGunIdChg,ePrivDrvFaultCode_dispenserDoorErr) == true)
    || (sFaultCodeChkCode(eGunIndex + cPrivDrvGunIdBase,ePrivDrvFaultCode_dcContactor1Err) == true)
    || (sFaultCodeChkCode(eGunIndex + cPrivDrvGunIdBase,ePrivDrvFaultCode_dcContactor2Err) == true)
    || (sFaultCodeChkCode(eGunIndex, ePrivDrvFaultCode_ccuDischargeErr) == true))
    {
            sPileCnSet_Authorize(eGunIndex, eAuthorizeTypeNone);
            sIoSetDrK(eGunIndex, false);
            sPileCnAuxPowerCtrl(eGunIndex, eRelayCtrlOff);
    }
    
    
    //正常停充
    if((sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFinish) || (sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultAB))
    {
        sModAppOptClose(eGunIndex);
        //增加K1K2必须达到安全电压电流才断开
        if((f32OutCurr < cJudgeSafeCurr) && (f32OutVolt < cJudgeSafeVolt))
        {
            sIoSetDrK(eGunIndex, false);
        }
        
        if(sBmsGbt15Get_AllowStopAuxPower(eGunIndex) == true)
        {
            sPileCnAuxPowerCtrl(eGunIndex, eRelayCtrlOff);
        }
        
        //检测电子锁是否解锁
        if((sIoDrvGetInput(sIoGetLockFb(eGunIndex)) == RESET) && (f32GunOutVolt < cJudgeSafeVolt))
        {
            sPileCnSetElockState(eGunIndex, eELockCtrlOff);
        }
        
    }
    else if(sBmsGbt15StateGet(eGunIndex) == eBmsGbt15StateFaultC)
    {
        sModAppOptClose(eGunIndex);
        
        if((f32OutCurr < cJudgeSafeCurr) && (f32OutVolt < cJudgeSafeVolt))
        {
            //断开K1K2直流接触器
            sIoSetDrK(eGunIndex, false);
            sBmsGbt15Set_ReChgCondition(eGunIndex, eBmsGbt15ReChgOk);
        }
    }
    
    if(sPileCnGet_Authorize(eGunIndex) == eAuthorizeTypeNone)
    {
        sBmsGbt15Set_AuthorizeState(eGunIndex, false);
    }
}













/***************************************************************************************************
* Description                           :   根据枪头温度动态调整需求电流
* Author                                :   haisheng.dang
* Creat Date                            :   2024-04-30
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15SetVoltCurr(ePileGunIndex_t eGunIndex, f32 f32Volt, f32 f32Curr)
{
    f32 f32CurrDelta;
    f32 f32Temp01, f32Temp02, f32MaxTemp;
    u16 u16BcpMaxcur;
    u16 u16UdcOutmax, u16IdcOutMax;
    f32 f32PdcMax;
    f32 f32CurrLimitbyPdcMax;
    
    //获取枪温
    sAdcAppSampleGetGunTemp(eGunIndex, &f32Temp01, &f32Temp02);
    
    //获取枪最高温度
    f32MaxTemp = (f32Temp01 >= f32Temp02) ? f32Temp01 : f32Temp02;
    
    //获取Bcp最大电流
    sBmsGbt15Get_BcpMaxChgCur(eGunIndex, &u16BcpMaxcur);
    
    
    //获取eeprom存储的电流上限值（由tcu设置）
    u16IdcOutMax = sEepromAppGetBlockFactoryIdcOutMax();
    u16UdcOutmax = sEepromAppGetBlockFactoryUdcOutMax();
    
    f32Volt = (f32Volt <= u16UdcOutmax) ? f32Volt : u16UdcOutmax;
    
    u16BcpMaxcur = u16BcpMaxcur/10;
    
    //获取功率限制的电流
    f32PdcMax = sEepromAppGetBlockFactoryPowerMax();
    f32CurrLimitbyPdcMax = f32PdcMax / f32Volt;
    
    if(f32MaxTemp <= cJudgeCurrDropMinTemper)
    {
        //取枪线最大电流限制
        f32Curr = (f32Curr <= pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun) ? f32Curr : pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun;
        
        //取bcp最高允许电流
        f32Curr = (f32Curr <= u16BcpMaxcur) ? f32Curr : u16BcpMaxcur;
        
        //取电流上限值限制
        f32Curr = (f32Curr <= u16IdcOutMax) ? f32Curr : u16IdcOutMax;
        
        //取功率限制值
        f32Curr = (f32Curr <= f32CurrLimitbyPdcMax) ? f32Curr : f32CurrLimitbyPdcMax;
        
        //向ModApp组件申请电压电流
        sModAppOptSetOutput(eGunIndex, f32Volt, f32Curr);
    }
    else if((f32MaxTemp > cJudgeCurrDropMinTemper) && (f32MaxTemp <= cJudgeCurrDropMaxTemper))
    {
        //二分之一的需求电流--->按降载温度区间均分
        f32CurrDelta = (pPileCnCache->stChgInfo[eGunIndex].f32CurrLimitByGun * 0.5) / (cJudgeCurrDropMaxTemper - cJudgeCurrDropMinTemper);
        
        //实际需求电流 = 需求电流 - 温度升高需要下降的电流
        f32Curr = f32Curr - (f32CurrDelta * (f32MaxTemp - cJudgeCurrDropMinTemper));
        
        //向ModApp组件申请电压电流
        sModAppOptSetOutput(eGunIndex, f32Volt, f32Curr);
    }
}









/***************************************************************************************************
* Description                           :   刷新bms协议驱动必须参数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-06
* notice                                :   
*                                       
****************************************************************************************************/
void sPileCnGbt15SetDrvData(ePileGunIndex_t eGunIndex)
{
    u32 u32CurMeterWh;
    f32 f32TempVolt, f32TempCurr;
    
    //获取当前充电电量，单位：1wh----不需要考虑电损系数
    u32CurMeterWh = pPileCnCache->stGateData.stNewRcd[eGunIndex].stRcdData.u32MeterStart - pPileCnCache->stGateData.stNewRcd[eGunIndex].stRcdData.u32MeterStop;
    
    sMeterDrvOptGetUdcIdcPdc((u8)eGunIndex, &f32TempVolt, &f32TempCurr, NULL);
    pPileCnCache->stChgInfo[eGunIndex].u16OutputVol = (u16)(f32TempVolt * 10);
    pPileCnCache->stChgInfo[eGunIndex].u16OutputCur = (u16)(f32TempCurr * 10);
    sBmsGbt15Set_OutputVoltage(eGunIndex, pPileCnCache->stChgInfo[eGunIndex].u16OutputVol);
    sBmsGbt15Set_OutputCurrent(eGunIndex, pPileCnCache->stChgInfo[eGunIndex].u16OutputCur);
    sBmsGbt15Set_ChargeTime(eGunIndex, (pPileCnCache->stGateData.stNewRcd[eGunIndex].stRcdData.u32ChargingTime / 60));
    sBmsGbt15Set_ChargeTotalWh(eGunIndex, u32CurMeterWh);
    
}










/***************************************************************************************************
* Description                           :   Vin码充电处理
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-06
* notice                                :   
*                                       
****************************************************************************************************/
ePileCnVinChgCheckState_t sPileCnGbt15VinChgProc(ePileGunIndex_t eGunIndex)
{
    if(sPileCnGet_Authorize(eGunIndex) != eAuthorizeTypeVin)
    {
        //不是Vin码充电默认校验通过
        return eVinChgCheckSuccess;
    }
    else if(pPileCnCache->stChgInfo[eGunIndex].eVinCheckState == eVinChgCheckWait)
    {
        return eVinChgCheckWait;
    }
    else if(pPileCnCache->stChgInfo[eGunIndex].eVinCheckState == eVinChgCheckFailed)
    {
        return eVinChgCheckFailed;
    }
    
    return eVinChgCheckSuccess;
}











