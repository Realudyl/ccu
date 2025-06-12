/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_drv_mt626.h
* Description                           :   铭特MT626刷卡器驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :   
****************************************************************************************************/
#ifndef _card_drv_mt626_h_
#define _card_drv_mt626_h_
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "usart_drv_trans.h"








//读卡器串口
#ifndef cSdkCardComUartNum
#define cCardComUartNum                 (eUsartNum1)
#else
#define cCardComUartNum                 (cSdkCardComUartNum)
#endif


//读卡器串口波特率
#ifndef cSdkCardComBaudrate
#define cCardComBaudrate                (115200)
#else
#define cCardComBaudrate                (cSdkCardComBaudrate)
#endif









//刷卡器收发缓存区大小
#define cCardDrvMt626BufSize            (64)





//--------------------------------------协议实现--------------------------------

//通用长度定义
#define cCardDrvMt626LenKey             (6)                                     //密码
#define cCardDrvMt626LenBlock           (16)                                    //block数据长度






//铭特MT626刷卡器协议 起始字节 定义
typedef enum
{
    eCardDrvMt626Stx                    = 0x02,                                 //起始字节
    
    eCardDrvMt626StxMax
}__attribute__((packed)) eCardDrvMt626Stx_t;





//铭特MT626刷卡器协议 命令字 定义
typedef enum
{
    eCardDrvMt626CmdSys                 = 0x31,                                 //系统操作
    eCardDrvMt626CmdCpuA_m1             = 0x34,                                 //TYPE A CPU卡 & 射频M1卡 操作
    eCardDrvMt626CmdCpuB                = 0x35,                                 //TYPE B CPU卡 操作
    eCardDrvMt626CmdSim                 = 0x3D,                                 //SIM卡操作
    
    eCardDrvMt626CmdMax
}__attribute__((packed)) eCardDrvMt626Cmd_t;



//铭特MT626刷卡器协议 命令参数 定义
typedef enum
{
    //系统操作命令参数
    eCardDrvMt626CmdParaBuzz            = 0x3E,                                 //蜂鸣器控制
    eCardDrvMt626CmdParaGetVer          = 0x40,                                 //查询卡机版本
    eCardDrvMt626CmdParaSoftRst         = 0x55,                                 //读卡器软复位
    
    //TYPE A/B CPU卡操作命令参数
    eCardDrvMt626CmdParaActive          = 0x40,                                 //激活射频卡
    eCardDrvMt626CmdParaApdu            = 0x41,                                 //APDU应用命令
    eCardDrvMt626CmdParaDeSelect        = 0x42,                                 //DESELECT命令
    
    //射频M1卡操作命令参数
    eCardDrvMt626CmdParaSearch          = 0x30,                                 //寻射频卡
    eCardDrvMt626CmdParaMefare1         = 0x31,                                 //获取Mefare1 卡序列号
    eCardDrvMt626CmdParaKeyA            = 0x32,                                 //验证Key_A密码
    eCardDrvMt626CmdRead                = 0x33,                                 //读扇区块数据
    eCardDrvMt626CmdWrite               = 0x34,                                 //写扇区块数据
    eCardDrvMt626CmdChangeKey           = 0x35,                                 //更改密码
    eCardDrvMt626CmdValueAdd            = 0x37,                                 //增值操作
    eCardDrvMt626CmdValueSub            = 0x37,                                 //减值操作
    eCardDrvMt626CmdParaKeyB            = 0x39,                                 //验证Key_B密码
    
    //SIM卡操作命令参数
    eCardDrvMt626CmdParaReset3          = 0x40,                                 //3.0 V的SIM卡进行复位操作
    eCardDrvMt626CmdParaReset5          = 0x41,                                 //5.0 V的SIM卡进行复位操作
    eCardDrvMt626CmdParaPowerOff        = 0x42,                                 //SIM卡下电操作
    eCardDrvMt626CmdParaApdu_C          = 0x43,                                 //SIM卡C-APDU命令操作
    
    
    eCardDrvMt626CmdParaMax             = 0xFF
}__attribute__((packed)) eCardDrvMt626CmdPara_t;







//铭特MT626刷卡器协议 帧头结构 定义
typedef struct
{
    eCardDrvMt626Stx_t                  eStx;                                   //起始字节
    u16                                 u16Len;                                 //长度
}__attribute__((packed)) stCardDrvMt626Head_t;
#define cCardDrvMt626HeadSize           sizeof(stCardDrvMt626Head_t)            //head长度






//铭特MT626刷卡器协议 结束字节 定义
typedef enum
{
    eCardDrvMt626Etx                    = 0x03,                                 //结束字节
    
    eCardDrvMt626EtxMax
}__attribute__((packed)) eCardDrvMt626Etx_t;



//铭特MT626刷卡器协议 帧尾结构 定义
typedef struct
{
    eCardDrvMt626Etx_t                  eEtx;                                   //结束字节
    u8                                  u8Bcc;                                  //BCC数据异或校验
}__attribute__((packed)) stCardDrvMt626Tail_t;
#define cCardDrvMt626TailSize           sizeof(stCardDrvMt626Tail_t)            //Tail长度









//铭特MT626刷卡器协议 操作状态字 定义
typedef enum
{
    eCardDrvMt626OpRstErrSector         = '1',                                  //操作扇区号错（不是验证密码后的扇区）
    eCardDrvMt626OpRstErrKey            = '3',                                  //密码验证错误
    eCardDrvMt626OpRstErrData           = '4',                                  //数据错
    eCardDrvMt626OpRstFail              = 'N',                                  //操作失败
    eCardDrvMt626OpRstSuccess           = 'Y',                                  //操作成功
    
    
    eCardDrvMt626OpRstMax
}__attribute__((packed)) eCardDrvMt626OpRst_t;










