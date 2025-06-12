/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_para.h
* Description                           :     EN+ 内网私有协议驱动 实现
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-02
* notice                                :     参数地址表及参数字段结构定义
*
*                                             此文件由内网协议与ATE协议公用
****************************************************************************************************/
#ifndef _private_drv_para_H_
#define _private_drv_para_H_
#include "en_common.h"
#include "private_drv_data_def.h"







//参数地址表定义
typedef enum
{
    ePrivDrvParamAddr001                = 1,                                    //SN号
    ePrivDrvParamAddr002                = 2,                                    //对时
    ePrivDrvParamAddr003                = 3,                                    //服务器IP
    ePrivDrvParamAddr004                = 4,                                    //服务器PORT
    ePrivDrvParamAddr005                = 5,                                    //服务器PATH
    ePrivDrvParamAddr006                = 6,                                    //OCPP是否加密
    ePrivDrvParamAddr007                = 7,                                    //重置配网信息
    ePrivDrvParamAddr008                = 8,                                    //网络硬件类型
    ePrivDrvParamAddr009                = 9,                                    //Wifi信息
    ePrivDrvParamAddr010                = 10,                                   //字符型KEY
    ePrivDrvParamAddr011                = 11,                                   //整  型KEY
    ePrivDrvParamAddr012                = 12,                                   //蓝牙MAC地址
    ePrivDrvParamAddr013                = 13,                                   //桩模式
    ePrivDrvParamAddr014                = 14,                                   //服务器URL
    ePrivDrvParamAddr016                = 16,                                   //OCPP下发卡号
    ePrivDrvParamAddr017                = 17,                                   //软件版本号
    ePrivDrvParamAddr018                = 18,                                   //设备重启
    ePrivDrvParamAddr020                = 20,                                   //网关设备类型
    ePrivDrvParamAddr026                = 26,                                   //桩设备类型(型号代码)
    ePrivDrvParamAddr027                = 27,                                   //开始充电
    ePrivDrvParamAddr028                = 28,                                   //停止充电
    ePrivDrvParamAddr029                = 29,                                   //校正电流系数
    ePrivDrvParamAddr030                = 30,                                   //校正电压系数
    ePrivDrvParamAddr031                = 31,                                   //ATE操作
    ePrivDrvParamAddr035                = 35,                                   //采样电流偏置值
    ePrivDrvParamAddr038                = 38,                                   //接地检测开关
    ePrivDrvParamAddr041                = 41,                                   //清除数据
    ePrivDrvParamAddr058                = 58,                                   //ATE数据
    ePrivDrvParamAddr062                = 62,                                   //二维码
    ePrivDrvParamAddr063                = 63,                                   //ICCID号码
    ePrivDrvParamAddr064                = 64,                                   //网络状态
    ePrivDrvParamAddr065                = 65,                                   //计量芯片电量值
    ePrivDrvParamAddr066                = 66,                                   //Get:计量芯片功率值           Set:外接设备参数
    ePrivDrvParamAddr067                = 67,                                   //设备编码
    ePrivDrvParamAddr068                = 68,                                   //Get:计量芯片的电压和电流     Set:计量芯片HFCONST值
    ePrivDrvParamAddr069                = 69,                                   //Get:断电标志                 Set:计量芯片GP1值
    ePrivDrvParamAddr070                = 70,                                   //Get:计量芯片小信号功率值     Set:计量芯片Kp值
    ePrivDrvParamAddr071                = 71,                                   //Get:计量芯片GP1值            Set:计量芯片KI值和KU 值
    ePrivDrvParamAddr072                = 72,                                   //Get:计量芯片Kp值             Set:无
    ePrivDrvParamAddr073                = 73,                                   //Get:计量芯片HFCONST          Set:电能表功率
    ePrivDrvParamAddr078                = 78,                                   //设置直流桩输出电压
    ePrivDrvParamAddr079                = 79,                                   //设置直流桩输出电流
    ePrivDrvParamAddr080                 =80,                                   //开关使能
    
    ePrivDrvParamAddr081                = 81,                                   //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr082                = 82,                                   //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr083                = 83,                                   //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr084                = 84,                                   //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr085                = 85,                                   //自定义-----------------------传输的内容由具体项目约定
    
    ePrivDrvParamAddr093                = 93,                                   //延时启动开关-----------------0:关闭 1: 开启
    ePrivDrvParamAddr094                = 94,                                   //SIM 卡参数设置---------------
    ePrivDrvParamAddr095                = 95,                                   //二维码
    ePrivDrvParamAddr096                = 96,                                   //保护开关配置
    ePrivDrvParamAddr097                = 97,                                   //桩出厂配置参数1--------------出厂后用户不需要更改的
    ePrivDrvParamAddr098                = 98,                                   //桩出厂配置参数2--------------后续可能用户去更改的
    ePrivDrvParamAddr099                = 99,                                   //模块软件版本号
    ePrivDrvParamAddr100                = 100,                                  //外侧(电池)电压校准
    ePrivDrvParamAddr101                = 101,                                  //绝缘检测
    ePrivDrvParamAddr102                = 102,                                  //VIN码列表
    ePrivDrvParamAddr103                = 103,                                  //直流输出电压校正
    ePrivDrvParamAddr104                = 104,                                  //直流输出电流校正
    ePrivDrvParamAddr105                = 105,                                  //直流输出功率校正
    ePrivDrvParamAddr106                = 106,                                  //最大电流
    ePrivDrvParamAddr107                = 107,                                  //静音功能开关
    ePrivDrvParamAddr108                = 108,                                  //功率百分比
    ePrivDrvParamAddr109                = 109,                                  //灯语模式
    ePrivDrvParamAddr110                = 110,                                  //网关工作模式
    ePrivDrvParamAddr111                = 111,                                  //灯板软件版本号
    ePrivDrvParamAddr112                = 112,                                  //控制板软件版本号
    ePrivDrvParamAddr113                = 113,                                  //设置三相计量芯片
    ePrivDrvParamAddr114                = 114,                                  //查询三相计量芯片
    ePrivDrvParamAddr115                = 115,                                  //线损值设置
    ePrivDrvParamAddr116                = 116,                                  //预留
    ePrivDrvParamAddr117                = 117,                                  //预留
    ePrivDrvParamAddr118                = 118,                                  //预留
    ePrivDrvParamAddr119                = 119,                                  //预留
    ePrivDrvParamAddr120                = 120,                                  //预留
    ePrivDrvParamAddr121                = 121,                                  //预留
    ePrivDrvParamAddr122                = 122,                                  //预留
    ePrivDrvParamAddr149                = 149,                                  //订制产品预留字段
    
    //以上地址由ATE平台与网关公用
    
    //以下地址由网关与桩端通讯专用
    ePrivDrvParamAddr150                = 150,                                  //桩禁用
    ePrivDrvParamAddr151                = 151,                                  //结算时机
    ePrivDrvParamAddr152                = 152,                                  //计费端
    ePrivDrvParamAddr153                = 153,                                  //费率模型
    ePrivDrvParamAddr154                = 154,                                  //启动模式
    ePrivDrvParamAddr155                = 155,                                  //预留
    ePrivDrvParamAddr156                = 156,                                  //Flash格式化
    ePrivDrvParamAddr157                = 157,                                  //触发指令
    ePrivDrvParamAddr158                = 158,                                  //车位锁控制
    ePrivDrvParamAddr159                = 159,                                  //OTA升级状态
    ePrivDrvParamAddr160                = 160,                                  //家庭总线最大电流
    ePrivDrvParamAddr161                = 161,                                  //认证控制时序
    ePrivDrvParamAddr162                = 162,                                  //电子锁功能
    ePrivDrvParamAddr163                = 163,                                  //协议一致性
    ePrivDrvParamAddr164                = 164,                                  //负载类型
    ePrivDrvParamAddr165                = 165,                                  //绝缘检测
    ePrivDrvParamAddr166                = 166,                                  //VIN充电开关
    ePrivDrvParamAddr167                = 167,                                  //触发上报VIN充电
    ePrivDrvParamAddr168                = 168,                                  //站点ID
    ePrivDrvParamAddr169                = 169,                                  //ESP32-WIFI模式MAC
    ePrivDrvParamAddr170                = 170,                                  //OCPP-key
    ePrivDrvParamAddr171                = 171,                                  //电损率
    ePrivDrvParamAddr172                = 172,                                  //当前工作状态
    ePrivDrvParamAddr173                = 173,                                  //当前电压
    ePrivDrvParamAddr174                = 174,                                  //当前电流
    ePrivDrvParamAddr175                = 175,                                  //当前功率
    ePrivDrvParamAddr176                = 176,                                  //当前电表读数
    ePrivDrvParamAddr177                = 177,                                  //召唤命令上报
    ePrivDrvParamAddr178                = 178,                                  //充电中数据上报间隔
    ePrivDrvParamAddr179                = 179,                                  //桩端遥测数据上报间隔
    ePrivDrvParamAddr180                = 180,                                  //签到上报周期间隔
    ePrivDrvParamAddr181                = 181,                                  //心跳上报周期间隔
    ePrivDrvParamAddr182                = 182,                                  //心跳超时次数
    ePrivDrvParamAddr183                = 183,                                  //设置最大限制功率
    ePrivDrvParamAddr188                = 188,                                  //设置/查询桩端的串口打印
    ePrivDrvParamAddr189                = 189,                                  //解绑设备
    ePrivDrvParamAddr190                = 190,                                  //预留
    ePrivDrvParamAddr191                = 191,                                  //桩端参数设置
    
    ePrivDrvParamAddr192                = 192,                                  //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr193                = 193,                                  //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr194                = 194,                                  //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr195                = 195,                                  //自定义-----------------------传输的内容由具体项目约定
    ePrivDrvParamAddr196                = 196,                                  //自定义-----------------------传输的内容由具体项目约定
    
    ePrivDrvParamAddr198                = 198,                                  //输出电压
    ePrivDrvParamAddr199                = 199,                                  //输出电流
    ePrivDrvParamAddr200                = 200,                                  //枪温
    ePrivDrvParamAddr201                = 201,                                  //终止soc
    ePrivDrvParamAddr202                = 202,                                  //绝缘电阻
    
    ePrivDrvParamAddrMax                = 0xffff                                //确保占2个字节
}__attribute__((packed)) ePrivDrvParamAddr_t;










