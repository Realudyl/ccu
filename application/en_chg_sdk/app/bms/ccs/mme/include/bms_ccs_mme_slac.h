/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_slac.h
* Description                           :   MME协议实现 之 SLAC相关的MME帧
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_mme_slac_h
#define _bms_ccs_mme_slac_h




#include "bms_ccs_mme_node.h"
#include "bms_ccs_mme_op.h"
#include "bms_ccs_mme_data_def.h"










//xEvtSlac 事件组事件定义
#define cEvtSlacAll                     (0x00ffffff)                            //所有事件
#define cEvtSlacNone                    (0)                                     //没有事件
#define cEvtSlacCpDisconnect            (1 << 0)                                //CP连接断开或异常
#define cEvtSlacMultiEvFail             (1 << 1)                                //多EV端请求冲突---对本MME设备发起的多个SLAC请求中，第一个完成匹配时 其他尚未完成匹配的请求需要全部中断
#define cEvtSlacCmSlacParmReq           (1 << 2)                                //收到消息 CM_SLAC_PARM.req
#define cEvtSlacCmStartAttenCharInd     (1 << 3)                                //收到消息 CM_START_ATTEN_CHAR.ind
#define cEvtSlacCmAttenProfileInd       (1 << 4)                                //收到消息 CM_ATTEN_PROFILE.ind 1次
#define cEvtSlacCmAttenProfileIndX      (1 << 5)                                //收到消息 CM_ATTEN_PROFILE.ind x次
#define cEvtSlacCmAttenCharRsp          (1 << 6)                                //收到消息 CM_ATTEN_CHAR.rsp
#define cEvtSlacCmAttenCharIndRetryOut  (1 << 7)                                //发送消息 CM_ATTEN_CHAR.ind重发次数用尽
#define cEvtSlacCmSlacMatchReq          (1 << 8)                                //收到消息 CM_SLAC_MATCH.req
#define cEvtSlacJoinLogic               (1 << 9)                                //TT_match_join 定时器逻辑
#define cEvtSlacNetInit                 (1 << 10)                               //SLAC 重新初始化网络参数













#define AAG_STANDARD                    28                                      //AAG补偿基准



//MME SLAC 不同消息的结构payload定义-----------------------------------------------------------


//0:CM_SET_KEY
// CM_SET_KEY.REQ
#pragma pack(1)
typedef struct
{
    uint8_t key_type;
    uint8_t my_nonce[4];
    uint8_t your_nonce[4];
    uint8_t pid;
    uint8_t prn[2];
    uint8_t pmn;
    uint8_t cco_capability;
    uint8_t nid[7];
    uint8_t new_eks;
    uint8_t new_key[16];
} set_key_req;
// CM_SET_KEY.CNF
typedef struct
{
    uint8_t result;
    uint8_t my_nonce[4];
    uint8_t your_nonce[4];
    uint8_t pid;
    char prn[2];
    uint8_t pmn;
    uint8_t cco_capability;
} set_key_cnf;




//1:CM_SLAC_PARM
// CM_SLAC_PARM.REQ
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    uint8_t run_id[8];
    uint8_t ciphersuitesetsize;
    char ciphersuite[2];
} slac_parm_req;
// CM_SLAC_PARM.CNF
typedef struct
{
    uint8_t m_sound_target[6];
    uint8_t num_sounds;
    uint8_t time_out;         // TT_EVSE_match_MNBC
    uint8_t resp_type;
    uint8_t forwarding_sta[6];
    uint8_t application_type;
    uint8_t security_type;
    uint8_t run_id[8];
    char ciphersuite[2];
} slac_parm_cnf;





//2:CM_START_ATTEN_CHAR.IND
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    struct
    {
        uint8_t num_sounds;    // C_EV_match_MNBC
        uint8_t time_out;         // TT_EVSE_match_MNBC
        uint8_t resp_type;
        uint8_t forwarding_sta[6];
        uint8_t run_id[8];
    } ACVarField;
} start_atten_char_ind;




//3:CM_MNBC_SOUND
// CM_MNBC_SOUND.IND
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    struct
    {
        char sender_ID[17];
        uint8_t cnt;
        uint8_t run_id[8];
        char rsvd[8];             //other vendor didn't include this var
        char rnd[16];
    } MSVarField;
} mnbc_sound_ind;
// CM_MNBC_SOUND.IND for other vendor
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    struct
    {
        char sender_ID[17];
        uint8_t cnt;
        uint8_t run_id[8];
        //char rsvd[8];             //other vendor didn't include this var
        char rnd[16];
    } MSVarField;
} mnbc_sound_ind_q;






//4:CM_ATTEN_PROFILE.IND
typedef struct
{
    uint8_t pev_mac[6];
    uint8_t num_groups[1];
    uint8_t rsvd[1];
    uint8_t AAG[cAttenuationGroup];
} atten_profile_ind;




//5:CM_ATTEN_CHAR
// CM_ATTEN_CHAR.IND
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    struct
    {
        uint8_t source_address[6];
        uint8_t run_id[8];
        uint8_t source_ID[17];
        uint8_t resp_ID[17];
        uint8_t num_sounds;
        struct
        {
            uint8_t num_groups;              // numGroups comes from CM_ATTEN_PROFILE.IND's NumGroups
            uint8_t AAG[cAttenuationGroup];  // "58"  comes from CM_ATTEN_PROFILE.IND's NumGroups
        } ATTEN_PROFILE;
    } ACVarField;
} atten_char_ind;
// CM_ATTEN_CHAR.RSP
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    struct
    {
        uint8_t source_address[6];
        uint8_t run_id[8];
        uint8_t source_ID[17];
        uint8_t resp_ID[17];
        uint8_t result;
    } ACVarField;
} atten_char_rsp;




//6:CM_SLAC_MATCH
// CM_SLAC_MATCH.REQ
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    uint8_t mvflength[2];
    struct
    {
        uint8_t pev_id[17];
        uint8_t pev_mac[6];
        uint8_t evse_id[17];
        uint8_t evse_mac[6];
        uint8_t run_id[8];
        uint8_t rsvd[8];
    } MatchVarField;
} slac_match_req;
// CM_SLAC_MATCH.CNF
typedef struct
{
    uint8_t application_type;
    uint8_t security_type;
    uint8_t mvflength[2];
    struct
    {
        uint8_t pev_id[17];
        uint8_t pev_mac[6];
        uint8_t evse_id[17];
        uint8_t evse_mac[6];
        uint8_t run_id[8];
        uint8_t rsvd[8];
        uint8_t nid[7];
        uint8_t rsvd2;
        uint8_t nmk[16];
    } MatchVarField;
} slac_match_cnf;
#pragma pack()









































extern void  sMmeSlacTaskSend(void *pParam);


extern void  sMmeSlacSetEvtCpDisconn(struct pev_item *pHead);


//MME SLAC 帧发送函数
extern bool  sMmePktSendCmAttenCharInd(struct pev_item *pPev);

//MME SLAC 帧接收处理函数
extern bool  sMmePktRecvCmSetKeyCnf(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmSlacParmReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmStartAttenCharInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmMnbcSoundInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmAttenProfileInd(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmAttenCharRsp(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
extern bool  sMmePktRecvCmSlacMatchReq(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);






#endif /* _Slac_h */

