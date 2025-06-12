/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   hmi_drv_dwin.c
* Description                           :   迪文屏初驱动
* Version                               :   
* Author                                :   hh
* Creat Date                            :   2024-04-23
* notice                                :   使用DMG80480T070_05WTC迪文屏幕，迪文自定串口协议 分辨率800x480
屏幕操作按地址，一个地址长度有2个字节

串口指令：
    写寄存器数据指令0x80    下发：寄存器页面 ID（0x00-0x08） +寄存器地址(0x00-0xFF)+写入的数据
    读寄存器数据指令0x81    下发：寄存器页面 ID（0x00-0x08） +寄存器地址(0x00-0xFF)+读取数据字节长度(0x01-0xFB)
    写变量存储器指令0x82    下发：变量空间首地址(0x0000-0xFFFF)+写入的数据，系统保留的空间不要写。
    读变量存储器指令0x83    下发：变量空间首地址(0x0000-0xFFFF)+读取数据字长度(0x01-0x7D)
    
    80 81 指令一般用户不需要使用，通常用于 OS 功能的调试访问
    
指令示例
    0x81    读取 00 寄存器页面 os 寄存器 R10~R13 的 04 个字节长度的数据。
    5A A5 04 81 00 0A 04 
    0x82    向 1000 变量地址里写数值 2 为例：
    5A A5 05 82 10 00 00 02
    0x83    读 1000 变量地址里的数值（假设当前数值为 2）为例
    下发 5A A5 04 83 10 00 01
    应答 5A A5 06 83 10 00 01 00 02
    
*触摸按键返回到串口数据（0x83）*
    按返回变量地址 0x1001，键值 0x0002 为例：
    5A A5 06 83 1001 01 0002

    比较复杂的ASCII录入，软件解析读取地址0x51BD,工程勾选“输入状态返回”后变量地址0x51BE,SN长度16 SN20240514555666
    5A A5 18 83 51 BD 0A 5A 10 53 4E 32 30 32 34 30 35 31 34 35 35 35 36 36 36 FF FF
****************************************************************************************************/
#include "hmi_drv_dwin.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "hmi_drv_dwin";


//---------------------------------------------------------------------------------------------------------

//仅限在本文件内部使用 禁止其他文件直接访问本结构体
stHmiDwinrnCache_t *pHmiDwinCache    = NULL;
stHmiDwinRcvCache_t stHmiDwinRcvCache;






bool sHmiDrvDwinInit(void);
bool sHmDwinQueneInit(void);
void sHmiDwinSendTask(void *pParam);
void sHmiDwinrnRecvTask(void *pParam);
void sHmiDrvDwinRecv(i32 i32Len, const u8 *pData);
i32 sPrivDwinAddrSearch(eHmiDwinAddr_t eAddr);
void sHmiDwinRecProc(i32 i32Len, const u8 *pData);
bool sHmiDwinTime(const u8 *pBuf, i32 i32Len);
void sHmiDrvSwitchPageEvent(u16 u16Addr);
bool sHmiDwinNowPage(const u8 *pBuf, i32 i32Len);
bool sHmiDwinBright(const u8 *pBuf, i32 i32Len);
bool sHmiDwinRedSysInfo(u8 u8Len, eHmiDwinFunction_t eFct, u16 u16Addr, u8 u8RedLen);
bool sHmiDwinWrData(u16 u16addr, const u8 *pBuf, u8 u8len);
bool sHmiDwinSendCmd(u16 u16Len, const u8 *pBuf);




//发送函数不需要用map，接收函数需要，驱动层只需处理系统变量地址范围:0x0000-0x0FFF
stHmiDwinCmdMap_t stHmiDwinCmdMap[] = 
{
    {eDwinTime,         sHmiDwinTime,           "解析接收屏幕时间"         },
    {eDwinPage,         sHmiDwinNowPage,        "解析屏幕当前页面"         },
    {eDwinBrightness,   sHmiDwinBright,         "解析屏幕当前亮度"         },
};


