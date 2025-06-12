/******************** (C) COPYRIGHT 2015 EN-plus ************************************
* 文件名：
* 作者：              zzl
* 版本号：            v1.1
* 日期：              2024-03-20
* 描述：              j1939驱动
*
* 公司网址：          www.en-plus.com.cn
* 修改日期及描述：
*************************************************************************************/

#ifndef J1939_DRV_H_
#define J1939_DRV_H_

#include "en_log.h"
#include "en_common.h"
#include "can_drv_trans.h"


#define MULT_PKG_LEN    1785

#define COMM_PORT_EVCC01                cSdkEvcc01CanPort
#define COMM_PORT_EVCC02                cSdkEvcc02CanPort



//多包传输协议
typedef struct
{
    u8                                  ctl_byte;                               //控制字节
    u16                                 msg_lenth;                              //消息总长度
    u8                                  pkg_cnt;                                //包数
    u8                                  not_use;                                //保留 0xff
    u8                                  pgn[3];                                 //PGN
}__attribute__((packed)) st_tp_cm_rts_t;




//管理多包接收
typedef struct
{
    u16                                 len;                                    //消息总数据长度
    u16                                 received_len;                           //实际收到的长度
    u8                                  pkg_cnt;                                //分包个数
    u8                                  recv_pkg_id;                            //下一个希望收到的id
    u8                                  pkg_rx_en;                              //是否接收分包 0 不接收 1接收
    u8                                  pgn[3];                                 //pgn
    u8                                  *buf;                                   //数据缓存区
}st_mult_pkg_t;



u8 more_pkg_tranc_0xeb_proc(u8 id, u8 *data);
/*static*/ void more_pkg_tranc_0xec_proc(u8 id, u8 *data);
#endif
