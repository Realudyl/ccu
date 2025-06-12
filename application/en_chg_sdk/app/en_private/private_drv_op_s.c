/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :     private_drv_op_s.c
* Description                           :     
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2024-05-27
* notice                                :     
****************************************************************************************************/
#include "private_drv_opt.h"




#if   (cSdkPrivDevType == cSdkPrivDevTypeS)                                     //Slave  侧有周期性主动发送报文




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_op_s";



extern stPrivDrvCache_t *pPrivDrvCache;






void sPrivDrvSendTask(void *pParam);
u8   sPrivDrvGetGunId(ePileGunIndex_t eGunIndex);
bool sPrivDrvGetCommFailSts(void);
bool sPrivDrvSetCallCmd(u8 u8GunId, ePrivDrvCmd_t eCmd);


void sPrivDrvCycleProc02(u32 u32Time);
void sPrivDrvCycleProc03(u32 u32Time, u8 u8GunId);
void sPrivDrvCycleProc04(u32 u32Time);
void sPrivDrvCycleProc08(u32 u32Time, u8 u8GunId);
void sPrivDrvCycleProc0E(u32 u32Time, u8 u8GunId);
void sPrivDrvCycleProc13(u32 u32Time, u8 u8GunId);

void sPrivDrvChkCommFail(void);










