/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_v2g_hand.c
* Description                           :   V2G 应用层消息 handshake报文
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-05-11
* notice                                :   
****************************************************************************************************/
#include "bms_ccs_net_v2gtp.h"

#include "bms_ccs_v2g_din.h"
#include "bms_ccs_v2g_opt.h"
#include "bms_ccs_v2g_hand.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_v2g_hand";



extern stV2gOptCache_t *pV2gOptCache[cMse102xDevNum];




//appHandresponseCodeType 字段字符串定义
char u8AppHandresponseCodeTypeString[appHandresponseCodeType_Failed_NoNegotiation + 1][128] =
{
    "0 ->OK_SuccessfulNegotiation,协议协商成功",
    "1 ->OK_SuccessfulNegotiationWithMinorDeviation,协议协商成功,有微小偏差",
    "2 ->Failed_NoNegotiation,协议协商失败",
};









void sV2gPrintSupportedAppProtocolReq(i32 i32DevIndex, struct appHandEXIDocument *pDoc);
void sV2gPrintSupportedAppProtocolRes(i32 i32DevIndex, struct appHandEXIDocument *pDoc);

i32  sV2gHandshakeProc(i32 i32DevIndex, const u8 *pBuf, u32 u32Len);

bool sV2gHandshakeGetNegotiationFailed(i32 i32DevIndex);

















