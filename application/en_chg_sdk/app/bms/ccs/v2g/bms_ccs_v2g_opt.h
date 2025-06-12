/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_v2g_opt.h
* Description                           :   V2G 应用层消息 协议管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_v2g_opt_h_
#define _bms_ccs_v2g_opt_h_
#include "EXITypes.h"
#include "dinEXIDatatypes.h"
#include "iso1EXIDatatypes.h"

#include "appHandEXIDatatypesDecoder.h"
#include "appHandEXIDatatypesEncoder.h"

#include "en_common.h"
#include "en_log.h"

#include "bms_ccs_v2g_hand.h"
#include "bms_ccs_v2g_timing.h"
















//V2G msg 之协议类型
typedef enum
{
    eBmsCcsNetV2gTypeDin                = 0x00,                                 //DIN70121
    eBmsCcsNetV2gTypeIso1               = 0x01,                                 //ISO15118
    eBmsCcsNetV2gTypeIso2               = 0x02,                                 //15118-2-2016 标准----ISO版本2
    
    eBmsCcsNetV2gTypeMax
}eBmsCcsNetV2gType_t;





























//V2G msg 协议管理
typedef struct
{
    //协议 类型
    eBmsCcsNetV2gType_t                 eType;
    
    //协议 描述字符串
    char                                *pDescString;
    
    //协议 命名空间字符串
    char                                *pNameSpace;
    
    //协议 初始化函数
    bool                                (*pInit)(i32 i32DevIndex,
                                                 void (*pCb0)(i32 i32DevIndex),
                                                 void (*pCb1)(i32 i32DevIndex),
                                                 void (*pCb2)(i32 i32DevIndex),
                                                 void (*pCb3)(i32 i32DevIndex, eChgCpState_t eCpState),
                                                 bool (*pCb4)(i32 i32DevIndex));
    
    //协议 缓存初始化函数
    bool                                (*pCacheInit)(i32 i32DevIndex);
    
    //协议 (非 supportedAppProtocol)V2G消息处理
    i32                                 (*pProc)(i32 i32DevIndex, const u8 *pData, i32 i32DataLen, u8 *pRxBuf, u8 *pTxBuf, i32 i32BufLen);
    
    //协议 cp状态 设置
    bool                                (*pSetCpState)(i32 i32DevIndex, eChgCpState_t eCpState);
    
    //协议 SessionStop会话 查询
    bool                                (*pGetSessionStop)(i32 i32DevIndex);
    
    //协议 cp状态检测超时 responseCode 填充
    bool                                (*pSetResponseCodeForCpStateTimeout)(i32 i32DevIndex, eChgCpState_t eCpState);
    
    
}stV2gOptMap_t;
































//定义 v2g 消息 长度
#define cV2gBufSize                     (1024)



//v2g opt 缓存结构定义
typedef struct
{
    //所匹配的协议map
    stV2gOptMap_t                       *pMap;
    
    //接收发送buf
    u8                                  u8RxBuf[cV2gBufSize];
    u8                                  u8TxBuf[cV2gBufSize];
    
    
    
    //v2g协议 handshake消息解析收发doc的结构体---这一对结构体消耗18K的ram
    struct appHandEXIDocument           supportedAppProtocolReq;
    struct appHandEXIDocument           supportedAppProtocolRes;
    
    //状态机操作资源
    void                                (*pCbSwitchV2gToShutdownEmg)(i32 i32DevIndex);
    
    //v2g协议 时序资源
    TaskHandle_t                        xTaskSeccCommunicationSetupPerformance; //V2G_SECC_CommunicationSetup_Performance_Time  定时器监视任务任务句柄
    TaskHandle_t                        xTaskSeccSequenceTimer;                 //V2G_SECC_Sequence_Timer                       定时器监视任务任务句柄
    TaskHandle_t                        xTaskSeccCpStateDetection;              //V2G_SECC_CPState_Detection_Timeout            定时器监视任务任务句柄
    EventGroupHandle_t                  xEvtTiming;                             //时序事件组
    ePileGunIndex_t                     eGunIndex;                              //对应的枪序号, 操作CP资源需要
    
    //外部传入的参数
    void                                (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn);//回调函数:CP PWM控制函数
    
}stV2gOptCache_t;



















extern bool  sV2gXxxInit(i32 i32DevIndex, stV2gOptMap_t *pMap);
extern stV2gOptMap_t *sV2gTypeMatch(i32 i32Len, const char *pNamespace);


extern bool  sV2gOptInit(i32 i32DevIndex, ePileGunIndex_t eGunIndex, void (*pCbSwitchV2gToShutdownEmg)(i32 i32DevIndex), void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));
extern bool  sV2gOptHandCacheInit(i32 i32DevIndex);
extern bool  sV2gOptCacheInit(i32 i32DevIndex);
extern i32   sV2gOptProc(i32 i32DevIndex, const u8 *pData, i32 i32DataLen);

extern bool  sV2gOptSetCpState(i32 i32DevIndex, eChgCpState_t eCpState);
extern bool  sV2gOptGetSessionStop(i32 i32DevIndex);
extern bool  sV2gOptSetResponseCodeForCpStateDetectionTimeout(i32 i32DevIndex, eChgCpState_t eCpState);







#endif






