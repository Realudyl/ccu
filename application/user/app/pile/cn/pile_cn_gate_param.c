/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_param.c
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 参数管理
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
#include "pile_cn.h"
#include "rtc_drv_interface.h"





//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "pile_cn_gate_param";




extern stPileCnCache_t *pPileCnCache;










bool sPileCnGateParamProc   (const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc001(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc002(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc018(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc067(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc078(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc079(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc080(const stPrivDrvCmdAA_t *pCmdAa);


bool sPileCnGateParamProc153(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc157(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc159(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc167(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc171(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc183(const stPrivDrvCmdAA_t *pCmdAa);



bool sPileCnGateParamProc192(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc198(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc199(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc200(const stPrivDrvCmdAA_t *pCmdAa);
bool sPileCnGateParamProc201(const stPrivDrvCmdAA_t *pCmdAa);






sPrivDrvParamMap_t sPrivDrvParamMap[] = 
{
    {   ePrivDrvParamAddr001,       sPileCnGateParamProc001,    "SN号"                         },
    {   ePrivDrvParamAddr002,       sPileCnGateParamProc002,    "对时"                         },
    {   ePrivDrvParamAddr003,       NULL,                       "服务器IP"                     },
    {   ePrivDrvParamAddr004,       NULL,                       "服务器PORT"                   },
    {   ePrivDrvParamAddr005,       NULL,                       "服务器PATH"                   },
    {   ePrivDrvParamAddr006,       NULL,                       "OCPP是否加密"                 },
    {   ePrivDrvParamAddr007,       NULL,                       "重置配网信息"                 },
    {   ePrivDrvParamAddr008,       NULL,                       "网络硬件类型"                 },
    {   ePrivDrvParamAddr009,       NULL,                       "Wifi信息"                     },
    {   ePrivDrvParamAddr010,       NULL,                       "字符型KEY"                    },
    {   ePrivDrvParamAddr011,       NULL,                       "整  型KEY"                    },
    {   ePrivDrvParamAddr012,       NULL,                       "蓝牙MAC地址"                  },
    {   ePrivDrvParamAddr013,       NULL,                       "桩模式"                       },
    {   ePrivDrvParamAddr014,       NULL,                       "服务器URL"                    },
    {   ePrivDrvParamAddr016,       NULL,                       "OCPP下发卡号"                 },
    {   ePrivDrvParamAddr017,       NULL,                       "软件版本号"                   },
    {   ePrivDrvParamAddr018,       sPileCnGateParamProc018,    "设备重启"                     },
    {   ePrivDrvParamAddr020,       NULL,                       "网关设备类型"                 },
    {   ePrivDrvParamAddr026,       NULL,                       "桩设备类型"                   },
    {   ePrivDrvParamAddr027,       NULL,                       "开始充电"                     },
    {   ePrivDrvParamAddr028,       NULL,                       "停止充电"                     },
    {   ePrivDrvParamAddr029,       NULL,                       "校正电流系数"                 },
    {   ePrivDrvParamAddr030,       NULL,                       "校正电压系数"                 },
    {   ePrivDrvParamAddr031,       NULL,                       "ATE操作"                      },
    {   ePrivDrvParamAddr035,       NULL,                       "采样电流偏置值"               },
    {   ePrivDrvParamAddr038,       NULL,                       "接地检测开关"                 },
    {   ePrivDrvParamAddr041,       NULL,                       "清除数据"                     },
    {   ePrivDrvParamAddr058,       NULL,                       "ATE数据"                      },
    {   ePrivDrvParamAddr062,       NULL,                       "二维码(已弃用,建议用95)"      },
    {   ePrivDrvParamAddr063,       NULL,                       "ICCID号码"                    },
    {   ePrivDrvParamAddr064,       NULL,                       "网络状态"                     },
    {   ePrivDrvParamAddr065,       NULL,                       "计量芯片电量值"               },
    {   ePrivDrvParamAddr066,       NULL,                       "计量芯片功率值"               },
    {   ePrivDrvParamAddr067,       sPileCnGateParamProc067,    "设备编码"                     },
    {   ePrivDrvParamAddr068,       NULL,                       "计量芯片的电压和电流"         },
    {   ePrivDrvParamAddr069,       NULL,                       "断电标志"                     },
    {   ePrivDrvParamAddr070,       NULL,                       "计量芯片小信号功率值"         },
    {   ePrivDrvParamAddr071,       NULL,                       "计量芯片GP1值"                },
    {   ePrivDrvParamAddr072,       NULL,                       "计量芯片Kp值"                 },
    {   ePrivDrvParamAddr073,       NULL,                       "计量芯片HFCONST"              },
    {   ePrivDrvParamAddr078,       sPileCnGateParamProc078,    "设置直流桩输出电压"           },
    {   ePrivDrvParamAddr079,       sPileCnGateParamProc079,    "设置直流桩输出电流"           },
    {   ePrivDrvParamAddr080,       sPileCnGateParamProc080,    "开关使能"                     },
    
    {   ePrivDrvParamAddr081,       NULL                   ,    "自定义1"                      },
    {   ePrivDrvParamAddr082,       NULL,                       "自定义2"                      },
    {   ePrivDrvParamAddr083,       NULL,                       "自定义3"                      },
    {   ePrivDrvParamAddr084,       NULL,                       "自定义4"                      },
    {   ePrivDrvParamAddr085,       NULL,                       "自定义5"                      },
    {   ePrivDrvParamAddr093,       NULL,                       "延时启动开关"                 },
    {   ePrivDrvParamAddr094,       NULL,                       "SIM 卡参数设置"               },
    {   ePrivDrvParamAddr095,       NULL,                       "二维码"                       },
    {   ePrivDrvParamAddr096,       NULL,                       "保护开关配置"                 },
    {   ePrivDrvParamAddr097,       NULL,                       "桩出厂配置参数1"              },
    {   ePrivDrvParamAddr098,       NULL,                       "桩出厂配置参数2"              },
    {   ePrivDrvParamAddr099,       NULL,                       "模块软件版本号"               },
    {   ePrivDrvParamAddr100,       NULL,                       "外侧(电池)电压校准"           },
    {   ePrivDrvParamAddr101,       NULL,                       "绝缘检测"                     },
    {   ePrivDrvParamAddr102,       NULL,                       "VIN码列表"                    },
    {   ePrivDrvParamAddr103,       NULL,                       "直流输出电压校正"             },
    {   ePrivDrvParamAddr104,       NULL,                       "直流输出电流校正"             },
    {   ePrivDrvParamAddr105,       NULL,                       "直流输出功率校正"             },
    {   ePrivDrvParamAddr106,       NULL,                       "最大电流"                     },
    {   ePrivDrvParamAddr107,       NULL,                       "静音功能开关"                 },
    {   ePrivDrvParamAddr108,       NULL,                       "功率百分比"                   },
    {   ePrivDrvParamAddr109,       NULL,                       "灯语模式"                     },
    {   ePrivDrvParamAddr110,       NULL,                       "网关工作模式"                 },
    {   ePrivDrvParamAddr111,       NULL,                       "灯板软件版本号"               },
    {   ePrivDrvParamAddr112,       NULL,                       "控制板软件版本号"             },
    {   ePrivDrvParamAddr113,       NULL,                       "设置三相计量芯片"             },
    {   ePrivDrvParamAddr114,       NULL,                       "查询三相计量芯片"             },
    {   ePrivDrvParamAddr115,       NULL,                       "线损值设置"                   },
    {   ePrivDrvParamAddr116,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr117,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr118,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr119,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr120,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr121,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr122,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr149,       NULL,                       "订制产品预留字段"             },
    //以上地址由ATE平台与网关公用
    
    //以下地址由网关与桩端通讯专用
    {   ePrivDrvParamAddr150,       NULL,                       "桩禁用"                       },
    {   ePrivDrvParamAddr151,       NULL,                       "结算时机"                     },
    {   ePrivDrvParamAddr152,       NULL,                       "计费端"                       },
    {   ePrivDrvParamAddr153,       sPileCnGateParamProc153,    "费率模型"                     },
    {   ePrivDrvParamAddr154,       NULL,                       "启动模式"                     },
    {   ePrivDrvParamAddr155,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr156,       NULL,                       "Flash格式化"                  },
    {   ePrivDrvParamAddr157,       sPileCnGateParamProc157,    "触发指令"                     },
    {   ePrivDrvParamAddr158,       NULL,                       "车位锁控制"                   },
    {   ePrivDrvParamAddr159,       sPileCnGateParamProc159,    "OTA升级状态"                  },
    {   ePrivDrvParamAddr160,       NULL,                       "家庭总线最大电流"             },
    {   ePrivDrvParamAddr161,       NULL,                       "认证控制时序"                 },
    {   ePrivDrvParamAddr162,       NULL,                       "电子锁功能"                   },
    {   ePrivDrvParamAddr163,       NULL,                       "协议一致性"                   },
    {   ePrivDrvParamAddr164,       NULL,                       "负载类型"                     },
    {   ePrivDrvParamAddr165,       NULL,                       "绝缘检测"                     },
    {   ePrivDrvParamAddr166,       NULL,                       "VIN充电开关"                  },
    {   ePrivDrvParamAddr167,       sPileCnGateParamProc167,    "触发上报VIN充电"              },
    {   ePrivDrvParamAddr168,       NULL,                       "站点ID"                       },
    {   ePrivDrvParamAddr169,       NULL,                       "ESP32-WIFI模式MAC"            },
    {   ePrivDrvParamAddr170,       NULL,                       "OCPP-key"                     },
    {   ePrivDrvParamAddr171,       sPileCnGateParamProc171,    "电损率"                       },
    {   ePrivDrvParamAddr172,       NULL,                       "当前工作状态"                 },
    {   ePrivDrvParamAddr173,       NULL,                       "当前电压"                     },
    {   ePrivDrvParamAddr174,       NULL,                       "当前电流"                     },
    {   ePrivDrvParamAddr175,       NULL,                       "当前功率"                     },
    {   ePrivDrvParamAddr176,       NULL,                       "当前电表读数"                 },
    {   ePrivDrvParamAddr177,       NULL,                       "召唤命令上报"                 },
    {   ePrivDrvParamAddr178,       NULL,                       "充电中数据上报间隔"           },
    {   ePrivDrvParamAddr179,       NULL,                       "桩端遥测数据上报间隔"         },
    {   ePrivDrvParamAddr180,       NULL,                       "签到上报周期间隔"             },
    {   ePrivDrvParamAddr181,       NULL,                       "心跳上报周期间隔"             },
    {   ePrivDrvParamAddr182,       NULL,                       "心跳超时次数"                 },
    {   ePrivDrvParamAddr183,       sPileCnGateParamProc183,    "设置最大限制功率"             },
    {   ePrivDrvParamAddr188,       NULL,                       "设置/查询桩端的串口打印"      },
    {   ePrivDrvParamAddr189,       NULL,                       "解绑设备"                     },
    {   ePrivDrvParamAddr190,       NULL,                       "预留"                         },
    {   ePrivDrvParamAddr191,       NULL,                       "桩端参数设置"                 },
    {   ePrivDrvParamAddr192,       sPileCnGateParamProc192,    "自定义1:寻卡控制命令"         },
    {   ePrivDrvParamAddr193,       NULL,                       "自定义2"                      },
    {   ePrivDrvParamAddr194,       NULL,                       "自定义3"                      },
    {   ePrivDrvParamAddr195,       NULL,                       "自定义4"                      },
    {   ePrivDrvParamAddr196,       NULL,                       "自定义5"                      },
    {   ePrivDrvParamAddr198,       sPileCnGateParamProc198,    "输出电压"                     },
    {   ePrivDrvParamAddr199,       sPileCnGateParamProc199,    "输出电流"                     },
    {   ePrivDrvParamAddr200,       sPileCnGateParamProc200,    "枪温"                         },
    {   ePrivDrvParamAddr201,       sPileCnGateParamProc201,    "终止soc"                      },
    {   ePrivDrvParamAddr202,       NULL                   ,    "绝缘电阻"                      },
};





















/***************************************************************************************************
* Description                           :   参数管理命令处理
* Author                                :   Hall
* Creat Date                            :   2024-06-12
* notice                                :   
****************************************************************************************************/
bool sPileCnGateParamProc(const stPrivDrvCmdAA_t *pCmdAa)
{
    i32  i;
    char u8String[2][8] = { "查询", "设置"};
    
    for(i = 0; i < sArraySize(sPrivDrvParamMap); i++)
    {
        if(pCmdAa->eAddr == sPrivDrvParamMap[i].eAddr)
        {
            EN_SLOGI(TAG, "参数管理命令:%s--->%s", u8String[pCmdAa->eType], sPrivDrvParamMap[i].pString);
            if(sPrivDrvParamMap[i].pProc != NULL)
            {
                return sPrivDrvParamMap[i].pProc(pCmdAa);
            }
        }
    }
    
    return false;
}







//设置SN
bool sPileCnGateParamProc001(const stPrivDrvCmdAA_t *pCmdAa)
{
    bool bRst;
    ePrivDrvCmdRst_t eRst;
    
    u16  u16ParamLen = 0;
    u8   *pParam = NULL;
    u8   u8Sn[cPrivDrvLenSn + 1];
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        bRst = sEepromAppSetBlockFactorySn((const char *)pCmdAa->u8Data, pCmdAa->u16Len);
        bRst =  bRst & sEepromAppSetBlock(eEepromAppBlockFactory);
        eRst = (bRst == true) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    }
    else
    {
        bRst = sEepromAppGetBlockFactorySn((char *)u8Sn, sizeof(u8Sn));
        eRst = (bRst == true) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
        pParam = (bRst == true) ? u8Sn : NULL;
        u16ParamLen = (bRst == true) ? strlen((char *)u8Sn) : 0;
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, u16ParamLen, pParam, 0);
}







//对时---含时区
bool sPileCnGateParamProc002(const stPrivDrvCmdAA_t *pCmdAa)
{
    u8 u8Data[7];
    eTimeZone_t eTimeZone;
    stTime_t *pTime = (stTime_t *)u8Data;
    
    u16  u16ParamLen = 0;
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        memcpy(u8Data, pCmdAa->u8Data, pCmdAa->u16Len);
        sSetTime(pTime);
        u16ParamLen = pCmdAa->u16Len;
        if(u16ParamLen == 7)
        {
            eTimeZone = (eTimeZone_t)pCmdAa->u8Data[sizeof(stTime_t)];
            sRtcSetTimeZone(eTimeZone);
            sEepromAppSetBlockFactoryTimeZone(eTimeZone);
            sEepromAppSetBlock(eEepromAppBlockFactory);
        }
    }
    else
    {
       (*pTime) = sGetTime();
       u16ParamLen = sizeof(stTime_t);
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, ePrivDrvCmdRstSuccess, pCmdAa->eAddr, u16ParamLen, u8Data, 0);
}







//设备重启
bool sPileCnGateParamProc018(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    
    eRst = (pCmdAa->eType == ePrivDrvCfgCmdTypeSet) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 0, NULL, 0);
    
    switch(pCmdAa->u8Data[0])
    {
    case ePrivDrvParamGateOpSilentModeIn:
        break;
    case ePrivDrvParamGateOpSilentModeOut:
        break;
    case ePrivDrvParamGateOpReLogin:
        //无需操作 SDK-->en_private 组件内部已经处理
        break;
    case ePrivDrvParamGateOpReboot:
    case ePrivDrvParamGateOpRebootHw:
        break;
    case ePrivDrvParamGateOpBootload:
        break;
    case ePrivDrvParamGateOpRebootForce:
        break;
    case ePrivDrvParamGateOpReFlase:
        break;
    case ePrivDrvParamGateOpRebootGate:
        sIoSetGatePower(RESET, 500, SET, 500);
        break;
    default:
        break;
    }
        
    return true;
}





//设备类型
bool sPileCnGateParamProc067(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamDeviceType_t* pPrivDrvParamDeviceTypeSet = NULL;
    stPrivDrvParamDeviceType_t  stPrivDrvParamDeviceTypeGet = {0};
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        pPrivDrvParamDeviceTypeSet = (stPrivDrvParamDeviceType_t*) pCmdAa->u8Data;
        sEepromAppSetBlockFactoryDevName(pPrivDrvParamDeviceTypeSet->u8PileDeviceType, sizeof(pPrivDrvParamDeviceTypeSet->u8PileDeviceType));
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        sEepromAppGetBlockFactoryDevName(stPrivDrvParamDeviceTypeGet.u8PileDeviceType,sizeof(stPrivDrvParamDeviceTypeGet.u8PileDeviceType));
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(stPrivDrvParamDeviceType_t), (u8*)&stPrivDrvParamDeviceTypeGet, 0);
    }
}





//手动输出电压（方案没定义好暂时保留解析但不执行动作）
bool sPileCnGateParamProc078(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    u16  u16VoltSet;
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        u16VoltSet = pCmdAa->u8Data[0] | (pCmdAa->u8Data[1] << 8);
        eRst = ePrivDrvCmdRstSuccess;
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
}






//手动输出电流（方案没定义好暂时保留解析但不执行动作）
bool sPileCnGateParamProc079(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    u16  u16CurSet;
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        u16CurSet = pCmdAa->u8Data[0] | (pCmdAa->u8Data[1] << 8);
        eRst = ePrivDrvCmdRstSuccess;
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);

}







//开关使能
bool sPileCnGateParamProc080(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    u16  u16CurSet;
    stPrivDrvParamSwitch_t *pPrivDrvParamSwitchSet = NULL;
    stPrivDrvParamSwitch_t stPrivDrvParamSwitchGet = {0};
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        pPrivDrvParamSwitchSet = (stPrivDrvParamSwitch_t*) pCmdAa->u8Data;
        
        if(pPrivDrvParamSwitchSet->bDoorFlag)
        {
            sEepromAppSetBlockFactoryDoorFlag(true);
        }
        else
        {
            sEepromAppSetBlockFactoryDoorFlag(false);
        }
        
        if(pPrivDrvParamSwitchSet->bElock)
        {
            sEepromAppSetBlockFactoryElockFlag(true);
        }
        else
        {
            sEepromAppSetBlockFactoryElockFlag(false);
        }
        
        if(pPrivDrvParamSwitchSet->bIsoFlag)
        {
            sEepromAppSetBlockFactoryIsoChkFlag(true);
        }
        else
        {
            sEepromAppSetBlockFactoryIsoChkFlag(false);
        }
        
        if(pPrivDrvParamSwitchSet->bResLoadType)
        {
            sEepromAppSetBlockFactoryResFlag(true);
        }
        else
        {
            sEepromAppSetBlockFactoryResFlag(false);
        }
        
        if(pPrivDrvParamSwitchSet->bprotocolconformance)
        {
            sEepromAppSetBlockFactoryProtocolFlag(true);
        }
        else
        {
            sEepromAppSetBlockFactoryProtocolFlag(false);
        }
        
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        if(sEepromAppGetBlockFactoryDoorFlag())
        {
            stPrivDrvParamSwitchGet.bDoorFlag = true;
        }
        else
        {
            stPrivDrvParamSwitchGet.bDoorFlag = false;
        }
        
        if(sEepromAppGetBlockFactoryElockFlag())
        {
            stPrivDrvParamSwitchGet.bElock = true;
        }
        else
        {
            stPrivDrvParamSwitchGet.bElock = false;
        }
        
        if(sEepromAppGetBlockFactoryIsoChkFlag())
        {
            stPrivDrvParamSwitchGet.bIsoFlag = true;
        }
        else
        {
            stPrivDrvParamSwitchGet.bIsoFlag = false;
        }
        
        if(sEepromAppGetBlockFactoryResFlag())
        {
            stPrivDrvParamSwitchGet.bResLoadType = true;
        }
        else
        {
            stPrivDrvParamSwitchGet.bResLoadType = false;
        }
        
        if(sEepromAppGetBlockFactoryProtocolFlag())
        {
            stPrivDrvParamSwitchGet.bprotocolconformance = true;
        }
        else
        {
            stPrivDrvParamSwitchGet.bprotocolconformance = false;
        }
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(stPrivDrvParamSwitch_t), (u8*)&stPrivDrvParamSwitchGet, 0);
    }
}








//费率模型
bool sPileCnGateParamProc153(const stPrivDrvCmdAA_t *pCmdAa)
{
    bool bRst;
    ePrivDrvCmdRst_t eRst;
    
    u16  u16ParamLen = 0;
    unPrivDrvRate_t *pRate = NULL;
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        sEepromAppSetBlockOperatorRate((unPrivDrvRate_t *)pCmdAa->u8Data);
        sEepromAppSetBlock(eEepromAppBlockOperator);
        
        pRate = NULL;
        u16ParamLen = 0;
        eRst = ePrivDrvCmdRstSuccess;
    }
    else
    {
        pRate = MALLOC(sizeof(unPrivDrvRate_t));
        memset(pRate, 0, sizeof(unPrivDrvRate_t));
        bRst = sEepromAppGetBlockOperatorRate(pRate);
        
        
        u16ParamLen = sizeof(unPrivDrvRate_t);
        eRst = (bRst == true) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    }
    
    bRst = sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, u16ParamLen, (const u8 *)pRate, 0);
    
    if(pRate != NULL)
    {
        FREE(pRate);
        pRate = NULL;
    }
    
    return bRst;
}







//召唤指定报文
bool sPileCnGateParamProc157(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst = ePrivDrvCmdRstFail;
    
    
    if((pCmdAa->u8GunId > cPrivDrvGunIdChg) && (pCmdAa->eType == ePrivDrvCfgCmdTypeSet))
    {
        if(sPrivDrvSetCallCmd(pCmdAa->u8GunId, pCmdAa->u8Data[0]) == true)
        {
            eRst = ePrivDrvCmdRstSuccess;
        }
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 0, NULL, 0);
}







//网关下发OTA状态
bool sPileCnGateParamProc159(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst = ePrivDrvCmdRstFail;
    
    
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        eRst = ePrivDrvCmdRstSuccess;
        pPileCnCache->stGateData.eOtaStatus = pCmdAa->u8Data[0];
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 0, NULL, 0);
}








//vin码充电
bool sPileCnGateParamProc167(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamFindCard_t *pFindCard = NULL;
    
    eRst = (pCmdAa->eType == ePrivDrvCfgCmdTypeSet) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    if((pCmdAa->u8GunId > cPrivDrvGunIdChg) && (pCmdAa->eType == ePrivDrvCfgCmdTypeSet))
    {
        pFindCard = (stPrivDrvParamFindCard_t *)pCmdAa->u8Data;
        sVinAppStart(pCmdAa->u8GunId - cPrivDrvGunIdBase, ePrivDrvChargingModeFull, 0);
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 0, NULL, 0);
}







//电损率
bool sPileCnGateParamProc171(const stPrivDrvCmdAA_t *pCmdAa)
{
    u16  u16Vlue;
    ePrivDrvCmdRst_t eRst;
    
    eRst = ePrivDrvCmdRstFail;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        u16Vlue = MAKE_UINT16(pCmdAa->u8Data[1], pCmdAa->u8Data[0]);
        if((u16Vlue >= 1000) && (u16Vlue <= 1100))
        {
            eRst = ePrivDrvCmdRstSuccess;
            sOrderSetEleLoss(u16Vlue);
            sEepromAppSetBlockOperatorEleLoss(u16Vlue);
            sEepromAppSetBlock(eEepromAppBlockOperator);
        }
    }
    else
    {
        eRst = ePrivDrvCmdRstSuccess;
        u16Vlue = sEepromAppGetBlockOperatorEleLoss();
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(u16Vlue), (const u8 *)&u16Vlue, 0);
}








//最大限制功率
bool sPileCnGateParamProc183(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst; 
    f32 f32PdcMax;
    u32 u32PdcMax;
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        u32PdcMax =MAKE_UINT32( pCmdAa->u8Data[3], pCmdAa->u8Data[2], pCmdAa->u8Data[1], pCmdAa->u8Data[0]);
        f32PdcMax = (f32)u32PdcMax;
        sEepromAppSetBlockFactoryPowerMax(f32PdcMax);
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        u32PdcMax = (u32)sEepromAppGetBlockFactoryPowerMax();
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 4, (u8*)&u32PdcMax, 0);
    }
}












//寻卡控制
bool sPileCnGateParamProc192(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamFindCard_t *pFindCard = NULL;
    
    eRst = (pCmdAa->eType == ePrivDrvCfgCmdTypeSet) ? ePrivDrvCmdRstSuccess : ePrivDrvCmdRstFail;
    if((pCmdAa->u8GunId > cPrivDrvGunIdChg) && (pCmdAa->eType == ePrivDrvCfgCmdTypeSet))
    {
        pFindCard = (stPrivDrvParamFindCard_t *)pCmdAa->u8Data;
        if(pFindCard->u16CardAllowt)
        {
            sCardAppStartForStart(pCmdAa->u8GunId - cPrivDrvGunIdBase, pFindCard->eMode, pFindCard->unParam.u32Value);
        }
        else
        {
            sCardAppStop();
        }
    }
    
    return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, 0, NULL, 0);
}










