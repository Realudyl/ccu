/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   eeprom_app_chg_rcd.c
* Description                           :   eeprom芯片用户程序实现 之 充电记录存储
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-08
* notice                                :   
****************************************************************************************************/
#include "eeprom_app.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_app_chg_rcd";





extern stEepromAppCache_t stEepromAppCache;







//基于内容(xxx Id)去查找一条充电记录并返回其序号
i32  sEepromAppGetBlockChgRcdById(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd,
                 bool (*pCbCheck)(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd));




//充电记录head管理api
//New 新建充电记录成功后head需要更新
//Upd 上报充电记录成功后head需要更新
//Get 读取充电记录head
i32  sEepromAppNewBlockChgRcdHead(void);
void sEepromAppUpdBlockChgRcdHead(stEepromAppBlockChgRcd_t *pRcd);
bool sEepromAppGetBlockChgRcdHead(bool bReadFlag, stEepromAppBlockChgRcdHead_t **pHead);


//充电记录管理api
//New 新建一条充电记录并返回其序号:i32RcdIndex
//Get/Set 基于该序号读写充电记录
i32  sEepromAppNewBlockChgRcd(stEepromAppBlockChgRcd_t *pRcd);
i32  sEepromAppGetBlockChgRcdByIndex(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd);
bool sEepromAppSetBlockChgRcdByIndex(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd);











/**********************************************************************************************
* Description                           :   基于内容(xxx Id)去查找一条充电记录并返回其序号
* Author                                :   Hall
* modified Date                         :   2024-06-03
* notice                                :   
***********************************************************************************************/
i32 sEepromAppGetBlockChgRcdById(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd,
                bool (*pCbCheck)(i32 i32Len, const u8 *pId, stEepromAppBlockChgRcd_t *pRcd))
{
    u16 u16Index;
    stEepromAppBlockChgRcdHead_t *pRcdHead = &stEepromAppCache.stRcdHead;
    
    //更新head
    if(sEepromAppGetBlockChgRcdHead(true, NULL) == false)
    {
        return (-1);
    }
    
    //设定查找起点---从  u16ReadIndex 位置开始查找到 u16WriteIndex 位置
    u16Index = pRcdHead->u16ReadIndex;
    while(u16Index != pRcdHead->u16WriteIndex)
    {
        //读取记录
        u16Index = sEepromAppGetBlockChgRcdByIndex(u16Index, pRcd);
        if(u16Index < 0)
        {
            return (-1);
        }
        
        
        //比较记录---比较的逻辑是外部传入的回调函数因为参与比较的对象是可变的
        if(pCbCheck(i32Len, pId, pRcd) == true)
        {
            EN_SLOGI(TAG, "查找充电记录, 成功, 序号:%d", u16Index);
            return u16Index;                                                    //匹配就返回该记录序号
        }
        
        u16Index++;                                                             //不匹配就切换下一条
    }
    
    //未查找到符合的记录
    return (-1);
}









/**********************************************************************************************
* Description                           :   充电记录head管理
* Author                                :   Hall
* modified Date                         :   2024-06-01
* notice                                :   新建充电记录成功后 head->u16WriteIndex 成员要加1
***********************************************************************************************/
i32 sEepromAppNewBlockChgRcdHead(void)
{
    i32  i32RcdIndex;
    stEepromAppBlockChgRcdHead_t *pRcdHead = &stEepromAppCache.stRcdHead;
    
    xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcdHead], portMAX_DELAY);
    i32RcdIndex = pRcdHead->u16WriteIndex;
    EN_SLOGI(TAG, "新建充电记录前, 总条数:%02d, 当前写位置:%02d, 当前读位置:%02d", pRcdHead->u16Total, pRcdHead->u16WriteIndex, pRcdHead->u16ReadIndex);
    
    pRcdHead->u16WriteIndex++;
    if(pRcdHead->u16WriteIndex >= cEepromAppBlockChgRcdNumMax)
    {
        pRcdHead->u16WriteIndex = 0;
    }
    if(pRcdHead->u16Total < cEepromAppBlockChgRcdNumMax)
    {
        pRcdHead->u16Total++;
    }
    xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcdHead]);
    
    
    if(sEepromAppSetBlock(eEepromAppBlockChgRcdHead) == false)
    {
        i32RcdIndex = -1;
    }
    EN_SLOGI(TAG, "新建充电记录后, 总条数:%02d, 当前写位置:%02d, 当前读位置:%02d", pRcdHead->u16Total, pRcdHead->u16WriteIndex, pRcdHead->u16ReadIndex);
    
    return i32RcdIndex;
}





