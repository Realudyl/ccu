/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_shell.c
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-11
* notice                                :     本文件负责实现内网协议组件在shell界面上的一些接口
****************************************************************************************************/
#include "en_shell.h"

#include "private_drv_api_m.h"
#include "private_drv_shell.h"

#include "private_drv_opt.h"





//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_shell";



extern stPrivDrvCache_t *pPrivDrvCache;



bool sPrivDrvShellRegister(void);



#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧的一些shell指令资源
bool sPrivDrvShellSetCpSn(const stShellPkt_t *pkg);
bool sPrivDrvShellSetCpName(const stShellPkt_t *pkg);
bool sPrivDrvShellGetCpSn(const stShellPkt_t *pkg);
bool sPrivDrvShellGetInfo(const stShellPkt_t *pkg);
bool sPrivDrvShellCpParam(const stShellPkt_t *pkg);
bool sPrivDrvShellCpSend(const stShellPkt_t *pkg);

ePrivDrvCmd_t sPrivDrvShellCpCmdConvert(ePrivDrvCmd_t eCmd);

#else                                                                           //Slave 侧的一些shell指令资源

#endif












#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧的一些shell指令资源
stShellCmd_t stSellCmdSetCpSn = 
{
    .pCmd       = "setcpsn",
    .pFormat    = "格式:setcpsn 编号 gun_id",
    .pFunction  = "功能:设置桩端SN编号",
    .pRemarks   = "备注:gun_id---枪号(0(整桩),1,2...)无默认0, 编号",
    .pFunc      = sPrivDrvShellSetCpSn,
};



stShellCmd_t stSellCmdSetCpName = 
{
    .pCmd       = "setcpname",
    .pFormat    = "格式:setcpname 设备名称",
    .pFunction  = "功能:设置桩端设备名称",
    .pRemarks   = "备注:AC7000-AG-XX",
    .pFunc      = sPrivDrvShellSetCpName,
};



stShellCmd_t stSellCmdGetCpSn = 
{
    .pCmd       = "getcpsn",
    .pFormat    = "格式:getcpsn gun_id(可选)",
    .pFunction  = "功能:获取桩端SN编号",
    .pRemarks   = "备注:gun_id---枪号(0(整桩),1,2...)无默认0",
    .pFunc      = sPrivDrvShellGetCpSn,
};



stShellCmd_t stSellCmdGetCpInfo = 
{
    .pCmd       = "getcpinfo",
    .pFormat    = "格式:getcpinfo",
    .pFunction  = "功能:获取桩端信息",
    .pRemarks   = "备注:",
    .pFunc      = sPrivDrvShellGetInfo,
};



stShellCmd_t stSellCmdCpParam = 
{
    .pCmd       = "cpparam",
    .pFormat    = "格式:cpparam gun_id addr cmd rpt rst w_data",
    .pFunction  = "功能:gun_id:枪号,addr:AA地址,cmd:命令,rpt:方向,rst:结果,w_data:写数据(可选)",
    .pRemarks   = "备注:cmd(0查询1设置254桩查询255桩设置),rpt(0发送1应答),rst(0成功1失败2max),w_data(hex数据)",
    .pFunc      = sPrivDrvShellCpParam,
};



stShellCmd_t stSellCmdCpSend = 
{
    .pCmd       = "cpsend",
    .pFormat    = "格式:cpsend addr rpt w_data",
    .pFunction  = "功能:addr:AA地址,rpt:方向,w_data:写数据(可选)",
    .pRemarks   = "备注:addr(01~CmdMax),rpt(0发送1应答),w_data(hex数据)",
    .pFunc      = sPrivDrvShellCpSend,
};



#else                                                                           //Slave 侧的一些shell指令资源

#endif












/**********************************************************************************************
* Description       :     内网-shell-注册
* Author            :     XRG
* modified Date     :     2024-02-01
* return            :     bRst
* notice            :     
***********************************************************************************************/
bool sPrivDrvShellRegister(void)
{
    bool bRst = true;
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧shell指令注册
    bRst = bRst & sShellCmdRegister(&stSellCmdSetCpSn);
    bRst = bRst & sShellCmdRegister(&stSellCmdSetCpName);
    bRst = bRst & sShellCmdRegister(&stSellCmdGetCpSn);
    bRst = bRst & sShellCmdRegister(&stSellCmdGetCpInfo);
    bRst = bRst & sShellCmdRegister(&stSellCmdCpParam);
    bRst = bRst & sShellCmdRegister(&stSellCmdCpSend);
#else                                                                           //Slave 侧shell指令注册
#endif

    return(bRst);
}














