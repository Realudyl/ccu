/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   mod_drv_en_api.c
* Description                           :   EN+模块电源CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   
****************************************************************************************************/
#include "mod_drv_en_api.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_en_drv_api";



extern stModDrvEnCache_t *pModDrvEnCache;








stModDrvEnData_t *sModDrvEnDataSearch(u8 u8Addr);


//对外的api接口
bool  sModDrvEnGetOnlineAddrList(u8 u8MaxNum, i8 *pList);

bool  sModDrvEnOpen(u8 u8Addr, bool bGroupFlag);
bool  sModDrvEnClose(u8 u8Addr, bool bGroupFlag);
bool  sModDrvEnSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit);
bool  sModDrvEnSetGroupNum(u8 u8Addr, u32 u32GroupNum);

bool  sModDrvEnGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc);
bool  sModDrvEnGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc);
bool  sModDrvEnGetFanSpeed(u8 u8Addr, u8 *pSpeed);
bool  sModDrvEnGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode);
bool  sModDrvEnGetTenv(u8 u8Addr, f32 *pTenv);
bool  sModDrvEnGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp);
bool  sModDrvEnGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca);
bool  sModDrvEnGetWorkStatus(u8 u8Addr, u8 *pStatus);
bool  sModDrvEnGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime);
bool  sModDrvEnGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc);
bool  sModDrvEnGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac);
bool  sModDrvEnGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime);
bool  sModDrvEnGetMaterialNum(u8 u8Addr, u32 *pNum);
bool  sModDrvEnGetMatchCode(u8 u8Addr, u16 *pCode);
bool  sModDrvEnGetModelType(u8 u8Addr, u32 *pType);
bool  sModDrvEnGetFeatureWord(u8 u8Addr, u32 *pWord);
bool  sModDrvEnGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum);
bool  sModDrvEnGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName);
bool  sModDrvEnGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId);
bool  sModDrvEnGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer);

















/***************************************************************************************************
* Description                           :   根据地址搜索对应的缓存数据地址
* Author                                :   Hall
* Creat Date                            :   2024-03-06
* notice                                :   
****************************************************************************************************/
stModDrvEnData_t *sModDrvEnDataSearch(u8 u8Addr)
{
    stModDrvEnData_t *pModData = NULL;
    
    //0:限幅
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrMax))
    {
        return(NULL);
    }
    
    //2:根据在线的模块列表 返回数据
    pModData = (pModDrvEnCache->stData[u8Addr - 1].bModExistSts == false) ? NULL : (&pModDrvEnCache->stData[u8Addr - 1]);
    
    return(pModData);
}










