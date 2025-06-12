/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   meter_drv_djsf2669.c
* Description                           :   青岛英利达电表驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   具体型号为:DJSF2669D 型双回路导轨式直流电能表
*                                           使用modbus-rtu协议,而不是DLT645规约
*示例报文-------实测
1:读电压电流功率----实测回复延迟约50-70ms
TX:01 03 20 3C 00 0C 8E 03
RX:01 03 18 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 6C F4 

2:读取1路电量---读取浮点数格式的电量数据时，地址01对应1路，地址02对应2路----实测回复延迟约700ms
TX:01 03 9A 00 00 1E EB 1A
RX:01 03 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7A D7 

3:读取2路电量---读取浮点数格式的电量数据时，地址01对应1路，地址02对应2路----实测回复延迟约200ms
TX:02 03 9A 00 00 1E EB 29
RX:02 03 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B9 D7 

4:读取日期时间
TX:01 03 B0 01 00 03 72 CB
RX:01 03 06 24 05 15 21 57 33 C1 B2 

5:电能清零---看来写单个寄存器也可以用0x10功能码嘛@2024年5月15日by Hall
TX:01 10 B0 00 00 01 02 5A 01 EC FB □
RX:01 10 B0 00 00 01 27 09 

6:写入日期时间
TX:01 10 B0 01 00 03 06 24 05 15 21 01 02 FF 34
RX:01 10 B0 01 00 03 F7 08 

****************************************************************************************************/
#include "meter_drv_djsf2669.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_djsf2669";


//---------------------------------------------------------------------------------------------------------

stMeterDjsf2669dCache_t *pDjsf2669dCache = NULL;







//---------------------------------------------------------------------------------------------------------
bool  sMeterDrvDjsf2669dInit(void);
void  sMeterDrvDjsf2669dSendTask(void *pParam);
void  sMeterDrvDjsf2669dRecvTask(void *pParam);
f32   sMeterDrvDjsf2669dProcData754(const u16 *pRegValue);
f32   sMeterDrvDjsf2669dProcDataInt(const u16 *pRegValue, bool bSignedFlag);


//特定寄存器的读写函数
bool sMeterDrvDjsf2669dReadUdc(u8 u8Addr);
bool sMeterDrvDjsf2669dReadEdc(u8 u8Addr);
bool sMeterDrvDjsf2669dReadTime(u8 u8Addr);

bool sMeterDrvDjsf2669dWriteRegClear(u8 u8Addr);
bool sMeterDrvDjsf2669dWriteRegTime(u8 u8Addr);


//报文接收和校验
void  sMeterDrvDjsf2669dRecv(i32 i32Len, const u8 *pData);
void  sMeterDrvDjsf2669dCheckCrc(i32 i32Len, const u8 *pData);


//命令发送
bool  sMeterDrvDjsf2669dSendCmd(u8 u8Addr, eMeterDjsf2669dFunCode_t eCode, eMeterDjsf2669dReg_t eReg, u16 u16RegNum, u8 u8Len, const u16 *pData, unMeterDjsf2669dPkt_t **pPkt);

//底层发送
bool  sMeterDrvDjsf2669dSend(u16 u16Len, const u8 *pBuf);
//---------------------------------------------------------------------------------------------------------










