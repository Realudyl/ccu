/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     mse102x_drv.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-8
 * @Attention             :     
 * @Brief                 :     
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-8
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "netif_mse102x.h"
#include "mse102x_drv.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mse102x_drv";










stMse102xDrvCache_t stMse102xDrvCache;











extern stMse102xDrvDevice_t stMse102xDrvDevMap[cMse102xDevNum];










bool sMse102xOpen(i32 i32Channel);
i32  sMse102xReadHpav(i32 i32Channel, i32 i32MaxLen, u8 *pBuf);
void sMse102xTask(void *pPara);

void sMse102xDrvResetDevice(i32 i32Port, i32 i32Pin);
void sMse102xPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len);


bool sMse102xRecv(i32 i32Channel, i32 *pDataLen, u8 **pData);
bool sMse102xRecvSof(i32 i32Channel, stMse102xDrvSpiFrameSof_t *pFrame, i32 i32DataLen);
bool sMse102xRecvCmd(i32 i32Channel, stMse102xDrvSpiFrameCmd_t *pFrame);

bool sMse102xSend(i32 i32Channel, i32 i32DataLen, const u8 *pData);
bool sMse102xSendSof(i32 i32Channel, stMse102xDrvSpiFrameSof_t *pFrame, i32 i32Len);
bool sMse102xSendCmd(i32 i32Channel, u16 u16Param, eMse102xDrvSpiCmdType_t eCmd);

#ifdef DTT
i32  sMse102xReadEnet(i32 i32Channel, i32 i32MaxLen, u8 *pBuf);
#endif



























/*******************************************************************************
 * @FunctionName   :      sMse102xOpen
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  13:22:10
 * @Description    :      
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sMse102xOpen(i32 i32Channel)
{
    bool bRst;
    i32  i;
    
    i32 *pChannel = NULL;
    stMse102xDrvDevice_t *pDevice = NULL;
    
    
    
    //0:参数限制=
    EN_SLOGI(TAG, "MSE102X 驱动, 开启PLC通道:%d", i32Channel);
    if(i32Channel >= cMse102xDevNum)
    {
        EN_SLOGE(TAG, "MSE102X 端口:%d, 超限:%d", i32Channel, cMse102xDevNum);
        return(false);
    }
    pDevice = &stMse102xDrvDevMap[i32Channel];
    
    
    //1:配置map表
    memset(&stMse102xDrvCache, 0, sizeof(stMse102xDrvCache));
    if(en_queueInit(&stMse102xDrvCache.stHpavQueue[i32Channel], cMse102xDrvHpavQueueBufLen , cMse102xDrvHpavQueueSize, 0) == false)
    {
        EN_SLOGE(TAG, "内存不足, 队列创建失败!!!");
        return(false);
    }
    stMse102xDrvCache.xSemSend[i32Channel] = xSemaphoreCreateBinary();
    stMse102xDrvCache.xSemHpavQueue[i32Channel] = xSemaphoreCreateBinary();
    xSemaphoreGive(stMse102xDrvCache.xSemSend[i32Channel]);
    xSemaphoreGive(stMse102xDrvCache.xSemHpavQueue[i32Channel]);
    
#ifdef DTT
    if(en_queueInit(&stMse102xDrvCache.stEnetQueue[i32Channel], cMse102xDrvEnetQueueBufLen , cMse102xDrvEnetQueueSize, 0) == false)
    {
        EN_SLOGE(TAG, "内存不足, 队列创建失败!!!");
        return(false);
    }
    stMse102xDrvCache.xSemEnetQueue[i32Channel] = xSemaphoreCreateBinary();
    xSemaphoreGive(stMse102xDrvCache.xSemEnetQueue[i32Channel]);
#endif
    
    //2:硬件初始化
    for(i = 0; i < sArraySize(pDevice->stGpio); i++)
    {
        rcu_periph_clock_enable(pDevice->stGpio[i].i32PeriphRcu);
        gpio_mode_set(pDevice->stGpio[i].i32Periph, pDevice->stGpio[i].i32Mode, GPIO_PUPD_NONE, pDevice->stGpio[i].i32Pin);
        if(pDevice->stGpio[i].i32Mode == GPIO_MODE_OUTPUT)
        {
            gpio_output_options_set(pDevice->stGpio[i].i32Periph, GPIO_OTYPE_PP, pDevice->stGpio[i].i32Speed,  pDevice->stGpio[i].i32Pin);
        }
    }
    sMse102xDrvResetDevice(pDevice->stGpio[eMse102xDrvCfgGpioReset].i32Periph, pDevice->stGpio[eMse102xDrvCfgGpioReset].i32Pin);
    bRst = sSpiDrvOpen(pDevice->eSpiNum, &pDevice->stSpiCfg);
    
    
    //3:创建MSE102X任务
    pChannel = (i32 *)MALLOC(sizeof(i32));
    (*pChannel) = i32Channel;
    xTaskCreate(sMse102xTask, "sMse102xTask", (512), pChannel, 27, NULL);
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :     MSE102X驱动提供的一个接口 供外部读取homeplug av帧
* Author                                :     Hall
* Creat Date                            :     2022-07-04
* notice                                :     
****************************************************************************************************/
i32 sMse102xReadHpav(i32 i32Channel, i32 i32MaxLen, u8 *pBuf)
{
    i32 i32Len = 0;
    
    if(en_queueSize(&stMse102xDrvCache.stHpavQueue[i32Channel]) > 0)
    {
        xSemaphoreTake(stMse102xDrvCache.xSemHpavQueue[i32Channel], 2000 / portTICK_PERIOD_MS);
        i32Len = en_queuePop(&stMse102xDrvCache.stHpavQueue[i32Channel], NULL, pBuf, i32MaxLen);
        xSemaphoreGive(stMse102xDrvCache.xSemHpavQueue[i32Channel]);
    }
    
    return(i32Len);
}




