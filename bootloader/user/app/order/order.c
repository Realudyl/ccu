/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   order.c
* Description                           :   充电订单计量计费管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
#include "order.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "order";






stOrderCache_t stOrderCache;








bool sOrderInit(void);
void sOrderTask(void *pParam);
void sOrderProc(void);
void sOrderProcStep(ePileGunIndex_t eGunIndex,
                    void (*pCbStart)(ePileGunIndex_t eGunIndex),
                    void (*pCbRun)  (ePileGunIndex_t eGunIndex),
                    void (*pCbStop) (ePileGunIndex_t eGunIndex));
u32  sOrderProcCalcMoney(u32 u32Price, u32 u32Elect);
u32  sOrderProcCalcElectAdj(u32 u32Elect);





bool sOrderStop (ePileGunIndex_t eGunIndex, u32 u32Reason);
bool sOrderStart(ePileGunIndex_t eGunIndex, i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd, unPrivDrvRate_t *pRate, i32 (*pCbGetSoc)(ePileGunIndex_t eGunIndex));

eOrderStep_t sOrderGetStep(ePileGunIndex_t eGunIndex);
void sOrderSetEleLoss(u32 u32Coef);









/***************************************************************************************************
* Description                           :   充电订单计量计费管理组件 资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
bool sOrderInit(void)
{
    i32 i;
    memset(&stOrderCache, 0, sizeof(stOrderCache));
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        stOrderCache.eStep[i] = eOrderStepWait;
    }
    
    i = xTaskCreate(sOrderTask, "sOrderTask", (512), NULL, 20, NULL);
    
    return ((i == pdTRUE) ? true : false);
}







/***************************************************************************************************
* Description                           :   充电订单计量计费管理任务
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
void sOrderTask(void *pParam)
{
    EN_SLOGI(TAG, "任务建立:订单计量计费管理任务");
    
    while(1)
    {
        sOrderProc();
        
        vTaskDelay(500 / portTICK_RATE_MS);
    }
    
    EN_SLOGI(TAG, "任务删除:订单计量计费管理任务");
    vTaskDelete(NULL);
}








/***************************************************************************************************
* Description                           :   计量计费操作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   根据不同计费模型去执行计量操作
****************************************************************************************************/
void sOrderProc(void)
{
    i32  i;
    
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        switch(stOrderCache.eMode)
        {
        case ePrivDrvRateModelDivEqually:                                       //平均分段  模型
            sOrderProcStep(i, sOrderProcDivStart,      sOrderProcDivRun,      sOrderProcDivStop     );
            break;
        case ePrivDrvRateModelTopDown1:                                         //尖峰平谷1 模型
            sOrderProcStep(i, sOrderProcTopDown1Start, sOrderProcTopDown1Run, sOrderProcTopDown1Stop);
            break;
        case ePrivDrvRateModelTopDown2:                                         //尖峰平谷2 模型
            break;
        case ePrivDrvRateModelUserDef:                                          //用户自定义模型
            break;
        default:
            break;
        }
    }
}









/***************************************************************************************************
* Description                           :   计量计费操作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   根据当前枪的不同计量阶段去执行计量操作
****************************************************************************************************/
void sOrderProcStep(ePileGunIndex_t eGunIndex,
                    void (*pCbStart)(ePileGunIndex_t eGunIndex),
                    void (*pCbRun)  (ePileGunIndex_t eGunIndex),
                    void (*pCbStop) (ePileGunIndex_t eGunIndex))
{
    switch(stOrderCache.eStep[eGunIndex])
    {
    case eOrderStepWait:                                                    //  无计量 do nothing
        break;
    case eOrderStepStart:                                                   //开始计量 初始化订单结构体
        pCbStart(eGunIndex);
        stOrderCache.eStep[eGunIndex] = eOrderStepRun;                      //状态切换
        break;
    case eOrderStepRun:                                                     //正在计量 持续更新订单结构体并周期性写入eeprom
        pCbRun(eGunIndex);
        break;
    case eOrderStepStop:                                                    //停止计量 最后一次更新订单结构体并写入eeprom
        pCbStop(eGunIndex);
        stOrderCache.eStep[eGunIndex] = eOrderStepWait;                     //状态切换
        break;
    default:
        stOrderCache.eStep[eGunIndex] = eOrderStepWait;
        break;
    }

}