//--------------------------------------系统操作命令payload结构-----------------
//基础控制命令---无数据包字段
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
}__attribute__((packed)) stCardDrvMt626CmdBasic_t;

//基础应答命令---无数据包字段
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
}__attribute__((packed)) stCardDrvMt626CmdBasicAck_t;




//获取Mefare1 卡序列号应答
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
    u32                                 u32Mefare1Sn;                           //Mefare1 卡序列号
}__attribute__((packed)) stCardDrvMt626CmdMefare1Ack_t;




//验证Key_A/B密码命令
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Key[cCardDrvMt626LenKey];             //密码
}__attribute__((packed)) stCardDrvMt626CmdKey_t;

//验证Key_A/B密码命令应答
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
}__attribute__((packed)) stCardDrvMt626CmdKeyAck_t;




//读扇区块数据命令
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Block;                                //块号
}__attribute__((packed)) stCardDrvMt626CmdRead_t;

//读扇区块数据命令应答---失败时
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Block;                                //块号
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
}__attribute__((packed)) stCardDrvMt626CmdReadAckFail_t;

//读扇区块数据命令应答---成功时 eOp = 'Y'
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Block;                                //块号
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
    u8                                  u8Data[cCardDrvMt626LenBlock];          //数据-----------eOp = 'Y'
}__attribute__((packed)) stCardDrvMt626CmdReadAckSuccess_t;




//写扇区块数据命令
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Block;                                //块号
    u8                                  u8Data[cCardDrvMt626LenBlock];          //数据-----------eOp = 'Y'
}__attribute__((packed)) stCardDrvMt626CmdWrite_t;

//写扇区块数据命令应答
typedef struct
{
    eCardDrvMt626Cmd_t                  eCmd;                                   //命令字
    eCardDrvMt626CmdPara_t              ePara;                                  //命令参数
    u8                                  u8Sector;                               //扇区号
    u8                                  u8Block;                                //块号
    eCardDrvMt626OpRst_t                eOp;                                    //操作状态字
}__attribute__((packed)) stCardDrvMt626CmdWriteAck_t;












//铭特MT626刷卡器协议 payload 定义
typedef union
{
    stCardDrvMt626CmdBasic_t            stSearch;                               //寻卡命令
    stCardDrvMt626CmdBasicAck_t         stSearchAck;                            //寻卡应答
    
    stCardDrvMt626CmdBasic_t            stMefare1;                              //获取Mefare1 卡序列号
    stCardDrvMt626CmdMefare1Ack_t       stMefare1Ack;                           //获取Mefare1 卡序列号应答
    
    stCardDrvMt626CmdKey_t              stKey;                                  //验证Key_A/B密码
    stCardDrvMt626CmdKeyAck_t           stKeyAck;                               //验证Key_A/B密码应答
    
    stCardDrvMt626CmdRead_t             stRead;                                 //读扇区块数据
    stCardDrvMt626CmdReadAckFail_t      stReadFail;                             //读扇区块数据应答---失败
    stCardDrvMt626CmdReadAckSuccess_t   stReadSuccess;                          //读扇区块数据应答---成功
    
    stCardDrvMt626CmdWrite_t            stWrite;                                //写扇区块数据
    stCardDrvMt626CmdWriteAck_t         stWriteAck;                             //写扇区块数据应答
    
    
    u8                                  u8Buf[cCardDrvMt626BufSize - cCardDrvMt626HeadSize - cCardDrvMt626TailSize];
}__attribute__((packed)) unCardDrvMt626Payload_t;






//铭特MT626刷卡器协议 整个帧结构定义
typedef struct
{
    stCardDrvMt626Head_t                stHead;
    unCardDrvMt626Payload_t             unPayload;
}__attribute__((packed)) stCardDrvMt626Pkt_t;

//铭特MT626刷卡器协议 整个帧结构定义---union 形式
typedef union 
{
    stCardDrvMt626Pkt_t                 stPkt;
    u8                                  u8Buf[cCardDrvMt626BufSize];
}__attribute__((packed)) unCardDrvMt626Pkt_t;























//铭特MT626刷卡器驱动 缓存结构定义
typedef struct
{
    //收发资源
    u8                                  u8Buf[cCardDrvMt626BufSize];            //接收buf
    unCardDrvMt626Pkt_t                 unTxPkt;                                //发送buf
    SemaphoreHandle_t                   xSemTxPkt;
    
    //断帧资源
    i32                                 i32RxIndex;
    unCardDrvMt626Pkt_t                 unRxPkt;
    unCardDrvMt626Pkt_t                 unRxPktTemp;
    
    //命令应答信号量
    SemaphoreHandle_t                   xSemAck;

    //读卡器在线标志
    bool                                bOnlineStatus;
}stCardDrvMt626Cache_t;






















extern bool sCardDrvMt626Init(void);


//M1卡操作命令---目前只用到以下几个命令
extern bool sCardDrvMt626M1Search(void);
extern bool sCardDrvMt626M1Mefare1(u32 *pSn);
extern bool sCardDrvMt626M1Key(eCardDrvMt626CmdPara_t ePara, u8 u8Sector, i32 i32Len, const u8 *pKey);
extern bool sCardDrvMt626M1Read(u8 u8Sector, u8 u8Block, i32 i32MaxLen, u8 *pBuf);
extern bool sCardDrvMt626M1Write(u8 u8Sector, u8 u8Block, i32 i32Len, const u8 *pData);


extern bool sCardDrvMt626GetOnLineState(void);






#endif




