#ifdef DTT
/*******************************************************************************
 * @FunctionName   :      sMse102xReadEnet
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年10月8日  10:31:52
 * @Description    :      
 * @Input          :      i32Channel  
 * @Input          :      i32MaxLen   
 * @Input          :      pBuf        
 * @Return         :      
*******************************************************************************/
i32 sMse102xReadEnet(i32 i32Channel, i32 i32MaxLen, u8 *pBuf)
{
    i32 i32Len = 0;
    
    if(en_queueSize(&stMse102xDrvCache.stEnetQueue[i32Channel]) > 0)
    {
        xSemaphoreTake(stMse102xDrvCache.xSemEnetQueue[i32Channel], 2000 / portTICK_PERIOD_MS);
        i32Len = en_queuePop(&stMse102xDrvCache.stEnetQueue[i32Channel], NULL, pBuf, i32MaxLen);
        xSemaphoreGive(stMse102xDrvCache.xSemEnetQueue[i32Channel]);
    }
    
    return(i32Len);
}
#endif






/*******************************************************************************
 * @FunctionName   :      sMse102xTask
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  13:22:45
 * @Description    :      MSE102X组件————轮询任务
 * @Input          :      pPara       
*******************************************************************************/
void sMse102xTask(void *pPara)
{
    u8   u8Name[4];
    i32  i32Channel;
    struct netif *pNetif = NULL;
    struct eth_hdr *pEthHdr = NULL;
    
    stMes102xNetifData_t *pNetifData = NULL;
    stMse102xDrvDevice_t *pDevice = NULL;
    
    u8   *pData = NULL;
    i32  i32DataLen;
    
    
    i32Channel = (*(i32 *)pPara);
    pDevice = &stMse102xDrvDevMap[i32Channel];
    
    
    //遍历网卡链表 寻找本网卡对应节点
    vTaskDelay(2000 / portTICK_RATE_MS);                                        //需要等待协议栈添加网卡之后才能遍历
    NETIF_FOREACH(pNetif)
    {
        pNetifData = pNetif->state;
        if(pNetifData->i32PlcDevIndex == i32Channel)
        {
            break;
        }
    }
    
    memset(u8Name, 0, sizeof(u8Name));
    memcpy(u8Name, pNetifData->u8Name, sizeof(pNetifData->u8Name));
    EN_SLOGI(TAG, "MSE102X 驱动, PLC通道:%d 主线程开始运行!!!", i32Channel);
    EN_SLOGI(TAG, "PLC通    道:%d", pNetifData->i32PlcDevIndex);
    EN_SLOGI(TAG, "PLC网  卡名:%s", u8Name);
    EN_HLOGI(TAG, "PLC物理地址:",   pNetifData->u8Mac, sizeof(pNetifData->u8Mac));
    
    while((i32Channel < cMse102xDevNum) && (pNetif != NULL))
    {
        //轮询检测SPI_INRT引脚电平
        if(sMse102xDrvHasData(pDevice->stGpio[eMse102xDrvCfgGpioIntRx].i32Periph, pDevice->stGpio[eMse102xDrvCfgGpioIntRx].i32Pin) == true)
        {
            if(sMse102xRecv(i32Channel, &i32DataLen, &pData) == true)
            {
                sMse102xPrint(false, pData, i32DataLen);
#ifdef DTT
                //以太网帧报文入列
                xSemaphoreTake(stMse102xDrvCache.xSemEnetQueue[i32Channel], 1000 / portTICK_PERIOD_MS);
                en_queuePush(&stMse102xDrvCache.stEnetQueue[i32Channel], 0, (const u8 *)pData, i32DataLen);
                xSemaphoreGive(stMse102xDrvCache.xSemEnetQueue[i32Channel]);
#else
                pEthHdr = (struct eth_hdr *)pData;
                if(pEthHdr->type == ntohs(ETH_P_HPAV))
                {
                    //homeplug帧入列
                    xSemaphoreTake(stMse102xDrvCache.xSemHpavQueue[i32Channel], 1000 / portTICK_PERIOD_MS);
                    en_queuePush(&stMse102xDrvCache.stHpavQueue[i32Channel], 0, (const u8 *)pData, i32DataLen);
                    xSemaphoreGive(stMse102xDrvCache.xSemHpavQueue[i32Channel]);
                }
                else
                {
                    //其他帧送入lwip协议栈
                    netif_mse102x_input(pNetif, i32DataLen, pData);
                }
#endif
            }
        }
        
        
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    
    EN_SLOGI(TAG, "MSE102X 驱动, PLC通道:%d 主线程退出运行!!!", i32Channel);
    vTaskDelete(NULL);
}








/***************************************************************************************************
* Description                           :     Mse102x驱动 PLC载波芯片复位函数
* Author                                :     Hall
* Creat Date                            :     2024-01-03
* notice                                :     
****************************************************************************************************/
void sMse102xDrvResetDevice(i32 i32Port, i32 i32Pin)
{
    i32  i32TimeCnt = 0;
    
    gpio_bit_set(i32Port, i32Pin);
    i32TimeCnt = 0;
    while((i32TimeCnt++) < 1000000)
    {
    }
    
    gpio_bit_reset(i32Port, i32Pin);
    i32TimeCnt = 0;
    while((i32TimeCnt++) < 1000000)
    {
    }
    
    gpio_bit_set(i32Port, i32Pin);
}








/**********************************************************************************************
* Description       :     Mse102x 接收和发送数据打印函数
* Author            :     Hall
* modified Date     :     2024-01-16
* notice            :     
***********************************************************************************************/
void sMse102xPrint(bool bSendFlag, const u8 *pBuf, u16 u16Len)
{
    char u8HeadStr[48] = { 0 };
    char u8SendRecvStr[3][8] = { "RX", "TX"};
    
    struct eth_hdr *pEthHdr = NULL;
    struct ip6_hdr *pIp6Hdr = NULL;
    
    pEthHdr = (struct eth_hdr *)(pBuf);
    pIp6Hdr = (struct ip6_hdr *)(pBuf + sizeof(struct eth_hdr));
    
    
    if((pEthHdr->type != ntohs(ETH_P_IPV6)) 
    || ((pIp6Hdr->_nexth != IP6_NEXTH_UDP) && (pIp6Hdr->_nexth != IP6_NEXTH_ICMP6)))
    {
        //选择ICMP6和UDP报文打印--主要是为了方便查看NDP协议和SDP协议交互报文
        return;
    }
    
    memset(u8HeadStr, 0, sizeof(u8HeadStr));
    snprintf(u8HeadStr, sizeof(u8HeadStr), "mse102x %s(len:%04d)%s", u8SendRecvStr[bSendFlag], u16Len, bSendFlag ? "--->" : "<---");
    EN_HLOGD(TAG, u8HeadStr, pBuf, u16Len);
}






/*******************************************************************************
 * @FunctionName   :      sMse102xRecv
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  15:44:02
 * @Description    :      
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sMse102xRecv(i32 i32Channel, i32 *pDataLen, u8 **pData)
{
    unMse102xDrvSpiCmd_t unCmd;
    stMse102xDrvSpiFrameCmd_t stFrameCmd;
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (pData ==  NULL) || (pDataLen ==  NULL))
    {
        return (false);
    }
    
    //1:发送CTR命令
    if(sMse102xSendCmd(i32Channel, 0, eMse102xDrvSpiCmdTypeCtr) == false)
    {
        return (false);
    }
    
    
    //2:接收从机回复RTS命令
    memset(&stFrameCmd, 0, sizeof(stFrameCmd));
    if(sMse102xRecvCmd(i32Channel, &stFrameCmd) == false)
    {
        return (false);
    }
    unCmd.u16Cmd = ntohs(stFrameCmd.unCmd.u16Cmd);
    
    
    //3:接收SOF数据
    (*pData)    = stMse102xDrvCache.stRxFrameSof[i32Channel].u8Data;
    (*pDataLen) = unCmd.stCmd.bPara;
    memset(&stMse102xDrvCache.stRxFrameSof[i32Channel], 0, sizeof(stMse102xDrvSpiFrameSof_t));
    return sMse102xRecvSof(i32Channel, &stMse102xDrvCache.stRxFrameSof[i32Channel], unCmd.stCmd.bPara);
}






/*******************************************************************************
 * @FunctionName   :      sMse102xRecvSof
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  15:25:33
 * @Description    :      
 * @Input          :      pBuf        
 * @Input          :      i32DataLen  
 * @Return         :      
*******************************************************************************/
bool sMse102xRecvSof(i32 i32Channel, stMse102xDrvSpiFrameSof_t *pFrame, i32 i32DataLen)
{
    i32  i32FrameLen;
    eMse102xDrvSpiDft_t *pDft = NULL;
    stMse102xDrvDevice_t *pDevice = NULL;
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (pFrame == NULL) || (i32DataLen <= 0) || (i32DataLen > cMse102xDrvSpiSofDataLenMax))
    {
        return (false);
    }
    
    //1:计算数据包总长度
    i32FrameLen = sizeof(stMse102xDrvSpiFrameSof_t) - cMse102xDrvSpiSofDataLenMax + i32DataLen;
    
    
    //2:接收数据
    pDevice = &stMse102xDrvDevMap[i32Channel];
    if(false == sSpiDrvRecv(pDevice->eSpiNum, i32FrameLen, (u8 *)pFrame))
    {
        return (false);
    }
    
    
    //3:字段校验
    pDft = (eMse102xDrvSpiDft_t *)&pFrame->u8Data[i32DataLen];
    if((pFrame->eDelimiter != ntohs(eMse102xDrvSpiDelimiterSof)) || ((*pDft) != ntohs(eMse102xDrvSpiDft)))
    {
        return (false);
    }
    
    
    return (true);
}





