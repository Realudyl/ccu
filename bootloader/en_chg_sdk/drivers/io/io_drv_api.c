/**
 * @file io_drv_api.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief i32IoIndex driver api
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Shenzhen EN Plus TechnoEN_SLOGDes Co., Ltd. 2015-2024. All rights reserved 
 * 
 */

#include "io_drv_api.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelInfo
static const char *TAG = "io_drv_api";



//引脚配置结构体
extern stIoDrvDevice_t stIoDrvDevMap;




/**
 * @brief 设置io输出状态
 * 
 * @param i32IoIndex io索引
 * @param flagStatus 输出状态 
 * @return true 设置高电平 false 设置低
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-22
 */
bool sIoDrvSetOutput(i32 i32IoIndex, FlagStatus eStatus)
{
    //1.获取io参数配置表
    stIoDrvDevice_t* io = &stIoDrvDevMap;
    
    
    //2.检查io索引是否合法
    if(i32IoIndex >= io->u32PinNum  || i32IoIndex < 0)
    {
        return false;
    }
    
    //检查io是否为输出模式
    if (io->stIoDrvParaMap[i32IoIndex].u32PinInOutMode != GPIO_MODE_OUTPUT)
    {
        return false;
    }
    
    //设置io输出状态
    gpio_bit_write(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin, eStatus);
    
    
    //3.检查io设置是否成功
    if(gpio_output_bit_get(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin) == eStatus)
    {
        return true;
    }
    
    return false;
}




/**
 * @brief 获取io状态
 * 
 * @param i32IoIndex io索引
 * @return FlagStatus io状态。SET:1为高电平, RESET:0为低电平
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-22
 */
FlagStatus sIoDrvGetOutput(i32 i32IoIndex)
{
    //1.获取io参数配置表
    stIoDrvDevice_t* io = &stIoDrvDevMap;
    
    
    //2.检查io索引是否合法
    if(i32IoIndex >= io->u32PinNum || i32IoIndex < 0)
    {
        EN_SLOGD(TAG, "引脚索引错误");
        return false;
    }
    
    
    //3.检查io是否为输出模式
    if (io->stIoDrvParaMap[i32IoIndex].u32PinInOutMode != GPIO_MODE_OUTPUT)
    {
        EN_SLOGD(TAG, "引脚不是输出模式，不能获取输出状态");
        return false;
    }
    
    
    //4.获取io状态
    return gpio_output_bit_get(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin);
}




/**
 * @brief 翻转io状态
 * 
 * @param i32IoIndex io索引
 * @return true 翻转成功
 * @return false 翻转失败
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-22
 */
bool sIoDrvTogOutput(i32 i32IoIndex)
{
    //1.获取io参数配置表
    stIoDrvDevice_t* io = &stIoDrvDevMap;
    
    FlagStatus flagStatus = RESET;
    
    
    //2.检查io索引是否合法
    if(i32IoIndex >= io->u32PinNum  || i32IoIndex < 0)
    {
        EN_SLOGD(TAG, "引脚索引错误");
        return false;
    }
    
    
    //3.检查io是否为输出模式
    if (io->stIoDrvParaMap[i32IoIndex].u32PinInOutMode != GPIO_MODE_OUTPUT)
    {
        EN_SLOGD(TAG, "引脚不是输出模式，不能设置输出状态");
        return false;
    }
    
    
    //4.操作引脚状态
    flagStatus = gpio_output_bit_get(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin);
    gpio_bit_toggle(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin);
    
    
    
    //5.检查io翻转是否成功
    if(gpio_output_bit_get(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin) != flagStatus)
    {
        return true;
    }
    
    return false;
}




/**
 * @brief 获取输入引脚的输入状态
 * 
 * @param i32IoIndex 
 * @return FlagStatus 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-25
 */
FlagStatus sIoDrvGetInput(i32 i32IoIndex)
{
    //1.获取io参数配置表
    stIoDrvDevice_t* io = &stIoDrvDevMap;
    
    
    //2.检查io索引是否合法
    if(i32IoIndex >= io->u32PinNum  || i32IoIndex < 0)
    {
        EN_SLOGD(TAG, "引脚索引错误");
        return false;
    }
    
    
    //3.检查io是否为输入模式
    if (io->stIoDrvParaMap[i32IoIndex].u32PinInOutMode != GPIO_MODE_INPUT)
    {
        EN_SLOGD(TAG, "引脚不是输入模式，不能获取输入状态");
        return false;
    }
    
    //4.获取io状态
    return gpio_input_bit_get(io->stIoDrvParaMap[i32IoIndex].u32Port, io->stIoDrvParaMap[i32IoIndex].u32Pin);
}













