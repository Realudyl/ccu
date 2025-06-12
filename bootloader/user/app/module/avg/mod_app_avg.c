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
bool sModAppAvgInit(i32 i32ModNum, eIo_t eIoAcContactor);
bool sModAppAvgInitMod(void);
void sModAppAvgTask(void *pParam);


//对外的api接口函数
bool sModAppAvgOpen(ePileGunIndex_t eGunIndex);
bool sModAppAvgClose(ePileGunIndex_t eGunIndex);
bool sModAppAvgCloseContactor(void);
bool sModAppAvgSetOutput(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit);
bool sModAppAvgGetUdcMax(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppAvgGetIdcMax(ePileGunIndex_t eGunIndex, i32 *pIdc);
bool sModAppAvgGetUdcNow(ePileGunIndex_t eGunIndex, i32 *pUdc);
bool sModAppAvgGetAttrMod(i32 i32Index, stModAppAvgAttrModule_t **pMod);






void sModAppAvgSwitch(void);
void sModAppAvgSwitchProtect(void);
void sModAppAvgAttrGunUpdate(void);
void sModAppAvgAttrModUpdate(void);
void sModAppAvgAttrModUpdateGunIndex(void);
void sModAppAvgAttrModUpdateUdcIdcSet(void);
bool sModAppAvgAttrModAddrCheck(u32 u32Addr);
void sModAppAvgAttrContactorYUpdate(ePileGunIndex_t eGunIndexL, ePileGunIndex_t eGunIndexR);
u8   sModAppAvgCalcYba(unModAppAvgSwitchStatus_t unStatus);
u8   sModAppAvgSearchSwitchStatus(ePileGunIndex_t eGunIndex, bool bFlag);







//稳态下的切换操作
//稳态下收到有效的 sModAppAvgOpen/sModAppAvgClose 操作后触发
void sModAppAvgFunOp_025(void);
void sModAppAvgFunOp_086(void);
void sModAppAvgFunOp_345(void);
void sModAppAvgFunOp_316(void);
void sModAppAvgFunOp_510(void);
void sModAppAvgFunOp_583(void);
void sModAppAvgFunOp_640(void);
void sModAppAvgFunOp_623(void);

//暂态下的切换操作
//暂态下收到有效的 sModAppAvgOpen/sModAppAvgClose 操作 或 等待保持时间t超时后 触发
void sModAppAvgFunOp_110(void);
void sModAppAvgFunOp_103(void);
void sModAppAvgFunOp_183(void);
void sModAppAvgFunOp_105(void);
void sModAppAvgFunOp_240(void);
void sModAppAvgFunOp_203(void);
void sModAppAvgFunOp_223(void);
void sModAppAvgFunOp_206(void);
void sModAppAvgFunOp_400(void);
void sModAppAvgFunOp_425(void);
void sModAppAvgFunOp_486(void);





//稳态下的切换操作矩阵---请参考<均充模块投切时序及矩阵表.xls>
stModAppAvgSwitchMatrix_t stModAppAvgSwitchMatrixSteady[] = 
{
    {0,     2,      5,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_025 },
    {0,     8,      6,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_086 },
    {3,     4,      5,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_345 },
    {3,     1,      6,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_316 },
    {5,     1,      0,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 0),   sModAppAvgFunOp_510 },
    {5,     8,      3,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 0),   sModAppAvgFunOp_583 },
    {6,     4,      0,  (ePileGunIndex0 + 1),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_640 },
    {6,     2,      3,  (ePileGunIndex0 + 1),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_623 },
    
};



//暂态下的切换操作矩阵---请参考<均充模块投切时序及矩阵表.xls>
stModAppAvgSwitchMatrix_t stModAppAvgSwitchMatrixTransient[] = 
{
    {1,     1,      0,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_110 },
    {1,     0,      3,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_103 },
    {1,     8,      3,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_183 },
    {1,     0,      5,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_105 },
    {2,     4,      0,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_240 },
    {2,     0,      3,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_203 },
    {2,     2,      3,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_223 },
    {2,     0,      6,  (ePileGunIndex0 + 0),   (ePileGunIndex0 + 1),   sModAppAvgFunOp_206 },
    {4,     0,      0,  (ePileGunIndex0 + 2),   (ePileGunIndex0 + 2),   sModAppAvgFunOp_400 },//4的时候 Y闭合, AB均不充电, 模块不属于任何枪
    {4,     2,      5,  (ePileGunIndex0 + 2),   (ePileGunIndex0 + 2),   sModAppAvgFunOp_425 },
    {4,     8,      6,  (ePileGunIndex0 + 2),   (ePileGunIndex0 + 2),   sModAppAvgFunOp_486 },
    
};
















