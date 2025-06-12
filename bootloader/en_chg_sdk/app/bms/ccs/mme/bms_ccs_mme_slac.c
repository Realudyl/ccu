/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_slac.c
* Description                           :   MME协议实现 之 SLAC相关的MME帧
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   实现SLAC过程所需要的MME消息发送和接收
****************************************************************************************************/
#include <math.h>

#include "en_common.h"
#include "en_log.h"
#include "io_pwm_app.h"

#include "hpav_security.h"
#include "bms_ccs_mme.h"
#include "bms_ccs_mme_op.h"
#include "bms_ccs_mme_slac.h"
#include "bms_ccs_mme_slac_param.h"
#include "bms_ccs_mme_slac_data_def.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme_slac";





//<ISO 15118-3-2015.pdf>@Table A.4、@Table A.7
//定义这4个ID值为全0
const char u8PesrId[pev_evse_source_resp_id_len] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};




extern stMmeCache_t *pMmeCache[];
extern stMmeCmdTypeMap_t stMmeCmdTypeMap[];



//MME SLAC 过程
void  sMmeSlacTask(void *pParam);
void  sMmeSlacTaskSend(void *pParam);

void  sMmeSlacNmkNidGenerate(u8 *pNmk, u8 *pNid);
bool  sMmeSlacNetInit(struct pev_item *pPev);
void  sMmeSlacSetEvtCpDisconn(struct pev_item *pHead);
i32   sMmeSlacWaitCmStartAttenCharInd(struct pev_item *pPev);
i32   sMmeSlacWaitCmAttenProfileInd(struct pev_item *pPev);
i32   sMmeSlacWaitCmAttenCharRsp(struct pev_item *pPev);
i32   sMmeSlacWaitCmSlacMatchReq(struct pev_item *pPev);
i32   sMmeSlacWaitTtMatchJoinLogic(struct pev_item *pPev);
i32   sMmeSlacWaitCmSlacParmReq(struct pev_item *pPev);
void  sMmeSlacSendVsGetStatusReqCycle(struct pev_item *pPev);



//MME SLAC 帧发送函数
bool  sMmePktSendCmSetKeyReq(struct pev_item *pPev);
bool  sMmePktSendCmSlacParmCnf(struct pev_item *pPev);
bool  sMmePktSendCmAttenCharInd(struct pev_item *pPev);
bool  sMmePktSendCmSlacMatchCnf(struct pev_item *pPev, slac_match_req *pReq);


//MME SLAC 帧接收处理函数
bool  sMmePktRecvCmSetKeyCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmSlacParmReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmStartAttenCharInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmMnbcSoundInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmAttenProfileInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmAttenCharRsp(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
bool  sMmePktRecvCmSlacMatchReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);















