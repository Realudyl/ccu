/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_rcd_rpt.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 充电记录上报
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-31
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate_rcd_rpt";




extern stPileCnCache_t *pPileCnCache;








void sPileCnGateNewRcdRpt(ePileGunIndex_t eGunIndex);
void sPileCnGateOldRcdRpt(ePileGunIndex_t eGunIndex);
void sPileCnGateRptAck   (ePileGunIndex_t eGunIndex);


void sPileCnGateNewRcdInvalid    (ePileGunIndex_t eGunIndex);
void sPileCnGateNewRcdInitForPtc (ePileGunIndex_t eGunIndex);
void sPileCnGateNewRcdInitForRmt (ePileGunIndex_t eGunIndex, stPrivDrvCmdA6_t *pRmt,         stPrivDrvCmdA7_t *pTransAck);
void sPileCnGateNewRcdInitForCard(ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pCardAuthAck, stPrivDrvCmdA7_t *pTransAck);
void sPileCnGateNewRcdInitForVin (ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pVinAuthAck,  stPrivDrvCmdA7_t *pTransAck);

bool sPileCnGateOldRcdChargingConfirm(stEepromAppBlockChgRcd_t *pRcd);



i32  sPileCnGateGetSoc(ePileGunIndex_t eGunIndex);
bool sPileCnGateChkRcd(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd);



//生成一系列id
bool sPileCnGateMakeUserIdForPtc(ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId);
bool sPileCnGateMakeOrderId     (ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId);
bool sPileCnGateMakeDevTradeId  (ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId);









/***************************************************************************************************
* Description                           :   新充电记录的上报逻辑
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdRpt(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcd_t *pRcd = &pData->stNewRcd[eGunIndex];
    
    if((pRcd->bRptFlag == false)                                                //  该记录未上报
    && (pRcd->bChargingFlag == false))                                          //且该记录不在充电中
    {
        if((sGetTimestamp() - pData->u32OldRptTime) >= 5)
        {
            pData->u32OldChkTime = sGetTimestamp();
            sPrivDrvSetRcdRpt(&pRcd->stRcdData);
            
            //执行一次上报就就清除整个记录，后续上报由旧记录上报逻辑处理
            sPileCnGateNewRcdInvalid(eGunIndex);
        }
    }
}





/***************************************************************************************************
* Description                           :   旧充电记录的上报逻辑
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   遍历充电记录存储区 依次上报未上报记录
*
*                                           在多枪环境下，此函数会被多个线程调用，为避免多线程可能的
*                                           资源冲突，限制由其中一把枪执行就够了
*
*                                           有枪充电时 u16ReadIndex & u16WriteIndex 不等，会周期的执
*                                           行充电记录读取操作，此设计认为不影响eeprom寿命，可以接受
****************************************************************************************************/
void sPileCnGateOldRcdRpt(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    stEepromAppBlockChgRcdHead_t *pRcdHead;
    
    
    if((eGunIndex != ePileGunIndex0)                                            //确保由枪1执行
    || (sEepromAppGetBlockChgRcdHead(false, &pRcdHead) == false)
    || (pRcdHead->u16ReadIndex == pRcdHead->u16WriteIndex))                     //相等表示无未上报记录
    {
        return;
    }
    
    if((sGetTimestamp() - pData->u32OldChkTime) >= 60)
    {
        pData->u32OldChkTime = sGetTimestamp();
        pData->u16CheckIndex = sEepromAppGetBlockChgRcdByIndex(pData->u16CheckIndex, &pData->stOldRcd);
        if(pData->u16CheckIndex >= 0)
        {
            if(pData->stOldRcd.bRptFlag == false)                               //该记录未上报
            {
                if(pData->stOldRcd.bChargingFlag == false)                      //且不在充电中
                {
                    EN_SLOGI(TAG, "旧记录上报逻辑:检查到未上报记录,位置序号:%d, 执行上报", pData->u16CheckIndex);
                    pData->u32OldRptTime = sGetTimestamp();
                    sPrivDrvSetRcdRpt(&pData->stOldRcd.stRcdData);              //就上报一次
                }
                else
                {
                    if(sPileCnGateOldRcdChargingConfirm(&pData->stOldRcd) == true)
                    {
                        EN_SLOGI(TAG, "旧记录上报逻辑:检查到未上报记录（充电中，实际未充电）,位置序号:%d, 执行上报", pData->u16CheckIndex);
                        pData->u32OldRptTime = sGetTimestamp();
                        sPrivDrvSetRcdRpt(&pData->stOldRcd.stRcdData);          //上报
                    }
                    else
                    {
                        EN_SLOGI(TAG, "旧记录上报逻辑:检查到未上报记录（充电中）,位置序号:%d, 不予上报", pData->u16CheckIndex);
                    }
                }
                
                pData->u16CheckIndex++;                                         //然后指向下一条
                if(pData->u16CheckIndex == pRcdHead->u16WriteIndex)
                {
                    pData->u16CheckIndex = pRcdHead->u16ReadIndex;
                }
            }
            else if(pData->stOldRcd.bRptFlag == true)
            {
                EN_SLOGI(TAG, "旧记录上报逻辑:检查到已上报记录,位置序号:%d, 直接更新head信息", pData->u16CheckIndex);
                sEepromAppUpdBlockChgRcdHead(&pData->stOldRcd);
                pData->u16CheckIndex++;
                if(pData->u16CheckIndex < pRcdHead->u16ReadIndex)
                {
                    pData->u16CheckIndex = pRcdHead->u16ReadIndex;              //head更新以后指向最新的记录
                }
            }
        }
        
    }
}







