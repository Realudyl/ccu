/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     boot.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-6-12
 * @Attention             :     
 * @Brief                 :     Bootloader主文件
 * 
 * @History:
 * 
 * 1.@Date: 2024-6-12
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "AutoGenVersion.h"
#include "en_common.h"
#include "en_log.h"
#include "private_drv_opt.h"

#include "boot.h"
#include "dfu_mal_user.h"




#define cLogLevel eEnLogLevelDebug
static const char *TAG = "boot";










//目标CPU地址
#define cTargetCpuAddr                  0


/* SRAM start address and SRAM end address */
#define cSramStartAddr                  SRAM_BASE
#define cSramEndAddr                    (SRAM_BASE + GET_BITS(REG32(0x1FF0F7E0U), 0U, 15U) * 1024U)


//App程序升级完成标志位储存地址
#define cFlagOkAddr                     0x083BFFFC                              //< BOOT中OTA状态/正常启动状态  标志判断位存储区，位于flash 上


//APP程序加载起始地址
#define cAppLoadedAddr                  APPENTRYADDR


//APP程序完整标志
#define cCodeFlag_O                     (*((uint8_t*)0x083BFFFC))
#define cCodeFlag_K                     (*((uint8_t*)0x083BFFFD))









void sBootOtaInit(void);
void sBootOtaTask(void *pParam);

void sBootOtaGateInit(void);
void sBootOtaJumpToApp(void);
void sBootOtaGetInApp(void);
void sBootOtaHandshake(void);
void sBootOtaErase(void);
void sBootOtaFlash(stPrivDrvCmdAB_t *pCmdAB);
void sBootOtaVerify(void);
void sBootOtaGetInBoot(void);











/*******************************************************************************
 * @FunctionName   :      sBootOtaInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  17:45:59
 * @Description    :      Bootloader OTA升级 初始化
 * @Input          :      void        
*******************************************************************************/
void sBootOtaInit(void)
{
    sLedAppSet(0, eLedColorYellow, eLedSignalTypeBlink, cLedUpdateTimeQuickBlink);
    sLedAppSet(1, eLedColorYellow, eLedSignalTypeBlink, cLedUpdateTimeQuickBlink);
    
    sBootOtaGateInit();
    
    xTaskCreate(sBootOtaTask, "sBootOtaTask", (512), NULL, 26, NULL);
}





