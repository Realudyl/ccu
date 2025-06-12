/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   bms_gbt_15_data_def.h
* Description                           :   GBT27930 协议字段内容定义
* Version                               :   V1.0
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-04-16
* notice                                :   
****************************************************************************************************/
#ifndef _bms_gbt_15_data_def_h_
#define _bms_gbt_15_data_def_h_

#include "en_common.h"




//充电机通信协议版本号长度
#define cGbtChgProtVerLen               3

//bms通信协议版本号长度
#define cGbtBmsProtVerLen               3

//充电机编号长度
#define cSeccNumLen                     4

//充电机区域编码长度
#define cAreaCodeLen                    3

//充电机时间信息长度
#define cSeccTimeLen                    7

//电池生产商名称长度
#define cFactNameLen                    4

//电池组序号长度
#define cBatSerialLen                   4

//电池组生产日期年份的基数
#define cBatYearBase                    (1985)

//vin码长度
#define cVinCodeLen                     17

//bms软件版本号长度
#define cBmsSoftVerLen                  8

//bms Bmv电压检测节数
#define cBattUnitVoltNum                256

//bms Bmt温度检测节数
#define cBattUnitTempNum                128

//bms Bsp电池预留字节数
#define cBatteryResNum                  16










//设备地址
typedef enum
{
    eBmsGbt15SeccAddr                   = 0x56,                                 //桩设备地址
    eBmsGbt15EvccAddr                   = 0xF4,                                 //车设备地址
}__attribute__((packed)) eBmsGbt15DevAddr_t;






//车端是否允许充电
typedef enum
{
    eBmsGbt15ForbidChg                  = 0x00,                                 //禁止充电
    eBmsGbt15AllowChg                   = 0x01,                                 //允许充电
}__attribute__((packed)) eBmsGbt15AllowChg_t;






//故障停止类型
typedef enum
{
    eBmsGbt15StopReasonNone             = 0,                                    //系统正常
    eBmsGbt15StopReasonNormal           = 1,                                    //正常停止
    eBmsGbt15StopReasonDanger           = 2,                                    //故障停止
}__attribute__((packed)) eBmsGbt15StopReasonType_t;





//绝缘检测前枪口电压结果（由用户层提供）
typedef enum
{
    eBmsGbt15PreIsoChecking             = 0,                                    //绝缘检测前检测进行中
    eBmsGbt15PreIsoCheckFail            = 1,                                    //绝缘检测前检测故障
    eBmsGbt15PreIsoCheckNormal          = 2,                                    //绝缘检测前检测正常
}__attribute__((packed)) eBmsGbt15PreIsoResult_t;





//绝缘检测结果（由用户层提供）
typedef enum
{
    eBmsGbt15IsoChecking                = 0,                                    //绝缘检测进行中
    eBmsGbt15IsoCheckFail               = 1,                                    //绝缘检测故障
    eBmsGbt15IsoCheckWarn               = 2,                                    //绝缘检测告警
    eBmsGbt15IsoCheckNormal             = 3,                                    //绝缘检测正常
}__attribute__((packed)) eBmsGbt15IsoResult_t;




//车辆参数检查
typedef enum
{
    eBmsGbt15EvParamChecking            = 0,                                    //车辆参数检查中
    eBmsGbt15EvParamCheckFail           = 1,                                    //车辆参数检查不通过
    eBmsGbt15EvParamCheckNormal         = 2,                                    //车辆参数检查正常
}__attribute__((packed)) eBmsGbt15EvParamCheck_t;





//预充结果，由用户层提供
typedef enum
{
    eBmsGbt15PreChging                  = 0,                                    //预充进行中
    eBmsGbt15PreChgFail                 = 1,                                    //预充失败
    eBmsGbt15PreChgNormal               = 2,                                    //预充成功
}__attribute__((packed)) eBmsGbt15PreChgRst_t;




//充电中CC1状态，由用户层提供
typedef enum
{
    eCc1stateChgOk                      = 0,                                    //重新握手条件准备中（电流值<5A,断开K1K2）
    eCc1stateChgFail                    = 1,                                    //重新握手条件已达成，可以进入重新握手阶段
}__attribute__((packed)) eBmsGbt15ChgCc1State_t;







//重新握手前置条件，由用户层提供
typedef enum
{
    eBmsGbt15ReChgPrepare               = 0,                                    //重新握手条件准备中（电流值<5A,断开K1K2）
    eBmsGbt15ReChgOk                    = 1,                                    //重新握手条件已达成，可以进入重新握手阶段
}__attribute__((packed)) eBmsGbt15ReChgCondition_t;






