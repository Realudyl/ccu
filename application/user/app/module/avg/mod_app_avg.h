/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_avg.h
* Description                           :   模块电源管理用户代码 之 均充算法实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-03-25
* notice                                :   
****************************************************************************************************/
#ifndef _mod_app_avg_h_
#define _mod_app_avg_h_
#include "mod_drv_opt.h"
#include "io_app.h"










//定义均充算法管理的最大模块数量---公司均充硬件的机型目前最多就4个模块
#ifndef cSdkModAppAvgModNumMax
#define cModAppAvgModNumMax             (4)
#else
#define cModAppAvgModNumMax             (cSdkModAppAvgModNumMax)
#endif





//空闲模块投入对侧枪的等待时间
#ifndef cSdkModAppAvgTimeSwitchIn
#define cModAppAvgTimeSwitchIn          (60)
#else
#define cModAppAvgTimeSwitchIn          (cSdkModAppAvgTimeSwitchIn)
#endif




//定义模块的无效地址
//模块地址一般都是 u8 类型, -1的16进制等于0xff 可能是一些模块协议定义的广播地址
//但不妨碍这里将其作为无效地址使用 因为模块自己的地址不可能是广播地址
#define cModAppAvgModAddrInvalid        (-1)




//一些切换判断的阈值定义
#ifndef cSdkModDrvUdcMin
#define cModAppAvgModUdcOpen            (130.0f)
#else
#define cModAppAvgModUdcOpen            (cSdkModDrvUdcMin - 20.0f)
#endif
#define cModAppAvgModUdcClose           (60.0f)


#define sModAppAvgGetModUdcOpen(x)      ((x > cModAppAvgModUdcOpen) ? (x - 20.0f) : cModAppAvgModUdcOpen)







//均充算法切换状态 类型定义
typedef enum 
{
    eModAppAvgStateSteady               = 0,                                    //稳态
    eModAppAvgStateTransient            = 1,                                    //暂态
    
    eModAppAvgStateMax
}eModAppAvgState_t;








//开关状态 结构定义
typedef struct
{
    u8                                  bA : 1;                                 //A枪输出  状态
    u8                                  bB : 1;                                 //B枪输出  状态
    u8                                  bY : 1;                                 //Y  接触器状态
    u8                                  bX : 1;                                 //X  接触器状态----均充硬件下无X/Z接触器,控制信号需要给常闭以便其（与逻辑）硬件保护单元可以起作用
    u8                                  bZ : 1;                                 //Z  接触器状态----由于常闭合,因此放在下面定义，不参与矩阵值计算
    u8                                  bR : 3;                                 //保留
    
}stModAppAvgSwitchStatus_t;

typedef union
{
    stModAppAvgSwitchStatus_t           stValue;                                //结构体访问
    u8                                  u8Value;                                //8位访问
}unModAppAvgSwitchStatus_t;

//开关状态 掩码---仅保留 bA & bB & bY 的值
#define cModAppAvgSwitchStatusMask      (0x07)










//开关状态切换操作矩阵 结构定义
typedef struct
{
    u8                                  u8Status;                               //当前开关状态
    ePileGunIndex_t                     eGunIndexYl;                            //当前开关状态下Y接触器左侧(A枪侧)模块所属的枪号
    ePileGunIndex_t                     eGunIndexYr;                            //当前开关状态下Y接触器右侧(B枪侧)模块所属的枪号
    
}stModAppAvgSwitchMatrix_t;











//枪的属性 结构定义
typedef struct
{
    u32                                 u32ModNum;                              //有效模块的个数
    
    f32                                 f32Udc;                                 //输出电压实际值
    f32                                 f32Idc;                                 //输出电流实际值
    
    f32                                 f32UdcSet;                              //输出电压设定值
    f32                                 f32IdcSet;                              //输出电流设定值---是总的需求电流,不会按模块平分
    
}stModAppAvgAttrGun_t;










