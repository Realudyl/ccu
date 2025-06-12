/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_opt.c
* Description                           :   模块电源管理用户代码
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-09-12
* notice                                :   
****************************************************************************************************/
#include "mod_app_opt.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_app_opt";











//模块功能映射表
static stModAppOpt_t stModAppOptMap[] = 
{
    //均充算法
    {
        .eType                          = eModAppTypeAvg,
        
        //api操作接口函数
        .pOpen                          = sModAppAvgOpen,
        .pClose                         = sModAppAvgClose,
        .pCloseContactor                = sModAppAvgCloseContactor,
        .pSetOutput                     = sModAppAvgSetOutput,
        .pGetUdcMax                     = sModAppAvgGetUdcMax,
        .pGetIdcMax                     = sModAppAvgGetIdcMax,
        .pGetUdcNow                     = sModAppAvgGetUdcNow,
        .pGetIdcNow                     = sModAppAvgGetIdcNow,
        .pGetIdcMaxAct                  = NULL,
        .pAttrModUpdateGunIndex         = sModAppAvgAttrModUpdateGunIndex,
        .pInitMod                       = sModAppAvgInitMod,
        
    },
    
    //柔性分配算法
    {
        .eType                          = eModAppTypeFlex,
        
        //api操作接口函数
        .pOpen                          = NULL,
        .pClose                         = NULL,
        .pCloseContactor                = NULL,
        .pSetOutput                     = NULL,
        .pGetUdcMax                     = NULL,
        .pGetIdcMax                     = NULL,
        .pGetUdcNow                     = NULL,
        .pGetIdcNow                     = NULL,
        .pGetIdcMaxAct                  = NULL,
        .pAttrModUpdateGunIndex         = NULL,
        .pInitMod                       = NULL,
        
    },
    
    //PCU控制
    {
        .eType                          = eModAppTypePcu,
        
        //api操作接口函数
        .pOpen                          = sModAppPcuOpen,
        .pClose                         = sModAppPcuClose,
        .pCloseContactor                = sModAppPcuCloseContactor,
        .pSetOutput                     = sModAppPcuSetOutput,
        .pGetUdcMax                     = sModAppPcuGetUdcMax,
        .pGetIdcMax                     = sModAppPcuGetIdcMax,
        .pGetUdcNow                     = sModAppPcuGetUdcNow,
        .pGetIdcNow                     = NULL,
        .pGetIdcMaxAct                  = NULL,
        .pAttrModUpdateGunIndex         = NULL,
        .pInitMod                       = NULL,
        
    },
};

stModAppOpt_t *pModAppOpt = &stModAppOptMap[eModAppTypeAvg];
stModAppOptCache_t stModAppOptCache;











bool sModAppOptInit(eModAppType_t eType, u32 u32P0, u32 u32P1, u32 u32P2, bool bP3);

bool sModAppOptOpen(ePileGunIndex_t eGunIndex);
bool sModAppOptClose(ePileGunIndex_t eGunIndex);
bool sModAppOptSwitchContactorY (void);
bool sModAppOptSwitchContactorAc(bool bOnOff);
bool sModAppOptSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
bool sModAppOptGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppOptGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
bool sModAppOptGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppOptGetIdcNow(ePileGunIndex_t eGunIndex, i32 *pIdc);
bool sModAppOptGetIdcMaxAct(ePileGunIndex_t eGunIndex, i32 *pIdc);
void sModAppOptAttrModUpdateGunIndex(void);
bool sModAppOptInitMod(void);











/***************************************************************************************************
* Description                           :   初始化
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   这里输入参数比较多，不一定每种投切算法都要用到所有参数
*                                           由调用者自行决定各个参数在不同投切逻辑下具体的作用。
****************************************************************************************************/
bool sModAppOptInit(eModAppType_t eType, u32 u32P0, u32 u32P1, u32 u32P2, bool bP3)
{
    bool bRst;
    
    memset(&stModAppOptCache, 0, sizeof(stModAppOptCache));
    stModAppOptCache.eIoAcContactor = eIoMax;
    switch(eType)
    {
    case eModAppTypeAvg:
        if(bP3 == false)
        {
            sIoDrvSetOutput(u32P2, SET);                                        //非低功耗模式下，需要直接闭合模块供电的交流接触器
            vTaskDelay(500 / portTICK_RATE_MS);
        }
        pModAppOpt = &stModAppOptMap[eModAppTypeAvg];
        stModAppOptCache.eIoAcContactor = u32P2;
        bRst = sModAppAvgInit(u32P0, u32P1, u32P2);
        if(bP3 == false)
        {
            sModDrvOptSetLowPowerSts(false);                                    //非低功耗模式下AC交流接触器常闭合, 底层驱动要设置非低功耗状态
            sModAppAvgInitMod();
        }
        sFaultChkRegister(sModAppOptNoneMode_NME);
        sFaultChkRegister(sModAppOptModeFault_MDE);
        sFaultChkRegister(sModAppOptModeFault_OV);
        sFaultChkRegister(sModAppOptModeFault_UV);
        break;
    case eModAppTypeFlex:
        pModAppOpt = &stModAppOptMap[eModAppTypeFlex];
        bRst = false;
        break;
    case eModAppTypePcu:
        pModAppOpt = &stModAppOptMap[eModAppTypePcu];
        bRst = sModAppPcuInit(u32P1, bP3);
        sFaultChkRegister(sModAppOptPcuFault_MCE);
        sFaultChkRegister(sModAppOptPcuFault_CPCE);
        break;
    default:
        pModAppOpt = NULL;
        bRst = false;
        break;
    }
    
    return bRst;
}







