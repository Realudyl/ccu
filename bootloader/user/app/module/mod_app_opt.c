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
        .pGetIdcMaxAct                  = NULL,
        
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
        .pGetIdcMaxAct                  = NULL,
        
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
        .pGetIdcMaxAct                  = NULL,
        
    },
};

stModAppOpt_t *pModAppOpt = &stModAppOptMap[eModAppTypeAvg];











bool sModAppOptInit(eModAppType_t eType, u32 u3P1, u32 u3P2, bool bP3);

bool sModAppOptOpen(ePileGunIndex_t eGunIndex);
bool sModAppOptClose(ePileGunIndex_t eGunIndex);
bool sModAppOptCloseContactor(void);
bool sModAppOptSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
bool sModAppOptGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppOptGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
bool sModAppOptGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppOptGetIdcMaxAct(ePileGunIndex_t eGunIndex, i32 *pIdc);











/***************************************************************************************************
* Description                           :   初始化
* Author                                :   Hall
* Creat Date                            :   2024-09-13
* notice                                :   
****************************************************************************************************/
bool sModAppOptInit(eModAppType_t eType, u32 u3P1, u32 u3P2, bool bP3)
{
    bool bRst;
    
    switch(eType)
    {
    case eModAppTypeAvg:
        pModAppOpt = &stModAppOptMap[eModAppTypeAvg];
        bRst = sModAppAvgInit(u3P1, u3P2);
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
        bRst = sModAppPcuInit(u3P1, bP3);
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
* Description                           :   切断接触器
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   
****************************************************************************************************/
bool sModAppOptCloseContactor(void)
{
    bool bCondition = ((pModAppOpt == NULL) || (pModAppOpt->pCloseContactor == NULL));
    
    return ((bCondition == true) ? false : pModAppOpt->pCloseContactor());
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





















