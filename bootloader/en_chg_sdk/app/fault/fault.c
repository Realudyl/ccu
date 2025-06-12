/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   fault.c
* Description                           :   故障告警码系统实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-11
* notice                                :   
****************************************************************************************************/
#include "fault.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "fault";








//与内网协议中的 u8GunId 字段一样，
//cPrivDrvGunIdChg 对应桩, 后面的对应枪
stPrivDrvFaultCode_t stFaultCode[ePileGunIndexNum + 1];

//stFaultCode 访问信号量
SemaphoreHandle_t xSemFaultCode;






void sFaultInit(void);

//故障告警码增减api
bool sFaultCodeAdd    (u8 u8GunId, ePrivDrvFaultCode_t eCode);
bool sFaultCodeSub    (u8 u8GunId, ePrivDrvFaultCode_t eCode);
bool sFaultCodeGet    (u8 u8GunId, stPrivDrvFaultCode_t *pFault);

bool sFaultCodeChkNum (u8 u8GunId);
bool sFaultCodeChkCode(u8 u8GunId, ePrivDrvFaultCode_t eCode);


bool sFaultCodeAddGun (u8 u8GunId, ePrivDrvFaultCode_t eCode);
bool sFaultCodeSubGun (u8 u8GunId, ePrivDrvFaultCode_t eCode);


//序列移除和插入操作
i32  sFaultCodeRemove (ePrivDrvFaultCode_t eCode, u16 u16Sum, ePrivDrvFaultCode_t *pCodeBuf);
i32  sFaultCodeInsert (ePrivDrvFaultCode_t eCode, u16 u16Sum, u16 u16MaxSize, ePrivDrvFaultCode_t *pCodeBuf);






/**********************************************************************************************
* Description                           :   故障告警码系统初始化
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
***********************************************************************************************/
void sFaultInit(void)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stFaultCode); i++)
    {
        memset(&stFaultCode[i], 0, sizeof(stPrivDrvFaultCode_t));
    }
    
    xSemFaultCode = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemFaultCode);
}









/**********************************************************************************************
* Description                           :   在故障告警码列表中增加 eCode
* Author                                :   Hall
* modified Date                         :   2024-07-04
* notice                                :   
***********************************************************************************************/
bool sFaultCodeAdd(u8 u8GunId, ePrivDrvFaultCode_t eCode)
{
    i32  i;
    
    if(u8GunId == cPrivDrvGunIdChg)
    {
        //如果是桩的码，则需要在所有枪的故障告警码表中添加该码
        for(i = cPrivDrvGunIdChg; i < (cPrivDrvGunIdBase + cPrivDrvGunNumMax); i++)
        {
            sFaultCodeAddGun(i, eCode);
        }
    }
    else
    {
        sFaultCodeAddGun(u8GunId, eCode);
    }
    
    return false;
}








/**********************************************************************************************
* Description                           :   在故障告警码列表中减少 eCode
* Author                                :   Hall
* modified Date                         :   2024-07-04
* notice                                :   
***********************************************************************************************/
bool sFaultCodeSub(u8 u8GunId, ePrivDrvFaultCode_t eCode)
{
    i32  i;
    
    if(u8GunId == cPrivDrvGunIdChg)
    {
        //如果是桩的码，则需要从所有枪的故障告警码表中删除该码
        for(i = cPrivDrvGunIdChg; i < (cPrivDrvGunIdBase + cPrivDrvGunNumMax); i++)
        {
            sFaultCodeSubGun(i, eCode);
        }
    }
    else
    {
        sFaultCodeSubGun(u8GunId, eCode);
    }
    
    return true;
}