#if (cSdkPrivDevType == cSdkPrivDevTypeM)                                       //Master 侧的一些shell指令资源

/**********************************************************************************************
* Description       :     内网-shell-设置桩端SN
* Author            :     XRG
* modified Date     :     2024-02-01
* param[in]         :     pkg      shell输入结构体
* return            :     bRst
* notice            :     
***********************************************************************************************/
bool sPrivDrvShellSetCpSn(const stShellPkt_t *pkg)
{
    bool bRst;
    u8   u8Sn[cPrioDrvLenParamSn +1];
    int  len;
    i32  i32GunId = 0;
    
    stPrivDrvCmd0A_t *pCmd0A = NULL;
    
    if(pkg->paraNum < 1)
    {
        EN_SLOGE(TAG, "参数%d错误, 请参考指令%s", pkg->paraNum, stSellCmdSetCpSn.pFormat);
        return(false);
    }
    
    if((pkg->paraNum > 1) && (pkg->para[1] != NULL))
    {
        i32GunId = atoi(pkg->para[1]);
        if((i32GunId < 0) || (i32GunId > cPrivDrvGunNumMax))
        {
            EN_SLOGE(TAG, "抢号错误%d, 请参考指令%s", i32GunId, stSellCmdGetCpSn.pFormat);
            return(false);
        }
    }
    
    len = strlen((pkg->para[0]));
    if((len <= 0) || (len > cPrioDrvLenParamSn))
    {
        EN_SLOGE(TAG, "参数%d错误, 请参考指令%s", pkg->paraNum, stSellCmdSetCpSn.pFormat);
        return(false);
    }
    
    memset(u8Sn, 0, sizeof(u8Sn));
    memcpy(u8Sn, (const u8 *)pkg->para[0], len);
    bRst = sPrivDrvSetCmdParam(true, (u8)i32GunId, ePrivDrvCfgCmdTypeSet, ePrivDrvCmdRstMax, ePrivDrvParamAddr001, cPrioDrvLenParamSn, u8Sn, 0);
    if(bRst)
    {
        sPrivDrvGetCmdParam(false, &pCmd0A);
        memset(u8Sn, 0, sizeof(u8Sn));
        memcpy(u8Sn, pCmd0A->u8Data, pCmd0A->u16Len);
        EN_SLOGI(TAG, "桩端SN:%s",   u8Sn);
        EN_SLOGI(TAG, "1秒后复位");
        vTaskDelay(1000 / portTICK_RATE_MS);
        esp_restart();
    }
    else
    {
        EN_SLOGI(TAG, "设置桩端SN失败");
    }
    return(bRst);
}





/**********************************************************************************************
* Description       :     内网-shell-设置桩端设备名称
* Author            :     XRG
* modified Date     :     2024-04-29
* param[in]         :     pkg      shell输入结构体
* return            :     bRst
* notice            :     
***********************************************************************************************/
bool sPrivDrvShellSetCpName(const stShellPkt_t *pkg)
{
    bool                    bRst;
    stPrivDrvChgParam1_t    stParam1;
    stPrivDrvCmd0A_t        *pCmd0A = NULL;
    
    if(pkg->paraNum < 1)
    {
        EN_SLOGE(TAG, "参数%d错误, 请参考指令%s", pkg->paraNum, stSellCmdSetCpSn.pFormat);
        return(false);
    }
    
    
    bRst = sPrivDrvSetCmdParam(true, 1, ePrivDrvCfgCmdTypeGet, ePrivDrvCmdRstMax, ePrivDrvParamAddr097, 0, NULL, 0);
    if(bRst)
    {
        bRst = sPrivDrvGetCmdParam(false, &pCmd0A);
        if((bRst) && (pCmd0A->eAddr == ePrivDrvParamAddr097))
        {
            memset(&stParam1,           0,              sizeof(stParam1));
            memcpy(&stParam1,           pCmd0A->u8Data, sizeof(stParam1));
            memset( stParam1.u8DevName, 0,              sizeof(stParam1.u8DevName));
            memcpy( stParam1.u8DevName, pkg->para[0],   strlen(pkg->para[0]));
            bRst = sPrivDrvSetCmdParam(true, 1, ePrivDrvCfgCmdTypeSet, ePrivDrvCmdRstMax, ePrivDrvParamAddr097, sizeof(stParam1), (const u8 *)&stParam1, 0);
        }
        else
        {
            bRst = false;
        }
    }
    
    EN_SLOGI(TAG, "设置桩端名称:%s,结果:%s", pkg->para[0], (bRst) ? "成功":"失败");
    if(bRst)
    {
        EN_SLOGI(TAG, "1秒后复位");
        vTaskDelay(1000 / portTICK_RATE_MS);
        esp_restart();
    }
    return(bRst);
}





