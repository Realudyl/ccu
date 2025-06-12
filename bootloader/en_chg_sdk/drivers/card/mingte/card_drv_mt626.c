/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   card_drv_mt626.c
* Description                           :   铭特MT626刷卡器驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :   


示例报文-------实测
1:寻卡
[16:08:18.977]发→◇02 00 02 34 30 03 07 □
[16:08:18.991]收←◆02 00 03 34 30 59 03 5F 

2:获取Mefare1 卡序列号
[16:08:22.172]发→◇02 00 02 34 31 03 06 □
[16:08:22.175]收←◆02 00 07 34 31 59 10 A8 DB FA 03 C3 

3:验证扇区0的Key_A密码
[20:26:06.040]发→◇02 00 09 34 32 00 FF FF FF FF FF FF 03 0E □
[20:26:06.060]收←◆2D 3E 20 70 63 62 63 6F 6D 6D 4F 4B 0D 0A 02 00 09 34 32 00 59 FF FF FF FF FF 03 A8 
----------------←◆-> pcbcommOK02 00 09 34 32 00 59 FF FF FF FF FF 03 A8 


4:读取扇区0块0数据----
[2024-06-05 21:33:16.183] 02 00 04 34 33 00 00 03 02
[2024-06-05 21:33:16.193] 02 00 15 34 33 00 00 59 10 A8 DB FA 99 08 04 00 03 57 14 3A 1B 25 98 90 03 0A


5:验证扇区2的Key_A密码
[2024-06-05 21:19:21.921] 02 00 09 34 32 02 8A 72 64 53 4F 3C 03 B0
[2024-06-05 21:19:21.958] 2D 3E 20 70 63 62 63 6F 6D 6D 4F 4B 0D 0A 02 00 09 34 32 02 59 FF FF FF FF FF 03 AA
                          -> pcbcommOK02 00 09 34 32 02 59 FF FF FF FF FF 03 AA

6:读取扇区2块0数据
[2024-06-05 21:19:22.588] 02 00 04 34 33 02 00 03 00
[2024-06-05 21:19:22.598] 02 00 15 34 33 02 00 59 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 48

7:扇区6,7略


****************************************************************************************************/
#include "card_drv_mt626.h"








//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "card_drv_mt626";




stCardDrvMt626Cache_t *pCardDrvMt626Cache = NULL;









bool sCardDrvMt626Init(void);
void sCardDrvMt626RecvTask(void *pParam);


void sCardDrvMt626TailCalc(eCardDrvMt626Cmd_t eCmd, eCardDrvMt626CmdPara_t ePara, unCardDrvMt626Pkt_t *pTxPkt);


//M1卡操作命令---目前只用到以下几个命令
bool sCardDrvMt626M1Search(void);
bool sCardDrvMt626M1Mefare1(u32 *pSn);
bool sCardDrvMt626M1Key(eCardDrvMt626CmdPara_t ePara, u8 u8Sector, i32 i32Len, const u8 *pKey);
bool sCardDrvMt626M1Read(u8 u8Sector, u8 u8Block, i32 i32MaxLen, u8 *pBuf);
bool sCardDrvMt626M1Write(u8 u8Sector, u8 u8Block, i32 i32Len, const u8 *pData);






//报文接收和校验
void sCardDrvMt626Recv(i32 i32Len, const u8 *pData);
void sCardDrvMt626CheckBcc(i32 i32Len, const u8 *pData);

bool sCardDrvMt626SendCmd(eCardDrvMt626Cmd_t eCmd, eCardDrvMt626CmdPara_t ePara, i32 i32PayloadLen, unCardDrvMt626Payload_t *pPayload);

//底层发送
bool  sCardDrvMt626Send(u16 u16Len, const u8 *pBuf);





//对外函数 获取当前读卡器在线状态
bool sCardDrvMt626GetOnLineState(void);




