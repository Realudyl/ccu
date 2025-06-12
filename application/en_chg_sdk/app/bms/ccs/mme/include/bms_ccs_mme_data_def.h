/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_data_def.h
* Description                           :   MME协议驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   
****************************************************************************************************/
#ifndef _bms_ccs_mme_data_def_h
#define _bms_ccs_mme_data_def_h






#define MMTYPE_BASE(mmtype)             ((mmtype) & 0xFFFC)

#define CC_MIN                          0x0000
#define CC_MAX                          0x1fff
#define CP_MIN                          0x2000
#define CP_MAX                          0x3fff
#define NN_MIN                          0x4000
#define NN_MAX                          0x5fff
#define CM_MIN                          0x6000
#define CM_MAX                          0x7fff
#define DRV_MIN                         0xb000
#define DRV_MAX                         0xbfff
#define VS_MIN                          0xa000
#define VS_MAX                          0xafff
#define MS_MIN                          0x8000
#define MS_MAX                          0x801c
#define IMAC_MIN                        0xa800
#define IMAC_MAX                        0xa804


#define MMTYPE_IS_CC(mmtype)            ((mmtype) >= CC_MIN   && (mmtype) <= CC_MAX)
#define MMTYPE_IS_CP(mmtype)            ((mmtype) >= CP_MIN   && (mmtype) <= CP_MAX)
#define MMTYPE_IS_NN(mmtype)            ((mmtype) >= NN_MIN   && (mmtype) <= NN_MAX)
#define MMTYPE_IS_CM(mmtype)            ((mmtype) >= CM_MIN   && (mmtype) <= CM_MAX)
#define MMTYPE_IS_DRV(mmtype)           ((mmtype) >= DRV_MIN  && (mmtype) <= DRV_MAX)
#define MMTYPE_IS_VS(mmtype)            ((mmtype) >= VS_MIN   && (mmtype) <= VS_MAX)
#define MMTYPE_IS_MS(mmtype)            ((mmtype) >= MS_MIN   && (mmtype) <= MS_MAX)
#define MMTYPE_IS_IMAC(mmtype)          ((mmtype) >= IMAC_MIN && (mmtype) <= IMAC_MAX)


#define CC_CCO_APPOINT                  0x0000
#define CC_LINK_INFO                    0x0008
#define CC_DISCOVER_LIST                0x0014
#define CC_WHO_RU                       0x002C
#define CM_UNASSOCIATED_STA             0x6000
#define CM_ENCRYPTED_PAYLOAD            0x6004
#define CM_SET_KEY                      0x6008
#define CM_GET_KEY                      0x600C
#define CM_AMP_MAP                      0x601C
#define CM_BRG_INFO                     0x6020


#define CM_CONN_INFO                    0x6030
#define CM_STA_CAP                      0x6034
#define CM_NW_INFO                      0x6038
#define CM_GET_BEACON                   0x603C
#define CM_HFID                         0x6040
#define CM_MME_ERROR                    0x6044
#define CM_MNBC_SOUND                   0x6074
#define CM_ATTEN_PROFILE                0x6084
#define CM_SLAC_PARM                    0x6064
#define CM_SLAC_MATCH                   0x607c
#define CM_START_ATTEN_CHAR             0x6068
#define CM_ATTEN_CHAR                   0x606c

#define MME_CNF                         0x01
#define MME_IND                         0x02
#define MME_REQ                         0x00
#define MME_RSP                         0x03






//CM_ATTEN_PROFILE.ind 报文里 NumGroups 值定义
#define cAttenuationGroup               58                                      //


















































/**
 * status of MME context for initialization
 */
typedef enum
{
    MME_STATUS_INIT = 0,
    MME_STATUS_OK = 0xabcdefab,
} mme_status_t;



/** Type of OUI */
typedef enum
{
    MME_OUI_NOT_PRESENT = 0,
    MME_OUI_MSTAR,
    MME_OUI_NOT_MSTAR
} mme_oui_type_t;


/**
 * Context structure of an MME message
 * \struct mme_ctx_t
 */
typedef struct
{
    /** context current status */
    mme_status_t status;
    /** management message type */
    unsigned short mmtype;
    /** buffer  where to store MME data */
    unsigned char *buffer;
    /** total length of buffer */
    unsigned int length;
    /** start of MME payload */
    unsigned int head;
    /** end of MME payload */
    unsigned int tail;
    /** OUI retrieved from MME */
    mme_oui_type_t oui;
} mme_ctx_t;








/**
 * list of errors returned by libmme functions
 */
typedef enum
{
    MME_SUCCESS = 0,
    /** context not initialized */
    MME_ERROR_NOT_INIT = -1,
    /** not enough available space */
    MME_ERROR_SPACE = -2,
    /** not enough available data */
    MME_ERROR_ENOUGH = -3,
    /** timeout on waiting on response */
    MME_ERROR_TIMEOUT = -4,
    /** socket send/receive error */
    MME_ERROR_TXRX = -5,
    /** general error */
    MME_ERROR_GEN = -6,
    /** not expected result */
    MME_ERROR_RESULT = -7
} mme_error_t;





















#endif 












