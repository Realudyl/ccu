/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   i2c_drv.c
* Description                           :   GPIO软件模拟I2C接口驱动程序实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-07-03
* notice                                :   
****************************************************************************************************/
#include "i2c_drv.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "i2c_drv";




extern stI2cDrvDevice_t stI2cDrvDevMap[eI2cNumMax];

#define NOPs() sI2cDrvDelayUs(2)
#define SDA_IN()
#define SDA_OUT()
#define SCL_OUT()











void sI2cDrvDelay  (__IO uint32_t nCount);
void sI2cDrvDelayUs(__IO uint32_t nCount);


u8   sI2cDrvGetSda (eI2cNum_t eI2cNum);
void sI2cDrvSetSdaH(eI2cNum_t eI2cNum);
void sI2cDrvSetSdaL(eI2cNum_t eI2cNum);
u8   sI2cDrvGetScl (eI2cNum_t eI2cNum);
void sI2cDrvSetSclH(eI2cNum_t eI2cNum);
void sI2cDrvSetSclL(eI2cNum_t eI2cNum);


void sI2cDrvStart   (eI2cNum_t eI2cNum);
void sI2cDrvStop    (eI2cNum_t eI2cNum);
u8   sI2cDrvWaitAck (eI2cNum_t eI2cNum);
void sI2cDrvSendAck (eI2cNum_t eI2cNum);
void sI2cDrvSendNAck(eI2cNum_t eI2cNum);
u8   sI2cDrvRecvByte(eI2cNum_t eI2cNum);
void sI2cDrvSendByte(eI2cNum_t eI2cNum, u8 u8Byte);


bool sI2cDrvOpen(eI2cNum_t eI2cNum);









void sI2cDrvDelay(__IO uint32_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}






void sI2cDrvDelayUs(__IO uint32_t nCount)
{
    /* 10uS延迟=>10.8uS,100uS延迟=>104uS*/
    while (nCount != 0)
    {
        nCount--;
        sI2cDrvDelay(10);
    }
}








u8 sI2cDrvGetSda(eI2cNum_t eI2cNum)
{
    return gpio_input_bit_get(stI2cDrvDevMap[eI2cNum].stGpioSda.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioSda.i32Pin);
}



void sI2cDrvSetSdaH(eI2cNum_t eI2cNum)
{
    gpio_bit_set(stI2cDrvDevMap[eI2cNum].stGpioSda.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioSda.i32Pin);
}


void sI2cDrvSetSdaL(eI2cNum_t eI2cNum)
{
    gpio_bit_reset(stI2cDrvDevMap[eI2cNum].stGpioSda.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioSda.i32Pin);
}



u8 sI2cDrvGetScl(eI2cNum_t eI2cNum)
{
    return gpio_input_bit_get(stI2cDrvDevMap[eI2cNum].stGpioScl.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioScl.i32Pin);
}


void sI2cDrvSetSclH(eI2cNum_t eI2cNum)
{
    gpio_bit_set(stI2cDrvDevMap[eI2cNum].stGpioScl.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioScl.i32Pin);
}


void sI2cDrvSetSclL(eI2cNum_t eI2cNum)
{
    gpio_bit_reset(stI2cDrvDevMap[eI2cNum].stGpioScl.i32Periph, stI2cDrvDevMap[eI2cNum].stGpioScl.i32Pin);
}




void sI2cDrvStart(eI2cNum_t eI2cNum)
{
    SDA_OUT();
    NOPs();
    sI2cDrvSetSdaH(eI2cNum);
    sI2cDrvSetSclH(eI2cNum);
    NOPs();
    sI2cDrvSetSdaL(eI2cNum);
    NOPs();
    sI2cDrvSetSclL(eI2cNum);
}




void sI2cDrvStop(eI2cNum_t eI2cNum)
{
    SDA_OUT();
    NOPs();
    sI2cDrvSetSclL(eI2cNum);
    sI2cDrvSetSdaL(eI2cNum);
    NOPs(); 
    sI2cDrvSetSclH(eI2cNum);
    NOPs();
    sI2cDrvSetSdaH(eI2cNum);
}