/***************************************************************************************************
* Description                           :   充电记录上报应答结果处理
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   stOldRcd 其他函数共用未上锁 这些函数应在同一线程内执行
****************************************************************************************************/
void sPileCnGateRptAck(ePileGunIndex_t eGunIndex)
{
    i32  i32Index;
    stPrivDrvCmdA9_t *pRcdAck = NULL;
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    //检查网关应答结果
    if(sPrivDrvGetRcdRptAck(sPrivDrvGetGunId(eGunIndex), &pRcdAck) == true)
    {
        if(pRcdAck->eRst == ePrivDrvCmdRstSuccess)
        {
            //查找该记录
            i32Index = sEepromAppGetBlockChgRcdById(sizeof(pRcdAck->u8OrderId), pRcdAck->u8OrderId, &pData->stOldRcd, sPileCnGateChkRcd);
            if(i32Index >= 0)
            {
                //设置为已上报并更新eeprom存储内容
                pData->stOldRcd.bRptFlag = true;
                pData->stOldRcd.bChargingFlag = false;
                sEepromAppSetBlockChgRcdByIndex(i32Index, &pData->stOldRcd);
                sEepromAppUpdBlockChgRcdHead(&pData->stOldRcd);
            }
        }
    }
}







/***************************************************************************************************
* Description                           :   stNewRcd 字段在初始化和充电结束以后 需要恢复无效值
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdInvalid(ePileGunIndex_t eGunIndex)
{
    stPileCnGateData_t *pData = &pPileCnCache->stGateData;
    
    memset(&pData->stNewRcd[eGunIndex], 0xff, sizeof(stEepromAppBlockChgRcd_t));
}





/***************************************************************************************************
* Description                           :   stNewRcd 字段在即插即用（PTC）启动充电时初始化
* Author                                :   Hall
* Creat Date                            :   2024-06-06
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdInitForPtc(ePileGunIndex_t eGunIndex)
{
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stEepromAppBlockChgRcd_t *pRcd = &pPileCnCache->stGateData.stNewRcd[eGunIndex];
    
    memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
    pRcd->bRptFlag = false;
    pRcd->bChargingFlag = true;                                                 //这个成员必须在此处赋值, 上报逻辑需要用到它
    pRcd->stRcdData.u8GunId = u8GunId;
    pRcd->stRcdData.eType = ePrivDrvStartTransTypePnc;
    pRcd->stRcdData.eMode = ePrivDrvChargingModeFull;
    pRcd->stRcdData.unParam.u32Value = 0;
    pRcd->stRcdData.stTimeStart = sGetTime();
    sPileCnGateMakeUserIdForPtc(u8GunId, sizeof(pRcd->stRcdData.u8UserId    ), pRcd->stRcdData.u8UserId    );
    sPileCnGateMakeOrderId     (u8GunId, sizeof(pRcd->stRcdData.u8OrderId   ), pRcd->stRcdData.u8OrderId   );
    sPileCnGateMakeDevTradeId  (u8GunId, sizeof(pRcd->stRcdData.u8DevTradeId), pRcd->stRcdData.u8DevTradeId);
    
}






/***************************************************************************************************
* Description                           :   stNewRcd 字段在远程（RMT）启动充电时初始化
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdInitForRmt(ePileGunIndex_t eGunIndex, stPrivDrvCmdA6_t *pRmt, stPrivDrvCmdA7_t *pTransAck)
{
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stEepromAppBlockChgRcd_t *pRcd = &pPileCnCache->stGateData.stNewRcd[eGunIndex];
    
    memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
    pRcd->bRptFlag = false;
    pRcd->bChargingFlag = true;                                                 //这个成员必须在此处赋值, 上报逻辑需要用到它
    pRcd->stRcdData.u8GunId = u8GunId;
    pRcd->stRcdData.eType = pRmt->eType;
    pRcd->stRcdData.eMode = pRmt->eMode;
    pRcd->stRcdData.unParam.u32Value = pRmt->unParam.u32Value;
    pRcd->stRcdData.stTimeStart = sGetTime();
    memcpy(pRcd->stRcdData.u8UserId,     pRmt->u8UserId,            sizeof(pRmt->u8UserId       ));
    memcpy(pRcd->stRcdData.u8OrderId,    pTransAck->u8OrderId,      sizeof(pTransAck->u8OrderId ));
    memcpy(pRcd->stRcdData.u8SrvTradeId, pTransAck->u8SrvTradeId,   sizeof(pRmt->u8SrvTradeId   ));
    sPileCnGateMakeDevTradeId(u8GunId, sizeof(pRcd->stRcdData.u8DevTradeId), pRcd->stRcdData.u8DevTradeId);
    
}






/***************************************************************************************************
* Description                           :   stNewRcd 字段在远程（刷卡）启动充电时初始化
* Author                                :   Hall
* Creat Date                            :   2024-06-11
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdInitForCard(ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pCardAuthAck, stPrivDrvCmdA7_t *pTransAck)
{
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stCardData_t *pCard = &pPileCnCache->stGateData.stCard[eGunIndex];
    stEepromAppBlockChgRcd_t *pRcd = &pPileCnCache->stGateData.stNewRcd[eGunIndex];
    
    memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
    pRcd->bRptFlag = false;
    pRcd->bChargingFlag = true;                                                 //这个成员必须在此处赋值, 上报逻辑需要用到它
    pRcd->stRcdData.u8GunId = u8GunId;
    pRcd->stRcdData.eType = pCard->eStartType;
    pRcd->stRcdData.eMode = pCard->eMode;
    pRcd->stRcdData.unParam.u32Value = pCard->unParam.u32Value;
    pRcd->stRcdData.stTimeStart = sGetTime();
    memcpy(pRcd->stRcdData.u8UserId,     pCardAuthAck->u8UserId,     sizeof(pCardAuthAck->u8UserId));
    memcpy(pRcd->stRcdData.u8OrderId,    pTransAck->u8OrderId,       sizeof(pTransAck->u8OrderId));
    memcpy(pRcd->stRcdData.u8SrvTradeId, pTransAck->u8SrvTradeId,    sizeof(pTransAck->u8SrvTradeId));
    sPileCnGateMakeDevTradeId(u8GunId, sizeof(pRcd->stRcdData.u8DevTradeId), pRcd->stRcdData.u8DevTradeId);
    
}





/***************************************************************************************************
* Description                           :   stNewRcd 字段在远程（Vin）启动充电时初始化
* Author                                :   Hall
* Creat Date                            :   2024-06-11
* notice                                :   
****************************************************************************************************/
void sPileCnGateNewRcdInitForVin(ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pVinAuthAck, stPrivDrvCmdA7_t *pTransAck)
{
    u8   u8GunId = sPrivDrvGetGunId(eGunIndex);                                 //内网驱动api函数的枪id参数 u8GunId
    stVinData_t *pVin = &pPileCnCache->stGateData.stVin[eGunIndex];
    stEepromAppBlockChgRcd_t *pRcd = &pPileCnCache->stGateData.stNewRcd[eGunIndex];
    
    memset(pRcd, 0, sizeof(stEepromAppBlockChgRcd_t));
    pRcd->bRptFlag = false;
    pRcd->bChargingFlag = true;                                                 //这个成员必须在此处赋值, 上报逻辑需要用到它
    pRcd->stRcdData.u8GunId = u8GunId;
    pRcd->stRcdData.eType = pVin->eStartType;
    pRcd->stRcdData.eMode = pVin->eMode;
    pRcd->stRcdData.unParam.u32Value = pVin->unParam.u32Value;
    pRcd->stRcdData.stTimeStart = sGetTime();
    memcpy(pRcd->stRcdData.u8UserId,     pVinAuthAck->u8UserId,      sizeof(pVinAuthAck->u8UserId));
    memcpy(pRcd->stRcdData.u8OrderId,    pTransAck->u8OrderId,       sizeof(pTransAck->u8OrderId));
    memcpy(pRcd->stRcdData.u8SrvTradeId, pTransAck->u8SrvTradeId,    sizeof(pTransAck->u8SrvTradeId));
    sPileCnGateMakeDevTradeId(u8GunId, sizeof(pRcd->stRcdData.u8DevTradeId), pRcd->stRcdData.u8DevTradeId);
    
}







