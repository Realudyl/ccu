/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   meter_drv_djsf1352.c
* Description                           :   安科瑞电表驱动
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-08-09
* notice                                :   具体型号为:DJSF1352-RN 导轨式直流电能表
*                                           使用modbus协议,而不是DLT645规约
*示例报文-------实测
*读寄存器命令---读直流电压电流
*发送：01 03 00 00 00 06 C5 C8
*接收：01 03 0C 00 00 00 03 00 00 00 02 00 00 01 4D 3F E5
*
*读寄存器命令---读功率
*发送：01 03 00 08 00 02 45 C9
*接收：01 03 04 00 00 00 05 3A 30
*
*读寄存器命令---读有功电能
*发送：01 03 00 0C 00 08 84 0F
*接收：01 03 10 00 00 52 1A 00 00 00 00 00 01 00 FA 00 00 00 00 59 85
*
*读寄存器命令---读日期、时间、当前抄表日、当前费率
*发送：01 03 00 1E 00 04 24 0F
*接收：01 03 08 18 03 13 0E 14 09 01 03 59 7C
*
*读寄存器命令---读软件版本号
*发送：01 03 00 23 00 01 75 C0
*接收：01 03 02 01 03 F9 D5
*
*写寄存器命令---对时
*发送：01 10 00 1E 00 03 06 17 03 13 0E 18 00 AE 80
*接收：01 10 00 1E 00 03 E0 0E
****************************************************************************************************/
#include "meter_drv_djsf1352.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "meter_drv_djsf1352";


//---------------------------------------------------------------------------------------------------------

//为安科瑞DJSF1352-RN电表分配一个结构体
//仅限在本文件内部使用 禁止其他文件直接访问本结构体
stMeterDjsf1352rnCache_t *pDjsf1352rnCache = NULL;







//---------------------------------------------------------------------------------------------------------
bool  sMeterDrvDjsf1352rnInit(void);
void  sMeterDrvDjsf1352rnSendTask(void *pParam);
void  sMeterDrvDjsf1352rnRecvTask(void *pParam);


//特定寄存器的读写函数
bool sMeterDrvDjsf1352rnReadUdc(u8 u8Addr);
bool sMeterDrvDjsf1352rnReadPower(u8 u8Addr);
bool sMeterDrvDjsf1352rnReadEposH(u8 u8Addr);
bool sMeterDrvDjsf1352rnReadTime(u8 u8Addr);
bool sMeterDrvDjsf1352rnReadSwVer(u8 u8Addr);

bool sMeterDrvDjsf1352rnWriteRegTime(u8 u8Addr);


//报文接收和校验
void  sMeterDrvDjsf1352rnRecv(i32 i32Len, const u8 *pData);
void  sMeterDrvDjsf1352rnCheckCrc(i32 i32Len, const u8 *pData);


//命令发送
bool  sMeterDrvDjsf1352rnSendCmd(u8 u8Addr, eMeterDjsf1352rnFunction_t eFunction, eMeterDjsf1352rnReg_t eReg, u16 u16RegNum, u8 u8Len, const u16 *pData, unMeterDjsf1352rnPkt_t **pPkt);

//底层发送
bool  sMeterDrvDjsf1352rnSend(u16 u16Len, const u8 *pBuf);
//---------------------------------------------------------------------------------------------------------