//状态机报文切换前置条件，暴露给用户层设置
typedef struct
{
    bool                                bReturn;                                //返回待机
    bool                                bAuthorize;                             //用户层授权标志 (辅源闭合后用户层才会设置这个标志)

    eBmsGbt15PreIsoResult_t             ePreIsoCheckResult;                     //绝缘检测前枪外侧电压结果 (低于10V则进行绝缘检测)
    eBmsGbt15IsoResult_t                eCrmIsoCheckResult;                     //绝缘检测结果 (绝缘检测正常或告警，则开始发送Crm报文)
    eBmsGbt15PreChgRst_t                eCroPreChgSuccess;                      //预充结果 (预充成功后，开始发送Cro—AA报文)
    eBmsGbt15ChgCc1State_t              eCc1stateInChg;                         //充电中的CC1状态
    eBmsGbt15ReChgCondition_t           eCemReChgCondition;                     //重新握手充电的前置条件
}__attribute__((packed)) stPacketSwitchCondition_t;











//停止充电原因
typedef enum
{
    eReasonUnKnown                      = 0x00,                                 //未知原因
    eReasonFinished                     = 0x01,                                 //达到条件停止
    eReasonManual                       = 0x04,                                 //人工停止
    eReasonFaulted                      = 0x10,                                 //故障中止
    eReasonBstFrame                     = 0x40,                                 //bms主动停止
}eBmsGbt15StopReason_t;







//停止充电故障原因
typedef enum
{
    eFaultNone                          = 0x00,                                 //无故障
    eFaultSeccOverTemp                  = 0x01,                                 //充电桩过温
    eFaultConnectAbnormal               = 0x04,                                 //连接器异常
    eFaultBoardOverTemp                 = 0x10,                                 //内部温度过高
    eFaultPowerForbid                   = 0x40,                                 //能量不能传输
    eFaultEstopPress                    = 0x100,                                //急停按下
    eFaultOther                         = 0x400,                                //其它故障（27930-2023有扩充）
}eBmsGbt15StopFault_t;







//停止充电错误原因
typedef enum
{
    eErrNone                            = 0x00,                                 //无错误
    eErrCurrNotMatch                    = 0x01,                                 //电流不匹配
    eErrVoltAbnormal                    = 0x04,                                 //电压异常
}eBmsGbt15StopErr_t;













//------------------------------------通讯帧相关---------------------------------------------


//报文类型定义
typedef enum
{
    //握手阶段
    eBmsGbt15PgnChm                     = 0x26,                                 //充电机握手
    eBmsGbt15PgnBhm                     = 0x27,                                 //bms握手
    eBmsGbt15PgnCrm                     = 0x01,                                 //充电机辨识
    eBmsGbt15PgnBrm                     = 0x02,                                 //bms辨识
    
    
    //配置阶段
    eBmsGbt15PgnBcp                     = 0x06,                                 //bms电池充电参数
    eBmsGbt15PgnCts                     = 0x07,                                 //充电机发送时间同步
    eBmsGbt15PgnCml                     = 0x08,                                 //充电机最大输出能力
    eBmsGbt15PgnBro                     = 0x09,                                 //bms准备就绪
    eBmsGbt15PgnCro                     = 0x0A,                                 //充电机准备就绪
    
    
    //充电阶段
    eBmsGbt15PgnBcl                     = 0x10,                                 //bms充电需求
    eBmsGbt15PgnBcs                     = 0x11,                                 //bms电池总状态
    eBmsGbt15PgnCcs                     = 0x12,                                 //充电机充电状态
    eBmsGbt15PgnBsm                     = 0x13,                                 //bms蓄电池状态
    eBmsGbt15PgnBmv                     = 0x15,                                 //bms电池电压信息
    eBmsGbt15PgnBmt                     = 0x16,                                 //bms电池温度信息
    eBmsGbt15PgnBsp                     = 0x17,                                 //bms电池预留报文
    eBmsGbt15PgnBst                     = 0x19,                                 //bms中止充电
    eBmsGbt15PgnCst                     = 0x1A,                                 //充电机中止充电
    
    
    //充电结束
    eBmsGbt15PgnBsd                     = 0x1C,                                 //bms统计数据
    eBmsGbt15PgnCsd                     = 0x1D,                                 //充电机统计数据
    
    
    
    //错误报文
    eBmsGbt15PgnBem                     = 0x1E,                                 //bms错误报文
    eBmsGbt15PgnCem                     = 0x1F,                                 //充电机错误报文
    
    //长帧传输
    eBmsGbt15PgnDt                      = 0xEB,                                 //多包数据标识
    eBmsGbt15PgnCm                      = 0xEC,                                 //长帧传输请求
    
}__attribute__((packed)) eBmsGbt15Pgn_t;