/**********************************************************************************************
* Description                           :   充电记录head管理
* Author                                :   Hall
* modified Date                         :   2024-06-03
* notice                                :   充电记录上报成功后用户程序会修改对应记录的上报状态并
*                                           回写至eeprom，此时需要检查充电记录head->u16ReadIndex
*                                           字段是否需要更新
***********************************************************************************************/
void sEepromAppUpdBlockChgRcdHead(stEepromAppBlockChgRcd_t *pRcd)
{
    i32  i32Start;
    i32  i32RcdIndex;
    
    stEepromAppBlockChgRcdHead_t *pRcdHead = &stEepromAppCache.stRcdHead;
    
    i32Start = pRcdHead->u16ReadIndex;
    EN_SLOGI(TAG, "上报充电记录前, 总条数:%02d, 当前写位置:%02d, 当前读位置:%02d", pRcdHead->u16Total, pRcdHead->u16WriteIndex, pRcdHead->u16ReadIndex);
    while(pRcdHead->u16ReadIndex != pRcdHead->u16WriteIndex)
    {
        i32RcdIndex = sEepromAppGetBlockChgRcdByIndex(pRcdHead->u16ReadIndex, pRcd);
        if(pRcd->bRptFlag == true)
        {
            pRcdHead->u16ReadIndex = i32RcdIndex + 1;
            if(pRcdHead->u16ReadIndex >= cEepromAppBlockChgRcdNumMax)
            {
                pRcdHead->u16ReadIndex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    if(pRcdHead->u16ReadIndex != i32Start)
    {
        if(sEepromAppSetBlock(eEepromAppBlockChgRcdHead) == false)
        {
            i32RcdIndex = -1;
        }
    }
    EN_SLOGI(TAG, "上报充电记录后, 总条数:%02d, 当前写位置:%02d, 当前读位置:%02d", pRcdHead->u16Total, pRcdHead->u16WriteIndex, pRcdHead->u16ReadIndex);
    
}





/**********************************************************************************************
* Description                           :   读取充电记录head
* Author                                :   Hall
* modified Date                         :   2024-05-08
* notice                                :   根据参数决定是否执行读操作, 将head结构体地址传递给参数
***********************************************************************************************/
bool sEepromAppGetBlockChgRcdHead(bool bReadFlag, stEepromAppBlockChgRcdHead_t **pHead)
{
    //1:读取充电记录head
    if(bReadFlag == true)
    {
        if(sEepromAppGetBlock(eEepromAppBlockChgRcdHead) == false)
        {
            EN_SLOGE(TAG, "充电记录head读取失败");
            return(false);
        }
    }
    
    //2:head 内容校对
    if((stEepromAppCache.stRcdHead.u16Total      > cEepromAppBlockChgRcdNumMax)
    || (stEepromAppCache.stRcdHead.u16ReadIndex  > stEepromAppCache.stRcdHead.u16Total)
    || (stEepromAppCache.stRcdHead.u16WriteIndex > stEepromAppCache.stRcdHead.u16Total))
    {
        //清零并写入
        xSemaphoreTake(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcdHead], portMAX_DELAY);
        memset(&stEepromAppCache.stRcdHead, 0, sizeof(stEepromAppBlockChgRcdHead_t));
        xSemaphoreGive(stEepromAppCache.xSemBlock[eEepromAppBlockChgRcdHead]);
        
        if(sEepromAppSetBlock(eEepromAppBlockChgRcdHead) == false)
        {
            EN_SLOGE(TAG, "充电记录head内容错误, 清零后写入失败");
            return(false);
        }
        EN_SLOGI(TAG, "充电记录head内容错误, 清零后写入成功");
    }
    if(pHead != NULL)
    {
        (*pHead) = &stEepromAppCache.stRcdHead;
    }
    
    return(true);
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->充电记录存储区--->新建一条充电记录
* Author                                :   Hall
* modified Date                         :   2024-05-08
* notice                                :   新建的充电记录默认写在 u16WriteIndex 位置
***********************************************************************************************/
i32 sEepromAppNewBlockChgRcd(stEepromAppBlockChgRcd_t *pRcd)
{
    bool bRst;
    
    //1:准备数据---更新校验值
    pRcd->u16CheckCrc = sCrc16Modbus(((const u8 *)pRcd) + cEepromAppBlockRcdCrcSize, sizeof(stEepromAppBlockChgRcd_t) - cEepromAppBlockRcdCrcSize);
    
    
    //2:更新充电记录head
    if(sEepromAppGetBlockChgRcdHead(true, NULL) == false)
    {
        EN_SLOGE(TAG, "新建充电记录失败, head更新及内容校验出错");
        return(-1);
    }
    
    
    //3:写入充电记录---新建的充电记录默认写在 u16WriteIndex 位置
    bRst = sEepromAppSetBlockRcd(stEepromAppCache.stRcdHead.u16WriteIndex, (const stEepromAppBlockChgRcd_t *)pRcd);
    
    
    //4:写入成功的话 更新充电记录head
    return ((bRst == false) ? (-1) : sEepromAppNewBlockChgRcdHead());
}








/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->充电记录存储区--->读指定序号的充电记录
* Author                                :   Hall
* modified Date                         :   2024-05-08
* notice                                :   
***********************************************************************************************/
i32 sEepromAppGetBlockChgRcdByIndex(i32 i32RcdIndex, stEepromAppBlockChgRcd_t *pRcd)
{
    return sEepromAppGetBlockRcd(i32RcdIndex, pRcd);
}






/**********************************************************************************************
* Description                           :   eeprom芯片用户程序--->充电记录存储区--->写指定序号的充电记录
* Author                                :   Hall
* modified Date                         :   2024-05-08
* notice                                :   
***********************************************************************************************/
bool sEepromAppSetBlockChgRcdByIndex(i32 i32RcdIndex, const stEepromAppBlockChgRcd_t *pRcd)
{
   return (pRcd == NULL) ? false : sEepromAppSetBlockRcd(i32RcdIndex, pRcd);
}