//参数地址表--->部分参数内容长度
#define cPrioDrvLenParamSn              (32)                                    //SN
#define cPrioDrvLenParamData            (512)                                   //参数内容的长度最大值
#define cPrivDrvLenParamHostIp          (64)                                    //服务器IP
#define cPrivDrvLenParamHostPath        (100)                                   //服务器PATH
#define cPrivDrvLenParamWifi            (64)                                    //Wifi信息
#define cPrivDrvLenParamMac             (6)                                     //MAC地址
#define cPrivDrvLenParamHostUrl         (256)                                   //服务器URL
#define cPrivDrvLenParamQrcode          (256)                                   //二维码
#define cPrivDrvLenParamIccid           (20)                                    //ICCID号码
#define cPrivDrvLenParamVinMax          (10)                                    //VIN码最大条数
#define cPrivDrvLenParamVinCode         (17)                                    //VIN码长度



//参数地址表:6--->OCPP是否加密 字段值定义
typedef enum
{
    ePrivDrvParamOcppWsTypeWs           = 0x00,                                 //不加密（WS）
    ePrivDrvParamOcppWsTypeWss          = 0x01,                                 //  加密（WSS）
    
    ePrivDrvParamOcppWsTypeMax
}__attribute__((packed)) ePrivDrvParamOcppWsType_t;



