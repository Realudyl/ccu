/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0cac_update.c
* Description                           :     充电桩端模块电源升级
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-04-01
* notice                                :     从李向前调试的代码中提取出来
****************************************************************************************************/
#include "en_common.h"
#include "private_drv_0cac.h"
#include "private_drv_0cac_update.h"




//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0cac_update";





static stModUpdateFileInfo_t *pModUpInfo = NULL;









void modFwUpdateModuleRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst);
bool modFwUpdateModuleParseHeader(const char* filename, sFwHeader_t *header, stModFwSegInto_t* segment);
bool modFwUpdateModuleSendCmd(const stModFwUpdateData_t *data);
bool modFwUpdateModuleSingleBoardHello(stModUpdateFileInfo_t *info);
bool modFwUpdateModuleSingleBoardErase(stModUpdateFileInfo_t *info);
u8   modFwUpdateModuleSingleBoardParses(stModUpdateFileInfo_t *info);
bool modFwUpdateModuleSingleBoardProgram(stModUpdateFileInfo_t *info);
bool modFwUpdateModuleUpdateFirmware(const char* filename, stModUpdateFileInfo_t *info);
bool modFwUpdateModule(const char* filename);




//<chg_update.c> 文件内的资源
//本文件是借助原有的桩端升级驱动架构,在里面增加了模块电源的升级流程
//有一部分资源需要从<chg_update.c>文件内引用 申明在下面
extern bool chgFwUpdateChargerParseLine(const char* line_str, u8 *line_info);

















void modFwUpdateModuleRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst)
{
    if(pModUpInfo)
    {       
        pModUpInfo->update_ack.update_cmd = cmd;
        pModUpInfo->update_ack.update_rst = rst;
        xSemaphoreGive(pModUpInfo->fw_update_sem);
    }
}








bool modFwUpdateModuleParseHeader(const char* filename, sFwHeader_t *header, stModFwSegInto_t* segment)
{
    if(!filename || !header || !segment)
    {
        return false;
    }
    
    int boardCnt = 0;
    char tmpbuf[128] = {0}, *result;
    int index = 0;
    int cnt;
    
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", filename);
        return false;
    }
    fseek(file, 0, SEEK_SET);
    
    index = 0;
    while(1)
    {
        if(fgets(tmpbuf, sizeof(tmpbuf), file) == NULL)
        {
            goto ERR_OUT;
        }
        
        if ((result = strchr(tmpbuf, ':')) == NULL)
        {
            continue;
        }
        
        EN_SLOGD(TAG, "readline[%d]: %s", index, tmpbuf);
        
        switch(index)
        {
            case 0://软件版本 
                strcpy(header->sw_ver, result+1);
                EN_SLOGD(TAG, "sw_ver: %s", header->sw_ver);
                break;
            case 1://设备名称
                strcpy(header->dev_name, result+1);
                EN_SLOGD(TAG, "dev_name: %s", header->dev_name);
                break;
            case 2://段信息个数
                segment->segCnt = strtol(result + 1, 0, 10); 
                EN_SLOGD(TAG, "segCnt: %d",  segment->segCnt);
                goto PARSE_SEGMET;
                break;
            default:
                break;
        }
        index ++;       
    }
    
