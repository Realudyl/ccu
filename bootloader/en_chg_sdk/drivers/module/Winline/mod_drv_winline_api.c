/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_drv_winline_api.c
* Description                           :   永联科技 NXR 系列充电模块CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-04-02
* notice                                :   基于  <NXR 系列充电模块通讯协议 V1.48>  设计
****************************************************************************************************/
#include "mod_drv_winline_api.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_drv_winline_api";



extern stModDrvWinlineCache_t *pModDrvWinlineCache;









stModDrvWinlineData_t *sModDrvWinlineDataSearch(u8 u8Addr);


//对外的api接口
bool  sModDrvWinlineGetOnlineAddrList(u8 u8MaxNum, i8 *pList);

bool  sModDrvWinlineOpen(u8 u8Addr, bool bGroupFlag);
bool  sModDrvWinlineClose(u8 u8Addr, bool bGroupFlag);
bool  sModDrvWinlineSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
bool  sModDrvWinlineSetGroup(u8 u8Addr, u32 u32Group);
bool  sModDrvWinlineSetAddrMode(u8 u8Addr, eModDrvWinlineAddrMode_t eMode);
bool  sModDrvWinlineSetAltitude(u8 u8Addr, u32 u32Altitude);

bool  sModDrvWinlineGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
bool  sModDrvWinlineGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
bool  sModDrvWinlineGetGroup(u8 u8Addr, u32 *pGroup);
bool  sModDrvWinlineGetAddrBySwitch(u8 u8Addr, u32 *pAddrBySwitch);
bool  sModDrvWinlineGetAltitude(u8 u8Addr, u32 *pAltitude);
bool  sModDrvWinlineGetFanSpeed(u8 u8Addr, u8 *pFanS);
bool  sModDrvWinlineGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
bool  sModDrvWinlineGetTenv(u8 u8Addr, f32 *pTenv);
bool  sModDrvWinlineGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
bool  sModDrvWinlineGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
bool  sModDrvWinlineGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
bool  sModDrvWinlineGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);
















/***************************************************************************************************
* Description                           :   根据地址搜索对应的缓存数据地址
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
****************************************************************************************************/
stModDrvWinlineData_t *sModDrvWinlineDataSearch(u8 u8Addr)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    //0:限幅
    if(u8Addr > cModDrvWinlineAddrMax)
    {
        return(NULL);
    }
    
    //2:根据在线的模块列表 返回数据
    pModData = (pModDrvWinlineCache->stData[u8Addr].bModExistSts == false) ? NULL : (&pModDrvWinlineCache->stData[u8Addr]);
    
    return(pModData);
}










/***************************************************************************************************
* Description                           :   模块电源驱动->获取在线模块地址的list
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   u8MaxNum：pList指针指向的内存空间长度
*                                           pList   ：指向一个内存空间 用于接收返回的地址列表
****************************************************************************************************/
bool sModDrvWinlineGetOnlineAddrList(u8 u8MaxNum, i8 *pList)
{
    u8   u8Index;
    i32  i;
    
    //2:填充 pList
    u8Index = 0;
    for(i = 0; i < cModDrvWinlineAddrNumMax; i++)
    {
        if(pModDrvWinlineCache->stData[i].bModExistSts == true)
        {
            // i 对应模块的地址  如果该模块在线 则将地址填充到 pList[u8Index]
            pList[u8Index] = i;
            u8Index++;
            if(u8Index >= u8MaxNum)
            {
                //如果上层提供的 存储空间填满 即便 i后面还有模块 也返回
                //这样当CAN总线上有很多模块时 驱动能忽略多余模块 这些模块将不会参与输出
                break;
            }
        }
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->开启输出
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineOpen(u8 u8Addr, bool bGroupFlag)
{
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.eTurn = eModDrvWinlineTurnOn;
    
    return sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetTurnOnOff, unPayload, true);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->关闭输出
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineClose(u8 u8Addr, bool bGroupFlag)
{
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.eTurn = eModDrvWinlineTurnOff;
    
    return sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetTurnOnOff, unPayload, true);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->设置电源输出电压和限流点
* Author                                :   Hall
* Creat Date                            :   2024-04-08
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit)
{
    bool bRst = true;
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.unValue.f32Value = f32Udc;
    bRst = bRst & sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetUdcOut, unPayload, true);
    
    unPayload.u32Value = (u32)(f32IdcLimit * 1024);
    bRst = bRst & sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetIdcOut, unPayload, true);
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->设置组号
* Author                                :   anlada
* Creat Date                            :   2024-09-3
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineSetGroup(u8 u8Addr, u32 u32Group)
{
    bool bRet ;
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.u32Group = u32Group;
    
    //永联模块地址模式为拨码开关设定的情况下 才允许指令设置组号
    //为防止此模块是热插入模块且地址模式不是拨码开关设定导致无法设置组号 需要先设置地址模式
    bRet  = sModDrvWinlineSetAddrMode(u8Addr, eModDrvWinlineAddrModeSwitch);
    bRet &= sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetGroup, unPayload, true);
    
    return bRet;
}






