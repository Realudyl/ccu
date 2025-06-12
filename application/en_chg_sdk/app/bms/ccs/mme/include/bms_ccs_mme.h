/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme.h
* Description                           :   MME协议实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_mme_h_
#define _bms_ccs_mme_h_
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "adc_app_sample.h"
#include "netif_mse102x.h"
#include "mse102x_drv.h"

#include "bms_ccs_mme_op.h"
#include "bms_ccs_mme_node.h"
#include "bms_ccs_mme_data_def.h"

#include "bms_ccs_mme_slac.h"
#include "bms_ccs_mme_ven_spec.h"
















#define MME_TYPE                        0x88e1                                  //MME Ethertype (MStar MME Protocol)
#define MME_VERSION                     0x01
#define MME_HEADER_SIZE                 5                                       //MMV - MMTYPE - FMI
#define MME_MAX_SIZE                    (ETH_PACKET_MAX_PAYLOAD_SIZE - MME_HEADER_SIZE)//maximum size of MME payload
#define MME_MIN_SIZE                    (ETH_PACKET_MIN_PAYLOAD_SIZE - MME_HEADER_SIZE)//minimum size of MME payload
#define MME_TOUT                        5                                       //communication timeout in seconds

#define OUI_MSTAR                       "\x00\x13\xd7"
#define MSTAR_OUI_SIZE                  3

#define SET_MME_PAYLOAD_MAX_SIZE(type, val) (val = MMTYPE_IS_VS(type) ? MME_MAX_SIZE - MSTAR_OUI_SIZE : MME_MAX_SIZE)
#define SET_MME_PAYLOAD_MIN_SIZE(type, val) (val = MMTYPE_IS_VS(type) ? MME_MIN_SIZE - MSTAR_OUI_SIZE : MME_MIN_SIZE)
#define SET_MME_HEADER_SIZE(type, val)      (val = MMTYPE_IS_VS(type) ? sizeof(MME_t) + MSTAR_OUI_SIZE : sizeof(MME_t))

#define MME_TYPE_MASK                   (0x0003)
#define CHARGING_TIME                   (3000)










