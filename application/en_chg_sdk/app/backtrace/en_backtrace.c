/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     en_backtrace.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-1-8
 * @Attention             :     
 * @Brief                 :     CmBackTrace驱动 ———— 根据以往工程整理
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-8
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include <stdarg.h>

#include "en_backtrace.h"
#include "en_mem.h"
#include "en_shell.h"

#include "cm_backtrace.h"
#include "usart_drv_trans.h"










bool sCmBackTraceInit(const char *pFirmwareName);

bool sShellGetStack(const stShellPkt_t *pkg);







//错误诊断定位测试命令 注册表
stShellCmd_t stSellCmdCmBackTraceTestCmd = 
{
    .pCmd       = "getstack",
    .pFormat    = "格式:getstack",
    .pFunction  = "功能:打印堆栈信息",
    .pRemarks   = "备注:无",
    .pFunc      = sShellGetStack,
};









/*******************************************************************************
 * @FunctionName   :      sCmBackTraceInit
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月8日  19:55:49
 * @Description    :      CmBackTrace驱动 初始化
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sCmBackTraceInit(const char *pFirmwareName)
{
    //初始化hardfault追踪工具
    cm_backtrace_init(pFirmwareName, "V1.0.0", "V1.0.0");
    
    //注册错误诊断追踪测试命令
    return(sShellCmdRegister(&stSellCmdCmBackTraceTestCmd));
}











/*******************************************************************************
 * @FunctionName   :      sCmBackTracePrint
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年1月8日  19:58:59
 * @Description    :      CmBackTrace驱动 之 故障追踪打印   用于<cmb_cfg.h>中的cmb_println宏定义
 * @Input          :      fmt         
 * @Input          :      ...         
*******************************************************************************/
void sCmBackTracePrint(const char *fmt, ...)
{
    u16 u16Len = 0;
    static char u8Buf[256];
    va_list ap;
    
    //申请ap并初始化
    va_start(ap, fmt);
    
    //将可变参数格式化为字符串，并将结果存储在指定的缓冲区中
    u16Len = vsnprintf(u8Buf, sizeof(u8Buf), fmt, ap);
    
    //释放ap
    va_end(ap);
    
    for(u8 i = 0; i < u16Len; i++)
    {
        sUsartDrvSendCmBackTrace(cShellComUartNum, (u8Buf[i] & 0xFFFF));
    }
}







//测试定位错误命令
bool sShellGetStack(const stShellPkt_t *pkg)
{
    sys_print_memory_used_info();
    return(true);
}