//充电机辨识结果
typedef enum
{
    eBmsGbt15RecogFailed                = 0x00,                                 //不能辨识
    eBmsGbt15RecogSuccess               = 0xAA,                                 //辨识成功
    
}__attribute__((packed)) eBmsGbt15RecogResult_t;





//bms端电池类型
typedef enum
{
    eBmsGbt15BatType_LeadAcid           = 0x01,                                 //1铅酸电池
    eBmsGbt15BatType_NiMH               = 0x02,                                 //2镍氢电池
    eBmsGbt15BatType_LithIrPh           = 0x03,                                 //3磷酸铁锂电池
    eBmsGbt15BatType_LithMaOxide        = 0x04,                                 //4锰酸锂电池
    eBmsGbt15BatType_LithCoOxide        = 0x05,                                 //5钴酸锂电池
    eBmsGbt15BatType_Ternary            = 0x06,                                 //6三元材料电池
    eBmsGbt15BatType_PolyLitIon         = 0x07,                                 //7聚合物锂离子电池
    eBmsGbt15BatType_LithTitan          = 0x08,                                 //8钛酸锂电池
    eBmsGbt15BatType_Others             = 0xFF,                                 //FF其它电池类型
}__attribute__((packed)) eBmsGbt15BatType_t;





//电池组产权标识
typedef enum
{
    eBmsGbt15BatTypeTitleMarkLease      = 0x00,                                 //电池组所属：租赁
    eBmsGbt15BatTypeTitleMarkOwn        = 0x01,                                 //电池组所属：自有
    
    eGbtBatTypeTitleMarkMax,
}__attribute__((packed)) eBmsGbt15BatTitleMark_t;





//输出准备就绪标识
typedef enum
{
    eBmsGbt15ChgReadyStatusNotReady     = 0x00,                                 //未准备好充电
    eBmsGbt15ChgReadyStatusGetReady     = 0xAA,                                 //已准备好充电
    eBmsGbt15ChgReadyStatusInvaild      = 0xFF,                                 //无效
}__attribute__((packed)) eBmsGbt15ChgReadyStatus_t;






//充电模式
typedef enum
{
    eBmsGbt15ChgModeConstVol            = 0x01,                                 //恒压充电
    eBmsGbt15ChgModeConstCur            = 0x02,                                 //恒流充电
}__attribute__((packed)) eBmsGbt15ChgMode_t;







//充电状态
typedef enum
{
    eBmsGbt15ChgStatusPause             = 0x00,                                 //充电暂停
    eBmsGbt15ChgStatusPermit            = 0x01,                                 //充电允许
}__attribute__((packed)) eBmsGbt15ChgStatus_t;




//最高电压单体电池信息
typedef struct
{
    u16                                 u16MaxCellVol       : 12;               //最高单体电池电压，单位0.01V
    u16                                 u16OwnGroup         : 4;                //该电池所在组号
}__attribute__((packed)) stBmsGbt15CellInfo_t;




//ccs帧充电状态
typedef struct
{
    u8                                  bAllow              : 2;                //是否允许充电 0x00-暂停 0x01-允许
    u8                                  bStandby            : 6;                //预留
}__attribute__((packed)) stBmsGbt15CcsCode_t;





//表20-BSM电池故障描述
typedef struct
{
    //byte[6]
    u16                                 bBatVolFault        : 2;                //蓄电池电压故障 00-正常 01-过高 10-过低
    u16                                 bBatSocFault        : 2;                //蓄电池soc故障 00-正常 01-过高 10-过低
    u16                                 bBatCurFault        : 2;                //蓄电池电流故障 00-正常 01-过流 10-不可信状态
    u16                                 bBatTempFault       : 2;                //蓄电池过温故障 00-正常 01-过温 10-不可信状态
    
    //byte[7]
    u16                                 bBatIsulstatus      : 2;                //蓄电池绝缘故障 00-正常 01-不正常 10-不可信状态	
    u16                                 bBatConnStatus      : 2;                //蓄电池输出连接器故障 00-正常 01-不正常 10-不可信状态
    u16                                 bAllowChg           : 2;                //车端充电允许标志 00-禁止充电 01-允许充电
    u16                                 bStandby            : 2;
}__attribute__((packed)) stBmsGbt15BatFaultCode_t;