/***************************************************************************************************
* Description                           :   模块电源驱动->设置地址分配方式。实际上是模块组号设置方式
* Author                                :   anlada
* Creat Date                            :   2024-09-3
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineSetAddrMode(u8 u8Addr, eModDrvWinlineAddrMode_t eMode)
{
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.u32Value = eMode;
    
    return sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetAddrMode, unPayload, true);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->设置的工作海拔
* Author                                :   anlada
* Creat Date                            :   2024-09-3
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineSetAltitude(u8 u8Addr, u32 pAltitude)
{
    unModDrvWinlinePayload_t unPayload;
    
    unPayload.u32Value = pAltitude;
    
    return sModDrvWinlineSend(u8Addr, eModDrvWinlineFunSet, eModDrvWinlineRegSetTurnOnOff, unPayload, true);
}









/***************************************************************************************************
* Description                           :   模块电源驱动->输入输出特性曲线
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   此参数模块协议并未传送，这里采用程序内置的方式上报
****************************************************************************************************/
bool sModDrvWinlineGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc)
{
    if(pUdcMax != NULL)
    {
        (*pUdcMax) = cModDrvWinlineUdcMax;
    }
    if(pUdcMin != NULL)
    {
        (*pUdcMin) = cModDrvWinlineUdcMin;
    }
    if(pIdcMax != NULL)
    {
        (*pIdcMax) = cModDrvWinlineIdcMax;
    }
    if(pPdc != NULL)
    {
        (*pPdc) = cModDrvWinlinePdc;
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取当前输出电压和电流
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(pUdc != NULL)
        {
            (*pUdc) = pModData->f32UdcOut;
        }
        if(pIdc != NULL)
        {
            (*pIdc) = pModData->f32IdcOut;
        }
        
        return(true);
    }
    
    return(false);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取组号
* Author                                :   anlada
* Creat Date                            :   2024-09-4
* notice                                :   u32AddrBySwitch:其中后16位是模块地址，前16位是模块组号
****************************************************************************************************/
bool sModDrvWinlineGetGroup(u8 u8Addr, u32 *pGroup)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(NULL != pModData)
        {
            *pGroup = (pModData->u32AddrBySwitch & 0xffff0000) >> 16;
            return(true);
        }
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取拨码地址。
* Author                                :   anlada
* Creat Date                            :   2024-09-11
* notice                                :   u32AddrBySwitch:其中后16位是模块地址，前16位是模块组号
****************************************************************************************************/
bool sModDrvWinlineGetAddrBySwitch(u8 u8Addr, u32 *pAddrBySwitch)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(NULL != pModData)
        {
            *pAddrBySwitch = (pModData->u32AddrBySwitch & 0x0000ffff);
            return(true);
        }
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   模块电源驱动->读取工作海拔，读取缓存中的值
* Author                                :   anlada
* Creat Date                            :   2024-09-3
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetAltitude(u8 u8Addr, u32 *pAltitude)
{
    bool bRst;
    unModDrvWinlinePayload_t unPayload;
    stModDrvWinlineData_t *pModData = NULL;
    
    bRst = sModDrvWinlineSend(u8Addr, eModDrvWinlineFunGet, eModDrvWinlineRegGetAltitude, unPayload, true);
    if((bRst == true) && ((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL))
    {
        if(NULL != pAltitude)
        {
            *pAltitude = pModData->u32Altitude;
            return(true);
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取风扇转速等效值0~100对应转速比0~1
* Author                                :   anlada
* Creat Date                            :   2024-06-7
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetFanSpeed(u8 u8Addr, u8 *pFanS)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(NULL != pFanS)
        {
            //风扇转速等效值0~2400对应转速比0~1,转换为0~100对应转速比0~1，误差小于1%
            *pFanS = (u8)(pModData->u32FanS / 24);
            return(true);
        }
    }
    
    return(false);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取错误码、告警码、状态码...
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if((pCode != NULL) && (sizeof(pModData->stWarningCode) <= u8MaxLen))
        {
            memcpy(pCode, &pModData->stWarningCode, sizeof(pModData->stWarningCode));
            
            return(true);
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取环境温度
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetTenv(u8 u8Addr, f32 *pTenv)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(pTenv != NULL)
        {
            (*pTenv) = pModData->f32Tenv;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取温度详情
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(u8MaxLen >= 2)
        {
            //永联模块温度详情有2个温度值
            if(pTemp != NULL)
            {
                pTemp[0] = (f32)pModData->f32Tdc;                               //模块 DC  板温度
                pTemp[1] = (f32)pModData->f32Tpfc;                              //模块 PFC 板温度
                
                return(true);
            }
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入电压
* Author                                :   Hall
* Creat Date                            :   2024-04-09
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(pUab != NULL)
        {
            (*pUab) = pModData->f32Ua;
        }
        if(pUbc != NULL)
        {
            (*pUbc) = pModData->f32Ub;
        }
        if(pUca != NULL)
        {
            (*pUca) = pModData->f32Uc;
        }
        
        return(true);
    }
    
    return(false);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入功率和频率
* Author                                :   Hall
* Creat Date                            :   2024-04-10
* notice                                :   永联模块没有频率字段
****************************************************************************************************/
bool sModDrvWinlineGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac)
{
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(pPac != NULL)
        {
            (*pPac) = (f32)pModData->u32PowerInput;
        }
        
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入电压
* Author                                :   Hall
* Creat Date                            :   2024-04-10
* notice                                :   
****************************************************************************************************/
bool sModDrvWinlineGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer)
{
    char u8SwVer[32];
    stModDrvWinlineData_t *pModData = NULL;
    
    if((pModData = sModDrvWinlineDataSearch(u8Addr)) != NULL)
    {
        if(pSwVer != NULL)
        {
            memset(u8SwVer, 0, sizeof(u8SwVer));
            snprintf(u8SwVer, sizeof(u8SwVer), "DC.Ver:V%d, Pfc.Ver:V%d", pModData->u32SwVerDcdc, pModData->u32SwVerPfc);
            if(u8MaxLen >= strlen(u8SwVer))
            {
                memcpy(pSwVer, u8SwVer, strlen(u8SwVer));
                return(true);
            }
        }
    }
    
    return(false);
}


