/***************************************************************************************************
* Description                           :   SECC SLAC匹配流程 主任务
* Author                                :   Hall
* Creat Date                            :   2022-06-20
* notice                                :   
*
//注意
//本任务不可被长时间阻塞，它必须时刻监测cp状态以便可以在cp状态变化时立即结束slac匹配流程
//一些函数含有潜在长时间阻塞，直接调用此类函数将会导致slac匹配流程在阻塞阶段无法实时跟进cp状态的变化
****************************************************************************************************/
void sMmeSlacTask(void *pParam)
{
    i32  i32EventSlacBit;
    stMmeCache_t *pCache = NULL;
    struct pev_item *pPev = NULL;
    
    
    
MME_SLAC_RESTART:
    //1:初始化数据及相关事件标志
    pPev = (struct pev_item *)pParam;
    EN_SLOGI(TAG, "SECC[%d], Slac:开始执行...", pPev->stMmeDev.i32Index);
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    xEventGroupClearBits(pPev->xEvtSlac, cEvtSlacAll);
    
    
    //此函数就可以在本任务内直接调用 因为它属于ack发送 不会阻塞等待对侧应答
    //当然，发送内部也有一些信号量的阻塞 但那些都属于共享资源同步锁 
    //一般认为此类阻塞都是瞬间完成的 不会出现人类可以感知的长时间阻塞
    if((pCache->eCpState == eChgCpStateB) || (pCache->eCpState == eChgCpStateC))
    {
        //2024-06-28 : 这里新增了CP状态C，因为交流桩插枪后进行SLAC匹配时有可能是状态C
        sMmePktSendCmSlacParmCnf(pPev);
    }
    else
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    
    
    
    //2:等待 CM_START_ATTEN_CHAR.ind 消息
    i32EventSlacBit = sMmeSlacWaitCmStartAttenCharInd(pPev);
    if(i32EventSlacBit == cEvtSlacCpDisconnect)
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    else if(i32EventSlacBit == cEvtSlacNone)
    {
        goto MME_SLAC_FAIL;
    }
    else if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
    {
        goto MME_SLAC_RESTART;
    }
    if(pPev->i32SlacState == eRecvCmStartAttenCharInd)
    {
        pPev->i32SlacState = eSendCmAttenCharInd;
    }
    
    
    //3:等待 CM_ATTEN_PROFILE.ind 消息 1~x 次
    //vTaskDelay(100 / portTICK_RATE_MS);
    i32EventSlacBit = sMmeSlacWaitCmAttenProfileInd(pPev);
    if(i32EventSlacBit == cEvtSlacCpDisconnect)
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    else if(i32EventSlacBit == cEvtSlacNone)
    {
        goto MME_SLAC_FAIL;
    }
    else if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
    {
        goto MME_SLAC_RESTART;
    }
    
    
    //收到 CM_ATTEN_PROFILE.ind 消息之后 创建任务去执行发送任务
    //为啥不在本任务内部直接发送？
    //因为该消息属于 rpt发送, 需要阻塞等待对侧的 ack应答, 这可能会造成长时间的阻塞
    xTaskNotify(pCache->xTaskSlacSend, (u32)(pPev), eSetValueWithOverwrite);
    
    //4:等待 CM_ATTEN_CHAR.rsp 消息
    i32EventSlacBit = sMmeSlacWaitCmAttenCharRsp(pPev);
    if(i32EventSlacBit == cEvtSlacCpDisconnect)
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    else if((i32EventSlacBit == cEvtSlacNone) || (i32EventSlacBit == cEvtSlacCmAttenCharIndRetryOut))
    {
        goto MME_SLAC_FAIL;
    }
    else if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
    {
        //这里需要给负值, 停止重发 CM_ATTEN_CHAR.ind 消息,
        pPev->i32AttenCharIndRetryCnt = -1;
        goto MME_SLAC_RESTART;
    }
    
    
    //5:等待 CM_SLAC_MATCH.req 消息
    i32EventSlacBit = sMmeSlacWaitCmSlacMatchReq(pPev);
    if(i32EventSlacBit == cEvtSlacCpDisconnect)
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    else if(i32EventSlacBit == cEvtSlacNone)
    {
        goto MME_SLAC_FAIL;
    }
    else if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
    {
        goto MME_SLAC_RESTART;
    }
    
    
    //6:TT_match_join 逻辑---创建以下任务去周期性查询PLC节点连接状态 直到连接建立或超时或其他异常
    xTaskNotify(pCache->xTaskSlacSend, (u32)(pPev), eSetValueWithOverwrite);
    i32EventSlacBit = sMmeSlacWaitTtMatchJoinLogic(pPev);
    if(i32EventSlacBit == cEvtSlacCpDisconnect)
    {
        goto MME_SLAC_FAIL_BY_CP;
    }
    else if(i32EventSlacBit == cEvtSlacNone)
    {
        goto MME_SLAC_FAIL;
    }
    else if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
    {
        goto MME_SLAC_RESTART;
    }
    EN_SLOGI(TAG, "SECC[%d], Slac:Slac匹配成功!", pPev->stMmeDev.i32Index);
    
    
    //跳转到等待区 等待新的Slac请求
    pPev->bSlacMatchFlag = true;
    goto MME_SLAC_SUCCESS;
    
    
MME_SLAC_FAIL_BY_CP:
    //SLAC匹配失败立即关闭CP振荡器----此处是SLAC阶段就失败了，需要关闭CP振荡器
    if(pCache->pCpPwmControl != NULL)
    {
        pCache->pCpPwmControl(pCache->eGunIndex, false);
    }
    
MME_SLAC_FAIL:
    //失败的时候，清零SLAC状态机
    pPev->i32SlacState = eSendCmSekKey;
    
MME_SLAC_SUCCESS:
    while(1)
    {
        i32EventSlacBit = sMmeSlacWaitCmSlacParmReq(pPev);
        if(i32EventSlacBit == cEvtSlacCmSlacParmReq)
        {
            EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
            goto MME_SLAC_RESTART;
        }
        
        sMmeSlacSendVsGetStatusReqCycle(pPev);
    }
}







/***************************************************************************************************
* Description                           :   SECC SLAC匹配流程 消息发送任务
* Author                                :   Hall
* Creat Date                            :   2024-01-06
* notice                                :   所有的pev节点公用本任务执行slac报文发送，通过任务通知
*                                           将pev节点的地址发送到任务
****************************************************************************************************/
void sMmeSlacTaskSend(void *pParam)
{
    stMmeVsGetStatusCnf_t stCnf;
    
    u32  u32Notify;
    struct pev_item stPevDummy;                                                 //虚拟的pev节点,
    struct pev_item *pPev = NULL;
    stMmeDev_t   *pMmeDev = (stMmeDev_t *)pParam;
    stMmeCache_t *pCache = pMmeCache[pMmeDev->i32Index];
    
    
    //上电后向PLC节点发送CM_SET_KEY消息更新NMK/NID参数
    vTaskDelay(2000 / portTICK_RATE_MS);
    memset(&stPevDummy, 0, sizeof(stPevDummy));
    memcpy(&stPevDummy.stMmeDev, pMmeDev, sizeof(stMmeDev_t));
    stPevDummy.i32SlacState = eSendCmSekKey;
    stPevDummy.next = NULL;
    xTaskNotify(pCache->xTaskSlacSend, (u32)(&stPevDummy), eSetValueWithOverwrite);
    
    
    EN_SLOGI(TAG, "任务建立:SLAC匹配流程 消息发送任务。");
    while(1)
    {
        //等待接收通知数据 并转换为pev节点地址
        xTaskNotifyWait(0, ULONG_MAX, &u32Notify, portMAX_DELAY);
        pPev = (struct pev_item *)u32Notify;
        
        if(pPev->i32SlacState == eSendCmSekKey)
        {
            sMmeSlacNetInit(pPev);
        }
        else if(pPev->i32SlacState == eSendCmAttenCharInd)
        {
            while(pPev->i32AttenCharIndRetryCnt >= 0)
            {
                pPev->i32AttenCharIndRetryCnt--;
                if(sMmePktSendCmAttenCharInd(pPev) == true)
                {
                    xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmAttenCharRsp);
                    break;
                }
            }
            
            if(pPev->i32AttenCharIndRetryCnt < 0)
            {
                xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmAttenCharIndRetryOut);
            }
        }
        else if(pPev->i32SlacState == eSendCmSlacMatchCnf)
        {
            pPev->i32TtMatchJoinWaitCnt = TT_match_join / 100;
            while(pPev->i32TtMatchJoinWaitCnt > 0)
            {
                memset(&stCnf, 0, sizeof(stCnf));
                if((sMmePacketSendVsGetStatusReq(pPev, &stCnf) == true) && (stCnf.eLinkState == eMmeVsGetStatusLinkStateConn))
                {
                    xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacJoinLogic);
                    break;
                }
                
                pPev->i32TtMatchJoinWaitCnt--;
                vTaskDelay(100 / portTICK_RATE_MS);
            }
        }
    }
    
    EN_SLOGI(TAG, "任务删除:SLAC匹配流程 消息发送任务。");
    vTaskDelete(NULL);
}














