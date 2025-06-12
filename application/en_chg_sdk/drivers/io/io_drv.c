/**
 * @file io_drv.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief io driver data base
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Shenzhen EN Plus TechnoEN_SLOGDes Co., Ltd. 2015-2024. All rights reserved 
 * 
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "io_drv.h"




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "io_drv";




extern stIoDrvDevice_t stIoDrvDevMap;






















/**
 * @brief 初始化io引脚
 * 
 * @return true 初始化成功
 * @return false 初始化失败
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-25
 */
bool sIoDrvInitDev(void)
{
    u32 u32PinIndex = 0;
    stIoDrvDevice_t* io = &stIoDrvDevMap;
    
    
    EN_SLOGD(TAG, "初始化GPIO引脚");
    
    
    //1.判断io数量是否超过最大数量
    if((io->u32PinNum > cIoDrvPinMaxNum) || (io->u32PinNum == 0))
    {
        EN_SLOGD(TAG, "io 数量超限或为0");
        return false;
    }
    
    
    //2.初始化io引脚
    for (u32PinIndex = 0; u32PinIndex < io->u32PinNum; u32PinIndex++)
    {
        //2.1使能io引脚时钟
        rcu_periph_clock_enable(io->stIoDrvParaMap[u32PinIndex].u32Rcu);
        
        
        //2.2设置io引脚模式
        gpio_mode_set(io->stIoDrvParaMap[u32PinIndex].u32Port,  
                      io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode, 
                      io->stIoDrvParaMap[u32PinIndex].u32PinPullUpDownMode,  
                      io->stIoDrvParaMap[u32PinIndex].u32Pin);
                      
                      
        //2.3如果是输出引脚，设置输出类型和速度
        if(io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode == GPIO_MODE_OUTPUT)
        {
            gpio_output_options_set(io->stIoDrvParaMap[u32PinIndex].u32Port, 
                                    io->stIoDrvParaMap[u32PinIndex].u32PinOutType, 
                                    io->stIoDrvParaMap[u32PinIndex].u32PinOutSpeed, 
                                    io->stIoDrvParaMap[u32PinIndex].u32Pin);
        }
        else if(io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode == GPIO_MODE_INPUT)
        {
            //2.4如果是输入引脚，设置输入滤波器
            gpio_input_filter_set(io->stIoDrvParaMap[u32PinIndex].u32Port, 
                                  io->stIoDrvParaMap[u32PinIndex].u32PinInPeriod, 
                                  io->stIoDrvParaMap[u32PinIndex].u32PinInFType, 
                                  io->stIoDrvParaMap[u32PinIndex].u32Pin);
        }
        else
        {
            //2.5io引脚模式错误
            EN_SLOGD(TAG, "io模式错误,io引脚索引:%d", u32PinIndex);
            return false;
        }
    }
    
    return true;
}





/**
 * @brief 运行中改变io引脚的配置
 * 
 * @param io io引脚配置
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-06-13
 */
bool sIoDrvInit(stIoDrvDevice_t *io)
{
    u32 u32PinIndex = 0;
    
    
    EN_SLOGD(TAG, "初始化GPIO引脚");
    
    
    
    //1.判断io数量是否超过最大数量
    if((io->u32PinNum > cIoDrvPinMaxNum) || (io->u32PinNum == 0))
    {
        EN_SLOGD(TAG, "io 数量超限或为0");
        return false;
    }
    
    
    //2.初始化io引脚
    for (u32PinIndex = 0; u32PinIndex < io->u32PinNum; u32PinIndex++)
    {
        //2.1使能io引脚时钟
        rcu_periph_clock_enable(io->stIoDrvParaMap[u32PinIndex].u32Rcu);
        
        
        //2.2设置io引脚模式
        gpio_mode_set(io->stIoDrvParaMap[u32PinIndex].u32Port,  
                      io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode, 
                      io->stIoDrvParaMap[u32PinIndex].u32PinPullUpDownMode,  
                      io->stIoDrvParaMap[u32PinIndex].u32Pin);
                      
                      
        //2.3如果是输出引脚，设置输出类型和速度
        if(io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode == GPIO_MODE_OUTPUT)
        {
            gpio_output_options_set(io->stIoDrvParaMap[u32PinIndex].u32Port, 
                                    io->stIoDrvParaMap[u32PinIndex].u32PinOutType, 
                                    io->stIoDrvParaMap[u32PinIndex].u32PinOutSpeed, 
                                    io->stIoDrvParaMap[u32PinIndex].u32Pin);
        }
        else if(io->stIoDrvParaMap[u32PinIndex].u32PinInOutMode == GPIO_MODE_INPUT)
        {
            //2.4如果是输入引脚，设置输入滤波器
            gpio_input_filter_set(io->stIoDrvParaMap[u32PinIndex].u32Port, 
                                  io->stIoDrvParaMap[u32PinIndex].u32PinInPeriod, 
                                  io->stIoDrvParaMap[u32PinIndex].u32PinInFType, 
                                  io->stIoDrvParaMap[u32PinIndex].u32Pin);
        }
        else
        {
            //2.5io引脚模式错误
            EN_SLOGD(TAG, "io模式错误,io引脚索引:%d", u32PinIndex);
            return false;
        }
    }
    
    return true;
}

#ifdef __cplusplus
}
#endif