/**********************************************************************************************
* Description                           :   获取 u8GunId 的故障告警信息 
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
***********************************************************************************************/
bool sFaultCodeGet(u8 u8GunId, stPrivDrvFaultCode_t *pCode)
{
    if((u8GunId >= sArraySize(stFaultCode)) || (pCode == NULL))
    {
        return false;
    }
    
    
    xSemaphoreTake(xSemFaultCode, portMAX_DELAY);
    memcpy(pCode, &stFaultCode[u8GunId], sizeof(stPrivDrvFaultCode_t));
    xSemaphoreGive(xSemFaultCode);
    
    return true;
}








/**********************************************************************************************
* Description                           :   检查当前故障数量
* Author                                :   Hall
* modified Date                         :   2024-06-12
* notice                                :   
*                                           返回值：小于0---异常，0---无故障，大于0---故障数量
***********************************************************************************************/
bool sFaultCodeChkNum(u8 u8GunId)
{
    ePrivDrvLeve_t eLevel;
    stPrivDrvFaultCode_t stCodeTemp;
    
    if(u8GunId >= sArraySize(stFaultCode))
    {
        return (-1);
    }
    
    return stFaultCode[u8GunId].u16SumF;
}








/**********************************************************************************************
* Description                           :   检查是否存在特定故障告警码
* Author                                :   Hall
* modified Date                         :   2024-06-12
* notice                                :   
*                                           
***********************************************************************************************/
bool sFaultCodeChkCode(u8 u8GunId, ePrivDrvFaultCode_t eCode)
{
    i32 i;
    ePrivDrvLeve_t eLevel;
    
    u16 u16Sum;
    ePrivDrvFaultCode_t *pCodeBuf = NULL;
    
    if(u8GunId >= sArraySize(stFaultCode))
    {
        return false;
    }
    
    
    if(sPrivDrvGetFaultLevel(eCode, &eLevel) == true)
    {
        u16Sum   = (eLevel == ePrivDrvLeve1G1) ? stFaultCode[u8GunId].u16SumW : stFaultCode[u8GunId].u16SumF;
        pCodeBuf = (eLevel == ePrivDrvLeve1G1) ? stFaultCode[u8GunId].eCodeW  : stFaultCode[u8GunId].eCodeF;
        
        if(u16Sum > 0)
        {
            for(i = 0; i < u16Sum; i++)
            {
                if (pCodeBuf[i] == eCode)
                {
                    //找到指定的码 返回 true
                    return true;
                }
            }
        }
    }
    
    return false;
}






/**********************************************************************************************
* Description                           :   在故障告警码列表中增加 eCode
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
***********************************************************************************************/
bool sFaultCodeAddGun(u8 u8GunId, ePrivDrvFaultCode_t eCode)
{
    ePrivDrvLeve_t eLevel;
    stPrivDrvFaultCode_t stCodeTemp;
    
    if(u8GunId >= sArraySize(stFaultCode))
    {
        return false;
    }
    
    memset(&stCodeTemp, 0, sizeof(stCodeTemp));
    memcpy(&stCodeTemp, &stFaultCode[u8GunId], sizeof(stCodeTemp));
    if(sPrivDrvGetFaultLevel(eCode, &eLevel) == true)
    {
        if(eLevel == ePrivDrvLeve1G1)                                           //告警码
        {
            stCodeTemp.u16SumW = sFaultCodeInsert(eCode, stCodeTemp.u16SumW, cPrivDrvFaultCodeLen, stCodeTemp.eCodeW);
        }
        else                                                                    //故障码
        {
            stCodeTemp.u16SumF = sFaultCodeInsert(eCode, stCodeTemp.u16SumF, cPrivDrvFaultCodeLen, stCodeTemp.eCodeF);
        }
    }
    
    xSemaphoreTake(xSemFaultCode, portMAX_DELAY);
    memcpy(&stFaultCode[u8GunId], &stCodeTemp, sizeof(stCodeTemp));
    xSemaphoreGive(xSemFaultCode);
    
    return false;
}








