/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_frame_def.h
* Description                           :   GBT27930 协议所有帧结构定义
* Version                               :   
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-17
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_frame_def_h_
#define _bms_gbt_15_frame_def_h_

#include "bms_gbt_15_data_def.h"














//------------------------------------------------------------------------------------
//--------------------------------------充电机SECC数据帧------------------------------
//------------------------------------------------------------------------------------

//当前发送帧类型
typedef enum
{
    eSendFrameTypeIdle                  = 0,
    eSendFrameTypeChm                   = 1,
    eSendFrameTypeCrm                   = 2,
    eSendFrameTypeCtsAndCml             = 3,
    eSendFrameTypeCro                   = 4,
    eSendFrameTypeCcs                   = 5,
    eSendFrameTypeCst                   = 6,
    eSendFrameTypeCsd                   = 7,
    eSendFrameTypeCem                   = 8,
    eSendFrameTypeMax                   = 9,
}__attribute__((packed)) eBmsGbt15SendFrameType;











//充电机握手报文 3byte
typedef struct
{
    u8                                  u8ProtVer[cGbtChgProtVerLen];           //充电机通讯协议版本
}__attribute__((packed)) stBmsGbt15Chm_t;




//充电机辨识报文 8byte
typedef struct
{
    u8                                  u8RecogResult;                          //辨识结果 0x00:不能辨识 0xAA:辨识成功
    u8                                  u8SeccNum[cSeccNumLen];                 //充电机编码
    u8                                  u8AreaCode[cAreaCodeLen];               //充电机地区编码
}__attribute__((packed)) stBmsGbt15Crm_t;




//时间同步信息 7byte
typedef struct
{
    u8                                  u8SeccTime[cSeccTimeLen];               //时间:秒 分 时 日 月 年L 年H
}__attribute__((packed)) stBmsGbt15Cts_t;




//充电桩最大输出能力 8byte
typedef struct
{
    u16                                 u16MaxOutVol;                           //桩最大输出电压，单位0.1
    u16                                 u16MinOutVol;                           //桩最小输出电压，单位0.1
    u16                                 u16MaxOutCur;                           //桩最大输出电流，单位0.1
    u16                                 u16MinOutCur;                           //桩最小输出电流，单位0.1
}stBmsGbt15Cml_t;





//桩输出准备就绪状态 1byte
typedef struct
{
    eBmsGbt15ChgReadyStatus_t           eReady;                                 //桩是否准备好，0x00-准备未完成 0xAA-准备完成
}__attribute__((packed)) stBmsGbt15Cro_t;





//充电状态 7byte
typedef struct
{
    u16                                 u16OutVol;                              //当前电压输出值，单位0.1
    u16                                 u16OutCur;                              //当前电流输出值，单位0.1
    u16                                 u16ChgTime;                             //累计充电时间，单位：min
    stBmsGbt15CcsCode_t                 stAllowChg;                             //是否暂停充电，0-暂停 1-允许
}__attribute__((packed)) stBmsGbt15Ccs_t;





//中止充电 4byte
typedef struct
{
    unBmsGbt15SeccStopReason_t          unStopReason;                           //桩中止充电原因
    unBmsGbt15SeccStopChgFault_t        unStopFault;                            //桩中止充电故障
    unBmsGbt15SeccStopChgErr_t          unStopErr;                              //桩中止充电错误
}__attribute__((packed)) stBmsGbt15Cst_t;





//统计数据 8byte
typedef struct
{
    u16                                 u16ChgTime;                             //累计充电时间，单位：min
    u16                                 u16ChgKwh;                              //累计充电电量，单位:0.1kw.h
    u8                                  u8SeccNum[cSeccNumLen];                 //充电机编号
}__attribute__((packed)) stBmsGbt15Csd_t;






//错误报文 4byte
typedef struct
{
    unBmsGbt15CemCode01                 unCode01;
    unBmsGbt15CemCode02                 unCode02;
    unBmsGbt15CemCode03                 unCode03;
    unBmsGbt15CemCode04                 unCode04;
}__attribute__((packed)) stBmsGbt15Cem_t;





//secc数据
typedef struct
{
    stBmsGbt15Chm_t                     stChmData;                              //PGN=0x26 充电机握手
    stBmsGbt15Crm_t                     stCrmData;                              //PGN=0x01 充电机辨识
    stBmsGbt15Cts_t                     stCtsData;                              //PGN=0x07 时间同步信息
    stBmsGbt15Cml_t                     stCmlData;                              //PGN=0x08 充电机最大输出能力
    stBmsGbt15Cro_t                     stCroData;                              //PGN=0x0A 输出准备就绪
    stBmsGbt15Ccs_t                     stCcsData;                              //PGN=0x12 充电数据
    stBmsGbt15Cst_t                     stCstData;                              //PGN=0x1A 中止充电
    stBmsGbt15Csd_t                     stCsdData;                              //PGN=0x1D 统计数据
    stBmsGbt15Cem_t                     stCemData;                              //PGN=0x1F 错误报文
    
    eBmsGbt15SendFrameType              eSendFrameType;                         //发送标识
}stBmsGbt15Secc_t;



