/**********************************************************************************************
* Description       :     内网-shell-获取桩端SN
* Author            :     XRG
* modified Date     :     2024-02-01
* param[in]         :     pkg      shell输入结构体
* return            :     bRst
* notice            :     
***********************************************************************************************/
bool sPrivDrvShellGetCpSn(const stShellPkt_t *pkg)
{
    bool bRst;
    char u8Sn[128];
    i32  i32GunId = 0;
    stPrivDrvCmd0A_t *pCmd0A = NULL;
    
    if((pkg->paraNum == 1) && (pkg->para[0] != NULL))
    {
        i32GunId = atoi(pkg->para[0]);
        if((i32GunId < 0) || (i32GunId > cPrivDrvGunNumMax))
        {
            EN_SLOGE(TAG, "抢号错误%d, 请参考指令%s", i32GunId, stSellCmdGetCpSn.pFormat);
            return(false);
        }
    }
    
    bRst = sPrivDrvSetCmdParam(true, (u8)i32GunId, ePrivDrvCfgCmdTypeGet, ePrivDrvCmdRstMax, ePrivDrvParamAddr001, 0, NULL, 0);
    if(bRst == true)
    {
        sPrivDrvGetCmdParam(false, &pCmd0A);
        memset(u8Sn, 0, sizeof(u8Sn));
        memcpy(u8Sn, pCmd0A->u8Data, pCmd0A->u16Len);
        EN_SLOGI(TAG, "桩端SN:%s",   u8Sn);
    }
    
    return(bRst);
}





