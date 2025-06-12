#include "gd32h7xx.h"

#include "AutoGenVersion.h"

#include "en_backtrace.h"
#include "en_common.h"
#include "en_shell.h"
#include "en_mem.h"
#include "rtc_drv_interface.h"
#include "model_dc_cn_en_480kw.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "MAIN";







void cache_enable(void);
void mpu_setup_global(void);

int  main(void);
void sInitTask(void *pvParameters);










/*!
    \brief      enable the CPU cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable i-cache */
    SCB_EnableICache();
    
    /* enable d-cache */
    SCB_EnableDCache();
}






void mpu_setup_global(void)
{
    mpu_region_init_struct mpu_init_struct;
    
    //开启OSPI 相关时钟，方式访问该区域的时候 程序跑飞，SWD不能连接
    rcu_periph_clock_enable(RCU_OSPI0);
    rcu_periph_clock_enable(RCU_OSPI1);
    
    
    /* disable MPU */
    ARM_MPU_Disable();
    
    
    /* 全部4G区域防止非法访问，起到保护非法访问地址错误*/
    mpu_init_struct.region_number       = MPU_REGION_NUMBER0;
    mpu_init_struct.region_base_address = 0x00000000U;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.access_permission   = MPU_AP_NO_ACCESS;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_init_struct.access_shareable    = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.subregion_disable   = 0x87;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_4GB;
    
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();
    
    
    /* enable MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}






/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    //判断APP程序完整性标识符并跳转至APP，APP程序标识符不完整进入Bootloader程序
    sBootOtaJumpToApp();
    
    
    //开启OSPI 相关时钟，方式访问该区域的时候 程序跑飞，SWD不能连接。
    //初始化的时候，将所有flash地址保护，防止PC指针无访问，假如误访问会进MemManage_Handler()函数，则在里面相应处理
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_OSPI0);
    rcu_periph_clock_enable(RCU_OSPI1);
    mpu_setup_global();
    
    
    /* init task */
    xTaskCreate(sInitTask, "sInitTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    
    /* start scheduler */
    vTaskStartScheduler();
    
    while(1)
    {
    }
}






void sInitTask(void *pvParameters)
{
    /* enable the CPU Cache */
    cache_enable();
    
    //可设置范围：抢占优先级0~3,响应优先级0~3
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    
    
    //shell界面初始化
    sShellInit();
    
    //RTC初始化
    sRtcDrvInitDev();
    
    //充电桩用户代码初始化
    EN_SLOGI(TAG, "CCU充电控制板软件启动:");
    EN_SLOGI(TAG, "App固件版本:%s", cAppVersion);
    EN_SLOGI(TAG, "App固件日期:%s", cAppCommitTime);
    EN_SLOGI(TAG, "SDK固件版本:%s", cSdkVersion);
    EN_SLOGI(TAG, "SDK固件日期:%s", cSdkCommitTime);
    
    
    //机型初始化
    sModelInit_dc_cn_en_480k();
    
    //CmBackTrace初始化
    sCmBackTraceInit("Ccu");
    
    //打印堆栈使用情况
    sys_print_memory_used_info();
    
    
    for(;;)
    {
        vTaskDelete(NULL);
    }
}






