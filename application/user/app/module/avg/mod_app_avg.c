/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   mod_app_avg.c
* Description                           :   模块电源管理用户代码 之 均充算法实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-03-25
* notice                                :   
*                                           均充逻辑主要定义：
*                                           1：枪数只能为2
*                                           2：模块数必须是偶数，
*                                           3：模块地址必须按u32ModAppAvgAddrYl/u32ModAppAvgAddrYr配置来设置 不接受其他地址
*
*                                           安装必须确保以上前置条件，才能保证算法正常运行
****************************************************************************************************/
#include "mod_app_avg.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mod_app_avg";




stModAppAvgCache_t *pModAppAvgCache = NULL;


extern const u32 u32ModAppAvgAddrYl[];                                          //均充算法:接触器Y 靠近左侧(A枪一侧)的模块地址列表
extern const u32 u32ModAppAvgAddrYr[];                                          //均充算法:接触器Y 靠近右侧(B枪一侧)的模块地址列表


//---------------------------------------------------------------------------------------------------------
bool sModAppAvgInit(i32 i32ModNum, eModType_t eType, eIo_t eIoAcContactor);
bool sModAppAvgInitMod(void);
void sModAppAvgTask(void *pParam);


//对外的api接口函数
bool sModAppAvgOpen(ePileGunIndex_t eGunIndex);
bool sModAppAvgClose(ePileGunIndex_t eGunIndex);
bool sModAppAvgCloseContactor(void);
void sModAppAvgOpenContactor(void);
bool sModAppAvgSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
bool sModAppAvgGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppAvgGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
bool sModAppAvgGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppAvgGetIdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppAvgGetAttrMod(i32 i32Index, stModAppAvgAttrModule_t **pMod);






void sModAppAvgSwitch (void);
void sModAppAvgSwitchA(void);
void sModAppAvgSwitchB(void);
void sModAppAvgSwitchY(void);



void sModAppAvgSwitchProtect(void);
void sModAppAvgAttrGunUpdate(void);
void sModAppAvgAttrModUpdate(void);
void sModAppAvgAttrModUpdateGunIndex(void);
void sModAppAvgAttrModUpdateUdcIdcSet(void);
bool sModAppAvgAttrModAddrCheck(u32 u32Addr);
void sModAppAvgAttrContactorYUpdate(ePileGunIndex_t eGunIndexL, ePileGunIndex_t eGunIndexR);
u8   sModAppAvgCalcYba(unModAppAvgSwitchStatus_t unStatus);







//切换操作
void sModAppAvgSwitchOn  (ePileGunIndex_t eGunIndex);
void sModAppAvgSwitchOff (ePileGunIndex_t eGunIndex);
void sModAppAvgSwitchIn  (ePileGunIndex_t eGunIndex);
void sModAppAvgSwitchOut (ePileGunIndex_t eGunIndex);
void sModAppAvgSwitchOffY(void);







//稳态下的切换操作矩阵---请参考<均充模块投切时序及矩阵表.xls>
stModAppAvgSwitchMatrix_t stModAppAvgSwitchMatrix[] = 
{
    {0,     (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),       },
    {1,     (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),       },
    {2,     (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),       },
    {3,     (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),       },
    {4,     (ePileGunIndex0 + 2),   (ePileGunIndex0 + 2),       },//YBA=4:A枪未充电，B枪未充电，Y闭合, 模块不属于任何枪
    {5,     (ePileGunIndex0 + 0),   (ePileGunIndex0 + 0),       },//YBA=5:A枪在充电，B枪未充电，Y闭合, 模块全部属于A枪
    {6,     (ePileGunIndex0 + 1),   (ePileGunIndex0 + 1),       },//YBA=6:A枪未充电，B枪在充电，Y闭合, 模块全部属于B枪
    
};
