//参数地址表:7--->重置配网信息 字段值定义
typedef enum
{
    ePrivDrvParamResetOpFormat          = 0x01,                                 //格式化文件系统
    ePrivDrvParamResetOpClearNvs        = 0x02,                                 //清除网关信息
    ePrivDrvParamResetOpReset           = 0x55,                                 //重置配网信息
    
    ePrivDrvParamResetOpMax
}__attribute__((packed)) ePrivDrvParamResetOp_t;



//参数地址表:8--->网络硬件类型 字段值定义
typedef enum
{
    ePrivDrvParamNetHwTypeWifi          = 0x00,                                 //wifi
    ePrivDrvParamNetHwTypeLan           = 0x01,                                 //以太网
    ePrivDrvParamNetHwType4g            = 0x02,                                 //4G
    
    ePrivDrvParamNetHwTypeMax
}__attribute__((packed)) ePrivDrvParamNetHwType_t;



//参数地址表:10/11--->字符型&整型KEY 字段值定义
#define cPrivDrvLenParamKeyName         64                                      //键名长度
#define cPrivDrvLenParamKeyValue        100                                     //键值长度
typedef struct 
{
    u8                                  u8Name[cPrivDrvLenParamKeyName];        //键名
    u8                                  u8Value[cPrivDrvLenParamKeyValue];      //键值
}__attribute__((packed)) stPrivDrvParamKey_t;