/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 通讯资源初始化
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dInit(void)
{
    bool bRst = false;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cMeterComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    bRst = sUsartDrvOpen(cMeterComUartNum, &stUsartCfg_t, cMeterDjsf2669dBufSize, cMeterDjsf2669dBufSize);
    
    pDjsf2669dCache = (stMeterDjsf2669dCache_t *)MALLOC(sizeof(stMeterDjsf2669dCache_t));
    memset(pDjsf2669dCache, 0, sizeof(stMeterDjsf2669dCache_t));
    pDjsf2669dCache->xSemAck = xSemaphoreCreateBinary();
    pDjsf2669dCache->xSemTxPkt = xSemaphoreCreateBinary();
    xSemaphoreGive(pDjsf2669dCache->xSemTxPkt);
    
    //先建立接收线程 再建立轮询线程
    xTaskCreate(sMeterDrvDjsf2669dRecvTask, "sMeterDrvDjsf2669dRecvTask", (512), NULL, 16, NULL);
    xTaskCreate(sMeterDrvDjsf2669dSendTask, "sMeterDrvDjsf2669dSendTask", (512), NULL, 16, NULL);
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 轮询任务
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   根据协议手册 依次读取所需要的电表数据
*
*                                           根据实测的报文延迟数据，全部数据读取一轮，耗时约1.2s
****************************************************************************************************/
void sMeterDrvDjsf2669dSendTask(void *pParam)
{
    i32 i;
    u32 u32Time = 0;
    stTime_t stTime;
    
    EN_SLOGD(TAG, "任务建立:英利达DJSF2669D电表轮询任务");
    
    while(1)
    {
        for(i = 0; i < cMeterNum; i++)
        {
            //电压电流功率字段一次读取双路
            sMeterDrvDjsf2669dReadUdc(i + 1);
            
            //电量字段按路读取，地址不一样
            sMeterDrvDjsf2669dReadEdc(2 * i + 1);
            sMeterDrvDjsf2669dReadEdc(2 * i + 2);
            
            //定期更新日期+时间---每10个小时执行一次就够了
            if((sGetTimestamp() - u32Time) >= (3600 * 10))
            {
                u32Time = sGetTimestamp();
                sMeterDrvDjsf2669dReadTime(i + 1);
                
                stTime = sGetTime();
                if((stTime.year != pDjsf2669dCache->stData[i].stTime.year)
                || (stTime.mon  != pDjsf2669dCache->stData[i].stTime.mon )
                || (stTime.day  != pDjsf2669dCache->stData[i].stTime.day )
                || (stTime.hour != pDjsf2669dCache->stData[i].stTime.hour)
                || (stTime.min  != pDjsf2669dCache->stData[i].stTime.min ))
                {
                    //误差达到1分钟就对时
                    sMeterDrvDjsf2669dWriteRegTime(i + 1);
                }
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}








/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 接收处理任务
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :     
****************************************************************************************************/
void sMeterDrvDjsf2669dRecvTask(void *pParam)
{
    i32 i32ReadLen;
    
    
    EN_SLOGD(TAG, "任务建立:英利达DJSF2669D电表接收任务");
    while(1 > 0)
    {
        memset(pDjsf2669dCache->u8Buf, 0, sizeof(pDjsf2669dCache->u8Buf));
        i32ReadLen = sUsartDrvRecv(cMeterComUartNum, sizeof(pDjsf2669dCache->u8Buf), pDjsf2669dCache->u8Buf);
        if(i32ReadLen > 0)
        {
            //接受到电表数据更新时间戳并取消通讯错误
            pDjsf2669dCache->u32RecvTimeStamp = xTaskGetTickCount();
            pDjsf2669dCache->bMeterFault_MCF = false;
            
            sMeterDrvDjsf2669dRecv(i32ReadLen, pDjsf2669dCache->u8Buf);
        }
        else
        {
            //30秒内未接收到电表回复则认为通讯故障
            if(xTaskGetTickCount() - pDjsf2669dCache->u32RecvTimeStamp >= cMeterDjsf2669dFaultTimeCheck)
            {
                pDjsf2669dCache->bMeterFault_MCF = true;
                memset(pDjsf2669dCache->stData, 0, sizeof(pDjsf2669dCache->stData));
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}








/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 寄存器数据处理 之 IEEE754浮点数处理
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   3FA00000
****************************************************************************************************/
f32 sMeterDrvDjsf2669dProcData754(const u16 *pRegValue)
{
    unIeee754_t unValue;
    
    unValue.u8Buf[3] = (ntohs(pRegValue[0]) & 0xff00) >> 8;
    unValue.u8Buf[2] = (ntohs(pRegValue[0]) & 0x00ff) >> 0;
    unValue.u8Buf[1] = (ntohs(pRegValue[1]) & 0xff00) >> 8;
    unValue.u8Buf[0] = (ntohs(pRegValue[1]) & 0x00ff) >> 0;
    
    return unValue.f32Value;
}








/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 寄存器数据处理 之 整数(integer) 转浮点数
* Author                                :   Hall
* Creat Date                            :   2024-05-16
* notice                                :   bSignedFlag:有符号标志
****************************************************************************************************/
f32 sMeterDrvDjsf2669dProcDataInt(const u16 *pRegValue, bool bSignedFlag)
{
    i32 i32Temp;
    u32 u32Temp;
    
    i32Temp = ntohs(pRegValue[0]);
    u32Temp = ntohs(pRegValue[0]);
    i32Temp = (i32Temp << 16) + ntohs(pRegValue[1]);
    u32Temp = (u32Temp << 16) + ntohs(pRegValue[1]);
    
    return ((bSignedFlag == true) ? ((f32)i32Temp) : ((f32)u32Temp));
}











/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 读取以下寄存器数据---电压电流功率
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   eMeterDjsf2669dRegU1H ~ eMeterDjsf2669dRegP2L
****************************************************************************************************/
bool sMeterDrvDjsf2669dReadUdc(u8 u8Addr)
{
    bool bRst;
    u16  u16MeterIndex1, u16MeterIndex2;
    u16  *pRegValue = NULL;
    
    unMeterDjsf2669dPkt_t *pPktRx = NULL;
    
    if((u8Addr > (cMeterNum * 2)) || (u8Addr == 0))
    {
        return(false);
    }
    
    //此命令读取的寄存器包含两路的电压电流功率
    u16MeterIndex1 = (2 * (u8Addr - 1)) + 0;                                    //第1路数据的索引
    u16MeterIndex2 = (2 * (u8Addr - 1)) + 1;                                    //第2路数据的索引
    bRst = sMeterDrvDjsf2669dSendCmd(u8Addr, eMeterDjsf2669dFunCodeRead, eMeterDjsf2669dRegU1H, 12, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        pRegValue = pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue;
        
        //f32Udc
        pDjsf2669dCache->stData[u16MeterIndex1].f32Udc = 0.0001f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[0 ]), true);
        pDjsf2669dCache->stData[u16MeterIndex2].f32Udc = 0.0001f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[2 ]), true);
        
        //f32Idc
        pDjsf2669dCache->stData[u16MeterIndex1].f32Idc = 0.0001f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[4 ]), true);
        pDjsf2669dCache->stData[u16MeterIndex2].f32Idc = 0.0001f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[6 ]), true);
        
        //f32Pdc
        pDjsf2669dCache->stData[u16MeterIndex1].f32Pdc = 0.1f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[8 ]), true);
        pDjsf2669dCache->stData[u16MeterIndex2].f32Pdc = 0.1f * sMeterDrvDjsf2669dProcDataInt((const u16 *)(&pRegValue[10]), true);
        
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 读取以下寄存器数据---读取电量数据
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   eMeterDjsf2669dRegEtH ~ eMeterDjsf2669dRegEnvL
****************************************************************************************************/
bool sMeterDrvDjsf2669dReadEdc(u8 u8Addr)
{
    bool bRst;
    u16  u16MeterIndex;
    u16  *pRegValue = NULL;
    
    unMeterDjsf2669dPkt_t *pPktRx = NULL;
    
    if((u8Addr > (cMeterNum * 2)) || (u8Addr == 0))
    {
        return(false);
    }
    
    //此命令分地址读取1路和2路的电量值,
    u16MeterIndex = u8Addr - 1;
    bRst = sMeterDrvDjsf2669dSendCmd(u8Addr, eMeterDjsf2669dFunCodeRead, eMeterDjsf2669dRegEtH, 30, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        pRegValue = pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue;
        
        pDjsf2669dCache->stData[u16MeterIndex].f32Et  = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[0 ]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Es  = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[2 ]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Ep  = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[4 ]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Ef  = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[6 ]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Ev  = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[8 ]));
        
        pDjsf2669dCache->stData[u16MeterIndex].f32Ept = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[10]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Eps = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[12]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Epp = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[14]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Epf = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[16]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Epv = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[18]));
        
        pDjsf2669dCache->stData[u16MeterIndex].f32Ent = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[20]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Ens = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[22]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Enp = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[24]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Enf = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[26]));
        pDjsf2669dCache->stData[u16MeterIndex].f32Env = sMeterDrvDjsf2669dProcData754((const u16 *)(&pRegValue[28]));
        
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 读取以下寄存器数据---读取电表时间
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   eMeterDjsf2669dRegTime1 ~ eMeterDjsf2669dRegTime3
****************************************************************************************************/
bool sMeterDrvDjsf2669dReadTime(u8 u8Addr)
{
    bool bRst;
    u16  u16MeterIndex1, u16MeterIndex2;
    u16  *pRegValue = NULL;
    
    unMeterDjsf2669dPkt_t *pPktRx = NULL;
    
    
    if((u8Addr > (cMeterNum * 2)) || (u8Addr == 0))
    {
        return(false);
    }
    
    
    u16MeterIndex1 = (2 * (u8Addr - 1)) + 0;                                    //第1路数据的索引
    u16MeterIndex2 = (2 * (u8Addr - 1)) + 1;                                    //第2路数据的索引
    bRst = sMeterDrvDjsf2669dSendCmd(u8Addr, eMeterDjsf2669dFunCodeRead, eMeterDjsf2669dRegTime1, 3, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        pRegValue = pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue;
        
        //第1路
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.year = sBcd2Hex((ntohs(pRegValue[0]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.mon  = sBcd2Hex((ntohs(pRegValue[0]) & 0x00ff) >> 0);
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.day  = sBcd2Hex((ntohs(pRegValue[1]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.hour = sBcd2Hex((ntohs(pRegValue[1]) & 0x00ff) >> 0);
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.min  = sBcd2Hex((ntohs(pRegValue[2]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex1].stTime.sec  = sBcd2Hex((ntohs(pRegValue[2]) & 0x00ff) >> 0);
        
        //第2路
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.year = sBcd2Hex((ntohs(pRegValue[0]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.mon  = sBcd2Hex((ntohs(pRegValue[0]) & 0x00ff) >> 0);
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.day  = sBcd2Hex((ntohs(pRegValue[1]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.hour = sBcd2Hex((ntohs(pRegValue[1]) & 0x00ff) >> 0);
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.min  = sBcd2Hex((ntohs(pRegValue[2]) & 0xff00) >> 8);
        pDjsf2669dCache->stData[u16MeterIndex2].stTime.sec  = sBcd2Hex((ntohs(pRegValue[2]) & 0x00ff) >> 0);
        
        
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 写以下寄存器数据---清电表电量数据
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   eMeterDjsf2669dRegClear
****************************************************************************************************/
bool sMeterDrvDjsf2669dWriteRegClear(u8 u8Addr)
{
    bool bRst;
    u16  u16Value = htons(cMeterDjsf2669dRegValueClearE);
    
    unMeterDjsf2669dPkt_t *pPktRx = NULL;
    
    if((u8Addr > (cMeterNum * 2)) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf2669dSendCmd(u8Addr, eMeterDjsf2669dFunCodeWriteM, eMeterDjsf2669dRegClear, 1, 2, (const u16 *)&u16Value, &pPktRx);
    bRst = bRst & (pPktRx->stPkt.stHead.eCode == eMeterDjsf2669dFunCodeWriteM);
    bRst = bRst & (ntohs(pPktRx->stPkt.unPayload.stFun10Ack.u16RegAddr) == eMeterDjsf2669dRegClear);
    bRst = bRst & (ntohs(pPktRx->stPkt.unPayload.stFun10Ack.u16RegNum) == 1);
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 写以下寄存器数据---电表对时
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   eMeterDjsf2669dRegTime1 ~ eMeterDjsf2669dRegTime3
****************************************************************************************************/
bool sMeterDrvDjsf2669dWriteRegTime(u8 u8Addr)
{
    bool bRst;
    u16  u16Temp;
    u16  u16Time[3];
    stTime_t stTime;
    
    unMeterDjsf2669dPkt_t *pPktRx = NULL;
    
    if((u8Addr > (cMeterNum * 2)) || (u8Addr == 0))
    {
        return(false);
    }
    
    stTime      = sGetTime();
    u16Temp     = sHex2Bcd(stTime.year);
    u16Temp     = (u16Temp << 8) + sHex2Bcd(stTime.mon);
    u16Time[0]  = htons(u16Temp);
    u16Temp     = sHex2Bcd(stTime.day);
    u16Temp     = (u16Temp << 8) + sHex2Bcd(stTime.hour);
    u16Time[1]  = htons(u16Temp);
    u16Temp     = sHex2Bcd(stTime.min);
    u16Temp     = (u16Temp << 8) + sHex2Bcd(stTime.sec);
    u16Time[2]  = htons(u16Temp);
    
    bRst = sMeterDrvDjsf2669dSendCmd(u8Addr, eMeterDjsf2669dFunCodeWriteM, eMeterDjsf2669dRegTime1, 3, 6, (const u16 *)u16Time, &pPktRx);
    bRst = bRst & (pPktRx->stPkt.stHead.eCode == eMeterDjsf2669dFunCodeWriteM);
    bRst = bRst & (ntohs(pPktRx->stPkt.unPayload.stFun10Ack.u16RegAddr) == eMeterDjsf2669dRegTime1);
    bRst = bRst & (ntohs(pPktRx->stPkt.unPayload.stFun10Ack.u16RegNum) == 3);
    
    return(bRst);
}














/**********************************************************************************************
* Description                           :   DJSF2669D电表驱动 命令发送函数
* Author                                :   Hall
* modified Date                         :   2024-05-15
* notice                                :   
*                                           u8Addr      :地址
*                                           eCode       :功能码
*                                           eReg        :寄存器地址
*                                           u16RegNum   :寄存器个数
*                                           u8Len       :写入命令时 待写数据长度,字节数
*                                           pData       :写入命令时 待写数据首地址
*                                           pPkt        :发送成功时将接收帧的首地址存入本指针---用于返回接收报文
***********************************************************************************************/
bool sMeterDrvDjsf2669dSendCmd(u8 u8Addr, eMeterDjsf2669dFunCode_t eCode, eMeterDjsf2669dReg_t eReg, u16 u16RegNum, u8 u8Len, const u16 *pData, unMeterDjsf2669dPkt_t **pPkt)
{
    bool bRst = false;
    u16  u16Reg;
    u16  u16Crc, u16Len;
    
    
    //0:参数保护
    if(((eCode != eMeterDjsf2669dFunCodeRead) && (eCode != eMeterDjsf2669dFunCodeWriteM)) || (eReg >= eMeterDjsf2669dRegMax) || (u16RegNum == 0) || (pPkt == NULL))
    {
        return(false);
    }
    
    //1:访问保护
    xSemaphoreTake(pDjsf2669dCache->xSemTxPkt, portMAX_DELAY);
    
    
    //2:填充报文
    u16Reg = (u16)eReg;
    memset(pDjsf2669dCache->unTxPkt.u8Buf, 0, sizeof(pDjsf2669dCache->unTxPkt));
    pDjsf2669dCache->unTxPkt.stPkt.stHead.u8Addr = u8Addr;
    pDjsf2669dCache->unTxPkt.stPkt.stHead.eCode = eCode;
    if(eCode == eMeterDjsf2669dFunCodeRead)
    {
        pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun03.u16RegAddr = htons(u16Reg);
        pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun03.u16RegNum = htons(u16RegNum);
        u16Len = cMeterDjsf2669dHeadSize + sizeof(u16Reg) + sizeof(u16RegNum);
    }
    else
    {
        pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun10.u16RegAddr = htons(u16Reg);
        pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun10.u16RegNum = htons(u16RegNum);
        pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun10.u8Len = u8Len;
        memcpy(pDjsf2669dCache->unTxPkt.stPkt.unPayload.stFun10.u16RegValue, pData, u8Len);
        u16Len = cMeterDjsf2669dHeadSize + sizeof(u16Reg) + sizeof(u16RegNum) + sizeof(u8Len) + u8Len;
    }
    
    //2.1:填充CRC
    u16Crc = sCrc16Modbus(pDjsf2669dCache->unTxPkt.u8Buf, u16Len);
    pDjsf2669dCache->unTxPkt.u8Buf[u16Len + 0] = (u16Crc >> 0) & 0xff;            //CRC低字节在前
    pDjsf2669dCache->unTxPkt.u8Buf[u16Len + 1] = (u16Crc >> 8) & 0xff;
    u16Len = u16Len + sizeof(u16Crc);
    
    
    //3:发送报文
    bRst = sMeterDrvDjsf2669dSend(u16Len, pDjsf2669dCache->unTxPkt.u8Buf);
    (*pPkt) = (bRst == false) ? NULL : (&pDjsf2669dCache->unRxPkt);
    
    
    //4:访问保护解除
    //这里延时解除保护 是为了让外部在同一线程内顺序调用发送函数或多线程同时调用发送函数时
    //有一定的发送间隔，否则电表无法响应命令
    vTaskDelay(20 / portTICK_RATE_MS);
    xSemaphoreGive(pDjsf2669dCache->xSemTxPkt);
    
    return(bRst);
}















/**********************************************************************************************
* Description                           :   DJSF2669D电表驱动 串口数据接收
* Author                                :   Hall
* modified Date                         :   2024-05-15
* notice                                :   
***********************************************************************************************/
void sMeterDrvDjsf2669dRecv(i32 i32Len, const u8 *pData)
{
    u32 i;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pDjsf2669dCache->i32RxIndex)
        {
        case 0:                                                                 //地址域 Address
            if((pData[i] > 0) && (pData[i] <= (2 * cMeterNum)))
            {
                pDjsf2669dCache->unRxPktTemp.u8Buf[pDjsf2669dCache->i32RxIndex] = pData[i];
                pDjsf2669dCache->i32RxIndex++;
            }
            break;
        case 1:                                                                 //功能码 eCode
            if((pData[i] == eMeterDjsf2669dFunCodeRead) || (pData[i] == eMeterDjsf2669dFunCodeWriteM))
            {
                pDjsf2669dCache->unRxPktTemp.u8Buf[pDjsf2669dCache->i32RxIndex] = pData[i];
                pDjsf2669dCache->i32RxIndex++;
            }
            else
            {
                pDjsf2669dCache->i32RxIndex = 0;
            }
            break;
        default:
            pDjsf2669dCache->unRxPktTemp.u8Buf[pDjsf2669dCache->i32RxIndex] = pData[i];
            pDjsf2669dCache->i32RxIndex++;
            if(pDjsf2669dCache->unRxPktTemp.u8Buf[1] == eMeterDjsf2669dFunCodeRead)
            {
                //                                 报头+校验和长度             + 数据长度                              + len本身长度
                if(pDjsf2669dCache->i32RxIndex >= (cMeterDjsf2669dHeadTailSize + pDjsf2669dCache->unRxPktTemp.u8Buf[2] + 1))
                {
                    memset(pDjsf2669dCache->unRxPkt.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPkt));
                    memcpy(pDjsf2669dCache->unRxPkt.u8Buf, pDjsf2669dCache->unRxPktTemp.u8Buf, pDjsf2669dCache->i32RxIndex);
                    sMeterDrvDjsf2669dCheckCrc(pDjsf2669dCache->i32RxIndex, pDjsf2669dCache->unRxPkt.u8Buf);
                    
                    memset(pDjsf2669dCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPktTemp));
                    pDjsf2669dCache->i32RxIndex = 0;
                }
                else if(pDjsf2669dCache->i32RxIndex >= sizeof(pDjsf2669dCache->unRxPktTemp))
                {
                    memset(pDjsf2669dCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPktTemp));
                    pDjsf2669dCache->i32RxIndex = 0;
                }
            }
            else if(pDjsf2669dCache->unRxPktTemp.u8Buf[1] == eMeterDjsf2669dFunCodeWriteM)
            {
                //                                 报头+校验和长度             + payload长度
                if(pDjsf2669dCache->i32RxIndex >= (cMeterDjsf2669dHeadTailSize + sizeof(stMeterDjsf2669dFun10Ack_t)))
                {
                    memset(pDjsf2669dCache->unRxPkt.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPkt));
                    memcpy(pDjsf2669dCache->unRxPkt.u8Buf, pDjsf2669dCache->unRxPktTemp.u8Buf, pDjsf2669dCache->i32RxIndex);
                    sMeterDrvDjsf2669dCheckCrc(pDjsf2669dCache->i32RxIndex, pDjsf2669dCache->unRxPkt.u8Buf);
                    
                    memset(pDjsf2669dCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPktTemp));
                    pDjsf2669dCache->i32RxIndex = 0;
                }
            }
            else if(pDjsf2669dCache->i32RxIndex >= sizeof(pDjsf2669dCache->unRxPktTemp))
            {
                memset(pDjsf2669dCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf2669dCache->unRxPktTemp));
                pDjsf2669dCache->i32RxIndex = 0;
            }
            break;
        }
    }

}








/**********************************************************************************************
* Description                           :   DJSF2669D电表驱动 串口数据包 CRC校验函数
* Author                                :   Hall
* modified Date                         :   2024-05-15
* notice                                :   
***********************************************************************************************/
void sMeterDrvDjsf2669dCheckCrc(i32 i32Len, const u8 *pData)
{
    u16  u16CrcRecv;
    u16  u16CrcCalc;
    char u8String[96];
    
    u16CrcRecv = *(pData + i32Len - 1);                                         //报文中CRC部分低字节在前
    u16CrcRecv = (u16CrcRecv << 8) + (*(pData + i32Len - 2));                   //             高字节在后
    
    u16CrcCalc = sCrc16Modbus(pData, i32Len - 2);
    if(u16CrcCalc == u16CrcRecv)
    {
        //解锁
        if(pDjsf2669dCache->xSemAck != NULL)
        {
            xSemaphoreGive(pDjsf2669dCache->xSemAck);
        }
    }
    else
    {
        memset(u8String, 0, sizeof(u8String));
        snprintf(u8String, sizeof(u8String), "Meter RX(len:%02d) 校验和错误,计算值:0x%04X,接收值:0x%04X--->", i32Len, u16CrcCalc, u16CrcRecv);
        EN_HLOGE(TAG, u8String, pData, i32Len);
    }
}









/***************************************************************************************************
* Description                           :   DJSF2669D电表驱动 底层发送函数
* Author                                :   Hall
* Creat Date                            :   2024-05-15
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf2669dSend(u16 u16Len, const u8 *pBuf)
{
    xSemaphoreTake(pDjsf2669dCache->xSemAck, 0);
    
    sUsartDrvSend(cMeterComUartNum, u16Len, pBuf);
    
    return xSemaphoreTake(pDjsf2669dCache->xSemAck, 3000 / portTICK_PERIOD_MS);
}