//MME 消息类型定义
enum mmtype_t
{
    CC_CCO_APPOINT_REQ                  = CC_CCO_APPOINT            + MME_REQ,
    CC_CCO_APPOINT_CNF                  = CC_CCO_APPOINT            + MME_CNF,
    CC_LINK_INFO_REQ                    = CC_LINK_INFO              + MME_REQ,
    CC_LINK_INFO_CNF                    = CC_LINK_INFO              + MME_CNF,
    CC_LINK_INFO_IND                    = CC_LINK_INFO              + MME_IND,
    CC_LINK_INFO_RSP                    = CC_LINK_INFO              + MME_RSP,
    CC_DISCOVER_LIST_REQ                = CC_DISCOVER_LIST          + MME_REQ,
    CC_DISCOVER_LIST_CNF                = CC_DISCOVER_LIST          + MME_CNF,
    CC_DISCOVER_LIST_IND                = CC_DISCOVER_LIST          + MME_IND,
    CC_WHO_RU_REQ                       = CC_WHO_RU                 + MME_REQ,
    CC_WHO_RU_CNF                       = CC_WHO_RU                 + MME_CNF,
    CM_UNASSOCIATED_STA_IND             = CM_UNASSOCIATED_STA       + MME_IND,
    CM_ENCRYPTED_PAYLOAD_IND            = CM_ENCRYPTED_PAYLOAD      + MME_IND,
    CM_ENCRYPTED_PAYLOAD_RSP            = CM_ENCRYPTED_PAYLOAD      + MME_RSP,
    CM_SET_KEY_REQ                      = CM_SET_KEY                + MME_REQ,
    CM_SET_KEY_CNF                      = CM_SET_KEY                + MME_CNF,
    CM_GET_KEY_REQ                      = CM_GET_KEY                + MME_REQ,
    CM_GET_KEY_CNF                      = CM_GET_KEY                + MME_CNF,
    CM_SLAC_PARM_REQ                    = CM_SLAC_PARM              + MME_REQ,
    CM_SLAC_PARM_CNF                    = CM_SLAC_PARM              + MME_CNF,
    CM_SLAC_MATCH_REQ                   = CM_SLAC_MATCH             + MME_REQ,
    CM_SLAC_MATCH_CNF                   = CM_SLAC_MATCH             + MME_CNF,
    CM_START_ATTEN_CHAR_IND             = CM_START_ATTEN_CHAR       + MME_IND,
    CM_ATTEN_CHAR_IND                   = CM_ATTEN_CHAR             + MME_IND,
    CM_ATTEN_CHAR_RSP                   = CM_ATTEN_CHAR             + MME_RSP,
    CM_ATTEN_PROFILE_IND                = CM_ATTEN_PROFILE          + MME_IND,
    CM_AMP_MAP_REQ                      = CM_AMP_MAP                + MME_REQ,
    CM_AMP_MAP_CNF                      = CM_AMP_MAP                + MME_CNF,
    CM_BRG_INFO_REQ                     = CM_BRG_INFO               + MME_REQ,
    CM_BRG_INFO_CNF                     = CM_BRG_INFO               + MME_CNF,
    CM_CONN_INFO_REQ                    = CM_CONN_INFO              + MME_REQ,
    CM_CONN_INFO_CNF                    = CM_CONN_INFO              + MME_CNF,
    CM_STA_CAP_REQ                      = CM_STA_CAP                + MME_REQ,
    CM_STA_CAP_CNF                      = CM_STA_CAP                + MME_CNF,
    CM_NW_INFO_REQ                      = CM_NW_INFO                + MME_REQ,
    CM_NW_INFO_CNF                      = CM_NW_INFO                + MME_CNF,
    CM_GET_BEACON_REQ                   = CM_GET_BEACON             + MME_REQ,
    CM_GET_BEACON_CNF                   = CM_GET_BEACON             + MME_CNF,
    CM_HFID_REQ                         = CM_HFID                   + MME_REQ,
    CM_HFID_CNF                         = CM_HFID                   + MME_CNF,
    CM_MME_ERROR_IND                    = CM_MME_ERROR              + MME_IND,
    CM_MNBC_SOUND_IND                   = CM_MNBC_SOUND             + MME_IND,
    
    
    //供应商特定的MME帧
    VS_GET_VERSION_REQ                  = VS_GET_VERSION            + MME_REQ,
    VS_GET_VERSION_CNF                  = VS_GET_VERSION            + MME_CNF,
    
    VS_RESET_REQ                        = VS_RESET                  + MME_REQ,
    VS_RESET_CNF                        = VS_RESET                  + MME_CNF,
    
    VS_SET_KEY_REQ                      = VS_SET_KEY                + MME_REQ,
    VS_SET_KEY_CNF                      = VS_SET_KEY                + MME_CNF,
    
    VS_GET_TONEMASK_REQ                 = VS_GET_TONEMASK           + MME_REQ,
    VS_GET_TONEMASK_CNF                 = VS_GET_TONEMASK           + MME_CNF,
    
    VS_GET_ETH_PHY_REQ                  = VS_GET_ETH_PHY            + MME_REQ,
    VS_GET_ETH_PHY_CNF                  = VS_GET_ETH_PHY            + MME_CNF,
    
    VS_ETH_STATS_REQ                    = VS_ETH_STATS              + MME_REQ,
    VS_ETH_STATS_CNF                    = VS_ETH_STATS              + MME_CNF,
    
    VS_GET_STATUS_REQ                   = VS_GET_STATUS             + MME_REQ,
    VS_GET_STATUS_CNF                   = VS_GET_STATUS             + MME_CNF,
    
    VS_GET_TONEMAP_REQ                  = VS_GET_TONEMAP            + MME_REQ,
    VS_GET_TONEMAP_CNF                  = VS_GET_TONEMAP            + MME_CNF,
    
    VS_GET_SNR_REQ                      = VS_GET_SNR                + MME_REQ,
    VS_GET_SNR_CNF                      = VS_GET_SNR                + MME_CNF,
    
    VS_GET_SPECTRUM_REQ                 = VS_GET_SPECTRUM           + MME_REQ,
    VS_GET_SPECTRUM_CNF                 = VS_GET_SPECTRUM           + MME_CNF,
    