/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动--->初始化
* Author                                :   Hall
* modified Date                         :   2024-06-04
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626Init(void)
{
    bool bRst = false;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cCardComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    bRst = sUsartDrvOpen(cCardComUartNum, &stUsartCfg_t, cCardDrvMt626BufSize, cCardDrvMt626BufSize);
    
    pCardDrvMt626Cache = MALLOC(sizeof(stCardDrvMt626Cache_t));
    memset(pCardDrvMt626Cache, 0, sizeof(stCardDrvMt626Cache_t));
    pCardDrvMt626Cache->xSemAck = xSemaphoreCreateBinary();
    pCardDrvMt626Cache->xSemTxPkt = xSemaphoreCreateBinary();
    xSemaphoreGive(pCardDrvMt626Cache->xSemTxPkt);
    
    //建立接收线程
    xTaskCreate(sCardDrvMt626RecvTask, "sCardDrvMt626RecvTask", (512), NULL, 16, NULL);
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 接收处理任务
* Author                                :   Hall
* Creat Date                            :   2024-06-04
* notice                                :     
****************************************************************************************************/
void sCardDrvMt626RecvTask(void *pParam)
{
    i32 i32ReadLen;
    
    
    EN_SLOGD(TAG, "任务建立:铭特MT626刷卡器驱动 接收任务");
    while(1)
    {
        memset(pCardDrvMt626Cache->u8Buf, 0, sizeof(pCardDrvMt626Cache->u8Buf));
        i32ReadLen = sUsartDrvRecv(cCardComUartNum, sizeof(pCardDrvMt626Cache->u8Buf), pCardDrvMt626Cache->u8Buf);
        if(i32ReadLen > 0)
        {
            sCardDrvMt626Recv(i32ReadLen, pCardDrvMt626Cache->u8Buf);
            
            //有回复就认为读卡器正常
            pCardDrvMt626Cache->bOnlineStatus = true;
        }
        else 
        {
            //读卡器断线
            pCardDrvMt626Cache->bOnlineStatus = false;
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 帧尾BCC校验值计算和填充
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
void sCardDrvMt626TailCalc(eCardDrvMt626Cmd_t eCmd, eCardDrvMt626CmdPara_t ePara, unCardDrvMt626Pkt_t *pTxPkt)
{
    u16  u16BccLen;
    stCardDrvMt626Tail_t *pTail = NULL;
    
    if(eCmd == eCardDrvMt626CmdCpuA_m1)
    {
        switch(ePara)
        {
        case eCardDrvMt626CmdParaSearch:
        case eCardDrvMt626CmdParaMefare1:
            pTail = (stCardDrvMt626Tail_t *)&pTxPkt->stPkt.unPayload.u8Buf[sizeof(stCardDrvMt626CmdBasic_t)];
            break;
        case eCardDrvMt626CmdParaKeyA:
        case eCardDrvMt626CmdParaKeyB:
            pTail = (stCardDrvMt626Tail_t *)&pTxPkt->stPkt.unPayload.u8Buf[sizeof(stCardDrvMt626CmdKey_t)];
            break;
        case eCardDrvMt626CmdRead:
            pTail = (stCardDrvMt626Tail_t *)&pTxPkt->stPkt.unPayload.u8Buf[sizeof(stCardDrvMt626CmdRead_t)];
            break;
        case eCardDrvMt626CmdWrite:
            pTail = (stCardDrvMt626Tail_t *)&pTxPkt->stPkt.unPayload.u8Buf[sizeof(stCardDrvMt626CmdWrite_t)];
            break;
        default:
            break;
        }
    }
    
    if(pTail != NULL)
    {
        u16BccLen = ntohs(pTxPkt->stPkt.stHead.u16Len) + cCardDrvMt626HeadSize + cCardDrvMt626TailSize - 1;
        pTail->eEtx = eCardDrvMt626Etx;
        pTail->u8Bcc = sCheckBcc8(pTxPkt->u8Buf, u16BccLen);
    }
}














/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 寻卡命令
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626M1Search(void)
{
    bool bRst;
    stCardDrvMt626CmdBasicAck_t *pSearchAck = &pCardDrvMt626Cache->unRxPkt.stPkt.unPayload.stSearchAck;
    
    unCardDrvMt626Payload_t unPayload;
    
    memset(&unPayload, 0, sizeof(unPayload));
    unPayload.stSearch.eCmd = eCardDrvMt626CmdCpuA_m1;
    unPayload.stSearch.ePara = eCardDrvMt626CmdParaSearch;
    bRst = sCardDrvMt626SendCmd(eCardDrvMt626CmdCpuA_m1, eCardDrvMt626CmdParaSearch, sizeof(unPayload.stSearch), &unPayload);
    
    //接收数据解析
    if(bRst == true)
    {
        bRst = (pSearchAck->eOp == eCardDrvMt626OpRstSuccess) ? true : false;
    }
    
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 获取Mefare1 卡序列号命令
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626M1Mefare1(u32 *pSn)
{
    bool bRst;
    stCardDrvMt626CmdMefare1Ack_t *pMefare1Ack = &pCardDrvMt626Cache->unRxPkt.stPkt.unPayload.stMefare1Ack;
    
    unCardDrvMt626Payload_t unPayload;
    
    memset(&unPayload, 0, sizeof(unPayload));
    unPayload.stMefare1.eCmd = eCardDrvMt626CmdCpuA_m1;
    unPayload.stMefare1.ePara = eCardDrvMt626CmdParaMefare1;
    bRst = sCardDrvMt626SendCmd(eCardDrvMt626CmdCpuA_m1, eCardDrvMt626CmdParaMefare1, sizeof(unPayload.stMefare1), &unPayload);
    
    //接收数据解析
    if(bRst == true)
    {
        if((pSn != NULL) && (pMefare1Ack->eOp == eCardDrvMt626OpRstSuccess))
        {
            (*pSn) =  ntohl(pMefare1Ack->u32Mefare1Sn);
        }
        bRst   = (pMefare1Ack->eOp == eCardDrvMt626OpRstSuccess) ? true : false;
    }
    
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 验证Key_A/B密码命令
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626M1Key(eCardDrvMt626CmdPara_t ePara, u8 u8Sector, i32 i32Len, const u8 *pKey)
{
    bool bRst;
    stCardDrvMt626CmdKeyAck_t *pKeyAck = &pCardDrvMt626Cache->unRxPkt.stPkt.unPayload.stKeyAck;
    
    unCardDrvMt626Payload_t unPayload;
    
    if((i32Len > sizeof(unPayload.stKey.u8Key)) || (pKey == NULL))
    {
        return false;
    }
    
    memset(&unPayload, 0, sizeof(unPayload));
    unPayload.stKey.eCmd = eCardDrvMt626CmdCpuA_m1;
    unPayload.stKey.ePara = ePara;
    unPayload.stKey.u8Sector = u8Sector;
    memcpy(unPayload.stKey.u8Key, pKey, i32Len);
    bRst = sCardDrvMt626SendCmd(eCardDrvMt626CmdCpuA_m1, ePara, sizeof(unPayload.stKey), &unPayload);
    
    //接收数据解析
    if(bRst == true)
    {
        bRst = (pKeyAck->eOp == eCardDrvMt626OpRstSuccess) ? true : false;
    }
    
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 读扇区块数据命令
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626M1Read(u8 u8Sector, u8 u8Block, i32 i32MaxLen, u8 *pBuf)
{
    bool bRst;
    u16  u16DataLen;
    stCardDrvMt626CmdReadAckSuccess_t *pReadAck = &pCardDrvMt626Cache->unRxPkt.stPkt.unPayload.stReadSuccess;
    
    unCardDrvMt626Payload_t unPayload;
    
    memset(&unPayload, 0, sizeof(unPayload));
    unPayload.stRead.eCmd = eCardDrvMt626CmdCpuA_m1;
    unPayload.stRead.ePara = eCardDrvMt626CmdRead;
    unPayload.stRead.u8Sector = u8Sector;
    unPayload.stRead.u8Block = u8Block;
    bRst = sCardDrvMt626SendCmd(eCardDrvMt626CmdCpuA_m1, eCardDrvMt626CmdRead, sizeof(unPayload.stRead), &unPayload);
    
    //接收数据解析
    if(bRst == true)
    {
        if((pBuf != NULL) && (pReadAck->eOp == eCardDrvMt626OpRstSuccess))
        {
            u16DataLen = (i32MaxLen >= sizeof(pReadAck->u8Data)) ? sizeof(pReadAck->u8Data) : i32MaxLen;
            memcpy(pBuf, pReadAck->u8Data, u16DataLen);
        }
        bRst = (pReadAck->eOp == eCardDrvMt626OpRstSuccess) ? true : false;
        
    }
    
    
    return(bRst);
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 写扇区块数据命令
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626M1Write(u8 u8Sector, u8 u8Block, i32 i32Len, const u8 *pData)
{
    bool bRst;
    u16  u16DataLen;
    stCardDrvMt626CmdWriteAck_t *pWriteAck = &pCardDrvMt626Cache->unRxPkt.stPkt.unPayload.stWriteAck;
    
    unCardDrvMt626Payload_t unPayload;
    
    if((i32Len > sizeof(unPayload.stWrite.u8Data)) || (pData == NULL))
    {
        return false;
    }
    
    memset(&unPayload, 0, sizeof(unPayload));
    unPayload.stWrite.eCmd = eCardDrvMt626CmdCpuA_m1;
    unPayload.stWrite.ePara = eCardDrvMt626CmdWrite;
    unPayload.stWrite.u8Sector = u8Sector;
    unPayload.stWrite.u8Block = u8Block;
    bRst = sCardDrvMt626SendCmd(eCardDrvMt626CmdCpuA_m1, eCardDrvMt626CmdRead, sizeof(unPayload.stRead), &unPayload);
    
    //接收数据解析
    if(bRst == true)
    {
        bRst = (pWriteAck->eOp == eCardDrvMt626OpRstSuccess) ? true : false;
    }
    
    
    return(bRst);
}

















/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 串口数据接收
* Author                                :   Hall
* modified Date                         :   2024-06-04
* notice                                :   
***********************************************************************************************/
void sCardDrvMt626Recv(i32 i32Len, const u8 *pData)
{
    u32 i;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pCardDrvMt626Cache->i32RxIndex)
        {
        case 0:                                                                 //Stx
            if(pData[i] == eCardDrvMt626Stx)
            {
                pCardDrvMt626Cache->unRxPktTemp.u8Buf[pCardDrvMt626Cache->i32RxIndex] = pData[i];
                pCardDrvMt626Cache->i32RxIndex++;
            }
            break;
        case 3:                                                                 //命令字
            if((pData[i] == eCardDrvMt626CmdSys)
            || (pData[i] == eCardDrvMt626CmdCpuA_m1)
            || (pData[i] == eCardDrvMt626CmdCpuB)
            || (pData[i] == eCardDrvMt626CmdSim))
            {
                pCardDrvMt626Cache->unRxPktTemp.u8Buf[pCardDrvMt626Cache->i32RxIndex] = pData[i];
                pCardDrvMt626Cache->i32RxIndex++;
            }
            else
            {
                pCardDrvMt626Cache->i32RxIndex = 0;
            }
            break;
        case 4:                                                                 //命令参数
            if((pData[i] == eCardDrvMt626CmdParaBuzz)
            || (pData[i] == eCardDrvMt626CmdParaGetVer)
            || (pData[i] == eCardDrvMt626CmdParaSoftRst)
            || (pData[i] == eCardDrvMt626CmdParaActive)
            || (pData[i] == eCardDrvMt626CmdParaApdu)
            || (pData[i] == eCardDrvMt626CmdParaDeSelect)
            || (pData[i] == eCardDrvMt626CmdParaSearch)
            || (pData[i] == eCardDrvMt626CmdParaMefare1)
            || (pData[i] == eCardDrvMt626CmdParaKeyA)
            || (pData[i] == eCardDrvMt626CmdRead)
            || (pData[i] == eCardDrvMt626CmdWrite)
            || (pData[i] == eCardDrvMt626CmdChangeKey)
            || (pData[i] == eCardDrvMt626CmdValueAdd)
            || (pData[i] == eCardDrvMt626CmdValueSub)
            || (pData[i] == eCardDrvMt626CmdParaKeyB)
            || (pData[i] == eCardDrvMt626CmdParaReset3)
            || (pData[i] == eCardDrvMt626CmdParaReset5)
            || (pData[i] == eCardDrvMt626CmdParaPowerOff)
            || (pData[i] == eCardDrvMt626CmdParaApdu_C))
            {
                pCardDrvMt626Cache->unRxPktTemp.u8Buf[pCardDrvMt626Cache->i32RxIndex] = pData[i];
                pCardDrvMt626Cache->i32RxIndex++;
            }
            else
            {
                pCardDrvMt626Cache->i32RxIndex = 0;
            }
            break;
        default:
            pCardDrvMt626Cache->unRxPktTemp.u8Buf[pCardDrvMt626Cache->i32RxIndex] = pData[i];
            pCardDrvMt626Cache->i32RxIndex++;
            if(pCardDrvMt626Cache->i32RxIndex < sizeof(pCardDrvMt626Cache->unRxPktTemp))
            {
                //                                   payload长度                                          + 帧head长度            + 帧tail长度
                if(pCardDrvMt626Cache->i32RxIndex >= (ntohs(pCardDrvMt626Cache->unRxPktTemp.stPkt.stHead.u16Len) + cCardDrvMt626HeadSize + cCardDrvMt626TailSize))
                {
                    memset(pCardDrvMt626Cache->unRxPkt.u8Buf, 0, sizeof(pCardDrvMt626Cache->unRxPkt));
                    memcpy(pCardDrvMt626Cache->unRxPkt.u8Buf, &pCardDrvMt626Cache->unRxPktTemp.u8Buf, pCardDrvMt626Cache->i32RxIndex);
                    sCardDrvMt626CheckBcc(pCardDrvMt626Cache->i32RxIndex, pCardDrvMt626Cache->unRxPkt.u8Buf);
                    
                    memset(pCardDrvMt626Cache->unRxPktTemp.u8Buf, 0, sizeof(pCardDrvMt626Cache->unRxPktTemp));
                    pCardDrvMt626Cache->i32RxIndex = 0;
                }
            }
            else
            {
                pCardDrvMt626Cache->i32RxIndex = 0;
            }
            break;
        }
    }
}






/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 串口数据包 Bcc校验函数
* Author                                :   Dai
* modified Date                         :   2024-06-04
* notice                                :   
***********************************************************************************************/
void sCardDrvMt626CheckBcc(i32 i32Len, const u8 *pData)
{
    u8   u8Etx;
    u8   u8BccRecv;
    u8   u8BccCalc = 0;
    char u8String[96];
    
    u8BccRecv = pData[i32Len - 1];
    u8Etx     = pData[i32Len - 2];
    
    u8BccCalc = sCheckBcc8(pData, i32Len - 1);
    if((u8BccRecv == u8BccCalc) && (u8Etx == eCardDrvMt626Etx))
    {
        //解锁
        if(pCardDrvMt626Cache->xSemAck != NULL)
        {
            xSemaphoreGive(pCardDrvMt626Cache->xSemAck);
        }
        
    }
    else
    {
        memset(u8String, 0, sizeof(u8String));
        snprintf(u8String, sizeof(u8String), "CardReader RX(len:%02d) 校验错误,计算值:%d,接收值:%d--->", i32Len, u8BccRecv, u8BccCalc);
        EN_HLOGE(TAG, u8String, pData, i32Len);
    }
}














/**********************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 命令发送函数
* Author                                :   Hall
* modified Date                         :   2024-06-05
* notice                                :   
***********************************************************************************************/
bool sCardDrvMt626SendCmd(eCardDrvMt626Cmd_t eCmd, eCardDrvMt626CmdPara_t ePara, i32 i32PayloadLen, unCardDrvMt626Payload_t *pPayload)
{
    bool bRst;
    u16  u16FrameLen;                                                           //帧长度
    unCardDrvMt626Pkt_t *pTxPkt = &pCardDrvMt626Cache->unTxPkt;
    
    //1:访问保护
    xSemaphoreTake(pCardDrvMt626Cache->xSemTxPkt, portMAX_DELAY);
    
    
    //2:填充报文
    memset(pTxPkt, 0, sizeof(unCardDrvMt626Pkt_t));
    pTxPkt->stPkt.stHead.eStx = eCardDrvMt626Stx;
    pTxPkt->stPkt.stHead.u16Len = htons(i32PayloadLen);
    memcpy(pTxPkt->stPkt.unPayload.u8Buf, pPayload, i32PayloadLen);
    sCardDrvMt626TailCalc(eCmd, ePara, pTxPkt);
    
    u16FrameLen = i32PayloadLen + cCardDrvMt626HeadSize + cCardDrvMt626TailSize;
    
    
    //3:发送报文
    bRst = sCardDrvMt626Send(u16FrameLen, pCardDrvMt626Cache->unTxPkt.u8Buf);
    
    
    //4:访问保护解除
    xSemaphoreGive(pCardDrvMt626Cache->xSemTxPkt);
    
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 底层发送函数
* Author                                :   Hall
* Creat Date                            :   2024-06-05
* notice                                :   
****************************************************************************************************/
bool sCardDrvMt626Send(u16 u16Len, const u8 *pBuf)
{
    xSemaphoreTake(pCardDrvMt626Cache->xSemAck, 0);
    
    sUsartDrvSend(cSdkCardComUartNum, u16Len, pBuf);
    
    return xSemaphoreTake(pCardDrvMt626Cache->xSemAck, 100 / portTICK_PERIOD_MS);
}










/***************************************************************************************************
* Description                           :   铭特MT626刷卡器驱动 获取读卡器在线状态
* Author                                :   Dai
* Creat Date                            :   2024-07-19
* notice                                :   
****************************************************************************************************/
bool sCardDrvMt626GetOnLineState(void)
{
    return pCardDrvMt626Cache->bOnlineStatus;
}

