PARSE_SEGMET:
    
    for (int i = 0; i <segment->segCnt; i++)
    {
        cnt = 3;
        while(fgets(tmpbuf, sizeof(tmpbuf), file) && --cnt)
        {
            EN_SLOGD(TAG, "readline[%d]: %s", index, tmpbuf);
            if ((result = strchr(tmpbuf, ':')) != NULL)
            {
                break;
            }
        }
        //段名称 
        strcpy(segment->segName[i], result+1);
        EN_SLOGD(TAG, "segName[%d]:%s", i, segment->segName[i]);
        
        cnt = 3;
        while(fgets(tmpbuf, sizeof(tmpbuf), file) && --cnt)
        {
            EN_SLOGD(TAG, "readline[%d]: %s", index, tmpbuf);
            if ((result = strchr(tmpbuf, ':')) != NULL)
            {
                break;
            }
        }
        //段校验码
        segment->segCrc[i] = atoi(result+1);
        EN_SLOGD(TAG, "Device CRC[%d]:%d", i, segment->segCrc[i]);
        
        //板id
        cnt = 3;
        while(fgets(tmpbuf, sizeof(tmpbuf), file) && --cnt)
        {
            EN_SLOGD(TAG, "readline[%d]: %s", index, tmpbuf);
            if ((result = strchr(tmpbuf, ':')) != NULL)
            {
                break;
            }
        }
        
        u8 idx = 0;
        char* pt = strchr(tmpbuf, ':') + 1;
        result = strtok(pt, ",");
        
        while(result)
        {
            segment->segList[i][idx] = atoi(result);
            EN_SLOGD(TAG, "segList[%d][%d]:%d", i, idx, segment->segList[i][idx]);
            idx++;
            result = strtok( NULL, ",");
        }
        
        segment->segBoardCnt[i] = idx;
        boardCnt += idx;
        EN_SLOGD(TAG, "segBoardCnt[%d]:%d", i, segment->segBoardCnt[i]);
    }
    segment->boardCnt = boardCnt;
    EN_SLOGD(TAG, "boardCnt:%d", segment->boardCnt);
    
    if(file != NULL)
    {
        fclose(file);
        file = NULL;
    }
    return true;
    
ERR_OUT:
    if(file != NULL)
    {
        fclose(file);
        file = NULL;
    }
    return false;
}








bool modFwUpdateModuleSendCmd(const stModFwUpdateData_t *data)
{
    if(!data)
    {
        return false;
    }
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    //Master 端才有这个函数
    sPrivDrvPktSendAC(data->target_cpu, data->base_addr, data->offset, data->update_cmd, data->segCrc, data->data_words, (u8*)(data->data_len));
#endif
    
    return true;
}






bool modFwUpdateModuleSingleBoardHello(stModUpdateFileInfo_t *info)
{
    bool res = false;
    int  repeat = 3;
    eChgFwState_t step = CHG_FW_STATE_1;
    
    EN_SLOGD(TAG, "dcmkFwUpdateChargerSingleBoardHello1111111");
    
    //clear sem
    xSemaphoreTake(info->fw_update_sem, 0);
    
    EN_SLOGD(TAG, "dcmkFwUpdateChargerSingleBoardHello222222");
    while (1)
    {
        switch(step)
        {
        case CHG_FW_STATE_1: //发送握手包
            EN_SLOGD(TAG, "send update_hello to charger");
            info->update_ctrl.update_cmd = ePrivDrvUpdateCmdHandshake;
            modFwUpdateModuleSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_2;                 
            break;
        case CHG_FW_STATE_2: //等待握手成功
            EN_SLOGD(TAG, "wait for hello ack from charger 10s");
            if(xSemaphoreTake(info->fw_update_sem, 20000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdHandshake))
                {
                    res = true;
                    goto EXIT;
                }
                else if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstFail)
                     && (info->update_ack.update_cmd == ePrivDrvUpdateCmdFlash))
                {
                    res = false;
                    EN_SLOGD(TAG, "rsv updata fail !!!");
                    goto EXIT;
                }
            }
            
            EN_SLOGE(TAG, "wait for ack from cp timeout or rst:%d", info->update_ack.update_rst);
            
            //重发次数
            if (repeat > 0)
            {
                repeat --;
                step = CHG_FW_STATE_1;
                EN_SLOGI(TAG, "wait for ack from cp timeout , retry %d", 3-repeat);
            }
            else
            {
                // to do :再次发送握手命名，使单板进入boot模式
                res = false;
                EN_SLOGI(TAG, "wait for ack from cp timeout , retry 3 time but not work");
                goto EXIT;
            }
            break;
        default:
            break;
        }
    }
    
EXIT:
     return res;
}