//输出电压
bool sPileCnGateParamProc198(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamUdcOut_t *pPrivDrvParamUdcOutSet = NULL;
    stPrivDrvParamUdcOut_t stPrivDrvParamUdcOutGet = {0};
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        pPrivDrvParamUdcOutSet = (stPrivDrvParamUdcOut_t*) pCmdAa->u8Data;
        sEepromAppSetBlockFactoryUdcOutMax((u16)pPrivDrvParamUdcOutSet->u32UdcMax);
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        stPrivDrvParamUdcOutGet.u32UdcMax = (u32)sEepromAppGetBlockFactoryUdcOutMax();
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(stPrivDrvParamUdcOut_t), (u8*)&stPrivDrvParamUdcOutGet, 0);
    }
}





//输出电流
bool sPileCnGateParamProc199(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamIdcOut_t *pPrivDrvParamIdcOutSet = NULL;
    stPrivDrvParamIdcOut_t stPrivDrvParamIdcOutGet = {0};
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        pPrivDrvParamIdcOutSet = (stPrivDrvParamIdcOut_t*) pCmdAa->u8Data;
        sEepromAppSetBlockFactoryIdcOutMax((u16)pPrivDrvParamIdcOutSet->u32IdcMax);
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        stPrivDrvParamIdcOutGet.u32IdcMax = (u32)sEepromAppGetBlockFactoryIdcOutMax();
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(stPrivDrvParamIdcOut_t), (u8*)&stPrivDrvParamIdcOutGet, 0);
    }
}







