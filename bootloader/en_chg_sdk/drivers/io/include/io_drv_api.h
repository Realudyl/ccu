/**
 * @file io_drv_api.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief io driver api
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif



#include "io_drv.h"




extern bool sIoDrvSetOutput(i32 i32IoIndex, FlagStatus eStatus);
extern FlagStatus sIoDrvGetOutput(i32 i32IoIndex);
extern bool sIoDrvTogOutput(i32 i32IoIndex);
extern FlagStatus sIoDrvGetInput(i32 i32IoIndex);



#ifdef __cplusplus
}
#endif