bool modFwUpdateModuleSingleBoardErase(stModUpdateFileInfo_t *info)
{
    bool res = false;
    int  repeat = 3;
    eChgFwState_t step = CHG_FW_STATE_1;
    
    //clear sem
    xSemaphoreTake(info->fw_update_sem, 0);
    while (1)
    {
        switch(step)
        {
        case CHG_FW_STATE_1: //发送握手包
            EN_SLOGD(TAG, "send ERASE to charger");
            info->update_ctrl.update_cmd = ePrivDrvUpdateCmdErase;
            modFwUpdateModuleSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_2;                 
            break;
        case CHG_FW_STATE_2: //等待擦除成功
            EN_SLOGD(TAG, "wait for ack from charger 20s");
            if(xSemaphoreTake(info->fw_update_sem, 20000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdErase))
                {
                    res = true;
                    goto EXIT;
                }
                else if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstFail)
                     && (info->update_ack.update_cmd == ePrivDrvUpdateCmdFlash))
                {
                    res = false;
                    EN_SLOGD(TAG, "rsv updata fail !!!");
                    goto EXIT;
                }
            }
            
            EN_SLOGE(TAG, "wait for ack from cp timeout or rst:%d", info->update_ack.update_rst);
            
            //重发次数
            if (repeat > 0)
            {
                repeat --;
                step = CHG_FW_STATE_1;
                EN_SLOGI(TAG, "wait for ack from cp timeout , retry %d", 3-repeat);
            }
            else 
            {
                // to do :再次发送握手命名，使单板进入boot模式
                res = false;
                EN_SLOGI(TAG, "wait for ack from cp timeout , retry 3 time but not work");
                goto EXIT;
            }
            break;
        default:
            break;
        }
    }
    
EXIT:
     return res;
}