//枪温
bool sPileCnGateParamProc200(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    stPrivDrvParamGunTemp_t *pPrivDrvParamGunTempSet = NULL;
    stPrivDrvParamGunTemp_t stPrivDrvParamGunTempGet = {0};
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        pPrivDrvParamGunTempSet = (stPrivDrvParamGunTemp_t*) pCmdAa->u8Data;
        sEepromAppSetBlockFactoryTgunHighMax((u8)pPrivDrvParamGunTempSet->u16TgunHighMax);
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        stPrivDrvParamGunTempGet.u16TgunHighMax = (u16)sEepromAppGetBlockFactoryTgunHighMax();
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(stPrivDrvParamGunTemp_t), (u8*)&stPrivDrvParamGunTempGet, 0);
    }
}




//停冲soc
bool sPileCnGateParamProc201(const stPrivDrvCmdAA_t *pCmdAa)
{
    ePrivDrvCmdRst_t eRst;
    u8               u8SocMax;
    
    eRst = ePrivDrvCmdRstSuccess;
    if(pCmdAa->eType == ePrivDrvCfgCmdTypeSet)
    {
        u8SocMax = pCmdAa->u8Data[0];
        sEepromAppSetBlockFactorySocMax(u8SocMax);
        sEepromAppSetBlock(eEepromAppBlockFactory);
        
        return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, pCmdAa->u16Len, pCmdAa->u8Data, 0);
    }
    else
    {
        u8SocMax = sEepromAppGetBlockFactorySocMax();
        
       return sPrivDrvSetCmdParam(false, pCmdAa->u8GunId, pCmdAa->eType, eRst, pCmdAa->eAddr, sizeof(u8), &u8SocMax, 0);
    }
}

