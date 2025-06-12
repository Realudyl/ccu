/******************************************************************************
 Copyright  Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.

 ******************************************************************************
 * @file j1939.c
 * @version V1.0
 * @author zzl
 * @date 2024-03-05
 * @attention j1939多包传输
 * @brief freertos系统

 * @History:
 * 1.@Date: 2024-03-05
 *   @Author: zzl 
 *   @Modify: 创建文件,j1939多包传输

******************************************************************************/
#include "j1939_drv.h"


//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "j1939_drv";

//多包传输
static void tp_send_cts(u8 gun_id, u8 pkg_cnt, u8 id, u8 *pgn);
static void tp_send_endof_msg_ack(u8 id, u16 len,u8 pkg_cnt, u8 *pgn);
//分包管理  当数据大于8字节时会用分包传输协议

u8 rx_pkg_buf[ePileGunIndexNum][MULT_PKG_LEN];            //多包数据

st_mult_pkg_t mult_pkg_rx_info[] =
{
    {
        0,
        0,
        0,
        0,
        0,
        {0, 0, 0},
        rx_pkg_buf[0],
    },
    
    {
        0,
        0,
        0,
        0,
        0,
        {0, 0, 0},
        rx_pkg_buf[1],
    }
};



/*
 * @brief  当收到BMS请求发送多包数据时用于回应BMS 表示以准备好接收数据
 * @param  
 * @param  
 * @param  
 * @return 
 */
static void tp_send_cts(u8 gun_id, u8 pkg_cnt, u8 id, u8 *pgn)
{
    stCanMsg_t tx_msg;
    
    tx_msg.u32Id = 0x9CECF456;
    tx_msg.u8Len = 8;
    
    tx_msg.u8Data[0] = 0x11;            //控制字节
    tx_msg.u8Data[1] = pkg_cnt;         //可发送的数据包数目
    tx_msg.u8Data[2] = id;              //下一个将要发送的数据包编号
    
    tx_msg.u8Data[3] = 0xff;
    tx_msg.u8Data[4] = 0xff;
    
    tx_msg.u8Data[5] = pgn[0];
    tx_msg.u8Data[6] = pgn[1];
    tx_msg.u8Data[7] = pgn[2];
    
    //入发送队列
    if(gun_id == 0)
    {
        sCanDrvSend(COMM_PORT_EVCC01,&tx_msg);
    }
    else if(gun_id == 1)
    {
        sCanDrvSend(COMM_PORT_EVCC02,&tx_msg);
    }
    
}



/*
 * @brief  通知发送者 多包数据接收完成
 * @param  
 * @param  
 * @param  
 * @return 
 */
static void tp_send_endof_msg_ack(u8 id, u16 len,u8 pkg_cnt, u8 *pgn)
{
    stCanMsg_t tx_msg;
    
    tx_msg.u32Id = 0x9CECF456;
    tx_msg.u8Len     = 8;
    
    tx_msg.u8Data[0] = 0x13;            //控制字节
    tx_msg.u8Data[1] = len;             //总数据长度
    tx_msg.u8Data[2] = (u8)(len >> 8);
    
    tx_msg.u8Data[3] = pkg_cnt;         //数据数
    tx_msg.u8Data[4] = 0xff;
    
    tx_msg.u8Data[5] = pgn[0];
    tx_msg.u8Data[6] = pgn[1];
    tx_msg.u8Data[7] = pgn[2];
    
    //入发送队列
    if(id == 0)
    {
        sCanDrvSend(COMM_PORT_EVCC01,&tx_msg);
    }
    else if(id == 1)
    {
        sCanDrvSend(COMM_PORT_EVCC02,&tx_msg);
    }
    
}



/*
 * @brief  多包传输0x10请求响应处理
 * @param  pf：参数组编号
 * @param  
 * @param  
 * @return u32 id.
 */
void pkg_tranc_0x10(u8 id, st_tp_cm_rts_t *prts)
{
    mult_pkg_rx_info[id].len                = prts->msg_lenth;
    mult_pkg_rx_info[id].pkg_cnt            = prts->pkg_cnt;
    mult_pkg_rx_info[id].received_len       = 0;
    mult_pkg_rx_info[id].recv_pkg_id        = 1;  //分包id从1开始
    mult_pkg_rx_info[id].pgn[0]             = prts->pgn[0];
    mult_pkg_rx_info[id].pgn[1]             = prts->pgn[1];
    mult_pkg_rx_info[id].pgn[2]             = prts->pgn[2];
    mult_pkg_rx_info[id].pkg_rx_en          = 0;
    
    if(mult_pkg_rx_info[id].len <= MULT_PKG_LEN)  //有足够的存储空间
    {
        //接收id 默认1
        tp_send_cts(id, prts->pkg_cnt, 1, prts->pgn);  //准备接收BMS多包数据
        
        mult_pkg_rx_info[id].pkg_rx_en      = 1;  //使能接收
    }
}