//----------------------------------------------------------------------------------
//--------------------------------------车端EVCC数据帧------------------------------
//----------------------------------------------------------------------------------

//是否已经收到了bms的数据帧
typedef struct
{
    u16                                 bBhm                : 1;                //收到BHM
    u16                                 bBrm                : 1;                //收到BRM
    u16                                 bBcp                : 1;                //收到BCP
    u16                                 bBro                : 1;                //收到BRO
    u16                                 bBcl                : 1;                //收到BCL
    u16                                 bBcs                : 1;                //收到BCS
    u16                                 bBsm                : 1;                //收到BSM
    u16                                 bBst                : 1;                //收到BST
    u16                                 bBsd                : 1;                //收到BSD
    u16                                 bBem                : 1;                //收到BEM
    u16                                 bStandby            : 6;                //预留
    
}__attribute__((packed)) stBmsGbt15RecvFlag_t;


typedef union
{
    stBmsGbt15RecvFlag_t                stRecvFlag;
    u16                                 u16RecvFlag;
}__attribute__((packed)) unBmsGbt15RecvFlag_t;






//bms报文超时标识
typedef struct
{
    u16                                 bBhmTimeout         : 1;                //BHM接收超时
    u16                                 bBrmTimeout         : 1;                //BRM接收超时
    u16                                 bBcpTimeout         : 1;                //BCP接收超时
    u16                                 bBroTimeout         : 1;                //BRO接收超时
    u16                                 bBclTimeout         : 1;                //BCL接收超时
    u16                                 bBcsTimeout         : 1;                //BCS接收超时
    u16                                 bBsmTimeout         : 1;                //BSM接收超时
    u16                                 bBstTimeout         : 1;                //BST接收超时
    u16                                 bBsdTimeout         : 1;                //BSD接收超时
    u16                                 bBemTimeout         : 1;                //BEM接收超时
    u16                                 bStandby            : 6;                //预留
    
}__attribute__((packed)) stBmsGbt15RecvTimeout;;



typedef union
{
    stBmsGbt15RecvTimeout               stRxTimeout;
    u16                                 u16RxTimeout;
}__attribute__((packed)) unBmsGbt15RecvTimeout;







//车辆握手 2byte
typedef struct
{
    u16                                 u16MaxPermitChgVol;                     //最高允许充电电压，单位：0.1V
}__attribute__((packed)) stBmsGbt15Bhm_t;





//辨识报文 49byte
typedef struct
{
    u8                                  u8Protver[cGbtBmsProtVerLen];           //bms通讯协议版本
    eBmsGbt15BatType_t                  eBatType;                               //电池类型
    u16                                 u16BatRateCap;                          //电池额定容量，单位0.1Ah
    u16                                 u16BatRateVol;                          //电池额定总电压，单位0.1V
    u8                                  u8BatFactName[cFactNameLen];            //电池生产商名称
    u8                                  u8BatSerial[cBatSerialLen];             //电池组序列号
    u8                                  u8BatYear;                              //电池组生产日期：年 1985~2235
    u8                                  u8BatMonth;                             //电池组生产日期：月
    u8                                  u8BatDay;                               //电池组生产日期：日
    u32                                 u32BatChgCount;                         //电池组充电次数
    eBmsGbt15BatTitleMark_t             eBatProt;                               //电池产权标识
    u8                                  u8Res;                                  //预留
    u8                                  u8Vin[cVinCodeLen];                     //vin码
    u8                                  u8BmsSoftVer[cBmsSoftVerLen];           //bms软件版本号
}__attribute__((packed)) stBmsGbt15Brm_t;






//蓄电池充电参数 13byte
typedef struct
{
    u16                                 u16CellMaxVol;                          //单体电池最高允许充电电压，单位0.01V
    u16                                 u16AllowMaxCur;                         //最高允许充电电流，单位0.1A
    u16                                 u16BatteryCap;                          //电池组标称总能量，单位0.1Kw.h
    u16                                 u16AllowMaxVol;                         //最高允许充电总电压，单位0.1V
    u8                                  u8AllowMaxTemp;                         //最高允许温度
    u16                                 u16BatterySoc;                          //整车电池电荷状态，单位0.1%
    u16                                 u16CurBatVol;                           //当前电池组的电压，单位0.1V
    
}__attribute__((packed)) stBmsGbt15Bcp_t;





//车辆准备就绪 1byte
typedef struct
{
    bool                                bBroAAFlag;                             //已经收到了BroAA报文
    eBmsGbt15ChgReadyStatus_t           eReady;                                 //bms是否准备好充电
}__attribute__((packed)) stBmsGbt15Bro_t;