//supportedAppProtocolReq/Res
void sV2gPrintSupportedAppProtocolReq(i32 i32DevIndex, struct appHandEXIDocument *pDoc)
{
    u16 i,j;
    char u8String[128];
    
    EN_SLOGI(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    EN_SLOGD(TAG, "SECC[%d]接收<<<<supportedAppProtocol Req", i32DevIndex);
    EN_SLOGI(TAG, "EVCC端支持的协议列表 :");
    for(i=0; i<pDoc->supportedAppProtocolReq.AppProtocol.arrayLen; i++) 
    {
        memset(u8String, 0, sizeof(u8String));
        for(j=0; j<pDoc->supportedAppProtocolReq.AppProtocol.array[i].ProtocolNamespace.charactersLen; j++)
        {
            u8String[j] = pDoc->supportedAppProtocolReq.AppProtocol.array[i].ProtocolNamespace.characters[j];
        }
        EN_SLOGI(TAG, "**********************************************");
        EN_SLOGI(TAG, "Protocol Entry#%d", (i + 1));
        EN_SLOGI(TAG, "Namespace        :%s", u8String);
        EN_SLOGI(TAG, "Version          :%d.%d", pDoc->supportedAppProtocolReq.AppProtocol.array[i].VersionNumberMajor, pDoc->supportedAppProtocolReq.AppProtocol.array[i].VersionNumberMinor);
        EN_SLOGI(TAG, "SchemaID         :%d", pDoc->supportedAppProtocolReq.AppProtocol.array[i].SchemaID);
        EN_SLOGI(TAG, "Priority         :%d", pDoc->supportedAppProtocolReq.AppProtocol.array[i].Priority);
        EN_SLOGI(TAG, "**********************************************");
    }
    EN_SLOGI(TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void sV2gPrintSupportedAppProtocolRes(i32 i32DevIndex, struct appHandEXIDocument *pDoc)
{
    
    EN_SLOGI(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    EN_SLOGD(TAG, "SECC[%d]发送<<<<supportedAppProtocol Res", i32DevIndex);
    EN_SLOGI(TAG, "ResponseCode     :%s", u8AppHandresponseCodeTypeString[pDoc->supportedAppProtocolRes.ResponseCode]);
    EN_SLOGI(TAG, "SchemaID         :%d", pDoc->supportedAppProtocolRes.SchemaID);
    EN_SLOGI(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}
















/***************************************************************************************************
* Description                           :   supportedAppProtocol 消息 解析处理函数
* Author                                :   Hall
* Creat Date                            :   2022-07-04
* notice                                :   由于DIN70121 ISO15118-1.0/2.0 的handshake过程一致,
*                                           因此可以放在本文件集中处理, 而后续V2G消息则不行
****************************************************************************************************/
i32 sV2gHandshakeProc(i32 i32DevIndex, const u8 *pBuf, u32 u32Len)
{
    char u8String[128];
    
    bitstream_t stStreamRx;
    bitstream_t stStreamTx;
    
    size_t posRx = 0;
    size_t posTx = 0;
    
    u8  u8Index    = 255;
    u8  u8Priority = 255;                                                       //协议优先级 值越小优先级越高
    stV2gOptMap_t *pMap = NULL;
    stV2gOptMap_t *pMapTemp = NULL;
    
    i32 i32Err, i,j;
    
    stV2gOptCache_t *pCache    = pV2gOptCache[i32DevIndex];
    
    
    stStreamRx.size = sizeof(pCache->u8RxBuf);
    stStreamRx.data = pCache->u8RxBuf;
    stStreamRx.pos = &posRx;
    memset(stStreamRx.data, 0, stStreamRx.size);
    
    stStreamTx.size = sizeof(pCache->u8TxBuf);
    stStreamTx.data = pCache->u8TxBuf;
    stStreamTx.pos = &posTx;
    memset(stStreamTx.data, 0, stStreamTx.size);
    
    
    memcpy(stStreamRx.data, pBuf, u32Len);
    *(stStreamRx.pos) = 0;
    if((i32Err = decode_appHandExiDocument(&stStreamRx, &pCache->supportedAppProtocolReq))) 
    {
        EN_SLOGE(TAG, "supportedAppProtocol Req消息解码错误, errn:%d", i32Err);
        return(i32Err);
    }
    
    
    //打印收到的V2G请求消息
    sV2gPrintSupportedAppProtocolReq(i32DevIndex, &pCache->supportedAppProtocolReq);
    
    
    //tcp 连接建立以后 外部的状态机尚未按 [V2G-DC-432] 启动的定时器时
    //supportedAppProtocol req 就已经收到，程序已经执行到这里
    //所以增加本延时 等 [V2G-DC-432] 启动定时器之后 再执行这里的清除动作
    vTaskDelay(200 / portTICK_RATE_MS);
    sV2gSeccSequenceTimerRst(i32DevIndex);                                      //清除 [V2G-DC-432] 启动的定时器
    
    //寻找优先级最高的协议
    for(i = 0; i < pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.arrayLen; i++)
    {
        //比对协议 NameSpace
        memset(u8String, 0, sizeof(u8String));
        for(j = 0; j < pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[i].ProtocolNamespace.charactersLen; j++)
        {
            u8String[j] = pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[i].ProtocolNamespace.characters[j];
        }
        
        if((pMapTemp = sV2gTypeMatch(strlen(u8String), u8String)) != NULL)
        {
            if(u8Priority > pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[i].Priority)
            {
                u8Index     = i;
                pMap        = pMapTemp;
                u8Priority  = pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[i].Priority;
            }
        }
        
    }
    
    
    //
    if(u8Index < pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.arrayLen)
    {
        //比对主版本号
        if(APP_PROTOCOL_MAJOR_VERSION == pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[u8Index].VersionNumberMajor)
        {
            //比对次版本号
            if(APP_PROTOCOL_MINOR_VERSION == pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[u8Index].VersionNumberMinor)
            {
                //协商成功
                pCache->supportedAppProtocolRes.supportedAppProtocolRes.ResponseCode = appHandresponseCodeType_OK_SuccessfulNegotiation;
            }
            else
            {
                //协商成功，但次版本号有细微偏差
                pCache->supportedAppProtocolRes.supportedAppProtocolRes.ResponseCode = appHandresponseCodeType_OK_SuccessfulNegotiationWithMinorDeviation;
            }
        }
        pCache->supportedAppProtocolRes.supportedAppProtocolRes.SchemaID = pCache->supportedAppProtocolReq.supportedAppProtocolReq.AppProtocol.array[u8Index].SchemaID; //signal the protocol by the provided schema id
        pCache->supportedAppProtocolRes.supportedAppProtocolRes.SchemaID_isUsed = 1u;
        
        //初始化v2g协议
        sV2gXxxInit(i32DevIndex, pMap);
    }
    else
    {
        //协商失败
        pCache->supportedAppProtocolRes.supportedAppProtocolRes.ResponseCode = appHandresponseCodeType_Failed_NoNegotiation;
        pCache->supportedAppProtocolRes.supportedAppProtocolRes.SchemaID_isUsed = 0u;
    }
    pCache->supportedAppProtocolRes.supportedAppProtocolReq_isUsed = 0u;
    pCache->supportedAppProtocolRes.supportedAppProtocolRes_isUsed = 1u;
    
    //打印将要发出的V2G响应消息
    sV2gPrintSupportedAppProtocolRes(i32DevIndex, &pCache->supportedAppProtocolRes);
    
    
    *stStreamTx.pos = 0;
    if((i32Err = encode_appHandExiDocument(&stStreamTx, &pCache->supportedAppProtocolRes)) == 0) 
    {
        //按 [V2G-DC-364] 启动 V2G_SECC_Sequence_Timer 定时器
        //但是为了避免 与 [V2G-DC-432] 启动定时器后的处理逻辑冲突 还要增加本延时
        //等时序逻辑任务处理完[V2G-DC-432] 启动的定时器 以后才启动
        vTaskDelay(300 / portTICK_RATE_MS);
        sV2gSeccSequenceTimerStart(i32DevIndex);
        sBmsCcsNetV2gtpSendViaTcp(i32DevIndex, stStreamTx.data, (*stStreamTx.pos), cV2gtpVer, cV2gtpPayloadTypeExi);
    }
    else
    {
        EN_SLOGE(TAG, "supportedAppProtocol Res消息编码错误, errn:%d", i32Err);
    }
    
    return(i32Err);
}











/*******************************************************************************
 * @FunctionName   :      sV2gHandshakeGetNegotiation
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年4月18日  20:54:23
 * @Description    :      supportedAppProtocol 消息 之 判断握手协商是否成功
 * @Input          :      i32DevIndex 
 * @Return         :      
*******************************************************************************/
bool sV2gHandshakeGetNegotiation(i32 i32DevIndex)
{
    bool bRst = true;
    
    if(pV2gOptCache[i32DevIndex]->supportedAppProtocolRes.supportedAppProtocolRes_isUsed == 1)
    {
        bRst = (pV2gOptCache[i32DevIndex]->supportedAppProtocolRes.supportedAppProtocolRes.ResponseCode != appHandresponseCodeType_Failed_NoNegotiation) ? true : false;
    }
    
    return (bRst);
}





















































