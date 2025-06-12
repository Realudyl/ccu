/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_drv.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-22
 * @Attention             :     
 * @Brief                 :     LED驱动程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-22
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "led_drv.h"








stLedDrvCache_t stLedDrvCache;







//RGB灯板资源初始化
void sLedDrvInit(void);
void sLedDrvChgParam(bool bIsSuperChg);




//RGB颜色设置/获取
u32  sLedDrvSetColor(u8 u8Green, u8 u8Red, u8 u8Blue);
void sLedDrvGetColor(u8 *pColor, u32 u32Color);


//RGB颜色数据写入
void sLedDrvWriteByte(u8 u8LedIndex, u32 u32Data);


//RGB灯板亮灯效果
void sLedDrvLight(i32 i32GunIndex, u32 u32Color);
void sLedDrvWaterfall(i32 i32GunIndex, u32 u32Color);
void sLedDrvBreath(i32 i32GunIndex, u32 u32Color);
void sLedDrvBLink(i32 i32GunIndex, u32 u32Color);












/*******************************************************************************
 * @FunctionName   :      sLedDrvInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  15:36:50
 * @Description    :      LED灯板初始化
*******************************************************************************/
void sLedDrvInit(void)
{
    u8 i;
    
    
    //LED灯板资源初始化
    stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedStartIndex = cLedStartIndex_GunA;
    stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedNum        = cLedNum_GunA;
    stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedStartIndex = cLedStartIndex_GunB;
    stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedNum        = cLedNum_GunB;
    
    
    //缓存数组初始化
    for(i = 0; i < (24 * cLedNum); i++)
    {
        stLedDrvCache.u16LedBuff[i] = T0H;
    }
    
    
    //开启PWM
    sIoPwmDrvSetPwm(eIoPwm_LED_Board, true);
}







/*******************************************************************************
 * @FunctionName   :      sLedDrvChgParam
 * @Author         :      haisheng.dang
 * @DateTime       :      2024.06.24
 * @Description    :      LED灯板资源更新
*******************************************************************************/
void sLedDrvChgParam(bool bIsSuperChg)
{
    //LED灯板资源随液冷通讯状态改变而改变
    if(bIsSuperChg == true)
    {
        stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedStartIndex = cLedStartIndex_GunA_spec;
        stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedNum        = cLedNum_GunA_spec;
        stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedStartIndex = cLedStartIndex_GunB_spec;
        stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedNum        = cLedNum_GunB_spec;
    }
    else
    {
        stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedStartIndex = cLedStartIndex_GunA;
        stLedDrvCache.stLedConfig[ePileGunIndex0].u8LedNum        = cLedNum_GunA;
        stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedStartIndex = cLedStartIndex_GunB;
        stLedDrvCache.stLedConfig[ePileGunIndex1].u8LedNum        = cLedNum_GunB;
    }
    
}







/*******************************************************************************
 * @FunctionName   :      sLedDrvSetColor
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  16:07:08
 * @Description    :      LED灯板 设置颜色
 * @Input          :      u8Green     
 * @Input          :      u8Red       
 * @Input          :      u8Blue      
 * @Return         :      
*******************************************************************************/
u32 sLedDrvSetColor(u8 u8Green, u8 u8Red, u8 u8Blue)
{
    return ((u8Green << 16) | (u8Red << 8) | u8Blue);
}






/*******************************************************************************
 * @FunctionName   :      sLedDrvGetColor
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  16:07:12
 * @Description    :      LED灯板 获取颜色
 * @Input          :      pColor      分离出来的颜色
 * @Input          :      u32Color    要分离的颜色
*******************************************************************************/
void sLedDrvGetColor(u8 *pColor, u32 u32Color)
{
    pColor[2] = (u32Color >> 16) & 0xff;            //GREEN
    pColor[1] = (u32Color >>  8) & 0xff;            //RED
    pColor[0] =  u32Color        & 0xff;            //BLUE
}






/*******************************************************************************
 * @FunctionName   :      sLedDrvWriteByte
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月16日  10:27:47
 * @Description    :      LED灯板 写数据
 * @Input          :      u8LedIndex        LED灯号
 * @Input          :      u32Data           RGB颜色数据
*******************************************************************************/
void sLedDrvWriteByte(u8 u8LedIndex, u32 u32Data)
{
    u8 i;
    
    
    for(i = 0; i < 24; i++)
    {
        stLedDrvCache.u16LedBuff[(u8LedIndex * 24) + i] = (u32Data & 0x800000) ? T1H : T0H;
        
        u32Data = u32Data << 1;
    }
}