/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 通讯资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-08-09
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnInit(void)
{
    bool bRst = false;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cMeterComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    bRst = sUsartDrvOpen(cMeterComUartNum, &stUsartCfg_t, cMeterDjsf1352rnBufSize, cMeterDjsf1352rnBufSize);
    
    pDjsf1352rnCache = (stMeterDjsf1352rnCache_t *)MALLOC(sizeof(stMeterDjsf1352rnCache_t));
    memset(pDjsf1352rnCache, 0, sizeof(stMeterDjsf1352rnCache_t));
    pDjsf1352rnCache->xSemAck = xSemaphoreCreateBinary();
    pDjsf1352rnCache->xSemTxPkt = xSemaphoreCreateBinary();
    xSemaphoreGive(pDjsf1352rnCache->xSemTxPkt);
    
    //先建立接收线程 再建立轮询线程
    xTaskCreate(sMeterDrvDjsf1352rnRecvTask, "sMeterDrvDjsf1352rnRecvTask", (512), NULL, 16, NULL);
    xTaskCreate(sMeterDrvDjsf1352rnSendTask, "sMeterDrvDjsf1352rnSendTask", (512), NULL, 16, NULL);
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 轮询任务
* Author                                :   Hall
* Creat Date                            :   2022-08-09
* notice                                :   根据协议手册 依次读取所需要的电表数据
****************************************************************************************************/
void sMeterDrvDjsf1352rnSendTask(void *pParam)
{
    i32 i;
    
    EN_SLOGD(TAG, "任务建立:安科瑞DJSF1352-RN电表轮询任务");
    
    while(1)
    {
        for(i = 0; i < cMeterNum; i++)
        {
            sMeterDrvDjsf1352rnReadUdc(i + 1);
            sMeterDrvDjsf1352rnReadPower(i + 1);
            sMeterDrvDjsf1352rnReadEposH(i + 1);
            sMeterDrvDjsf1352rnReadTime(i + 1);
            sMeterDrvDjsf1352rnReadSwVer(i + 1);
        }
        
        vTaskDelay(300 / portTICK_RATE_MS);
    }
}








/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 接收处理任务
* Author                                :   Hall
* Creat Date                            :   2022-08-09
* notice                                :     
****************************************************************************************************/
void sMeterDrvDjsf1352rnRecvTask(void *pParam)
{
    i32 i32ReadLen;
    
    
    EN_SLOGD(TAG, "任务建立:安科瑞DJSF1352-RN电表接收任务");
    while(1 > 0)
    {
        memset(pDjsf1352rnCache->u8Buf, 0, sizeof(pDjsf1352rnCache->u8Buf));
        i32ReadLen = sUsartDrvRecv(cMeterComUartNum, sizeof(pDjsf1352rnCache->u8Buf), pDjsf1352rnCache->u8Buf);
        if(i32ReadLen > 0)
        {
            //接受到电表数据更新时间戳并取消通讯错误
            pDjsf1352rnCache->u32RecvTimeStamp = xTaskGetTickCount();
            pDjsf1352rnCache->bMeterFault_MCF = false;
            
            sMeterDrvDjsf1352rnRecv(i32ReadLen, pDjsf1352rnCache->u8Buf);
        }
        else
        {
            //30秒内未接收到电表回复则认为通讯故障
            if(xTaskGetTickCount() - pDjsf1352rnCache->u32RecvTimeStamp >= cMeterDjsf1352rnFaultTimeCheck)
            {
                pDjsf1352rnCache->bMeterFault_MCF = true;
                memset(pDjsf1352rnCache->stData, 0, sizeof(pDjsf1352rnCache->stData));
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}












/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 读取以下寄存器数据---电压电流温度
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegUdc ~ eMeterDjsf1352rnRegTinter
****************************************************************************************************/
bool sMeterDrvDjsf1352rnReadUdc(u8 u8Addr)
{
    bool   bRst;
    u16    u16RegValue1, u16RegValue2;
    f32    f32Temp1, f32Temp2;
    f64    f64Temp;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionRead, eMeterDjsf1352rnRegUdc, 6, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        //f32Udc
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[0]);
        u16RegValue2 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[1]);
        f32Temp1 = (f32)u16RegValue1;
        f64Temp = pow(10, u16RegValue2 - 3);
        f32Temp2 = (f32)f64Temp;
        pDjsf1352rnCache->stData[u8Addr - 1].f32Udc = f32Temp1 * f32Temp2;
        
        //f32Idc
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[2]);
        u16RegValue2 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[3]);
        f32Temp1 = (f32)u16RegValue1;
        f64Temp = pow(10, u16RegValue2 - 3);
        f32Temp2 = (f32)f64Temp;
        pDjsf1352rnCache->stData[u8Addr - 1].f32Idc = f32Temp1 * f32Temp2;
        
        //eBreaklineSts
        pDjsf1352rnCache->stData[u8Addr - 1].eBreaklineSts = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[4]);
        
        //f32Tmeter
        pDjsf1352rnCache->stData[u8Addr - 1].f32Tmeter = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[5]) * 0.1;
    }
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 读取以下寄存器数据---功率
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegPower ~ eMeterDjsf1352rnRegPowerDp
****************************************************************************************************/
bool sMeterDrvDjsf1352rnReadPower(u8 u8Addr)
{
    bool   bRst;
    u16    u16RegValue1, u16RegValue2;
    f32    f32Temp1, f32Temp2;
    f64    f64Temp;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionRead, eMeterDjsf1352rnRegPower, 2, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        //f32Pdc
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[0]);
        u16RegValue2 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[1]);
        f32Temp1 = (f32)u16RegValue1;
        f64Temp = pow(10, u16RegValue2 - 3);
        f32Temp2 = (f32)f64Temp;
        pDjsf1352rnCache->stData[u8Addr - 1].f32Pdc = f32Temp1 * f32Temp2;
    }
    
    return(bRst);
}