u8 modFwUpdateModuleSingleBoardParses(stModUpdateFileInfo_t *info)
{
    char tmpbuf[128] = {0};
    sFwHexLineInfo_t tmpLine;
    u16 len = 0;
    u8  num = 0;
    u16 base_addr;
    
    if (info->nextBaseAddr != 0)  // 地址发生了跳变，重新设置基地址
    {
        info->update_ctrl.base_addr = info->nextBaseAddr;
        info->nextBaseAddr = 0;
        EN_SLOGE(TAG, "nextBaseAddr_change !!!");
    }
    
    memset(&info->update_ctrl.data_len,   0, sizeof(info->update_ctrl.data_len  ));
    memset(&info->update_ctrl.data_words, 0, sizeof(info->update_ctrl.data_words));
    for(num = 0; num < 20; num++)
    {  
        info->update_ctrl.data_len[num] = 0;
        
        do
        {
            //读取一行
            if(!fgets((char*)tmpbuf, sizeof(tmpbuf), info->file))
            {
                EN_SLOGE(TAG, "SingleBoardParses fgets error!");
                return 0;
            }
            EN_SLOGD(TAG, "SingleBoardParses ParseLine:%s", tmpbuf);
            
            if(tmpbuf[0] != ':')
            {
                continue;
            }
            
            
            if(chgFwUpdateChargerParseLine(tmpbuf, (u8*)&tmpLine))
            {
                switch(tmpLine.data_type)
                {
                case FW_LINE_TYPE_DATA:
                    //记录起始偏移地址
                    if(info->update_ctrl.data_len[num] == 0)
                    {
                        info->update_ctrl.offset = BigLittleSwap16(tmpLine.offset);
                    }
                    
                    info->update_ctrl.data_words[0 + len] = ':';
                    memcpy((u8*)&(info->update_ctrl.data_words[1 + len]), &tmpLine.data_len, (tmpLine.data_len + 5));
                    info->update_ctrl.data_len[num] = tmpLine.data_len + 6;
                    len += tmpLine.data_len + 6;
                    break;
                case FW_LINE_TYPE_EX_LINE_ADDR:     //扩展地址
                    base_addr = tmpLine.data[0] << 8 | tmpLine.data[1];
                    
                    if (!info->update_ctrl.data_len[num])
                    {
                        info->update_ctrl.base_addr = base_addr;
                        EN_SLOGD(TAG, "FW_LINE_TYPE_EX_LINE_ADDR -> data_len:%d  base_addr:%04X nextBaseAddr:%04X", info->update_ctrl.data_len[num], info->update_ctrl.base_addr,  info->nextBaseAddr);
                        
                        info->update_ctrl.data_words[0 + len] = ':';
                        memcpy((u8*)&(info->update_ctrl.data_words[1 + len]), &tmpLine.data_len, (tmpLine.data_len + 5));
                        info->update_ctrl.data_len[num] = tmpLine.data_len + 6;
                        len += tmpLine.data_len + 6;
                    }
                    else
                    {
                        info->nextBaseAddr += base_addr;
                        
                        EN_SLOGD(TAG, "FW_LINE_TYPE_EX_LINE_ADDR OUT -> data_len:%d  offset:%04X nextBaseAddr:%04X", info->update_ctrl.data_len[num], info->update_ctrl.offset,  info->nextBaseAddr);
                        
                        info->update_ctrl.data_words[0 + len] = ':';
                        memcpy((u8*)&(info->update_ctrl.data_words[1 + len]), &tmpLine.data_len, (tmpLine.data_len + 5));
                        info->update_ctrl.data_len[num] = tmpLine.data_len + 6;
                        len += tmpLine.data_len + 6;
                    }
                    break;
                case FW_LINE_TYPE_LINE_ADDR:     //下一行,不作解析
                    continue;
                case FW_LINE_TYPE_END:   //文件结束,记录下一段position, 单板升级完成
                    if ((info->currSegment + 1) < SENMENT_CNT_MAX)
                    {
                        info->file_segmet_info.segPosition[info->currSegment + 1] = ftell(info->file);
                        EN_SLOGD(TAG, "FW_LINE_TYPE_END -> data_len:%d  offset:%04X nextBaseAddr:%04X", info->update_ctrl.data_len[num], info->update_ctrl.offset,  info->nextBaseAddr);
                        
                    }
                    info->update_ctrl.data_words[0 + len] = ':';
                    memcpy((u8*)&(info->update_ctrl.data_words[1 + len]), &tmpLine.data_len, (tmpLine.data_len + 5));
                    info->update_ctrl.data_len[num] = tmpLine.data_len + 6;
                    len += tmpLine.data_len + 6;
                    
                    return 0xFF;
                default:
                    break;
                }
                
                if(info->update_ctrl.data_len[num] > 0)
                {
                    break;
                }
            }
            else
            {
                EN_SLOGD(TAG, "SingleBoardParses ParseLine faults:%s", tmpbuf);
            }
        }
        while (1);
    }
    
    EN_SLOGD(TAG, "SingleBoardParses out with 0; data_len:%d %d %d %d %d %d %d %d %d %d"
            ,info->update_ctrl.data_len[0], info->update_ctrl.data_len[1], info->update_ctrl.data_len[2]
            ,info->update_ctrl.data_len[3], info->update_ctrl.data_len[4], info->update_ctrl.data_len[5]
            ,info->update_ctrl.data_len[6], info->update_ctrl.data_len[7], info->update_ctrl.data_len[8]
            ,info->update_ctrl.data_len[9]);
    
    return 1;
}