u8 sI2cDrvWaitAck(eI2cNum_t eI2cNum)
{
    u8 u8ErrCnt = 255;
    
    SDA_OUT();
    
    sI2cDrvSetSdaH(eI2cNum);
    NOPs();
    SDA_IN();
    sI2cDrvSetSclL(eI2cNum);
    NOPs();
    sI2cDrvSetSclH(eI2cNum);
    NOPs();
    while(sI2cDrvGetSda(eI2cNum))
    {
        u8ErrCnt--;
        if (!u8ErrCnt)
        {
            sI2cDrvStop(eI2cNum);
            return 0;
        }
    }
    sI2cDrvSetSclL(eI2cNum);
    
    return 1;
}





void sI2cDrvSendAck(eI2cNum_t eI2cNum)
{
    SDA_OUT();
    sI2cDrvSetSdaL(eI2cNum);
    NOPs();
    sI2cDrvSetSclH(eI2cNum);
    NOPs();
    sI2cDrvSetSclL(eI2cNum);
}






void sI2cDrvSendNAck(eI2cNum_t eI2cNum)
{
    SDA_OUT();
    sI2cDrvSetSdaH(eI2cNum);
    NOPs();
    sI2cDrvSetSclH(eI2cNum);
    NOPs();
    sI2cDrvSetSclL(eI2cNum);
}






u8 sI2cDrvRecvByte(eI2cNum_t eI2cNum)
{
    u8 i = 8;
    u8 u8Data = 0;
    
    SDA_IN();
    sI2cDrvSetSdaH(eI2cNum);
    while (i--)
    {
        u8Data <<= 1;
        sI2cDrvSetSclL(eI2cNum);NOPs();
        sI2cDrvSetSclH(eI2cNum);NOPs();
        u8Data |= sI2cDrvGetSda(eI2cNum);
    }
    sI2cDrvSetSclL(eI2cNum);
    SDA_OUT();
    
    return u8Data;
}






void sI2cDrvSendByte(eI2cNum_t eI2cNum, u8 u8Byte)
{
    u8 i = 8;
    
    SDA_OUT();
    while (i--)
    {
        sI2cDrvSetSclL(eI2cNum);
        NOPs();
        if(u8Byte & 0x80)
        {
            sI2cDrvSetSdaH(eI2cNum);
        }
        else
        {
            sI2cDrvSetSdaL(eI2cNum);
        }
        u8Byte <<= 1;
        NOPs();
        sI2cDrvSetSclH(eI2cNum);
        NOPs();
        sI2cDrvSetSclL(eI2cNum);
    }
    sI2cDrvSetSclL(eI2cNum);
}














/***************************************************************************************************
* Description                           :   I2C 端口打开
* Author                                :   Hall
* Creat Date                            :   2024-07-03
* notice                                :   
****************************************************************************************************/
bool sI2cDrvOpen(eI2cNum_t eI2cNum)
{
    stI2cDrvDevice_t *pDevice = NULL;
    
    if(eI2cNum >= eI2cNumMax)
    {
        //I2C接口超限
        return(false);
    }
    pDevice = &stI2cDrvDevMap[eI2cNum];
    
    rcu_periph_clock_enable(pDevice->stGpioScl.i32PeriphRcu);
    rcu_periph_clock_enable(pDevice->stGpioSda.i32PeriphRcu);
    
    gpio_mode_set(pDevice->stGpioScl.i32Periph, pDevice->stGpioScl.i32Mode, pDevice->stGpioScl.i32Pull, pDevice->stGpioScl.i32Pin);
    gpio_mode_set(pDevice->stGpioSda.i32Periph, pDevice->stGpioSda.i32Mode, pDevice->stGpioSda.i32Pull, pDevice->stGpioSda.i32Pin);
    
    gpio_output_options_set(pDevice->stGpioScl.i32Periph, GPIO_OTYPE_OD, pDevice->stGpioScl.i32Speed, pDevice->stGpioScl.i32Pin);
    gpio_output_options_set(pDevice->stGpioSda.i32Periph, GPIO_OTYPE_OD, pDevice->stGpioSda.i32Speed, pDevice->stGpioSda.i32Pin);
    
    return true;
}








































