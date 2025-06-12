/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0cac_update.h
* Description                           :     充电桩端模块电源升级
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-04-01
* notice                                :     从李向前调试的代码中提取出来
****************************************************************************************************/
#ifndef _private_drv_0cac_update_H_
#define _private_drv_0cac_update_H_
#include "en_common.h"
#include "private_drv_data_def.h"
#include "private_drv_0bab_update.h"                                            //需要与桩端固件更新组件公用一部分资源










typedef struct
{
    u8                                  segCnt;                                 //段数量
    u8                                  boardCnt;                               //单板数量
    u8                                  segBoardCnt[SENMENT_CNT_MAX];           //段对应的单板数量
    char                                segName[SENMENT_CNT_MAX][10];           //段名== 单板名字
    u16                                 segCrc[SENMENT_CNT_MAX];                //段校验码
    u8                                  segList[SENMENT_CNT_MAX][10];           //段列表:适用的单板局部id，可以通过配置文件获取
    u32                                 segPosition[SENMENT_CNT_MAX];           //段在文件中起始位置
    u32                                 segLength[SENMENT_CNT_MAX];             //段数据
}stModFwSegInto_t;




typedef struct 
{
    u16                                 target_cpu;                             //目标cpu地址
    u16                                 base_addr;                              //Flash基地址
    u16                                 offset;                                 //Flash偏移地址
    ePrivDrvUpdateCmd_t                 update_cmd;                             //操作命令
    u16                                 segCrc;                                 //总行校验码
    u8                                  line_num;                               //有效行数
    u8                                  data_len[20];                           //每行数据有效长度
    u8                                  data_words[cPrioDrvLenUpdateData * 2];  //有效数据包
}stModFwUpdateData_t;



typedef struct 
{
    sFwHeader_t                         file_header_info;
    stModFwSegInto_t                    file_segmet_info;
    stModFwUpdateData_t                 update_ctrl;
    sFwUpdateAck_t                      update_ack;
    
    uint8_t                             currBoard;
    uint8_t                             currBoardIdx;                           //正在升级单板索引，"boardlist"
    uint8_t                             currSegment;                            //正在处理的段
    uint16_t                            nextBaseAddr;      
    
    FILE*                               file;
    SemaphoreHandle_t                   fw_update_sem;
}stModUpdateFileInfo_t;













extern void modFwUpdateModuleRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst);
extern bool modFwUpdateFileInstall(const char *pPath);







#endif