/**********************************************************************************************
* Description                           :   在故障告警码列表中减少 eCode
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
***********************************************************************************************/
bool sFaultCodeSubGun(u8 u8GunId, ePrivDrvFaultCode_t eCode)
{
    ePrivDrvLeve_t eLevel;
    stPrivDrvFaultCode_t stCodeTemp;
    
    if(u8GunId >= sArraySize(stFaultCode))
    {
        return false;
    }
    
    memset(&stCodeTemp, 0, sizeof(stCodeTemp));
    memcpy(&stCodeTemp, &stFaultCode[u8GunId], sizeof(stCodeTemp));
    if(sPrivDrvGetFaultLevel(eCode, &eLevel) == true)
    {
        if(eLevel == ePrivDrvLeve1G1)                                           //告警码
        {
            stCodeTemp.u16SumW = sFaultCodeRemove(eCode, stCodeTemp.u16SumW, stCodeTemp.eCodeW);
        }
        else                                                                    //故障码
        {
            stCodeTemp.u16SumF = sFaultCodeRemove(eCode, stCodeTemp.u16SumF, stCodeTemp.eCodeF);
        }
    }
    
    xSemaphoreTake(xSemFaultCode, portMAX_DELAY);
    memcpy(&stFaultCode[u8GunId], &stCodeTemp, sizeof(stCodeTemp));
    xSemaphoreGive(xSemFaultCode);
    
    return false;
}










/**********************************************************************************************
* Description                           :   故障码或告警码列表移除操作
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
*                                           eCode       :待移除码
*                                           u16Sum      :码列表当前个数
*                                           pCodeBuf    :码列表首地址
***********************************************************************************************/
i32 sFaultCodeRemove(ePrivDrvFaultCode_t eCode, u16 u16Sum, ePrivDrvFaultCode_t *pCodeBuf)
{
    i32 i;
    i32 i32Pos = -1;
    
    
    //1:查找要删除的元素
    for(i = 0; i < u16Sum; i++)
    {
        if(pCodeBuf[i] == eCode)
        {
            i32Pos = i;
            break;
        }
    }
    
    //2:如果找到要删除的元素
    if(i32Pos != -1)
    {
        //删除元素并将后续元素前移
        for(i = i32Pos; i < (u16Sum - 1); i++)
        {
            pCodeBuf[i] = pCodeBuf[i + 1];
        }
        
        //原序列最后那个元素清0
        pCodeBuf[i] = 0;
        
        return (u16Sum - 1);
    }
    else
    {
        // 没有找到要删除的元素
        return u16Sum;
    }
}







/**********************************************************************************************
* Description                           :   故障码或告警码列表插入操作
* Author                                :   Hall
* modified Date                         :   2024-06-11
* notice                                :   
*                                           eCode       :待插入码
*                                           u16Sum      :码列表当前个数
*                                           u16MaxSize  :码列表最大个数
*                                           pCodeBuf    :码列表首地址
***********************************************************************************************/
i32 sFaultCodeInsert(ePrivDrvFaultCode_t eCode, u16 u16Sum, u16 u16MaxSize, ePrivDrvFaultCode_t *pCodeBuf)
{
    i32  i;
    i32  i32Pos;
    
    //1:总数限制
    if(u16Sum >= u16MaxSize)
    {
        return u16MaxSize;
    }
    
    
    //2:码已经存在 不必插入
    for(i = 0; i < u16Sum; i++)
    {
        if(pCodeBuf[i] == eCode)
        {
            return u16Sum;
        }
    }
    
    //3:遍历码列表，找到待插入位置
    for(i32Pos = 0; i32Pos < u16Sum; i32Pos++)
    {
        //此条件确保增序排列
        if(pCodeBuf[i32Pos] > eCode)
        {
            break;
        }
    }
    
    //4:i32Pos 位置往后的所有码 依次后移 为待插入码腾出位置
    for(i = u16Sum; i > i32Pos; i--)
    {
        pCodeBuf[i] = pCodeBuf[i - 1];
    }
    
    //5:插入
    pCodeBuf[i32Pos] = eCode;
    
    return (u16Sum + 1);
}






