/***************************************************************************************************
* Description                           :   迪文串口屏驱动 通讯资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-08-09
* notice                                :   
****************************************************************************************************/
bool sHmiDrvDwinInit(void)
{
    bool bRst = false;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cHmiComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
    bRst = sUsartDrvOpen(cHmiComUartNum, &stUsartCfg_t, cHmiDwinBufSize, cHmiDwinBufSize);
    bRst &= sHmDwinQueneInit();
    
    pHmiDwinCache = (stHmiDwinrnCache_t *)MALLOC(sizeof(stHmiDwinrnCache_t));
    memset(pHmiDwinCache, 0, sizeof(stHmiDwinrnCache_t));
    pHmiDwinCache->xSemAck = xSemaphoreCreateBinary();
    pHmiDwinCache->xSemTxPkt = xSemaphoreCreateBinary();
    pHmiDwinCache->xSemDrvTxPkt = xSemaphoreCreateBinary();
    xSemaphoreGive(pHmiDwinCache->xSemTxPkt);
    xSemaphoreGive(pHmiDwinCache->xSemAck);
    xSemaphoreGive(pHmiDwinCache->xSemDrvTxPkt);
    
    //先建立接收线程 再建立轮询线程
    xTaskCreate(sHmiDwinrnRecvTask, "sHmiDwinrnRecvTask", (512), NULL, 24, NULL);
    xTaskCreate(sHmiDwinSendTask, "sHmiDwinSendTask", (512), NULL, 27, NULL);
    
    return(bRst);
}





/**********************************************************************************************
* Description                           :   迪文屏幕接受数据缓存队列初始化
* Author                                :   hh
* modified Date                         :   2023-12-18
* notice                                :   
***********************************************************************************************/

bool sHmDwinQueneInit(void)
{
    bool bRst;
    
    memset(&stHmiDwinRcvCache, 0, sizeof(stHmiDwinRcvCache));
    stHmiDwinRcvCache.xSemEventQueue = xSemaphoreCreateBinary();
    xSemaphoreGive(stHmiDwinRcvCache.xSemEventQueue);
    bRst = en_queueInit(&stHmiDwinRcvCache.stEventQueue, 1024 , 100, 0);
    if(bRst == false)
    {
        return(false);
    }
    
    return(bRst);
}



