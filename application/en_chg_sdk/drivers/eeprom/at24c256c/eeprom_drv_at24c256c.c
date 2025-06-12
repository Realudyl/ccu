/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     eeprom_drv_at24c256c.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-2-23
 * @Attention             :     
 * @Brief                 :     eeprom驱动实现
 * 
 * @History:
 * 
 * 1.@Date: 2024-2-23
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "eeprom_drv_at24c256c.h"

#include "en_log.h"











//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "eeprom_drv_at24c256c";




extern stEepromDrvDevice_t stEepromDrvDev;





















bool sEepromOpen(void);
bool sEepromErase(void);
bool sEepromRecv(u16 u16WordAddr, u32 u32Len, u16 *pBuf);
bool sEepromSend(u16 u16WordAddr, u32 u32Len, const u16 * data);












/*******************************************************************************
 * @FunctionName   :      sEepromOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年2月27日  13:45:08
 * @Description    :      eeprom驱动实现 之 初始化外设
 * @Input          :      eEepromNum        EEPROM设备号
 * @Return         :      
*******************************************************************************/
bool sEepromOpen(void)
{
    stEepromDrvDev.xSemEeprom = xSemaphoreCreateBinary();
    xSemaphoreGive(stEepromDrvDev.xSemEeprom);
    
    return sI2cDrvOpen(stEepromDrvDev.eI2cNum);
}








/***************************************************************************************************
* Description                           :   EEPROM驱动 写操作函数
* Author                                :   Hall
* Creat Date                            :   2024-05-08
* notice                                :   将整个eeprom芯片内容全部擦写为初始值:0xff
****************************************************************************************************/
bool sEepromErase(void)
{
    i32  i;
    
    bool bRst;
    i32  i32Len;
    u32  u32WordAddr;
    
    u16   u16Buf[32];
    
    
    bRst = true;
    i32Len = stEepromDrvDev.u8DevPageSize;
    memset(u16Buf, 0xff, sizeof(u16Buf));                                       //memset按字节去初始化
    for(i = 0; i < stEepromDrvDev.u16DevPageNum; i++)
    {
        u32WordAddr = i32Len * i;
        bRst = bRst & sEepromSend(u32WordAddr, i32Len / sizeof(u16), u16Buf);
        vTaskDelay(cEepromDrvAt24c256cTwr / portTICK_RATE_MS);
    }
    
    return(bRst);
}













/***************************************************************************************************
* Description                           :   EEPROM驱动 读操作函数
* Author                                :   Hall
* Creat Date                            :   2024-07-12
* notice                                :   从旧程序移植过来的读操作函数
*
*                                           u32Len：是读取数据的字数---16位，不是8位
****************************************************************************************************/
bool sEepromRecv(u16 u16WordAddr, u32 u32Len, u16 *pBuf)
{
    u16 u16LenTemp;
    stEepromDrvDevice_t *pDevice = &stEepromDrvDev;
    
    if(u32Len == 0)
    {
        return true;
    }
    
    xSemaphoreTake(pDevice->xSemEeprom, portMAX_DELAY);
    u16WordAddr &= 0xFFFE; //must be even
    u32Len <<= 1; //change to byte
    
    do
    {
        u16LenTemp = pDevice->u8DevPageSize - (u16WordAddr % pDevice->u8DevPageSize);
        if(u16LenTemp > u32Len)
        {
            u16LenTemp = u32Len;
        }
        
        u32Len -= u16LenTemp;
        sI2cDrvStart(pDevice->eI2cNum);
        
        pDevice->unDevAddr.stAddr.bRw = cEepromDrvAt24c256cAddrWr;
        sI2cDrvSendByte(pDevice->eI2cNum, pDevice->unDevAddr.u8Addr);
        sI2cDrvWaitAck(pDevice->eI2cNum);
        sI2cDrvSendByte(pDevice->eI2cNum, (u16WordAddr >> 8)&0xFF);
        
        sI2cDrvWaitAck(pDevice->eI2cNum);
        sI2cDrvSendByte(pDevice->eI2cNum, u16WordAddr & 0xFF);
        sI2cDrvWaitAck(pDevice->eI2cNum);
        
        pDevice->unDevAddr.stAddr.bRw = cEepromDrvAt24c256cAddrRd;
        u16WordAddr += u16LenTemp;
        sI2cDrvStart(pDevice->eI2cNum);
        sI2cDrvSendByte(pDevice->eI2cNum, pDevice->unDevAddr.u8Addr);
        sI2cDrvWaitAck(pDevice->eI2cNum);
        
        while (u16LenTemp)
        {
            u16LenTemp -= 2;
            *pBuf = sI2cDrvRecvByte(pDevice->eI2cNum);
            sI2cDrvSendAck(pDevice->eI2cNum);
            *pBuf += ((uint16_t)sI2cDrvRecvByte(pDevice->eI2cNum) << 8);
            if(u16LenTemp != 0)sI2cDrvSendAck(pDevice->eI2cNum);
            pBuf++;
        }
        sI2cDrvSendNAck(pDevice->eI2cNum);
        sI2cDrvStop(pDevice->eI2cNum);
    }while(u32Len);
    
    xSemaphoreGive(pDevice->xSemEeprom);
    
    return true;
}