/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充逻辑资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-03-25
* notice                                :   i32ModNum:硬件支持的模块数量---可以理解为模块插槽的数量
****************************************************************************************************/
bool sModAppAvgInit(i32 i32ModNum, eIo_t eIoAcContactor)
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
    sIoDrvSetOutput(pModAppAvgCache->eIoAcContactor, SET);
    sModDrvOptInit(eModTypeWinline);
    
    
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
    
    //模块配置参数初始化
    sModAppAvgInitMod();
    while(1)
    {
        sModAppAvgAttrModUpdateUdcIdcSet();
        sModAppAvgAttrModUpdate();
        sModAppAvgAttrGunUpdate();
        if(sModAppAvgCalcYba(pCache->unStatusActual) != sModAppAvgCalcYba(pCache->unStatusDemand))
        {
            //实际的开关状态与期望的开关状态不相等 需要执行切换动作
            EN_SLOGI(TAG, "实际开关状态:%d, 需求开关状态:%d", sModAppAvgCalcYba(pCache->unStatusActual), sModAppAvgCalcYba(pCache->unStatusDemand));
            sModAppAvgSwitch();
        }
        
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
    
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    pCache->unStatusDemand.u8Value = sModAppAvgSearchSwitchStatus(eGunIndex, true);
    xSemaphoreGive(pCache->xSemStatus);
    EN_SLOGI(TAG, "枪%d开启输出, 需求开关状态:%d", eGunIndex, pCache->unStatusDemand.u8Value);
    
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
    
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    pCache->unStatusDemand.u8Value = sModAppAvgSearchSwitchStatus(eGunIndex, false);
    xSemaphoreGive(pCache->xSemStatus);
    EN_SLOGI(TAG, "枪%d关闭输出, 需求开关状态:%d", eGunIndex, pCache->unStatusDemand.u8Value);
    
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
    
    EN_SLOGI(TAG, "User程序断开Y接触器");
    switch(sModAppAvgCalcYba(pCache->unStatusActual))
    {
    case 1:
    case 2:
        pCache->u32StatusTimestamp = sGetTimestamp();                           //在105/206切换逻辑下有可能即将闭合Y接触器,让程序重新计时
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态1或2, Y接触器闭合计时器重新开始计时");
        break;
    case 5:                                                                     //A枪充电   B枪未充电 Y闭合---等效于切换动作583
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态5, 执行583动作断开接触器");
        while(pCache->unStatusActual.stValue.bY == 1)                           //执行583切换动作直到 Y断开
        {
            sModAppAvgFunOp_583();
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态5, 执行583动作断开接触器成功");
        break;
    case 6:                                                                     //A枪未充电 B枪充电   Y闭合---等效于切换动作623
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态6, 执行623动作断开接触器");
        while(pCache->unStatusActual.stValue.bY == 1)                           //执行623切换动作直到 Y断开
        {
            sModAppAvgFunOp_623();
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        EN_SLOGI(TAG, "User程序断开Y接触器:当前状态6, 执行623动作断开接触器成功");
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
* Description                           :   模块电源管理用户程序 之 均充算法--->投切函数
* Author                                :   Hall
* Creat Date                            :   2024-09-18
* notice                                :   
****************************************************************************************************/
void sModAppAvgSwitch(void)
{
    i32  i;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixSteady); i++)
    {
        if((stModAppAvgSwitchMatrixSteady[i].u8Status    == sModAppAvgCalcYba(pCache->unStatusActual))
        && (stModAppAvgSwitchMatrixSteady[i].u8StatusEnd == sModAppAvgCalcYba(pCache->unStatusDemand)))
        {
            stModAppAvgSwitchMatrixSteady[i].pFunOp();
            
            //一轮只执行一次切换动作就跳出, 等待下一轮执行
            return;
        }
    }
    
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixTransient); i++)
    {
        if((stModAppAvgSwitchMatrixTransient[i].u8Status    == sModAppAvgCalcYba(pCache->unStatusActual))
        && (stModAppAvgSwitchMatrixTransient[i].u8StatusEnd == sModAppAvgCalcYba(pCache->unStatusDemand)))
        {
            stModAppAvgSwitchMatrixTransient[i].pFunOp();
            
            //一轮只执行一次切换动作就跳出, 等待下一轮执行
            return;
        }
    }
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->切换保护逻辑
* Author                                :   Hall
* Creat Date                            :   2024-09-29
* notice                                :   极端情况下, 模块开机成功但未能满足条件去更新 YBA 值---假如代码有bug,未能覆盖到那种情况
*                                           这会导致模块有输出但 YBA 的实际值与需求值相同从而切
*                                           换逻辑卡住不变，这很危险，需要识别这种情况并保护，关闭模块输出
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
            if((pCache->unStatusActual.stValue.bA == 0)                         // A枪无输出
            && (pCache->stAttrModule[i].f32Udc >= cModAppAvgModUdcOpen))        //但枪下模块有输出
            {
                bFlagA = true;
            }
        }
        else if(pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        {
            if((pCache->unStatusActual.stValue.bB == 0)                         // B枪无输出
            && (pCache->stAttrModule[i].f32Udc >= cModAppAvgModUdcOpen))        //但枪下模块有输出
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
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixSteady); i++)
    {
        if(stModAppAvgSwitchMatrixSteady[i].u8Status == sModAppAvgCalcYba(pCache->unStatusActual))
        {
            //搜索到状态以后 更新 Y接触器属性---两侧模块所属的枪序号
            sModAppAvgAttrContactorYUpdate(stModAppAvgSwitchMatrixSteady[i].eGunIndexYl, stModAppAvgSwitchMatrixSteady[i].eGunIndexYr);
            goto Update;
        }
    }
    
    //从矩阵中搜索当前开关状态
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixTransient); i++)
    {
        if(stModAppAvgSwitchMatrixTransient[i].u8Status == sModAppAvgCalcYba(pCache->unStatusActual))
        {
            //搜索到状态以后 更新 Y接触器属性---两侧模块所属的枪序号
            sModAppAvgAttrContactorYUpdate(stModAppAvgSwitchMatrixTransient[i].eGunIndexYl, stModAppAvgSwitchMatrixTransient[i].eGunIndexYr);
            goto Update;
        }
    }
    
    //开始更新模块属性 同时更新模块的组号
