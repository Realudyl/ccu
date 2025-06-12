/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_basic.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-18
* notice                                :     负责实现协议中一些基础的功能，例如打印，校验，字段解析...
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_basic.h"





//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_basic";





extern stPrivDrvCache_t *pPrivDrvCache;





void sPrivDrvDataPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);
bool sPrivDrvDataPrintCheck(const u8 *pBuf);
bool sPrivDrvCheckCrc(const u8  *pBuf, i32 i32Len);












/**********************************************************************************************
* Description       :     EN+ （串口）内网私有 协议 接收和发送数据打印函数
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
void sPrivDrvDataPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len)
{
    stPrivDrvHead_t *pHead = NULL;
    
    char u8HeadStr[128] = { 0 };
    char u8SendRecvStr[3][8] = { "接收", "发送"};
    u16  u16SendLen;
    pHead = (stPrivDrvHead_t *)pBuf;
    
    if(sPrivDrvDataPrintCheck(pBuf) == false)
    {
        return;
    }
    
    u16SendLen = u16Len;
    if(u16Len >= cPrivDrvDataPrintMax)
    {
        //OTA数据(长度较大,800多)不打印,打印它很耗时导致OTA慢
        u16SendLen = (cPrivDrvDataPrintMaxVaule == 0) ? 1 : cPrivDrvDataPrintMaxVaule;
    }
    
    memset(u8HeadStr, 0, sizeof(u8HeadStr));
    snprintf(u8HeadStr, sizeof(u8HeadStr), "EN+ 内网协议%s(Cmd:%02X Len:%03d)%s", u8SendRecvStr[bSendFlag], pHead->eCmd, u16Len, bSendFlag ? "--->" : "<---");
    EN_HLOGW(TAG, u8HeadStr, pBuf, u16SendLen);
}







/**********************************************************************************************
* Description       :     串口数据 打印检查
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     08/A8 0E/AE 13/B3报文有的场景下是秒级收发的 但打印不能那么快 需要检查
*                         是否满足打印条件
***********************************************************************************************/
bool sPrivDrvDataPrintCheck(const u8 *pBuf)
{
    bool bRst = false;
    u8   u8GunId;
    u32  *pTime = NULL;
    stPrivDrvPkt_t *pPkt = (stPrivDrvPkt_t *)pBuf;
    
    
    switch(pPkt->stHead.eCmd)
    {
    case ePrivDrvCmd08:
        u8GunId = pPkt->unPayload.stCmd08.u8GunId;
        pTime = &pPrivDrvCache->u32Time08Print[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            bRst = true;
        }
        break;
    case ePrivDrvCmdA8:
        u8GunId = pPkt->unPayload.stCmdA8.u8GunId;
        pTime = &pPrivDrvCache->u32Time08Print[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            (*pTime) = sGetTimestamp();
            bRst = true;
        }
        break;
    case ePrivDrvCmd0E:
        u8GunId = pPkt->unPayload.stCmd0E.u8GunId;
        pTime = &pPrivDrvCache->u32Time0EPrint[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            bRst = true;
        }
        break;
    case ePrivDrvCmdAE:
        u8GunId = pPkt->unPayload.stCmdAE.u8GunId;
        pTime = &pPrivDrvCache->u32Time0EPrint[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            (*pTime) = sGetTimestamp();
            bRst = true;
        }
        break;
    case ePrivDrvCmd13:
        u8GunId = pPkt->unPayload.stCmd13.u8GunId;
        pTime = &pPrivDrvCache->u32Time13Print[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            bRst = true;
        }
        break;
    case ePrivDrvCmdB3:
        u8GunId = pPkt->unPayload.stCmdB3.u8GunId;
        pTime = &pPrivDrvCache->u32Time13Print[u8GunId];
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            (*pTime) = sGetTimestamp();
            bRst = true;
        }
        break;
    case ePrivDrvCmd15:
        pTime = &pPrivDrvCache->u32Time15Print;
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            bRst = true;
        }
        break;
    case ePrivDrvCmdB5:
        pTime = &pPrivDrvCache->u32Time15Print;
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            (*pTime) = sGetTimestamp();
            bRst = true;
        }
        break;
    case ePrivDrvCmd16:
        pTime = &pPrivDrvCache->u32Time16Print;
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            bRst = true;
        }
        break;
    case ePrivDrvCmdB6:
        pTime = &pPrivDrvCache->u32Time16Print;
        if(sGetTimestamp() - (*pTime) >= cPrivDrvPrintCycle)
        {
            (*pTime) = sGetTimestamp();
            bRst = true;
        }
        break;
    default:
        bRst = true;
        break;
    }
    
    
    return bRst;
}








/**********************************************************************************************
* Description       :     串口数据 校验值检查
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     对于接收到完整帧 检查其校验值
***********************************************************************************************/
bool sPrivDrvCheckCrc(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u16  u16CrcCalc;
    u16  u16CrcRecv;
    
    
    u16CrcCalc = sCrc16WithA001(pBuf, i32Len - 2);
    u16CrcRecv = pBuf[i32Len - 1];
    u16CrcRecv = (u16CrcRecv << 8) + pBuf[i32Len - 2];
    if(u16CrcCalc == u16CrcRecv)
    {
        bRst = true;
        sPrivDrvDataPrint(false, pBuf, i32Len);
    }
    else
    {
        bRst = false;
        EN_SLOGI(TAG, "EN+ 内网协议, 数据接收:%d字节, CRC校验失败, 接收值:0x%04x, 计算值:0x%04x", i32Len, u16CrcRecv, u16CrcCalc);
    }
    
    return(bRst);
}










































