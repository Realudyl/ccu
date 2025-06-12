/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_fault_bms.c
* Description                           :   pile_cn_fault_bms-----bms相关故障告警检查注册
* Version                               :   
* Author                                :   Dai
* Creat Date                            :   2024-08-12
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"




    //设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_fault_bms";








void sPileCnFaultChkRegister_Bms(void);


void sPileCnFaultCheck_bmsTimeout(void);
void sPileCnFaultCheck_BhmErr(void);
void sPileCnFaultCheck_bmsComErr(void);
void sPileCnFaultCheck_BstErr(void);
void sPileCnFaultCheck_BatVoltErr(void);
void sPileCnFaultCheck_PrechgVoltErr();
void sPileCnFaultCheck_BsmErr(void);
void sPileCnFaultCheck_PileComTimeout(void);
void sPileCnFaultCheck_BVH(void);
void sPileCnFaultCheck_BCH(void);








/**********************************************************************************************
* Description                           :   bms故障码注册
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultChkRegister_Bms(void)
{
    sFaultChkRegister(sPileCnFaultCheck_bmsTimeout);
    sFaultChkRegister(sPileCnFaultCheck_BhmErr);
    sFaultChkRegister(sPileCnFaultCheck_bmsComErr);
    sFaultChkRegister(sPileCnFaultCheck_BstErr);
    sFaultChkRegister(sPileCnFaultCheck_BsmErr);
    sFaultChkRegister(sPileCnFaultCheck_PileComTimeout);
    sFaultChkRegister(sPileCnFaultCheck_BVH);
    sFaultChkRegister(sPileCnFaultCheck_BCH);
    
    if(sEepromAppGetBlockFactoryResFlag() == false)
    {
        sFaultChkRegister(sPileCnFaultCheck_BatVoltErr);
        sFaultChkRegister(sPileCnFaultCheck_PrechgVoltErr);
    }
}





/**********************************************************************************************
* Description                           :   车端超时检测--枪告警
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_bmsTimeout(void)
{
    u8 i;
    unBmsGbt15RecvTimeout unBmsTimeout;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_EvccFrameTimeout(i, &unBmsTimeout);         //获取车端报文超时信息，用于告警
        
        //bcp超时告警检测
        if(unBmsTimeout.stRxTimeout.bBcpTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcpTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcpTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcpTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcpTimeout);
            }
        }
        
        //bro告警检测
        if(unBmsTimeout.stRxTimeout.bBroTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_broAaTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_broAaTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_broAaTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_broAaTimeout);
            }
        }
       
       
        //bcs告警检测
        if(unBmsTimeout.stRxTimeout.bBcsTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcsTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcsTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcsTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bcsTimeout);
            }
        }
        
        //bcl告警检测
        if(unBmsTimeout.stRxTimeout.bBclTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclTimeout);
            }
        }
        
        //bst告警检测
        if(unBmsTimeout.stRxTimeout.bBstTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bstTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bstTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bstTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bstTimeout);
            }
        }
        
        //bsd告警检测
        if(unBmsTimeout.stRxTimeout.bBsdTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsdTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsdTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsdTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsdTimeout);
            }
        }
        
        //bsm告警检测
        if(unBmsTimeout.stRxTimeout.bBsmTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsmTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsmTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsmTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bsmTimeout);
            }
        }
        
        //brm告警检测
        if(unBmsTimeout.stRxTimeout.bBrmTimeout != eBmsCheckOk)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_brmTimeout) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_brmTimeout);
            }
        }
        else
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_brmTimeout) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_brmTimeout);
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   握手失败--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BhmErr(void)
{
    u8 i;
    u16 u16MaxChgVol;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_EvccMaxChgVol(i, &u16MaxChgVol);
        
        //应该从eeprom获取的桩端能力，eeprom没数据暂定200V测试
        if((u16MaxChgVol < 2000) && (u16MaxChgVol > 0))
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bhmMismErr) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bhmMismErr);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bhmMismErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bhmMismErr);
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   bms超时检测--枪故障（三次超时后触发）
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_bmsComErr(void)
{
    u8 i;
    u16 u16BmsShankhandTimes;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sBmsGbt15StateGet(i) == eBmsGbt15StateFaultAB)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsCommErr) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsCommErr);
            }
        }
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsCommErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsCommErr);
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   bst故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BstErr(void)
{
    u8 i;
    stBmsGbt15Evcc_t pCache;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sBmsGbt15Get_EvccCacheData(i, &pCache) == true)
        {
            if(pCache.stBstData.unStopFault.stFault.bInsulFault == eBmsCheckfail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsImdErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsImdErr);
                }
            }
            
            if(pCache.stBstData.unStopFault.stFault.bComponConntFault == eBmsCheckfail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverTemp) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverTemp);
                }
            }
            
            if(pCache.stBstData.unStopErr.stErr.bVolAbnormal == eBmsCheckfail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverVolt) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverVolt);
                }
            }
            
            if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsImdErr) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsImdErr);
                }
                
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverTemp) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverTemp);
                }
                
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverVolt) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsOverVolt);
                }
            }
        }
    }
}







/**********************************************************************************************
* Description                           :   电池过欠压故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BatVoltErr(void)
{
    u8 i;
    u16 u16BcpCurBatVol;
    f32 f32SampleVol;
    eBmsGbt15PreChgRst_t eResult;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if((sAdcAppSampleGetGunDcOut(i, &f32SampleVol) == true)
        && (sBmsGbt15Get_BcpCurBatVol(i, &u16BcpCurBatVol) == true)
        &&(sBmsGbt15Get_PreChgResult(i, &eResult) == true ))
        {
            u16BcpCurBatVol = u16BcpCurBatVol / 10;
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batUnderVoltErr) == false)
            {
                if((eResult == eBmsGbt15PreChgFail) 
                && ((fabs(f32SampleVol - (f32)u16BcpCurBatVol) / fmin(f32SampleVol, (f32)u16BcpCurBatVol)) > 0.05)
                && (f32SampleVol < (f32)u16BcpCurBatVol))
                {
                     sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batUnderVoltErr);
                }
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverVolt) == false)
            {
                if((eResult == eBmsGbt15PreChgFail)
                && ((fabs(f32SampleVol - (f32)u16BcpCurBatVol) / fmin(f32SampleVol, (f32)u16BcpCurBatVol)) > 0.05)
                && (f32SampleVol > (f32)u16BcpCurBatVol))
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverVolt);
                }
            }
            
            if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverVolt) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverVolt);
                }
                
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batUnderVoltErr) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batUnderVoltErr);
                }
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   预充电压不匹障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_PrechgVoltErr(void)
{
    u8 i;
    f32 f32SampleVol;
    i32 i32ChgMaxOutVol;
    u16 u16BcpAllowMaxVol, u16BcpCurBatVol;
    eBmsGbt15PreChgRst_t eResult;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        //获取外侧采样电压
        sAdcAppSampleGetGunDcOut(i, &f32SampleVol);
        
        //获取充电机最大输出电压
        sModAppOptGetUdcMax(i, &i32ChgMaxOutVol);
        
        //获取电池允许充电最高电压
        sBmsGbt15Get_BcpMaxChgVol(i, &u16BcpAllowMaxVol);
        u16BcpAllowMaxVol = u16BcpAllowMaxVol / 10;
        
        //获取当前电池组的电压
        sBmsGbt15Get_BcpCurBatVol(i, &u16BcpCurBatVol);
        u16BcpCurBatVol = u16BcpCurBatVol / 10;
        
        //获取预充结果
        sBmsGbt15Get_PreChgResult(i, &eResult);
        
        
        if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsPChVoltErr) == false)
        {
            if((eResult == eBmsGbt15PreChgFail) 
            && ((f32SampleVol > i32ChgMaxOutVol) || (f32SampleVol < 0) || (f32SampleVol > u16BcpAllowMaxVol)))
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsPChVoltErr);
            }
        }
        
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsPChVoltErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bmsPChVoltErr);
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   bsm相关故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BsmErr(void)
{
    u8 i;
    stBmsGbt15Evcc_t pCache;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sBmsGbt15Get_EvccCacheData(i, &pCache) == true)
        {
            //单体动力蓄电池电压过高
            if(pCache.stBsmData.stBatFaultCode.bBatVolFault == eBsmCheckHight)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltHighErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltHighErr);
                }
            }
            //单体动力蓄电池电压过低
            if(pCache.stBsmData.stBatFaultCode.bBatVolFault == eBsmCheckLow)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltLowErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltLowErr);
                }
            }
            //整车动力蓄电池荷电状态SOC过高
            if(pCache.stBsmData.stBatFaultCode.bBatSocFault == eBsmCheckHight)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocHighErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocHighErr);
                }
            }
            //整车动力蓄电池荷电状态SOC过低
            if(pCache.stBsmData.stBatFaultCode.bBatSocFault == eBsmCheckLow)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocLowErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocLowErr);
                }
            }
            //动力蓄电池充电过流
            if(pCache.stBsmData.stBatFaultCode.bBatCurFault == eBsmCheckHight)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverCurr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverCurr);
                }
            }
            //动力蓄电池温度过高
            if(pCache.stBsmData.stBatFaultCode.bBatTempFault == eBsmCheckHight)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverTemp) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverTemp);
                }
            }
            //动力蓄电池绝缘故障
            if(pCache.stBsmData.stBatFaultCode.bBatIsulstatus == eBmsCheckfail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batImdErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batImdErr);
                }
            }
            //动力蓄电池连接器故障
            if(pCache.stBsmData.stBatFaultCode.bBatConnStatus == eBmsCheckfail)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batConnErr) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batConnErr);
                }
            } 
        }
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltHighErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltHighErr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltLowErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batVoltLowErr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocHighErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocHighErr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocLowErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batSocLowErr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverCurr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverCurr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverTemp) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batOverTemp);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batImdErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batImdErr);
            }
            
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batConnErr) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_batConnErr);
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   桩端报文超时故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_PileComTimeout(void)
{
    u8 i;
    stBmsGbt15Evcc_t pCache;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        if(sBmsGbt15Get_EvccCacheData(i, &pCache) == true)
        {
            //cro报文超时
            if(pCache.stBemData.unCode02.stBemCode02.bCroTimeout != eBmsCheckOk)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_croTimeout) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_croTimeout);
                }
            }
            else
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_croTimeout) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_croTimeout);
                }
            }
            
            //ccs报文超时
            if(pCache.stBemData.unCode03.stBemCode03.bCcsTimeout != eBmsCheckOk)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccsTimeout) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccsTimeout);
                }
            }
            else
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccsTimeout) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_ccsTimeout);
                }
            }
            
            //cst报文超时
            if(pCache.stBemData.unCode03.stBemCode03.bCstTimeout != eBmsCheckOk)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csTimeout) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csTimeout);
                }
            }
            
            if(pCache.stBemData.unCode04.stBemCode04.bCsdTimeout != eBmsCheckOk)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csdTimeout) == false)
                {
                    sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csdTimeout);
                }
            }
            
            if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csTimeout) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csTimeout);
                }
                
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csdTimeout) == true)
                {
                    sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_csdTimeout);
                }
            }
            
            //csd报文超时
        }
    }
}







/**********************************************************************************************
* Description                           :   BCL需求电压过高--枪故障
* Author                                :   Dai
* modified Date                         :   2024-11-04
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BVH(void)
{
    u8 i;
    u16 u16BhmMaxChgVol;
    u16 u16BcpMaxChgVol;
    u16 u16BclReqVolt;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_EvccMaxChgVol(i, &u16BhmMaxChgVol);
        sBmsGbt15Get_BcpMaxChgVol(i, &u16BcpMaxChgVol);
        sBmsGbt15Get_BclReqChgVol(i, &u16BclReqVolt);
        
        //应该从eeprom获取的桩端能力，eeprom没数据暂定200V测试
        if((u16BclReqVolt > u16BcpMaxChgVol) || (u16BclReqVolt > u16BhmMaxChgVol))
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclVoltHigh) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclVoltHigh);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclVoltHigh) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclVoltHigh);
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   BCL需求电压过高--枪故障
* Author                                :   Dai
* modified Date                         :   2024-11-04
* notice                                : 
***********************************************************************************************/
void sPileCnFaultCheck_BCH(void)
{
    u8 i;
    u16 u16BcpMaxChgCurr;
    u16 u16BclReqCurr;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        sBmsGbt15Get_BcpMaxChgCur(i, &u16BcpMaxChgCurr);
        sBmsGbt15Get_BclReqChgCur(i, &u16BclReqCurr);
        
        //应该从eeprom获取的桩端能力，eeprom没数据暂定200V测试
        if(u16BclReqCurr > u16BcpMaxChgCurr)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclCurrHigh) == false)
            {
                sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclCurrHigh);
            }
        }
        
        //拔枪才能恢复
        if(sAdcAppSampleGetCc1State(i) != eAdcAppCc1StateU1c)
        {
            if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclCurrHigh) == true)
            {
                sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_bclCurrHigh);
            }
        }
    }
}




