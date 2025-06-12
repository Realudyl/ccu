/**
 * @file fwdog.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 
 * @version 0.1
 * @date 2024-08-02
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */
 
#include "gd32h7xx.h"
#include "fwdog.h"
#include "en_log.h"




void sFwdogTask(void *pvParameters);
bool sFwdog_Init(void);

/**
 * @brief 看门狗
 * 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-08-02
 */
bool sFwdog_Init(void)
{
    return xTaskCreate(sFwdogTask, "sFwdogTask", 100u, NULL, cFwdogPriv, NULL);
}


/**
 * @brief 看门狗任务
 * 
 * 
 * @param pvParameters 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-08-02
 */
void sFwdogTask(void *pvParameters)
{
    //等待别的任务初始化
    vTaskDelay(pdMS_TO_TICKS((u32)(cFwdogInitWaitMs)));

    //进入看门狗配置模式
    fwdgt_write_enable();

    //计算看门狗计数器初值
    /*
     *时钟32k，分频32，则计数单位为ms
     */
    fwdgt_config(cFwdogMs, FWDGT_PSC_DIV32);

    //当arm core被挂起时，看门狗停止计数
    dbg_periph_enable(DBG_FWDGT_HOLD);

    //启动看门狗
    fwdgt_enable();
    while(1)
    {
        fwdgt_counter_reload();
        vTaskDelay(pdMS_TO_TICKS((u32)(cFwdogCycleMs)));
    }
}