bool modFwUpdateModuleSingleBoardProgram(stModUpdateFileInfo_t *info)
{
    bool res = false;
    int  repeat = 3;
    eChgFwState_t step = CHG_FW_STATE_1;
    u8 pkgIsOk;
    
    fseek(info->file, info->file_segmet_info.segPosition[info->currSegment], SEEK_SET);
    
    //clear sem
    xSemaphoreTake(info->fw_update_sem,  0);
    
    while (1)
    {
        switch(step)
        {
        case CHG_FW_STATE_1: //准备数据包
            //2022.3.2 修改为读取十行
            pkgIsOk = modFwUpdateModuleSingleBoardParses(info);
            EN_SLOGD(TAG, "parse segment: pkgIsOk:%02X ", pkgIsOk);
            
            if(pkgIsOk)
            {
                if(info->update_ctrl.data_len[0] != 0)
                {
                    step = CHG_FW_STATE_2;
                }
                else if(pkgIsOk == 0xFF)
                {
                    res = true;
                    EN_SLOGD(TAG, "pkgIsOk:%02X  read finished",pkgIsOk);
                    goto EXIT;
                }
            }
            else
            {
                res = false;
                goto EXIT;
            }
            break;
        case CHG_FW_STATE_2://发送数据              
            EN_SLOGD(TAG, "send single pkg to charger");
            info->update_ctrl.update_cmd = ePrivDrvUpdateCmdFlash;
            
            modFwUpdateModuleSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_3;                 
            break;
        case CHG_FW_STATE_3: //等待应答
            EN_SLOGD(TAG, "wait for ack from charger 5s");
            if(xSemaphoreTake(info->fw_update_sem, 30000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdFlash))
                {
                    if(pkgIsOk == 0xFF)
                    {
                        res = true;
                        EN_SLOGD(TAG, "pkgIsOk:%02X  read finished  wait 5s",pkgIsOk);
                        //2022.3.5 第一个主板升级成功后，等待10s再次握手
                        vTaskDelay(5*1000 / portTICK_RATE_MS);
                        goto EXIT;
                    }
                    else
                    {
                         step = CHG_FW_STATE_1;
                         continue;
                    }
                }
                else if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstFail)
                     && (info->update_ack.update_cmd == ePrivDrvUpdateCmdFlash))
                {
                        res = false;
                        EN_SLOGD(TAG, "rsv updata fail !!!");
                        goto EXIT;
                }
            }
            
            EN_SLOGE(TAG, "wait for ack from cp timeout or rst:%d", info->update_ack.update_rst);
            
            //重发次数
            if (repeat > 0)
            {
                repeat --;
                step = CHG_FW_STATE_2;
                EN_SLOGI(TAG, "wait for ack from cp timeout , retry %d", 3-repeat);
            }
            else 
            {
                // to do :再次发送握手命名，使单板进入boot模式
                res = false;
                EN_SLOGE(TAG, "wait for ack from cp timeout , retry 3 time but not work");
                goto EXIT;
            }
            break;
        default:
            break;
        }
    }
    
EXIT:
     return res;
}






bool modFwUpdateModuleUpdateFirmware(const char* filename, stModUpdateFileInfo_t *info)
{
    if(!info || !filename)
    {
        return false;
    }
    
    int retryTimer = 2, i;//5 只升级一次，失败立刻返回平台失败
    
ERR_RETRY:
    if(--retryTimer <= 0)
    {
        goto ERR_OUT;
    }
    
    memset(&info->update_ctrl, 0, sizeof(info->update_ctrl));
    
    //打开文件
    if((info->file = fopen(filename, "r")) == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", filename);
        goto ERR_OUT;
    }
    fseek(info->file, 0, SEEK_SET);
    EN_SLOGD(TAG, "open file :%s@%p,%p ok!", filename, info, info->file);
    
    //记录段的起始位置
    info->file_segmet_info.segPosition[0] = ftell(info->file);
    
    
    //打开文件升级
    for(info->currSegment = 0 ; info->currSegment < info->file_segmet_info.segCnt ; info->currSegment++)
    {
        i = info->currSegment;
        EN_SLOGD(TAG, "start update rrrrrr currSegment %d", info->currSegment);  
        for (info->currBoardIdx = 0; info->currBoardIdx < info->file_segmet_info.segBoardCnt[i]; info->currBoardIdx++)
        {
            //获取控制板逻辑地址
            info->currBoard = info->file_segmet_info.segList[info->currSegment][info->currBoardIdx];
            info->update_ctrl.segCrc = info->file_segmet_info.segCrc[info->currSegment];
            info->update_ctrl.target_cpu = info->currBoard;
            EN_SLOGD(TAG, "start iap currBoard %d  segBoardCnt %d", info->currBoard,info->file_segmet_info.segBoardCnt[i]);
            EN_SLOGD(TAG, "start iap currBoardIdx %d", info->currBoardIdx);
            EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardHello");
            if(!modFwUpdateModuleSingleBoardHello(info))    //step1, 进行握手
            {
                EN_SLOGE(TAG, "single board hello error, go retry");
                goto ERR_RETRY;
            }
            
            EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardErase");
            if(!modFwUpdateModuleSingleBoardErase(info))     //step2, 进行擦除
            {
                EN_SLOGE(TAG, "single board erase error, go retry");
                goto ERR_RETRY;
            }
            
            EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardProgram");
            if(!modFwUpdateModuleSingleBoardProgram(info))    //step3, 进行烧录
            {
                EN_SLOGE(TAG, "single board Program error, go retry");
                goto ERR_RETRY;
            }
        }
    }
    
    if(info->file)
    {
        fclose(info->file);
        info->file = NULL;
    }
    
    return true;
    
ERR_OUT:
    if(info->file) 
    {
        fclose(info->file);
        info->file = NULL;
    }
    
    return false;
}