    VS_GET_LINK_STATS_REQ               = VS_GET_LINK_STATS         + MME_REQ,
    VS_GET_LINK_STATS_CNF               = VS_GET_LINK_STATS         + MME_CNF,
    
    VS_GET_AMP_MAP_REQ                  = VS_GET_AMP_MAP            + MME_REQ,
    VS_GET_AMP_MAP_CNF                  = VS_GET_AMP_MAP            + MME_CNF,
    
    VS_GET_NW_INFO_REQ                  = VS_GET_NW_INFO            + MME_REQ,
    VS_GET_NW_INFO_CNF                  = VS_GET_NW_INFO            + MME_CNF,
    
    VS_SET_CAPTURE_STATE_REQ            = VS_SET_CAPTURE_STATE      + MME_REQ,
    VS_SET_CAPTURE_STATE_CNF            = VS_SET_CAPTURE_STATE      + MME_CNF,
    
    VS_SET_NVRAM_REQ                    = VS_SET_NVRAM              + MME_REQ,
    VS_SET_NVRAM_CNF                    = VS_SET_NVRAM              + MME_CNF,
    
    VS_GET_NVRAM_REQ                    = VS_GET_NVRAM              + MME_REQ,
    VS_GET_NVRAM_CNF                    = VS_GET_NVRAM              + MME_CNF,
    
    VS_GET_PWM_STATS_REQ                = VS_GET_PWM_STATS          + MME_REQ,
    VS_GET_PWM_STATS_CNF                = VS_GET_PWM_STATS          + MME_CNF,
    VS_GET_PWM_STATS_IND                = VS_GET_PWM_STATS          + MME_IND,
    
    VS_GET_PWM_CONF_REQ                 = VS_GET_PWM_CONF           + MME_REQ,
    VS_GET_PWM_CONF_CNF                 = VS_GET_PWM_CONF           + MME_CNF,
    
    VS_SET_PWM_CONF_REQ                 = VS_SET_PWM_CONF           + MME_REQ,
    VS_SET_PWM_CONF_CNF                 = VS_SET_PWM_CONF           + MME_CNF,
    
    VS_PWM_GENERATION_REQ               = VS_PWM_GENERATION         + MME_REQ,
    VS_PWM_GENERATION_CNF               = VS_PWM_GENERATION         + MME_CNF,
    
    VS_SPI_STATS_REQ                    = VS_SPI_STATS              + MME_REQ,
    VS_SPI_STATS_CNF                    = VS_SPI_STATS              + MME_CNF,
    
    VS_SET_DSP_RECORD_STATE_REQ         = VS_SET_DSP_RECORD_STATE   + MME_REQ,
    VS_SET_DSP_RECORD_STATE_CNF         = VS_SET_DSP_RECORD_STATE   + MME_CNF,
    
    VS_GET_DSP_RECORD_STATUS_REQ        = VS_GET_DSP_RECORD_STATUS  + MME_REQ,
    VS_GET_DSP_RECORD_STATUS_CNF        = VS_GET_DSP_RECORD_STATUS  + MME_CNF,
    
    VS_SET_TX_CAL_REQ                   = VS_SET_TX_CAL             + MME_REQ,
    VS_SET_TX_CAL_CNF                   = VS_SET_TX_CAL             + MME_CNF,
    
    VS_FILE_ACCESS_REQ                  = VS_FILE_ACCESS            + MME_REQ,
    VS_FILE_ACCESS_CNF                  = VS_FILE_ACCESS            + MME_CNF,
    
};
typedef enum mmtype_t mmtype_t;






//MME 消息结构head定义
typedef struct
{
    unsigned char                       mme_dest[ETH_ALEN];                     /* Destination node                         */
    unsigned char                       mme_src[ETH_ALEN];                      /* Source node                              */
    //unsigned int                        vlan_tag;                               /* ieee 802.1q VLAN tag (optional) [0x8100] */
    unsigned short                      mtype;                                  /* 0x88e1 (iee assigned Ethertype)          */
    unsigned char                       mmv;                                    /* Management Message Version               */
    unsigned short                      mmtype;                                 /* Management Message Type                  */
    unsigned short                      fmi;                                    /* Fragmentation Management Info            */
    //unsigned char                       buffer[41];
}__attribute__((packed)) MME_t;


