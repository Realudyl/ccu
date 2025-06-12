/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     pile_cn_gate_ota.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-6-12
 * @Attention             :     
 * @Brief                 :     国标充电桩实现 内网协议用户程序部分实现 之 OTA升级功能
 * 
 * @History:
 * 
 * 1.@Date: 2024-6-12
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "pile_cn.h"






//App程序升级完成标志位
volatile const uint8_t OkFlag[2] __attribute__((section("OKFLAG"),used)) = {'O','K'};




//目标CPU地址
#define cTargetCpuAddr                  0



//App程序升级完成标志位储存地址
#define cFlagOkAddr                     0x083BFFFC                              //< BOOT中OTA状态/正常启动状态  标志判断位存储区，位于flash 上



//APP程序完整标志
#define cCodeFlag_O                     (*((uint8_t*)0x083BFFFC))
#define cCodeFlag_K                     (*((uint8_t*)0x083BFFFD))













void sPileCnGateOtaGetInBoot(void);










/*******************************************************************************
 * @FunctionName   :      sPileCnGateOtaGetInBoot
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:03
 * @Description    :      进入BOOT模式
 * @Input          :      void        
*******************************************************************************/
void sPileCnGateOtaGetInBoot(void)
{
    u16 u16Rst = 1;
    
    fmc_unlock();
    
    //关中断
    __disable_irq();
    
    //擦除OTA升级标志
    if(fmc_sector_erase(cFlagOkAddr) == 0)
    {
        u16Rst = 0;
    }
    
    //开中断
    __enable_irq();
    
    fmc_lock();
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdRebootBoot, u16Rst);
    
    vTaskDelay(500 / portTICK_RATE_MS);
    
    __set_FAULTMASK(1);
    
    //系统复位
    NVIC_SystemReset();
}