/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 网络秘钥初始化NMK NID 生成
* Author                                :   Hall
* Creat Date                            :   2022-07-11
* notice                                :   
****************************************************************************************************/
void sMmeSlacNmkNidGenerate(u8 *pNmk, u8 *pNid)
{
    i32  i;
    i32  i32RandValue = 0;
    char uNetCode[HPAV_AES_KEY_SIZE + 1];
    
    u8   u8Nmk[HPAV_AES_KEY_SIZE];
    u8   u8Nid[HPAV_NID_SIZE];
    
    memset(uNetCode, 0, sizeof(uNetCode));
    for(i = 0; i < HPAV_AES_KEY_SIZE; i++)
    {
        i32RandValue = rand();
        
        //网络密码取ASCII码的 '!' 到 '~' 之间(包含边缘)随机值
        uNetCode[i] = (i32RandValue % ('~' - '!' + 1)) + '!';                       //生成一个16位长度的网络密码
    }
    
    hpav_generate_nmk(uNetCode, u8Nmk);
    hpav_generate_nid(u8Nmk, 0, u8Nid);
    
    memcpy(pNmk, u8Nmk, sizeof(u8Nmk));
    memcpy(pNid, u8Nid, sizeof(u8Nid));
    
    EN_SLOGI(TAG, "生成 网络密码: %s", uNetCode);
    EN_HLOGI(TAG, "生成      NID: ", u8Nid, sizeof(u8Nid));
    EN_HLOGI(TAG, "生成      NMK: ", u8Nmk, sizeof(u8Nmk));
}








/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 更新PLC载波芯片的NMK NID参数
* Author                                :   Hall
* Creat Date                            :   2024-04-11
* notice                                :   
****************************************************************************************************/
bool sMmeSlacNetInit(struct pev_item *pPev)
{
    stMmeCache_t *pCache = pMmeCache[pPev->stMmeDev.i32Index];
    
    sMmeSlacNmkNidGenerate(pCache->stEvse.nmk, pCache->stEvse.nid);
    pPev->bSlacMatchFlag = false;
    
    return sMmePktSendCmSetKeyReq(pPev);
}







/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 从外部设定CP断开或异常事件标志
* Author                                :   Hall
* Creat Date                            :   2024-01-30
* notice                                :   应该遍历链表，将有效事件组全部设置此事件
****************************************************************************************************/
void sMmeSlacSetEvtCpDisconn(struct pev_item *pHead)
{
    pev_item *pPev = pHead;
    while(pPev->next != NULL)
    {
        pPev = pPev->next;
        xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCpDisconnect);
    }
}









/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 等待 CM_START_ATTEN_CHAR.ind 消息
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-04
* notice                                :   
****************************************************************************************************/
i32 sMmeSlacWaitCmStartAttenCharInd(struct pev_item *pPev)
{
    i32  i32Ret;
    i32  i32Timeout;
    EventBits_t xEventSlacBitWait;
    
    //实测 按标准定义的超时时间去做检查的话 有较大概率超时 可能是联芯通芯片有bug
    //所以这里用1000ms
    i32Timeout = 1000;//TT_match_sequence + (TP_EV_batch_msg_interval * 2);
    
    xEventSlacBitWait = (cEvtSlacCmStartAttenCharInd | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, i32Timeout / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP连接异常, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmStartAttenCharInd) != 0)
    {
        i32Ret = cEvtSlacCmStartAttenCharInd;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_START_ATTEN_CHAR.ind, Slac流程执行下一步", pPev->stMmeDev.i32Index);
    }
    else
    {
        i32Ret = cEvtSlacNone;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_START_ATTEN_CHAR.ind, 超时, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    
    return(i32Ret);
}







