/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   io_pwm_drv.h
* Description                           :   PWM驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-31
* notice                                :   
****************************************************************************************************/
#ifndef _io_pwm_drv_H_
#define _io_pwm_drv_H_
#include "gd32h7xx_libopt.h"
#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"










//PWM DMA CH数量
#ifndef cSdkIoPwmDrvDmaChanNum
#define cIoPwmDrvDmaChanNum             (1)
#else
#define cIoPwmDrvDmaChanNum             (cSdkIoPwmDrvDmaChanNum)
#endif


#define cIoPwmDmaBufSize                1760                                    //DMA缓存数组大小












//PWM驱动 独立/同步模式
typedef enum
{
    ePwmIndependent                     = 0,                                    //PWM & PWM 各自独立
    ePwmParallelSynchro,                                                        //          并行同步
    
    ePwmAndPwmModeMax
}ePwmAndPwmMode_t;

//同步模式的主/从
typedef enum
{
    eParallelMaster                     = 0,                                    //同步并行 主机
    eParallelSlave,                                                             //         从机
    
    eParallelSynRelationMax
}eParallelSynRelation_t;







//PWM驱动 GPIO管脚配置
typedef struct
{
    i32                                 i32Periph;                              //GPIO     外设
    i32                                 i32PeriphRcu;                           //GPIO RCU 外设
    i32                                 i32Pin;                                 //GPIO     引脚
    i32                                 i32Mode;                                //GPIO     模式
    i32                                 i32Pull;                                //GPIO     上拉
    i32                                 i32Af;                                  //GPIO pin af function
    i32                                 i32Speed;                               //GPIO     速率
    
    
}stIoPwmDrvCfgGpio_t;





//PWM驱动 DMA配置
typedef struct
{
    i32                                 i32Periph;                              //DMA     外设
    i32                                 i32PeriphRcu;                           //DMA RCU 外设
    i32                                 i32Chan;                                //DMA     通道
    i32                                 i32Req;                                 //DMAMUX  请求路由通道的DMA请求输入源
    i32                                 i32PeriphAddr;                          //DMA     传输外设地址
    u16                                 *pDmaBuf;                               //DMA     数据缓存区
    i32                                 i32DmaBufSize;                          //DMA     数据缓存区大小
    i32                                 i32AccessBaseAddr;                      //DMA     数据存取基地址
    i32                                 i32TransTime;                           //DMA     传输时间
    i32                                 i32UpdateSource;                        //DMA     数据更新源
}stIoPwmDrvCfgDma_t;





//PWM驱动 TIMER_CH配置
typedef struct
{
    i32                                 i32Chan;                                //Timer     通道
    i8                                  i8IoPwm;                                //对应通道的定义
    stIoPwmDrvCfgGpio_t                 stGpio;                                 //          输出GPIO
    stIoPwmDrvCfgDma_t                  stDma;                                  //          DMA配置
    
    void                                (*pCbClose)(u32 u32Periph, u32 u32Pin); //          关闭打波的回调函数
    char                                *pString;                               //          描述字符串
    
}stIoPwmDrvCfgTimerCh_t;



#define cPwmChanNumMax                  (4)                                     //同一定时器下最多只能设置4个通道
//PWM驱动 TIMER配置
typedef struct
{
    i32                                 i32Periph;                              //Timer     外设
    i32                                 i32PeriphRcu;                           //Timer RCU 外设
    i32                                 i32Psc;                                 //          预分频系数
    i32                                 i32Period;                              //          周期值
    i8                                  i8ChanUsed;                             //Timer     通道实际使用数
    stIoPwmDrvCfgTimerCh_t              stChan[cPwmChanNumMax];                 //Timer     通道
    
}stIoPwmDrvCfgTimer_t;





//Trigsel驱动配置
typedef struct
{
    i32                                 i32TargetPeriph;                        //目标外设
    i32                                 i32TriggerSource;                       //触发源
    
    i32                                 i32MasterTrigger;                       //主机触发输出源
    
    i32                                 i32SlaveTrigger;                        //从机触发输入源
    i32                                 i32SlaveMode;                           //从机触发模式
    
}stIoPwmDrvCfgTrigsel_t;





//PWM驱动 设备
typedef struct
{
    ePwmAndPwmMode_t                    ePwmAndPwmMode;                         //PWM & PWM 之间的模式
    
    
    union
    {
        struct
        {
            stIoPwmDrvCfgTimer_t        stTimer;                                        //pwm设备使用的定时器
            
        }stPwmIndependent;
        
        struct
        {
            eParallelSynRelation_t      eRelation;                                      //pwm同步模式的主从关系
            stIoPwmDrvCfgTrigsel_t      stTrigsel;                                      //pwm同步模式触发源配置
            stIoPwmDrvCfgTimer_t        stTimer;                                        //pwm设备使用的定时器
            
        }stPwmParallelSynchro;
    };
    
}stIoPwmDrvDevice_t;











extern bool sIoPwmDrvInitDev(void);







#endif























