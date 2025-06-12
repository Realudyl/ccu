/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_opt.c
* Description                           :   V2G 应用层消息 协议管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   用于 V2gMsg 解析时指向 ISO15118 或 DIN70121,供底层调用
****************************************************************************************************/
#include "bms_ccs_v2g_opt.h"

#include "bms_ccs_v2g_din.h"
#include "bms_ccs_v2g_din_api.h"

#include "bms_ccs_v2g_iso1.h"
#include "bms_ccs_v2g_iso1_api.h"


#include "bms_ccs_net.h"




stV2gOptCache_t *pV2gOptCache[cMse102xDevNum];



//---------------------------------------------------------------------------------------------------------



//V2G 应用协议管理map
stV2gOptMap_t stV2gOptMap[eBmsCcsNetV2gTypeMax] =
{
    //DIN70121
    {
        .eType                              =   eBmsCcsNetV2gTypeDin,
        .pDescString                        =   "DIN70121",
        .pNameSpace                         =   "urn:din:70121:2012:MsgDef",
        .pInit                              =   sV2gDinInit,
        .pCacheInit                         =   sV2gDinCacheInit,
        .pProc                              =   sV2gDinProc,
        .pSetCpState                        =   sV2gDinSetCpState,
        .pGetSessionStop                    =   sV2gDinGetSessionStop,
        .pSetResponseCodeForCpStateTimeout  =   sV2gDinSetResponseCodeForCpStateDetectionTimeout,
        
    },
    
    //ISO15118-2 2013---IS015118版本1，市场主流版本
    {
        .eType                              =   eBmsCcsNetV2gTypeIso1,
        .pDescString                        =   "ISO15118v1",
        .pNameSpace                         =   "urn:iso:15118:2:2013:MsgDef",
        .pInit                              =   sV2gIso1Init,
        .pCacheInit                         =   sV2gIso1CacheInit,
        .pProc                              =   sV2gIso1Proc,
        .pSetCpState                        =   sV2gIso1SetCpState,
        .pGetSessionStop                    =   sV2gIso1GetSessionStop,
        .pSetResponseCodeForCpStateTimeout  =   sV2gIso1SetResponseCodeForCpStateDetectionTimeout,
        
    },
    
    //15118-2 2016 标准----IS015118版本2, ISO15118-20发布以后, 此版本已经废弃 不予考虑
    {
        .eType                              =   eBmsCcsNetV2gTypeIso2,
        .pDescString                        =   "ISO15118v2",
        .pNameSpace                         =   "urn:iso:15118:2:2016:MsgDef",
        .pInit                              =   NULL,
        .pCacheInit                         =   NULL,
        .pProc                              =   NULL,
        .pSetCpState                        =   NULL,
        .pGetSessionStop                    =   NULL,
        .pSetResponseCodeForCpStateTimeout  =   NULL,
        
    }
    
};





//---------------------------------------------------------------------------------------------------------

bool  sV2gXxxInit(i32 i32DevIndex, stV2gOptMap_t *pMap);
stV2gOptMap_t *sV2gTypeMatch(i32 i32Len, const char *pNamespace);


bool  sV2gOptInit(i32 i32DevIndex, ePileGunIndex_t eGunIndex, void (*pCbSwitchV2gToShutdownEmg)(i32 i32DevIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));
bool  sV2gOptHandCacheInit(i32 i32DevIndex);
bool  sV2gOptCacheInit(i32 i32DevIndex);
i32   sV2gOptProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen);


bool  sV2gOptSetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
bool  sV2gOptGetSessionStop(i32 i32DevIndex);
bool  sV2gOptSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);








//---------------------------------------------------------------------------------------------------------








