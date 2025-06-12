/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   bms_ccs_mme_slac_data_def.h
* Description                           :   MME协议实现 之 SLAC相关的MME帧
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-07-07
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_mme_slac_data_def_h
#define _bms_ccs_mme_slac_data_def_h







//--------------------------------------SLAC相关定义----------------------------


//<ISO 15118-3-2015.pdf>@[V2G3-M08-01]
//<din spec 70121 2014-12 212p.pdf>@[V2G-DC-568]
#define C_EVSE_match_parallel           (5)




//以下几个id的长度
//PEV_ID   /EVSE_ID@CM_SLAC_MATCH.req
//SOURCE_ID/RESP_ID@CM_ATTEN_CHAR.rsp
#define pev_evse_source_resp_id_len     (17)



//<ISO 15118-3-2015.pdf>@Table A.7 定义 MVFLength 字段长度值
#define cCmSlacMatchReqMvfLength        (0x3e)






//CM_SET_KEY req/cnf

//CM_SET_KEY.cnf Result 字段值定义
typedef enum
{
    eCmSetKeyCnfRstSuccess              = 0x00,                                 //
    eCmSetKeyCnfRstFail,                                                        //
    
    eCmSetKeyCnfRstMax
}eCmSetKeyCnfRst_t;








//APPLICATION_TYPE 字段值定义----多个消息公用
typedef enum
{
    eCmMsgAppTypePevEvseAssoc           = 0x00,                                 //PEV-EVSE Association
    
    eCmMsgAppTypeMax                    = 0xff                                  //Reserved
}__attribute__((packed)) eCmMsgAppType_t;

//SECURITY_TYPE 字段值定义----多个消息公用
typedef enum
{
    eCmMsgSecurityTypeNone              = 0x00,                                 //No Security
    eCmMsgSecurityTypePks               = 0x01,                                 //Public Key Signature.
    
    eCmMsgSecurityTypeMax               = 0xff                                  //Reserved
}__attribute__((packed)) eCmMsgSecurityType_t;













//CM_ATTEN_CHAR ind/rsp

//CM_ATTEN_CHAR.rsp ACVarField.Result 字段值定义
typedef enum
{
    eCmAttenCharRspAcVarFieldRstSuccess = 0x00,                                 //
    eCmAttenCharRspAcVarFieldRstFail,                                           //
    
    eCmAttenCharRspAcVarFieldRstMax
}eCmAttenCharRspAcVarFieldRst_t;




















#endif 



