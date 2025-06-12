/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   adc_app_sample_spec.h
* Description                           :   ADC采样计算 之 部分特殊采样通道的数据处理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-13
* notice                                :   
****************************************************************************************************/
#ifndef _adc_app_sample_spec_H_
#define _adc_app_sample_spec_H_
#include "en_common.h"















//--------------------------------------反馈信号相关----------------------------
//ADC采样通道实现的反馈信号状态定义
typedef enum
{
    eAdcAppFbState00                    = 0,                                    //DC+断开 DC-断开---3.0V
    eAdcAppFbStateX0                    = 1,                                    //DC+粘连 DC-断开---2.5V
    eAdcAppFbState0X                    = 2,                                    //DC+断开 DC-粘连---1.666V
    eAdcAppFbState11                    = 3,                                    //DC+闭合 DC-闭合---0V
    
    eAdcAppFbStateMax
}eAdcAppFbState_t;

//各个反馈状态对应的采样值 单位V
#define cAdcAppVoltFbState00            (2.9f)
#define cAdcAppVoltFbStateX0            (2.2f)
#define cAdcAppVoltFbState0X            (1.2f)
#define cAdcAppVoltFbState11            (0.5f)

//检测的允许误差 单位V
#define cAdcAppVoltFbErr                (0.4f)


//一个用于将此类采样值换算为上述枚举值的资源结构体
typedef struct
{
    i32                                 i32Cnt;                                 //反馈状态变化滤波器
    eAdcAppFbState_t                    eState;                                 //反馈状态
    eAdcAppFbState_t                    eStateTemp;                             //反馈状态中间值
    
}stAdcAppFbData_t;








//--------------------------------------枪外侧电压相关--------------------------
//用户程序一些逻辑需要等待枪外侧电压稳定后才能取值
typedef struct
{
    u32                                 u32Tick;                                //上一拍检测的tick值
    f32                                 f32Udc;                                 //枪线电压
    f32                                 f32UdcStable;                           //枪线电压稳定值
    bool                                bStableFlag;                            //枪线电压稳定标志
}stAdcAppGunDcOutData_t;









//--------------------------------------欧标PP相关------------------------------
//PP状态定义
typedef enum
{
    eAdcAppPpStateU1a                   = 0,                                    //2.77V----机械锁打开，未插枪（用户拿枪按下开关准备插枪）
    eAdcAppPpStateU1b                   = 1,                                    //0V-------机械锁闭合，未插枪
    eAdcAppPpStateU1c                   = 2,                                    //1.53V----机械锁闭合，已插枪（插枪状态）
    
    eAdcAppPpStateMax
}eAdcAppPpState_t;

//各个状态对应的采样值 单位V
#define cAdcAppVoltPpStateU1a           (2.77f)
#define cAdcAppVoltPpStateU1b           (0.00f)
#define cAdcAppVoltPpStateU1c           (1.53f)

//检测的允许误差 单位V
#define cAdcAppVoltPpErr                (0.39f)


//一个用于将此类采样值换算为上述枚举值的资源结构体
typedef struct
{
    f32                                 f32Value;                               //采样值
    i32                                 i32Cnt;                                 //状态变化滤波器
    eAdcAppPpState_t                    eState;                                 //状态
    eAdcAppPpState_t                    eStateTemp;                             //状态中间值
}stAdcAppPpData_t;










//--------------------------------------日标PD相关------------------------------
//PD状态定义
typedef enum
{
    eAdcAppPdStatePlugOut               = 0,                                    //0V----未插枪
    eAdcAppPdStatePlugIn                = 1,                                    //2V----已插枪---R1和R3对12V进行分压（插枪状态，枪和车完全闭合接触上）
    
    eAdcAppPdStateMax
}eAdcAppPdState_t;

//各个状态对应的采样值 单位V
#define eAdcAppPdStatePlugOut           (0.0f)
#define eAdcAppPdStatePlugIn            (2.0f)

//检测的允许误差 单位V
#define cAdcAppVoltPdErr                (1.0f)