/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 读取以下寄存器数据---
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegEposH ~ eMeterDjsf1352rnRegWarningSts
****************************************************************************************************/
bool sMeterDrvDjsf1352rnReadEposH(u8 u8Addr)
{
    bool    bRst;
    u16     u16RegValue1, u16RegValue2;
    u32     u32Temp;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionRead, eMeterDjsf1352rnRegEposH, 8, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        //u32Epos
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[0]);
        u16RegValue2 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[1]);
        u32Temp = (u16RegValue1 & 0xffff);
        u32Temp = (u32Temp << 16) + (u16RegValue2 & 0xffff);
        pDjsf1352rnCache->stData[u8Addr - 1].u32Epos = u32Temp;
        
        //u32Eneg
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[2]);
        u16RegValue2 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[3]);
        u32Temp = (u16RegValue1 & 0xffff);
        u32Temp = (u32Temp << 16) + (u16RegValue2 & 0xffff);
        pDjsf1352rnCache->stData[u8Addr - 1].u32Eneg = u32Temp;
        
        //u16UdcRatio
        pDjsf1352rnCache->stData[u8Addr - 1].u16UdcRatio = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[4]);
        
        //u16IdcRated1
        pDjsf1352rnCache->stData[u8Addr - 1].u16IdcRated1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[5]);
        
        //unDiDoSts
        pDjsf1352rnCache->stData[u8Addr - 1].unDiDoSts.u16Value = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[6]);
        
        //unWarningSts
        pDjsf1352rnCache->stData[u8Addr - 1].unWarningSts.u16Value = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[7]);
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 读取以下寄存器数据---日期和时间、抄表日、费率类型
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegTime1 ~ eMeterDjsf1352rnRegMeterReadInfo
****************************************************************************************************/
bool sMeterDrvDjsf1352rnReadTime(u8 u8Addr)
{
    bool   bRst;
    u16    u16RegValue1, u16RegValue2;
    float  f32Temp1, f32Temp2;
    double f64Temp;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionRead, eMeterDjsf1352rnRegTime1, 4, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        //u8Time
        memcpy(&pDjsf1352rnCache->stData[u8Addr - 1].stTime, pPktRx->stPkt.unPayload.u8Buf + 1, 6);
        
        //u8MeterReadingDay
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[3]);
        pDjsf1352rnCache->stData[u8Addr - 1].u8ReadingDay = (u16RegValue1 >> 8) & 0xff;
        
        //eRate
        pDjsf1352rnCache->stData[u8Addr - 1].eRate = (u16RegValue1 >> 0) & 0xff;
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 读取以下寄存器数据---软件版本
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegSwVer
****************************************************************************************************/
bool sMeterDrvDjsf1352rnReadSwVer(u8 u8Addr)
{
    bool   bRst;
    u16    u16RegValue1, u16RegValue2;
    float  f32Temp1, f32Temp2;
    double f64Temp;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionRead, eMeterDjsf1352rnRegSwVer, 1, 0, NULL, &pPktRx);
    if(bRst == true)
    {
        //u16SwVer
        u16RegValue1 = ntohs(pPktRx->stPkt.unPayload.stFun03Ack.u16RegValue[0]);
        pDjsf1352rnCache->stData[u8Addr - 1].u16SwVer = u16RegValue1;
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 写以下寄存器数据---电表对时
* Author                                :   Hall
* Creat Date                            :   2024-03-18
* notice                                :   eMeterDjsf1352rnRegTime1 ~ eMeterDjsf1352rnRegTime3
****************************************************************************************************/
bool sMeterDrvDjsf1352rnWriteRegTime(u8 u8Addr)
{
    bool bRst;
    stTime_t stTime;
    
    unMeterDjsf1352rnPkt_t *pPktRx = NULL;
    
    if((u8Addr > cMeterNum) || (u8Addr == 0))
    {
        return(false);
    }
    
    stTime = sGetTime();
    bRst = sMeterDrvDjsf1352rnSendCmd(u8Addr, eMeterAcrelFunctionWrite, eMeterDjsf1352rnRegTime1, 3, 6, (const u16 *)&stTime, &pPktRx);
    bRst = bRst & (pPktRx->stPkt.stHead.eFunction == eMeterAcrelFunctionWrite);
    bRst = bRst & (ntohs(pPktRx->stPkt.unPayload.stFun10Ack.u16RegAddr) == eMeterDjsf1352rnRegTime1);
    bRst = bRst & ntohs((pPktRx->stPkt.unPayload.stFun10Ack.u16RegNum) == 3);
    
    return(bRst);
}














/**********************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 命令发送函数
* Author                                :   Hall
* modified Date                         :   2022-08-10
* notice                                :   
*                                           u8Addr      :地址域
*                                           eFunction   :功能域
*                                           eReg        :寄存器地址
*                                           u16RegNum   :寄存器个数
*                                           u8Len       :写入命令时 待写数据长度,字节数
*                                           pData       :写入命令时 待写数据首地址
*                                           pPkt        :发送成功时将接收帧的首地址存入本指针---用于返回接收报文
***********************************************************************************************/
bool sMeterDrvDjsf1352rnSendCmd(u8 u8Addr, eMeterDjsf1352rnFunction_t eFunction, eMeterDjsf1352rnReg_t eReg, u16 u16RegNum, u8 u8Len, const u16 *pData, unMeterDjsf1352rnPkt_t **pPkt)
{
    bool bRst = false;
    u16  u16Reg;
    u16  u16Crc, u16Len;
    
    
    //0:参数保护
    if(((eFunction != eMeterAcrelFunctionRead) && (eFunction != eMeterAcrelFunctionWrite)) || (eReg >= eMeterDjsf1352rnRegMax) || (u16RegNum == 0) || (pPkt == NULL))
    {
        return(false);
    }
    
    //1:访问保护
    xSemaphoreTake(pDjsf1352rnCache->xSemTxPkt, portMAX_DELAY);
    
    
    //2:填充报文
    u16Reg = (u16)eReg;
    memset(pDjsf1352rnCache->unTxPkt.u8Buf, 0, sizeof(pDjsf1352rnCache->unTxPkt));
    pDjsf1352rnCache->unTxPkt.stPkt.stHead.u8Addr = u8Addr;
    pDjsf1352rnCache->unTxPkt.stPkt.stHead.eFunction = eFunction;
    if(eFunction == eMeterAcrelFunctionRead)
    {
        pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun03.u16RegAddr = htons(u16Reg);
        pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun03.u16RegNum = htons(u16RegNum);
        u16Len = cMeterDjsf1352rnHeadSize + sizeof(u16Reg) + sizeof(u16RegNum);
    }
    else
    {
        pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun10.u16RegAddr = htons(u16Reg);
        pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun10.u16RegNum = htons(u16RegNum);
        pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun10.u8Len = u8Len;
        memcpy(pDjsf1352rnCache->unTxPkt.stPkt.unPayload.stFun10.u16RegValue, pData, u8Len);
        u16Len = cMeterDjsf1352rnHeadSize + sizeof(u16Reg) + sizeof(u16RegNum) + sizeof(u8Len) + u8Len;
    }
    
    //2.1:填充CRC
    u16Crc = sCrc16Modbus(pDjsf1352rnCache->unTxPkt.u8Buf, u16Len);
    pDjsf1352rnCache->unTxPkt.u8Buf[u16Len + 0] = (u16Crc >> 0) & 0xff;            //CRC低字节在前
    pDjsf1352rnCache->unTxPkt.u8Buf[u16Len + 1] = (u16Crc >> 8) & 0xff;
    u16Len = u16Len + sizeof(u16Crc);
    
    
    //3:发送报文
    bRst = sMeterDrvDjsf1352rnSend(u16Len, pDjsf1352rnCache->unTxPkt.u8Buf);
    (*pPkt) = (bRst == false) ? NULL : (&pDjsf1352rnCache->unRxPkt);
    
    
    //4:访问保护解除
    //这里延时解除保护 是为了让外部在同一线程内顺序调用发送函数或多线程同时调用发送函数时
    //有一定的发送间隔，否则电表无法响应命令
    vTaskDelay(20 / portTICK_RATE_MS);
    xSemaphoreGive(pDjsf1352rnCache->xSemTxPkt);
    
    return(bRst);
}















/**********************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 串口数据接收
* Author                                :   Hall
* modified Date                         :   2022-08-10
* notice                                :   
***********************************************************************************************/
void sMeterDrvDjsf1352rnRecv(i32 i32Len, const u8 *pData)
{
    u32 i;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pDjsf1352rnCache->i32RxIndex)
        {
        case 0:                                                                 //地址域 Address
            if((pData[i] > 0) && (pData[i] <= cMeterNum))
            {
                memset(pDjsf1352rnCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPktTemp));
                pDjsf1352rnCache->unRxPktTemp.u8Buf[pDjsf1352rnCache->i32RxIndex] = pData[i];
                pDjsf1352rnCache->i32RxIndex++;
            }
            break;
        case 1:                                                                 //功能域 Function
            if((pData[i] == eMeterAcrelFunctionRead) || (pData[i] == eMeterAcrelFunctionWrite))
            {
                pDjsf1352rnCache->unRxPktTemp.u8Buf[pDjsf1352rnCache->i32RxIndex] = pData[i];
                pDjsf1352rnCache->i32RxIndex++;
            }
            else
            {
                pDjsf1352rnCache->i32RxIndex = 0;
            }
            break;
        default:
            pDjsf1352rnCache->unRxPktTemp.u8Buf[pDjsf1352rnCache->i32RxIndex] = pData[i];
            pDjsf1352rnCache->i32RxIndex++;
            if(pDjsf1352rnCache->unRxPktTemp.u8Buf[1] == eMeterAcrelFunctionRead)
            {
                //                                  报头+校验和长度              + 数据长度                               + len本身长度
                if(pDjsf1352rnCache->i32RxIndex >= (cMeterDjsf1352rnHeadTailSize + pDjsf1352rnCache->unRxPktTemp.u8Buf[2] + 1))
                {
                    memset(pDjsf1352rnCache->unRxPkt.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPkt));
                    memcpy(pDjsf1352rnCache->unRxPkt.u8Buf, pDjsf1352rnCache->unRxPktTemp.u8Buf, pDjsf1352rnCache->i32RxIndex);
                    sMeterDrvDjsf1352rnCheckCrc(pDjsf1352rnCache->i32RxIndex, pDjsf1352rnCache->unRxPkt.u8Buf);
                    
                    memset(pDjsf1352rnCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPktTemp));
                    pDjsf1352rnCache->i32RxIndex = 0;
                }
                else if(pDjsf1352rnCache->i32RxIndex >= sizeof(pDjsf1352rnCache->unRxPktTemp))
                {
                    memset(pDjsf1352rnCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPktTemp));
                    pDjsf1352rnCache->i32RxIndex = 0;
                }
            }
            else if(pDjsf1352rnCache->unRxPktTemp.u8Buf[1] == eMeterAcrelFunctionWrite)
            {
                //                                  报头+校验和长度              + payload长度
                if(pDjsf1352rnCache->i32RxIndex >= (cMeterDjsf1352rnHeadTailSize + sizeof(stMeterDjsf1352rnFun10Ack_t)))
                {
                    memset(pDjsf1352rnCache->unRxPkt.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPkt));
                    memcpy(pDjsf1352rnCache->unRxPkt.u8Buf, pDjsf1352rnCache->unRxPktTemp.u8Buf, pDjsf1352rnCache->i32RxIndex);
                    sMeterDrvDjsf1352rnCheckCrc(pDjsf1352rnCache->i32RxIndex, pDjsf1352rnCache->unRxPkt.u8Buf);
                    
                    memset(pDjsf1352rnCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPktTemp));
                    pDjsf1352rnCache->i32RxIndex = 0;
                }
            }
            else if(pDjsf1352rnCache->i32RxIndex >= sizeof(pDjsf1352rnCache->unRxPktTemp))
            {
                memset(pDjsf1352rnCache->unRxPktTemp.u8Buf, 0, sizeof(pDjsf1352rnCache->unRxPktTemp));
                pDjsf1352rnCache->i32RxIndex = 0;
            }
            break;
        }
    }

}








/**********************************************************************************************
* Description                           :   DJSF1352-RN电表驱动 串口数据包 CRC校验函数
* Author                                :   Hall
* modified Date                         :   2022-08-10
* notice                                :   
***********************************************************************************************/
void sMeterDrvDjsf1352rnCheckCrc(i32 i32Len, const u8 *pData)
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
        if(pDjsf1352rnCache->xSemAck != NULL)
        {
            xSemaphoreGive(pDjsf1352rnCache->xSemAck);
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
* Description                           :   DJSF1352-RN电表驱动 底层发送函数
* Author                                :   Hall
* Creat Date                            :   2022-08-10
* notice                                :   
****************************************************************************************************/
bool sMeterDrvDjsf1352rnSend(u16 u16Len, const u8 *pBuf)
{
    xSemaphoreTake(pDjsf1352rnCache->xSemAck, 0);
    
    sUsartDrvSend(cMeterComUartNum, u16Len, pBuf);
    
    return xSemaphoreTake(pDjsf1352rnCache->xSemAck, 1000 / portTICK_PERIOD_MS);
}



























