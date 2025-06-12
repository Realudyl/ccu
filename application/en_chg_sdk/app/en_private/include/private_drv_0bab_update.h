/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0bab_update.h
* Description                           :     充电桩端模块电源升级
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-04-01
* notice                                :     从李向前调试的代码中提取出来
****************************************************************************************************/
#ifndef _private_drv_0bab_update_H_
#define _private_drv_0bab_update_H_
#include "en_common.h"
#include "private_drv_data_def.h"





#ifdef  __cplusplus
extern "C" {
#endif


#define SENMENT_CNT_MAX                 4                                       //最多个段
#define LINE_DATA_MAX                   (32 + 1)                                //32 DATA + 1 CRC  增加模块hex文件后长度改为 32





//行数据类型
typedef enum
{
    FW_LINE_TYPE_DATA = 0,                                                      //'00' Data Rrecord：用来记录数据，HEX文件的大部分记录都是数据记录
    FW_LINE_TYPE_END,                                                           //'01' End of File Record:用来标识文件结束，放在文件的最后，标识HEX文件的结尾
    FW_LINE_TYPE_EX_SEG_ADDR,                                                   //'02' Extended Segment Address Record:用来标识扩展段地址的记录
    FW_LINE_TYPE_SEG_ADDR,                                                      //'03' Start Segment Address Record:开始段地址记录
    FW_LINE_TYPE_EX_LINE_ADDR,                                                  //'04' Extended Linear Address Record:用来标识扩展线性地址的记录
    FW_LINE_TYPE_LINE_ADDR,                                                     //'05' Start Linear Address Record:开始线性地址记录
}__attribute__((packed)) eFwLineType_t;




//行解析
typedef struct
{
    u8                                  data_len;                               //本行数据的长度
    u16                                 offset;                                 //本行数据的起始地址
    u8                                  data_type;                              //数据类型，数据类型有：0x00、0x01、0x02、0x03、0x04、0x05
    u8                                  data[LINE_DATA_MAX];                    //最多16bytes数据+1byte校验
}__attribute__((packed)) sFwHexLineInfo_t;





typedef struct
{
    u8                                  segCnt;                                 //段数量
    u8                                  boardCnt;                               //单板数量
    u8                                  segBoardCnt[SENMENT_CNT_MAX];           //段对应的单板数量
    char                                segName[SENMENT_CNT_MAX][10];           //段名== 单板名字
    u8                                  segList[SENMENT_CNT_MAX][10];           //段列表:适用的单板局部id，可以通过配置文件获取
    u32                                 segPosition[SENMENT_CNT_MAX];           //段在文件中起始位置
    u32                                 segLength[SENMENT_CNT_MAX];             //段数据
}sFwSegInto_t;





typedef struct
{
    u8                                  dev_type;                               //对应的设备类型
    char                                fw_time[32];                            //固件时间信息
    char                                sw_ver[32];                             //软件版本信息
    char                                hw_ver[32];                             //硬件版本信息
    char                                dev_name[32];                           //设备名称
}sFwHeader_t;





typedef struct 
{
    ePrivDrvUpdateCmd_t                 update_cmd;                             //操作命令
    ePrivDrvUpdateCmdRst_t              update_rst;
}sFwUpdateAck_t;





typedef struct 
{
    u16                                 target_cpu;                             //目标cpu地址
    u16                                 base_addr;                              //Flash基地址
    u16                                 offset;                                 //Flash偏移地址
    ePrivDrvUpdateCmd_t                 update_cmd;                             //操作命令
    u16                                 data_len;                               //有效数据长度 2*256
    u16                                 data_words[cPrioDrvLenUpdateData];      //有效数据包
}sFwUpdateData_t;





typedef struct 
{
    sFwHeader_t                         file_header_info;
    sFwSegInto_t                        file_segmet_info;
    sFwUpdateData_t                     update_ctrl;
    sFwUpdateAck_t                      update_ack;
    
    uint8_t                             currBoard;
    uint8_t                             currBoardIdx;                           //正在升级单板索引，"boardlist"
    uint8_t                             currSegment;                             //正在处理的段
    uint16_t                            nextBaseAddr;      
    
    FILE*                               file;
    SemaphoreHandle_t                   fw_update_sem;
}sCpUpdateFileInfo_t;





typedef enum 
{
    CHG_FW_STATE_1 = 0,
    CHG_FW_STATE_2,
    CHG_FW_STATE_3,
    CHG_FW_STATE_4,
}eChgFwState_t;









void chgFwUpdateChargerRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst);
bool chgFwUpdateIsChargerFile(const char* filename);
bool chgFwUpdateFileInstall(const char *pPath);





#ifdef __cplusplus 
}
#endif





#endif