typedef struct
{
    unsigned char                       mme_dest[ETH_ALEN];                     /* Destination node                         */
    unsigned char                       mme_src[ETH_ALEN];                      /* Source node                              */
    unsigned int                        vlan_tag;                               /* ieee 802.1q VLAN tag (optional) [0x8100] */
    unsigned short                      mtype;                                  /* 0x88e1 (iee assigned Ethertype)          */
    unsigned char                       mmv;                                    /* Management Message Version               */
    unsigned short                      mmtype;                                 /* Management Message Type                  */
    unsigned short                      fmi;                                    /* Fragmentation Management Info            */
}__attribute__((packed)) MME_vlan_t;















//MME协议 接收帧map结构定义
typedef struct
{
    mmtype_t                            eType;                                  //报文类型
    const char                          *pString;                               //该报文类型描述字符串
    SemaphoreHandle_t                   xSemAck;                                //该报文的接收信号量句柄
    i32                                 i32WaitTime;                            //该报文的最大等待时间 单位：ms 为0表示该报文信号量无需创建和检查
    bool                                bProcRst;                               //该报文的数据处理结果
    
    //该报文的接收处理函数
    bool                                (*pRecv)(u16 u16Index, stMmeDev_t *pMmeDev, void *pEthPkt, i32 i32EthPktLen);
}stMmeCmdTypeMap_t;





































//MME 协议 缓存结构定义
typedef struct
{
    eChgCpState_t                       eCpState;                               //CP状态
    ePileGunIndex_t                     eGunIndex;                              //对应的枪序号, 操作CP资源需要
    
    evse                                stEvse;
    TaskHandle_t                        xTaskSlacSend;                          //sMmeSlacTaskSend 任务句柄
    
    u8                                  u8RxBuf[ETH_PACKET_MAX_VLAN_SIZE];      //接收buf
    u8                                  u8TxBuf[ETH_PACKET_MAX_VLAN_SIZE];      //发送buf
    
    
    //MME 发送会话资源---发送处理函数要从会话结构体向 以太网帧 中填充 MME帧 再发送
    SemaphoreHandle_t                   xSemTx;                                 //发送 锁
    mme_ctx_t                           stTxCtx;                                //发送 MME会话结构体
    u8                                  u8TxCtxBuf[ETH_PACKET_MAX_VLAN_SIZE];   //发送 MME会话buf
    
    //MME 接收会话资源---接收处理函数要用会话结构体从 以太网帧 中提取 MME帧 再处理
    mme_ctx_t                           stRxCtx;                                //接收 MME会话结构体
    u8                                  u8RxCtxBuf[ETH_PACKET_MAX_VLAN_SIZE];   //接收 MME会话buf
    
    
    //PLC link state 周期查询资源
    u32                                 u32TimeOld;                             //最近一次查询的时间戳
    eMmeVsGetStatusLinkState_t          eLinkState;                             //此MME设备(PLC载波芯片上报)的link state
    
    //接收数据的缓存
    stMmeVsCache_t                      stMmeVs;                                //供应商特定MME帧
    
    //外部传入的参数
    void                                (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn);//回调函数:CP PWM控制函数
    
}stMmeCache_t;
























extern bool  sMmeInit(stMmeDev_t *pMmeDev, ePileGunIndex_t eGunIndex, void (*pCpPwmControl)(ePileGunIndex_t eGunIndex, bool bOn));
extern void  sMmePacketPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);

extern bool  sMmeSetNetInit(i32 i32MmeDevIndex);
extern void  sMmeSetCpState(eChgCpState_t eCpState);
extern bool  sMmeGetSlacMatchFlag(i32 i32MmeDevIndex);
extern struct pev_item *sMmeGetPevHead(i32 i32MmeDevIndex);

extern bool  sMmePacketSend(mmtype_t eType, bool bPlcNodeFlag, struct pev_item *pPev, mme_ctx_t *pCtx);










#endif 

















