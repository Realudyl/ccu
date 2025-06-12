/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_com.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-10-12
* notice                                :     串口硬件驱动
****************************************************************************************************/
#include "private_drv_opt.h"
#include "private_drv_com.h"
#include "private_drv_basic.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_com";







extern stPrivDrvCache_t *pPrivDrvCache;
extern stPrivDrvCmdMap_t stPrivDrvCmdMap[];






//初始化
bool sPrivComInit(void);
bool sPrivComSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag);
i32  sPrivComRecv(u32 u32Port, u32 u32Len, u8 *u8RxBuf);

void sPrivComRecvTask(void *pParam);

#if (cSdkPrivDevType == cSdkPrivDevTypeM)
i32  sPrivComRecvEsp32(u32 u32Port, u32 u32Len, u8 *u8RxBuf);
i32  sPrivComRead(uart_port_t port, u32 u32Len, u32 u32MaxLen, u8 *u8RxBuf);
void sPrivComRecvErrCheck(uart_event_type_t eType);
#endif

















/**********************************************************************************************
* Description       :     串口资源初始化
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
bool sPrivComInit(void)
{
    bool bRst;
    char u8TaskName[32];
    
    snprintf(u8TaskName, sizeof(u8TaskName), "Uart%dRecvTask", cPrivDrvComUartNum);
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    uart_config_t stConfig =
    {
        .baud_rate = cPrivDrvComBaudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    uart_set_pin(cPrivDrvComUartNum, cPrivDrvComTxPin, cPrivDrvComRxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_param_config(cPrivDrvComUartNum, &stConfig);
    esp_err_t err = uart_driver_install(cPrivDrvComUartNum, cPrivDrvComRxBuffSize, cPrivDrvComTxBuffSize, 20, &pPrivDrvCache->hUartQueue, 0);
    bRst = (err == ESP_OK) ? true : false;
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cPrivDrvComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    bRst = sUsartDrvOpen(cPrivDrvComUartNum, &stUsartCfg_t, cPrivDrvComRxBuffSize, cPrivDrvComTxBuffSize);
#endif
    
    if(pdPASS != xTaskCreate(sPrivComRecvTask, u8TaskName, cPrivComRecvTaskStackSize, NULL, cPrivComRecvTaskPrio, NULL))
    {
        bRst = false;
        EN_SLOGE(TAG, "EN+ 内网协议:串口初始化失败, 任务创建出错!!!");
    }
    
    return(bRst);
}








/**********************************************************************************************
* Description       :     串口 发送函数
* Author            :     Hall
* modified Date     :     2023-10-19
* notice            :     bRptFlag:用于表示本次发送是否属于 rpt发送
*
*                         负责：
*                         1：等待桩端的应答报文，并根据应答结果直接对调用者返回发送结果
***********************************************************************************************/
bool sPrivComSend(ePrivDrvCmd_t eCmd, const u8 *pData, u16 u16Len, bool bRptFlag)
{
    i32  i;
    i32  i32SendLen;
    bool bRst;
    
    //1:先清除残留的信号量
    i = sPrivDrvCmdSearch(eCmd);
    if(i < 0)
    {
        EN_SLOGE(TAG, "查找0x%2X出错,请检查是否注册!!!", eCmd);
        return(false);
    }
    if(stPrivDrvCmdMap[i].u32WaitTime != 0)
    {
        //等待时间不为0的信号量才需要清除
        xSemaphoreTake(stPrivDrvCmdMap[i].hAckMutex, 0);
    }
    
    
    //2:打印并发送数据
    sPrivDrvDataPrint(true, pData, u16Len);
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    i32SendLen = uart_write_bytes(cPrivDrvComUartNum, pData, u16Len);
    bRst = (i32SendLen == u16Len) ? true : false;
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    bRst = sUsartDrvSend(cPrivDrvComUartNum, u16Len, pData);
    i32SendLen = (bRst == true) ? u16Len : 0;
#endif
    
#if(cPrivDrvComPrintfEn == 1)
    EN_SLOGI(TAG, "发送方  :%d", bRptFlag);
    EN_SLOGI(TAG, "发送大小:%d,实际发送:%d", u16Len, i32SendLen);
    EN_SLOGI(TAG, "地址    :%d,等待时间:%u", i, stPrivDrvCmdMap[i].u32WaitTime);
 #endif
    
    //3:检查返回信号量
    if(bRst == true)
    {
        if((stPrivDrvCmdMap[i].u32WaitTime != 0) && (bRptFlag == true))
        {
            //等待时间不为0的信号量才需要检查
            bRst = xSemaphoreTake(stPrivDrvCmdMap[i].hAckMutex, stPrivDrvCmdMap[i].u32WaitTime / portTICK_PERIOD_MS);
            if(bRst == true)
            {
                bRst = stPrivDrvCmdMap[i].bProcRst;
            }
        }
        else
        {
            //无返回信号量可检查的报文 就返回发送结果
            bRst = (i32SendLen == u16Len) ? true : false;
        }
    }
    
#if(cPrivDrvComPrintfEn == 1)
    EN_SLOGI(TAG, "发送结果:%d,%s", bRst, (bRst)?"成功":"失败");
 #endif
    
    //4:附加处理
    if((bRst == true) && (bRptFlag == true))
    {
        //rpt发送 成功时 对应序列号应该自增
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
        pPrivDrvCache->unData.stMaster.stChg.u8RptSeqno++;
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
        pPrivDrvCache->unData.stSlave.stChg.u8RptSeqno++;
#endif
    }
    
    
    return(bRst);
}