/***************************************************************************************************
* Description                           :   EEPROM驱动 写操作函数
* Author                                :   Hall
* Creat Date                            :   2024-07-12
* notice                                :   u32WordAddr:eeprom芯片写操作的 word address
*
*                                           EEPROM芯片特性:从任意地址(u32WordAddr)开始写入长度u32Len
*                                           都不能操过页大小,
*                                           如果超过，需要拆分成多次页写操作依次执行
*
*                                           u32Len：是待写数据的字数---16位，不是8位
****************************************************************************************************/
bool sEepromSend(u16 u16WordAddr, u32 u32Len, const u16 * data)
{
    u16 u16LenTemp;
    stEepromDrvDevice_t *pDevice = &stEepromDrvDev;
    
    if(u32Len == 0)
    {
        return true;
    }
    xSemaphoreTake(pDevice->xSemEeprom, portMAX_DELAY);
    
    u16WordAddr &= 0xFFFE; //must be even
    u32Len <<= 1; //change to byte
    pDevice->unDevAddr.stAddr.bRw = cEepromDrvAt24c256cAddrWr;
    
    do
    {
        u16LenTemp =  pDevice->u8DevPageSize - (u16WordAddr % pDevice->u8DevPageSize);
        if(u16LenTemp > u32Len)u16LenTemp = u32Len;
        u32Len -= u16LenTemp;
        
        sI2cDrvStart(pDevice->eI2cNum);
        
        sI2cDrvSendByte(pDevice->eI2cNum, pDevice->unDevAddr.u8Addr);
        sI2cDrvWaitAck(pDevice->eI2cNum);
        sI2cDrvSendByte(pDevice->eI2cNum, (u16WordAddr >> 8) & 0xFF);
        
        sI2cDrvWaitAck(pDevice->eI2cNum);
        sI2cDrvSendByte(pDevice->eI2cNum, u16WordAddr & 0xFF);
        sI2cDrvWaitAck(pDevice->eI2cNum);
        
        u16WordAddr += u16LenTemp;
        
        while (u16LenTemp)
        {
            u16LenTemp -= 2;
            sI2cDrvSendByte(pDevice->eI2cNum, (*data >> 0) & 0xFF);
            sI2cDrvWaitAck(pDevice->eI2cNum);
            sI2cDrvSendByte(pDevice->eI2cNum, (*data >> 8) & 0xFF);
            sI2cDrvWaitAck(pDevice->eI2cNum);
            data++;
        }
        sI2cDrvStop(pDevice->eI2cNum);
        vTaskDelay(cEepromDrvAt24c256cTwr / portTICK_RATE_MS);
    }while(u32Len);
    xSemaphoreGive(pDevice->xSemEeprom);
    
    return true;
}


















