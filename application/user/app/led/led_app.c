/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     led_app.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-5-22
 * @Attention             :     
 * @Brief                 :     LED用户程序
 * 
 * @History:
 * 
 * 1.@Date: 2024-5-22
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "led_app.h"

#include "led_app_shell.h"







stLedAppCtrlParam_t stLedAppCtrlParam[ePileGunIndexNum];


extern stIoPwmDrvDevice_t stIoPwmDrvDevMap[];
extern stLedDrvCache_t    stLedDrvCache;




//LED用户程序开启
void sLedAppOpen(bool bIsSuperCharger);


//LED用户设置接口
bool sLedAppSet(i32 i32GunIndex, eLedColor_t eColor, eLedSignalType_t eSignalType, u32 u32UpdateTime);


//LED用户任务
void sLedAppTask(void *pParam);












/*******************************************************************************
 * @FunctionName   :      sLedAppOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月22日  17:11:44
 * @Description    :      LED用户程序开启
 * @Input          :      void        
*******************************************************************************/
void sLedAppOpen(bool bIsSuperCharger)
{
    //LED资源初始化
    sLedDrvInit();
    
    //LED Shell命令注册
    sLedAppShellRegister();
    
    //20240624_hason_add:液冷终端B枪的灯需同步A枪状态
    sLedDrvChgParam(bIsSuperCharger);
    xTaskCreate(sLedAppTask, "sLedAppTask", (48), NULL, 15, NULL);
}









/*******************************************************************************
 * @FunctionName   :      sLedAppSet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月22日  16:56:12
 * @Description    :      LED用户设置接口
 * @Input          :      i32GunIndex       枪号
 * @Input          :      eColor            RGB颜色
 * @Input          :      eSignalType       RGB亮灭模式
 * @Input          :      u32UpdateTime     数据更新时间 单位:ms
 * @Return         :      
*******************************************************************************/
bool sLedAppSet(i32 i32GunIndex, eLedColor_t eColor, eLedSignalType_t eSignalType, u32 u32UpdateTime)
{
    if((i32GunIndex >= ePileGunIndexNum) || (eSignalType >= eLedSignalTypeMax))
    {
        return(false);
    }
    
    stLedAppCtrlParam[i32GunIndex].eLedColor      = eColor;
    stLedAppCtrlParam[i32GunIndex].eLedSignalType = eSignalType;
    stLedAppCtrlParam[i32GunIndex].u32UpdataTime  = u32UpdateTime;
    
    return(true);
}













/*******************************************************************************
 * @FunctionName   :      sLedAppTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月22日  16:56:15
 * @Description    :      LED灯板任务
 * @Input          :      pParam      
*******************************************************************************/
void sLedAppTask(void *pParam)
{
    u8  i;
    u32 u32TimeStamp;
    
    
    while(1)
    {
        //获取当前时间戳
        u32TimeStamp = xTaskGetTickCount();
        
        for(i = 0; i < ePileGunIndexNum; i++)
        {
            //第i枪更新灯板数据
            if((u32TimeStamp - stLedAppCtrlParam[i].u32TimeStamp) >= stLedAppCtrlParam[i].u32UpdataTime)
            {
                stLedAppCtrlParam[i].u32TimeStamp = u32TimeStamp;
                
                switch(stLedAppCtrlParam[i].eLedSignalType)
                {
                case eLedSignalTypeLight:
                    sLedDrvLight(i, stLedAppCtrlParam[i].eLedColor);
                    break;
                    
                case eLedSignalTypeWaterfall:
                    sLedDrvWaterfall(i, stLedAppCtrlParam[i].eLedColor);
                    break;
                    
                case eLedSignalTypeBreath:
                    sLedDrvBreath(i, stLedAppCtrlParam[i].eLedColor);
                    break;
                    
                case eLedSignalTypeBlink:
                    sLedDrvBLink(i, stLedAppCtrlParam[i].eLedColor);
                    break;
                    
                default:
                    break;
                }
                
            }
        }
        
        sIoPwmDrvSendData(eIoPwm_LED_Board, stLedDrvCache.u16LedBuff, sizeof(stLedDrvCache.u16LedBuff));
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}