//参数地址表:16--->OCPP下发卡号 字段值定义
#define cPrivDrvLenParamOcppCard        20                                      //卡号
#define cPrivDrvLenParamOcppCardNum     5                                       //卡数量
typedef struct 
{
    u8                                  u8Card[cPrivDrvLenParamOcppCardNum][cPrivDrvLenParamOcppCard];
}__attribute__((packed)) stPrivDrvParamCard_t;



//参数地址表:17--->软件版本号 字段值定义
#define cPrivDrvLenParamSwVer           32                                      //软件版本号
typedef struct 
{
    u8                                  u8Charger[cPrivDrvLenParamSwVer];       //桩端  版本号
    u8                                  u8Gate[cPrivDrvLenParamSwVer];          //网关  版本号
    u8                                  u8Mobil[cPrivDrvLenParamSwVer];         //4G模组
    u8                                  u8DcMod[cPrivDrvLenParamSwVer];         //DC模块版本号
    
}__attribute__((packed)) stPrivDrvParamSwVer_t;



//参数地址表:18--->网关操作命令 字段值定义
typedef enum
{
    ePrivDrvParamGateOpSilentModeIn     = 0x01,                                 //禁止桩主动上报任何数据(升级模式)
    ePrivDrvParamGateOpSilentModeOut    = 0x02,                                 //恢复桩数据上报(升级完成)
    ePrivDrvParamGateOpReLogin          = 0x03,                                 //桩重新签到
    ePrivDrvParamGateOpReboot           = 0x04,                                 //桩重启
    ePrivDrvParamGateOpBootload         = 0x05,                                 //桩重启进入Bootloader模式
    ePrivDrvParamGateOpRebootForce      = 0x06,                                 //强制重启(如果是充电, 需要先停充, 上报订单再重启)（软件复位）
    ePrivDrvParamGateOpReFlase          = 0x07,                                 //升级过程中掉电，桩上报该指令通知蓝牙模块再次升级。（二代家庭版）
    ePrivDrvParamGateOpRebootHw         = 0x08,                                 //硬件复位（实际应用为软件复位）
    ePrivDrvParamGateOpRebootGate       = 0x09,                                 //桩复位网关供电---以便复位网关
    
    ePrivDrvParamGateOpMax
}__attribute__((packed)) ePrivDrvParamGateOp_t;