/**********************************************************************************************
* Description       :     串口 接收函数
* Author            :     XRG
* modified Date     :     2024-04-24
* notice            :     负责从串口n中断缓存数据到u8RxBuf,调用者根据返回数据大小进行断帧
***********************************************************************************************/
i32 sPrivComRecv(u32 u32Port, u32 u32Len, u8 *u8RxBuf)
{
    i32 i32ReadedLen = -1;
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    i32ReadedLen = sPrivComRecvEsp32(u32Port, u32Len, u8RxBuf);
#elif (cSdkPrivDevType == cSdkPrivDevTypeS)
    i32ReadedLen = sUsartDrvRecv(u32Port, u32Len, u8RxBuf);
#endif
    
    return(i32ReadedLen);
}






/**********************************************************************************************
* Description       :     串口数据接收 任务
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
void sPrivComRecvTask(void *pParam)
{
    i32 i32ReadedLen = -1;
    EN_SLOGI(TAG, "EN+ 内网协议 , 串口数据接收任务开始运行!!!");
    
    while(1)
    {
        memset(pPrivDrvCache->u8RxBuf, 0, sizeof(pPrivDrvCache->u8RxBuf));
        i32ReadedLen = sPrivComRecv(cPrivDrvComUartNum, sizeof(pPrivDrvCache->u8RxBuf), pPrivDrvCache->u8RxBuf);
        if(i32ReadedLen > 0)
        {
#if(cPrivDrvPktPrintfEn == 1)
            EN_SLOGI(TAG, "读大小:%d", i32ReadedLen);
            EN_HLOGW(TAG, "接收开始:", pPrivDrvCache->u8RxBuf, i32ReadedLen);
#endif
            sPrivDrvPktCheck(pPrivDrvCache->u8RxBuf, i32ReadedLen);
#if(cPrivDrvPktPrintfEn == 1)
            EN_SLOGI(TAG, "接收结束");
#endif
        }
        else
        {
            //没收到数据时 就要执行超时检查
            sPrivDrvPktCheckTimeout();
        }
#if(cPrivDrvRecvDelay > 0)
        vTaskDelay(cPrivDrvRecvDelay / portTICK_RATE_MS);
#endif
    }
    
    vTaskDelete(NULL);
}





#if (cSdkPrivDevType == cSdkPrivDevTypeM)


/**********************************************************************************************
* Description       :     串口数据接收
* Author            :     XRG
* modified Date     :     2024-04-24
* notice            :     ESP32串口接收
***********************************************************************************************/
i32 sPrivComRecvEsp32(u32 u32Port, u32 u32Len, u8 *u8RxBuf)
{
    i32 i32ReadedLen = -1;
    uart_event_t event;
    
    if(xQueueReceive(pPrivDrvCache->hUartQueue, (void * )&event, (pdMS_TO_TICKS(100))))
    {
        switch (event.type) 
        {
        case UART_DATA:
            i32ReadedLen = sPrivComRead(u32Port, event.size, u32Len, u8RxBuf);
            break;
        case UART_FIFO_OVF :
            EN_SLOGI(TAG, "hw fifo overflow");
            uart_flush_input(u32Port);
            xQueueReset(pPrivDrvCache->hUartQueue);
            break;
        //Event of UART ring buffer full
        case UART_BUFFER_FULL :
            EN_SLOGI(TAG, "ring buffer full");
            uart_flush_input(u32Port);
            xQueueReset(pPrivDrvCache->hUartQueue);
            break;
        //Event of UART RX break detected
        case UART_BREAK :
            EN_SLOGI(TAG, "uart rx break");
            uart_flush_input(u32Port);
            xQueueReset(pPrivDrvCache->hUartQueue);
            break;
        //Event of UART parity check error
        case UART_PARITY_ERR :
            EN_SLOGI(TAG, "uart parity error");
            uart_flush_input(u32Port);
            xQueueReset(pPrivDrvCache->hUartQueue);
            break;
        //Event of UART frame error
        case UART_FRAME_ERR :
            EN_SLOGI(TAG, "uart frame error2");
            uart_flush_input(u32Port);
            xQueueReset(pPrivDrvCache->hUartQueue);
            break;
        //UART_PATTERN_DET
        case UART_PATTERN_DET :
             EN_SLOGI(TAG, " UART_PATTERN_DET");
            break;
        //Others
        default :
            break;
        }
        
        //检查是否异常,如异常则统计异常次数,正常则清零
        sPrivComRecvErrCheck(event.type);
    }
    
    return(i32ReadedLen);
}





