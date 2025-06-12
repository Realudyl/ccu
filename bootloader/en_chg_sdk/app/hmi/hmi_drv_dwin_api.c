/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   hmi_drv_dwin_api.c
* Description                           :   迪文屏初驱动
* Version                               :   
* Author                                :   hh
* Creat Date                            :   2024-04-23
* notice                                :   使用DMG80480T070_05WTC迪文屏幕，迪文自定串口协议 分辨率800x480
驱动层api:
    1.一些系统参数(时间，亮度，页面ID等)的get函数
    2.队列数据获取函数，底层驱动将屏幕返回的一些应用层数据按照指定格式入列后需要在api获取，提供给应用层去处理
    3.写数据,做好写数据接口供应用层将一些页面id需要显示的内容发送给屏幕去显示
    4.页面跳转函数--
****************************************************************************************************/
#include "hmi_drv_dwin_api.h"






extern stHmiDwinrnCache_t *pHmiDwinCache;
extern stHmiDwinRcvCache_t stHmiDwinRcvCache;









bool sHmiDwinDrvGetDataIdBtnessRtc(u16 *Id, u8 *Brightness, stTime_t *Rtc);
bool sHmiDwinDrvSetRtc(stTime_t *Rtc);
bool sHmiDwinDrvSetPage(u16 u16page);










/***************************************************************************************************
* Description                           :   迪文串口屏驱动 api接口函数 之 获取屏幕页面ID、亮度、时间
* Author                                :   hh
* Creat Date                            :   2024-03-19
* notice                                :   
****************************************************************************************************/
bool sHmiDwinDrvGetDataIdBtnessRtc(u16 *Id, u8 *Brightness, stTime_t *Rtc)
{
    if(Id != NULL)
    {
        (*Id) = pHmiDwinCache->stData.u16nowpage;
    }
    
    if(Brightness != NULL)
    {
        (*Brightness) = pHmiDwinCache->stData.u8Brightness;
    }
    
    if(Rtc != NULL)
    {
        memcpy(Rtc, &pHmiDwinCache->stData.stTime, sizeof(stTime_t));
    }
    
    return(true);
}






/***************************************************************************************************
* Description                           :   迪文串口屏驱动 api接口函数 之 设置屏幕时间
* Author                                :   hh
* Creat Date                            :   2024-03-19
* notice                                :   
5A A5 0B 82 009C 5AA5 12 06 1B 15 15 15
****************************************************************************************************/
bool sHmiDwinDrvSetRtc(stTime_t *Rtc)
{
    stTime_t *bRtc = NULL;
    u8 u8data[10] = {0};
    u8 u8len;

    bRtc = (stTime_t *)Rtc;
    
    //
    u8data[0] = 0x5A;
    u8data[1] = 0xA5;
    u8data[2] = bRtc->year;
    u8data[3] = bRtc->mon;
    u8data[4] = bRtc->day;
    u8data[5] = bRtc->hour;
    u8data[6] = bRtc->min;
    u8data[7] = bRtc->sec;
    
    u8len = sizeof(u8data);
    
    sHmiDwinWrData(0x009C, (u8 *)u8data, u8len);
    return (true);
}






/***************************************************************************************************
* Description                           :   迪文串口屏驱动 api接口函数 之 设置页面
* Author                                :   hh
* Creat Date                            :   2024-03-19
* notice                                :   
功能码 -0x82,00 84 5A 01 xx xx,xx-图片ID
5A A5 07 82 0084 5A01 0001--显示01序列号图片,高字节在前
数据域0x5A-表示启动一次页面处理， CPU 处理完清零;处理模式,0x01=页面切换
****************************************************************************************************/
bool sHmiDwinDrvSetPage(u16 u16page)
{
    u8 u8len;
    bool bRst;
    u8 u8Data[4];
    
    
    //2:填充报文
    u8Data[0] = 0x5A;
    u8Data[1] = 0x01;
    u8Data[2] = u16page >> 8;
    u8Data[3] = u16page & 0xff;
    
    
    //3:发送报文
    bRst = sHmiDwinWrData(0x0084, u8Data, 4);
    

    return bRst;
    
}










/***************************************************************************************************
* Description                           :   迪文串口屏驱动 api接口函数 之 队列数据获取函数
* Author                                :   hh
* Creat Date                            :   2024-03-19
* notice                                :   串口屏返回的一些用户层地址的数据（按键返回，数据录入等）
解析后组合页面
****************************************************************************************************/
i32 sHmiDwinDrvGetData(i32 i32MaxLen, u8 *pBuf)
{
    i32 i32Len = 0;
    
    if(en_queueSize(&stHmiDwinRcvCache.stEventQueue) > 0)
    {
        xSemaphoreTake(stHmiDwinRcvCache.xSemEventQueue, portMAX_DELAY);
        i32Len = en_queuePop(&stHmiDwinRcvCache.stEventQueue, NULL, pBuf, i32MaxLen);
        xSemaphoreGive(stHmiDwinRcvCache.xSemEventQueue);
    }
    
    
    return i32Len;
}





/**********************************************************************************************
* Description                           :   迪文显示屏 应用处理 屏幕对时
* Author                                :   hh
* modified Date                         :   2022-08-10
* notice                                :   功能码 -0x82,地址:0x009C+5AA5-启动RTC设置+时间
示例:5A A5 0B 82 009C 5AA5 12 06 1B 15 15 15
***********************************************************************************************/
bool sHmiDwinSendTiming(u8 u8cmd, eHmiDwinFunction_t eFct, u16 u16Addr,u8 u8Len, const u8 *pBuf)
{
    bool bRst;
    u8 u8len;
    
    unHmiDwinPkt_t              *pPkt     = NULL;
    stHmiDwinrnHead_t           *pHead    = NULL;
    unHmiDwinPayload_t          *pPayload = NULL;
    
    //1:访问保护---使用 Ack发送 资源
    xSemaphoreTake(pHmiDwinCache->xSemTxPkt, portMAX_DELAY);
    pPkt     = &pHmiDwinCache->unTxPkt;
    pHead    = &pPkt->stPkt.stHead;
    pPayload = &pPkt->stPkt.unPayload;
    
    //2:填充报文
    memset(pHmiDwinCache->unTxPkt.u8Buf, 0, sizeof(pHmiDwinCache->unTxPkt));
    
    //2.1:head
    pHead->u16Head   = htons(eHmiDwinHead);
    pHead->u8len     = u8Len;
    pHead->eFunction = eFct;
    
    //2.2:payload
    pPayload->stFun82.u16RamAddr   = htons(u16Addr);
    //读取4个地址位数据
    memcpy(pPayload->stFun82.u8WrData, pBuf, u8Len-1);
    
    //2.3:crc,暂时无crc
    
    //3:发送报文,发送字节总长需要+2（报文头）+1（len本身长度）
    bRst = sHmiDwinSendCmd(u8Len+3, pHmiDwinCache->unTxPkt.u8Buf);
    
    //4:访问保护解除
    xSemaphoreGive(pHmiDwinCache->xSemTxPkt);
    
    return(bRst);
}