//参数地址表:29/30--->校正电流系数 & 校正电压系数 字段值定义
typedef struct 
{
    u16                                 u16CoefA;                               //A相
    u16                                 u16CoefB;                               //B相
    u16                                 u16CoefC;                               //C相
    
}__attribute__((packed)) stPrivDrvParamCoef_t;



//参数地址表:31--->ATE操作 字段值定义
typedef enum
{
    ePrivDrvParamAteOpIn                = 0x55,                                 //开启ATE
    ePrivDrvParamAteOpOut               = 0xAA,                                 //退出ATE
    
    ePrivDrvParamAteOpMax
}__attribute__((packed)) ePrivDrvParamAteOp_t;



//参数地址表:35--->采样电流偏置值 字段值定义
typedef struct 
{
    u16                                 u16OffsetA;                             //A相
    u16                                 u16OffsetB;                             //B相
    u16                                 u16OffsetC;                             //C相
    
}__attribute__((packed)) stPrivDrvParamOffset_t;



//参数地址表:41--->清除桩的数据 字段值定义
typedef enum
{
    ePrivDrvParamClearOpRcdFault        = 0x01,                                 //开启ATE
    ePrivDrvParamClearOpRcdChg          = 0x02,                                 //退出ATE
    ePrivDrvParamClearOpRcdAll          = 0x03,                                 //退出ATE
    
    ePrivDrvParamClearOpMax
}__attribute__((packed)) ePrivDrvParamClearOp_t;



//参数地址表:94--->SIM 卡参数设置 字段值定义
#define cPrivDrvLenParamSimApn          32                                      //APN   长度
#define cPrivDrvLenParamSimUsr          64                                      //用户名长度
#define cPrivDrvLenParamSimPsd          64                                      //密  码长度

//参数地址表:94--->SIM 卡参数设置--->认证方式 字段值定义
typedef enum
{
    ePrivDrvParamSimAuthModeNull        = 0x00,                                 //无
    ePrivDrvParamSimAuthModePap         = 0x01,                                 //PAP
    ePrivDrvParamSimAuthModeChap        = 0x02,                                 //CHAP
    
    ePrivDrvParamSimAuthModeMax         = 0xFF                                  //无效
}__attribute__((packed)) ePrivDrvParamSimAuthMode_t;

typedef struct
{
    u8                                  u8Apn[cPrivDrvLenParamSimApn];          //APN
    u8                                  u8Usr[cPrivDrvLenParamSimUsr];          //用户名
    u8                                  u8Psd[cPrivDrvLenParamSimPsd];          //密码
    ePrivDrvParamSimAuthMode_t          eMode;                                  //认证方式
}__attribute__((packed)) stPrivDrvParamSimCfg_t;






//参数地址:96--->保护开关
typedef union
{
    struct
    {
        u8                              u8TempDet    :1;                        //温度检测开关
        u8                              u8VoltDet    :1;                        //电压检测开关
        u8                              u8CurrDet    :1;                        //电流检测开关
        u8                              u8FreqDet    :1;                        //频率检测开关
        u8                              u8AcleakDet  :1;                        //交流漏电检测开关
        u8                              u8DcleakDet  :1;                        //直流漏电检测开关
        u8                              u8EpoDet     :1;                        //急停检测开关
        u8                              u8RelayDet   :1;                        //继电器异常检测开关
        
        u8                              u8NeDet      :1;                        //接地检测开关
        u8                              u8PhaseDet   :1;                        //反相检测开关
        u8                              u8SelfDet    :1;                        //漏电流电路异常开关(自检开关)
        u8                              u8CtDet      :1;                        //ct异常检测开关
        u8                              u8MeterDet   :1;                        //电表通讯异常检测开关
        u8                              u8CpDet      :1;                        //cp异常检测开关
        u8                              u8LockDet    :1;                        //枪锁异常检测开关
        u8                              u8CcDet      :1;                        //枪电流异常检测开关
        
        u8                              u8ShortDet   :1;                        //短路检测开关
        u8                              u8PmeDet     :1;                        //pme检测开关
        u8                              u8DoorDet    :1;                        //拆机告警开关
        u32                             u32Rev       :13;                       //预留
    }bit;
    u32                                 u32All;
}__attribute__((packed)) unPrivDrvParamSw_t;