/*******************************************************************************
 * @FunctionName   :      sBootOtaTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  17:46:00
 * @Description    :      Bootloader OTA升级 任务
 * @Input          :      pParam      
*******************************************************************************/
void sBootOtaTask(void *pParam)
{
    stPrivDrvCmdAB_t *pOta = NULL;
    
    EN_SLOGI(TAG, "任务建立:BootLoader OTA升级任务!!!");
    while(1)
    {
        if(sPrivDrvGetChgOta(&pOta) == true)
        {
            switch(pOta->eCmd)
            {
            case ePrivDrvUpdateCmdRebootApp:
                EN_SLOGI(TAG, "远程升级命令, 进入应用程序模式!!!");
                sBootOtaGetInApp();
                break;
            case ePrivDrvUpdateCmdHandshake:
                EN_SLOGI(TAG, "远程升级命令, 握手!!!");
                sBootOtaHandshake();
                break;
            case ePrivDrvUpdateCmdErase:
                EN_SLOGI(TAG, "远程升级命令, flash擦除指令!!!");
                vTaskDelay(100 / portTICK_RATE_MS);
                sBootOtaErase();
                break;
            case ePrivDrvUpdateCmdFlash:
                EN_SLOGI(TAG, "远程升级命令, flash写命令!!!");
                sBootOtaFlash(pOta);
                break;
            case ePrivDrvUpdateCmdVerify:
                EN_SLOGI(TAG, "远程升级命令, flash验证!!!");
                sBootOtaVerify();
                break;
            case ePrivDrvUpdateCmdRebootBoot:
                EN_SLOGI(TAG, "远程升级命令, 进入boot模式!!!");
                sBootOtaGetInBoot();
                break;
            default:
                EN_SLOGE(TAG, "远程升级命令, 未知!!!");
                break;
            }
        }
        
        vTaskDelay(20 / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}





/***************************************************************************************************
* Description                           :   BootLoader里与网关通讯的部分字段值初始化
* Author                                :   Hall
* Creat Date                            :   2024-08-06
* notice                                :   部分（签到报文里的）字段上电初始化赋值
****************************************************************************************************/
void sBootOtaGateInit(void)
{
    u8  u8Buf[48];
    
    sPrivDrvSetChargerType(sEepromAppGetBlockFactoryChargerType());
    sPrivDrvSetPayAt(sEepromAppGetBlockFactoryPayAt());
    sPrivDrvSetMoneyCalcBy(sEepromAppGetBlockFactoryMoneyCalcBy());
    sPrivDrvSetStartPowerMode(sEepromAppGetBlockFactoryPowerMode());
    sPrivDrvSetWorkMode(sEepromAppGetBlockFactoryWorkMode());
    sPrivDrvSetBalancePhase(sEepromAppGetBlockFactoryBalancePhase());
    sPrivDrvSetPowerMax((i32)sEepromAppGetBlockFactoryPowerMax());
    sPrivDrvSetCurrMax((i32)(sEepromAppGetBlockFactoryCurrMax() * 10));
    sPrivDrvSetPowerRated((i32)sEepromAppGetBlockFactoryPowerRated());
    sPrivDrvSetCurrRated((i32)(sEepromAppGetBlockFactoryCurrRated() * 10));
    sPrivDrvSetBookingViaBle(sEepromAppGetBlockFactoryBookingViaBle());
    sPrivDrvSetRebootReason(sEepromAppGetBlockFactoryRebootReason());
    
    sPrivDrvSetHwVer(strlen("V1.0"), (u8 *)"V1.0");
    sPrivDrvSetSwVer(strlen(cAppVersion), (u8 *)cAppVersion);
    sPrivDrvSetGunNum(ePileGunIndexNum);
    
    sPrivDrvSetDisableFlag(sEepromAppGetBlockFactoryDisableFlag());             //禁用标志
    
    //序列号
    sEepromAppGetBlockFactorySn((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetSn(strlen((char *)u8Buf), u8Buf);
    
    //DevCode
    sEepromAppGetBlockFactoryDevCode((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetDevCode(strlen((char *)u8Buf), u8Buf);
    
    //DevName
    sEepromAppGetBlockFactoryDevName((char *)u8Buf, sizeof(u8Buf));
    sPrivDrvSetDevName(strlen((char *)u8Buf), u8Buf);
}







/*******************************************************************************
 * @FunctionName   :      sBootOtaJumpToApp
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年5月27日  13:21:11
 * @Description    :      Bootloader跳转至用户程序
 * @Input          :      void        
*******************************************************************************/
void sBootOtaJumpToApp(void)
{
    void(*pApplication)(void);
    u32   u32AppAddr    = 0U;
    u32   u32SramSector = REG32(cAppLoadedAddr);
    
    //判断APP程序完整性标识符
    if((cCodeFlag_O == 'O') && (cCodeFlag_K == 'K'))
    {
        //关中断
        __disable_irq();
        
        if((u32SramSector >= cSramStartAddr) && (u32SramSector < cSramEndAddr))
        {
            //初始化用户应用程序的堆栈指针
            __set_MSP(*(__IO u32*) cAppLoadedAddr);
            
            pApplication = (void(*)(void))(*(__IO u32*) (cAppLoadedAddr + 4U));
            
            //跳转到APP
            pApplication();
        }
        
        //开中断
        __enable_irq();
    }
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaGetInApp
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:04
 * @Description    :      进入APP模式
 * @Input          :      void        
*******************************************************************************/
void sBootOtaGetInApp(void)
{
    u16 u16Rst = 0;
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdRebootApp, u16Rst);
    
    vTaskDelay(500 / portTICK_RATE_MS);
    
    //系统复位
    NVIC_SystemReset();
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaHandshake
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:54:57
 * @Description    :      握手
 * @Input          :      void        
*******************************************************************************/
void sBootOtaHandshake(void)
{
    u16 u16Rst = 0;
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdHandshake, u16Rst);
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaErase
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:06
 * @Description    :      擦除
 * @Input          :      void        
*******************************************************************************/
void sBootOtaErase(void)
{
    u16 u16Rst = 0;
    
    if(dfu_mal_erase() > 0)
    {
        //擦除失败
        u16Rst = 1;
    }
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdErase, u16Rst);
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaFlash
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:10
 * @Description    :      烧写
 * @Input          :      pCmdAB      
*******************************************************************************/
void sBootOtaFlash(stPrivDrvCmdAB_t *pCmdAB)
{
    u16 u16Rst  = 1;
    u32 u32Addr = 0;
    u16 u16Cnt  = 0;
    
    u32Addr = pCmdAB->u16AddrBase;
    u32Addr = (u32Addr<<16) | pCmdAB->u16Offset;
    
    if(u32Addr < APPENTRYADDR || u32Addr > APPENDADDR)
    {
        //bootloader区无需进行烧写,正常回复
        u16Rst = 0;
    }
    else if(dfu_mal_write(((u8*)pCmdAB->u16Data), u32Addr, (pCmdAB->u16Len * 2)) > 0)
    {
        //写入失败
        u16Rst = 1;
    }
    else
    {
        u16Rst = 0;
    }
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdFlash, u16Rst);
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaVerify
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:28
 * @Description    :      校验
 * @Input          :      void        
*******************************************************************************/
void sBootOtaVerify(void)
{
    u16 u16Rst = 0;
    
    //判断App程序升级完成标识符
    if((cCodeFlag_O != 'O') || (cCodeFlag_K != 'K'))
    {
        //验证失败
        u16Rst = 1;
    }
    
    sPrivDrvPktSend0B(cTargetCpuAddr, ePrivDrvUpdateCmdVerify, u16Rst);
}




/*******************************************************************************
 * @FunctionName   :      sBootOtaGetInBoot
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年6月12日  14:52:03
 * @Description    :      进入BOOT模式
 * @Input          :      void        
*******************************************************************************/
void sBootOtaGetInBoot(void)
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