bool modFwUpdateModule(const char* filename)
{
    bool rst = false;
    static bool chg_is_updating = false;
    
    if(!filename)
    {
        EN_SLOGE(TAG, "filename is empty!");
        return false;
    }
    
    if(chg_is_updating)
    {
        EN_SLOGW(TAG, "is updating now!");
        return true;
    }
    
    if(!pModUpInfo)
    {
        //这里由master侧执行
        //master侧esp32用 malloc 没问题， slave侧要用 MALLOC 函数
        pModUpInfo = (stModUpdateFileInfo_t*)malloc(sizeof(stModUpdateFileInfo_t));
        if(!pModUpInfo)
        {
            return false;
        }
        
        memset(pModUpInfo, 0, sizeof(stModUpdateFileInfo_t));
        
        if (!pModUpInfo->fw_update_sem)
        {
            pModUpInfo->fw_update_sem = xSemaphoreCreateBinary();
        }
    }
    
    chg_is_updating = true;
    
    //1.文件检测
    //if(!chgFwUpdateChargerCheckFile(filename, &pModUpInfo->file_segmet_info))
    //{
    //    EN_LOGE(TAG, "Failed to md5 check file:%s", filename);
    //    //goto OUT;
    //}
    
    //2.解析头
    //EN_SLOGD(TAG, "check file head is valid...");
    if(!modFwUpdateModuleParseHeader(filename, &pModUpInfo->file_header_info, &pModUpInfo->file_segmet_info))
    {
        EN_SLOGE(TAG, "dcmk Failed to parse header file:%s", filename);
        goto OUT;
    }
    EN_SLOGD(TAG, "dcmk file head is valid!");
        
    //5.开始升级
    EN_SLOGD(TAG, "dcmk Start to update...");
    
    if(!modFwUpdateModuleUpdateFirmware(filename, pModUpInfo))
    {
        EN_SLOGE(TAG, "dcmk UpdateChargerUpdateFirmware fault!");
        goto OUT;
    }
    
    EN_SLOGI(TAG, "dcmk UpdateCharger success!");
    rst = true;
    
OUT:
    pModUpInfo->update_ctrl.update_cmd = ePrivDrvUpdateCmdRebootApp;
    modFwUpdateModuleSendCmd(&pModUpInfo->update_ctrl);
    if(pModUpInfo->file) 
    {
        fclose(pModUpInfo->file);
        pModUpInfo->file = NULL;
    }
    if(pModUpInfo->fw_update_sem)
    {
       vSemaphoreDelete(pModUpInfo->fw_update_sem);
       pModUpInfo->fw_update_sem = NULL;
    }
    
    free(pModUpInfo);
    pModUpInfo = NULL;
    chg_is_updating = false;
    
    if(!rst)
    {
        EN_SLOGE(TAG, "dcmk UpdateCharger failed!");
    }
    
    return rst;
}







/**********************************************************************************************
* Description       :     模块电源固件安装
* Author            :     Hall
* modified Date     :     2023-04-03
* notice            :     
***********************************************************************************************/
bool modFwUpdateFileInstall(const char *pPath)
{
    if(!pPath)
    {
        return false;
    }
    
    EN_SLOGD(TAG, "桩端固件安装:检查文件类型");
    if(chgFwUpdateIsChargerFile(pPath))
    {
        EN_SLOGD(TAG, "桩端固件安装:%s 开始...", pPath);
        if(!modFwUpdateModule(pPath))
        {
            EN_SLOGE(TAG, "桩端固件安装:%s 出错!", pPath);
            return false;
        }
        EN_SLOGD(TAG, "桩端固件安装:%s 结束...", pPath);
    }
    else
    {
        EN_SLOGE(TAG, "桩端固件安装:固件文件:%s 不支持!", pPath);
        return false;
    }
    
    return true;
}






