/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_pcu_fault.c
* Description                           :   PCU相关故障实现 & 模块相关故障实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-09-12
* notice                                :   
****************************************************************************************************/
#include "mod_app_pcu.h"
#include "mod_app_avg.h"
#include "mod_app_flex.h"









extern stModAppAvgCache_t *pModAppAvgCache;








void sModAppOptPcuFault_MCE(void);
void sModAppOptPcuFault_CPCE(void);
void sModAppOptNoneMode_NME(void);
void sModAppOptModeFault_MDE(void);
void sModAppOptModeFault_OV(void);
void sModAppOptModeFault_UV(void);











/**********************************************************************************************
* Description                           :   主柜功率故障---枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sModAppOptPcuFault_MCE(void)
{
    u8   i;
    i32  i32UdcMax;
    i32  i32IdcMax;
    static u32 u32TimeStamp[ePileGunIndexNum];
    static u32 u32StopTimeStamp[ePileGunIndexNum];
    
    for(i = ePileGunIndex0; i < cPrivDrvGunNumMax; i++)
    {
        //获取Pcu最大可提供的电压
        if((sModAppPcuGetUdcMax(i, &i32UdcMax) == true)
        && (sModAppPcuGetIdcMax(i, &i32IdcMax ) == true))
        {
            
            if((sModAppPcuGetEnergyErrSt(i) == true) || ((i32UdcMax == 0) &&  (i32IdcMax == 0)))
            {
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_mainCabErr) == false)
                {
                    if(xTaskGetTickCount() - u32TimeStamp[i] >= 15000)
                    {
                        u32StopTimeStamp[i] = xTaskGetTickCount();
                        sFaultCodeAdd(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_mainCabErr);
                    }
                }
            }
            else
            {
                u32TimeStamp[i] = xTaskGetTickCount();
                if(sFaultCodeChkCode(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_mainCabErr) == true)
                {
                    if(xTaskGetTickCount() - u32StopTimeStamp[i] >= 5000)
                    {
                        sFaultCodeSub(i + cPrivDrvGunIdBase, ePrivDrvFaultCode_mainCabErr);
                    }
                }
            }
        }
    }
}












/**********************************************************************************************
* Description                           :   主柜通讯故障--枪故障
* Author                                :   Dai
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sModAppOptPcuFault_CPCE(void)
{
    u8 i;
    ePcuCommState_t pCommSt;
    static u32 u32StopTimeStamp;
    
    for(i = ePileGunIndex0; i < cPrivDrvGunNumMax; i++)
    {
        sModAppPcuGetCommSt(i, &pCommSt);
        if(pCommSt == ePcuCommStateFailed)
        {
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuPcuComErr) == false)
            {
                u32StopTimeStamp = xTaskGetTickCount();
                sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuPcuComErr);
            }
        }
        else
        {
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuPcuComErr) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
                {
                    sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_ccuPcuComErr);
                }
            }
        }
    }
}







/**********************************************************************************************
* Description                           :   模块通讯故障--无可用模块故障
* Author                                :   HH
* modified Date                         :   2024-06-18
* notice                                : 
***********************************************************************************************/
void sModAppOptNoneMode_NME(void)
{
    i32  i;
    u8 u8Total;
    i8   i8AddrList[cModAppAvgModNumMax];                                       //在线模块地址列表
    static u32 u32TimeStamp;
    static u32 u32StopTimeStamp;
    
    u8Total = 0;
    memset(i8AddrList, cModAppAvgModAddrInvalid, sizeof(i8AddrList));           //先全部设置无效值
    if(sModDrvOptGetOnlineAddrList(sizeof(i8AddrList), i8AddrList) == true)     //获取在线模块地址列表
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(i8AddrList[i] == cModAppAvgModAddrInvalid)
            {
                u8Total++;
            }
        }
    }

    if(sEepromAppGetBlockFactoryLowPowerFlag() == false)
    {
        if(u8Total == cModAppAvgModNumMax)
        {
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_devProtect) == false)
            {
                if(xTaskGetTickCount() - u32TimeStamp >= 15000)
                {
                    u32StopTimeStamp = xTaskGetTickCount();
                    sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_devProtect);
                }
            }
        }
        else
        {
            u32TimeStamp = xTaskGetTickCount();
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_devProtect) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
                {
                    sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_devProtect);
                }
            }
        }
    }
}