/*******************************************************************************
 * @FunctionName   :      sLedDrvLedLight
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月16日  10:27:50
 * @Description    :      LED灯板 常亮
 * @Input          :      i32GunIndex       枪号
 * @Input          :      u32Color          RGB颜色
*******************************************************************************/
void sLedDrvLight(i32 i32GunIndex, u32 u32Color)
{
    u8 i;
    
    
    for(i = stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex; i < (stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex + stLedDrvCache.stLedConfig[i32GunIndex].u8LedNum); i++)
    {
        sLedDrvWriteByte(i, u32Color);
    }
}







/*******************************************************************************
 * @FunctionName   :      sLedDrvWaterfall
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  16:07:42
 * @Description    :      LED灯板 流水
 * @Input          :      i32GunIndex       枪号
 * @Input          :      u32Color          RGB颜色
*******************************************************************************/
void sLedDrvWaterfall(i32 i32GunIndex, u32 u32Color)
{
    u8 i;
    static u8 u8Bright[ePileGunIndexNum];
    static u8 u8Count[ePileGunIndexNum];
    
    
    if(u8Bright[i32GunIndex] < stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex)
    {
        u8Bright[i32GunIndex] = stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex;
    }
    
    for(i = stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex; i < (stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex + stLedDrvCache.stLedConfig[i32GunIndex].u8LedNum); i++)
    {
        if(i == u8Bright[i32GunIndex])
        {
            sLedDrvWriteByte(i, u32Color);
        }
        else
        {
            sLedDrvWriteByte(i, eLedColorBlack);
        }
    }
    
    if(u8Count[i32GunIndex] == 4)
    {
        u8Bright[i32GunIndex] = (u8Bright[i32GunIndex] + 1) % (stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex + stLedDrvCache.stLedConfig[i32GunIndex].u8LedNum);
    }
    u8Count[i32GunIndex] = (u8Count[i32GunIndex] + 1) % 5;
}






/*******************************************************************************
 * @FunctionName   :      sLedDrvBreath
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  17:01:28
 * @Description    :      LED灯板 呼吸
 * @Input          :      i32GunIndex       枪号
 * @Input          :      u32Color          RGB颜色
*******************************************************************************/
void sLedDrvBreath(i32 i32GunIndex, u32 u32Color)
{
    u8  i;
    u8  u8RgbColor[3];
    u32 u32BreathColor;
    static u8  u8Breathflag[ePileGunIndexNum];
    static u16 u16BreathCount[ePileGunIndexNum];
    
    
    //分离RGB
    sLedDrvGetColor(u8RgbColor, u32Color);
    
    for(i = 0; i < 3; i++)
    {
        if(u8RgbColor[i] > 0)
        {
            u8RgbColor[i] = u8RgbColor[i] - u16BreathCount[i32GunIndex];
        }
    }
    
    //合成RGB颜色
    u32BreathColor = sLedDrvSetColor(u8RgbColor[2], u8RgbColor[1], u8RgbColor[0]);
    
    
    for(i = stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex; i < (stLedDrvCache.stLedConfig[i32GunIndex].u8LedStartIndex + stLedDrvCache.stLedConfig[i32GunIndex].u8LedNum); i++)
    {
        sLedDrvWriteByte(i, u32BreathColor);
    }
    
    
    if(u16BreathCount[i32GunIndex] >= 0xFF)
    {
        //切换为渐灭
        u8Breathflag[i32GunIndex] = 0;
    }
    else if(u16BreathCount[i32GunIndex] <= 0)
    {
        //切换为渐亮
        u8Breathflag[i32GunIndex] = 1;
    }
    
    
    if(u8Breathflag[i32GunIndex])
    {
        //渐亮
        u16BreathCount[i32GunIndex] ++;
    }
    else
    {
        //渐灭
        u16BreathCount[i32GunIndex] --;
    }
}






/*******************************************************************************
 * @FunctionName   :      sLedDrvBLink
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月20日  17:21:54
 * @Description    :      LED灯板 闪烁
 * @Input          :      i32GunIndex       枪号
 * @Input          :      u32Color          RGB颜色
*******************************************************************************/
void sLedDrvBLink(i32 i32GunIndex, u32 u32Color)
{
    static u8 u8BLinkFlag[ePileGunIndexNum];
    
    u8BLinkFlag[i32GunIndex] ^= 1;
    
    (u8BLinkFlag[i32GunIndex] == 1) ? (sLedDrvLight(i32GunIndex, u32Color)) : (sLedDrvLight(i32GunIndex, eLedColorBlack));
}