//表24-BMS中止充电原因
typedef struct
{
    u8                                  bSocFull            : 2;                //是否达到所需soc 00-未达到 01-达到 10-不可信状态
    u8                                  bBatVolTarget       : 2;                //是否达到总电压设定值 00-未达到 01-达到 10-不可信状态
    u8                                  bCellVolTarget      : 2;                //是否达到单体电压设定值 00-未达到 01-达到 10-不可信状态
    u8                                  bSeccStop           : 2;                //充电机主动停止 00-正常 01-充电机中止（发送CST）10-不可信状态
}__attribute__((packed)) stBmsGbt15BmsStopChgReason_t;


typedef union
{
    stBmsGbt15BmsStopChgReason_t        stReason;
    u8                                  u8Reason;
}__attribute__((packed)) unBmsGbt15BmsStopChgReason_t;







//表24-BMS中止充电故障原因
typedef struct
{
    //bit0~bit7
    u16                                 bInsulFault         : 2;                //绝缘故障 00-正常 01-故障 10-不可信状态
    u16                                 bOutConntFault      : 2;                //输出连接器过温 00-正常 01-故障 10-不可信状态
    u16                                 bComponConntFault   : 2;                //bms元件故障 00-正常 01-故障 10-不可信状态
    u16                                 bChgConntFault      : 2;                //充电连接器故障 00-正常 01-故障 10-不可信状态
    
    //bit8~bit15
    u16                                 bBatOverTemp        : 2;                //电池组温度过高 00-正常 01-故障 10-不可信状态
    u16                                 bHighVolRelay       : 2;                //高压继电器故障 00-正常 01-故障 10-不可信状态
    u16                                 bCC2CheckFault      : 2;                //CC2电压检测故障 00-正常 01-故障 10-不可信状态
    u16                                 bOtherFault         : 2;                //其他故障 00-正常 01-故障 10-不可信状态
}__attribute__((packed)) stBmsGbt15BmsStopChgFault_t;


typedef union
{
    stBmsGbt15BmsStopChgFault_t         stFault;
    u16                                 u16Fault;
}__attribute__((packed)) unBmsGbt15BmsStopChgFault_t;










//表24-BMS中止充电错误原因
typedef struct
{
    u8                                  bOverCur            : 2;                //充电电流过大 00-电流正常 01-电流超过需求值 10-不可信状态
    u8                                  bVolAbnormal        : 2;                //充电电压异常 00-电压正常 01-电压异常 10-不可信状态
    u8                                  bStandby            : 4;                //预留
}__attribute__((packed)) stBmsGbt15BmsStopChgErr_t;


typedef union
{
    stBmsGbt15BmsStopChgErr_t           stErr;
    u8                                  u8Err;
}__attribute__((packed))unBmsGbt15BmsStopChgErr_t;



















//表25-充电机中止充电原因
typedef struct
{
    u8                                  bReachGoal          : 2;                //达到充电机设定条件 00-正常 01-达到目标 10-不可信状态
    u8                                  bManual             : 2;                //人工中止 00-正常 01-人工中止 10-不可信状态
    u8                                  bSeccFault          : 2;                //充电机故障 00-正常 01-故障中止 10-不可信状态
    u8                                  bBmsStop            : 2;                //bms主动中止 00-正常 01-收到BST帧 10-不可信状态
}__attribute__((packed)) stBmsGbt15SeccStopReason_t;



typedef union
{
    stBmsGbt15SeccStopReason_t          stReason;
    u8                                  u8Reason;
}__attribute__((packed)) unBmsGbt15SeccStopReason_t;










//表25-充电机中止充电故障原因
typedef struct
{
    u16                                 bSeccOT             : 2;                //充电机过温故障 00-温度正常 01-充电机过温 10-不可信状态
    u16                                 bConnect            : 2;                //充电连接器故障 00-连接正常 01-连接器故障 10-不可信状态
    u16                                 bInternalOT         : 2;                //充电机内部过温 00-温度正常 01-内部过温 10-不可信状态
    u16                                 bEnergyAno          : 2;                //充电机能量异常 00-输出正常 01-不能传送 10-不可信状态
    u16                                 bEStop              : 2;                //急停按下故障 00-正常 01-急停按下 10-不可信状态
    u16                                 bOthers             : 2;                //其他故障
    u16                                 bStandby            : 4;                //预留
}__attribute__((packed)) stBmsGbt15SeccStopChgFault_t;