/*******************************************************************************
 * @FunctionName   :      sMse102xRecvCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  15:30:43
 * @Description    :      
 * @Input          :      u16Cmd      
 * @Return         :      
*******************************************************************************/
bool sMse102xRecvCmd(i32 i32Channel, stMse102xDrvSpiFrameCmd_t *pFrame)
{
    unMse102xDrvSpiCmd_t unCmd;
    stMse102xDrvDevice_t *pDevice = NULL;
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (pFrame == NULL))
    {
        return (false);
    }
    
    
    //1:接收
    pDevice = &stMse102xDrvDevMap[i32Channel];
    if(sSpiDrvRecv(pDevice->eSpiNum, sizeof(stMse102xDrvSpiFrameCmd_t), (u8 *)pFrame) == false)
    {
        return (false);
    }
    unCmd.u16Cmd = ntohs(pFrame->unCmd.u16Cmd);
    
    
    //2:字段校验
    if((pFrame->eDelimiter != ntohs(eMse102xDrvSpiDelimiterCmd))
    || ((unCmd.stCmd.bType != eMse102xDrvSpiCmdTypeRts) && (unCmd.stCmd.bType != eMse102xDrvSpiCmdTypeCtr)))
    {
        return (false);
    }
    
    return (true);
}






/*******************************************************************************
 * @FunctionName   :      sMse102xSend
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  15:44:00
 * @Description    :      本函数可以被多个线程调用,需要上锁保护，确保发送不重叠
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sMse102xSend(i32 i32Channel, i32 i32DataLen, const u8 *pData)
{
    i32  i32Len;
    bool bRst;
    eMse102xDrvSpiDft_t *pDft = NULL;
    stMse102xDrvSpiFrameCmd_t stFrameCmd;
    stMse102xDrvSpiFrameSof_t *pFrameSof = NULL;
    
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (pData == NULL) || (i32DataLen < 0) || (i32DataLen >= cMse102xDrvSpiSofDataLenMax))
    {
        return (false);
    }
    sMse102xPrint(true, pData, i32DataLen);
    
    
    //1:上锁，主机发送RTS命令
    xSemaphoreTake(stMse102xDrvCache.xSemSend[i32Channel], portMAX_DELAY);
    if(sMse102xSendCmd(i32Channel, i32DataLen, eMse102xDrvSpiCmdTypeRts) == false)
    {
        xSemaphoreGive(stMse102xDrvCache.xSemSend[i32Channel]);
        return (false);
    }
    
    
    //2:等待接收从机回复CTR命令
    memset(&stFrameCmd, 0, sizeof(stFrameCmd));
    if(sMse102xRecvCmd(i32Channel, &stFrameCmd) == false)
    {
        xSemaphoreGive(stMse102xDrvCache.xSemSend[i32Channel]);
        return (false);
    }
    
    
    //3:主机发送SOF数据
    pFrameSof = &stMse102xDrvCache.stTxFrameSof[i32Channel];
    memset(pFrameSof, 0, sizeof(stMse102xDrvSpiFrameSof_t));
    pFrameSof->eDelimiter = htons(eMse102xDrvSpiDelimiterSof);
    memcpy(pFrameSof->u8Data, pData, i32DataLen);
    
    pDft = (eMse102xDrvSpiDft_t *)&pFrameSof->u8Data[i32DataLen];
    (*pDft) = htons(eMse102xDrvSpiDft);
    
    i32Len = sizeof(stMse102xDrvSpiFrameSof_t) - cMse102xDrvSpiSofDataLenMax + i32DataLen;
    bRst = sMse102xSendSof(i32Channel, pFrameSof, i32Len);
    
    //解锁
    xSemaphoreGive(stMse102xDrvCache.xSemSend[i32Channel]);
    
    return (bRst);
}





/*******************************************************************************
 * @FunctionName   :      sMse102xSendSof
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月11日  9:31:55
 * @Description    :      
 * @Input          :      pBuf        
 * @Input          :      i32DataLen  
 * @Return         :      
*******************************************************************************/
bool sMse102xSendSof(i32 i32Channel, stMse102xDrvSpiFrameSof_t *pFrame, i32 i32Len)
{
    stMse102xDrvDevice_t *pDevice = NULL;
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (pFrame <= 0) || (i32Len < 0) || (i32Len > sizeof(stMse102xDrvSpiFrameSof_t)))
    {
        return (false);
    }
    
    //1:发送数据
    pDevice = &stMse102xDrvDevMap[i32Channel];
    return sSpiDrvSend(pDevice->eSpiNum, i32Len, (const u8 *)pFrame);
}






