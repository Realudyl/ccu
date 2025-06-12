/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :   en_chg_sdk_cfg.h
* Description                           :   EN+ chg SDK 参数配置文件
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2023-12-16
* notice                                :   本文件为SDK内部参数配置文件,开放给SDK用户使用
*
*                                           定制品应该按照实际的需求去配置以下参数
*                                           如果屏蔽某一项， SDK内部会按照默认值初始化
****************************************************************************************************/
#ifndef _en_chg_sdk_cfg_H_
#define _en_chg_sdk_cfg_H_





//******************************************************************************
//                                      充电枪配置
//******************************************************************************
typedef enum
{
    ePileGunIndex0                      = 0,                                    //第1把枪
    ePileGunIndex1                      = 1,                                    //第2把枪
    
    ePileGunIndexNum                                                            //枪数量
}ePileGunIndex_t;                                                               //枪序号枚举定义








//******************************************************************************
//                                      ADC采样驱动配置
//******************************************************************************
#define cSdkAdcDrvCycleIns              (20)                                    //注入采样值求平均值的累加次数
#define cSdkAdcDrvCycleReg              (360)                                   //规则采样值求平均值的累加次数
#define cSdkAdcDrvPinNum                (18)                                    //采样通道 数量



//******************************************************************************
//                                      看门狗设置
//******************************************************************************
#define cSdkFwdogPriv (30u)












//******************************************************************************
//                                      CAN驱动配置
//******************************************************************************










//******************************************************************************
//                                      刷卡器 配置
//******************************************************************************
#define cSdkCardComUartNum              (eUsartNum1)                            //串口号
#define cSdkCardComBaudrate             (115200)                                //波特率







//******************************************************************************
//                                      eeprom驱动配置
//******************************************************************************







//******************************************************************************
//                                      I2C驱动配置
//******************************************************************************







//******************************************************************************
//                                      io驱动配置
//******************************************************************************
#define cSdkIoDrvPinMaxNum              (36)                                    //IO口最大数量







//******************************************************************************
//                                      io_pwm驱动配置
//******************************************************************************
#define cSdkIoPwmDrvPwmTimerNum         (3)                                     //PWM Timer数量
#define cSdkIoPwmDrvDmaChanNum          (2)                                     //PWM DMA CH数量 ------ 该宏定义需与实际使用到的DMA通道数对应上, 关系到DMA数据缓存数组个数







//******************************************************************************
//                                      PLC/MSE102x驱动配置
//******************************************************************************

//PLC/MSE102x 通道数
//如果都是CCS枪, 就可以取枪数量 ePileGunIndexNum 值,因为每把枪都要对应一个PLC载波芯片
//但如果是CCS枪+国标或日标枪, 这里可以设置CCS枪的实际数量
#define cSdkMse102xDevNum               (ePileGunIndexNum)








//******************************************************************************
//                                      SPI驱动配置
//******************************************************************************








//******************************************************************************
//                                      UART/USART驱动配置
//******************************************************************************
//串口DMA buf size 定义
//内网OTA升级时需要接收帧长820字节, DMA buf小于此长度会造成无法接收完整包
#define cSdkUsartDmaBufSize             (1024)









//******************************************************************************
//                                      shell 界面配置
//******************************************************************************
//shell 界面配置--->串口硬件配置
#define cSdkShellComUartNum             (eUsartNum0)                            //串口号
#define cSdkShellComBaudrate            (230400)                                //波特率

//shell 界面配置--->支持的最大指令条数
#define cSdkShellCmdNumMax              (128)








//******************************************************************************
//                                      log 日志组件配置
//******************************************************************************
//log打印开关默认状态 true:开启打印 false:关闭打印
#define cSdkLogPrintSwitch              (true)









//******************************************************************************
//                                      电表驱动配置
//******************************************************************************
//电表驱动--->串口硬件配置
#define cSdkMeterComUartNum             (eUsartNum2)                            //串口
#define cSdkMeterComBaudrate            (9600)                                  //波特率

