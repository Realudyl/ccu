/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   cooling_app.c
* Description                           :   液冷机通讯 用户程序实现
* Version                               :   V1.0.0
* Author                                :   haisheng.dang@en-plus.com.cn
* Creat Date                            :   2024-06-18
* notice                                :   具体型号为:英特尼迪 液冷机
*                                           使用modbus-rtu协议
*示例报文-------ety上位机报文
1:读取液冷机故障数据
Tx:30 01 00 00 00 10 39 E7
Rx:30 01 02 00 40 C5 C8


2:读取系统流量
Tx:30 03 00 00 00 01 80 2B
Rx:30 03 02 00 00 C5 80


3:读取系统压力
Tx:30 03 00 02 00 01 21 EB
Rx:30 03 02 00 00 C5 80


4:读取回液温度和供液温度
Tx:30 03 00 05 00 02 D0 2B
Rx:30 03 04 03 03 03 1A AB 8F


5:读取循环泵转速和风机转速
Tx:30 03 00 0B 00 02 B1 E8
Rx:30 03 04 00 00 00 00 DA F0


6:读取当前运行模式
Tx:30 03 17 03 00 01 75 9F
Rx:30 03 02 00 00 C5 80


7:读取当前开关机状态
Tx:30 03 15 01 00 01 D5 E7
Rx:30 03 02 00 00 C5 80


****************************************************************************************************/
#include "cooling_app.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "cooling_app";









stCoolingAppCache_t *pCoolingAppCache = NULL;












bool sCoolingAppInit(void);
void sCoolingAppTask(void *pParam);

u16 sCoolingAppGetCrc16(u8 *pBuf, u16 u16Len);

//send
bool sCoolingAppReadErr(void);
bool sCoolingAppReadSysFlow(void);
bool sCoolingAppReadSysPressure(void);
bool sCoolingAppReadInOutTemper(void);
bool sCoolingAppReadSpeed(void);
bool sCoolingAppReadSoftVer(void);
bool sCoolingAppReadRunMode(void);
bool sCoolingAppReadRunState(void);
bool sCoolingAppReadSysCfg01(void);
bool sCoolingAppReadSysCfg02(void);


bool sCoolingAppWriteSysParam(void);
bool sCoolingAppWritePowerOnOff(void);
bool sCoolingAppWriteResumeFactory(void);

//recv
bool sCoolingAppReadErrRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSysFlowRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSysPressureRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadInOutTemprRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSpeedRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSoftVerRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadRunModeRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadRunStateRespone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSysCfg01Respone(u8 *pRx, u16 u16Len);
bool sCoolingAppReadSysCfg02Respone(u8 *pRx, u16 u16Len);

bool sCoolingAppDecodePreCheck(u8 *pRx, u16 u16Len);

void sCoolingAppRecvProc(void);


//外部调用的接口
eCoolingAppCommState_t sCoolingAppGetCommSt(void);                              //获取液冷机通讯状态
stCoolingAppErrCode_t  sCoolingAppGetErrCode(void);                             //获取液冷机故障码
bool sCoolingAppGetInfo(stCoolingAppRealInfo_t **pInfo);                        //获取液冷机实时信息

bool sCoolingAppSetPowerOnOff(eCoolingAppCtrlParam_t eCtrl);                    //设置液冷机启停
bool sCoolingAppSetResumeFactory(eCoolingAppCtrlParam_t eCtrl);                 //设置液冷机恢复出厂默认参数
bool sCoolingAppSetSysParam(eEtyWriteAdjustReg_t eReg, u16 u16RegValue);        //设置液冷机系统参数
































