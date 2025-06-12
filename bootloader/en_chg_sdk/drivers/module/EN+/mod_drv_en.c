/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   mo_drv_en.c
* Description                           :   EN+模块电源CAN通讯驱动实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-07-30
* notice                                :   负责实现EN+模块电源CAN协议驱动，即协议文本中所有的收发指令
*                                           基于  <EN 充电模块通讯协议（对外）V2.7.6.docx>  设计
****************************************************************************************************/
#include "mod_drv_en.h"







//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "mo_drv_en";

//---------------------------------------------------------------------------------------------------------


stModDrvEnCache_t *pModDrvEnCache = NULL;





//---------------------------------------------------------------------------------------------------------
bool  sModDrvEnInit(i32 i32Port, i32 i32Baudrate);
void  sModDrvEnRecvTask(void *pParam);
void  sModDrvEnSendTask(void *pParam);
bool  sModDrvEnRecv(void);

//协议中多指令合并的数据交互过程
bool  sModDrvEnConfig(u8 u8Addr);
bool  sModDrvEnReadData(void);
bool  sModDrvEnReadInfo(void);

//配套处理函数
bool  sModDrvEnDataPrint(bool bSendFlag, stCanMsg_t *pFrame);
i32   sModDrvEnCmdSearch(eModDrvEnCmd_t eCmd);
bool  sModDrvEnMsgIdCheck(unModDrvEnMsgId_t unMsgId);
bool  sModDrvEnMsgIdFill(stCanMsg_t *pFrame, u8 u8Addr, eModDrvEnCmd_t eCmd, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnCmdSend(eModDrvEnCmd_t eCmd, stCanMsg_t *pFrame, u16 u16Len);
bool  sModDrvEnSetModExistSts(void);

//命令发送
bool  sModDrvEnTxCmd01(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd02(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd03(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd04(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd05(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd06(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd08(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd09(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd0A(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd0B(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd0C(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd0D(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd13(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16WalkinTime);
bool  sModDrvEnTxCmd14(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32FlashTime);
bool  sModDrvEnTxCmd16(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 u8Group);
bool  sModDrvEnTxCmd19(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSleepValue_t eValue);
bool  sModDrvEnTxCmd1A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnTurnValue_t eValue);
bool  sModDrvEnTxCmd1C(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32Udc, u32 u32Idc);
bool  sModDrvEnTxCmd1F(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnAddrMode_t eMode);
bool  sModDrvEnTxCmd20(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd21(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSmartParaState_t eState);
bool  sModDrvEnTxCmd22(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnCanBroadcast_t eBdValue);
bool  sModDrvEnTxCmd23(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnVoltMode_t eMode);
bool  sModDrvEnTxCmd24(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16UpvOpen, u16 u16UpvMpp, u16 u16IpvMpp);
bool  sModDrvEnTxCmd25(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropU, u8 u8Coef);
bool  sModDrvEnTxCmd26(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropI, u8 u8Coef);
bool  sModDrvEnTxCmd28(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16Uspeed, u16 u16Ispeed);
bool  sModDrvEnTxCmd2A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, bool bAgingMode);
bool  sModDrvEnTxCmd2B(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 *pPassword, u8 u8Len);
bool  sModDrvEnTxCmd30(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd31(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd32(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd33(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd34(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd35(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd36(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd37(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd38(u8 u8Addr, eModDrvEnDevNum_t eDevNum);
bool  sModDrvEnTxCmd39(u8 u8Addr, eModDrvEnDevNum_t eDevNum);


//命令接收处理
void  sModDrvEnRxCmd01(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd02(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd03(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd04(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd05(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd06(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd08(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd09(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd0A(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd0B(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd0C(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd0D(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd13(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd14(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd16(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd19(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd1A(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd1C(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd1F(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd20(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd21(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd22(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd23(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd24(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd25(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd26(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd28(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd2A(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd2B(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd30(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd31(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd32(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd33(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd34(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd35(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd36(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd37(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd38(const stCanMsg_t *pFrame);
void  sModDrvEnRxCmd39(const stCanMsg_t *pFrame);





//---------------------------------------------------------------------------------------------------------

stModDrvEnCmdMap_t stModDrvEnMap[] =
{
    {eModDrvEnCmd01,   "Read:Udc/Idc总",           1000, NULL,   false,      sModDrvEnRxCmd01},
    {eModDrvEnCmd02,   "Read:模块数量",            1000, NULL,   false,      sModDrvEnRxCmd02},
    {eModDrvEnCmd03,   "Read:Udc/Idc",             1000, NULL,   false,      sModDrvEnRxCmd03},
    {eModDrvEnCmd04,   "Read:Rfan,T,Err code",     1000, NULL,   false,      sModDrvEnRxCmd04},
    {eModDrvEnCmd05,   "Read:系列温度",            1000, NULL,   false,      sModDrvEnRxCmd05},
    {eModDrvEnCmd06,   "Read:Uab,Ubc,Uca,State",   1000, NULL,   false,      sModDrvEnRxCmd06},
    {eModDrvEnCmd08,   "Read:运行时间",            1000, NULL,   false,      sModDrvEnRxCmd08},
    {eModDrvEnCmd09,   "Read:LLC/PFC内部数据",     1000, NULL,   false,      sModDrvEnRxCmd09},
    {eModDrvEnCmd0A,   "Read:模块组号",            1000, NULL,   false,      sModDrvEnRxCmd0A},
    {eModDrvEnCmd0B,   "Read:输入电流和输入功率",  1000, NULL,   false,      sModDrvEnRxCmd0B},
    {eModDrvEnCmd0C,   "Read:总电量和本次电量",    1000, NULL,   false,      sModDrvEnRxCmd0C},
    {eModDrvEnCmd0D,   "Read:LLC内部数据",         1000, NULL,   false,      sModDrvEnRxCmd0D},
    {eModDrvEnCmd13,   "Set :Walk in功能",         1000, NULL,   false,      sModDrvEnRxCmd13},
    {eModDrvEnCmd14,   "Ctrl:Green LED Flash",     1000, NULL,   false,      sModDrvEnRxCmd14},
    {eModDrvEnCmd16,   "Set :Group",               1000, NULL,   false,      sModDrvEnRxCmd16},
    {eModDrvEnCmd19,   "Ctrl:模块休眠",            1000, NULL,   false,      sModDrvEnRxCmd19},
    {eModDrvEnCmd1A,   "Ctrl:模块开关机",          1000, NULL,   false,      sModDrvEnRxCmd1A},
    {eModDrvEnCmd1C,   "Set :Udc,Idc",             1000, NULL,   false,      sModDrvEnRxCmd1C},
    {eModDrvEnCmd1F,   "Set :地址模式",            1000, NULL,   false,      sModDrvEnRxCmd1F},
    {eModDrvEnCmd20,   "Ctrl:模块重新分配地址",    1000, NULL,   false,      sModDrvEnRxCmd20},
    {eModDrvEnCmd21,   "Set :智能并机模式",        1000, NULL,   false,      sModDrvEnRxCmd21},
    {eModDrvEnCmd22,   "Set :广播报文使能和禁止",  1000, NULL,   false,      sModDrvEnRxCmd22},
    {eModDrvEnCmd23,   "Set :高低压模式",          1000, NULL,   false,      sModDrvEnRxCmd23},
    {eModDrvEnCmd24,   "Set :MPPT 曲线",           1000, NULL,   false,      sModDrvEnRxCmd24},
    {eModDrvEnCmd25,   "Set :模块间并联均流功能",  1000, NULL,   false,      sModDrvEnRxCmd25},
    {eModDrvEnCmd26,   "Set :模块间串联均压功能",  1000, NULL,   false,      sModDrvEnRxCmd26},
    {eModDrvEnCmd28,   "Set :电压/电流变化率",     1000, NULL,   false,      sModDrvEnRxCmd28},
    {eModDrvEnCmd2A,   "Set :老化模式",            1000, NULL,   false,      sModDrvEnRxCmd2A},
    {eModDrvEnCmd2B,   "校验:管理员密码",          1000, NULL,   false,      sModDrvEnRxCmd2B},
    {eModDrvEnCmd30,   "Read:模块固有信息1",       1000, NULL,   false,      sModDrvEnRxCmd30},
    {eModDrvEnCmd31,   "Read:模块固有信息2",       1000, NULL,   false,      sModDrvEnRxCmd31},
    {eModDrvEnCmd32,   "Read:模块固有信息3",       1000, NULL,   false,      sModDrvEnRxCmd32},
    {eModDrvEnCmd33,   "Read:模块固有信息4",       1000, NULL,   false,      sModDrvEnRxCmd33},
    {eModDrvEnCmd34,   "Read:模块固有信息5",       1000, NULL,   false,      sModDrvEnRxCmd34},
    {eModDrvEnCmd35,   "Read:模块固有信息6",       1000, NULL,   false,      sModDrvEnRxCmd35},
    {eModDrvEnCmd36,   "Read:模块固有信息7",       1000, NULL,   false,      sModDrvEnRxCmd36},
    {eModDrvEnCmd37,   "Read:模块固有信息8",       1000, NULL,   false,      sModDrvEnRxCmd37},
    {eModDrvEnCmd38,   "Read:模块固有信息9",       1000, NULL,   false,      sModDrvEnRxCmd38},
    {eModDrvEnCmd39,   "Read:模块固有信息A",       1000, NULL,   false,      sModDrvEnRxCmd39},
};



//---------------------------------------------------------------------------------------------------------



/***************************************************************************************************
* Description                           :   EN+模块电源驱动 资源初始化
* Author                                :   Hall
* Creat Date                            :   2022-07-29
* notice                                :   
****************************************************************************************************/
bool sModDrvEnInit(i32 i32Port, i32 i32Baudrate)
{
    i32  i;
    
    //初始化底层数据结构
    pModDrvEnCache = (stModDrvEnCache_t *)MALLOC(sizeof(stModDrvEnCache_t));
    memset(pModDrvEnCache, 0, sizeof(stModDrvEnCache_t));
    pModDrvEnCache->i32Port = i32Port;
    pModDrvEnCache->i32Baudrate = i32Baudrate;
    
    
    //初始化map表
    for(i = 0; i < sArraySize(stModDrvEnMap); i++)
    {
        if(stModDrvEnMap[i].i32WaitTime > 0)
        {
            stModDrvEnMap[i].xSemAck = xSemaphoreCreateBinary();
            xSemaphoreGive(stModDrvEnMap[i].xSemAck);
        }
    }
    
    sCanDrvOpen(i32Port, i32Baudrate);
    
    xTaskCreate(sModDrvEnRecvTask, "sModDrvEnRecvTask", (512), NULL, 21, NULL);
    xTaskCreate(sModDrvEnSendTask, "sModDrvEnSendTask", (512), NULL, 22, NULL);
    
    return(true);
}









/***************************************************************************************************
* Description                           :   EN+模块电源驱动 接收处理线程
* Author                                :   Hall
* Creat Date                            :   2022-07-29
* notice                                :   
****************************************************************************************************/
void sModDrvEnRecvTask(void *pParam)
{
    i32 i32TimeoutCnt = 0;
    
    EN_SLOGI(TAG, "任务建立:EN+科技 DC模块电源通讯接收任务");
    while(1)
    {
        i32TimeoutCnt = (sModDrvEnRecv() != true) ? (i32TimeoutCnt + 1) : 0;
        if(i32TimeoutCnt >= 1000)
        {
            i32TimeoutCnt = 0;
            EN_SLOGE(TAG, "错误:DC模块电源通讯接收报文超时!!!");
        }
        
        
        //检查掉线模块并清除其在线标志
        sModDrvEnSetModExistSts();
        
        vTaskDelay(cModDrvEnRecvTaskCycle / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}











/***************************************************************************************************
* Description                           :   EN+模块电源驱动 发送处理线程
* Author                                :   Hall
* Creat Date                            :   2022-07-29
* notice                                :   
****************************************************************************************************/
void sModDrvEnSendTask(void *pParam)
{
    i32  i32Time;
    i32  i32ReadInfoTimeOld = 0;
    
    vTaskDelay(2000 / portTICK_RATE_MS);                                        //等待模块在线状态更新
    EN_SLOGI(TAG, "任务建立:EN+科技 DC模块电源通讯发送任务");
    while(sModDrvEnConfig(cModDrvEnAddrBd) != true)
    {
        EN_SLOGI(TAG, "DC模块电源 工作模式配置失败,等待重试!!!");
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
    EN_SLOGI(TAG, "DC模块电源 工作模式配置成功!!!");
    
    while(1)
    {
        i32Time = sGetTimestamp();
        if((i32Time - i32ReadInfoTimeOld) > 150)
        {
            sModDrvEnReadInfo();
            i32ReadInfoTimeOld = i32Time;
        }
        else
        {
            sModDrvEnReadData();
        }
        
        vTaskDelay(cModDrvEnSendTaskCycle / portTICK_RATE_MS);
    }
    
    vTaskDelete(NULL);
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 数据接收函数
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvEnRecv(void)
{
    i32  i;
    bool bRst;
    
    
    stCanMsg_t stFrame;
    unModDrvEnMsgId_t unMsgId;
    
    memset(&stFrame, 0, sizeof(stCanMsg_t));
    bRst = sCanDrvRecv(pModDrvEnCache->i32Port, &stFrame);
    if(bRst == true)
    {
        unMsgId.u32Value = stFrame.u32Id;
        if(sModDrvEnMsgIdCheck(unMsgId) == true)
        {
            //sModDrvEnDataPrint(false, &stFrame);                                  //调试时打开看看
            for(i = 0; i < sArraySize(stModDrvEnMap); i++)
            {
                if(stModDrvEnMap[i].eCmd == unMsgId.stValue.eCmdNum)
                {
                    if(stModDrvEnMap[i].pCmdPro != NULL)
                    {
                        stModDrvEnMap[i].pCmdPro(&stFrame);
                        break;
                    }
                }
            }
        }
    }
    
    return(bRst);
}








/***************************************************************************************************
* Description                           :   模块电源驱动->工作模式配置
* Author                                :   Hall
* Creat Date                            :   2022-08-04
* notice                                :   EN+模块电源 上电后应该完成这几项设置 否则不会开启输出并
*                                           用开机命令的 state3 反馈尚缺少的设置项
****************************************************************************************************/
bool sModDrvEnConfig(u8 u8Addr)
{
    //固定使用拨码开关设定的地址
    return sModDrvEnTxCmd1F(u8Addr, eModDrvEnDevNum0A, eModDrvEnAddrModeSwitchSet);
}






/***************************************************************************************************
* Description                           :   模块电源驱动->读取模块运行实时数据
* Author                                :   Hall
* Creat Date                            :   2022-08-04
* notice                                :   
****************************************************************************************************/
bool sModDrvEnReadData(void)
{
    u8   i;
    bool bRst = true;
    
    for(i = 0; i < cModDrvEnAddrMax; i++)
    {
        if(pModDrvEnCache->stData[i].bModExistSts == true)
        {
            bRst = bRst & sModDrvEnTxCmd03(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd04(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd05(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd06(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd08(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd09(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd0B(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd0C(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd0D(i + 1, eModDrvEnDevNum0A);
        }
    }
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   模块电源驱动->读取模块固有信息
* Author                                :   Hall
* Creat Date                            :   2022-08-04
* notice                                :   
****************************************************************************************************/
bool sModDrvEnReadInfo(void)
{
    u8   i;
    bool bRst = true;
    
    for(i = 0; i < cModDrvEnAddrMax; i++)
    {
        if(pModDrvEnCache->stData[i].bModExistSts == true)
        {
            bRst = bRst & sModDrvEnTxCmd30(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd31(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd32(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd33(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd34(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd35(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd36(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd37(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd38(i + 1, eModDrvEnDevNum0A);
            bRst = bRst & sModDrvEnTxCmd39(i + 1, eModDrvEnDevNum0A);
        }
    }
    
    return(bRst);
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 报文打印函数
* Author                                :   Hall
* Creat Date                            :   2023-08-21
* notice                                :   
****************************************************************************************************/
bool sModDrvEnDataPrint(bool bSendFlag, stCanMsg_t *pFrame)
{
    unModDrvEnMsgId_t unMsgId;
    
    char u8Head[64];
    char u8String[2][16] = {"接收", "发送"};
    
    unMsgId.u32Value = pFrame->u32Id;
    memset(u8Head, 0, sizeof(u8Head));
    snprintf(u8Head, sizeof(u8Head), "模块通讯,%s(MsgId:0x%08X, CMD:0x%02X)--->", u8String[bSendFlag], pFrame->u32Id & 0x1FFFFFFF, unMsgId.stValue.eCmdNum);
    EN_HLOGD(TAG, u8Head, pFrame->u8Data, pFrame->u8Len);
    
    return(true);
}











/***************************************************************************************************
* Description                           :   从 stModDrvEnMap[] 中寻找 序号
* Author                                :   Hall
* Creat Date                            :   2022-08-02
* notice                                :   读取输出电压和总电流
****************************************************************************************************/
i32 sModDrvEnCmdSearch(eModDrvEnCmd_t eCmd)
{
    i32 i;
    i32 i32Index;
    
    i32Index = (-1);
    for(i = 0; i< sArraySize(stModDrvEnMap); i++)
    {
        if(eCmd == stModDrvEnMap[i].eCmd)
        {
            i32Index = i;
            break;
        }
    }
    
    return(i32Index);
}








/***************************************************************************************************
* Description                           :   EN+模块电源驱动 接收报文的 MsgId 检查
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   如果是不需要使用 map 去处理的报文 就返回 false
****************************************************************************************************/
bool sModDrvEnMsgIdCheck(unModDrvEnMsgId_t unMsgId)
{
    bool bRst;
    
    if((unMsgId.stValue.bAddSrc <  cModDrvEnAddrMin)
    || (unMsgId.stValue.bAddSrc >  cModDrvEnAddrMax)                            //源地址超限
    || (unMsgId.stValue.ebit29 !=  eModDrvEnMsgIdBit29DataFrame)                //错误消息帧
    || (unMsgId.stValue.ebit30 !=  eModDrvEnMsgIdBit30Data)                     //远程帧
    || (unMsgId.stValue.ebit31 !=  eModDrvEnMsgIdBit31Extended))                //标准帧------全部剔除
    {
        bRst = false;
    }
    else if((unMsgId.stValue.eDevNum  == eModDrvEnDevNum01)
         && (unMsgId.stValue.bAddDst  == cModDrvEnAddrBd)
         && ((unMsgId.stValue.eCmdNum == eModDrvEnCmd03) || (unMsgId.stValue.eCmdNum == eModDrvEnCmd07)))//这俩广播报文不需要处理 返回 false
    {
        //上位机利用 EN+模块电源 的CAN广播报文来标记地址在线的模块
        pModDrvEnCache->stData[unMsgId.stValue.bAddSrc - 1].bModExistSts = true;
        pModDrvEnCache->stData[unMsgId.stValue.bAddSrc - 1].u32RxTimestamp = sGetTimestamp();
        
        bRst = false;
    }
    else
    {
        bRst = true;
    }
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 MsgId 填充
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   
****************************************************************************************************/
bool sModDrvEnMsgIdFill(stCanMsg_t *pFrame, u8 u8Addr, eModDrvEnCmd_t eCmd, eModDrvEnDevNum_t eDevNum)
{
    unModDrvEnMsgId_t unMsgId;
    
    memset(&unMsgId, 0, sizeof(unMsgId));
    memset(pFrame, 0, sizeof(stCanMsg_t));
    unMsgId.stValue.bAddSrc             = cModDrvEnAddrMaster;
    unMsgId.stValue.bAddDst             = u8Addr;
    unMsgId.stValue.eCmdNum             = eCmd;
    unMsgId.stValue.eDevNum             = eDevNum;
    unMsgId.stValue.eErrCode            = eModDrvEnErrCodeNone;
    unMsgId.stValue.ebit29              = eModDrvEnMsgIdBit29DataFrame;
    unMsgId.stValue.ebit30              = eModDrvEnMsgIdBit30Data;
    unMsgId.stValue.ebit31              = eModDrvEnMsgIdBit31Extended;
    
    pFrame->u32Id                       = unMsgId.u32Value;
    
    return(true);
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动  CAN帧发送
* Author                                :   Hall
* Creat Date                            :   2022-08-05
* notice                                :   
****************************************************************************************************/
bool sModDrvEnCmdSend(eModDrvEnCmd_t eCmd, stCanMsg_t *pFrame, u16 u16Len)
{
    bool bRst;
    i32  i32Index;
    i32  i32WriteLen;
    
    //1:搜索序号
    i32Index = sModDrvEnCmdSearch(eCmd);
    if(i32Index < 0)
    {
        return(false);
    }
    
    //2:上锁 并清除 bProcRst
    if(stModDrvEnMap[i32Index].i32WaitTime > 0)
    {
        xSemaphoreTake(stModDrvEnMap[i32Index].xSemAck, 0);
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //3:发送
    //sModDrvEnDataPrint(true, pFrame);                                              //调试时打开看看
    bRst = sCanDrvSend(pModDrvEnCache->i32Port, pFrame);
    
    //4:等待解锁----接收到回复报文 解析OK时解锁
    if(bRst == true)
    {
        if(stModDrvEnMap[i32Index].i32WaitTime > 0)
        {
            bRst = xSemaphoreTake(stModDrvEnMap[i32Index].xSemAck, stModDrvEnMap[i32Index].i32WaitTime / portTICK_PERIOD_MS);
            if(bRst == true)
            {
                bRst = stModDrvEnMap[i32Index].bProcRst;
            }
        }
    }
    
    return(bRst);
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动指令 清除离线模块的 存在状态
* Author                                :   Hall
* Creat Date                            :   2023-08-18
* notice                                :   
****************************************************************************************************/
bool sModDrvEnSetModExistSts(void)
{
    u16 i;
    i32 i32Time;
    
    static i32 i32ModExistStsClearTime = 0;
    
    i32Time = sGetTimestamp();
    if((i32Time - i32ModExistStsClearTime) >= 1)
    {
        i32ModExistStsClearTime = i32Time;
        for(i = 0; i < cModDrvEnAddrMax; i++)
        {
            //暂定:3秒收不到标记报文 就清除
            if((i32Time - pModDrvEnCache->stData[i].u32RxTimestamp) > cModDrvEnExistStsClearTime)
            {
                pModDrvEnCache->stData[i].bModExistSts = false;
            }
        }
    }
    
    return(true);
}








/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD01指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-02
* notice                                :   读取输出电压和总电流
****************************************************************************************************/
bool sModDrvEnTxCmd01(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr != cModDrvEnAddrBd))
    {
        //本命令必须为广播地址 因为只有主模块才会回复本报文
        return(false);
    }
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd01, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd01, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD02指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-02
* notice                                :   读取模块数量
****************************************************************************************************/
bool sModDrvEnTxCmd02(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr != cModDrvEnAddrBd))
    {
        //本命令必须为广播地址 因为只有主模块才会回复本报文
        return(false);
    }
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd02, eDevNum);
    stFrame.u8Len = 8;
    
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd02, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD03指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取单个模块的输出电压和电流
****************************************************************************************************/
bool sModDrvEnTxCmd03(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd03, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd03, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD04指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取风扇转速、环境温度、错误码
****************************************************************************************************/
bool sModDrvEnTxCmd04(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd04, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd04, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD05指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取系列温度
****************************************************************************************************/
bool sModDrvEnTxCmd05(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd05, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd05, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD06指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取 Uab,Ubc,Uca,State1 State2
****************************************************************************************************/
bool sModDrvEnTxCmd06(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd06, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd06, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD08指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取 运行时间
****************************************************************************************************/
bool sModDrvEnTxCmd08(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd08, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd08, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD09指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   读取 PFC和LLC数据
****************************************************************************************************/
bool sModDrvEnTxCmd09(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd09, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd09, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0A指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   读取 模块组号
****************************************************************************************************/
bool sModDrvEnTxCmd0A(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd0A, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd0A, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0B指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   读取：输入电流、输入功率
****************************************************************************************************/
bool sModDrvEnTxCmd0B(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd0B, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd0B, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0C指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   读取：电量
****************************************************************************************************/
bool sModDrvEnTxCmd0C(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd0C, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd0C, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0D指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   读取：LLC数据
****************************************************************************************************/
bool sModDrvEnTxCmd0D(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd0D, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd0D, &stFrame, sizeof(stCanMsg_t));
}









/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD13指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置 walk in 功能
****************************************************************************************************/
bool sModDrvEnTxCmd13(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16WalkinTime)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd13, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = (u16WalkinTime >> 8) & 0xff;
    stFrame.u8Data[1] = (u16WalkinTime >> 0) & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd13, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD14指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 绿色LED等闪烁
****************************************************************************************************/
bool sModDrvEnTxCmd14(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32FlashTime)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd14, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = (u32FlashTime >> 24) & 0xff;
    stFrame.u8Data[1] = (u32FlashTime >> 16) & 0xff;
    stFrame.u8Data[2] = (u32FlashTime >>  8) & 0xff;
    stFrame.u8Data[3] = (u32FlashTime >>  0) & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd14, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD16指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置模块组号
****************************************************************************************************/
bool sModDrvEnTxCmd16(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 u8Group)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd16, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = u8Group & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd16, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD19指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 进入或退出休眠
****************************************************************************************************/
bool sModDrvEnTxCmd19(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSleepValue_t eValue)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd19, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eValue & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd19, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1A指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块开关机
****************************************************************************************************/
bool sModDrvEnTxCmd1A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnTurnValue_t eValue)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd1A, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eValue & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd1A, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1C指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 输出电压和电流
*                                           u32Udc：单位mV            u32Idc：单位mA
****************************************************************************************************/
bool sModDrvEnTxCmd1C(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u32 u32Udc, u32 u32Idc)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd1C, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = (u32Udc >> 24) & 0xff;
    stFrame.u8Data[1] = (u32Udc >> 16) & 0xff;
    stFrame.u8Data[2] = (u32Udc >>  8) & 0xff;
    stFrame.u8Data[3] = (u32Udc >>  0) & 0xff;
    stFrame.u8Data[4] = (u32Idc >> 24) & 0xff;
    stFrame.u8Data[5] = (u32Idc >> 16) & 0xff;
    stFrame.u8Data[6] = (u32Idc >>  8) & 0xff;
    stFrame.u8Data[7] = (u32Idc >>  0) & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd1C, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1F指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块的地址模式
****************************************************************************************************/
bool sModDrvEnTxCmd1F(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnAddrMode_t eMode)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd1F, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eMode & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd1F, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD20指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 所有模块重新分配地址
****************************************************************************************************/
bool sModDrvEnTxCmd20(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr != cModDrvEnAddrBd))
    {
        //本命令必须为广播地址 否则可能会造成CAN总线上地址冲突
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd20, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd20, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD21指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 智能并机模式
****************************************************************************************************
* Modify Date                           :   2024-03-06
* notice                                :   当前的模块电源程序已经废弃此指令
****************************************************************************************************/
bool sModDrvEnTxCmd21(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnSmartParaState_t eState)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd21, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eState & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd21, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD22指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 CAN广播报文使能禁止
****************************************************************************************************/
bool sModDrvEnTxCmd22(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnCanBroadcast_t eBdValue)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd22, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eBdValue & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd22, &stFrame, sizeof(stCanMsg_t));
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD23指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块的高低压模式
****************************************************************************************************/
bool sModDrvEnTxCmd23(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnVoltMode_t eMode)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd23, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eMode & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd23, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD24指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 MPPT曲线
****************************************************************************************************/
bool sModDrvEnTxCmd24(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16UpvOpen, u16 u16UpvMpp, u16 u16IpvMpp)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd24, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[2] = (u16UpvOpen >> 8) & 0xff;
    stFrame.u8Data[3] = (u16UpvOpen >> 0) & 0xff;
    stFrame.u8Data[4] = (u16UpvMpp >> 8) & 0xff;
    stFrame.u8Data[5] = (u16UpvMpp >> 0) & 0xff;
    stFrame.u8Data[6] = (u16IpvMpp >> 8) & 0xff;
    stFrame.u8Data[7] = (u16IpvMpp >> 0) & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd24, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD25指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置：模块间并联均流功能
****************************************************************************************************/
bool sModDrvEnTxCmd25(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropU, u8 u8Coef)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd25, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eMode & 0xff;
    stFrame.u8Data[1] = u8DropU & 0xff;
    stFrame.u8Data[2] = u8Coef & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd25, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD26指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   设置：模块间串联均压功能（要求地址相邻，如1-2,3-4,...）
****************************************************************************************************/
bool sModDrvEnTxCmd26(u8 u8Addr, eModDrvEnDevNum_t eDevNum, eModDrvEnFunMod_t eMode, u8 u8DropI, u8 u8Coef)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd26, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = eMode & 0xff;
    stFrame.u8Data[1] = u8DropI & 0xff;
    stFrame.u8Data[2] = u8Coef & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd26, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD28指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   设置：电压/电流变化率
****************************************************************************************************/
bool sModDrvEnTxCmd28(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u16 u16Uspeed, u16 u16Ispeed)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd28, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = (u16Uspeed >> 8) & 0xff;
    stFrame.u8Data[1] = (u16Uspeed >> 0) & 0xff;
    stFrame.u8Data[2] = (u16Ispeed >> 8) & 0xff;
    stFrame.u8Data[3] = (u16Ispeed >> 0) & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd28, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD2A指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   设置：老化模式（上电输出）
****************************************************************************************************/
bool sModDrvEnTxCmd2A(u8 u8Addr, eModDrvEnDevNum_t eDevNum, bool bAgingMode)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd2A, eDevNum);
    stFrame.u8Len = 8;
    stFrame.u8Data[0] = bAgingMode & 0xff;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd2A, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD2B指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   校验：管理员密码
****************************************************************************************************/
bool sModDrvEnTxCmd2B(u8 u8Addr, eModDrvEnDevNum_t eDevNum, u8 *pPassword, u8 u8Len)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        //本命令无需向上层返回数据 可以广播设置
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd2B, eDevNum);
    stFrame.u8Len = 8;
    if(u8Len <= 3)
    {
        memcpy(&stFrame.u8Data[1], pPassword, u8Len);
    }
    else
    {
        return(false);
    }
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd2B, &stFrame, sizeof(stCanMsg_t));
}








/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD30指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息1----软件物料号码、软件匹配码、机型
****************************************************************************************************/
bool sModDrvEnTxCmd30(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd30, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd30, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD31指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息2----模块特征字
****************************************************************************************************/
bool sModDrvEnTxCmd31(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd31, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd31, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD32指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息3----序列号高8位
****************************************************************************************************/
bool sModDrvEnTxCmd32(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd32, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd32, &stFrame, sizeof(stCanMsg_t));
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD33指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息4----序列号低8位
****************************************************************************************************/
bool sModDrvEnTxCmd33(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd33, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd33, &stFrame, sizeof(stCanMsg_t));
}








/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD34指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息5----model name高8位
****************************************************************************************************/
bool sModDrvEnTxCmd34(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd34, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd34, &stFrame, sizeof(stCanMsg_t));
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD35指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息6----model name低8位
****************************************************************************************************/
bool sModDrvEnTxCmd35(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd35, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd35, &stFrame, sizeof(stCanMsg_t));
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD36指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息7----制造商ID高8位
****************************************************************************************************/
bool sModDrvEnTxCmd36(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd36, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd36, &stFrame, sizeof(stCanMsg_t));
}








/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD37指令 发送
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息8----制造商ID低8位
****************************************************************************************************/
bool sModDrvEnTxCmd37(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd37, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd37, &stFrame, sizeof(stCanMsg_t));
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD38指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   模块固有信息9----软件版本号高8个字节
****************************************************************************************************/
bool sModDrvEnTxCmd38(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd38, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd38, &stFrame, sizeof(stCanMsg_t));
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD39指令 发送
* Author                                :   Hall
* Creat Date                            :   2023-08-16
* notice                                :   模块固有信息9----软件版本号低8个字节
****************************************************************************************************/
bool sModDrvEnTxCmd39(u8 u8Addr, eModDrvEnDevNum_t eDevNum)
{
    stCanMsg_t stFrame;
    
    if((u8Addr < cModDrvEnAddrMin) || (u8Addr > cModDrvEnAddrBd))
    {
        return(false);
    }
    
    
    //1:填充
    sModDrvEnMsgIdFill(&stFrame, u8Addr, eModDrvEnCmd39, eDevNum);
    stFrame.u8Len = 8;
    
    //2:发送
    return sModDrvEnCmdSend(eModDrvEnCmd39, &stFrame, sizeof(stCanMsg_t));
}















/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD01指令 接收处理
* Author                                :   Hall
* Creat Date                            :   2022-08-02
* notice                                :   输出电压和总电流
****************************************************************************************************/
void sModDrvEnRxCmd01(const stCanMsg_t *pFrame)
{
    u8   i;
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    unIeee754_t unUdc, unIdc;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd01)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    unUdc.u8Buf[3] = pFrame->u8Data[0];
    unUdc.u8Buf[2] = pFrame->u8Data[1];
    unUdc.u8Buf[1] = pFrame->u8Data[2];
    unUdc.u8Buf[0] = pFrame->u8Data[3];
    
    unIdc.u8Buf[3] = pFrame->u8Data[4];
    unIdc.u8Buf[2] = pFrame->u8Data[5];
    unIdc.u8Buf[1] = pFrame->u8Data[6];
    unIdc.u8Buf[0] = pFrame->u8Data[7];
    for(i = 0; i < cModDrvEnAddrBd; i++)
    {
        //CMD01/CMD02 命令只有主模块回复 但对所有模块是一样的
        //因此要将回复数据赋值给所有模块
        pModDrvEnCache->stData[i].f32UdcTotal = unUdc.f32Value;
        pModDrvEnCache->stData[i].f32IdcTotal = unIdc.f32Value;
    }
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD02指令 接收处理
* Author                                :   Hall
* Creat Date                            :   2022-08-02
* notice                                :   模块数量
****************************************************************************************************/
void sModDrvEnRxCmd02(const stCanMsg_t *pFrame)
{
    u8   i;
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd02)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    for(i = 0; i < cModDrvEnAddrBd; i++)
    {
        //CMD01/CMD02 命令只有主模块回复 但对所有模块是一样的
        //因此要将回复数据赋值给所有模块
        pModDrvEnCache->stData[i].u8ModNum = pFrame->u8Data[2];
    }
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD03指令 接收处理
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   单个模块的输出电压和电流
****************************************************************************************************/
void sModDrvEnRxCmd03(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    unIeee754_t unUdc, unIdc;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd03)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    unUdc.u8Buf[3] = pFrame->u8Data[0];
    unUdc.u8Buf[2] = pFrame->u8Data[1];
    unUdc.u8Buf[1] = pFrame->u8Data[2];
    unUdc.u8Buf[0] = pFrame->u8Data[3];
    pModDrvEnCache->stData[u8AddrSrc].f32Udc = unUdc.f32Value;
    
    unIdc.u8Buf[3] = pFrame->u8Data[4];
    unIdc.u8Buf[2] = pFrame->u8Data[5];
    unIdc.u8Buf[1] = pFrame->u8Data[6];
    unIdc.u8Buf[0] = pFrame->u8Data[7];
    pModDrvEnCache->stData[u8AddrSrc].f32Idc = unIdc.f32Value;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD04指令 接收处理
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   风扇转速、环境温度、错误码
****************************************************************************************************/
void sModDrvEnRxCmd04(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd04)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    pModDrvEnCache->stData[u8AddrSrc].u8FanSpeed           = pFrame->u8Data[0];
    pModDrvEnCache->stData[u8AddrSrc].i8Tenv               = pFrame->u8Data[1];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[0] = pFrame->u8Data[7];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[1] = pFrame->u8Data[6];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[2] = pFrame->u8Data[5];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[3] = pFrame->u8Data[4];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[4] = pFrame->u8Data[3];
    pModDrvEnCache->stData[u8AddrSrc].unErrCode.u8Value[5] = pFrame->u8Data[2];
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD05指令 接收处理
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   系列温度
****************************************************************************************************/
void sModDrvEnRxCmd05(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd05)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    pModDrvEnCache->stData[u8AddrSrc].i8Tmos1  = pFrame->u8Data[0];
    pModDrvEnCache->stData[u8AddrSrc].i8Tmos2  = pFrame->u8Data[1];
    pModDrvEnCache->stData[u8AddrSrc].i8Tdiod1 = pFrame->u8Data[2];
    pModDrvEnCache->stData[u8AddrSrc].i8Tdiod2 = pFrame->u8Data[3];
    pModDrvEnCache->stData[u8AddrSrc].i8Ttsf   = pFrame->u8Data[4];
    pModDrvEnCache->stData[u8AddrSrc].i8Tcpu   = pFrame->u8Data[5];
    pModDrvEnCache->stData[u8AddrSrc].i8Trad1  = pFrame->u8Data[6];
    pModDrvEnCache->stData[u8AddrSrc].i8Trad2  = pFrame->u8Data[7];
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD06指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   Uab,Ubc,Uca,State
****************************************************************************************************/
void sModDrvEnRxCmd06(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd06)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u16Temp = (pFrame->u8Data[0] & 0xff);
    u16Temp = (pFrame->u8Data[1] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Uab = u16Temp;
    
    u16Temp = (pFrame->u8Data[2] & 0xff);
    u16Temp = (pFrame->u8Data[3] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Ubc = u16Temp;
    
    u16Temp = (pFrame->u8Data[4] & 0xff);
    u16Temp = (pFrame->u8Data[5] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Uca = u16Temp;
    
    pModDrvEnCache->stData[u8AddrSrc].eState1 = pFrame->u8Data[6];
    pModDrvEnCache->stData[u8AddrSrc].eState2 = pFrame->u8Data[7];
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD08指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   运行时间
****************************************************************************************************/
void sModDrvEnRxCmd08(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u32  u32Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd08)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u32Temp = (pFrame->u8Data[0] & 0xff);
    u32Temp = (pFrame->u8Data[1] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[2] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[3] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u32Htotal = u32Temp;
    
    u32Temp = (pFrame->u8Data[4] & 0xff);
    u32Temp = (pFrame->u8Data[5] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[6] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[7] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u32Hthistime = u32Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD09指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   PFC和LLC数据
****************************************************************************************************/
void sModDrvEnRxCmd09(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd09)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u16Temp = (pFrame->u8Data[0] & 0xff);
    u16Temp = (pFrame->u8Data[1] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Fq = u16Temp;
    
    u16Temp = (pFrame->u8Data[2] & 0xff);
    u16Temp = (pFrame->u8Data[3] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Iq2 = u16Temp;
    
    u16Temp = (pFrame->u8Data[4] & 0xff);
    u16Temp = (pFrame->u8Data[5] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Fac = u16Temp;
    
    u16Temp = (pFrame->u8Data[6] & 0xff);
    u16Temp = (pFrame->u8Data[7] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Ubus = u16Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0A指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   读取：模块组号 应答
****************************************************************************************************/
void  sModDrvEnRxCmd0A(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd0A)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    pModDrvEnCache->stData[u8AddrSrc].u8Group = (pFrame->u8Data[0] & 0xff);
    
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0B指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   读取：输入电流、输入功率 应答
****************************************************************************************************/
void  sModDrvEnRxCmd0B(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd0B)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u16Temp = (pFrame->u8Data[0] & 0xff);
    u16Temp = (pFrame->u8Data[1] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Ia = u16Temp;
    
    u16Temp = (pFrame->u8Data[2] & 0xff);
    u16Temp = (pFrame->u8Data[3] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Ib = u16Temp;
    
    u16Temp = (pFrame->u8Data[4] & 0xff);
    u16Temp = (pFrame->u8Data[5] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Ic = u16Temp;
    
    u16Temp = (pFrame->u8Data[6] & 0xff);
    u16Temp = (pFrame->u8Data[7] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Pac = u16Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0C指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   读取：电量 应答
****************************************************************************************************/
void  sModDrvEnRxCmd0C(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u32  u32Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd0C)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u32Temp = (pFrame->u8Data[0] & 0xff);
    u32Temp = (pFrame->u8Data[1] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[2] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[3] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u32Etotal = u32Temp;
    
    u32Temp = (pFrame->u8Data[4] & 0xff);
    u32Temp = (pFrame->u8Data[5] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[6] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[7] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u32Ethistime = u32Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD0D指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   读取：LLC数据 应答
****************************************************************************************************/
void  sModDrvEnRxCmd0D(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd0D)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u16Temp = (pFrame->u8Data[0] & 0xff);
    u16Temp = (pFrame->u8Data[1] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16IrA = u16Temp;
    
    u16Temp = (pFrame->u8Data[2] & 0xff);
    u16Temp = (pFrame->u8Data[3] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16IrB = u16Temp;
    
    u16Temp = (pFrame->u8Data[6] & 0xff);
    u16Temp = (pFrame->u8Data[7] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16Pdc = u16Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}









/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD13指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置walk in 功能 应答
****************************************************************************************************/
void sModDrvEnRxCmd13(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd13)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD14指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 绿色LED等闪烁 应答
****************************************************************************************************/
void sModDrvEnRxCmd14(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd14)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD16指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置模块组号 应答
****************************************************************************************************/
void sModDrvEnRxCmd16(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd16)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD19指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 进入或退出休眠 应答
****************************************************************************************************/
void sModDrvEnRxCmd19(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd19)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1A指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块开关机 应答
****************************************************************************************************/
void sModDrvEnRxCmd1A(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd1A)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u16Temp = (pFrame->u8Data[6] & 0xff);
    u16Temp = (pFrame->u8Data[7] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].unState3.u16Value = u16Temp;
    
    //处理结果赋值
    if((u16Temp == 0)
    && (unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone))
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1C指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 输出电压和电流 应答
****************************************************************************************************/
void sModDrvEnRxCmd1C(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd1C)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}





/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD1F指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块的地址模式 应答
****************************************************************************************************/
void sModDrvEnRxCmd1F(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd1F)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD20指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 所有模块重新分配地址 应答
****************************************************************************************************/
void sModDrvEnRxCmd20(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd20)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD21指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 智能并机模式 应答
****************************************************************************************************/
void sModDrvEnRxCmd21(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd21)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD22指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 CAN广播报文使能禁止 应答
****************************************************************************************************/
void sModDrvEnRxCmd22(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd22)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD23指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 模块的高低压模式 应答
****************************************************************************************************/
void sModDrvEnRxCmd23(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd23)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD24指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   控制 MPPT曲线 应答
****************************************************************************************************/
void sModDrvEnRxCmd24(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd24)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD25指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   设置：模块间并联均流功能 应答
****************************************************************************************************/
void sModDrvEnRxCmd25(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd25)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD26指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   设置：模块间串联均压功能（要求地址相邻，如1-2,3-4,...）应答
****************************************************************************************************/
void sModDrvEnRxCmd26(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd26)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD28指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   设置：电压/电流变化率 应答
****************************************************************************************************/
void sModDrvEnRxCmd28(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd28)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD2A指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   设置：电压/电流变化率 应答
****************************************************************************************************/
void sModDrvEnRxCmd2A(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd2A)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD2B指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   设置：电压/电流变化率 应答
****************************************************************************************************/
void sModDrvEnRxCmd2B(const stCanMsg_t *pFrame)
{
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd2B)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    
    //处理结果赋值
    if(unMsgId.stValue.eErrCode == eModDrvEnErrCodeNone)
    {
        stModDrvEnMap[i32Index].bProcRst = true;
    }
    else
    {
        stModDrvEnMap[i32Index].bProcRst = false;
    }
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD30指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息1----软件物料、软件匹配码、机型
****************************************************************************************************/
void sModDrvEnRxCmd30(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u16  u16Temp;
    u32  u32Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd30)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u32Temp = (pFrame->u8Data[0] & 0xff);
    u32Temp = (pFrame->u8Data[1] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[2] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[3] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u32MaterialNum = u32Temp;
    
    u16Temp = (pFrame->u8Data[4] & 0xff);
    u16Temp = (pFrame->u8Data[5] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16MatchCode = u16Temp;
    
    u16Temp = (pFrame->u8Data[6] & 0xff);
    u16Temp = (pFrame->u8Data[7] & 0xff) + (u16Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].u16ModelType = u16Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD31指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息2----模块特征字
****************************************************************************************************/
void sModDrvEnRxCmd31(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    u32  u32Temp;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd31)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    u32Temp = (pFrame->u8Data[0] & 0xff);
    u32Temp = (pFrame->u8Data[1] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[2] & 0xff) + (u32Temp << 8);
    u32Temp = (pFrame->u8Data[3] & 0xff) + (u32Temp << 8);
    pModDrvEnCache->stData[u8AddrSrc].unFeatureWord.u32Value = u32Temp;
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD32指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息3----序列号高8位
****************************************************************************************************/
void sModDrvEnRxCmd32(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd32)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8SerialNum, pFrame->u8Data, cModDrvEnSerialNumLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD33指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息4----序列号低8位
****************************************************************************************************/
void sModDrvEnRxCmd33(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd33)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8SerialNum + 8, pFrame->u8Data, cModDrvEnSerialNumLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD34指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息5----model name高8位
****************************************************************************************************/
void sModDrvEnRxCmd34(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd34)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8ModelName, pFrame->u8Data, cModDrvEnModelNameLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD35指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息6----model name低8位
****************************************************************************************************/
void sModDrvEnRxCmd35(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd35)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8ModelName + 8, pFrame->u8Data, cModDrvEnModelNameLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}







/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD36指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息7----制造商ID高8位
****************************************************************************************************/
void sModDrvEnRxCmd36(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd36)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8ManufacturerId, pFrame->u8Data, cModDrvEnManufacturerIdLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD37指令 接收
* Author                                :   Hall
* Creat Date                            :   2022-08-03
* notice                                :   模块固有信息8----制造商ID低8位
****************************************************************************************************/
void sModDrvEnRxCmd37(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd37)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8ManufacturerId + 8, pFrame->u8Data, cModDrvEnManufacturerIdLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD38指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   模块固有信息9----软件版本号高8位
****************************************************************************************************/
void sModDrvEnRxCmd38(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd38)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8SwVer, pFrame->u8Data, cModDrvEnSwVerLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}






/***************************************************************************************************
* Description                           :   EN+模块电源驱动 CMD39指令 接收
* Author                                :   Hall
* Creat Date                            :   2023-08-17
* notice                                :   模块固有信息a----软件版本号低8位
****************************************************************************************************/
void sModDrvEnRxCmd39(const stCanMsg_t *pFrame)
{
    u8   u8AddrSrc;
    i32  i32Index;
    unModDrvEnMsgId_t unMsgId;
    
    if((i32Index = sModDrvEnCmdSearch(eModDrvEnCmd39)) < 0)
    {
        return;
    }
    
    unMsgId.u32Value = pFrame->u32Id;
    if((u8AddrSrc = unMsgId.stValue.bAddSrc - 1) >= cModDrvEnAddrMax)
    {
        //模块电源地址转换成的序号 应该在 0 ~ (cModDrvEnAddrMax - 1) 之间
        return;
    }
    
    memcpy(pModDrvEnCache->stData[u8AddrSrc].u8SwVer + 8, pFrame->u8Data, cModDrvEnSwVerLen / 2);
    
    //处理结果赋值
    stModDrvEnMap[i32Index].bProcRst = true;
    
    //解锁
    if(stModDrvEnMap[i32Index].xSemAck != NULL)
    {
        xSemaphoreGive(stModDrvEnMap[i32Index].xSemAck);
    }
}