/**********************************************************************************************
* Description       :     串口数据 接收函数
* Author            :     Hall
* modified Date     :     2023-10-18
* notice            :     
***********************************************************************************************/
i32 sPrivComRead(uart_port_t port, u32 u32Len, u32 u32MaxLen, u8 *u8RxBuf)
{
    u32     i32ReadLen;
    i32     i32ReadedLen;
    
    
    //检查FIFO缓冲区数据是否大于本次接收,如果大于则直接读完FIFO
    size_t    u32BufLen = 0;
    esp_err_t err       = uart_get_buffered_data_len(port, &u32BufLen);
    if((err == ESP_OK) && (u32BufLen > u32Len))
    {
        EN_SLOGW(TAG, "FIFO:%d > Len:%d", u32BufLen, u32Len);
        //u32Len = u32BufLen;
    }
    
    //读取串口数据
    i32ReadLen = (u32Len < u32MaxLen) ? u32Len : u32MaxLen;
    i32ReadedLen = uart_read_bytes(port, u8RxBuf, i32ReadLen, portMAX_DELAY);
    
    return(i32ReadedLen);
}







/**********************************************************************************************
* Description       :     串口数据接收 失败检查
* Author            :     XRG
* modified Date     :     2024-02-24
* notice            :     检查是否异常,如异常则统计异常次数,正常则清零
***********************************************************************************************/
void sPrivComRecvErrCheck(uart_event_type_t eType)
{
#if(cPrivDrvRecvErrCheckEn == 1)
    static u8 u8RxErrCnt = 0;
    
    switch(eType)
    {
    case UART_DATA:
         u8RxErrCnt = 0;
         break;
    case UART_FIFO_OVF:
    case UART_BUFFER_FULL:
    case UART_BREAK:
    case UART_PARITY_ERR:
    case UART_FRAME_ERR:
        sPrivDrvPktRecvReset();
#if(cPrivDrvRecvErrRebootCnt > 1)
        u8RxErrCnt++;
        if(u8RxErrCnt > cPrivDrvRecvErrRebootCnt)
        {
            u8RxErrCnt = 0;
            EN_SLOGI(TAG, "PrivComRxErr over reboot");
            vTaskDelay(1000 / portTICK_RATE_MS);
            esp_restart();
        }
#endif
        break;
    default:
        break;
    }
#endif
}


#endif