/***************************************************************************************************
* Description                           :   液冷机 通讯资源初始化
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sCoolingAppInit(void)
{
    bool bRst = false;
    
    stUsartDrvCfg_t stUsartCfg_t =
    {
        .i32Baudrate    = cCoolingComBaudrate,
        .i32WordLen     = USART_WL_8BIT,
        .i32StopBit     = USART_STB_1BIT,
        .i32Praity      = USART_PM_NONE,
    };
        
    bRst = sUsartDrvOpen(cCoolingComUartNum, &stUsartCfg_t, cCoolingBuffSize, cCoolingBuffSize);
    
    pCoolingAppCache = (stCoolingAppCache_t *)MALLOC(sizeof(stCoolingAppCache_t));
    memset(pCoolingAppCache, 0, sizeof(stCoolingAppCache_t));
    
    
    //初始化设备地址
    pCoolingAppCache->u16Addr = cCoolingDevAddr;
    
    //shell命令初始化
    sCoolingAppShellInit();
    
    
    //建立线程
    xTaskCreate(sCoolingAppTask, "sCoolingAppTask", (512), NULL, 15, NULL);
    
    return bRst;
}












/***************************************************************************************************
* Description                           :   液冷机 轮询任务
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
void sCoolingAppTask(void *pParam)
{
    EN_SLOGD(TAG, "任务建立:英特尼迪液冷机轮询发送任务");
    
    
    while(1)
    {
        //接收报文
        sCoolingAppRecvProc();
        
        //索引递增，轮询发送
        pCoolingAppCache->u16Index++;
        
        switch(pCoolingAppCache->u16Index)
        {
        case 1:
            sCoolingAppReadErr();
            break;
        case 2:
            sCoolingAppReadSysFlow();
            break;
        case 3:
            sCoolingAppReadSysPressure();
            break;
        case 4:
            sCoolingAppReadInOutTemper();
            break;
        case 5:
            sCoolingAppReadSpeed();
            break;
        case 6:
            sCoolingAppReadSoftVer();
            break;
        case 7:
            sCoolingAppReadRunMode();
            break;
        case 8:
            sCoolingAppReadRunState();
            break;
        case 9:
            sCoolingAppReadSysCfg01();
            break;
        case 10:
            sCoolingAppReadSysCfg02();
            break;
        case 11:
            if(pCoolingAppCache->stSetParam.bNeedWrite == true)
            {
                sCoolingAppWriteSysParam();
            }
            break;
        case 12:
            sCoolingAppWritePowerOnOff();
            break;
        case 13:
            if(pCoolingAppCache->eResumeFact == eRemoteCtrlStart)
            {
                sCoolingAppWriteResumeFactory();
            }
            break;
        default:
            pCoolingAppCache->u16Index = 0;
            break;
        }
        
        //通讯故障判断
        if(pCoolingAppCache->u16CommFailCount < cCoolingMaxErrCnt)
        {
            pCoolingAppCache->eCommSt = eCommStateNormal;
            pCoolingAppCache->u16CommFailCount++;
        }
        else
        {
            pCoolingAppCache->eCommSt = eCommStateFailed;
        }
        
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
















/***************************************************************************************************
* Description                           :   获取Crc16（和en_common.c里面的多项式不同，单独定义一个接口）
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   以下Crc代码来源于英特尼迪液冷通讯协议
*                                       
****************************************************************************************************/
u16 sCoolingAppGetCrc16(u8 *pBuf, u16 u16Len)
{
    u16 u16Crc = 0xffff;
    u16 u16CrcFlag = 0;
    u16 i = 0, j = 0;
    u8 u8data = 0;
    
    for(j=0; j<u16Len; j++)
    {
        u8data = pBuf[j];
        u16Crc = u16Crc ^ u8data;
        
        for(i=0; i<8; i++)
        {
            u16CrcFlag = u16Crc & 0x0001;
            u16Crc = u16Crc >> 1;
            
            if(u16CrcFlag == 1)
            {
                //0xa001 Crc校验多项式
                u16Crc = u16Crc ^ 0xa001;
            }
        }
    }
    
    return u16Crc;
}















