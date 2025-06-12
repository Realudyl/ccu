/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_node.h
* Description                           :   SLAC 匹配过程中 节点管理实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   从联芯通<HLE_SDK_v1.1>工程移植过来 再做命名和排版优化
****************************************************************************************************/
#ifndef _bms_ccs_mme_node_h
#define _bms_ccs_mme_node_h

/*  plc hle utils project {{{
 *
 * Copyright (C) 2018 MStar Semiconductor
 *
 * <<<Licence>>>
 *
 * }}} */
#include <stdint.h>
#include <time.h>

#include "en_common_eth.h"


#include "bms_ccs_mme_op.h"
#include "bms_ccs_mme_data_def.h"






/**
* This struct is used by PEV to manage its current state
*/
typedef enum
{
    SENT_CM_SLAC_PARM_REQ,
    RECEIVED_CM_SLAC_PARM_CNF,
    SENT_CM_START_ATTEN_CHAR_IND,
    RECEIVED_CM_ATTEN_CHAR_IND,
    RECEIVED_CM_SLAC_MATCH_CNF
}pev_state;




/**
 * This struct is used by EVSE to manage each potential PEVs
 */
typedef enum
{
    eSendCmSekKey,                                                              //需要发送 CM_SET_KEY.req 消息
    eRecvCmSlacParmReq,                                                         //    收到 CM_SLAC_PARM.req 消息
    eRecvCmStartAttenCharInd,                                                   //    收到 CM_START_ATTEN_CHAR.ind 消息
    eSendCmAttenCharInd,                                                        //    发出 CM_ATTEN_CHAR.ind 消息
    eRecvCmAttenCharRsp,                                                        //    收到 CM_ATTEN_CHAR.rsp 消息
    eSendCmSlacMatchCnf,                                                        //    发出 CM_SLAC_MATCH.cnf 消息
    
}PEV_matching_state;




/**
 * This struct is used by PEV to manage each potential EVSEs
 */
typedef struct evse_item
{
    uint8_t mac[ETH_ALEN];
    int send_cn_atten_char_ind_flag;
    int AAG;
    struct evse_item *next;
}evse_item;




/**
 * This struct is used by EVSE to manage each potential PEVs
 */
typedef struct pev_item
{
    //EVSE端在 CM_SLAC_PARM.cnf        消息中设定 i32SoundsTx
    //EV  端在 CM_START_ATTEN_CHAR.ind 消息中确认 i32SoundsTx
    i32                                 i32SoundsTx;                            //要发送的M-Sound数量
    i32                                 i32SoundsRx;                            //已收到的M-Sound数量
    i32                                 i32SlacState;
    i32                                 i32AttenCharIndRetryCnt;                //CM_ATTEN_CHAR.ind 消息的重发送次数
    i32                                 i32TtMatchJoinWaitCnt;                  //检测是否加入逻辑网络的检查次数
    bool                                bSlacMatchFlag;                         //SLAC 匹配成功标志
    
    u8                                  u8RunId[8];                             //EV端的 run id
    u8                                  u8MacEvHost[ETH_ALEN];                  //EV端的 host Mac地址
    i16                                 AAG[cAttenuationGroup];
    stMmeDev_t                          stMmeDev;                               //该节点的 MME设备信息
    EventGroupHandle_t                  xEvtSlac;                               //该节点的 SLAC 过程事件组
    
    struct pev_item                     *next;
}pev_item;




typedef struct pev
{
    int state;
    uint8_t run_id[8];
    uint8_t nid[7];
    uint8_t nmk[16];
    uint8_t received_nid[7];
    uint8_t received_nmk[16];
    pev_state current_state;
    
    int cm_slac_parm_timer_flag;
    int cm_slac_parm_req_retry;
    
    int cm_start_atten_char_timer_flag;
    int cm_slac_match_req_retry;
    int potential_EVSE;
    struct evse_item *evse_item_head;
    
}pev;




typedef struct evse
{
    uint8_t nid[7];
    uint8_t nmk[16];
    struct pev_item *pev_item_head;
}evse;







/**
 * create PEV_item for EVSE tp manage potential PEV
 * \param  run_id run ID for PEV_item
 * \param  mac MAC Address for PEV_item
 * \return  created PEV_item
 */
pev_item *create_pev_item(void *pMmeDev, uint8_t *run_id, uint8_t *mac);

/**
 * Update PEV_item
 * \param  pev PEV that you want to update its information
 * \param  run_id run ID for PEV_item
 * \param  mac MAC Address for PEV_item
 * \param  iface  selected communication interface
 */
void update_pev_item(void *pMmeDev, pev_item *pev, uint8_t *run_id, uint8_t *mac);



/**
 * insert PEV_item into PEV List
 * \param  head  head of EVSE list
 * \param  run_id run ID for PEV_item
 * \param  mac  MAC Address for EVSE_item
 */
void insert_pev_item(void *pMmeDev, struct pev_item *head, uint8_t *run_id, uint8_t *mac);




void delete_pev_item(struct pev_item *head, struct pev_item *pPev);



/**
 * print each PEV_item in the PEV List
 * \param  head  head of PEV list
 */
i32 print_pev_item_list(struct pev_item *head, bool bPrintFlag);



/**
 * Search the PEV with the specific MAC Address
 * \param  head  head of PEV list
 * \param  mac  MAC Address for PEV_item
 * \return  PEV_item with that specific MAC Address, NULL if not found
 */
pev_item *search_pev_item_list(pev_item *head, uint8_t *mac);



pev_item *search_pev_item_oldest(struct pev_item *head);




#endif /* slac_inc_node_h */