//接触器的属性 结构定义
typedef struct
{
    const u32                           *pAddrL;                                //接触器靠近左侧(A枪一侧)的模块地址列表
    ePileGunIndex_t                     eGunIndexL;                             //这一批模块当前所属的枪序号
    
    const u32                           *pAddrR;                                //接触器靠近右侧(B枪一侧)的模块地址列表
    ePileGunIndex_t                     eGunIndexR;                             //这一批模块当前所属的枪序号
    
    u8                                  u8AddrNum;                              //列表内地址的个数
}stModAppAvgAttrContactor_t;






//模块的属性 结构定义
typedef struct
{
    u32                                 u32Addr;                                //地址
    u32                                 u32Group;                               //组号实际值
    u32                                 u32GroupSet;                            //组号设定值
    ePileGunIndex_t                     eGunIndex;                              //所属的枪序号
    
    f32                                 f32Udc;                                 //输出电压实际值
    f32                                 f32Idc;                                 //输出电流实际值
    f32                                 f32Tenv;                                //此模块面板温度---环温
    
    f32                                 f32UdcSet;                              //输出电压设定值
    f32                                 f32IdcSet;                              //输出电流设定值
    bool                                bSwichFlag;                             //此模块切换标志---置1以后它的 f32UdcSet/f32IdcSet 值不跟踪所属枪的设定值, 由切换逻辑函数赋值
    
    f32                                 f32UdcMax;                              //最大输出电压
    f32                                 f32UdcMin;                              //最小输出电压
    f32                                 f32IdcMax;                              //最大输出电流
    f32                                 f32PdcMax;                              //最大输出功率
    u8                                  u8StatusCode[6];                        //模块故障码
    
}stModAppAvgAttrModule_t;









//模块投切以后电流的起点---新投入的模块限流点都给此值，后续再逐步调整
#define cModAppAvgIdcStart              (0.1f)




















//模块电源管理用户代码 均充逻辑缓存数据
typedef struct
{
    i32                                 i32ModNum;                              //模块（插槽）数量，实际上可能没插足额的模块数量也允许运行
    eIo_t                               eIoAcContactor;                         //交流接触器驱动
    
    stModAppAvgAttrGun_t                stAttrGun[ePileGunIndexNum];            //枪的属性
    stModAppAvgAttrModule_t             stAttrModule[cModAppAvgModNumMax];      //模块的属性
    stModAppAvgAttrContactor_t          stAttrContactorY;                       //Y接触器的属性
    
    unModAppAvgSwitchStatus_t           unStatusDemand;                         //需求的开关状态
    unModAppAvgSwitchStatus_t           unStatusActual;                         //实际的开关状态
    u32                                 u32StatusTimestamp;                     //此状态的时间戳，一些需要延时切换的动作函数需要此时间戳
    SemaphoreHandle_t                   xSemStatus;                             //Status访问（修改）信号量
    
}stModAppAvgCache_t;












//枪序号生成对应的组号----用于组控功能的组号生成
//生成逻辑：组号 = 枪序号 + 1
//因为枪序号从0开始，所有模块的默认组号是0，要避免使用该默认组号
#define sModAppAvgIndexToGroup(index)   (index + 1)













extern bool sModAppAvgInit(i32 i32ModNum, eModType_t eType, eIo_t eIoAcContactor);
extern bool sModAppAvgInitMod(void);

//对外的api接口函数
extern bool sModAppAvgOpen(ePileGunIndex_t eGunIndex);
extern bool sModAppAvgClose(ePileGunIndex_t eGunIndex);
extern bool sModAppAvgCloseContactor(void);
extern bool sModAppAvgSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
extern bool sModAppAvgGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
extern bool sModAppAvgGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
extern bool sModAppAvgGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
extern bool sModAppAvgGetIdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
extern bool sModAppAvgGetAttrMod(i32 i32Index, stModAppAvgAttrModule_t **pMod);

extern void sModAppAvgAttrModUpdateGunIndex(void);


#endif

