/***************************************************************************************************
* Description                           :   下发 读取液冷机当前故障的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadErr(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadBits;                                       //功能码：01
    u8TxBuf[u16Len++] = (eSigRegHighPress >> 8) & 0xFF;                         //起始地址H
    u8TxBuf[u16Len++] = (eSigRegHighPress >> 0) & 0xFF;                         //起始地址L
    u8TxBuf[u16Len++] = (eSigRegMax >> 8) & 0xFF;                               //查询个数H
    u8TxBuf[u16Len++] = (eSigRegMax >> 0) & 0xFF;                               //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}














/***************************************************************************************************
* Description                           :   下发 读取液冷机系统流量
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSysFlow(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegSysFlowRate >> 8) & 0xFF;                       //起始地址H
    u8TxBuf[u16Len++] = (eTelRegSysFlowRate >> 0) & 0xFF;                       //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}













/***************************************************************************************************
* Description                           :   下发 读取液冷机系统压力
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSysPressure(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegSysPressure >> 8) & 0xFF;                       //起始地址H
    u8TxBuf[u16Len++] = (eTelRegSysPressure >> 0) & 0xFF;                       //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}













/***************************************************************************************************
* Description                           :   下发 读取液冷机进出液温度
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadInOutTemper(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegInLiquidTemp >> 8) & 0xFF;                      //起始地址H
    u8TxBuf[u16Len++] = (eTelRegInLiquidTemp >> 0) & 0xFF;                      //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x02;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}














/***************************************************************************************************
* Description                           :   下发 读取液冷机循环泵和风机转速
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSpeed(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegPumpsSpeed >> 8) & 0xFF;                        //起始地址H
    u8TxBuf[u16Len++] = (eTelRegPumpsSpeed >> 0) & 0xFF;                        //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x02;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}














/***************************************************************************************************
* Description                           :   下发 读取液冷机软件版本号
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSoftVer(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegVersion >> 8) & 0xFF;                           //起始地址H
    u8TxBuf[u16Len++] = (eTelRegVersion >> 0) & 0xFF;                           //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}














/***************************************************************************************************
* Description                           :   下发 读取液冷机运行模式的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadRunMode(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegRunMode >> 8) & 0xFF;                           //起始地址H
    u8TxBuf[u16Len++] = (eTelRegRunMode >> 0) & 0xFF;                           //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}












/***************************************************************************************************
* Description                           :   下发 读取液冷机运行状态的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadRunState(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eTelRegRunning >> 8) & 0xFF;                           //起始地址H
    u8TxBuf[u16Len++] = (eTelRegRunning >> 0) & 0xFF;                           //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}












/***************************************************************************************************
* Description                           :   下发 读取液冷配置参数01的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSysCfg01(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eAdjustRegSpeed >> 8) & 0xFF;                          //起始地址H
    u8TxBuf[u16Len++] = (eAdjustRegSpeed >> 0) & 0xFF;                          //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x03;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}












/***************************************************************************************************
* Description                           :   下发 读取液冷配置参数02的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppReadSysCfg02(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeReadRegs;                                       //功能码：03
    u8TxBuf[u16Len++] = (eAdjustRegGunTemp >> 8) & 0xFF;                        //起始地址H
    u8TxBuf[u16Len++] = (eAdjustRegGunTemp >> 0) & 0xFF;                        //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //查询个数H
    u8TxBuf[u16Len++] = 0x15;                                                   //查询个数L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}












/***************************************************************************************************
* Description                           :   下发 设置液冷机系统参数的数据帧
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppWriteSysParam(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u16 u16Reg, u16Data;
    u8 u8TxBuf[cCoolingBuffSize];
    
    u16Reg = pCoolingAppCache->stSetParam.eReg;
    u16Data = pCoolingAppCache->stSetParam.u16RegValue;
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeWriteReg;                                       //功能码：06
    u8TxBuf[u16Len++] = (u16Reg >> 8) & 0xFF;                                   //起始地址H
    u8TxBuf[u16Len++] = (u16Reg >> 0) & 0xFF;                                   //起始地址L
    u8TxBuf[u16Len++] = (u16Data >> 8) & 0xFF;                                  //寄存器值H
    u8TxBuf[u16Len++] = (u16Data >> 0) & 0xFF;                                  //寄存器值L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    //4.标志位清零
    pCoolingAppCache->stSetParam.bNeedWrite = false;
    
    return true;
}













/***************************************************************************************************
* Description                           :   设置液冷机开关机
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppWritePowerOnOff(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u16 u16SendData = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    u16SendData = pCoolingAppCache->eStartup;
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeWriteBits;                                      //功能码：0F
    u8TxBuf[u16Len++] = (eCtrlRegPowerOnOff >> 8) & 0xFF;                       //起始地址H
    u8TxBuf[u16Len++] = (eCtrlRegPowerOnOff >> 0) & 0xFF;                       //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //设置个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //设置个数L
    u8TxBuf[u16Len++] = 0x02;                                                   //字节数
    u8TxBuf[u16Len++] = (u16SendData >> 8) & 0xFF;                              //强制数据H
    u8TxBuf[u16Len++] = (u16SendData >> 0) & 0xFF;                              //强制数据L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    return true;
}















/***************************************************************************************************
* Description                           :   设置液冷机恢复出厂参数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
*                                       
****************************************************************************************************/
bool sCoolingAppWriteResumeFactory(void)
{
    u16 u16Len= 0;
    u16 u16Crc = 0;
    u16 u16SendData = 0;
    u8 u8TxBuf[cCoolingBuffSize];
    
    u16SendData = pCoolingAppCache->eResumeFact;
    
    //1.组帧
    memset(u8TxBuf, 0, sizeof(u8TxBuf));
    u8TxBuf[u16Len++] = pCoolingAppCache->u16Addr;                              //液冷设备地址
    u8TxBuf[u16Len++] = eFunCodeWriteBits;                                      //功能码：0F
    u8TxBuf[u16Len++] = (eCtrlRegResumeFactory >> 8) & 0xFF;                    //起始地址H
    u8TxBuf[u16Len++] = (eCtrlRegResumeFactory >> 0) & 0xFF;                    //起始地址L
    u8TxBuf[u16Len++] = 0x00;                                                   //设置个数H
    u8TxBuf[u16Len++] = 0x01;                                                   //设置个数L
    u8TxBuf[u16Len++] = 0x02;                                                   //字节数
    u8TxBuf[u16Len++] = (u16SendData >> 8) & 0xFF;                              //强制数据H
    u8TxBuf[u16Len++] = (u16SendData >> 0) & 0xFF;                              //强制数据L
    
    //2.末尾增加Crc16
    u16Crc = sCoolingAppGetCrc16(u8TxBuf, u16Len);
    u8TxBuf[u16Len++] = (u16Crc >> 0) & 0xFF;                                   //Crc-L
    u8TxBuf[u16Len++] = (u16Crc >> 8) & 0xFF;                                   //Crc-H
    
    //3.发送数据
    sUsartDrvSend(cCoolingComUartNum, u16Len, u8TxBuf);
    
    //4.发送完就清除标识，避免重复发送，恢复后可能波特率不一致导致无法通讯，慎重！！！
    pCoolingAppCache->eResumeFact = eRemoteCtrlStop;
    
    return true;
}

