/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充逻辑资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-03-25
* notice                                :   i32ModNum:硬件支持的模块数量---可以理解为模块插槽的数量
****************************************************************************************************/
bool sModAppAvgInit(i32 i32ModNum, eModType_t eType, eIo_t eIoAcContactor)
{
    i32  i;
    
    //参数限幅
    if((i32ModNum > cModAppAvgModNumMax) || (i32ModNum < 0) || ((i32ModNum % ePileGunIndexNum) != 0))
    {
        EN_SLOGI(TAG, "模块电源管理用户程序, 均充逻辑资源初始化失败, 输入模块数量:%d, 不是枪数量%d的倍数或超上限:%d", i32ModNum, ePileGunIndexNum, cModAppAvgModNumMax);
        return(false);
    }
    
    
    //cache内存申请及初始化
    pModAppAvgCache = (stModAppAvgCache_t *)MALLOC(sizeof(stModAppAvgCache_t));
    if(pModAppAvgCache == NULL)
    {
        EN_SLOGI(TAG, "模块电源管理用户程序, 均充逻辑资源初始化失败, 内存申请失败");
        return(false);
    }
    memset(pModAppAvgCache, 0,  sizeof(stModAppAvgCache_t));
    pModAppAvgCache->i32ModNum  = i32ModNum;
    pModAppAvgCache->eIoAcContactor  = eIoAcContactor;
    pModAppAvgCache->stAttrContactorY.pAddrL = u32ModAppAvgAddrYl;
    pModAppAvgCache->stAttrContactorY.pAddrR = u32ModAppAvgAddrYr;
    pModAppAvgCache->stAttrContactorY.eGunIndexL = ePileGunIndex0;
    pModAppAvgCache->stAttrContactorY.eGunIndexR = ePileGunIndex0 + 1;
    pModAppAvgCache->stAttrContactorY.u8AddrNum  = cModAppAvgModNumMax >> 1;
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        //先都给无效值
        pModAppAvgCache->stAttrModule[i].u32Addr = cModAppAvgModAddrInvalid;
    }
    
    pModAppAvgCache->xSemStatus = xSemaphoreCreateBinary();
    xSemaphoreGive(pModAppAvgCache->xSemStatus);
    
    //模块电源驱动程序初始化
    sModDrvOptInit(eType);
    
    //均充算法下虽然没有XZ接触器，但为了硬件兼容，两个驱动信号需要给常闭---否则硬件驱动保护逻辑无法被触发
    sIoSetDrXyz('X', true);
    sIoSetDrXyz('Z', true);
    
    //创建任务
    xTaskCreate(sModAppAvgTask, "sModAppAvgTask", (512), NULL, 24, NULL);
    
    return true;
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法 模块配置初始化
* Author                                :   Hall
* Creat Date                            :   2024-10-18
* notice                                :   
****************************************************************************************************/
bool sModAppAvgInitMod(void)
{
    i32  i, m;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
        {
            if((pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrL[m])        //只接受配置好的地址
            || (pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrR[m]))
            {
                //初始化时将该模块地址模式设置为 拨码设定（默认），防止有的模块不是该默认模式
                //因为地址模式设置为 拨码设定 以后，模块才允许指令设置组号
                bRst = sModDrvOptSetAddrMod((u8)pCache->stAttrModule[i].u32Addr, eModDrvWinlineAddrModeSwitch);
            }
        }
    }
    
    return bRst;
}









