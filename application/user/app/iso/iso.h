/**
 * @file IsoCheck.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 绝缘检测
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#include "en_common.h"
#include "en_log.h"

#include "adc_app_sample.h"
#include "io_app.h"
#include "meter_drv_opt.h"
#include "mod_app_opt.h"



//绝缘检测标准，单位欧姆/伏特。>=cIsoOhmPerStd1:绝缘正常。<100:cIsoOhmPerStd2。其他绝缘异常
#define cIsoOhmPerStd1                  (500.0f)
#define cIsoOhmPerStd2                  (100.0f)



//判断车端接触器是否粘连：接触器外侧枪线电压大于此值，认为车端接触器是否粘连。
//单位1V
#define cIsoVolt1                       (100.0f)



/*判断模块输出是否正常
绝缘检测阈值，当模块输出电压指令值下发后，每隔cIsoTime1时间采样一次电压值，若cIsoTime1*cIsoCount1内，
模块输出电压与指令电压差值小于cIsoVolt2，认为模块输出正常。*/
//单位1V
#define cIsoVolt2                       (50.0f)
//单位ms
#define cIsoTime1                       (20u)
//单位次数
#define cIsoCount1                      (500u)



/*采样电压值
绝缘检测阈值，每隔cIsoTime2时间对比一次母排电压值，
超过cIsoTime2*cIsoCount3，认为采样处的电压不稳定*/
//单位s
#define cIsoTime2                       (500u)
//单位1V
#define cIsoVolt3                       (10.0f)
//次数
#define cIsoCount2                      (50u)



/*绝缘检测最后阶段，断开直流接触器后，需要判断母排电压是否小于60V，小于60v才能返回绝缘检测结束
每隔cIsoTime3查一次，最多查找cIsoCount4次。若连续cIsoCount4次母排电压仍然大于cIsoVolt4
则认为绝缘检测结束阶段母排电压异常*/
//单位s
#define cIsoTime3                       (200u)
//单位v
#define cIsoVolt4                       (60.0f)
//次数
#define cIsoCount3                      (50)



/**
 * @brief 绝缘检测类型
*/
typedef enum
{
    eIsoCheckBeforeCharge               = 0,                                    //充电前绝缘检测
    eIsoChekOnCharging                  = 1,                                    //充电中绝缘检测
}eIsoCheckMode_t;




/**
 * @brief 绝缘检测电压u1,u2枚举
 */
typedef enum
{ 
    eIsoTypeU1                          = 0,                                    //绝缘检测电压U1
    eIsoTypeU2                          = 1,                                    //绝缘检测电压U2
}eIsoTypeU_t;




/**
 * @brief 绝缘检测故障枚举
 */
typedef enum
{    
    eIsoOk                              = 0,                                    //绝缘检测正常  
    eIsoIndex                           = 1,                                    //枪索引超出范围
    eIsoCheckIng                        = 2,                                    //正在检测
    eIsoEvCs                            = 3,                                    //车端接触器粘连。Electric Vehicle Contactor Sticking
    eIsoEvseCs                          = 4,                                    //充电桩接触器粘连。 Electric Vehicle Charger Station Contactor Sticking
    eIsoEvseCf                          = 5,                                    //充电桩接触器故障。Electric Vehicle Charger Station Contactor Fault
    eIsoPmof                            = 6,                                    //电源模块输出故障。 Power Module Output Fault
    eIsoRelayf                          = 7,                                    //绝缘检测继电器故障
    eIsoRelayDrv1f                      = 8,                                    //绝缘检测正母排继电器故障
    eIsoRelayDrv2f                      = 9,                                    //绝缘检测负母排继电器故障
    eIsoU1UnSteady                      = 10,                                   //绝缘检测U1值不稳定
    eIsoU2UnSteady                      = 11,                                   //绝缘检测U2值不稳定
    eIsoBusVolt                         = 12,                                   //绝缘检测结束时母排电压在超时时间内未小于cIsoVolt4
    eIsoIa                              = 13,                                   //绝缘异常。Insulation Abnormal
    eIsoIf                              = 14,                                   //绝缘故障。Insulation Fault
    eIsoInvalid                         = 15,                                   //绝缘检测主动中断
}eIsoRst_t;




