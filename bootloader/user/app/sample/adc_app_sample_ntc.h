/**
 * @file adc_app_sample_ntc.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 计算NTC温度值
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */
#pragma once
#include "en_common.h"


/**
 * @brief NTC温度计算数据结构
*/
typedef struct
{
    f32                                 f32Temp;                                //温度
    f32                                 f32Rmax;                                //最大电阻
    f32                                 f32Rnor;                                //标准电阻
    f32                                 f32Rmin;                                //最小电阻
}stTempData_t;



extern bool sGetNtcTemp(f32 f32Ohm, f32* pTemp);