/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法 主任务
* Author                                :   Hall
* Creat Date                            :   2024-09-14
* notice                                :   
****************************************************************************************************/
void sModAppAvgTask(void *pParam)
{
    ePileGunIndex_t eGunIndex = 0;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    vTaskDelay(8000 / portTICK_RATE_MS);
    EN_SLOGI(TAG, "任务建立:模块电源管理用户程序,均充算法主任务");
    
    //先更新一下模块的属性
    sModAppAvgAttrModUpdate();
    sModAppAvgAttrModUpdateGunIndex();
    while(1)
    {
        sModAppAvgAttrModUpdateUdcIdcSet();
        sModAppAvgAttrModUpdate();
        sModAppAvgAttrGunUpdate();
        sModAppAvgSwitch();
        sModAppAvgOpenContactor();
        
        //切换保护函数
        sModAppAvgSwitchProtect();
        
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 开启枪输出
* Author                                :   Hall
* Creat Date                            :   2024-03-27
* notice                                :   
****************************************************************************************************/
bool sModAppAvgOpen(ePileGunIndex_t eGunIndex)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    pCache->unStatusDemand.u8Value |= (1 << eGunIndex);
    
    return true;
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 关闭枪输出
* Author                                :   Hall
* Creat Date                            :   2024-03-27
* notice                                :   
****************************************************************************************************/
bool sModAppAvgClose(ePileGunIndex_t eGunIndex)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    pCache->unStatusDemand.u8Value &= (~(1 << eGunIndex));
    
    return true;
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 切断接触器
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   对于均充算法来说, 可以切断的接触器只有Y
*
*                                           硬件对接触器:Ka、X、Y、Z、Kb 的驱动有保护逻辑---均充硬件下X/Z不存在
*                                           当A或B单枪充电且Y闭合的情况下 另外一把枪要启动充电时
*                                           外部用户程序需要先调用本函数切断继电器Y，
*                                           否则硬件保护电路会锁定该枪的接触器驱动信号避免直通，
*
*                                           这种锁定会触发直流接触器K1/K2故障
****************************************************************************************************/
bool sModAppAvgCloseContactor(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "User程序断开Y接触器 当前状态 %d" , pCache->unStatusActual.u8Value);
    switch(sModAppAvgCalcYba(pCache->unStatusActual))
    {
    case 1:
    case 2:
        pCache->u32StatusTimestamp = sGetTimestamp();                           //在这俩状态下有可能即将闭合Y接触器,需要让程序重新计时避免闭合
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态1或2, Y接触器闭合计时器重新开始计时");
        break;
    case 5:                                                                     //A枪充电   B枪未充电 Y闭合
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态5, 切出B枪所属模块");
        while(pCache->unStatusActual.stValue.bY == 1)                           //执行切出动作直到 Y断开
        {
            sModAppAvgSwitchOut(ePileGunIndex0 + 1);
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态5, 切出B枪所属模块 断开接触器成功");
        break;
    case 6:                                                                     //A枪未充电 B枪充电   Y闭合
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态6, 切出A枪所属模块");
        while(pCache->unStatusActual.stValue.bY == 1)                           //执行切出动作直到 Y断开
        {
            sModAppAvgSwitchOut(ePileGunIndex0);
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态6, 切出A枪所属模块 断开接触器成功");
        break;
    case 4:                                                                     //A枪未充电 B枪未充电 Y闭合---不用处理
    case 7:                                                                     //A枪充电   B枪充电   Y闭合---不可能存在这个状态
    default:                                                                    //0,1,2,3,  Y本来就是断开的---不用处理
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态无需操作");
        break;
    }
    
    
    return true;
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法---投切前准备
* Author                                :   Hall
* Creat Date                            :   2024-12-09
* notice                                :   
****************************************************************************************************/
void sModAppAvgOpenContactor(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    static u32 u32StartTimeStamp = 0;
    u32 u32AvgTimeSwitchIn = 0;
    
    u32AvgTimeSwitchIn = (sEepromAppGetBlockFactoryAuthenFlag()) ? 6 : cModAppAvgTimeSwitchIn;
    switch(sModAppAvgCalcYba(pCache->unStatusActual))
    {
    case 1:
        if((sGetTimestamp() - pCache->u32StatusTimestamp) >= u32AvgTimeSwitchIn)
        {
            sModAppAvgSwitchIn(ePileGunIndex1);
        }
        else
        {
            if((sGetTimestamp() - u32StartTimeStamp) >= 5)
            {
                u32StartTimeStamp = sGetTimestamp();
                EN_SLOGI(TAG, "模块均充算法投切动作:等待%d秒后执行切入B枪模块", u32AvgTimeSwitchIn - (sGetTimestamp() - pCache->u32StatusTimestamp));
            }
        }
        break;
    case 2:
        if((sGetTimestamp() - pCache->u32StatusTimestamp) >= u32AvgTimeSwitchIn)
        {
            sModAppAvgSwitchIn(ePileGunIndex0);
        }
        else
        {
            if((sGetTimestamp() - u32StartTimeStamp) >= 5)
            {
                u32StartTimeStamp = sGetTimestamp();
                EN_SLOGI(TAG, "模块均充算法投切动作:等待%d秒后执行切入A枪模块", u32AvgTimeSwitchIn - (sGetTimestamp() - pCache->u32StatusTimestamp));
            }
        }
        break;
    default:
        break;
    }
    
}









/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 设置枪输出
* Author                                :   Hall
* Creat Date                            :   2024-03-27
* notice                                :   
****************************************************************************************************/
bool sModAppAvgSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    pCache->stAttrGun[eGunIndex].f32UdcSet = f32Udc;
    pCache->stAttrGun[eGunIndex].f32IdcSet = f32IdcLimit;
    
    
    return true;
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 获取枪x最大输出电压
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   
****************************************************************************************************/
bool sModAppAvgGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc)
{
    f32  f32UdcMax = 0;
    f32  f32IdcMax = 0;
    
    if((pUdc != NULL) && (sModDrvOptGetCurve(&f32UdcMax, NULL, &f32IdcMax, NULL) == true))
    {
        *pUdc = f32UdcMax;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 获取枪x最大输出电流
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   最大硬件配置下的最大输出电流，而不是当前硬件配置下的最大输出电流
****************************************************************************************************/
bool sModAppAvgGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc)
{
    f32  f32UdcMax = 0;
    f32  f32IdcMax = 0;
    
    if((pIdc != NULL) && (sModDrvOptGetCurve(&f32UdcMax, NULL, &f32IdcMax, NULL) == true))
    {
        *pIdc = f32IdcMax * cModAppAvgModNumMax;
        
        return true;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 获取枪x当前实际输出电压
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   
****************************************************************************************************/
bool sModAppAvgGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc)
{
    i32  i;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pUdc != NULL)
    {
        *pUdc = pCache->stAttrGun[eGunIndex].f32Udc;
        
        return true;
    }
    
    return false;
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 获取模块的属性
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   i32Index:模块的排序序号，取值范围:0 ~ cModAppAvgModNumMax
*
*                                           用户程序可以调用本函数遍历获取 0 ~ cModAppAvgModNumMax 之
*                                           间的所有模块属性用于显示获通讯
****************************************************************************************************/
bool sModAppAvgGetAttrMod(i32 i32Index, stModAppAvgAttrModule_t **pMod)
{
    i32  m;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if((pMod != NULL) && (i32Index >= 0) && (i32Index < cModAppAvgModNumMax))
    {
        for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
        {
            //该序号对应的模块地址在配置的地址表中 才返回其属性
            if((pCache->stAttrModule[i32Index].u32Addr == pCache->stAttrContactorY.pAddrL[m])
            || (pCache->stAttrModule[i32Index].u32Addr == pCache->stAttrContactorY.pAddrR[m]))
            {
                *pMod = &pCache->stAttrModule[i32Index];
                
                return true;
            }
        }
    }
    
    return false;
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 api接口函数 之 获取枪x当前模块总输出电流
* Author                                :   HH
* Creat Date                            :   2024-09-23
* notice                                :   
****************************************************************************************************/
bool sModAppAvgGetIdcNow(ePileGunIndex_t eGunIndex, i32 *pIdc)
{
    i32  i;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(pIdc != NULL)
    {
        *pIdc = pCache->stAttrGun[eGunIndex].f32Idc;
        
        return true;
    }
    
    return false;
}


































/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->投切函数
* Author                                :   Hall
* Creat Date                            :   2024-09-18
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitch(void)
{
    //A枪
    sModAppAvgSwitchA();
    
    //B枪
    sModAppAvgSwitchB();
    
    //Y接触器
    sModAppAvgSwitchY();
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->投切函数 之 A枪投切
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchA(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(pCache->unStatusActual.stValue.bA != pCache->unStatusDemand.stValue.bA)
    {
        if(pCache->unStatusActual.stValue.bA == 1)
        {
            sModAppAvgSwitchOff(ePileGunIndex0);
        }
        else
        {
            sModAppAvgSwitchOn(ePileGunIndex0);
        }
    }
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->投切函数 之 B枪投切
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchB(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(pCache->unStatusActual.stValue.bB != pCache->unStatusDemand.stValue.bB)
    {
        if(pCache->unStatusActual.stValue.bB == 1)
        {
            sModAppAvgSwitchOff(ePileGunIndex1);
        }
        else
        {
            sModAppAvgSwitchOn(ePileGunIndex1);
        }
    }
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->投切函数 之 Y接触器投切
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchY(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    if(pCache->unStatusActual.stValue.bY != pCache->unStatusDemand.stValue.bY)
    {
        switch(sModAppAvgCalcYba(pCache->unStatusActual))
        {
        case 0:                                                                 //A枪未充电，B枪未充电，Y断开--->Y要闭合
            //不可能有这种切换需求
            break;
        case 1:                                                                 //A枪在充电，B枪未充电，Y断开--->Y要闭合
            sModAppAvgSwitchIn(ePileGunIndex1);                                 //切入B枪所属模块
            break;
        case 2:                                                                 //A枪未充电，B枪在充电，Y断开--->Y要闭合
            sModAppAvgSwitchIn(ePileGunIndex0);                                 //切入A枪所属模块
            break;
        case 3:                                                                 //A枪在充电，B枪在充电，Y断开--->Y要闭合
            //不可能有这种切换需求
            break;
        case 4:                                                                 //A枪未充电，B枪未充电，Y闭合--->Y要断开
            sModAppAvgSwitchOffY();                                             //直接断开Y接触器
            break;
        case 5:                                                                 //A枪在充电，B枪未充电，Y闭合--->Y要断开
        case 6:                                                                 //A枪未充电，B枪在充电，Y闭合--->Y要断开
            //sModAppAvgSwitchOut(ePileGunIndex0/1);
            //由于用户程序会先调用 sModAppAvgCloseContactor 函数执行切出操作
            //所以实际上，对这两种切换需求，无需执行切出操作
            break;
        default:
            break;
        }
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->切换保护逻辑
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   潜在的可能性:模块已经开启输出，但 YBA 实际值由于bug未更新
*                                           需要对这种情况做出保护。
****************************************************************************************************/
void sModAppAvgSwitchProtect(void)
{
    i32  i;
    bool bFlagA = false;                                                        //A枪下模块非法输出标志
    bool bFlagB = false;                                                        //
    
    static u32 u32TimeA = 0;                                                    //B枪下模块非法输出标志置1的起始时间戳
    static u32 u32TimeB = 0;                                                    //
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if(pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        {
            if((pCache->unStatusActual.stValue.bA == 0)                         //A枪无输出 但枪下模块有输出
            && (pCache->stAttrModule[i].f32Udc >= sModAppAvgGetModUdcOpen(pCache->stAttrModule[i].f32UdcMin)))
            {
                bFlagA = true;
            }
        }
        else if(pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        {
            if((pCache->unStatusActual.stValue.bB == 0)                         //B枪无输出 但枪下模块有输出
            && (pCache->stAttrModule[i].f32Udc >= sModAppAvgGetModUdcOpen(pCache->stAttrModule[i].f32UdcMin)))
            {
                bFlagB = true;
            }
        }
    }
    
    
    if(bFlagA == true)
    {
        if(((sGetTimestamp() - u32TimeA)) >= 3)
        {
            //A枪下模块非法输出标志为1且持续时间超过3秒---关闭A枪下所有模块的输出
            for(i = 0; i < cModAppAvgModNumMax; i++)
            {
                if(pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
                {
                    EN_SLOGI(TAG, "均充算法之投切保护逻辑:关闭A枪下模块(地址:%d)", pCache->stAttrModule[i].u32Addr);
                    sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
                }
            }
        }
    }
    else
    {
        //A枪下模块非法输出标志为0---持续更新此时间戳
        u32TimeA = sGetTimestamp();
    }
    
    
    if(bFlagB == true)
    {
        if(((sGetTimestamp() - u32TimeB)) >= 3)
        {
            //B枪下模块非法输出标志为1且持续时间超过3秒---关闭B枪下所有模块的输出
            for(i = 0; i < cModAppAvgModNumMax; i++)
            {
                if(pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
                {
                    EN_SLOGI(TAG, "均充算法之投切保护逻辑:关闭B枪下模块(地址:%d)", pCache->stAttrModule[i].u32Addr);
                    sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
                }
            }
        }
    }
    else
    {
        //B枪下模块非法输出标志为0---持续更新此时间戳
        u32TimeB = sGetTimestamp();
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->枪属性更新
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   
****************************************************************************************************/
void sModAppAvgAttrGunUpdate(void)
{
    i32  i, m;
    f32  f32Udc = 0;
    f32  f32Idc = 0;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        f32Udc = 0.0f;
        f32Idc = 0.0f;
        for(m = 0; m < cModAppAvgModNumMax; m++)
        {
            if(pCache->stAttrModule[m].eGunIndex == i)
            {
                f32Idc = f32Idc + pCache->stAttrModule[m].f32Idc;               //枪输出电流实际值需要累加枪下所有模块电流
                if(f32Udc < pCache->stAttrModule[m].f32Udc)
                {
                    f32Udc = pCache->stAttrModule[m].f32Udc;                    //枪输出电压实际值取枪下所有模块最大值即可
                }
            }
        }
        
        pCache->stAttrGun[i].f32Udc = f32Udc;
        pCache->stAttrGun[i].f32Idc = f32Idc;
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->模块属性更新
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   
****************************************************************************************************/
void sModAppAvgAttrModUpdate(void)
{
    i32  i, m;
    i8   i8AddrList[cModAppAvgModNumMax];                                       //在线模块地址列表
    
    stModAppAvgAttrModule_t *pMod = NULL;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    memset(i8AddrList, cModAppAvgModAddrInvalid, sizeof(i8AddrList));           //先全部设置无效值
    if(sModDrvOptGetOnlineAddrList(sizeof(i8AddrList), i8AddrList) == true)     //获取在线模块地址列表
    {
        for(i = 0; i < sizeof(i8AddrList); i++)
        {
            for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
            {
                if((i8AddrList[i] == pCache->stAttrContactorY.pAddrL[m])        //只接受配置好的地址
                || (i8AddrList[i] == pCache->stAttrContactorY.pAddrR[m]))
                {
                    pMod = &pCache->stAttrModule[i];
                    pMod->u32Addr = i8AddrList[i];
                    sModDrvOptGetOutput  (pMod->u32Addr, &pMod->f32Udc, &pMod->f32Idc);
                    sModDrvOptGetCurve   (&pMod->f32UdcMax, &pMod->f32UdcMin, &pMod->f32IdcMax, &pMod->f32PdcMax);
                    sModDrvOptGetGroupNum(pMod->u32Addr, &pMod->u32Group);
                    
                    if(pMod->u32Group != pMod->u32GroupSet)
                    {
                        //组号实际值不符合设定值时更新模块组号
                        sModDrvOptSetGroupNum(pMod->u32Addr, pMod->u32GroupSet);
                    }
                    
                    if((pMod->f32IdcSet <= pMod->f32IdcMax) && (pMod->f32UdcSet <= pMod->f32UdcMax) && (pMod->f32UdcSet >= pMod->f32UdcMin))
                    {
                        //有效的输出参数才设定, 超范围的无效参数模块不会接受
                        sModDrvOptSetOutput(pMod->u32Addr, false, pMod->f32UdcSet, pMod->f32IdcSet);
                    }
                    
                    //更新模块温度
                    sModDrvOptGetTenv(pMod->u32Addr, &pMod->f32Tenv);
                    //更新模块故障码
                    sModDrvOptGetErrCode(pMod->u32Addr, 4, pMod->u8StatusCode);
                }
            }
        }
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->模块属性eGunIndex字段更新
* Author                                :   Hall
* Creat Date                            :   2024-09-21
* notice                                :   
****************************************************************************************************/
void sModAppAvgAttrModUpdateGunIndex(void)
{
    i32  i, m;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    //从矩阵中搜索当前开关状态
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrix); i++)
    {
        if(stModAppAvgSwitchMatrix[i].u8Status == sModAppAvgCalcYba(pCache->unStatusActual))
        {
            //搜索到状态以后 更新 Y接触器属性---两侧模块所属的枪序号
            sModAppAvgAttrContactorYUpdate(stModAppAvgSwitchMatrix[i].eGunIndexYl, stModAppAvgSwitchMatrix[i].eGunIndexYr);
            break;
        }
    }
    
    
    //开始更新模块属性 同时更新模块的组号
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrL[m])
            {
                pCache->stAttrModule[i].eGunIndex   = pCache->stAttrContactorY.eGunIndexL;
                pCache->stAttrModule[i].u32GroupSet = sModAppAvgIndexToGroup(pCache->stAttrModule[i].eGunIndex);
            }
            else if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrR[m])
            {
                pCache->stAttrModule[i].eGunIndex   = pCache->stAttrContactorY.eGunIndexR;
                pCache->stAttrModule[i].u32GroupSet = sModAppAvgIndexToGroup(pCache->stAttrModule[i].eGunIndex);
            }
        }
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->模块属性 f32UdcSet/f32IdcSet 字段更新
* Author                                :   Hall
* Creat Date                            :   2024-09-21
* notice                                :   
****************************************************************************************************/
void sModAppAvgAttrModUpdateUdcIdcSet(void)
{
    i32  i, j;
    f32  f32IdcSetTemp;
    u32  u32ModNum = 0;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        u32ModNum = 0;
        for(j = 0; j < cModAppAvgModNumMax; j++)
        {
            if(pCache->stAttrModule[j].eGunIndex == i)
            {
                if(pCache->stAttrModule[j].f32Udc > sModAppAvgGetModUdcOpen(pCache->stAttrModule[j].f32UdcMin))
                {
                    //已开机的模块计数
                    u32ModNum++;
                }
                
                if(pCache->stAttrModule[j].bSwichFlag != true)
                {
                    //f32UdcSet 值的计算 无需考虑其是否能开机
                    if(pCache->stAttrGun[i].f32UdcSet > pCache->stAttrModule[j].f32UdcMax)
                    {
                        pCache->stAttrModule[j].f32UdcSet = pCache->stAttrModule[j].f32UdcMax;
                    }
                    else if(pCache->stAttrGun[i].f32UdcSet < pCache->stAttrModule[j].f32UdcMin)
                    {
                        pCache->stAttrModule[j].f32UdcSet = pCache->stAttrModule[j].f32UdcMin;
                    }
                    else
                    {
                        pCache->stAttrModule[j].f32UdcSet = pCache->stAttrGun[i].f32UdcSet;
                    }
                }
            }
        }
        
        if(u32ModNum > 0)
        {
            pCache->stAttrGun[i].u32ModNum = u32ModNum;
            for(j = 0; j < cModAppAvgModNumMax; j++)
            {
                if(pCache->stAttrModule[j].eGunIndex == i)
                {
                    if(pCache->stAttrModule[j].bSwichFlag != true)
                    {
                        //f32IdcSet 的计算必须考虑有效模块的个数:u32ModNum
                        f32IdcSetTemp = pCache->stAttrGun[i].f32IdcSet / pCache->stAttrGun[i].u32ModNum;
                        if(f32IdcSetTemp > pCache->stAttrModule[j].f32IdcMax)
                        {
                            f32IdcSetTemp = pCache->stAttrModule[j].f32IdcMax;
                        }
                        if(f32IdcSetTemp > (pCache->stAttrModule[j].f32PdcMax / pCache->stAttrModule[j].f32UdcSet))
                        {
                            f32IdcSetTemp = pCache->stAttrModule[j].f32PdcMax / pCache->stAttrModule[j].f32UdcSet;
                        }
                        pCache->stAttrModule[j].f32IdcSet = f32IdcSetTemp;
                    }
                }
            }
        }
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->模块属性 u32Addr 字段检查
* Author                                :   Hall
* Creat Date                            :   2024-09-25
* notice                                :   遍历整个配置表 判断输入地址是否存在
****************************************************************************************************/
bool sModAppAvgAttrModAddrCheck(u32 u32Addr)
{
    i32  i;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    //均充算法只有Y接触器, 它两侧的模块就是全部模块了
    for(i = 0; i < pCache->stAttrContactorY.u8AddrNum; i++)
    {
        if((u32Addr == pCache->stAttrContactorY.pAddrL[i])
        || (u32Addr == pCache->stAttrContactorY.pAddrR[i]))
        {
            return true;
        }
    }
    
    return false;
}









/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->Y接触器属性更新
* Author                                :   Hall
* Creat Date                            :   2024-09-23
* notice                                :   
****************************************************************************************************/
void sModAppAvgAttrContactorYUpdate(ePileGunIndex_t eGunIndexL, ePileGunIndex_t eGunIndexR)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    pCache->stAttrContactorY.eGunIndexL = eGunIndexL;
    pCache->stAttrContactorY.eGunIndexR = eGunIndexR;
}









/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->计算YBA值
* Author                                :   Hall
* Creat Date                            :   2024-09-18
* notice                                :   需要用掩码屏蔽无关位
****************************************************************************************************/
u8 sModAppAvgCalcYba(unModAppAvgSwitchStatus_t unStatus)
{
    return (unStatus.u8Value & cModAppAvgSwitchStatusMask);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法动作函数--->开启x枪所属模块
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchOn(ePileGunIndex_t eGunIndex)
{
    i32  i;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:开启%s枪所属模块 当前枪电压: %0.2f", (eGunIndex == ePileGunIndex0) ? "A" : "B", pCache->stAttrGun[eGunIndex].f32Udc);
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == eGunIndex)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst | (pCache->stAttrModule[i].f32Udc > sModAppAvgGetModUdcOpen(pCache->stAttrModule[i].f32UdcMin));//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
        }
    }
    
    
    if(bRst == true)
    {
        pCache->unStatusActual.u8Value |= (1 << eGunIndex);
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法动作函数--->关闭x枪所属模块
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchOff(ePileGunIndex_t eGunIndex)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:关闭%s枪所属模块 当前枪电压:%0.2f,", (eGunIndex == ePileGunIndex0) ? "A" : "B" , pCache->stAttrGun[eGunIndex].f32Udc);
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == eGunIndex)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.u8Value &= (~(1 << eGunIndex));
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法动作函数--->切入x枪所属模块(给对侧枪使用)
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchIn(ePileGunIndex_t eGunIndex)
{
    i32  i, m;
    bool bRst = false;
    bool bUdcDifferenceFlag = false;                                            //电压差值OK标志
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    EN_SLOGI(TAG, "模块均充算法投切动作:切入%s枪所属模块---给对侧枪使用", (eGunIndex == ePileGunIndex0) ? "A" : "B");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(pCache->stAttrModule[i].u32Addr == ((eGunIndex == ePileGunIndex0) ? pCache->stAttrContactorY.pAddrL[m] : pCache->stAttrContactorY.pAddrR[m]))
            {
                //这一组模块现在要切到对侧枪, 先给该枪的实际电压（避免压差）,电流给初值 后续慢慢调整
                pCache->stAttrModule[i].bSwichFlag = true;
                pCache->stAttrModule[i].f32UdcSet = pCache->stAttrGun[(eGunIndex == ePileGunIndex0) ? (ePileGunIndex0 + 1) : ePileGunIndex0].f32Udc;
                pCache->stAttrModule[i].f32IdcSet = cModAppAvgIdcStart;
                
                //这里此模块输出参数变更之后应该先设置参数 下面再执行开机动作更严谨一点
                sModDrvOptSetOutput(pCache->stAttrModule[i].u32Addr, false, pCache->stAttrModule[i].f32UdcSet, pCache->stAttrModule[i].f32IdcSet);
                
                //pAddrL[]/pAddrR[] 的模块是原本属于A/B枪的模块 要全部开启
                sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
                bRst = bRst | (pCache->stAttrModule[i].f32Udc > sModAppAvgGetModUdcOpen(pCache->stAttrModule[i].f32UdcMin));//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
                
                //判断这一组模块输出电压是否达到x枪实际电压
                bUdcDifferenceFlag = bUdcDifferenceFlag | ((fabs(pCache->stAttrModule[i].f32Udc - pCache->stAttrGun[eGunIndex].f32Udc) < 10.0f) ? true : false);
            }
        }
    }
    
    
    if((bRst == true) && (bUdcDifferenceFlag == true))
    {
        if(sIoSetDrXyz('Y', true) == true)
        {
            pCache->unStatusActual.stValue.bY = 1;
            pCache->u32StatusTimestamp = sGetTimestamp();
            sModAppAvgAttrModUpdateGunIndex();                                  //更新模块所属枪序号
            for(i = 0; i < cModAppAvgModNumMax; i++)
            {
                //这里已经完成切换并且模块已经更新了所属枪序号
                //bSwichFlag 标志应该清除 以便模块的输出值去跟着各枪的输出值
                pCache->stAttrModule[i].bSwichFlag = false;
            }
            
        }
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法动作函数--->切出x枪所属模块
* Author                                :   Hall
* Creat Date                            :   2024-12-06
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchOut(ePileGunIndex_t eGunIndex)
{
    i32  i, m;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:切出%s枪所属模块", (eGunIndex == ePileGunIndex0) ? "A" : "B");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            //pAddrL[]/pAddrR[] 的模块是原本属于A/B枪的模块 要全部关闭
            if(pCache->stAttrModule[i].u32Addr == ((eGunIndex == ePileGunIndex0) ? pCache->stAttrContactorY.pAddrL[m] : pCache->stAttrContactorY.pAddrR[m]))
            {
                bMatchFlag = true;
                sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
                bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
            }
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        if(sIoSetDrXyz('Y', false) == true)
        {
            pCache->unStatusActual.stValue.bY = 0;
            pCache->u32StatusTimestamp = sGetTimestamp();
            sModAppAvgAttrModUpdateGunIndex();
        }
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->断开Y接触器
* Author                                :   Hall
* Creat Date                            :   2024-12-09
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitchOffY(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:断开Y接触器");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    if(sIoSetDrXyz('Y', false) == true)
    {
        pCache->unStatusActual.stValue.bY = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    xSemaphoreGive(pCache->xSemStatus);
}