/**********************************************************************************************
* Description       :     桩端-读取桩端信息回调
* Author            :     XRG
* modified Date     :     2024-02-05
* param[in]         :     pkg:shell参数结构体
* notice            :     
***********************************************************************************************/
bool sPrivDrvShellGetInfo(const stShellPkt_t *pkg)
{
    typedef union 
    {
        ePrivDrvMoneyCalcBy_t         eMoneyCalcBy;
        ePrivDrvChargerDisableFlag_t  eFlag;
        ePrivDrvStartPowerMode_t      eStartMode;
        ePrivDrvChargerWorkMode_t     eWorkMode;
        ePrivDrvGunWorkStatus_t       eStatus;
        ePrivDrvGunConnStatus_t       eConnStatus;
        ePrivDrvChargerType_t         eType;
        ePrivDrvRebootReason_t        eReason;
        ePrivDrvVersion_t             eVer;
        u8                            u8GunNum;
        i32                           i32PowerMax;
        i32                           i32Curr;
        u8                            u8Dat[64];
    }__attribute__((packed)) unTempCache_t;
    
    
    unTempCache_t            unTempCache;
    const char              *pChgType[]  = {"单相交流","三相交流","单相直流","三相直流"};
    const char              *pWorkMode[] = {"联网","离网","即插即用","一卡一桩","定时充电","蓝牙模式"};
    const char              *pWorkSts[]  = {"无","Available","Preparing","Charging","SuspendedEVSE","SuspendedEV","Finishing","Reserved","Unavailable","Faulted","BMS启动中","BMS开机准备中","BMS运行暂停"};
    const char              *pConnSts[]  = {"断开","半连接","连接","未知"};
    const char              *pRsn[]      = {"NULL","正常断电","硬件重启","看门狗复位","异常断电"};
    u8                       u8GunNum;
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetProtocolVersion(&unTempCache.eVer);
    EN_SLOGI(TAG,"PrivDrvVersion  :0x%.2X",  unTempCache.eVer);
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetSn(sizeof(unTempCache), unTempCache.u8Dat);
    EN_SLOGI(TAG,"SN              :%s",  unTempCache.u8Dat);
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetDevCode(sizeof(unTempCache), unTempCache.u8Dat);
    EN_SLOGI(TAG,"DevCode         :%s",  unTempCache.u8Dat);
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetDevName(sizeof(unTempCache), unTempCache.u8Dat);
    EN_SLOGI(TAG,"DevName         :%s",  unTempCache.u8Dat);
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetHwVer(sizeof(unTempCache), unTempCache.u8Dat);
    EN_SLOGI(TAG,"HwVer           :%s",  unTempCache.u8Dat);
    
    memset(&unTempCache, 0, sizeof(unTempCache));
    sPrivDrvGetSwVer(sizeof(unTempCache), unTempCache.u8Dat);
    EN_SLOGI(TAG,"SwVer           :%s",  unTempCache.u8Dat);
    
    u8GunNum = 0;
    sPrivDrvGetGunNum(&u8GunNum);
    EN_SLOGI(TAG,"GunNum          :%d", u8GunNum);
    
    unTempCache.eType = 0;
    sPrivDrvGetChargerType(&unTempCache.eType);
    EN_SLOGI(TAG,"Type            :%s", pChgType[unTempCache.eType]);
    
    unTempCache.eMoneyCalcBy = 0;
    sPrivDrvGetMoneyCalcBy(&unTempCache.eMoneyCalcBy);
    EN_SLOGI(TAG,"MoneyCalcBy     :%s",  (unTempCache.eMoneyCalcBy ==ePrivDrvMoneyCalcByChg)?"桩端计费":"服务器计费");
    
    unTempCache.eFlag = 0;
    sPrivDrvGetDisableFlag(&unTempCache.eFlag);
    EN_SLOGI(TAG,"Disable         :%s", (unTempCache.eFlag == ePrivDrvChargerDisableFlagFalse)?"未禁用":"禁用");
    
    unTempCache.eStartMode = 0;
    sPrivDrvGetStartPowerMode(&unTempCache.eStartMode);
    EN_SLOGI(TAG,"StartMode       :%s", (unTempCache.eStartMode == ePrivDrvStartPowerModeNormal)?"正常启动":"最小功率启动");
    
    unTempCache.eWorkMode = 0;
    sPrivDrvGetWorkMode(&unTempCache.eWorkMode);
    EN_SLOGI(TAG,"WorkMode        :%s", pWorkMode[(u8)unTempCache.eWorkMode]);
    
    unTempCache.i32PowerMax = 0;
    sPrivDrvGetPowerMax(&unTempCache.i32PowerMax);
    EN_SLOGI(TAG,"PowerMax        :%d", unTempCache.i32PowerMax);
    
    unTempCache.i32Curr = 0;
    sPrivDrvGetCurrMax(&unTempCache.i32Curr);
    EN_SLOGI(TAG,"CurrMax         :%d", unTempCache.i32Curr);
    
    unTempCache.eReason = 0;
    sPrivDrvGetRebootReason(&unTempCache.eReason);
    EN_SLOGI(TAG,"RebootReason    :%s", pRsn[unTempCache.eReason]);
    for(u8 i = 0; i < u8GunNum; i++)
    {
        unTempCache.eStatus = 0;
        sPrivDrvGetGunWorkStatus(i + cPrivDrvGunIdBase, &unTempCache.eStatus);
        EN_SLOGI(TAG,"%dGunSts         :%s", i, pWorkSts[(u8)unTempCache.eStatus]);
        
        unTempCache.eConnStatus = 0;
        sPrivDrvGetGunConnStatus(i + cPrivDrvGunIdBase, &unTempCache.eConnStatus);
        EN_SLOGI(TAG,"%dGunConnSts     :%s", i, pConnSts[(u8)unTempCache.eConnStatus]);
    }
    
    return(true);
}