/***************************************************************************************************
* Description                           :   开启输出
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptOpen(ePileGunIndex_t eGunIndex)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pOpen == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pOpen(eGunIndex));
}







/***************************************************************************************************
* Description                           :   开启输出
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptClose(ePileGunIndex_t eGunIndex)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pClose == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pClose(eGunIndex));
}







/***************************************************************************************************
* Description                           :   Y接触器操作
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   
****************************************************************************************************/
bool sModAppOptSwitchContactorY(void)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pCloseContactor == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pCloseContactor());
}








/***************************************************************************************************
* Description                           :   AC交流接触器操作
* Author                                :   Hall
* Creat Date                            :   2024-12-18
* notice                                :   
****************************************************************************************************/
bool sModAppOptSwitchContactorAc(bool bOnOff)
{
    bool bRst;
    
    //控制交流接触器
    bRst = sIoDrvSetOutput(stModAppOptCache.eIoAcContactor, (bOnOff == true) ? SET : RESET);
    vTaskDelay(2000 / portTICK_RATE_MS);
    
    //更新底层驱动的低功耗状态，如果是闭合接触器，则还要对模块进行必要的初始化配置
    sModDrvOptSetLowPowerSts(!bOnOff);
    if(bOnOff == true)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        sModAppOptAttrModUpdateGunIndex();
        sModAppOptInitMod();
    }
    
    return bRst;
}







/***************************************************************************************************
* Description                           :   设置输出
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pSetOutput == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pSetOutput(eGunIndex, f32Udc, f32IdcLimit));
}






/***************************************************************************************************
* Description                           :   获取枪x最大输出电压
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pGetUdcMax == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pGetUdcMax(eGunIndex, pUdc));
}






/***************************************************************************************************
* Description                           :   获取枪x最大输出电流
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   这个最大电流是最高硬件配置下的电流，而不是当前实际配置下的电流
****************************************************************************************************/
bool sModAppOptGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pGetIdcMax == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pGetIdcMax(eGunIndex, pIdc));
}






/***************************************************************************************************
* Description                           :   获取枪x当前实际输出电压
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pGetUdcNow == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pGetUdcNow(eGunIndex, pUdc));
}






/***************************************************************************************************
* Description                           :   获取枪x当前实际输出电流(模块总电流)
* Author                                :   HH
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptGetIdcNow(ePileGunIndex_t eGunIndex, i32 *pIdc)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pGetIdcNow == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pGetIdcNow(eGunIndex, pIdc));
}






/***************************************************************************************************
* Description                           :   获取枪x最大输出电流---当前实际值
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   这个最大电流不是最高硬件配置下的电流，而是当前实际硬件配置下的电流
*                                           例如最多可以插4个模块，当前只插了一个模块,那就是一个模块的最大电流
****************************************************************************************************/
bool sModAppOptGetIdcMaxAct(ePileGunIndex_t eGunIndex, i32 *pIdc)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pGetIdcMaxAct == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pGetIdcMaxAct(eGunIndex, pIdc));
}






/***************************************************************************************************
* Description                           :   更新模块属性所属枪序号
* Author                                :   HH
* Creat Date                            :   2024-12-25
* notice                                :   
****************************************************************************************************/
void sModAppOptAttrModUpdateGunIndex(void)
{
    if((pModAppOpt != NULL) && (pModAppOpt->pAttrModUpdateGunIndex != NULL))
    {
        pModAppOpt->pAttrModUpdateGunIndex();
    }
}






/***************************************************************************************************
* Description                           :   模块配置初始化
* Author                                :   HH
* Creat Date                            :   2024-12-25
* notice                                :   
****************************************************************************************************/
bool sModAppOptInitMod(void)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pInitMod == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pInitMod());
}





