typedef union
{
    stBmsGbt15SeccStopChgFault_t        stFault;
    u16                                 u16Fault;
}__attribute__((packed)) unBmsGbt15SeccStopChgFault_t;













//表25-充电机中止充电错误原因
typedef struct
{
    u8                                  bCurNotMatch        : 2;                //电流不匹配	00-正常 01-电流不匹配 10-不可信状态
    u8                                  bVolAbnormal        : 2;                //电压异常 00-正常 01-电压异常 10-不可信状态
    u8                                  bStandby            : 4;
}__attribute__((packed)) stBmsGbt15SeccStopChgErr_t;




typedef union
{
    stBmsGbt15SeccStopChgErr_t          stErr;
    u8                                  u8Err;
}__attribute__((packed)) unBmsGbt15SeccStopChgErr_t;















//表29-bms发送错误报文原因
// 错误报文字节01
typedef struct
{
    u8                                  bCrm00Timeout       : 2;                //接收crm_00超时
    u8                                  bCrmAATimeout       : 2;                //接收crm_AA超时
    u8                                  bStandby            : 4;
}__attribute__((packed)) stBmsGbt15BemCode01;


typedef union
{
    stBmsGbt15BemCode01                 stBemCode01;
    u8                                  u8BemCode01;
}__attribute__((packed)) unBmsGbt15BemCode01;






// 错误报文字节02
typedef struct
{
    u8                                  bCmlTimeout         : 2;                //接收cml超时
    u8                                  bCroTimeout         : 2;                //接收cro超时
    u8                                  bStandby            : 4;
}__attribute__((packed)) stBmsGbt15BemCode02;


typedef union
{
    stBmsGbt15BemCode02                 stBemCode02;
    u8                                  u8BemCode02;
}__attribute__((packed)) unBmsGbt15BemCode02;





// 错误报文字节03
typedef struct
{
    u8                                  bCcsTimeout         : 2;                //接收ccs超时
    u8                                  bCstTimeout         : 2;                //接收cst超时
    u8                                  bStandby            : 2;
}__attribute__((packed)) stBmsGbt15BemCode03;


typedef union
{
    stBmsGbt15BemCode03                 stBemCode03;
    u8                                  u8BemCode03;
}__attribute__((packed)) unBmsGbt15BemCode03;






// 错误报文字节04
typedef struct
{
    u8                                  bCsdTimeout         : 2;                //接收csd超时
    u8                                  bStandby            : 6;
}__attribute__((packed)) stBmsGbt15BemCode04;


typedef union
{
    stBmsGbt15BemCode04                 stBemCode04;
    u8                                  u8BemCode04;
}__attribute__((packed)) unBmsGbt15BemCode04;











//表29-充电机发送错误报文原因
// 错误报文字节01
typedef struct
{
    u8                                  bBrmTimeout         : 2;                //接收brm超时
    u8                                  bStandby            : 6;
}__attribute__((packed)) stBmsGbt15CemCode01;


typedef union
{
    stBmsGbt15CemCode01                 stCemCode01;
    u8                                  u8CemCode01;
}unBmsGbt15CemCode01;




// 错误报文字节02
typedef struct
{
    u8                                  bBcpTimeout         : 2;                //接收bcp超时
    u8                                  bBroTimeout         : 2;                //接收bro超时
    u8                                  bStandby            : 4;
}__attribute__((packed)) stBmsGbt15CemCode02;


typedef union
{
    stBmsGbt15CemCode02                 stCemCode02;
    u8                                  u8CemCode02;
}unBmsGbt15CemCode02;







// 错误报文字节03
typedef struct
{
    u8                                  bBcsTimeout         : 2;                //接收bcs超时
    u8                                  bBclTimeout         : 2;                //接收bcl超时
    u8                                  bBstTimeout         : 2;                //接收bst超时
    u8                                  bStandby            : 2;
}__attribute__((packed)) stBmsGbt15CemCode03;



typedef union
{
    stBmsGbt15CemCode03 stCemCode03;
    u8                  u8CemCode03;
}unBmsGbt15CemCode03;







// 错误报文字节04
typedef struct
{
    u8                                  bBsdTimeout         : 2;                //接收bsd超时
    u8                                  bStandby            : 6;
}__attribute__((packed)) stBmsGbt15CemCode04;



typedef union
{
    stBmsGbt15CemCode04 stCemCode04;
    u8                  u8CemCode04;
}unBmsGbt15CemCode04;











#endif