/***************************************************************************************************
* Description                           :   液冷机 故障数据 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadErrRespone(u8 *pRx, u16 u16Len)
{
    pCoolingAppCache->unErrCode.u16ErrCode = (pRx[0] << 8) | pRx[1];
    
    return true;
}














/***************************************************************************************************
* Description                           :   液冷机 系统流量 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSysFlowRespone(u8 *pRx, u16 u16Len)
{
    //系统流量，单位：0.01L/min
    pCoolingAppCache->stRealInfo.f32SysFlowRate = ((pRx[0] << 8) | pRx[1]) * 0.01;
    
    return true;
}













/***************************************************************************************************
* Description                           :   液冷机 系统压力 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSysPressureRespone(u8 *pRx, u16 u16Len)
{
    //系统压力，单位：0.01bar
    pCoolingAppCache->stRealInfo.f32SysPressure = ((pRx[0] << 8) | pRx[1]) * 0.01;
    
    return true;
}













/***************************************************************************************************
* Description                           :   液冷机 回液温度和供液温度 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadInOutTemprRespone(u8 *pRx, u16 u16Len)
{
    //回液温度，单位：0.01°C，偏移：-50°C
    pCoolingAppCache->stRealInfo.f32InTemper = (((pRx[0] << 8) | pRx[1]) * 0.1) - 50.0;
    
    //供液温度，单位：0.01°C，偏移：-50°C
    pCoolingAppCache->stRealInfo.f32OutTemper = (((pRx[2] << 8) | pRx[3]) * 0.1) - 50.0;
    
    return true;
}













/***************************************************************************************************
* Description                           :   液冷机 循环泵和风机转速 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSpeedRespone(u8 *pRx, u16 u16Len)
{
    //循环泵转速
    pCoolingAppCache->stRealInfo.u16PumpsSpeed = (pRx[0] << 8) | pRx[1];
    
    //风机转速
    pCoolingAppCache->stRealInfo.u16FanSpeed = (pRx[2] << 8) | pRx[3];
    
    return true;
}














/***************************************************************************************************
* Description                           :   液冷机 软件版本号 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSoftVerRespone(u8 *pRx, u16 u16Len)
{
    pCoolingAppCache->stRealInfo.u16Version = (pRx[0] << 8) | pRx[1];
    
    return true;
}
















/***************************************************************************************************
* Description                           :   液冷机 运行模式 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadRunModeRespone(u8 *pRx, u16 u16Len)
{
    pCoolingAppCache->stRealInfo.eRunMode = (pRx[0] << 8) | pRx[1];
    
    return true;
}















/***************************************************************************************************
* Description                           :   液冷机 运行状态 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadRunStateRespone(u8 *pRx, u16 u16Len)
{
    //开关机状态
    pCoolingAppCache->stRealInfo.u16RunState = (pRx[0] << 8) | pRx[1];
    
    return true;
}















/***************************************************************************************************
* Description                           :   液冷机 系统配置信息01 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSysCfg01Respone(u8 *pRx, u16 u16Len)
{
    //速度
    pCoolingAppCache->stCfgParam.u16Speed = (pRx[0] << 8) | pRx[1];
    //从站地址
    pCoolingAppCache->stCfgParam.u16DevAddr = (pRx[2] << 8) | pRx[3];
    //波特率设置字
    pCoolingAppCache->stCfgParam.u16BaudType = (pRx[4] << 8) | pRx[5];
    
    return true;
}














/***************************************************************************************************
* Description                           :   液冷机 系统配置信息02 读取响应
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppReadSysCfg02Respone(u8 *pRx, u16 u16Len)
{
    //枪头温度
    pCoolingAppCache->stCfgParam.f32GunTemp = (((pRx[0] << 8) | pRx[1]) * 0.1) - 50.0f;
    
    //1档温度
    pCoolingAppCache->stCfgParam.f32Lv1Temp = (((pRx[2] << 8) | pRx[3]) * 0.1) - 50.0f;
    //1档泵转速
    pCoolingAppCache->stCfgParam.u16Lv1PumpSpeed = (pRx[4] << 8) | pRx[5];
    //1档风机转速
    pCoolingAppCache->stCfgParam.u16Lv1FanSpeed = (pRx[6] << 8) | pRx[7];
    
    //2档温度
    pCoolingAppCache->stCfgParam.f32Lv2Temp = (((pRx[8] << 8) | pRx[9]) * 0.1) - 50.0f;
    //2档泵转速
    pCoolingAppCache->stCfgParam.u16Lv2PumpSpeed = (pRx[10] << 8) | pRx[11];
    //2档风机转速
    pCoolingAppCache->stCfgParam.u16Lv2FanSpeed = (pRx[12] << 8) | pRx[13];
    
    //3档温度
    pCoolingAppCache->stCfgParam.f32Lv3Temp = (((pRx[14] << 8) | pRx[15]) * 0.1) - 50.0f;
    //3档泵转速
    pCoolingAppCache->stCfgParam.u16Lv3PumpSpeed = (pRx[16] << 8) | pRx[17];
    //3档风机转速
    pCoolingAppCache->stCfgParam.u16Lv3FanSpeed = (pRx[18] << 8) | pRx[19];
    
    //4档温度
    pCoolingAppCache->stCfgParam.f32Lv4Temp = (((pRx[20] << 8) | pRx[21]) * 0.1) - 50.0f;
    //4档泵转速
    pCoolingAppCache->stCfgParam.u16Lv4PumpSpeed = (pRx[22] << 8) | pRx[23];
    //4档风机转速
    pCoolingAppCache->stCfgParam.u16Lv4FanSpeed = (pRx[24] << 8) | pRx[25];
    
    //5档温度
    pCoolingAppCache->stCfgParam.f32Lv5Temp = (((pRx[26] << 8) | pRx[27]) * 0.1) - 50.0f;
    //5档泵转速
    pCoolingAppCache->stCfgParam.u16Lv5PumpSpeed = (pRx[28] << 8) | pRx[29];
    //5档风机转速
    pCoolingAppCache->stCfgParam.u16Lv5FanSpeed = (pRx[30] << 8) | pRx[31];
    
    //高温报警值
    pCoolingAppCache->stCfgParam.f32HighTempAlarm = (((pRx[32] << 8) | pRx[33]) * 0.1) - 50.0f;
    
    //低压报警值
    pCoolingAppCache->stCfgParam.f32SlowPressure = ((pRx[38] << 8) | pRx[39]) * 0.01;
    //高压报警值
    pCoolingAppCache->stCfgParam.f32HighPressure = ((pRx[40] << 8) | pRx[41]) * 0.01;
    
    return true;
}
















/***************************************************************************************************
* Description                           :   液冷机 数据解析前的检查
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
bool sCoolingAppDecodePreCheck(u8 *pRx, u16 u16Len)
{
    u16 u16RecvCrc, u16CalCrc;
    
    //1.判断地址是否正确
    if(pRx[0] != pCoolingAppCache->u16Addr)
    {
        EN_SLOGI(TAG, "Recv:液冷机地址错误，不解析");
        return false;
    }
    
    //2.判断功能码是否符合
    if((pRx[1] != eFunCodeReadBits) 
        && (pRx[1] != eFunCodeReadRegs)
        && (pRx[1] != eFunCodeWriteReg)
        && (pRx[1] != eFunCodeWriteBits))
    {
        if((pRx[1] & 0x80) == 0x80)
        {
            EN_SLOGI(TAG, "Recv:液冷机功能码%x错误，不解析", pRx[1]);
        }
        else
        {
            EN_SLOGI(TAG, "Recv:液冷机功能码未识别，不解析");
        }
        
        return false;
    }
    
    //3.判断Crc16是否正确
    u16CalCrc = sCoolingAppGetCrc16(pRx, (u16Len-2));
    u16RecvCrc = (pRx[u16Len-1] << 8) | pRx[u16Len-2];
    
    if(u16CalCrc != u16RecvCrc)
    {
        EN_SLOGI(TAG, "Recv:液冷机Crc错误，不解析");
        return false;
    }
    
    return true;
}














/***************************************************************************************************
* Description                           :   液冷机 接收数据解析
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-19
* notice                                :   
****************************************************************************************************/
void sCoolingAppRecvProc(void)
{
    i32 i32ReadLen;
    u8 u8RxBuff[cCoolingBuffSize];
    
    memset(u8RxBuff, 0, cCoolingBuffSize);
    i32ReadLen = sUsartDrvRecv(cCoolingComUartNum, sizeof(u8RxBuff), u8RxBuff);
    if(i32ReadLen > 0)
    {
        //判断数据是否符合要求
        if(sCoolingAppDecodePreCheck(u8RxBuff, i32ReadLen) == false)
        {
            return;
        }
        
        //故障清零
        pCoolingAppCache->u16CommFailCount = 0;
        
        switch(pCoolingAppCache->u16Index)
        {
        case 1:
            sCoolingAppReadErrRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 2:
            sCoolingAppReadSysFlowRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 3:
            sCoolingAppReadSysPressureRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 4:
            sCoolingAppReadInOutTemprRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 5:
            sCoolingAppReadSpeedRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 6:
            sCoolingAppReadSoftVerRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 7:
            sCoolingAppReadRunModeRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 8:
            sCoolingAppReadRunStateRespone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 9:
            sCoolingAppReadSysCfg01Respone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        case 10:
            sCoolingAppReadSysCfg02Respone(&u8RxBuff[3], u8RxBuff[2]);
            break;
        default:
            break;
        }
    }
}


