/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 等待接收 CM_ATTEN_PROFILE.ind 消息
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-06
* notice                                :   
*                                           最少要收到1次
*                                           最好能收到x次, x 在 CM_SLAC_PARM.cnf        消息中设定
*                                                            在 CM_START_ATTEN_CHAR.ind 消息中确认
****************************************************************************************************/
i32 sMmeSlacWaitCmAttenProfileInd(struct pev_item *pPev)
{
    i32  i32Ret;
    EventBits_t xEventSlacBitWait;
    
    
    //等待 CM_ATTEN_PROFILE.ind 消息 x 次
    xEventSlacBitWait = (cEvtSlacCmAttenProfileIndX | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, (TT_EVSE_match_MNBC * 100) / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP连接异常, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmAttenProfileIndX) != 0)
    {
        i32Ret = i32Ret & cEvtSlacCmAttenProfileIndX;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_PROFILE.ind, %d次, Slac流程执行下一步", pPev->stMmeDev.i32Index, pPev->i32SoundsRx);
        
        //清除 cEvtSlacCmAttenProfileInd 事件标志
        xEventGroupWaitBits(pPev->xEvtSlac, cEvtSlacCmAttenProfileInd, pdTRUE, pdFALSE, 1 / portTICK_RATE_MS);
    }
    else
    {
        i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, cEvtSlacCmAttenProfileInd, pdTRUE, pdFALSE, 1 / portTICK_RATE_MS);
        if((i32Ret & cEvtSlacCmAttenProfileInd) != 0)
        {
            i32Ret = i32Ret & cEvtSlacCmAttenProfileInd;
            EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_PROFILE.ind, %d次, Slac流程执行下一步", pPev->stMmeDev.i32Index, pPev->i32SoundsRx);
        }
        else
        {
            i32Ret = cEvtSlacNone;
            EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_PROFILE.ind, Slac匹配失败!", pPev->stMmeDev.i32Index);
        }
    }
    
    return(i32Ret);
}