/**********************************************************************************************
* Description                           :   模块故障-短路故障
* Author                                :   HH
* modified Date                         :   2024-06-18
* notice                                :   短路保护等故障桩端无法检测,需要通过模块保护
后给到对应的故障信息去做保护。短路故障不可自恢复，需要断电重启
***********************************************************************************************/
void sModAppOptModeFault_MDE(void)
{
    static u32 u32TimeStamp;
    stModAppAvgAttrModule_t *pMod = NULL;
    u32 i = 0;
    bool bFaultFlag = false;
    
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    //如果模块类型是永联模块,默认永联模块,未用eerprom存储的模块类型做区分,待使用后可以用到
    for(i = 0; i < pModAppAvgCache->i32ModNum; i++)
    {
        pMod = &pCache->stAttrModule[i];
        if(1)
        {
            //永联模块短路保护故障位和EN模块不一样
            if(pMod->u8StatusCode[3] & 0x10)
            {
                bFaultFlag = true;
                if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_dcShort) == false)
                {
                    if(xTaskGetTickCount() - u32TimeStamp >= 100)
                    {
                        sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_dcShort);
                        break;
                    }
                }
            }
        }
        else
        {
            //EN模块,短路故障位
            if(pMod->u8StatusCode[3] & 0x80)
            {
                if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_dcShort) == false)
                {
                    if(xTaskGetTickCount() - u32TimeStamp >= 100)
                    {
                        sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_dcShort);
                        break;
                    }
                }
            }
        }
    }
    
    if(!bFaultFlag)
    {
        u32TimeStamp = xTaskGetTickCount();
    }
}






/**********************************************************************************************
* Description                           :   模块故障--过压故障
* Author                                :   HH
* modified Date                         :   2024-06-18
* notice                                :   过压故障等故障桩端无法检测,需要通过模块保护
后给到对应的故障信息去做保护
***********************************************************************************************/
void sModAppOptModeFault_OV(void)
{
    i32  i;
    u8 u8Total;
    i8   i8AddrList[cModAppAvgModNumMax];                                       //在线模块地址列表
    static u32 u32TimeStamp;
    static u32 u32StopTimeStamp;
    f32 f32Uab, f32Ubc, f32Uca, f32UMax;
    
    u8Total = 0;
    f32Uab  = 0;
    memset(i8AddrList, cModAppAvgModAddrInvalid, sizeof(i8AddrList));           //先全部设置无效值
    if(sModDrvOptGetOnlineAddrList(sizeof(i8AddrList), i8AddrList) == true)     //获取在线模块地址列表
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(i8AddrList[i] == cModAppAvgModAddrInvalid)
            {
                u8Total++;
            }
            else
            {
                sModDrvOptGetUacInput(i, &f32Uab, &f32Ubc, &f32Uca);
            }
        }
    }
    
    //读取到正常电压后检测,电压大于267V报故障,小于260V恢复
    if(f32Uab > 100)
    {
        f32UMax = f32Uab / 1.73;
        
        if(f32UMax > 252)
        {
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_acInputOverVoltage) == false)
            {
                if(xTaskGetTickCount() - u32TimeStamp >= 5000)
                {
                    u32StopTimeStamp = xTaskGetTickCount();
                    sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_acInputOverVoltage);
                }
            }
        }
        else if(f32UMax < 260)
        {
            u32TimeStamp = xTaskGetTickCount();
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_acInputOverVoltage) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
                {
                    sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_acInputOverVoltage);
                }
            }
        }
    }
}





/**********************************************************************************************
* Description                           :   模块故障--欠压故障
* Author                                :   HH
* modified Date                         :   2024-06-18
* notice                                :   欠压故障等故障桩端无法检测,需要通过模块保护
后给到对应的故障信息去做保护
***********************************************************************************************/
void sModAppOptModeFault_UV(void)
{
    i32  i;
    u8 u8Total;
    i8   i8AddrList[cModAppAvgModNumMax];                                       //在线模块地址列表
    static u32 u32TimeStamp;
    static u32 u32StopTimeStamp;
    f32 f32Uab, f32Ubc, f32Uca, f32UMax;
    
    if(sEepromAppGetBlockFactoryLowPowerFlag() == true)
    {
        return;
    }
    
    u8Total = 0;
    f32Uab  = 0;
    memset(i8AddrList, cModAppAvgModAddrInvalid, sizeof(i8AddrList));           //先全部设置无效值
    if(sModDrvOptGetOnlineAddrList(sizeof(i8AddrList), i8AddrList) == true)     //获取在线模块地址列表
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(i8AddrList[i] == cModAppAvgModAddrInvalid)
            {
                u8Total++;
            }
            else
            {
                sModDrvOptGetUacInput(i, &f32Uab, &f32Ubc, &f32Uca);
            }
        }
    }
    
    //读取到正常电压后检测,电压小于160V报故障,大于167V恢复
    if(f32Uab > 100)
    {
        f32UMax = f32Uab / 1.73;
        
        if(f32UMax < 160)
        {
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_acUndervolt) == false)
            {
                if(xTaskGetTickCount() - u32TimeStamp >= 5000)
                {
                    u32StopTimeStamp = xTaskGetTickCount();
                    sFaultCodeAdd(cPrivDrvGunIdChg, ePrivDrvFaultCode_acUndervolt);
                }
            }
        }
        else if(f32UMax > 190)
        {
            u32TimeStamp = xTaskGetTickCount();
            if(sFaultCodeChkCode(cPrivDrvGunIdChg, ePrivDrvFaultCode_acUndervolt) == true)
            {
                if(xTaskGetTickCount() - u32StopTimeStamp >= 5000)
                {
                    sFaultCodeSub(cPrivDrvGunIdChg, ePrivDrvFaultCode_acUndervolt);
                }
            }
        }
    }
}