//一个用于将此类采样值换算为上述枚举值的资源结构体
typedef struct
{
    f32                                 f32Value;                               //采样值
    i32                                 i32Cnt;                                 //状态变化滤波器
    eAdcAppPdState_t                    eState;                                 //状态
    eAdcAppPdState_t                    eStateTemp;                             //状态中间值
}stAdcAppPdData_t;








//--------------------------------------CC1(检测点1)相关------------------------
//CC1状态定义
typedef enum
{
    eAdcAppCc1StateU1a                  = 0,                                    //12V---机械锁打开，未插枪（用户拿枪按下开关准备插枪）
    eAdcAppCc1StateU1b                  = 1,                                    //6V----机械锁闭合，未插枪
    eAdcAppCc1StateU1c                  = 2,                                    //4V----机械锁闭合，已插枪（插枪状态）
    
    eAdcAppCc1StateMax
}eAdcAppCc1State_t;

//各个状态对应的采样值 单位V
#define cAdcAppVoltCc1StateU1a          (12.0f)
#define cAdcAppVoltCc1StateU1b          (6.0f)
#define cAdcAppVoltCc1StateU1c          (4.0f)

//检测的允许误差 单位V
#define cAdcAppVoltCc1Err               (1.0f)


//一个用于将此类采样值换算为上述枚举值的资源结构体
typedef struct
{
    f32                                 f32Value;                               //采样值
    i32                                 i32Cnt;                                 //状态变化滤波器
    eAdcAppCc1State_t                   eState;                                 //状态
    eAdcAppCc1State_t                   eStateTemp;                             //状态中间值
}stAdcAppCc1Data_t;









//--------------------------------------欧标CP相关------------------------------
//IEC 61851 定义的CP信号
typedef enum
{
    eChgCpStateA                        = 0,                                    //State A (+12V)  电动汽车没有连接到充电桩
    eChgCpStateB                        = 1,                                    //State B (+9V)   电动汽车连接到充电桩，但没有开始充电
    eChgCpStateC                        = 2,                                    //State C (+6V)   电动汽车连接到充电桩上，准备好开始充电 （不需要通风 ventilation）
    eChgCpStateD                        = 3,                                    //State D (+3V)   电动汽车连接到充电桩上，准备好开始充电 （  需要通风 ventilation）
    eChgCpStateE                        = 4,                                    //State E (+0V)   电网供电有问题或未连接到电网。
    eChgCpStateF                        = 5,                                    //State F (-12V)  充电桩未准备好。
    
    eChgCpStateMax
}eChgCpState_t;


//IEC 61851 定义的CP电压值 单位V
#define cChgUcpA                        (12)
#define cChgUcpB                        (9)
#define cChgUcpC                        (6)
#define cChgUcpD                        (3)
#define cChgUcpE                        (0)
#define cChgUcpF                        (-12)

//CP电压值检测的允许误差
#define cChgUcpErr                      (1)


//
typedef struct
{
    f32                                 f32Value;                               //采样值
    i32                                 i32CpCnt;                               //cp状态变化滤波器
    eChgCpState_t                       eCpState;                               //CP状态当前值
    eChgCpState_t                       eCpStateTemp;                           //CP状态中间值
}stAdcAppCpData_t;









//--------------------------------------日标J电压相关---------------------------


























extern void sAdcAppSampleCheckFbState (stAdcAppFbData_t  *pFbData,  f32 f32Ufb);
extern void sAdcAppSampleCheckGunDcOut(ePileGunIndex_t eGunIndex,   f32 f32UdcOut);
extern void sAdcAppSampleCheckPpState (ePileGunIndex_t eGunIndex,   f32 f32Upp);
extern void sAdcAppSampleCheckPdState (ePileGunIndex_t eGunIndex,   f32 f32Upd);
extern void sAdcAppSampleCheckCc1State(ePileGunIndex_t eGunIndex,   f32 f32Ucc1);
extern void sAdcAppSampleCheckCpState (ePileGunIndex_t eGunIndex,   f32 f32Ucp);





#endif






