/***************************************************************************************************
* Description                           :   计算金额
* Author                                :   Hall
* Creat Date                            :   2024-05-22
* notice                                :   根据电量和价格计算电费和服务费
*
*                                           u32Price:价格，精度： cPrivDrvPrecisionPrice
*                                           u32Elect:电量，精度： 0.001kWh
*                                           返回值  :金额，精度： cPrivDrvPrecisionMoney
*
*                                           如果参与运算的价格和金额精度很高，32位运算会溢出,
*                                           所以运算过程用64位，最后结果恢复为32位
****************************************************************************************************/
u32 sOrderProcCalcMoney(u32 u32Price, u32 u32Elect)
{
    u64 u64Temp1, u64Temp2;
    
    i8  i8Exp = 0;                                                              //精度调整的指数
    static u32 u32Factor = 0;                                                   //精度调整的因子
    
    
    if(u32Factor == 0)                                                          //为避免重复计算消耗资源,设置此逻辑确保仅上电计算一次
    {
        i8Exp = cPrivDrvPrecisionPrice + 3 - cPrivDrvPrecisionMoney;            //2个宏的值都被限制了范围 此值是安全的
        i8Exp = (i8Exp < 0) ? 0 : i8Exp;                                        //负数保护, 不接受指数部分为负值的情况
        u32Factor = pow(10, i8Exp);                                             //因此这里的取幂也安全
    }
    
    
    u64Temp1 = u32Price;                                                        //cPrivDrvPrecisionPrice
    u64Temp2 = u32Elect;                                                        //0.001kWh
    
    //四舍五入以后除以因子 得到需求精度的金额值
    u64Temp1 = u64Temp1 * u64Temp2;                                             //
    u64Temp1 = (u64Temp1 + (u32Factor >> 1)) / u32Factor;                       //cPrivDrvPrecisionMoney
    
    return (u32)u64Temp1;
}








/***************************************************************************************************
* Description                           :   计算电损补充后的电量
* Author                                :   Hall
* Creat Date                            :   2024-07-18
* notice                                :   u32Elect:输入的电量
*
*                                           如果参与运算的电量精度很高，32位运算会溢出,
*                                           所以运算过程用64位，最后结果恢复为32位
****************************************************************************************************/
u32 sOrderProcCalcElectAdj(u32 u32Elect)
{
    u64 u64Temp1;
    
    u64Temp1 = u32Elect;
    u64Temp1 = u64Temp1 * stOrderCache.u32EleLoss;
    u64Temp1 = u64Temp1 / cEleLossDefault;
    
    return (u32)u64Temp1;
}















/***************************************************************************************************
* Description                           :   检查指定枪当前的计量状态
* Author                                :   Hall
* Creat Date                            :   2024-05-24
* notice                                :   
****************************************************************************************************/
eOrderStep_t sOrderGetStep(ePileGunIndex_t eGunIndex)
{
    return ((eGunIndex >= ePileGunIndexNum) ? eOrderStepMax : stOrderCache.eStep[eGunIndex]);
}








/***************************************************************************************************
* Description                           :   停止指定枪的计量计费操作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   
****************************************************************************************************/
bool sOrderStop(ePileGunIndex_t eGunIndex, u32 u32Reason)
{
    if(eGunIndex >= ePileGunIndexNum)
    {
        EN_SLOGI(TAG, "停止计量失败, 计量参数错误");
        return false;
    }
    
    stOrderCache.eStep[eGunIndex] = eOrderStepStop;
    stOrderCache.u32Stopreason[eGunIndex] = u32Reason;
    EN_SLOGI(TAG, "停止计量成功, 枪号:%d", eGunIndex);
    
    return true;
}






/***************************************************************************************************
* Description                           :   开始指定枪的计量计费操作
* Author                                :   Hall
* Creat Date                            :   2024-05-21
* notice                                :   要点:需要bms驱动已经获取到车端soc以后才能调用, 
*                                           否则充电前soc值不能正常设定
****************************************************************************************************/
bool sOrderStart(ePileGunIndex_t eGunIndex, i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd, unPrivDrvRate_t *pRate, i32 (*pCbGetSoc)(ePileGunIndex_t eGunIndex))
{
    if((eGunIndex >= ePileGunIndexNum) || (pRcd == NULL) || (pRate == NULL) || (stOrderCache.eStep[eGunIndex] != eOrderStepWait))
    {
        EN_SLOGI(TAG, "启动计量失败, 计量参数错误, 枪号:%d, rcd序号:%d", eGunIndex, i32RcdIndex);
        return false;
    }
    
    stOrderCache.eMode                  = pRate->stDiv.eMode;
    stOrderCache.eStep[eGunIndex]       = eOrderStepStart;
    stOrderCache.pRcd[eGunIndex]        = pRcd;
    stOrderCache.pRate                  = pRate;
    stOrderCache.i32RcdIndex[eGunIndex] = i32RcdIndex;
    stOrderCache.pCbGetSoc              = pCbGetSoc;
    EN_SLOGI(TAG, "启动计量成功, 枪号:%d, rcd序号:%d", eGunIndex, i32RcdIndex);
    
    return true;
}








/***************************************************************************************************
* Description                           :   为计量计费组件设定电损补偿系数
* Author                                :   Hall
* Creat Date                            :   2024-05-22
* notice                                :   
****************************************************************************************************/
void sOrderSetEleLoss(u32 u32Coef)
{
    stOrderCache.u32EleLoss = u32Coef;
}