/**********************************************************************************************
* Description       :     内网-shell-AA参数设置/读取
* Author            :     XRG
* modified Date     :     2024-04-17
* param[in]         :     pkg      shell输入结构体
* return            :     bRst
* notice            :     如果是设置，设置的数据需要2的倍数字符
                          查询桩时间:cpparam 0 2   0 1 2
                          设置桩时间:cpparam 0 2   1 0 2 180203040506
                          设置桩模式:cpparam 0 13  1 0 2 00
                          触发cmd08 :cpparam 1 157 1 0 2 08
***********************************************************************************************/
bool sPrivDrvShellCpParam(const stShellPkt_t *pkg)
{
    bool bRst;
    i32  i32GunId;
    i32  i32Addr;
    i32  i32Cmd;
    i32  i32Rpt;
    i32  i32Rst;
    i32  i32Len;
    u8   *pDat = NULL;
    const char              *pCmd[]  = {"查询","设置","桩查询","桩设置"};
    
    
    if((pkg->paraNum < 5) || (pkg->paraNum > 7))
    {
        EN_SLOGE(TAG, "参数长度%d错误, 请参考指令%s", pkg->paraNum, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    i32GunId = atoi(pkg->para[0]);
    if((i32GunId < 0) || (i32GunId > cPrivDrvGunNumMax))
    {
        EN_SLOGE(TAG, "抢号%d错误, 请参考指令%s", i32GunId, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    i32Addr = atoi(pkg->para[1]);
    if((i32Addr < 0) || (i32Addr > ePrivDrvParamAddrMax))
    {
        EN_SLOGE(TAG, "地址%d错误, 请参考指令%s", i32GunId, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    i32Cmd = atoi(pkg->para[2]);
    if(!((i32Cmd == ePrivDrvCfgCmdTypeGet) || (i32Cmd == ePrivDrvCfgCmdTypeSet) ||
         (i32Cmd == ePrivDrvCfgCmdTypeChargGet) || (i32Cmd == ePrivDrvCfgCmdTypeChargSet)))
    {
        EN_SLOGE(TAG, "命令%d错误, 请参考指令%s", i32Cmd, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    i32Rpt = atoi(pkg->para[3]);
    if(!((i32Rpt == 0) || (i32Rpt == 1)))
    {
        EN_SLOGE(TAG, "方向%d错误, 请参考指令%s", i32Cmd, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    i32Rst = atoi(pkg->para[4]);
    i32Len = 0;
    if((i32Cmd != ePrivDrvCfgCmdTypeGet) && (pkg->para[5] != NULL))
    {
        i32Len = strlen(pkg->para[5]);
        if((i32Len <= 0) || ((i32Len % 2) != 0))
        {
            EN_SLOGE(TAG, "设置参数长度%d异常, 请参考指令%s", i32Len, stSellCmdGetCpSn.pFormat);
            return(false);
        }
        
        pDat   = heap_caps_malloc(1024, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if(pDat == NULL)
        {
            return(false);
        }
        memset(pDat, 0, 1024);
        char  u8Hex[3] = {0, 0, 0};
        i32Len /= 2;
        for(int i = 0; i < i32Len; i++)
        {
            memcpy(u8Hex, pkg->para[5]+(i*2), 2);
            u8 tmp  = strtol((char *)u8Hex, 0, 16);
            pDat[i] = tmp;
        }
    }
    
    bRst = sPrivDrvSetCmdParam(i32Rpt, (u8)i32GunId, i32Cmd, i32Rst, i32Addr, i32Len, pDat, 0);
    i32Cmd = (i32Cmd > ePrivDrvCfgCmdTypeChargGet) ? i32Cmd - 0xFC : i32Cmd;
    EN_SLOGI(TAG, "地址:%d,命令:%s,结果:%s",     i32Addr, pCmd[i32Cmd], (bRst == true)?"成功":"失败");
    
    if(pDat)
    {
        free(pDat);
        pDat = NULL;
    }
    return(bRst);
}





/**********************************************************************************************
* Description       :     内网-shell-发送到桩
* Author            :     XRG
* modified Date     :     2024-05-11
* param[in]         :     pkg      shell输入结构体
* return            :     bRst
* notice            :     应答cmdA4:cpsend 04 1 01050001000100
                          发送cmdA6:cpsend A6 0 01030000000000FB9600000030303030303030303030303
                          030303030303030303030303030383537333838310000000000000000000000000000
                          000000000000000000000000000000000000000000000000000000000000000000000
                          000000000000000000000000000000000000000000000000000000000000000000000
                          000000
***********************************************************************************************/
bool sPrivDrvShellCpSend(const stShellPkt_t *pkg)
{
    bool                 bRst;
    u8                   u8Addr;
    i32                  i32Rpt;
    i32                  i32Len = 0;
    u8                  *pDat   = NULL;
    unPrivDrvPkt_t      *pPkt   = NULL;
    stPrivDrvHead_t     *pHead  = NULL;
    SemaphoreHandle_t    hMutex = NULL;
    
    
    if((pkg->paraNum < 2) || (pkg->paraNum > 3))
    {
        EN_SLOGE(TAG, "参数长度%d错误, 请参考指令%s", pkg->paraNum, stSellCmdGetCpSn.pFormat);
        return(false);
    }
    
    //地址
    if(strlen(pkg->para[0]) != 2)
    {
        EN_SLOGE(TAG, "执行出错, 输入参数异常");
        return(false);
    }
    u8Addr = strtol(pkg->para[0], 0, 16);
    if(u8Addr >= ePrivDrvCmdMax)
    {
        EN_SLOGE(TAG, "执行出错, 输入参数异常");
        return(false);
    }
    
    //方向
    i32Rpt  = atoi(pkg->para[1]);
    i32Rpt  = !i32Rpt;//取反目的是统一
    if(!((i32Rpt == 0) || (i32Rpt == 1)))
    {
        EN_SLOGE(TAG, "执行出错, 输入参数异常");
        return(false);
    }
    
    //数据域
    if(pkg->para[2] != NULL)
    {
        i32Len = strlen(pkg->para[2]);
        if((i32Len <= 0) || ((i32Len % 2) != 0))
        {
            EN_SLOGE(TAG, "设置数据长度%d异常, 请参考指令%s", i32Len, stSellCmdGetCpSn.pFormat);
            return(false);
        }
        
        pDat   = heap_caps_malloc(1024, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if(pDat == NULL)
        {
            return(false);
        }
        memset(pDat, 0, 1024);
        char  u8Hex[3] = {0, 0, 0};
        i32Len /= 2;
        for(int i = 0; i < i32Len; i++)
        {
            memcpy(u8Hex, pkg->para[2]+(i*2), 2);
            u8 tmp  = strtol((char *)u8Hex, 0, 16);
            pDat[i] = tmp;
        }
    }
    
    //组包
    hMutex  = (i32Rpt == 1) ? pPrivDrvCache->hRptTxBufMutex :pPrivDrvCache->hAckTxBufMutex;
    xSemaphoreTake(hMutex, portMAX_DELAY);
    pPkt    = (unPrivDrvPkt_t *)((i32Rpt == 1) ? pPrivDrvCache->u8RptTxBuf : pPrivDrvCache->u8AckTxBuf);
    memset(pPkt, 0, sizeof(unPrivDrvPkt_t));
    pHead   = &pPkt->stPkt.stHead;
    sPrivDrvSetHead(pHead, (ePrivDrvCmd_t)u8Addr, 0, 0, (u16)i32Len);
    if(i32Len != 0)
    {
        memcpy(pPkt->stPkt.unPayload.u8Data, pDat, i32Len);
    }
    
    i32Len += cPrivDrvHeadSize;
    sPrivDrvSetCrc((u8 *)pPkt, (u16)i32Len);
    i32Len += cPrivDrvCrcSize;
    
    //发送
    bRst = sPrivDrvSend(sPrivDrvShellCpCmdConvert(u8Addr), (u8 *)pPkt, (u16)i32Len, (bool)i32Rpt);
    xSemaphoreGive(hMutex);
    EN_SLOGI(TAG, "%s地址:0x%02X,结果:%s", (i32Rpt == 1)?"发送":"应答"    , u8Addr, (bRst == true)?"成功":"失败");
    
    //释放资源
    if(pDat)
    {
        free(pDat);
        pDat = NULL;
    }
    return(bRst);
}






/**********************************************************************************************
* Description       :     内网-shell-地址转换
* Author            :     XRG
* modified Date     :     2024-05-11
* notice            :     供 Master 端使用
***********************************************************************************************/
ePrivDrvCmd_t sPrivDrvShellCpCmdConvert(ePrivDrvCmd_t eCmd)
{
    ePrivDrvCmd_t eRstCmd = ePrivDrvCmdMax;
    
    switch(eCmd)
    {
    case ePrivDrvCmd01:
        eRstCmd = ePrivDrvCmdA1;
        break;
    case ePrivDrvCmd02:
        eRstCmd = ePrivDrvCmdA2;
        break;
    case ePrivDrvCmd03:
        eRstCmd = ePrivDrvCmdA3;
        break;
    case ePrivDrvCmd04:
        eRstCmd = ePrivDrvCmdA4;
        break;
    case ePrivDrvCmd05:
        eRstCmd = ePrivDrvCmdA5;
        break;
    case ePrivDrvCmd06:
        eRstCmd = ePrivDrvCmdA6;
        break;
    case ePrivDrvCmd07:
        eRstCmd = ePrivDrvCmdA7;
        break;
    case ePrivDrvCmd08:
        eRstCmd = ePrivDrvCmdA8;
        break;
    case ePrivDrvCmd09:
        eRstCmd = ePrivDrvCmdA9;
        break;
    case ePrivDrvCmd0A:
        eRstCmd = ePrivDrvCmdAA;
        break;
    case ePrivDrvCmd0B:
        eRstCmd = ePrivDrvCmdAB;
        break;
    case ePrivDrvCmd0C:
        eRstCmd = ePrivDrvCmdAC;
        break;
    case ePrivDrvCmd0D:
        eRstCmd = ePrivDrvCmdAD;
        break;
    case ePrivDrvCmd0E:
        eRstCmd = ePrivDrvCmdAE;
        break;
    case ePrivDrvCmd0F:
        eRstCmd = ePrivDrvCmdAF;
        break;
    case ePrivDrvCmd11:
        eRstCmd = ePrivDrvCmdB1;
        break;
    case ePrivDrvCmd12:
        eRstCmd = ePrivDrvCmdB2;
        break;
    case ePrivDrvCmd13:
        eRstCmd = ePrivDrvCmdB3;
        break;
    case ePrivDrvCmd14:
        eRstCmd = ePrivDrvCmdB4;
        break;
    case ePrivDrvCmdA1:
        eRstCmd = ePrivDrvCmd01;
        break;
    case ePrivDrvCmdA2:
        eRstCmd = ePrivDrvCmd02;
        break;
    case ePrivDrvCmdA3:
        eRstCmd = ePrivDrvCmd03;
        break;
    case ePrivDrvCmdA4:
        eRstCmd = ePrivDrvCmd04;
        break;
    case ePrivDrvCmdA5:
        eRstCmd = ePrivDrvCmd05;
        break;
    case ePrivDrvCmdA6:
        eRstCmd = ePrivDrvCmd06;
        break;
    case ePrivDrvCmdA7:
        eRstCmd = ePrivDrvCmd07;
        break;
    case ePrivDrvCmdA8:
        eRstCmd = ePrivDrvCmd08;
        break;
    case ePrivDrvCmdA9:
        eRstCmd = ePrivDrvCmd09;
        break;
    case ePrivDrvCmdAA:
        eRstCmd = ePrivDrvCmd0A;
        break;
    case ePrivDrvCmdAB:
        eRstCmd = ePrivDrvCmd0B;
        break;
    case ePrivDrvCmdAC:
        eRstCmd = ePrivDrvCmd0C;
        break;
    case ePrivDrvCmdAD:
        eRstCmd = ePrivDrvCmd0D;
        break;
    case ePrivDrvCmdAE:
        eRstCmd = ePrivDrvCmd0E;
        break;
    case ePrivDrvCmdAF:
        eRstCmd = ePrivDrvCmd0F;
        break;
    case ePrivDrvCmdB1:
        eRstCmd = ePrivDrvCmd11;
        break;
    case ePrivDrvCmdB2:
        eRstCmd = ePrivDrvCmd12;
        break;
    case ePrivDrvCmdB3:
        eRstCmd = ePrivDrvCmd13;
        break;
    case ePrivDrvCmdB4:
        eRstCmd = ePrivDrvCmd14;
        break;
    default:
        break;
    }
    return(eRstCmd);
}

#endif