Update:
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
    u32  u32ModNum = 0;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    for(i = 0; i < ePileGunIndexNum; i++)
    {
        u32ModNum = 0;
        for(j = 0; j < cModAppAvgModNumMax; j++)
        {
            if(pCache->stAttrModule[j].eGunIndex == i)
            {
                if(pCache->stAttrModule[j].f32Udc > cModAppAvgModUdcOpen)
                {
                    //已开机的模块计数
                    u32ModNum++;
                }
                
                if(pCache->stAttrModule[j].bSwichFlag != true)
                {
                    //f32UdcSet 值的计算 无需考虑其是否能开机
                    pCache->stAttrModule[j].f32UdcSet = pCache->stAttrGun[i].f32UdcSet;
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
                        pCache->stAttrModule[j].f32IdcSet = pCache->stAttrGun[i].f32IdcSet / pCache->stAttrGun[i].u32ModNum;
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
* Description                           :   模块电源管理用户程序 之 均充算法--->搜索 u8StatusEnd 字段
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   根据初始的开关状态及外部给的操作指令去搜索
*                                           bFlag : 开启输出---true，关闭输出---false
****************************************************************************************************/
u8 sModAppAvgSearchSwitchStatus(ePileGunIndex_t eGunIndex, bool bFlag)
{
    i32  i;
    u8   u8Status;
    u8   u8OpValue;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    u8Status  = sModAppAvgCalcYba(pCache->unStatusActual);
    u8OpValue = sModAppAvgCalcOpValue(eGunIndex, bFlag);
    
    //分3种情况
    //1:当前开关状态是稳态---稳态下当然随时都可以接受外部操作指令
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixSteady); i++)
    {
        if((stModAppAvgSwitchMatrixSteady[i].u8Status  == u8Status )
        && (stModAppAvgSwitchMatrixSteady[i].u8OpValue == u8OpValue))
        {
            return stModAppAvgSwitchMatrixSteady[i].u8StatusEnd;
        }
    }
    
    
    //2:当前开关状态是暂态---暂态下也允许接受外部操作指令，即刻计算该状态在操作指令作用下最后的稳定状态
    for(i = 0; i < sArraySize(stModAppAvgSwitchMatrixTransient); i++)
    {
        if((stModAppAvgSwitchMatrixTransient[i].u8Status  == u8Status )
        && (stModAppAvgSwitchMatrixTransient[i].u8OpValue == u8OpValue))
        {
            return stModAppAvgSwitchMatrixTransient[i].u8StatusEnd;
        }
    }
    
    
    //3:前面两个矩阵都找不到目标状态
    //说明传入的操作是非法操作----即当前开关状态下不存在的操作，例如AB枪均未充电时收到A或B停充命令
    //这种情况应该不修改开关状态期望值
    return sModAppAvgCalcYba(pCache->unStatusDemand);
}


























/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 025
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:0--->5
*                                           动作：开启A枪下所有模块 如果成功则将实际状态的bA字段置1
****************************************************************************************************/
void sModAppAvgFunOp_025(void)
{
    i32  i;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:025, AB枪均未充电Y断开--->A枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
        }
    }
    
    
    if(bRst == true)
    {
        pCache->unStatusActual.stValue.bA = 1;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}





/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 086
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:0--->6
*                                           动作：开启B枪下所有模块 如果成功则将实际状态的bB字段置1
****************************************************************************************************/
void sModAppAvgFunOp_086(void)
{
    i32  i;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:086, AB枪均未充电Y断开--->B枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
        }
    }
    
    
    if(bRst == true)
    {
        pCache->unStatusActual.stValue.bB = 1;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 345
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:3--->5
*                                           动作：关闭B枪下所有模块 如果成功则将实际状态的bB字段置0
****************************************************************************************************/
void sModAppAvgFunOp_345(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:345, AB枪均在充电Y断开--->A枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bB = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 316
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:3--->6
*                                           动作：关闭A枪下所有模块 如果成功则将实际状态的bA字段置0
****************************************************************************************************/
void sModAppAvgFunOp_316(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:316, AB枪均在充电Y断开--->B枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bA = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 510
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:5--->0
*                                           动作：关闭A枪下所有模块 如果成功则将实际状态的bA字段置0
****************************************************************************************************/
void sModAppAvgFunOp_510(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:510, A枪充电Y闭合--->AB枪均未充电Y闭合");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bA = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 583
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:5--->3
*                                           动作：先关闭原本属于B枪的模块，
*                                                 等待输出电压到0以后断开Y接触器 如果成功则将实际状态的bY字段置0
****************************************************************************************************/
void sModAppAvgFunOp_583(void)
{
    i32  i, m;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:583, A枪充电Y闭合--->A枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            //pAddrR[] 的模块是原本属于B枪的模块 要全部关闭
            if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrR[m])
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
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 640
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:6--->0
*                                           动作：关闭B枪下所有模块 如果成功则将实际状态的bB字段置0
****************************************************************************************************/
void sModAppAvgFunOp_640(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:640, B枪充电Y闭合--->AB枪均未充电Y闭合");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bB = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 623
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:6--->3
*                                           动作：先关闭原本属于A枪的模块
*                                                 等待输出电压到0以后断开Y接触器 如果成功则将实际状态的bY字段置0
****************************************************************************************************/
void sModAppAvgFunOp_623(void)
{
    i32  i, m;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:623, B枪充电Y闭合--->B枪充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            //pAddrL[] 的模块是原本属于A枪的模块 要全部关闭
            if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrL[m])
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
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 110
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:1--->0
*                                           动作：关闭A枪下所有模块 如果成功则将实际状态的bA字段置0
****************************************************************************************************/
void sModAppAvgFunOp_110(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:110, A枪充电Y断开--->AB均未充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bA = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 103
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:1--->3
*                                           动作：开启B枪下所有模块 如果成功则将实际状态的bB字段置1
****************************************************************************************************/
void sModAppAvgFunOp_103(void)
{
    i32  i;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:103, A枪充电Y断开--->AB均在充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
        }
    }
    
    
    if(bRst == true)
    {
        pCache->unStatusActual.stValue.bB = 1;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 183
* Author                                :   Hall
* Creat Date                            :   2024-09-26
* notice                                :   场景：YBA值:1--->3
*                                           A枪充电Y断开--->AB均在充电Y断开,与103等效
****************************************************************************************************/
void sModAppAvgFunOp_183(void)
{
    EN_SLOGI(TAG, "模块均充算法投切动作:183, A枪充电Y断开--->AB均在充电Y断开---此切换等效103,按103执行");
    sModAppAvgFunOp_103();
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 105
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:1--->5
*                                           动作：开启原本属于B枪的模块
*                                                 调整各个模块的电流限值
*                                                 Y接触器两侧的模块输出电压差小于阈值后闭合Y 如果成功则将实际状态的bY字段置1
****************************************************************************************************/
void sModAppAvgFunOp_105(void)
{
    i32  i, m;
    bool bRst = false;
    bool bUdcDifferenceFlag = false;                                            //电压差值OK标志
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    if((sGetTimestamp() - pCache->u32StatusTimestamp) < cModAppAvgTimeSwitchIn)
    {
        //105切换逻辑需要延迟 cModAppAvgTimeSwitchIn 秒才执行 等待本枪输出稳定
        EN_SLOGI(TAG, "模块均充算法投切动作:105, 等待%d秒后执行", cModAppAvgTimeSwitchIn - (sGetTimestamp() - pCache->u32StatusTimestamp));
        return;
    }
    
    EN_SLOGI(TAG, "模块均充算法投切动作:105, A枪充电Y断开--->A枪充电Y闭合");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrR[m])
            {
                //这一组模块现在要切到A枪, 先给A枪的实际电压（避免压差）,电流给初值 后续慢慢调整
                pCache->stAttrModule[i].bSwichFlag = true;
                pCache->stAttrModule[i].f32UdcSet = pCache->stAttrGun[ePileGunIndex0].f32Udc;
                pCache->stAttrModule[i].f32IdcSet = cModAppAvgIdcStart;
                
                //这里此模块输出参数变更之后应该先设置参数 下面再执行开机动作更严谨一点
                sModDrvOptSetOutput(pCache->stAttrModule[i].u32Addr, false, pCache->stAttrModule[i].f32UdcSet, pCache->stAttrModule[i].f32IdcSet);
                
                //pAddrR[] 的模块是原本属于B枪的模块 要全部开启
                sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
                bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
                
                //判断这一组模块输出电压是否达到A枪实际电压
                bUdcDifferenceFlag = bUdcDifferenceFlag | (fabs(pCache->stAttrModule[i].f32Udc - pCache->stAttrGun[ePileGunIndex0].f32Udc) < 10.0f) ? true : false;
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
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 240
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   场景：YBA值:2--->0
*                                           动作：关闭B枪下所有模块 如果成功则将实际状态的bB字段置0
****************************************************************************************************/
void sModAppAvgFunOp_240(void)
{
    i32  i;
    bool bRst = true;
    bool bMatchFlag = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:240, B枪充电Y断开--->AB枪均未充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == (ePileGunIndex0 + 1))
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            bMatchFlag = true;
            sModDrvOptClose(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst &  (pCache->stAttrModule[i].f32Udc < cModAppAvgModUdcClose);//确保所有模块关机
        }
    }
    
    
    if((bRst == true) && (bMatchFlag == true))
    {
        pCache->unStatusActual.stValue.bB = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}







/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 203
* Author                                :   Hall
* Creat Date                            :   2024-09-19
* notice                                :   场景：YBA值:2--->3
*                                           动作：开启A枪下所有模块 如果成功则将实际状态的bA字段置1
****************************************************************************************************/
void sModAppAvgFunOp_203(void)
{
    i32  i;
    bool bRst = false;
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:203, B枪充电Y断开--->AB枪均在充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(i = 0; i < cModAppAvgModNumMax; i++)
    {
        if((pCache->stAttrModule[i].eGunIndex == ePileGunIndex0)
        && (sModAppAvgAttrModAddrCheck(pCache->stAttrModule[i].u32Addr) == true))
        {
            sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
            bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
        }
    }
    
    
    if(bRst == true)
    {
        pCache->unStatusActual.stValue.bA = 1;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 223
* Author                                :   Hall
* Creat Date                            :   2024-09-26
* notice                                :   场景：YBA值:1--->3
*                                           B枪充电Y断开--->AB枪均在充电Y断开,与223等效
****************************************************************************************************/
void sModAppAvgFunOp_223(void)
{
    EN_SLOGI(TAG, "模块均充算法投切动作:223, B枪充电Y断开--->AB枪均在充电Y断开---此切换等效203,按203执行");
    sModAppAvgFunOp_203();
}








/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 206
* Author                                :   Hall
* Creat Date                            :   2024-09-20
* notice                                :   场景：YBA值:1--->5
*                                           动作：开启原本属于A枪的模块
*                                                 调整各个模块的电流限值
*                                                 Y接触器两侧的模块输出电压差小于阈值后闭合Y 如果成功则将实际状态的bY字段置1
****************************************************************************************************/
void sModAppAvgFunOp_206(void)
{
    i32  i, m;
    bool bRst = false;
    bool bUdcDifferenceFlag = false;                                            //电压差值OK标志
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    
    if((sGetTimestamp() - pCache->u32StatusTimestamp) < cModAppAvgTimeSwitchIn)
    {
        //206切换逻辑需要延迟 cModAppAvgTimeSwitchIn 秒才执行 等待本枪输出稳定
        EN_SLOGI(TAG, "模块均充算法投切动作:206, 等待%d秒后执行", cModAppAvgTimeSwitchIn - (sGetTimestamp() - pCache->u32StatusTimestamp));
        return;
    }
    
    EN_SLOGI(TAG, "模块均充算法投切动作:206, B枪充电Y断开--->B枪充电Y闭合");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    for(m = 0; m < pCache->stAttrContactorY.u8AddrNum; m++)
    {
        for(i = 0; i < cModAppAvgModNumMax; i++)
        {
            if(pCache->stAttrModule[i].u32Addr == pCache->stAttrContactorY.pAddrL[m])
            {
                //这一组模块现在要切到B枪, 先给B枪的实际电压（避免压差）,电流给初值 后续慢慢调整
                pCache->stAttrModule[i].bSwichFlag = true;
                pCache->stAttrModule[i].f32UdcSet = pCache->stAttrGun[ePileGunIndex0 + 1].f32Udc;
                pCache->stAttrModule[i].f32IdcSet = cModAppAvgIdcStart;
                
                //这里此模块输出参数变更之后应该先设置参数 下面再执行开机动作更严谨一点
                sModDrvOptSetOutput(pCache->stAttrModule[i].u32Addr, false, pCache->stAttrModule[i].f32UdcSet, pCache->stAttrModule[i].f32IdcSet);
                
                //pAddrL[] 的模块是原本属于A枪的模块 要全部开启
                sModDrvOptOpen(pCache->stAttrModule[i].u32Addr, false);
                bRst = bRst | (pCache->stAttrModule[i].f32Udc > cModAppAvgModUdcOpen);//只要有一个模块开机即可---考虑有的模块在线但已经故障态无法开机输出
                
                //判断这一组模块输出电压是否达到A枪实际电压
                bUdcDifferenceFlag = bUdcDifferenceFlag | (fabs(pCache->stAttrModule[i].f32Udc - pCache->stAttrGun[ePileGunIndex0 + 1].f32Udc) < 10.0f) ? true : false;
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
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 400
* Author                                :   Hall
* Creat Date                            :   2024-09-21
* notice                                :   场景：YBA值:4--->0
*                                           动作：断开Y接触器 如果成功则将实际状态的bY字段置0
****************************************************************************************************/
void sModAppAvgFunOp_400(void)
{
    stModAppAvgCache_t *pCache = pModAppAvgCache;
    
    EN_SLOGI(TAG, "模块均充算法投切动作:400, AB枪均未充电Y闭合--->AB枪均未充电Y断开");
    xSemaphoreTake(pCache->xSemStatus, portMAX_DELAY);
    if(sIoSetDrXyz('Y', false) == true)
    {
        pCache->unStatusActual.stValue.bY = 0;
        pCache->u32StatusTimestamp = sGetTimestamp();
        sModAppAvgAttrModUpdateGunIndex();
    }
    xSemaphoreGive(pCache->xSemStatus);
}






/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 425
* Author                                :   Hall
* Creat Date                            :   2024-09-21
* notice                                :   场景：YBA值:4--->5
*                                           虽然期望值是4--->5, 但实际动作还是按4--->0执行, 后续再执行0--->5
****************************************************************************************************/
void sModAppAvgFunOp_425(void)
{
    EN_SLOGI(TAG, "模块均充算法投切动作:425, AB枪均未充电Y闭合--->A枪充电Y闭合---此切换不支持,分400/025两次执行");
    sModAppAvgFunOp_400();
}





/***************************************************************************************************
* Description                           :   模块电源管理用户程序 之 均充算法--->状态切换动作函数 486
* Author                                :   Hall
* Creat Date                            :   2024-09-21
* notice                                :   场景：YBA值:4--->6
*                                           虽然期望值是4--->5, 但实际动作还是按4--->0执行, 后续再执行0--->5
****************************************************************************************************/
void sModAppAvgFunOp_486(void)
{
    EN_SLOGI(TAG, "模块均充算法投切动作:486, AB枪均未充电Y闭合--->B枪充电Y闭合---此切换不支持,分400/026两次执行");
    sModAppAvgFunOp_400();
}











































