/***************************************************************************************************
* Description                           :   获取液冷机通讯状态
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
eCoolingAppCommState_t sCoolingAppGetCommSt(void)
{
    if(pCoolingAppCache != NULL)
    {
        return pCoolingAppCache->eCommSt;
    }
    
    return eCommStateFailed;
}
















/***************************************************************************************************
* Description                           :   获取液冷机故障码
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
stCoolingAppErrCode_t sCoolingAppGetErrCode(void)
{
    stCoolingAppErrCode_t stErrCode;
    
    memset(&stErrCode, 0, sizeof(stCoolingAppErrCode_t));
    if(pCoolingAppCache != NULL)
    {
        stErrCode = pCoolingAppCache->unErrCode.stErrCode;
    }
    
    return stErrCode;
}







/***************************************************************************************************
* Description                           :   获取液冷机实时信息
* Author                                :   Hall
* Creat Date                            :   2024-08-22
* notice                                :   
****************************************************************************************************/
bool sCoolingAppGetInfo(stCoolingAppRealInfo_t **pInfo)
{
    if((pInfo != NULL) && (pCoolingAppCache != NULL))
    {
        (*pInfo) = &pCoolingAppCache->stRealInfo;
        
        return true;
    }
    
    return false;
}
















/***************************************************************************************************
* Description                           :   设置液冷机启停
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppSetPowerOnOff(eCoolingAppCtrlParam_t eCtrl)
{
    if(pCoolingAppCache != NULL)
    {
        pCoolingAppCache->eStartup = eCtrl;
        return true;
    }
    
    return false;
}
















/***************************************************************************************************
* Description                           :   设置液冷机恢复出厂默认参数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppSetResumeFactory(eCoolingAppCtrlParam_t eCtrl)
{
    if(pCoolingAppCache != NULL)
    {
        pCoolingAppCache->eResumeFact = eCtrl;
        return true;
    }
    
    return false;
}















/***************************************************************************************************
* Description                           :   设置设置液冷机系统参数
* Author                                :   haisheng.dang
* Creat Date                            :   2024-06-20
* notice                                :   
****************************************************************************************************/
bool sCoolingAppSetSysParam(eEtyWriteAdjustReg_t eReg, u16 u16RegValue)
{
    if(pCoolingAppCache != NULL)
    {
        pCoolingAppCache->stSetParam.eReg = eReg;
        pCoolingAppCache->stSetParam.u16RegValue = u16RegValue;
        
        //刷新标志置true
        pCoolingAppCache->stSetParam.bNeedWrite = true;
        
        return true;
    }
    
    return false;
}











