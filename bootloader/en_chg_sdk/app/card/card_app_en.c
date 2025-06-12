/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_app_en.c
* Description                           :   EN+卡数据格式下的操作实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :   
****************************************************************************************************/
#include "card_app.h"
#include "card_app_en.h"








//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "card_app_en";





//EN+扇区密钥
const u8 u8KeySector0A[cCardDrvMt626LenKey] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const u8 u8KeySector0B[cCardDrvMt626LenKey] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const u8 u8KeySector2A[cCardDrvMt626LenKey] = {0x8A, 0x72, 0x64, 0x53, 0x4F, 0x3C};
const u8 u8KeySector2B[cCardDrvMt626LenKey] = {0x83, 0x7D, 0x62, 0x59, 0x4E, 0x32};
const u8 u8KeySector6A[cCardDrvMt626LenKey] = {0x3E, 0x7F, 0x28, 0xAD, 0x8C, 0x22};
const u8 u8KeySector6B[cCardDrvMt626LenKey] = {0x27, 0xAA, 0x93, 0x5D, 0x35, 0x09};
const u8 u8KeySector7A[cCardDrvMt626LenKey] = {0x08, 0x3D, 0x54, 0x25, 0x76, 0x6F};
const u8 u8KeySector7B[cCardDrvMt626LenKey] = {0x92, 0x48, 0x6A, 0x12, 0x7C, 0x49};





bool sCardAppEnRead(void *pData);
















/**********************************************************************************************
* Description                           :   EN+卡数据格式下的完整读数据流程
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardAppEnRead(void *pData)
{
    bool bRst;
    //unCardAppEnData_t unData;
    stCardData_t *pCard = (stCardData_t *)pData;
    
    //寻卡
    bRst = false;
    if(sCardDrvMt626M1Search() == true)
    {
        //读Mefare1 卡序列号
        bRst = sCardDrvMt626M1Mefare1(&pCard->u32Mefare1Sn);
        
        /*
        根据刘威的说法，当前已经废弃了EN+过去在卡上定义的各种信息，只需要上报卡号即可
        //sector:0, block:0
        memset(unData.u8Buf, 0, sizeof(unData));
        bRst = bRst & sCardDrvMt626M1Key (eCardDrvMt626CmdParaKeyA, 0, cCardDrvMt626LenKey, u8KeySector0A);
        bRst = bRst & sCardDrvMt626M1Read(0, 0, sizeof(unData), unData.u8Buf);
        
        
        //sector:2, block:0
        memset(unData.u8Buf, 0, sizeof(unData));
        bRst = bRst & sCardDrvMt626M1Key (eCardDrvMt626CmdParaKeyA, 2, cCardDrvMt626LenKey, u8KeySector2A);
        bRst = bRst & sCardDrvMt626M1Read(2, 0, sizeof(unData), unData.u8Buf);
        if(bRst == true)
        {
            pCard->u32Balance = unData.stS2B0.u32Balance;
            pCard->bOnlineFlag = unData.stS2B0.bOnlineFlag;
            pCard->bUnlockFlag = unData.stS2B0.bUnlockFlag;
        }
        
        
        //sector:6, block:0
        memset(unData.u8Buf, 0, sizeof(unData));
        bRst = bRst & sCardDrvMt626M1Key (eCardDrvMt626CmdParaKeyA, 6, cCardDrvMt626LenKey, u8KeySector6A);
        bRst = bRst & sCardDrvMt626M1Read(6, 0, sizeof(unData), unData.u8Buf);
        if(bRst == true)
        {
            memcpy(&pCard->u8CardNo, &unData.stS6B0.u8CardNo, sizeof(unData.stS6B0.u8CardNo));
        }
        
        
        //sector:7, block:0
        memset(unData.u8Buf, 0, sizeof(unData));
        bRst = bRst & sCardDrvMt626M1Key (eCardDrvMt626CmdParaKeyA, 7, cCardDrvMt626LenKey, u8KeySector7A);
        bRst = bRst & sCardDrvMt626M1Read(7, 0, sizeof(unData), unData.u8Buf);
        if(bRst == true)
        {
            memcpy(&pCard->u8Sn, &unData.stS7B0.u8Sn, sizeof(unData.stS7B0.u8Sn));
        }
        */
        pCard->eType = ePrivDrvCardTypeM1;
        pCard->stTime = sGetTime();
    }
    
    
    return bRst;
}


















