/***************************************************************************************************
* Description                           :   旧记录上报逻辑中 正在充电的订单  核查处理
* Author                                :   Hall
* Creat Date                            :   2024-06-24
* notice                                :   
*                                           可能存在bug，使得充电结束以后对应的订单 bChargingFlag 未
*                                           置 false ，导致卡单，函数对这种情况做出识别并返回给外部
*
*                                           旧记录上报逻辑需要对这种订单执行上报，避免卡单
****************************************************************************************************/
bool sPileCnGateOldRcdChargingConfirm(stEepromAppBlockChgRcd_t *pRcd)
{
    stTime_t stTime;
    u32 u32ChargingTime;
    
    if(sPileCnStateGet(pRcd->stRcdData.u8GunId - cPrivDrvGunIdBase) == ePrivDrvGunWorkStatusAvailable)
    {
        stTime = sGetTime();
        u32ChargingTime = sGetTimeByStructTime(stTime, pRcd->stRcdData.stTimeStart);
        
        EN_SLOGI(TAG, "充电中订单确认: 充电启动时间:20%02d-%02d-%02d %02d:%02d:%02d, 已过去:%ds, 且充电枪处于可用状态", 
                pRcd->stRcdData.stTimeStart.year,
                pRcd->stRcdData.stTimeStart.mon,
                pRcd->stRcdData.stTimeStart.day,
                pRcd->stRcdData.stTimeStart.hour,
                pRcd->stRcdData.stTimeStart.min,
                pRcd->stRcdData.stTimeStart.sec,
                u32ChargingTime);
        
        return true;
    }
    
    return false;
}