/**********************************************************************************************
* Description       :     数据发送任务
* Author            :     Hall
* modified Date     :     2024-04-19
* notice            :     
***********************************************************************************************/
void sPrivDrvSendTask(void *pParam)
{
    u32  u32Time;
    u8   u8GunId = cPrivDrvGunIdChg;
    stPrivDrvDataSlave_t *pData  = &pPrivDrvCache->unData.stSlave;;
    
    vTaskDelay(2000 / portTICK_RATE_MS);
    EN_SLOGI(TAG, "EN+ 内网协议 , 数据发送任务开始运行!!!");
    
    while(1)
    {
        u32Time = sGetTimestamp();
        sPrivDrvCycleProc02(u32Time);
        sPrivDrvCycleProc03(u32Time, u8GunId);
        sPrivDrvCycleProc04(u32Time);
        sPrivDrvCycleProc13(u32Time, u8GunId);
        sPrivDrvCycleProc08(u32Time, u8GunId);
        sPrivDrvCycleProc0E(u32Time, u8GunId);
        sPrivDrvChkCommFail();
        
        //非充电态下持续更新时间戳，充电开始以后就不要更新了
        if((u8GunId != cPrivDrvGunIdChg) && (pData->stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus != ePrivDrvGunWorkStatusCharging))
        {
            pPrivDrvCache->u32TimeStartCharging[u8GunId] = sGetTimestamp();
        }
        
        u8GunId++;
        if(u8GunId > cPrivDrvGunNumMax)
        {
            u8GunId = cPrivDrvGunIdChg;
        }
        
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/**********************************************************************************************
* Description       :     内网协议驱动中 u8GunId 字段的值计算
* Author            :     Hall
* modified Date     :     2024-05-29
* notice            :     从 Slave 侧的枪序号 eGunIndex 换算 u8GunId 字段
***********************************************************************************************/
u8 sPrivDrvGetGunId(ePileGunIndex_t eGunIndex)
{
    return (eGunIndex + cPrivDrvGunIdBase);
}







/**********************************************************************************************
* Description       :     内网协议驱动中 获取当前通讯超时故障状态
* Author            :     Hall
* modified Date     :     2024-07-01
* notice            :     
***********************************************************************************************/
bool sPrivDrvGetCommFailSts(void)
{
    return pPrivDrvCache->bTimeoutFlag;
}






/**********************************************************************************************
* Description       :     内网协议驱动中 召唤指定报文功能实现
* Author            :     Hall
* modified Date     :     2024-07-01
* notice            :     
***********************************************************************************************/
bool sPrivDrvSetCallCmd(u8 u8GunId, ePrivDrvCmd_t eCmd)
{
    bool bRst = true;
    
    switch(eCmd)
    {
    case ePrivDrvCmd02:
        pPrivDrvCache->u32Time02 = 0;
        pPrivDrvCache->unData.stSlave.stChg.bCacheA2Flag = false;
        EN_SLOGI(TAG, "召唤02报文成功");
        break;
    case ePrivDrvCmd03:
        if(u8GunId > cPrivDrvGunNumMax)
        {
            EN_SLOGE(TAG, "召唤03报文失败:枪号%d错误!!!", u8GunId);
            bRst = false;
        }
        else
        {
            pPrivDrvCache->u32Time03[u8GunId] = 0;
            EN_SLOGI(TAG, "召唤03报文成功");
        }
        break;
    case ePrivDrvCmd04:
        pPrivDrvCache->u32Time04 = 0;
        EN_SLOGI(TAG, "召唤04报文成功");
        break;
    case ePrivDrvCmd08:
        if((u8GunId < cPrivDrvGunIdBase)
        || (u8GunId > cPrivDrvGunNumMax)
        || (pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus == ePrivDrvGunWorkStatusCharging))
        {
            EN_SLOGE(TAG, "召唤08报文失败:枪号%d错误或该枪当前不是充电状态!!!", u8GunId);
            bRst = false;
        }
        else
        {
            pPrivDrvCache->u32Time08[u8GunId] = 0;
            EN_SLOGI(TAG, "召唤08报文成功");
        }
        break;
    case ePrivDrvCmd0E:
        if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
        {
            EN_SLOGE(TAG, "召唤0E报文失败:枪号%d错误!!!", u8GunId);
            bRst = false;
        }
        else
        {
            pPrivDrvCache->u32Time0E[u8GunId] = 0;
            EN_SLOGI(TAG, "召唤0E报文成功");
        }
        break;
    case ePrivDrvCmd13:
        if((u8GunId < cPrivDrvGunIdBase) || (u8GunId > cPrivDrvGunNumMax))
        {
            EN_SLOGE(TAG, "召唤13报文失败:枪号%d错误!!!", u8GunId);
            bRst = false;
        }
        else
        {
            pPrivDrvCache->u32Time13[u8GunId] = 0;
            EN_SLOGI(TAG, "召唤13报文成功");
        }
        break;
    default:
        bRst = false;
        EN_SLOGI(TAG, "召唤0x%02X报文失败, 此报文不支持召唤", eCmd);
        break;
    }
    
    return bRst;
}





/**********************************************************************************************
* Description       :     获取充电总电量函数 get:充电订单总电量
* Author            :     HH
* modified Date     :     2024-08-09
* notice            :     根据订单获取总电量，根据费率模型，将模型中的分段电量统计累加
***********************************************************************************************/
u32 sPrivDrvGetToatlEnergy(unPrivDrvMoney_t *pMoney)
{
    u32 u32Energy;
    u8 i;
    
    u32Energy = 0;
    if(pMoney->stDiv.eMode == ePrivDrvRateModelDivEqually)
    {
        for(i = 0; i < cPrivDrvRateSection; i++)
        {
            u32Energy += pMoney->stDiv.u32Elect[i];
        }
    }
    
    if(pMoney->stTopDown1.eMode == ePrivDrvRateModelTopDown1)
    {
        for(i = 0; i < cPrivDrvRateTimeSection; i++)
        {
            u32Energy += pMoney->stTopDown1.stUnit[i].u32Elect;
        }
    }
    
    if(pMoney->stTopDown2.eMode == ePrivDrvRateModelTopDown2)
    {
        for(i = 0; i < ePrivDrvRateTypeMax; i++)
        {
            u32Energy += pMoney->stTopDown2.u32Elect[i];
        }
    }
    
    return u32Energy;
}










/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 02报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc02(u32 u32Time)
{
    if((u32Time >= pPrivDrvCache->u32Time02)
    && ((u32Time - pPrivDrvCache->u32Time02) >= cPrivDrvCycle02)
    && (pPrivDrvCache->unData.stSlave.stChg.bCacheA2Flag != true))
    {
        if(pPrivDrvCache->unData.stSlave.stChg.u8GunNum > 0)                    //确保用户层已经初始化内网 才签到
        {
            pPrivDrvCache->u32Time02 = u32Time;
            sPrivDrvPktSend02();
        }
    }
}






/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 03报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc03(u32 u32Time, u8 u8GunId)
{
    if((u32Time >= pPrivDrvCache->u32Time03[u8GunId])
    && ((u32Time - pPrivDrvCache->u32Time03[u8GunId]) >= cPrivDrvCycle03))
    {
        pPrivDrvCache->u32Time03[u8GunId] = u32Time;
        sPrivDrvPktSend03(u8GunId);
    }
}






/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 04报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc04(u32 u32Time)
{
    if((u32Time >= pPrivDrvCache->u32Time04)
    && ((u32Time - pPrivDrvCache->u32Time04) >= cPrivDrvCycle04))
    {
        if(pPrivDrvCache->unData.stSlave.stChg.u8GunNum > 0)                    //确保用户层已经初始化内网 才签到
        {
            pPrivDrvCache->u32Time04 = u32Time;
            sPrivDrvPktSend04();
        }
    }
}






/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 08报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc08(u32 u32Time, u8 u8GunId)
{
    i32  i32Cycle = 0;
    i32  i32ChargingTime = 0;
    
    i32ChargingTime  = (u32Time < pPrivDrvCache->u32TimeStartCharging[u8GunId]) ? 0 : (u32Time - pPrivDrvCache->u32TimeStartCharging[u8GunId]);
    i32Cycle = (i32ChargingTime >= cPrivDrvCycleFastTime) ? cPrivDrvCycle08 : cPrivDrvCycle08Fast;
    
    if((u8GunId != cPrivDrvGunIdChg) && (pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus == ePrivDrvGunWorkStatusCharging))
    {
        if((u32Time >= pPrivDrvCache->u32Time08[u8GunId])
        && ((u32Time - pPrivDrvCache->u32Time08[u8GunId]) >= i32Cycle))
        {
            pPrivDrvCache->u32Time08[u8GunId] = u32Time;
            pPrivDrvCache->bPkt08EnableFlag[u8GunId] = true;
        }
    }
}






/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 0E报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc0E(u32 u32Time, u8 u8GunId)
{
    i32  i32Cycle = 0;
    i32  i32ChargingTime = 0;
    
    if(u8GunId == cPrivDrvGunIdChg)
    {
        //cPrivDrvGunIdChg 不上报0E
        return;
    }
    
    if(pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus == ePrivDrvGunWorkStatusCharging)
    {
        i32ChargingTime  = (u32Time < pPrivDrvCache->u32TimeStartCharging[u8GunId]) ? 0 : (u32Time - pPrivDrvCache->u32TimeStartCharging[u8GunId]);
        i32Cycle = (i32ChargingTime >= cPrivDrvCycleFastTime) ? cPrivDrvCycle0E : cPrivDrvCycle0EFast;
    }
    else
    {
        i32Cycle = cPrivDrvCycle0ESlow;
    }
    
    if((u32Time >= pPrivDrvCache->u32Time0E[u8GunId])
    && ((u32Time - pPrivDrvCache->u32Time0E[u8GunId]) >= i32Cycle))
    {
        pPrivDrvCache->u32Time0E[u8GunId] = u32Time;
        pPrivDrvCache->bPkt0EEnableFlag[u8GunId] = true;
    }
    
}






/**********************************************************************************************
* Description       :     slave 端 周期上报报文处理 之 13报文处理
* Author            :     Hall
* modified Date     :     2024-06-20
* notice            :     
***********************************************************************************************/
void sPrivDrvCycleProc13(u32 u32Time, u8 u8GunId)
{
    i32  i32Cycle = 0;
    i32  i32ChargingTime = 0;
    
    i32ChargingTime  = (u32Time < pPrivDrvCache->u32TimeStartCharging[u8GunId]) ? 0 : (u32Time - pPrivDrvCache->u32TimeStartCharging[u8GunId]);
    i32Cycle = (i32ChargingTime >= cPrivDrvCycleFastTime) ? cPrivDrvCycle13 : cPrivDrvCycle13Fast;
    
    if((u8GunId != cPrivDrvGunIdChg) && (pPrivDrvCache->unData.stSlave.stGun[u8GunId - cPrivDrvGunIdBase].eWorkStatus == ePrivDrvGunWorkStatusCharging))
    {
        if((u32Time >= pPrivDrvCache->u32Time13[u8GunId])
        && ((u32Time - pPrivDrvCache->u32Time13[u8GunId]) >= i32Cycle))
        {
            pPrivDrvCache->u32Time13[u8GunId] = u32Time;
            pPrivDrvCache->bPkt13EnableFlag[u8GunId] = true;
        }
    }
}








/**********************************************************************************************
* Description       :     slave 端 通讯超时故障检查
* Author            :     Hall
* modified Date     :     2024-07-01
* notice            :     
***********************************************************************************************/
void sPrivDrvChkCommFail(void)
{
    if(pPrivDrvCache->bTimeoutFlag == false)
    {
        if((pPrivDrvCache->u32TimeA4 > 0)
        && ((sGetTimestamp() - pPrivDrvCache->u32TimeA4) > (3 * cPrivDrvCycle04)))
        {
            //连续3个心跳周期未收到A4心跳应答报文
            pPrivDrvCache->bCmdA3Flag = false;                                  //先清除A3报文标志
            pPrivDrvCache->bTimeoutFlag = true;                                 //再置故障标志
        }
    }
    else
    {
        if((pPrivDrvCache->bCmdA3Flag == true)                                  //且收到A3状态应答报文---确保master端收到此故障码
        && ((sGetTimestamp() - pPrivDrvCache->u32TimeA4) < cPrivDrvCycle04))    //收到A4心跳应答报文
        {
            pPrivDrvCache->bTimeoutFlag = false;                                //之后再清除故障码
        }
    }
}




























#endif









