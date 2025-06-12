/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_app_shell.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-23
 * @Attention             :     
 * @Brief                 :     LED灯语shell命令
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-23
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "led_app_shell.h"






stLedSignalTypeShell_t stLedSignalTypeShell[] = 
{
    {"light",  eLedSignalTypeLight    },
    {"water",  eLedSignalTypeWaterfall},
    {"breath", eLedSignalTypeBreath   },
    {"blink",  eLedSignalTypeBlink    },
};


stLedColorShell_t stLedColorShell[] = 
{
    {"black",  eLedColorBlack },
    {"blue",   eLedColorBlue  },
    {"red",    eLedColorRed   },
    {"green",  eLedColorGreen },
    {"yellow", eLedColorYellow},
    {"white",  eLedColorWhite },
};






bool sLedAppShellRegister(void);

bool sEnlogShellSetLed(const stShellPkt_t *pkg);






stShellCmd_t stSellCmdSetLed = 
{
    .pCmd       = "setled",
    .pFormat    = "格式:setled 枪号 亮灯模式 颜色 数据更新时间(ms)",
    .pFunction  = "功能:设置led",
    .pRemarks   = "备注:亮灯模式 : 1.light 2.water 3.breath 4.blink\
\n\r\t\t\t\t\t\t\t\t  颜色     : 1.black 2.blue 3.red 4.green 5.yellow 6.white",
    .pFunc      = sEnlogShellSetLed,
};












/*******************************************************************************
 * @FunctionName   :      sLedAppShellRegister
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月23日  17:56:54
 * @Description    :      LED灯语Shell命令注册
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sLedAppShellRegister(void)
{
    bool bRst = true;
    
    bRst = bRst & sShellCmdRegister(&stSellCmdSetLed);
    
    return(bRst);
}









/*******************************************************************************
 * @FunctionName   :      sEnlogShellSetLed
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月23日  17:56:55
 * @Description    :      LED灯语Shell命令 设置LED
 * @Input          :      pkg         
 * @Return         :      
*******************************************************************************/
bool sEnlogShellSetLed(const stShellPkt_t *pkg)
{
    u8   u8GunIndex, u8SignalType, i;
    u16  u32UpdateTime;
    u32  u32Color;
    char u8SignalString[32];
    char u8ColorString[32];
    
    u8GunIndex = (u8)atoi(pkg->para[0]);
    
    for(i = 0; i < (sizeof(stLedSignalTypeShell)/sizeof(stLedSignalTypeShell[0])); i++)
    {
        if(memcmp(pkg->para[1], stLedSignalTypeShell[i].pString, strlen(stLedSignalTypeShell[i].pString)) == 0)
        {
            u8SignalType = stLedSignalTypeShell[i].eLedSignalType;
            break;
        }
    }
    
    for(i = 0; i < (sizeof(stLedColorShell)/sizeof(stLedColorShell[0])); i++)
    {
        if(memcmp(pkg->para[2], stLedColorShell[i].pString, strlen(stLedColorShell[i].pString)) == 0)
        {
            u32Color = stLedColorShell[i].eLedColor;
            break;
        }
    }
    
    u32UpdateTime = (u8)atoi(pkg->para[3]);
    
    sLedAppSet(u8GunIndex, u32Color, u8SignalType, u32UpdateTime);
    
    return(true);
}