/***************************************************************************************************
* Description                           :   从bms获取soc字段
* Author                                :   Hall
* Creat Date                            :   2024-06-06
* notice                                :   
****************************************************************************************************/
i32 sPileCnGateGetSoc(ePileGunIndex_t eGunIndex)
{
    return (sPileCnStateGet(eGunIndex) != ePrivDrvGunWorkStatusCharging) ? (-1) : sBmsGbt15Get_EvccCurSoc(eGunIndex);
}






/***************************************************************************************************
* Description                           :   基于内容id查找充电记录时 用于比较的逻辑
* Author                                :   Hall
* Creat Date                            :   2024-06-03
* notice                                :   在不同的需求下,会根据不同的内容id去查找充电记录
*
*                                           当下逻辑是使用订单编号:order id
****************************************************************************************************/
bool sPileCnGateChkRcd(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd)
{
    if(memcmp(pRcd->stRcdData.u8OrderId, pId, i32Len) == 0)
    {
        EN_HLOGI(TAG, "查找充电记录, 成功, 订单编号:", pId, i32Len);
        
        return true;
    }
    
    return false;
}














/***************************************************************************************************
* Description                           :   PTC充电时的 user id 生成
* Author                                :   Hall
* Creat Date                            :   2024-06-06
* notice                                :   一些场景启动充电，例如PTC，需要由桩端生成user id 
****************************************************************************************************/
bool sPileCnGateMakeUserIdForPtc(ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId)
{
    stFullTime_t stTime;
    
    stTime = sGetFullTime();
    memset(pId, 0, i32MaxLen);
    snprintf((char *)pId, i32MaxLen, "PTC-%04d%02d%02d%02d%02d%02d%02d", stTime.year, stTime.mon, stTime.day, stTime.hour, stTime.min, stTime.sec, eGunIndex);
    
    return true;
}







/***************************************************************************************************
* Description                           :   设备端生成订单编号
* Author                                :   Hall
* Creat Date                            :   2024-06-06
* notice                                :   一些场景启动充电，例如PTC，需要由桩端生成订单编号
****************************************************************************************************/
bool sPileCnGateMakeOrderId(ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId)
{
    return sMakeUuid32((char *)pId, i32MaxLen);
}








/***************************************************************************************************
* Description                           :   设备端生成交易流水号
* Author                                :   Hall
* Creat Date                            :   2024-06-06
* notice                                :   一些平台会要求桩端生成设备端交易流水号
****************************************************************************************************/
bool sPileCnGateMakeDevTradeId(ePileGunIndex_t eGunIndex, i32 i32MaxLen, u8 *pId)
{
    stFullTime_t stTime;
    
    stTime = sGetFullTime();
    memset(pId, 0, i32MaxLen);
    snprintf((char *)pId, i32MaxLen, "%04d%02d%02d%02d%02d%02d%02d%04d", stTime.year, stTime.mon, stTime.day, stTime.hour, stTime.min, stTime.sec, eGunIndex, 0001);
    
    return true;
}





