/***************************************************************************************************
* Description                           :   迪文串口屏驱动 轮询任务
* Author                                :   hh
* Creat Date                            :   2022-08-09
* notice                                :   根据有应用需求，定时向屏幕发送一些数据如查询页面
查询屏幕时间、亮度等
****************************************************************************************************/
void sHmiDwinSendTask(void *pParam)
{
    static u8 u8cmdstep = 0;
    
    EN_SLOGD(TAG, "任务建立:迪文串口屏轮询任务");
    
    while(1)
    {
        u8cmdstep++;
        switch(u8cmdstep)
        {
            default:
                u8cmdstep = 0;
            case 0:
                sHmiDwinRedSysInfo(4, eHmiDwinFunRdRam, 0x0014, 1);
                break;
            case 1:
                sHmiDwinRedSysInfo(4, eHmiDwinFunRdRam, 0x0010, 4);
                break;
            case 2:
                sHmiDwinRedSysInfo(4, eHmiDwinFunRdRam, 0x0031, 1);
                break;
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
void sHmiDwinrnRecvTask(void *pParam)
{
    i32 i32ReadLen;
    
    
    EN_SLOGD(TAG, "任务建立:迪文串口屏接收任务");
    while(1 > 0)
    {
        memset(pHmiDwinCache->u8Buf, 0, sizeof(pHmiDwinCache->u8Buf));
        i32ReadLen = sUsartDrvRecv(cHmiComUartNum, sizeof(pHmiDwinCache->u8Buf), pHmiDwinCache->u8Buf);
        if(i32ReadLen > 0)
        {
            sHmiDrvDwinRecv(i32ReadLen, pHmiDwinCache->u8Buf);
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}




/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 串口数据接收
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   
***********************************************************************************************/
void sHmiDrvDwinRecv(i32 i32Len, const u8 *pData)
{
    u32 i;
    
    for(i = 0; i < i32Len; i++)
    {
        switch(pHmiDwinCache->i32RxIndex)
        {
        case 0:                                                                 //包头5A A5---5A
            if(pData[i] == (eHmiDwinHead >> 8))
            {
                memset(pHmiDwinCache->unRxPktTemp.u8Buf, 0, sizeof(pHmiDwinCache->unRxPktTemp));
                pHmiDwinCache->unRxPktTemp.u8Buf[pHmiDwinCache->i32RxIndex] = pData[i];
                pHmiDwinCache->i32RxIndex++;
            }
            break;
        case 1:                                                                 //包头5A A5---A5
            if(pData[i] == (eHmiDwinHead & 0xff))
            {
                pHmiDwinCache->unRxPktTemp.u8Buf[pHmiDwinCache->i32RxIndex] = pData[i];
                pHmiDwinCache->i32RxIndex++;
            }
            else
            {
                pHmiDwinCache->i32RxIndex = 0;
            }
            break;
        default:
            pHmiDwinCache->unRxPktTemp.u8Buf[pHmiDwinCache->i32RxIndex] = pData[i];
            pHmiDwinCache->i32RxIndex++;
            if(pHmiDwinCache->i32RxIndex < sizeof(pHmiDwinCache->unRxPktTemp))
            {
                //                                 头                      +数据长度          +len本身长度
                if(pHmiDwinCache->i32RxIndex >= 2 + pHmiDwinCache->unRxPktTemp.u8Buf[2] + 1)
                {
                    memset(pHmiDwinCache->unRxPkt.u8Buf, 0, sizeof(pHmiDwinCache->unRxPkt));
                    memcpy(pHmiDwinCache->unRxPkt.u8Buf, pHmiDwinCache->unRxPktTemp.u8Buf, pHmiDwinCache->i32RxIndex);
                    if(pHmiDwinCache->unRxPkt.u8Buf[3] == eHmiDwinFunRdRam)
                    {
                        sHmiDwinRecProc(pHmiDwinCache->i32RxIndex, pHmiDwinCache->unRxPkt.u8Buf);
                    }
                    
                    memset(pHmiDwinCache->unRxPktTemp.u8Buf, 0, sizeof(pHmiDwinCache->unRxPktTemp));
                    pHmiDwinCache->i32RxIndex = 0;
                }
                else if(pHmiDwinCache->i32RxIndex >= sizeof(pHmiDwinCache->unRxPktTemp))
                {
                    memset(pHmiDwinCache->unRxPktTemp.u8Buf, 0, sizeof(pHmiDwinCache->unRxPktTemp));
                    pHmiDwinCache->i32RxIndex = 0;
                }
            }
            else
            {
                memset(pHmiDwinCache->unRxPktTemp.u8Buf, 0, sizeof(pHmiDwinCache->unRxPktTemp));
                pHmiDwinCache->i32RxIndex = 0;
            }
        }
    }
}





/**********************************************************************************************
* Description       :     报文类型 搜索
* Author            :     HH
* modified Date     :     2023-10-19
* notice            :     此函数用于通过指定的 eAddr 去匹配到对应的序号 以便引用 stHmiDwinCmdMap[] 成员
***********************************************************************************************/
i32 sPrivDwinAddrSearch(eHmiDwinAddr_t eAddr)
{
    i32 i;
    
    for(i = 0; i < sArraySize(stHmiDwinCmdMap); i++)
    {
        if(eAddr == stHmiDwinCmdMap[i].eAddr)
        {
            if(stHmiDwinCmdMap[i].pRecv != NULL)
            {
                return(i);
            }
        }
    }
    
    return(-1);
}


/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 串口数据解析
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   
***********************************************************************************************/
void sHmiDwinRecProc(i32 i32Len, const u8 *pData)
{
    i32               i;
    i32               i32PushLen;
    u16               u16Addr;
    
    stHmiDwinPkt_t      *pPck    = NULL;
    stHmiDwinEvent_t    *pStr     = NULL;
    stHmiDwinFun83Ack_t *pPayload = NULL;
    
    pPck     = (stHmiDwinPkt_t *)pData;
    pPayload = &pPck->unPayload.stFun83Ack;
    u16Addr  = htons(pPayload->u16Addr);
    pStr     = &stHmiDwinRcvCache.stEvent;
    memset(pStr, 0, sizeof(stHmiDwinEvent_t));
    
    i = sPrivDwinAddrSearch((eHmiDwinAddr_t)u16Addr);
    if(i >= 0)
    {
        stHmiDwinCmdMap[i].pRecv(pData, i32Len);
    }
    else
    {
        //非系统地址返回的数据组合后入列--当面页面id+地址+数据长度+数据
        pStr->u16addr = u16Addr;
        //数据域-功能码1-地址2-读取数据字节长度1
        pStr->u8len   = pPck->stHead.u8len-4;
        //数据域拷贝不需要将地址拷贝进去
        memcpy(pStr->u8Buf,pPayload->u8RamData,pStr->u8len);
        pStr->u16page = pHmiDwinCache->stData.u16nowpage;
    
        //入列的数据长度=数据长 + 页面ID + 地址 + 数据长度本身
        i32PushLen  = pStr->u8len + 2 + 2 + 1;
        //入列
        xSemaphoreTake(stHmiDwinRcvCache.xSemEventQueue, 1000 / portTICK_PERIOD_MS);
        en_queuePush(&stHmiDwinRcvCache.stEventQueue, 0, (const u8 *)pStr, i32PushLen);
        xSemaphoreGive(stHmiDwinRcvCache.xSemEventQueue);
        
    }
}







/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 串口数据解析
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   解析屏幕当前时间
读  :5A A5 04 83 00 10 04
应答:5A A5 0C 83 0010 04 13 0A 01 00 0B 0C 0D 00
***********************************************************************************************/
bool sHmiDwinTime(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unHmiDwinPkt_t      *pPkt  = NULL;
    stHmiDwinrnHead_t   *pHead = NULL;
    stHmiDwinRtc_t      *pRtc  = NULL;
    stHmiDwinrnData_t   *pDate = NULL;
    
    
    pPkt    = (unHmiDwinPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pRtc    = &pPkt->stPkt.unPayload.stHmiDwinRtc;
    pDate   = &pHmiDwinCache->stData;
    
    //数据校验
    
    //数据缓存
    pDate->stTime.year = pRtc->u8D7;
    pDate->stTime.mon  = pRtc->u8D6;
    pDate->stTime.day  = pRtc->u8D5;
    pDate->stTime.hour = pRtc->u8D3;
    pDate->stTime.min  = pRtc->u8D2;
    pDate->stTime.sec  = pRtc->u8D1;
    
    //处理结果填充及信号量释放
    bRst              = true;
    
    return bRst;
}




/**********************************************************************************************
* Description                           :   页面切换产生一个入列事件
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   页面更新产生一个入列事件，将数据写默认
***********************************************************************************************/
void sHmiDrvSwitchPageEvent(u16 u16Addr)
{
    i32  i;
    i32  i32PushLen;
    
    stHmiDwinEvent_t    *pStr     = NULL;
    
    pStr     = &stHmiDwinRcvCache.stEvent;
    memset(pStr, 0, sizeof(stHmiDwinEvent_t));
    
    //非系统地址返回的数据组合后入列--当面页面id+地址+键值（包括数据长度）
    pStr->u16addr = 0xFF;
    //数据域-功能码1-地址2
    pStr->u8len   = 0;
    //数据域拷贝不需要将地址拷贝进去
    pStr->u16page = u16Addr;

    //入列的数据长度=数据长 + 页面ID + 地址 + 数据长度本身
    i32PushLen  = pStr->u8len + 2 + 2 + 1;
    //入列
    xSemaphoreTake(stHmiDwinRcvCache.xSemEventQueue, 1000 / portTICK_PERIOD_MS);
    en_queuePush(&stHmiDwinRcvCache.stEventQueue, 0, (const u8 *)pStr, i32PushLen);
    xSemaphoreGive(stHmiDwinRcvCache.xSemEventQueue);
        
}






/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 串口数据解析
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   解析屏幕当前页面
读  :5A A5 04 83 0014 01
返回:5A A5 06 83 0014 01 0007(0007 为 07 号页面)
***********************************************************************************************/
bool sHmiDwinNowPage(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    u16 u16RcvPage;
    unHmiDwinPkt_t      *pPkt  = NULL;
    stHmiDwinrnHead_t   *pHead = NULL;
    stHmiDwinRedPage_t  *pPage  = NULL;
    stHmiDwinrnData_t   *pDate = NULL;
    
    
    pPkt    = (unHmiDwinPkt_t *)pBuf;
    pHead   = &pPkt->stPkt.stHead;
    pPage   = &pPkt->stPkt.unPayload.stHmiDwinRedPage;
    pDate   = &pHmiDwinCache->stData;
    
    u16RcvPage = htons(pPage->u16page);
    
    //数据校验
    
    //页面更新产生一个事件入列
    if(pDate->u16nowpage != u16RcvPage)
    {
        sHmiDrvSwitchPageEvent(u16RcvPage);
    }
    //数据缓存
    pDate->u16nowpage = u16RcvPage;
    
    //处理结果填充及信号量释放
    bRst              = true;
    
    return bRst;
}






/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 串口数据解析
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   解析屏幕当前亮屏的亮度
***********************************************************************************************/
bool sHmiDwinBright(const u8 *pBuf, i32 i32Len)
{
    bool bRst;
    
    unHmiDwinPkt_t       *pPkt        = NULL;
    stHmiDwinrnHead_t    *pHead       = NULL;
    stHmiDwinBrigtness_t *pBrightness = NULL;
    stHmiDwinrnData_t    *pDate       = NULL;
    
    
    pPkt        = (unHmiDwinPkt_t *)pBuf;
    pHead       = &pPkt->stPkt.stHead;
    pBrightness = &pPkt->stPkt.unPayload.stHmiDwinBrigtness;
    pDate       = &pHmiDwinCache->stData;
    
    //数据校验
    
    //数据缓存
    pDate->u8Brightness = pBrightness->u8D0;
    
    //处理结果填充及信号量释放
    bRst              = true;
    
    return bRst;
}





/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 当前显示页面查询
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x83,00 14 01
5A A5 04 83 0014 01--显示01序列号图片,高字节在前
***********************************************************************************************/






/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 读取屏幕时间
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x83,地址0x0010 读取长度4（一个长度对应2字节）
5A A5 04 83 0010 04--读取四个地址数据
应答：5A A5 0C 83 0010 04 13 0A 01 00 0B 0C 0D 00
***********************************************************************************************/




/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 读取屏幕亮度值
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x83 地址 0x0031 读取长度1
示例:5A A5 04 83 0031 01
***********************************************************************************************/






/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 读取对应系统变量
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x83 
读取屏幕亮度值地址 0x0031 读取长度1; 读取屏幕时间,地址 0x0010 读取长度4; 读取页面id：地址0x0014 长度01
示例:5A A5 04 83 0031 01;  5A A5 04 83 0010 04;  5A A5 04 83 0014 01
***********************************************************************************************/
bool sHmiDwinRedSysInfo(u8 u8Len, eHmiDwinFunction_t eFct, u16 u16Addr, u8 u8RedLen)
{
    bool bRst;
    
    unHmiDwinPkt_t              *pPkt     = NULL;
    stHmiDwinrnHead_t           *pHead    = NULL;
    unHmiDwinPayload_t          *pPayload = NULL;
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pHmiDwinCache->xSemDrvTxPkt, portMAX_DELAY);
    pPkt     = &pHmiDwinCache->unTxPkt;
    pHead    = &pPkt->stPkt.stHead;
    pPayload = &pPkt->stPkt.unPayload;
    
    //2:填充报文
    memset(pPkt, 0, sizeof(unHmiDwinPkt_t));
    
    //2.1:head
    pHead->u16Head   = htons(eHmiDwinHead);
    pHead->u8len     = u8Len;
    pHead->eFunction = eFct;
    
    //2.2:payload
    pPayload->stFun83.u16RamAddr   = htons(u16Addr);
    //读取1个地址位数据
    pPayload->stFun83.u8RdLen      = u8RedLen;
    
    //2.3:crc,暂时无crc
    
    //3:发送报文,发送字节总长需要+2（报文头）+1（len本身长度）
    bRst = sHmiDwinSendCmd(u8Len+3, (u8 *)pPkt);
    
    //4:访问保护解除
    xSemaphoreGive(pHmiDwinCache->xSemDrvTxPkt);
    
    return bRst;
}




/**********************************************************************************************
* Description                           :   迪文显示屏串口驱动 写数据
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x82 地址  写入的数据
示例:5A A5 len 82 0031 data
***********************************************************************************************/
bool sHmiDwinWrData(u16 u16addr, const u8 *pBuf, u8 u8len)
{
    bool bRst;
    
    unHmiDwinPkt_t              *pPkt     = NULL;
    stHmiDwinrnHead_t           *pHead    = NULL;
    unHmiDwinPayload_t          *pPayload = NULL;
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pHmiDwinCache->xSemTxPkt, portMAX_DELAY);
    pPkt     = &pHmiDwinCache->unTxPkt;
    pHead    = &pPkt->stPkt.stHead;
    pPayload = &pPkt->stPkt.unPayload;
    
    //2:填充报文
    memset(pPkt, 0, sizeof(unHmiDwinPkt_t));
    
    //2.1:head
    pHead->u16Head   = htons(eHmiDwinHead);
    //数据长度+地址+功能码
    pHead->u8len     = u8len + 2 + 1;
    pHead->eFunction = eHmiDwinFunWrRam;
    
    //2.2:payload
    pPayload->stFun82.u16RamAddr   = htons(u16addr);
    //拷贝发送数据
    memcpy(pPayload->stFun82.u8WrData, pBuf, u8len);
    
    //2.3:crc,暂时无crc
    
    //3:发送报文,发送字节总长需要+2（报文头）+1（len本身长度）
    bRst = sHmiDwinSendCmd(pHead->u8len+3, (u8 *)pPkt->u8Buf);
    
    //4:访问保护解除
    xSemaphoreGive(pHmiDwinCache->xSemTxPkt);
    
    return(bRst);
}









/***************************************************************************************************
* Description                           :   串口显示屏驱动 底层发送函数
* Author                                :   Hall
* Creat Date                            :   2022-08-10
* notice                                :   
****************************************************************************************************/
bool sHmiDwinSendCmd(u16 u16Len, const u8 *pBuf)
{
    bool bRst;
    
    bRst = sUsartDrvSend(cHmiComUartNum, u16Len, pBuf);
    
    return bRst;
}