//电表驱动--->电表个数
//如果硬件设计上为每把枪都配置了一个电表，这里就可以取枪数量 ePileGunIndexNum
//如果使用支持多路的电表，则要配置实际的电表数量
#define cSdkMeterNum                    (1)









//******************************************************************************
//                                      模块电源驱动配置
//******************************************************************************
//模块电源驱动--->CAN接口硬件配置
#define cSdkModCanPort                  (eCanNum0)                              //CAN端口
#define cSdkModCanBaudrate              (125000)                                //波特率
#define cSdkModDrvRecvTaskCycle         (10)                                    //接收任务周期 ms
#define cSdkModDrvSendTaskCycle         (300)                                   //发送任务周期 ms

//模块电源输入输出特性曲线定义
#define cSdkModDrvUdcMax                (1000.0f)                               //最大输出电压
#define cSdkModDrvUdcMin                (200.0f)                                //最小输出电压
#define cSdkModDrvIdcMax                (133.3f)                                //最大输出电流
#define cSdkModDrvPdc                   (40000.0f)                              //最大输出功率





//******************************************************************************
//                                      模块电源用户程序配置
//******************************************************************************
//均充算法相关配置
#define cSdkModAppAvgModNumMax          (4)                                     //控制的模块数量
#define cSdkModAppAvgTimeSwitchIn       (60)                                    //模块切入的等待时间






//******************************************************************************
//                                      国标bms驱动配置
//******************************************************************************
//bms驱动--->CAN接口硬件配置
#define cSdkEvcc01CanPort               (eCanNum1)                              //CAN端口
#define cSdkEvcc01CanBaudrate           (250000)                                //波特率

#define cSdkEvcc02CanPort               (eCanNum2)                              //CAN端口
#define cSdkEvcc02CanBaudrate           (250000)                                //波特率










//---------------------------------------------------4.内网私有协议---------------------------------------------------

//1.串口硬件配置
#define cSdkPrivDrvComUartNum           (eUsartNum5)                            //串口号
#define cSdkPrivDrvComBaudrate          (115200)                                //波特率

//2.内网私有协议--->协议配置
#define cSdkPrivComRecvTaskStackSize    (512)                                   //接收任务堆栈大小
#define cSdkPrivDrvSendTaskStackSize    (512)                                   //发送任务堆栈大小
#define cSdkPrivDrvAddrDefault          (0x0001)                                //桩端通讯地址默认值

#ifdef DC480K_G2SF312_250                                                       //风冷双枪
#define cSdkPrivDrvGunNum               (ePileGunIndexNum)                      //枪数量，最大允许254
#elif defined(DC480K_G2SF311_600)                                               //液冷单枪
#define cSdkPrivDrvGunNum               (1)                                     //枪数量，最大允许254
#endif

#define cSdkPrivDrvUserIdLen            (32)                                    //长度:用户id
#define cSdkPrivDrvOrderIdLen           (32)                                    //长度:订单编号
#define cSdkPrivDrvSrvTradeIdLen        (32)                                    //长度:平台交易流水号
#define cSdkPrivDrvDevTradeIdLen        (32)                                    //长度:设备交易流水号
#define cSdkPrivDrvParentIdLen          (20)                                    //长度:虚拟id号
#define cSdkPrivDrvRateUserDefineLen    (32)                                    //长度:自定义费率模型长度, 最大允许 511

//应保证 价格字段的精度 不低于 金额字段的精度
#define cSdkPrivDrvPrecisionPrice       (5)                                     //精度:价格字段精度---0~8, 超过时取默认值5
#define cSdkPrivDrvPrecisionMoney       (5)                                     //精度:金额字段精度---0~8, 超过时取默认值3