/*******************************************************************************
 * @FunctionName   :      sMse102xSendCmd
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2023年12月9日  15:25:24
 * @Description    :      
 * @Input          :      u16Cmd      
 * @Input          :      u16DataLen  
 * @Return         :      
*******************************************************************************/
bool sMse102xSendCmd(i32 i32Channel, u16 u16Param, eMse102xDrvSpiCmdType_t eCmd)
{
    unMse102xDrvSpiCmd_t unCmd;
    stMse102xDrvSpiFrameCmd_t stFrameCmd;
    
    stMse102xDrvDevice_t *pDevice = NULL;
    
    //0:参数限制
    if((i32Channel >= cMse102xDevNum) || (u16Param > cMse102xDrvSpiSofDataLenMax) || (eCmd >= eMse102xDrvSpiCmdTypeMax))
    {
        return (false);
    }
    
    
    //1:填充数据
    memset(&stFrameCmd, 0, sizeof(stFrameCmd));
    stFrameCmd.eDelimiter  = htons(eMse102xDrvSpiDelimiterCmd);
    unCmd.stCmd.bType = eCmd;
    unCmd.stCmd.bPara = u16Param & 0x0fff;
    stFrameCmd.unCmd.u16Cmd = htons(unCmd.u16Cmd);
    
    
    //2:发送数据
    pDevice = &stMse102xDrvDevMap[i32Channel];
    return sSpiDrvSend(pDevice->eSpiNum, sizeof(stFrameCmd), (const u8 *)&stFrameCmd);
}