/**
 * @brief 绝缘检测状态机状态
*/
typedef enum
{
    eIsoCheckStart                      = 0,                                    //开始
    eIsoCheckModule                     = 1,                                    //闭合输出接触器
    eIsoCheckRelayAction1               = 2,                                    //闭合绝缘检测继电器，闭合绝缘检测正母排继电器，断开绝缘检测负母排继电器
    eIsoCheckRelayAction2               = 3,                                    //闭合绝缘检测负母排继电器，断开绝缘检测正母排继电器
    eIsoCheckCalcResult                 = 4,                                    //判断绝缘情况
    eIsoCheckEnd                        = 5,                                    //结束
    
    eIsoCheckIdel                                                                   
}eIsoCheckStatus_t;




/**
 * @brief 绝缘检测状态机状态与结果
*/
typedef struct
{
    eIsoRst_t                          eIsoRst;
    eIsoCheckStatus_t                  eIsoCheckStatus;
}eIsoMechinaStatus_t;









/**
 * @brief 绝缘检测数据
*/
typedef struct
{
    f32                                 f32IsoCheckVlot;                        //绝缘检测测试电压
    f32                                 f32U1;                                  //绝缘检测正母排继电器闭合时读取的电压
    f32                                 f32U2;                                  //绝缘检测负母排继电器闭合时读取的电压
    f32                                 f32Rx;                                  //绝缘检测正母排继电器闭合时，计算的电阻
    f32                                 f32Ry;                                  //绝缘检测负母排继电器闭合时，计算的电阻
    f32                                 f32Ubus;                                //绝缘检测使用的电压
    f32                                 f32OhmPerV1;                            //绝缘检测正母排继电器闭合时，欧姆/伏特
    f32                                 f32OhmPerV2;                            //绝缘检测负母排继电器闭合时，欧姆/伏特
    f32                                 f32OhmPerV;                             //绝缘检测结果标准 欧姆/伏特
    bool                                bRst;                                   //绝缘检测结果
    f32                                 f32ModuleVoltRead;                      //电源模块读取电压
    u32                                 u32ModuleCount;                         //电源模块电压稳定计数
    u32                                 u32ModuleTimeStamp;                     //电源模块判断稳定用的时间戳
    f32                                 f32Voltbus;                             //绝缘检测时的母排电压，用于判断绝缘检测电压是否稳定
    u32                                 u32Count;                               //用于判断绝缘检测电压是否超时的计数
    f32                                 f32Voltbus1;                            //绝缘采样读取u1时对应的母排电压
    f32                                 f32Voltbus2;                            //绝缘采样读取u2时对应的母排母电压
    u32                                 u32UisoTimeStamp;                       //用于判断绝缘检测电压是否稳定的时间戳
    u32                                 u32EndCount;                            //绝缘检测结束信号检测计数
    u32                                 u32EndTimeStamp;                        //绝缘检测结束时间戳
}stIsoData_t;










//ISO检测 缓存数据结构定义
typedef struct
{
    //ISO检测需要控制枪输出，由外部传入相关回调函数去执行
    bool                                (*pCbOpen)(ePileGunIndex_t eGunIndex);  //开启输出
    bool                                (*pCbClose)(ePileGunIndex_t eGunIndex); //关闭输出
    bool                                (*pCbSet)(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
    
    stIsoData_t                         stIsoData[ePileGunIndexNum];
    eIsoMechinaStatus_t                 eIsoMechinaStatus[ePileGunIndexNum];
}stIsoCache_t;













extern bool sIsoCheckInit(bool (*pCbOpen )(ePileGunIndex_t eGunIndex), 
                   bool (*pCbClose)(ePileGunIndex_t eGunIndex), 
                   bool (*pCbSet  )(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit));


extern eIsoMechinaStatus_t* sIosCheck(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode);
extern void sIsoStart(ePileGunIndex_t eGunIndex, f32 f32IsoCheckVolt);
extern bool sIsoStop(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode);
extern bool sIsoGetOhm(ePileGunIndex_t eGunIndex, f32* Ohm);









#ifdef __cplusplus
}
#endif