#define cSdkPrivDrvRsvdLen02            (1)                                     //02报文----预留字段长度
#define cSdkPrivDrvRsvdLenA3            (1)                                     //A3报文
#define cSdkPrivDrvRsvdLen05            (1)                                     //05报文
#define cSdkPrivDrvRsvdLenA5            (1)                                     //A5报文
#define cSdkPrivDrvRsvdLen06            (1)                                     //06报文
#define cSdkPrivDrvRsvdLenA6            (1)                                     //A6报文
#define cSdkPrivDrvRsvdLen07            (1)                                     //07报文
#define cSdkPrivDrvRsvdLenA7            (1)                                     //A7报文
#define cSdkPrivDrvRsvdLenA8            (1)                                     //A8报文
#define cSdkPrivDrvRsvdLen0E            (1)                                     //0E报文
#define cSdkPrivDrvRsvdLenAE            (1)                                     //AE报文
#define cSdkPrivDrvRsvdLen14            (1)                                     //14报文
#define cSdkPrivDrvRsvdLenB4            (1)                                     //B4报文

//3.报文配置 单位 秒
#define cSdkPrivDrvCycle02              (30)                                    //02报文 发送周期------未收到A2时
#define cSdkPrivDrvReloginGap           (10)                                    //A2报文 重登陆间隔
#define cSdkPrivDrvCycle03              (3600)                                  //03报文 发送周期------状态没改变时
#define cSdkPrivDrvCycle04              (120)                                   //04报文 发送周期
#define cSdkPrivDrvCardAuthAckTimeout   (5)                                     //A5报文 卡认证请求响应超时时间

#define cSdkPrivDrvCycleFastTime        (180)                                   //各报文 快速周期时间--08/0E/13公用
#define cSdkPrivDrvCycle08              (1)                                     //08报文 发送周期------
#define cSdkPrivDrvCycle08Fast          (1)                                     //08报文 发送周期------
#define cSdkPrivDrvCycle0E              (1)                                     //0E报文 发送周期------充电稳定后
#define cSdkPrivDrvCycle0EFast          (1)                                     //0E报文 发送周期------刚开始充电
#define cSdkPrivDrvCycle0ESlow          (30)                                    //0E报文 发送周期------未充电时
#define cSdkPrivDrvCycle13              (1)                                     //13报文 发送周期
#define cSdkPrivDrvCycle13Fast          (1)                                     //13报文 发送周期
#define cSdkPrivDrvPrintCycle           (60)                                    //       打印周期------08/0E/13公用


//4.常用-打印配置
#define cSdkPrivDrvDataPrintMax         (800)                                   //打印最大长度
#define cSdkPrivDrvDataPrintMaxVaule    (40)                                    //打印最大之后需要打印的值

//5.常用-检查配置
#define cSdkPrivDrvPktTimeOutEn         (1)                                     //Pkt超时使能
#define cSdkPrivDrvPktTimeOutSec        (5)                                     //Pkt超时时间(使能有效才起左右)
#define cSdkPrivDrvPrintfEn             (0)                                     //串口Pkt/com/can测试打印使能

//6.常用-异常检查配置
#define cSdkPrivDrvRecvErrCheckEn       (1)                                     //接收异常检查使能
#define cSdkPrivDrvRecvErrRebootCnt     (20)                                    //接收异常检查复位次数(接收异常检查使能才有效,0不复位)
#define cSdkPrivDrvRecvErrResetEn       (1)                                     //接收异常复位参数使能(接收异常检查使能才有效)
#define cSdkPrivDrvRecvDelay            (20)                                    //桩端通讯接收延时时间(ms)-----此参数在esp32侧可以设置0(是阻塞接收的), 在GD32侧不能设置0(非阻塞接收, 等待时间为0会死占CPU)

//7.常用-异常检查配置
#define cSdkPrivDrvWorkStsConvertEn     (0)                                     //OCPP工作状态转换使能(兼容3.0~3.6协议因此需要打开)














#endif




