/***************************************************************************************************
* Description                           :   具体的 v2g 协议初始化
* Author                                :   Hall
* Creat Date                            :   2024-01-18
* notice                                :   收到 supportedAppProtocol 消息后匹配到对应的v2g协议后 
*                                           执行此初始化函数
****************************************************************************************************/
bool sV2gXxxInit(i32 i32DevIndex, stV2gOptMap_t *pMap)
{
    bool bRst = false;
    
    
    pV2gOptCache[i32DevIndex]->pMap = pMap;
    if(pV2gOptCache[i32DevIndex]->pMap->pInit != NULL)
    {
        bRst = pV2gOptCache[i32DevIndex]->pMap->pInit(i32DevIndex,
                                                      pV2gOptCache[i32DevIndex]->pCbSwitchV2gToShutdownEmg,
                                                      sV2gSeccSequenceTimerRst,
                                                      sV2gSeccSequenceTimerStart,
                                                      sV2gSeccCpStateDetectionTimerStart,
                                                      sBmsCcsNetSetTcpConnClose);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   v2g 协议类型匹配
* Author                                :   Hall
* Creat Date                            :   2024-01-18
* notice                                :   
****************************************************************************************************/
stV2gOptMap_t *sV2gTypeMatch(i32 i32Len, const char *pNamespace)
{
    i32 j;
    stV2gOptMap_t *pMap = NULL;
    
    for(j = 0; j < sArraySize(stV2gOptMap); j++)
    {
        if(memcmp(stV2gOptMap[j].pNameSpace, pNamespace, i32Len) == 0)
        {
            pMap = &stV2gOptMap[j];
            break;
        }
    }
    
    return(pMap);
}









/***************************************************************************************************
* Description                           :   v2g消息处理部分 资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-08-31
* notice                                :   
****************************************************************************************************/
bool sV2gOptInit(i32 i32DevIndex, ePileGunIndex_t eGunIndex, void (*pCbSwitchV2gToShutdownEmg)(i32 i32DevIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn))
{
    pV2gOptCache[i32DevIndex] = (stV2gOptCache_t *)MALLOC(sizeof(stV2gOptCache_t));
    memset(pV2gOptCache[i32DevIndex], 0, sizeof(stV2gOptCache_t));
    pV2gOptCache[i32DevIndex]->pCbSwitchV2gToShutdownEmg = pCbSwitchV2gToShutdownEmg;
    pV2gOptCache[i32DevIndex]->pCpPwmControl = pCpPwmControl;
    
    //时序逻辑资源初始化
    sV2gInitTiming(i32DevIndex, eGunIndex);
    
    return(true);
}







/*******************************************************************************
 * @FunctionName   :      sV2gOptHandCacheInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月20日  18:15:25
 * @Description    :      v2g 握手消息缓存初始化函数
 * @Input          :      i32DevIndex 
 * @Return         :      
*******************************************************************************/
bool sV2gOptHandCacheInit(i32 i32DevIndex)
{
    memset(&pV2gOptCache[i32DevIndex]->supportedAppProtocolReq, 0, sizeof(struct appHandEXIDocument));
    memset(&pV2gOptCache[i32DevIndex]->supportedAppProtocolRes, 0, sizeof(struct appHandEXIDocument));
    
    return(true);
}











/*******************************************************************************
 * @FunctionName   :      sV2gOptCacheInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年3月19日  15:11:44
 * @Description    :      v2g 相关缓存初始化函数
 * @Input          :      i32DevIndex 
 * @Return         :      
*******************************************************************************/
bool sV2gOptCacheInit(i32 i32DevIndex)
{
    if((pV2gOptCache[i32DevIndex] != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap->pCacheInit != NULL))
    {
        return pV2gOptCache[i32DevIndex]->pMap->pCacheInit(i32DevIndex);
    }
    
    return(false);
}











/***************************************************************************************************
* Description                           :   v2g msg 解析处理函数
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
i32 sV2gOptProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen)
{
    i32 i32Ret = -1;
    stV2gOptCache_t *pCache = pV2gOptCache[i32DevIndex];
    
    
    //先按非 supportedAppProtocol 消息解析
    if((pV2gOptCache[i32DevIndex] != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap->pProc != NULL))
    {
        i32Ret = pV2gOptCache[i32DevIndex]->pMap->pProc(i32DevIndex, pData, i32DataLen, pCache->u8RxBuf, pCache->u8TxBuf, sizeof(pCache->u8RxBuf));
    }
    
    
    //如果按 非supportedAppProtocol消息 解析失败 再按supportedAppProtocol消息解析
    if(i32Ret != 0)
    {
        i32Ret = sV2gHandshakeProc(i32DevIndex, pData, i32DataLen);
    }
    
    return(i32Ret);
}







/***************************************************************************************************
* Description                           :   v2g V2G_SECC_Sequence_Timer 停止计时
* Author                                :   Hall
* Creat Date                            :   2024-01-30
* notice                                :   见状态机转换逻辑图
                                            此函数是图中V2G通讯阶段需要循环调用的api函数
*                                           用于用户程序向状态机(V2G阶段)输入CP状态
****************************************************************************************************/
bool sV2gOptSetCpState(i32 i32DevIndex, eChgCpState_t eCpState)
{
    //为时序逻辑中的cp状态检测逻辑提供cp状态事件
    sV2gSeccCpStateDetectionTimerRst(i32DevIndex, eCpState);
    
    if((pV2gOptCache[i32DevIndex] != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap->pSetCpState != NULL))
    {
        return pV2gOptCache[i32DevIndex]->pMap->pSetCpState(i32DevIndex, eCpState);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   v2g 协议内部信息查询函数---SessionStop 消息查询
* Author                                :   Hall
* Creat Date                            :   2023-06-21
* notice                                :   功能是查询V2G协议内部是否交互到 SessionStop 消息阶段
****************************************************************************************************/
bool sV2gOptGetSessionStop(i32 i32DevIndex)
{
    if((pV2gOptCache[i32DevIndex] != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap->pGetSessionStop != NULL))
    {
        return pV2gOptCache[i32DevIndex]->pMap->pGetSessionStop(i32DevIndex);
    }
    
    return(false);
}







/***************************************************************************************************
* Description                           :   v2g 协议内部信息查询函数---V2G_SECC_CPState_Detection_Timeout cp状态检测超时 responseCode 填充
* Author                                :   Hall
* Creat Date                            :   2024-02-02
* notice                                :   
****************************************************************************************************/
bool sV2gOptSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState)
{
    if((pV2gOptCache[i32DevIndex] != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap != NULL)
    && (pV2gOptCache[i32DevIndex]->pMap->pSetResponseCodeForCpStateTimeout != NULL))
    {
        return pV2gOptCache[i32DevIndex]->pMap->pSetResponseCodeForCpStateTimeout(i32DevIndex, eCpState);
    }
    
    return(false);
}




























