//参数地址:97--->桩端配置参数1
#define cPrivDrvParam1DevNameLen         24                                      //设备名称长度
#define cPrivDrvParam1KeyLen             16                                      //m1卡片扇区秘钥长度
#define cPrivDrvParam1RsvLen             26                                      //预留长度

typedef struct
{
    u8                                  u8DevName[cPrivDrvParam1DevNameLen];    //设备名称
    u8                                  u8ChargStandard;                        //选择的充电标准
    u8                                  u8LcdType;                              //屏幕类型
    u8                                  u8ChargerCnt;                           //枪数量 
    u32                                 u32RatedPower;                          //额定功率，一般出厂后配置后续不改
    u32                                 u32RatedCurr;                           //额定电流，一般出厂后配置后续不改,A
    u8                                  u8Phase;                                //相数
    
    u8                                  u8M1sector;                             //m1卡扇区读取
    u8                                  u8Key[cPrivDrvParam1KeyLen];            //m1卡片扇区秘钥
    
    u8                                  u8LightEntity;                          //灯光实体类型
    u8                                  u8LightStyle;                           //灯语类型
    
    u8                                  u8PowerMeterModule;                     //功率电表模块
    u8                                  u8MeasureMeterModule;                   //计量电表模块
    
    u16                                 u16RelayTmpProtectOn;                   //继电器温度保护点
    u16                                 u16RelayTmpProtectOff;                  //继电器温度保护恢复点
    u16                                 u16RelayTmpLowerLoad;                   //继电器温度降载点
    u16                                 u16RelayTmpRecoverLoad;                 //继电器温度降载恢复点
    
    u16                                 u16InportTmpProtectOn;                  //输入端子温度检测点
    u16                                 u16InportTmpProtectOff;                 //输入端子温度检测点
    u16                                 u16InportTmpLowerLoad;                  //输入端子温度检测点
    u16                                 u16InportTmpRecoverLoad;                //输入端子温度检测点
    
    u8                                  u8CardType;                             //卡类型
    
    u8                                  u8EmsChgType;                           //EMS协议类型
    
    u8                                  u8ChgType;                              //家庭版还是运营版
    u8                                  u8ChgVendorid;                          //供应商id
    u16                                 u8SupportMode;                          //桩支持的模式
    u8                                  u8ModbusType;                           //modbus类型,TCP或RTU
    u8                                  u8Rev[cPrivDrvParam1RsvLen];            //预留字段
}__attribute__((packed)) stPrivDrvChgParam1_t;









//参数地址:98--->功能选配开关控制bit
typedef union
{
    struct
    {
        u8                              u8CpWideEn          :1;                 //cp宽范围
        u8                              u8SecurityEn        :1;                 //网安要求(m1卡加密)
        u8                              u8DelayStart        :1;                 //延时启动开关
        u8                              u8WakeCp            :1;                 //CP唤醒开关
        u8                              u8LittleCurrStop    :1;                 //小电流停充开关
        u8                              u8FaultRcd          :1;                 //任何故障直接结算
        u8                              u8TemLoad           :1;                 //温度降载开关
        u8                              u8GridLoad          :1;                 //电压降载开关
        u8                              u8EmsEncryptEn      :1;                 //EMS是否加密
        u8                              u8WifiReset         :1;                 //是否启用wifi复位功能(通过急停按钮复位)
        u32                             u32Rev              :22;                //预留
    }bit;
    u32                                 u32All;
}__attribute__((packed)) unPrivDrvExFun_t;