/***************************************************************************************************
* Description                           :   模块电源驱动->获取在线模块地址的list
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   u8MaxNum：pList指针指向的内存空间长度
*                                           pList   ：指向一个内存空间 用于接收返回的地址列表
****************************************************************************************************/
bool sModDrvEnGetOnlineAddrList(u8 u8MaxNum, i8 *pList)
{
    u8   u8Index;
    i32  i;
    
    //填充 pList
    u8Index = 0;
    for(i = 0; i < cModDrvEnAddrMax; i++)
    {
        if(pModDrvEnCache->stData[i].bModExistSts == true)
        {
            // i 对应模块的地址  如果该模块在线 则将地址填充到 pList[u8Index]
            pList[u8Index] = i + 1;
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
* Creat Date                            :   2022-08-04
* notice                                :   
****************************************************************************************************/
bool sModDrvEnOpen(u8 u8Addr, bool bGroupFlag)
{
    return sModDrvEnTxCmd1A(u8Addr, (bGroupFlag == true) ? eModDrvEnDevNum0B : eModDrvEnDevNum0A, eModDrvEnTurnValueOn);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->关闭输出
* Author                                :   Hall
* Creat Date                            :   2022-08-04
* notice                                :   
****************************************************************************************************/
bool sModDrvEnClose(u8 u8Addr, bool bGroupFlag)
{
    return sModDrvEnTxCmd1A(u8Addr, (bGroupFlag == true) ? eModDrvEnDevNum0B : eModDrvEnDevNum0A, eModDrvEnTurnValueOff);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->设置电源输出电压和限流点
* Author                                :   Hall
* Creat Date                            :   2022-08-06
* notice                                :   
****************************************************************************************************/
bool sModDrvEnSetOutput(u8 u8Addr, bool bGroupFlag, f32 f32Udc, f32 f32IdcLimit)
{
    //单位转换
    return sModDrvEnTxCmd1C(u8Addr, (bGroupFlag == true) ? eModDrvEnDevNum0B : eModDrvEnDevNum0A, (u32)(f32Udc * 1000), (u32)(f32IdcLimit * 1000));
}








/***************************************************************************************************
* Description                           :   模块电源驱动->设置电源组号
* Author                                :   Hall
* Creat Date                            :   2024-03-21
* notice                                :   
****************************************************************************************************/
bool sModDrvEnSetGroupNum(u8 u8Addr, u32 u32GroupNum)
{
    return sModDrvEnTxCmd16(u8Addr, eModDrvEnDevNum0A, u32GroupNum);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->输入输出特性曲线
* Author                                :   Hall
* Creat Date                            :   2024-03-26
* notice                                :   此参数模块协议并未传送，这里采用程序内置的方式上报
****************************************************************************************************/
bool sModDrvEnGetCurve(f32 *pUdcMax, f32 *pUdcMin, f32 *pIdcMax, f32 *pPdc)
{
    if(pUdcMax != NULL)
    {
        (*pUdcMax) = cModDrvEnUdcMax;
    }
    if(pUdcMin != NULL)
    {
        (*pUdcMin) = cModDrvEnUdcMin;
    }
    if(pIdcMax != NULL)
    {
        (*pIdcMax) = cModDrvEnIdcMax;
    }
    if(pPdc != NULL)
    {
        (*pPdc) = cModDrvEnPdc;
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->获取当前输出电压和电流
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetOutput(u8 u8Addr, f32 *pUdc, f32 *pIdc)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pUdc != NULL)
        {
            (*pUdc) = pModData->f32Udc;
        }
        if(pIdc != NULL)
        {
            (*pIdc) = pModData->f32Idc;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取风扇转速
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetFanSpeed(u8 u8Addr, u8 *pSpeed)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pSpeed != NULL)
        {
            (*pSpeed) = pModData->u8FanSpeed;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取错误码
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetErrCode(u8 u8Addr, u8 u8MaxLen, u8 *pCode)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if((pCode != NULL) && (sizeof(pModData->unErrCode) <= u8MaxLen))
        {
            memcpy(pCode, &pModData->unErrCode, sizeof(pModData->unErrCode));
            
            return(true);
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取环境温度
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetTenv(u8 u8Addr, f32 *pTenv)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pTenv != NULL)
        {
            (*pTenv) = (f32)pModData->i8Tenv;
        }
        
        return(true);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取温度详情
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetTempDetail(u8 u8Addr, u8 u8MaxLen, f32 *pTemp)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(u8MaxLen >= 8)
        {
            //温度详情有8个温度值
            if(pTemp != NULL)
            {
                pTemp[0] = (f32)pModData->i8Tmos1;                              //MOS 管1 温度
                pTemp[1] = (f32)pModData->i8Tmos2;                              //MOS 管2 温度
                pTemp[2] = (f32)pModData->i8Tdiod1;                             //二极管1 温度
                pTemp[3] = (f32)pModData->i8Tdiod2;                             //二极管2 温度
                pTemp[4] = (f32)pModData->i8Ttsf;                               //变压器  温度
                pTemp[5] = (f32)pModData->i8Tcpu;                               //CPU     温度
                pTemp[6] = (f32)pModData->i8Trad1;                              //散热片1 温度
                pTemp[7] = (f32)pModData->i8Trad2;                              //散热片1 温度
                
                return(true);
            }
        }
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入电压
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetUacInput(u8 u8Addr, f32 *pUab, f32 *pUbc, f32 *pUca)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pUab != NULL)
        {
            (*pUab) = ((f32)pModData->u16Uab) / 10;
        }
        if(pUbc != NULL)
        {
            (*pUbc) = ((f32)pModData->u16Ubc) / 10;
        }
        if(pUca != NULL)
        {
            (*pUca) = ((f32)pModData->u16Uca) / 10;
        }
        
        return(true);
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取工作状态
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetWorkStatus(u8 u8Addr, u8 *pStatus)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pStatus != NULL)
        {
            (*pStatus) = pModData->eState1;
        }
        
        return(true);
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取运行时间
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetRunTime(u8 u8Addr, u32 *pHtotal, u32 *pHthistime)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pHtotal != NULL)
        {
            (*pHtotal) = pModData->u32Htotal;
        }
        if(pHthistime != NULL)
        {
            (*pHthistime) = pModData->u32Hthistime;
        }
        
        return(true);
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入电流
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetIacInput(u8 u8Addr, f32 *pIa, f32 *pIb, f32 *pIc)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pIa != NULL)
        {
            (*pIa) = ((f32)pModData->u16Ia) / 10;
        }
        if(pIb != NULL)
        {
            (*pIb) = ((f32)pModData->u16Ib) / 10;
        }
        if(pIc != NULL)
        {
            (*pIc) = ((f32)pModData->u16Ic) / 10;
        }
        
        return(true);
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取输入功率和输入频率
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetPacFac(u8 u8Addr, f32 *pPac, f32 *pFac)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pPac != NULL)
        {
            (*pPac) = ((f32)pModData->u16Pac) / 10;
        }
        if(pFac != NULL)
        {
            (*pFac) = ((f32)pModData->u16Fac) / 100;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取电量信息
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetEnergy(u8 u8Addr, u32 *pEtotal, u32 *pEthistime)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pEtotal != NULL)
        {
            (*pEtotal) = pModData->u32Etotal;
        }
        if(pEthistime != NULL)
        {
            (*pEthistime) = pModData->u32Ethistime;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取软件物料代码
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetMaterialNum(u8 u8Addr, u32 *pNum)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pNum != NULL)
        {
            (*pNum) = pModData->u32MaterialNum;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取软件匹配码
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetMatchCode(u8 u8Addr, u16 *pCode)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pCode != NULL)
        {
            (*pCode) = pModData->u16MatchCode;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取机型
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetModelType(u8 u8Addr, u32 *pType)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pType != NULL)
        {
            (*pType) = pModData->u16ModelType;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取模块特征字
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetFeatureWord(u8 u8Addr, u32 *pWord)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(pWord != NULL)
        {
            (*pWord) = pModData->unFeatureWord.u32Value;
        }
        
        return(true);
    }
    
    return(false);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->获取序列号
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetSerialNum(u8 u8Addr, u8 u8MaxLen, char *pSerialNum)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(sizeof(pModData->u8SerialNum) <= u8MaxLen)
        {
            if(pSerialNum != NULL)
            {
                memcpy(pSerialNum, pModData->u8SerialNum, sizeof(pModData->u8SerialNum));
                
                return(true);
            }
        }
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取model name
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetModelName(u8 u8Addr, u8 u8MaxLen, char *pModelName)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(sizeof(pModData->u8ModelName) <= u8MaxLen)
        {
            if(pModelName != NULL)
            {
                memcpy(pModelName, pModData->u8ModelName, sizeof(pModData->u8ModelName));
                
                return(true);
            }
        }
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->获取制造商ID
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetManufacturerId(u8 u8Addr, u8 u8MaxLen, char *pId)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(sizeof(pModData->u8ManufacturerId) <= u8MaxLen)
        {
            if(pId != NULL)
            {
                memcpy(pId, pModData->u8ManufacturerId, sizeof(pModData->u8ManufacturerId));
                
                return(true);
            }
        }
    }
    
    return(false);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->软件版本
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnGetSwVer(u8 u8Addr, u8 u8MaxLen, char *pSwVer)
{
    stModDrvEnData_t *pModData = NULL;
    
    if((pModData = sModDrvEnDataSearch(u8Addr)) != NULL)
    {
        if(sizeof(pModData->u8SwVer) <= u8MaxLen)
        {
            if(pSwVer != NULL)
            {
                memcpy(pSwVer, pModData->u8SwVer, sizeof(pModData->u8SwVer));
                
                return(true);
            }
        }
    }
    
    return(false);
}