/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 等待接收 CM_ATTEN_CHAR.rsp 消息
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-06
* notice                                :   
****************************************************************************************************/
i32 sMmeSlacWaitCmAttenCharRsp(struct pev_item *pPev)
{
    i32  i32Ret;
    EventBits_t xEventSlacBitWait;
    
    xEventSlacBitWait = (cEvtSlacCmAttenCharIndRetryOut | cEvtSlacCmAttenCharRsp | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, (TT_match_response * 3) / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP连接异常, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmAttenCharRsp) != 0)
    {
        i32Ret = cEvtSlacCmAttenCharRsp;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_CHAR.rsp, Slac流程执行下一步", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmAttenCharIndRetryOut) != 0)
    {
        i32Ret = cEvtSlacCmAttenCharIndRetryOut;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_CHAR.rsp, CM_ATTEN_CHAR.ind重试次数用尽, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else
    {
        i32Ret = cEvtSlacNone;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_ATTEN_CHAR.rsp, 超时, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    
    return(i32Ret);
}








/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 等待接收 CM_SLAC_MATCH.req 消息
* Author                                :   LiHongzhao
* Creat Date                            :   2023-11-06
* notice                                :   
****************************************************************************************************/
i32 sMmeSlacWaitCmSlacMatchReq(struct pev_item *pPev)
{
    i32  i32Ret;
    EventBits_t xEventSlacBitWait;
    
    xEventSlacBitWait = (cEvtSlacCmSlacMatchReq | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, TT_EVSE_match_session / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP连接异常, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacMatchReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacMatchReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_MATCH.req, Slac流程执行下一步", pPev->stMmeDev.i32Index);
    }
    else
    {
        i32Ret = cEvtSlacNone;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_MATCH.req, 超时, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    
    return(i32Ret);
}





/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 TT_match_join 定时器逻辑
* Author                                :   Hall
* Creat Date                            :   2023-07-06
* notice                                :   
****************************************************************************************************/
i32 sMmeSlacWaitTtMatchJoinLogic(struct pev_item *pPev)
{
    i32  i32Ret;
    EventBits_t xEventSlacBitWait;
    
    xEventSlacBitWait = (cEvtSlacJoinLogic | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, TT_match_join / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP连接异常, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
        EN_SLOGI(TAG, "SECC[%d], Slac 接收:CM_SLAC_PARM.req, Slac流程重新开始!", pPev->stMmeDev.i32Index);
    }
    else if((i32Ret & cEvtSlacJoinLogic) != 0)
    {
        i32Ret = cEvtSlacJoinLogic;
        EN_SLOGI(TAG, "SECC[%d], Slac 加入逻辑网络成功, Slac匹配成功!", pPev->stMmeDev.i32Index);
    }
    else
    {
        i32Ret = cEvtSlacNone;
        EN_SLOGI(TAG, "SECC[%d], Slac 加入逻辑网络超时, Slac匹配失败!", pPev->stMmeDev.i32Index);
    }
    
    return(i32Ret);
}







/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 等待接收 CM_SLAC_PARM.req 消息
* Author                                :   Hall
* Creat Date                            :   2024-01-06
* notice                                :   
****************************************************************************************************/
i32 sMmeSlacWaitCmSlacParmReq(struct pev_item *pPev)
{
    i32  i32Ret;
    EventBits_t xEventSlacBitWait;
    stMmeCache_t *pCache = pMmeCache[pPev->stMmeDev.i32Index];
    
    xEventSlacBitWait = (cEvtSlacNetInit | cEvtSlacCmSlacParmReq | cEvtSlacCpDisconnect);
    i32Ret = xEventGroupWaitBits(pPev->xEvtSlac, xEventSlacBitWait, pdTRUE, pdFALSE, 1000 / portTICK_RATE_MS);
    if((i32Ret & cEvtSlacNetInit) != 0)
    {
        i32Ret = cEvtSlacNetInit;
        EN_SLOGI(TAG, "SECC[%d], Slac pev节点 执行网络秘钥参数初始化", pPev->stMmeDev.i32Index);
        sMmeSlacNetInit(pPev);
    }
    else if((i32Ret & cEvtSlacCpDisconnect) != 0)
    {
        //此处是SLAC成功以后, 例如SDP阶段, CP异常后 关闭CP振荡器
        i32Ret = cEvtSlacCpDisconnect;
        EN_SLOGI(TAG, "SECC[%d], Slac CP电压异常!!", pPev->stMmeDev.i32Index);
        if(pCache->pCpPwmControl != NULL)
        {
            pCache->pCpPwmControl(pCache->eGunIndex, false); //关闭CP振荡器
        }
    }
    else if((i32Ret & cEvtSlacCmSlacParmReq) != 0)
    {
        i32Ret = cEvtSlacCmSlacParmReq;
    }
    
    return(i32Ret);
}








/***************************************************************************************************
* Description                           :   SECC SLAC匹配功能 之 周期性查询PLC link state
* Author                                :   Hall
* Creat Date                            :   2024-04-11
* notice                                :   
****************************************************************************************************/
void sMmeSlacSendVsGetStatusReqCycle(struct pev_item *pPev)
{
    u32  u32Time;
    stMmeCache_t *pCache = pMmeCache[pPev->stMmeDev.i32Index];
    
    if(pCache->eLinkState == eMmeVsGetStatusLinkStateConn)
    {
        u32Time = sGetTimestamp();
        if((u32Time - pCache->u32TimeOld) >= 10)
        {
            pCache->u32TimeOld = u32Time;
            sMmePacketSendVsGetStatusReq(pPev, NULL);
        }
    }
}












/***************************************************************************************************
* Description                           :   MME 帧发送函数之 CM_SET_KEY.req 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktSendCmSetKeyReq(struct pev_item *pPev)
{
    bool bRst;
    u32  u32PayloadLen;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], SLAC 发送:CM_SET_KEY.req", pPev->stMmeDev.i32Index);
    
    //1.1:填充 payload
    set_key_req key_req =
    {
        .key_type = 1,
        .my_nonce = {0},
        .your_nonce = {0},
        .cco_capability = 1,
        .pid = 4,
        .prn = {0, 0},
        .pmn = 0,
        .new_eks = 1,
    };
    memcpy(key_req.nid,     pCache->stEvse.nid, sizeof(pCache->stEvse.nid));
    memcpy(key_req.new_key, pCache->stEvse.nmk, sizeof(pCache->stEvse.nmk));
    
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, CM_SET_KEY_REQ, pCache->u8TxCtxBuf, sizeof(pCache->u8TxCtxBuf));
    if(eError == MME_SUCCESS)
    {
        eError = mme_put(&pCache->stTxCtx, &key_req, sizeof(key_req), &u32PayloadLen);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(CM_SET_KEY_CNF, true, pPev, &pCache->stTxCtx);
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧发送函数之 CM_SLAC_PARM.cnf 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-06
* notice                                :   
****************************************************************************************************/
bool sMmePktSendCmSlacParmCnf(struct pev_item *pPev)
{
    bool bRst;
    u32  u32PayloadLen;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], SLAC 发送:CM_SLAC_PARM.cnf", pPev->stMmeDev.i32Index);
    
    
    //1.1:填充 payload
    slac_parm_cnf slac_parm_cnf =
    {
        .num_sounds = C_EV_match_MNBC,
        .time_out = 0x06,
        .resp_type = 0x01,
        .application_type = eCmMsgAppTypePevEvseAssoc,
        .security_type = eCmMsgSecurityTypeNone
    };
    memcpy(slac_parm_cnf.m_sound_target, u8MacLocalBroadcastAddr, sizeof(u8MacLocalBroadcastAddr));
    memcpy(slac_parm_cnf.forwarding_sta, pPev->u8MacEvHost,       sizeof(pPev->u8MacEvHost));
    memcpy(slac_parm_cnf.run_id,         pPev->u8RunId,           sizeof(pPev->u8RunId));
    pPev->i32SoundsTx = slac_parm_cnf.num_sounds;
    
    
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, CM_SLAC_PARM_CNF, pCache->u8TxCtxBuf, sizeof(pCache->u8TxCtxBuf));
    if(eError == MME_SUCCESS)
    {
        eError = mme_put(&pCache->stTxCtx, &slac_parm_cnf, sizeof(slac_parm_cnf), &u32PayloadLen);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(CM_SLAC_PARM_REQ, false, pPev, &pCache->stTxCtx);
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   MME 帧发送函数之 CM_ATTEN_CHAR.ind 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-06
* notice                                :   
****************************************************************************************************/
bool sMmePktSendCmAttenCharInd(struct pev_item *pPev)
{
    i32  i;
    bool bRst;
    u16  u16AAG_AVG = 0;
    u32  u32PayloadLen;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], SLAC 发送:CM_ATTEN_CHAR.ind", pPev->stMmeDev.i32Index);
    
    
    //1.1:填充 payload
    atten_char_ind atten_ind =
    {
        .application_type = eCmMsgAppTypePevEvseAssoc,
        .security_type = eCmMsgSecurityTypeNone,
        .ACVarField.num_sounds = pPev->i32SoundsRx,
        .ACVarField.ATTEN_PROFILE.num_groups = cAttenuationGroup
    };
    
    memcpy(atten_ind.ACVarField.source_address, pPev->u8MacEvHost, sizeof(pPev->u8MacEvHost));
    memcpy(atten_ind.ACVarField.run_id,         pPev->u8RunId,     sizeof(pPev->u8RunId    ));
    for(i = 0; i < cAttenuationGroup; i++)
    {
        atten_ind.ACVarField.ATTEN_PROFILE.AAG[i] = pPev->AAG[i] / pPev->i32SoundsRx;
        u16AAG_AVG += atten_ind.ACVarField.ATTEN_PROFILE.AAG[i];
    }
    
    //此处打印为了方便日后实车测试观测PLC信号质量
    EN_SLOGD(TAG, "SECC[%d], SLAC 发送:CM_ATTEN_CHAR.ind --- AAG_AVG : %d dB", pPev->stMmeDev.i32Index, (u16AAG_AVG / i));
    
    
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, CM_ATTEN_CHAR_IND, pCache->u8TxCtxBuf, sizeof(pCache->u8TxCtxBuf));
    if(eError == MME_SUCCESS)
    {
        eError = mme_put(&pCache->stTxCtx, &atten_ind, sizeof(atten_ind), &u32PayloadLen);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(CM_ATTEN_CHAR_RSP, false, pPev, &pCache->stTxCtx);
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   MME 帧发送函数之 CM_SLAC_MATCH.cnf 消息发送
* Author                                :   Hall
* Creat Date                            :   2023-07-06
* notice                                :   
****************************************************************************************************/
bool sMmePktSendCmSlacMatchCnf(struct pev_item *pPev, slac_match_req *pReq)
{
    bool bRst;
    u32  u32PayloadLen;
    mme_error_t eError;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pPev->stMmeDev.i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //0:访问保护
    xSemaphoreTake(pCache->xSemTx, portMAX_DELAY);
    EN_SLOGD(TAG, "SECC[%d], SLAC 发送:CM_SLAC_MATCH.cnf", pPev->stMmeDev.i32Index);
    
    
    //1.1:填充 payload
    slac_match_cnf slac_cnf =
    {
        .application_type = pReq->application_type,
        .security_type = pReq->security_type,
        .mvflength[0] = 0x56
    };
    
    memcpy(slac_cnf.MatchVarField.nid, pCache->stEvse.nid, sizeof(pCache->stEvse.nid));
    memcpy(slac_cnf.MatchVarField.nmk, pCache->stEvse.nmk, sizeof(pCache->stEvse.nmk));
    
    memcpy(slac_cnf.MatchVarField.pev_id,   pReq->MatchVarField.pev_id,     sizeof(pReq->MatchVarField.pev_id   ));
    memcpy(slac_cnf.MatchVarField.pev_mac,  pReq->MatchVarField.pev_mac,    sizeof(pReq->MatchVarField.pev_mac  ));
    memcpy(slac_cnf.MatchVarField.evse_id,  pReq->MatchVarField.evse_id,    sizeof(pReq->MatchVarField.evse_id  ));
    memcpy(slac_cnf.MatchVarField.evse_mac, pReq->MatchVarField.evse_mac,   sizeof(pReq->MatchVarField.evse_mac ));
    memcpy(slac_cnf.MatchVarField.run_id,   pReq->MatchVarField.run_id,     sizeof(pReq->MatchVarField.run_id   ));
    
    
    //1.2:填充 MME 帧
    eError = mme_init(&pCache->stTxCtx, CM_SLAC_MATCH_CNF, pCache->u8TxCtxBuf, sizeof(pCache->u8TxCtxBuf));
    if(eError == MME_SUCCESS)
    {
        eError = mme_put(&pCache->stTxCtx, &slac_cnf, sizeof(slac_cnf), &u32PayloadLen);
    }
    
    
    //2:发送 MME 帧
    bRst = false;
    if(eError == MME_SUCCESS)
    {
        bRst = sMmePacketSend(CM_SLAC_MATCH_REQ, false, pPev, &pCache->stTxCtx);
    }
    
    
    //3:访问保护解除
    xSemaphoreGive(pCache->xSemTx);
    
    return(bRst);
}

















/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_SET_KEY.cnf 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktRecvCmSetKeyCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    set_key_cnf key_cnf;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&key_cnf, 0, sizeof(key_cnf));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &key_cnf, sizeof(key_cnf), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        //此处 HomePlug Green PHY协议定义的 result 字段 0为成功  1为失败
        //但由于高通的芯片做反了，联芯通为了与高通兼容，也按照反的逻辑来做的
        //所以 实际上  0表示失败  1表示成功
        bRst = (key_cnf.result == eCmSetKeyCnfRstFail) ? true : false;
        EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_SET_KEY.cnf, 处理结果:%d(0--失败, 1--成功)", pMmeDev->i32Index, bRst);
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_SLAC_PARM.req 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   EVCC端发送此消息 请求启动SLAC流程 SECC端需要记录该节点
****************************************************************************************************/
bool sMmePktRecvCmSlacParmReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    slac_parm_req slac_req;
    
    struct pev_item *pPev = NULL;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&slac_req, 0, sizeof(slac_req));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &slac_req, sizeof(slac_req), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        if((slac_req.application_type == eCmMsgAppTypePevEvseAssoc) && (slac_req.security_type <= eCmMsgSecurityTypePks))
        {
            //去链表内搜索一个pev节点
            EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_SLAC_PARM.req", pMmeDev->i32Index);
            pPev = search_pev_item_oldest(pCache->stEvse.pev_item_head);
            if(pPev == NULL)
            {
                //找不到任何节点,说明是第一次SLAC, 需要插入该节点
                insert_pev_item(pMmeDev, pCache->stEvse.pev_item_head, slac_req.run_id, ((MME_t *)pEthPkt)->mme_src);
                print_pev_item_list(pCache->stEvse.pev_item_head, true);
                
                EN_SLOGD(TAG, "SECC[%d], pev节点的sMmeSlacTask任务不存在, 创建该任务");
                pPev = search_pev_item_list(pCache->stEvse.pev_item_head, ((MME_t *)pEthPkt)->mme_src);
                xTaskCreate(sMmeSlacTask, "sMmeSlacTask", (512), pPev, 24, NULL);
            }
            else
            {
                if(pPev->bSlacMatchFlag == false)
                {
                    EN_SLOGD(TAG, "SECC[%d], pev节点的sMmeSlacTask任务存在并且未连接PLC,更新参数并执行SLAC匹配");
                    update_pev_item(pMmeDev, pPev, slac_req.run_id, ((MME_t *)pEthPkt)->mme_src);
                    print_pev_item_list(pCache->stEvse.pev_item_head, true);
                    xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmSlacParmReq);
                }
                else
                {
                    EN_SLOGD(TAG, "SECC[%d], pev节点的sMmeSlacTask任务存在并且已连接PLC, 丢弃, 不必重复执行SLAC匹配");
                }
            }
        }
        else
        {
            bRst = false;
            EN_SLOGD(TAG, "SECC[%d], SLAC 接收到 无效 CM_SLAC_PARM.req", pMmeDev->i32Index);
        }
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_START_ATTEN_CHAR.ind 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktRecvCmStartAttenCharInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    char u8Head[64];
    
    bool bRst;
    u32  u32PayloadLen;
    start_atten_char_ind start_ind;
    
    struct pev_item *pPev = NULL;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&start_ind, 0, sizeof(start_ind));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &start_ind, sizeof(start_ind), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = false;
        
        //打印该消息的 run_id 字段
        memset(u8Head, 0, sizeof(u8Head));
        snprintf(u8Head, sizeof(u8Head), "SECC[%d], SLAC 接收:CM_START_ATTEN_CHAR.ind->run_id:", pMmeDev->i32Index);
        EN_HLOGI(TAG, u8Head, start_ind.ACVarField.run_id, sizeof(start_ind.ACVarField.run_id));
        
        pPev = search_pev_item_list(pCache->stEvse.pev_item_head, ((MME_t *)pEthPkt)->mme_src);
        if((pPev != NULL) && (pPev->i32SlacState == eRecvCmSlacParmReq))
        {
            if((start_ind.application_type      == eCmMsgAppTypePevEvseAssoc)
            && (start_ind.security_type         == eCmMsgSecurityTypeNone)
            && (start_ind.ACVarField.num_sounds == pPev->i32SoundsTx)
            && (start_ind.ACVarField.time_out   != 0)
            && (start_ind.ACVarField.resp_type  == 0x01)
            && (memcmp(start_ind.ACVarField.run_id, pPev->u8RunId, sizeof(pPev->u8RunId)) == 0)
            && (memcmp(start_ind.ACVarField.forwarding_sta, pPev->u8MacEvHost, sizeof(pPev->u8MacEvHost)) == 0))
            {
                bRst = true;
                pPev->i32SlacState = eRecvCmStartAttenCharInd;
                xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmStartAttenCharInd); //设置此事件推动SLAC流程往后执行
            }
        }
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_MNBC_SOUND.ind 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   EVCC端是联芯通的芯片或其他制造商的芯片时 帧格式有差异
****************************************************************************************************/
bool sMmePktRecvCmMnbcSoundInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    mnbc_sound_ind mnbc_ind;
    mnbc_sound_ind_q mnbc_ind_q;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    if((pCache->stRxCtx.tail - pCache->stRxCtx.head) == sizeof(mnbc_ind))
    {
        memset(&mnbc_ind, 0, sizeof(mnbc_ind));
        mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
        mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
        if(mme_pull(&pCache->stRxCtx, &mnbc_ind, sizeof(mnbc_ind), &u32PayloadLen) == MME_SUCCESS)
        {
            EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_MNBC_SOUND.ind---(%d)", pMmeDev->i32Index, mnbc_ind.MSVarField.cnt);
            bRst = true;
        }
    }
    else// This case is for other vendor
    {
        memset(&mnbc_ind_q, 0, sizeof(mnbc_ind_q));
        mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
        mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
        if(mme_pull(&pCache->stRxCtx, &mnbc_ind_q, sizeof(mnbc_ind_q), &u32PayloadLen) == MME_SUCCESS)
        {
            EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_MNBC_SOUND.ind---(%d)", pMmeDev->i32Index, mnbc_ind_q.MSVarField.cnt);
            bRst = true;
        }
    }
    
    
    //2:payload 内字段解析
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_ATTEN_PROFILE.ind 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktRecvCmAttenProfileInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    i32  i;
    bool bRst;
    u32  u32PayloadLen;
    atten_profile_ind atten_ind;
    
    struct pev_item *pPev = NULL;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&atten_ind, 0, sizeof(atten_ind));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &atten_ind, sizeof(atten_ind), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        EN_SLOGI(TAG, "SECC[%d], SLAC 接收:CM_ATTEN_PROFILE.ind", pMmeDev->i32Index);
        
        pPev = search_pev_item_list(pCache->stEvse.pev_item_head, atten_ind.pev_mac);
        if(pPev != NULL)
        {
            if(pPev->i32SlacState >= eRecvCmStartAttenCharInd)
            {
                pPev->i32SoundsRx++;
                xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmAttenProfileInd);
                if(pPev->i32SoundsRx >= pPev->i32SoundsTx)
                {
                    xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmAttenProfileIndX);
                }
                
                for(i = 0; i < cAttenuationGroup; i++)
                {
                    //加入插入损耗的补偿
                    pPev->AAG[i] += (atten_ind.AAG[i] - 15);                    //15为损耗补偿
                }
                bRst = true;
            }
        }
        else
        {
            EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_ATTEN_PROFILE.ind ---从未知的PEV节点", pMmeDev->i32Index);
            bRst = false;
        }
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_ATTEN_CHAR.rsp 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktRecvCmAttenCharRsp(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    atten_char_rsp atten_rsp;
    
    struct pev_item *pPev = NULL;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&atten_rsp, 0, sizeof(atten_rsp));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &atten_rsp, sizeof(atten_rsp), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = false;
        if((atten_rsp.security_type     == eCmMsgSecurityTypeNone)
        && (atten_rsp.application_type  == eCmMsgAppTypePevEvseAssoc) 
        && (atten_rsp.ACVarField.result == eCmAttenCharRspAcVarFieldRstSuccess)
        && (memcmp(atten_rsp.ACVarField.resp_ID,   u8PesrId, sizeof(u8PesrId)) == 0)
        && (memcmp(atten_rsp.ACVarField.source_ID, u8PesrId, sizeof(u8PesrId)) == 0))
        {
            pPev = search_pev_item_list(pCache->stEvse.pev_item_head, atten_rsp.ACVarField.source_address);
            if(pPev != NULL)
            {
                if((pPev->i32AttenCharIndRetryCnt >= 0)                         //重试次数用尽, 如果接收到正确的, 也不做处理
                && (memcmp(atten_rsp.ACVarField.run_id,    pPev->u8RunId, sizeof(pPev->u8RunId)) == 0))
                {
                    bRst = true;
                    pPev->i32SlacState = eRecvCmAttenCharRsp;
                }
            }
        }
        EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_ATTEN_CHAR.rsp, 处理结果:%d(0--失败, 1--成功)", pMmeDev->i32Index, bRst);
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   MME 帧处理函数之 CM_SLAC_MATCH.req 消息处理
* Author                                :   Hall
* Creat Date                            :   2023-07-04
* notice                                :   
****************************************************************************************************/
bool sMmePktRecvCmSlacMatchReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen)
{
    bool bRst;
    u32  u32PayloadLen;
    slac_match_req slac_req;
    
    struct pev_item *pPev = NULL;
    stMmeCache_t *pCache = NULL;
    
    pCache = pMmeCache[pMmeDev->i32Index];
    if(pCache == NULL)
    {
        return(false);
    }
    
    //1:先从 mme 会话中提取 payload
    bRst = false;
    memset(&slac_req, 0, sizeof(slac_req));
    mme_init(&pCache->stRxCtx, 0, pCache->u8RxCtxBuf, sizeof(pCache->u8RxCtxBuf));
    mme_remove_header(((MME_t *)pEthPkt)->mmtype, pEthPkt, i32EthPktLen, &pCache->stRxCtx);
    if(mme_pull(&pCache->stRxCtx, &slac_req, sizeof(slac_req), &u32PayloadLen) == MME_SUCCESS)
    {
        bRst = true;
    }
    
    //2:payload 内字段解析
    if(bRst == true)
    {
        bRst = false;
        pPev = search_pev_item_list(pCache->stEvse.pev_item_head, ((MME_t *)pEthPkt)->mme_src);
        if (pPev != NULL)
        {
            if((pPev->i32SlacState == eRecvCmAttenCharRsp)                      //
            && (slac_req.security_type    <= eCmMsgSecurityTypePks)
            && (slac_req.application_type == eCmMsgAppTypePevEvseAssoc)
            && (slac_req.mvflength[0]     == cCmSlacMatchReqMvfLength)
            && (memcmp(slac_req.MatchVarField.pev_id,   u8PesrId,                     sizeof(u8PesrId)) == 0)
            && (memcmp(slac_req.MatchVarField.pev_mac,  pPev->u8MacEvHost,            sizeof(pPev->u8MacEvHost)) == 0)
            && (memcmp(slac_req.MatchVarField.evse_id,  u8PesrId,                     sizeof(u8PesrId)) == 0)
            && (memcmp(slac_req.MatchVarField.evse_mac, pPev->stMmeDev.u8MacEvseHsot, sizeof(pPev->stMmeDev.u8MacEvseHsot)) == 0)
            && (memcmp(slac_req.MatchVarField.run_id,   pPev->u8RunId,                sizeof(pPev->u8RunId)) == 0))
            {
                EN_SLOGD(TAG, "SECC[%d], SLAC 接收:CM_SLAC_MATCH.req", pMmeDev->i32Index);
                
                bRst = sMmePktSendCmSlacMatchCnf(pPev, &slac_req);
                if(bRst == true)
                {
                    pPev->i32SlacState = eSendCmSlacMatchCnf;
                    xEventGroupSetBits(pPev->xEvtSlac, cEvtSlacCmSlacMatchReq);
                }
            }
            else
            {
                EN_SLOGD(TAG, "SECC[%d], SLAC 接收:无效 CM_SLAC_MATCH.req", pMmeDev->i32Index);
            }
        }
        else
        {
            EN_SLOGI(TAG, "SECC[%d], SLAC 接收:CM_SLAC_MATCH.req, 从未知的PEV节点.", pMmeDev->i32Index);
        }
    }
    
    //3:处理结果赋值
    stMmeCmdTypeMap[u16Index].bProcRst = bRst;
    
    //4:解锁
    if(stMmeCmdTypeMap[u16Index].xSemAck != NULL)
    {
        xSemaphoreGive(stMmeCmdTypeMap[u16Index].xSemAck);
    }
    
    return(bRst);
}