/*
 * @brief  多包传输0x10请求响应处理
 * @param  pf：参数组编号
 * @param  
 * @param  
 * @return u32 id.
 */
void pkg_tranc_Abort(u8 gun_id, u8 *pgn)
{
    stCanMsg_t tx_msg;
    
    tx_msg.u32Id = 0x9CECF456;
    tx_msg.u8Len     = 8;
    
    tx_msg.u8Data[0] = 0xFF;        //放弃连接
    tx_msg.u8Data[1] = 0xFF;
    tx_msg.u8Data[2] = 0xFF;
    tx_msg.u8Data[3] = 0xFF;
    tx_msg.u8Data[4] = 0xff;
    
    tx_msg.u8Data[5] = pgn[0];
    tx_msg.u8Data[6] = pgn[1];
    tx_msg.u8Data[7] = pgn[2];
    
    //入发送队列
    if(gun_id == 0)
    {
        sCanDrvSend(COMM_PORT_EVCC01,&tx_msg);
    }
    else if(gun_id == 1)
    {
        sCanDrvSend(COMM_PORT_EVCC02,&tx_msg);
    }
}




/*
 * @brief  多包传输处理
 * @param  pf：参数组编号
 * @param  
 * @param  
 * @return u32 id.
 */
/*static*/ void more_pkg_tranc_0xec_proc(u8 id, u8 *data)
{
    u32 temp_pgn = 0;
    st_tp_cm_rts_t *rts = NULL;
    switch((u8)data[0])
    {
        case 0x10:  //请求发送
            rts = (st_tp_cm_rts_t*)&data[0];
            temp_pgn  = rts->pgn[0] << 16;
            temp_pgn |= rts->pgn[1] << 8;
            temp_pgn |= rts->pgn[2];
            
            //PGN过滤
            if((temp_pgn == 0x000200) || (temp_pgn == 0x000600) || (temp_pgn == 0x001100)
            || (temp_pgn == 0x001500) || (temp_pgn == 0x001600) || (temp_pgn == 0x001700))
            {
                pkg_tranc_0x10(id, rts);
            }
            //else if((temp_pgn == 0x001500) || (temp_pgn == 0x001600) || (temp_pgn == 0x001700))
            //{
            //    pkg_tranc_Abort(id, rts->pgn);
            //}
        break;
        case 0xFF:  //放弃连接
            mult_pkg_rx_info[id].pkg_rx_en = 0;  //使能接收
        break;
        default:
        break;
    }
}

/*
 * @brief  多包传输处理
 * @param  pf：参数组编号
 * @param  
 * @param  
 * @return 
 */
u8 more_pkg_tranc_0xeb_proc(u8 id, u8 *data)
{
    u8 res = 0;
    
    u16 len = mult_pkg_rx_info[id].len - mult_pkg_rx_info[id].received_len;  //剩余长度

    //检查分包id   若不是期望的id则丢弃
    if(mult_pkg_rx_info[id].recv_pkg_id != data[0])  
    {
        return res;
    }

    //检查分包id合法性
    if(mult_pkg_rx_info[id].recv_pkg_id > mult_pkg_rx_info[id].pkg_cnt)  
    {
        return res;
    }
    if(len > 7)
    {
        memcpy(&mult_pkg_rx_info[id].buf[mult_pkg_rx_info[id].received_len], &data[1], 7);  //第一个字节为ID 后面7字节数据
        mult_pkg_rx_info[id].received_len += 7;
        mult_pkg_rx_info[id].recv_pkg_id++;  //接收下一个分包
        res = 1;
    }
    else  //最后一个包
    {
        memcpy(&mult_pkg_rx_info[id].buf[mult_pkg_rx_info[id].received_len], &data[1], len);
        mult_pkg_rx_info[id].received_len += len;
        
        tp_send_endof_msg_ack(id, mult_pkg_rx_info[id].len,mult_pkg_rx_info[id].pkg_cnt,mult_pkg_rx_info[id].pgn);  //多包数据接收完成  通知发送者

        mult_pkg_rx_info[id].pkg_rx_en = 0;  //接收完成
        res = 2;
    }

    return res;
}