//电池充电需求 5byte
typedef struct
{
    u16                                 u16ReqVol;                              //需求电压，单位0.1V
    u16                                 u16ReqCur;                              //需求电流，单位0.1A
    u8                                  u8ChgMode;                              //充电模式，0x01:恒压 0x02:恒流
}__attribute__((packed)) stBmsGbt15Bcl_t;





//车辆总状态 9byte
typedef struct
{
    u16                                 u16MeasureVol;                          //充电电压测量值，单位0.1V
    u16                                 u16MeasureCur;                          //充电电流测量值，单位0.1A
    stBmsGbt15CellInfo_t                stMaxCellInfo;                          //最高单体电池电压及其组号
    u8                                  u8CurSoc;                               //当前车端soc
    u16                                 u16RemainChgTime;                       //充电剩余时间
}__attribute__((packed)) stBmsGbt15Bcs_t;






//蓄电池状态信息
typedef struct
{
    u8                                  u8CellNum;                              //最高单体电池电压所在编号
    u8                                  u8CellMaxTemp;                          //最高电池温度
    u8                                  u8MaxTempIdx;                           //最高温度检测点编号
    u8                                  u8CellMinTemp;                          //最低电池温度
    u8                                  u8MinTempIdx;                           //最低电池检测点编号
    
    stBmsGbt15BatFaultCode_t            stBatFaultCode;                         //电池故障代码
}__attribute__((packed)) stBmsGbt15Bsm_t;






//单体蓄电池电压
typedef struct
{
    u16                                 u16CellVol[cBattUnitVoltNum];           //单体电池电压，单位0.01V
    u8                                  u8CellGrp[cBattUnitVoltNum];            //电池组号
}__attribute__((packed)) stBmsGbt15Bmv_t;






//单体蓄电池温度
typedef struct
{
    u8                                  u8CellTemp[cBattUnitTempNum];           //单体电池温度
}__attribute__((packed)) stBmsGbt15Bmt_t;





//电池预留字段
typedef struct
{
    u8                                  u8CellRes[cBatteryResNum];              //单体电池预留字段
}__attribute__((packed)) stBmsGbt15Bsp_t;





//bms中止数据
typedef struct
{
    unBmsGbt15BmsStopChgReason_t        unStopReason;                           //bms停止充电原因
    unBmsGbt15BmsStopChgFault_t         unStopFault;                            //bms停止充电故障
    unBmsGbt15BmsStopChgErr_t           unStopErr;                              //bms停止充电错误
}__attribute__((packed)) stBmsGbt15Bst_t;





//bms统计数据
typedef struct
{
    u8                                  u8StopSoc;                              //中止电荷状态
    u16                                 u16MinCellVol;                          //最低单体电池电压，单位0.01V
    u16                                 u16MaxCellVol;                          //最高单体电池电压，单位0.01V
    u8                                  u8MinBatTemp;                           //最低电池温度
    u8                                  u8MaxBatTemp;                           //最高电池温度
}__attribute__((packed)) stBmsGbt15Bsd_t;






//bms错误报文
typedef struct
{
    unBmsGbt15BemCode01                 unCode01;
    unBmsGbt15BemCode02                 unCode02;
    unBmsGbt15BemCode03                 unCode03;
    unBmsGbt15BemCode04                 unCode04;
}__attribute__((packed)) stBmsGbt15Bem_t;






//evcc数据
typedef struct
{
    stBmsGbt15Bhm_t                     stBhmData;                              //车辆握手
    stBmsGbt15Brm_t                     stBrmData;                              //辨识报文
    stBmsGbt15Bcp_t                     stBcpData;                              //蓄电池充电参数
    stBmsGbt15Bro_t                     stBrodata;                              //车辆准备就绪
    stBmsGbt15Bcl_t                     stBclData;                              //电池充电需求
    stBmsGbt15Bcs_t                     stBcsData;                              //车辆总状态
    stBmsGbt15Bsm_t                     stBsmData;                              //蓄电池状态信息
    stBmsGbt15Bmv_t                     stBmvData;                              //单体蓄电池电压
    stBmsGbt15Bmt_t                     stBmtData;                              //蓄电池温度
    stBmsGbt15Bsp_t                     stBspData;                              //蓄电池预留报文
    stBmsGbt15Bst_t                     stBstData;                              //中止数据
    stBmsGbt15Bsd_t                     stBsdData;                              //统计数据
    stBmsGbt15Bem_t                     stBemData;                              //错误报文
    
    unBmsGbt15RecvFlag_t                unRecvFlag;                             //接收标识
    unBmsGbt15RecvTimeout               unRecvTimeout;                          //车端报文超时
}stBmsGbt15Evcc_t;



#endif