//参数地址:98--->桩配置参数2
typedef struct
{
    u32                                 u32MaxPower;                            //最大功率
    u32                                 u32Maxcurr;                             //最大电流
    u8                                  u8Gridtype;                             //电网类型  0 IT     1 TT     2 TN
    u16                                 u16GridVoltMax;                         //市电过压点0.1v
    u16                                 u16GridVoltMaxBack;                     //市电过压恢复点0.1v
    u16                                 u16GridVoltMin;                         //市电欠压点0.1v
    u16                                 u16GridVoltMinBack;                     //市电欠压恢复点0.1v
    u16                                 u16DeviceAddr;                          //设备地址,一般485通讯需要
    u8                                  u8Language;                             //语言
    u8                                  u8Password[16];                         //屏幕密码
    unPrivDrvExFun_t                    uExFun;                                 //扩展功能控制 
    u8                                  u8Rev[32];                              //预留字段，如果需要添加从此处拆
}__attribute__((packed)) stPrivDrvChgCfg_t;





//参数地址:159--->OTA升级状态值定义
typedef enum
{
    ePrivDrvParamOtaIdle                = 0x00,                                 //空闲
    ePrivDrvParamOtaDown,                                                       //下载中
    ePrivDrvParamOtaDownFail,                                                   //下载失败
    ePrivDrvParamOtaInstall,                                                    //安装中
    ePrivDrvParamOtaSuccess,                                                    //升级完成
    ePrivDrvParamOtaInstallFail,                                                //安装失败
    
    ePrivDrvParamOtaMax
}__attribute__((packed)) ePrivDrvParamOta_t;















//参数地址表--->union结构定义
typedef union
{
    u8                                  u8Sn[cPrivDrvLenSn];                    //001
    stTime_t                            stTime;                                 //002
    u8                                  u8HostIp[cPrivDrvLenParamHostIp];       //003
    u32                                 u32HostPort;                            //004
    u8                                  u8HostPath[cPrivDrvLenParamHostPath];   //005
    ePrivDrvParamOcppWsType_t           eOcppWsType;                            //006
    ePrivDrvParamResetOp_t              eResetOp;                               //007
    ePrivDrvParamNetHwType_t            eNetHwType;                             //008
    u8                                  u8Wifi[cPrivDrvLenParamWifi];           //009
    stPrivDrvParamKey_t                 stKey;                                  //010 & 011
    u8                                  u8MacBle[cPrivDrvLenParamMac];          //012
    ePrivDrvChargerWorkMode_t           eWorkMode;                              //013
    u8                                  u8HostUrl[cPrivDrvLenParamHostUrl];     //014
    stPrivDrvParamCard_t                stCard;                                 //016
    stPrivDrvParamSwVer_t               stSwVer;                                //017
    ePrivDrvParamGateOp_t               eGateOp;                                //018
    u16                                 u16DevTypeGate;                         //020
    u8                                  u8DevTypeChg;                           //026
    u8                                  u8Byte;                                 //027 & 028
    stPrivDrvParamCoef_t                stCoef;                                 //029 & 030
    ePrivDrvParamAteOp_t                eAteOp;                                 //031
    stPrivDrvParamOffset_t              stOffset;                               //035
    ePrivDrvParamClearOp_t              eClearOp;                               //041
                                                                                //058---透传数据 就不定义了 很麻烦
    u8                                  u8Qrcode[cPrivDrvLenParamQrcode];       //062
    u8                                  u8Iccid[cPrivDrvLenParamIccid];         //063
    stPrivDrvParamSimCfg_t              stSimCfg;                               //094
    u8                                  u8VinCode[cPrivDrvLenParamVinMax][cPrivDrvLenParamVinCode];//102-VIN码
    u8                                  u8UnbindDev;                            //188-解绑设备
    
    //使用数组访问 便于拷贝
    u8                                  u8Data[cPrioDrvLenParamData];
}__attribute__((packed)) unPrivDrvParam_t;

















#endif


















