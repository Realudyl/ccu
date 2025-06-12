/***************************************************************************************************
* Copyright (C)2023, EN+.
* File Name                             :     private_drv_0bab_update.c
* Description                           :     充电桩端模块电源升级
* Version                               :     
* Author                                :     Hall
* Creat Date                            :     2023-11-04
* notice                                :     从老代码中移植过来
****************************************************************************************************/
#include "en_md5.h"
#include "private_drv_0bab.h"
#include "private_drv_0bab_30.h"
#include "private_drv_0bab_update.h"

#include "private_drv_api_m.h"
#include "private_drv_opt.h"


//设定本文件的日志打印等级和文件标签
//对en_iot_sdk无效,对en_chg_sdk有效
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "private_drv_0bab_update";




extern stPrivDrvCache_t *pPrivDrvCache;
static sCpUpdateFileInfo_t *gChgFwChargerInfo = NULL;





void chgFwUpdateChargerRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst);
bool chgFwUpdateChargerParseHeader(const char* filename, sFwHeader_t *header, sFwSegInto_t* segment);
bool chgFwUpdateChargerSendCmd(const sFwUpdateData_t* data);
bool chgFwUpdateChargerSingleBoardHello(sCpUpdateFileInfo_t *info);
bool chgFwUpdateChargerSingleBoardErase(sCpUpdateFileInfo_t *info);
bool chgFwUpdateChargerSingleBoardProgram(sCpUpdateFileInfo_t *info);
bool chgFwUpdateChargerUpdateFirmware(const char* filename, sCpUpdateFileInfo_t *info);
bool chgFwUpdateCharger(const char* filename);


bool chgFwUpdateIsChargerFile(const char* filename);
bool chgFwUpdateChargerParseLine(const char* line_str, u8 *line_info);
bool chgFwUpdateChargerCheckFile(const char* filename, sFwSegInto_t* segment);
bool chgFwUpdateFileInstall(const char *pPath);
















void chgFwUpdateChargerRecvAck(ePrivDrvUpdateCmd_t cmd, ePrivDrvUpdateCmdRst_t rst)
{   
    if(gChgFwChargerInfo)
    {       
        gChgFwChargerInfo->update_ack.update_cmd = cmd;
        gChgFwChargerInfo->update_ack.update_rst = rst;
        xSemaphoreGive(gChgFwChargerInfo->fw_update_sem);
    }
}







bool chgFwUpdateChargerParseHeader(const char* filename, sFwHeader_t *header, sFwSegInto_t* segment)
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
        
        if(pPrivDrvCache->bPrintfType)
        {
            EN_SLOGD(TAG, "readline[%d]: %s", index, tmpbuf);
        }
        
        switch(index)
        {
            case 0:// 文件生成日期  
                strcpy(header->fw_time, result+1);
                if(pPrivDrvCache->bPrintfType)
                {
                    EN_SLOGD(TAG, "fw_time: %s",  header->fw_time);
                }
                break;
            case 1://软件版本 
                strcpy(header->sw_ver, result+1);
                if(pPrivDrvCache->bPrintfType)
                {
                    EN_SLOGD(TAG, "sw_ver: %s", header->sw_ver);
                }
                break;
            case 2://设备类型
                header->dev_type = strtol(result + 1, 0, 16); 
                if(pPrivDrvCache->bPrintfType)
                {
                    EN_SLOGD(TAG, "dev_type: %02X", header->dev_type);
                }
                break;
            case 3://设备名称
                strcpy(header->dev_name, result+1);
                EN_SLOGD(TAG, "dev_name: %s", header->dev_name);
                break;
            case 4://段信息个数
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
        if(pPrivDrvCache->bPrintfType)
        {
            EN_SLOGD(TAG, "segName[%d]:%s", i, segment->segName[i]);
        }
        
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
        if(pPrivDrvCache->bPrintfType)
        {
            EN_SLOGD(TAG, "segBoardCnt[%d]:%d", i, segment->segBoardCnt[i]);
        }
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







bool chgFwUpdateChargerSendCmd(const sFwUpdateData_t* data)
{
    bool                bRst;
    ePrivDrvVersion_t   eVer;
    
    if(!data)
    {
        return false;
    }
    
#if (cSdkPrivDevType == cSdkPrivDevTypeM)
    //Master 端才有这个函数
    bRst = sPrivDrvGetProtocolVersion(&eVer);
    if((bRst) && (eVer == ePrivDrvVersion30))
    {
        sPrivDrvPktSendAB30(data->target_cpu, data->base_addr, data->offset, data->update_cmd, data->data_words, data->data_len);
    }
    else
    {
        sPrivDrvPktSendAB(data->target_cpu, data->base_addr, data->offset, data->update_cmd, data->data_words, data->data_len);
    }
#endif
    
    return true;
}





bool chgFwUpdateChargerSingleBoardHello(sCpUpdateFileInfo_t *info)
{
    bool res = false;
    int  repeat = 3;
    eChgFwState_t step = CHG_FW_STATE_1;
    
    EN_SLOGD(TAG, "chgFwUpdateChargerSingleBoardHello1111111");
    
    //clear sem
    xSemaphoreTake(info->fw_update_sem, 0);
    
    EN_SLOGD(TAG, "chgFwUpdateChargerSingleBoardHello222222");
    while (1)
    {
        switch(step)
        {
        case CHG_FW_STATE_1: //发送握手包
            EN_SLOGD(TAG, "send update_hello to charger");
            info->update_ctrl.update_cmd = ePrivDrvUpdateCmdHandshake;
            chgFwUpdateChargerSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_2;                 
            break;
        case CHG_FW_STATE_2: //等待握手成功
            EN_SLOGD(TAG, "wait for ack from charger 5s");
            if(xSemaphoreTake(info->fw_update_sem, 5000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdHandshake))
                {
                    res = true;
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








bool chgFwUpdateChargerSingleBoardErase(sCpUpdateFileInfo_t *info)
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
            chgFwUpdateChargerSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_2;                 
            break;
        case CHG_FW_STATE_2: //等待握手成功
            EN_SLOGD(TAG, "wait for ack from charger 35s");
            if(xSemaphoreTake(info->fw_update_sem, 35000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdErase))
                {
                    res = true;
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






u8 chgFwUpdateChargerSingleBoardParses(sCpUpdateFileInfo_t *info)
{
    char tmpbuf[128] = {0};
    sFwHexLineInfo_t tmpLine;
    u16 base_addr;
    
    info->update_ctrl.data_len = 0;
    if (info->nextBaseAddr != 0)  // 地址发生了跳变，重新设置基地址
    {
        info->update_ctrl.base_addr = info->nextBaseAddr;
        info->nextBaseAddr = 0;
    }
    
    if(pPrivDrvCache->bPrintfType)
    {
        EN_SLOGD(TAG, "SingleBoardParses -> data_len:%d addr:%04X%04X", info->update_ctrl.data_len, info->update_ctrl.base_addr, info->update_ctrl.offset);
    }
    
    do
    {
        //读取一行
        if(!fgets((char*)tmpbuf , sizeof(tmpbuf), info->file))
        {
            EN_SLOGE(TAG, "SingleBoardParses fgets error!");
            return 0;
        }
        
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
                if(info->update_ctrl.data_len == 0)
                {
                    info->update_ctrl.offset = BigLittleSwap16(tmpLine.offset);
                }
                
                memcpy((u8*)&(info->update_ctrl.data_words[info->update_ctrl.data_len]), tmpLine.data, tmpLine.data_len);
                info->update_ctrl.data_len += tmpLine.data_len >> 1;
                  
                if(info->update_ctrl.data_len >= cPrioDrvLenUpdateData)
                {
                    if(pPrivDrvCache->bPrintfType)
                    {
                        EN_SLOGD(TAG, "FW_LINE_TYPE_DATA -> data_len:%d addr:%04X%04X", info->update_ctrl.data_len, info->update_ctrl.base_addr, info->update_ctrl.offset);
                    }
                    return 1;
                }
                break;
            case FW_LINE_TYPE_EX_LINE_ADDR:     //扩展地址
                base_addr = tmpLine.data[0]<<8 | tmpLine.data[1];
                
                if (!info->update_ctrl.data_len)
                {
                    info->update_ctrl.base_addr = base_addr;
                    EN_SLOGD(TAG, "FW_LINE_TYPE_EX_LINE_ADDR -> data_len:%d  base_addr:%04X nextBaseAddr:%04X", info->update_ctrl.data_len, info->update_ctrl.base_addr,  info->nextBaseAddr);
                    
                }
                else
                {
                    info->nextBaseAddr = base_addr;
                    
                    EN_SLOGD(TAG, "FW_LINE_TYPE_EX_LINE_ADDR OUT -> data_len:%d  offset:%04X nextBaseAddr:%04X", info->update_ctrl.data_len, info->update_ctrl.offset,  info->nextBaseAddr);
                    
                    return 1;
                }
                break;
            case FW_LINE_TYPE_LINE_ADDR:     //下一行,不作解析
                continue;
            case FW_LINE_TYPE_END:   //文件结束,记录下一段position, 单板升级完成
                if ((info->currSegment + 1) < SENMENT_CNT_MAX)
                {
                    info->file_segmet_info.segPosition[info->currSegment + 1] = ftell(info->file);
                    EN_SLOGD(TAG, "FW_LINE_TYPE_END -> data_len:%d  offset:%04X nextBaseAddr:%04X", info->update_ctrl.data_len, info->update_ctrl.offset,  info->nextBaseAddr);
                }
                return 0xFF;
            default:
                break;
            }
        }
        else
            EN_SLOGD(TAG, "SingleBoardParses ParseLine faults:%s!", tmpbuf);
    }while (info->update_ctrl.data_len < cPrioDrvLenUpdateData);
    
    EN_SLOGD(TAG, "SingleBoardParses out with 0; data_len:%d", info->update_ctrl.data_len);
    return 0;
}








bool chgFwUpdateChargerSingleBoardProgram(sCpUpdateFileInfo_t *info)
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
            pkgIsOk = chgFwUpdateChargerSingleBoardParses(info);
            if(pPrivDrvCache->bPrintfType)
            {
                EN_SLOGD(TAG, "parse segment: pkgIsOk:%02X", pkgIsOk);
            }
            if(pkgIsOk)
            {
                if(info->update_ctrl.data_len != 0)
                {
                    step = CHG_FW_STATE_2;
                }
                else if(pkgIsOk == 0xFF)
                {
                    res = true;
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
            info->update_ctrl.update_cmd = ePrivDrvUpdateCmdFlash;
            
            chgFwUpdateChargerSendCmd(&info->update_ctrl);
            step = CHG_FW_STATE_3;                 
            break;
        case CHG_FW_STATE_3: //等待应答
            if(xSemaphoreTake(info->fw_update_sem, 2000 / portTICK_PERIOD_MS))
            {
                if((info->update_ack.update_rst == ePrivDrvUpdateCmdRstSuccess)
                && (info->update_ack.update_cmd == ePrivDrvUpdateCmdFlash))
                {
                    if(pkgIsOk == 0xFF)
                    {
                        res = true;
                        goto EXIT;
                    }
                    else
                    {
                         step = CHG_FW_STATE_1;
                         continue;
                    }
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






bool chgFwUpdateChargerUpdateFirmware(const char* filename, sCpUpdateFileInfo_t *info)
{
    if(!info || !filename)
    {
        return false;
    }
    
    int retryTimer = 5, i;
    
ERR_RETRY:
    if(--retryTimer <= 0)
    {
        goto ERR_OUT;
    }
    
    memset(&info->update_ctrl, 0, sizeof(info->update_ctrl));
    info->update_ctrl.update_cmd = ePrivDrvUpdateCmdRebootBoot;
    chgFwUpdateChargerSendCmd(&info->update_ctrl);


    //30s超时，等待桩重新上线
    bool rst = xSemaphoreTake(info->fw_update_sem,  5000 / portTICK_PERIOD_MS);
    if(!rst || (info->update_ack.update_rst == ePrivDrvUpdateCmdRstFail))
    {
        EN_SLOGE(TAG, "wait for boot mode error! sem:%d", rst);
        goto ERR_RETRY;
    }
    EN_SLOGD(TAG, "wait for boot mode ok");

    //打开文件
    if((info->file = fopen(filename, "r")) == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", filename);
        goto ERR_OUT;
    }
    fseek(info->file, 0, SEEK_SET);
    EN_SLOGD(TAG, "open file:%s ok!", filename);
    
    //记录段的起始位置
    info->file_segmet_info.segPosition[0] = ftell(info->file);
    
    
    //打开文件升级
    for(info->currSegment = 0 ; info->currSegment < info->file_segmet_info.segCnt ; info->currSegment++)
    {
        i = info->currSegment;
        for (info->currBoardIdx = 0; info->currBoardIdx < info->file_segmet_info.segBoardCnt[i]; info->currBoardIdx++)
        {
            //获取控制板逻辑地址
            info->currBoard = info->file_segmet_info.segList[info->currSegment][info->currBoardIdx];
            info->update_ctrl.target_cpu = info->currBoard;
            
            EN_SLOGD(TAG, "start iap board %d", info->currBoardIdx);
            if(pPrivDrvCache->bPrintfType)
            {
                EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardHello");
            }
            if(!chgFwUpdateChargerSingleBoardHello(info))    //step1, 进行握手
            {
                EN_SLOGE(TAG, "single board hello error, go retry");
                goto ERR_RETRY;
            }
            
            if(pPrivDrvCache->bPrintfType)
            {
                EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardErase");
            }
            if(!chgFwUpdateChargerSingleBoardErase(info))     //step2, 进行擦除
            {
                EN_SLOGE(TAG, "single board erase error, go retry");
                goto ERR_RETRY;
            }
            
            if(pPrivDrvCache->bPrintfType)
            {
                EN_SLOGD(TAG, "start chgFwUpdateChargerSingleBoardProgram");
            }
            if(!chgFwUpdateChargerSingleBoardProgram(info))    //step3, 进行烧录
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









bool chgFwUpdateCharger(const char* filename)
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
    
    if(!gChgFwChargerInfo)
    {
        //这里由master侧执行
        //master侧esp32用 malloc 没问题， slave侧要用 MALLOC 函数
        gChgFwChargerInfo = (sCpUpdateFileInfo_t*)malloc(sizeof(sCpUpdateFileInfo_t));
        if(!gChgFwChargerInfo)
        {
            return false;
        }
        
        memset(gChgFwChargerInfo, 0, sizeof(sCpUpdateFileInfo_t));
        
        if (!gChgFwChargerInfo->fw_update_sem)
        {
            gChgFwChargerInfo->fw_update_sem = xSemaphoreCreateBinary();
        }
    }
    
    chg_is_updating = true;
    
    //1.文件检测
    if(!chgFwUpdateChargerCheckFile(filename, &gChgFwChargerInfo->file_segmet_info))
    {
        EN_SLOGE(TAG, "Failed to md5 check file:%s", filename);
        goto exit;
    }
    
    //2.解析头
    if(!chgFwUpdateChargerParseHeader(filename, &gChgFwChargerInfo->file_header_info, &gChgFwChargerInfo->file_segmet_info))
    {
        EN_SLOGE(TAG, "Failed to parse header file:%s", filename);
        goto exit;
    }
    
    //4.开始升级
    EN_SLOGD(TAG, "Start to update...");
    if(!chgFwUpdateChargerUpdateFirmware(filename, gChgFwChargerInfo))
    {
        EN_SLOGE(TAG, "chgFwUpdateChargerUpdateFirmware fault!");
        goto OUT;
    }
    
    EN_SLOGI(TAG, "chgFwUpdateCharger success!");
    rst = true;
    
OUT:
    gChgFwChargerInfo->update_ctrl.update_cmd = ePrivDrvUpdateCmdRebootApp;
    gChgFwChargerInfo->update_ctrl.data_len   = 0;
    chgFwUpdateChargerSendCmd(&gChgFwChargerInfo->update_ctrl);
    
    //只要没进入升级,不允许给桩发AB指令
exit:
    if(gChgFwChargerInfo->file) 
    {
        fclose(gChgFwChargerInfo->file);
        gChgFwChargerInfo->file = NULL;
    }
    if(gChgFwChargerInfo->fw_update_sem)
    {
       vSemaphoreDelete(gChgFwChargerInfo->fw_update_sem);
       gChgFwChargerInfo->fw_update_sem = NULL;
    }
    
    free(gChgFwChargerInfo);
    gChgFwChargerInfo = NULL;
    chg_is_updating = false;
    
    if(!rst)
    {
        EN_SLOGE(TAG, "chgFwUpdateCharger failed!");
    }
    
    return rst;
}





bool chgFwUpdateIsChargerFile(const char* filename)
{
    if(!filename)
    {
        return false;
    }
    
    char tmpbuf[128] = { 0 };
    int i = 0;
    
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", filename);
        return false;
    }
    fseek(file, 0, SEEK_SET);
    
    for(i=0; i<10; i++)
    {
        if(fgets(tmpbuf, sizeof(tmpbuf), file) == NULL)
        {
            continue;
        }
        
        if (!strstr(tmpbuf, "Device Name:"))
        {
            continue;
        }
        
        EN_SLOGE(TAG, "readline: %s", tmpbuf);
        
        fclose(file);
        return true;
    }
    
    fclose(file);
    
    return false;
}






bool chgFwUpdateChargerParseLine(const char* line_str, u8 *line_info)
{
    if (!line_str || !line_info)
    {
        return false;
    }
    
    u8  sum = 0;
    
    int len = sStrToHex(line_str + 1, line_info, sizeof(sFwHexLineInfo_t));
    
    if(len > 0)
    {
        //校验方式为一行内容相加, 低8位为0
        for(int i=0; i<len; i++)
        {
            sum += line_info[i];
        }
        
        if(sum == 0)
        {
            return true;
        }
    }
    
    return false;
}






bool chgFwUpdateChargerCheckFile(const char* filename, sFwSegInto_t* segment)
{
    if(!filename)
    {
        return false;
    }
    
    int   lines = 0;
    char  tmpbuf[128] = {0};
    u8    tmpHexs[32] = {0};
    FILE *file = NULL;
    int   retryCnt = 3;
    sFwHexLineInfo_t tmpLine;
    int   res = false;
    uint8_t  index = 0;
    md5_context md5;
    uint8_t code[16];
    
REOPENFILE:
    
    file = fopen(filename, "r");
    if(file == NULL)
    {
        EN_SLOGE(TAG, "open file:%s error!", filename);
        return false;
    }
    fseek(file, 0, SEEK_SET);
    
    res = false; 
    lines = 0;
    index = 0;
    memset(&md5, 0, sizeof(md5));
    memset((u8*)segment->segLength, 0, sizeof(segment->segLength));
    
    md5_starts(&md5);
    
    //计算md5校验码,每次读取一行
    while (fgets(tmpbuf , sizeof(tmpbuf), file))
    {
        lines++;
            
        //段数据统计
        if (tmpbuf[0] == ':')
        {
            sStrToHex(tmpbuf+1, (u8*)&tmpLine, sizeof(tmpLine));
            if (tmpLine.data_type == FW_LINE_TYPE_DATA)
            {
                segment->segLength[index] += tmpLine.data_len;
            }
            else if (tmpLine.data_type == FW_LINE_TYPE_END)   //段结束
            {
                index++;
            }
        }
        
        if (strstr((char*)tmpbuf, "update.hex"))
        {
            md5_finish(&md5, code);
            EN_HLOGW(TAG, "count md5: ", code, 16);
            
            sStrToHex(tmpbuf, tmpHexs, sizeof(tmpHexs));
            EN_HLOGW(TAG, "read  md5: ", tmpHexs, 16);
            
            res = false;
            if(!memcmp(code, tmpHexs, 16))
            {
                res = true;
            }
             
            goto EXIT;
        }
        md5_update(&md5, (u8*)tmpbuf, strlen(tmpbuf)); 
    }
    
    if(lines < 5000) 
    {
        EN_SLOGE(TAG, "lines < 5000 XXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        if(file != NULL)
        {
            fclose(file);
            file = NULL;
        }
        if(--retryCnt == 0)
        {
            return false;
        }
        
        goto REOPENFILE;
    }
    
    
EXIT:
    if(file != NULL)
    {
        fclose(file);
        file = NULL;
    }
    
    return res;
}








/**********************************************************************************************
* Description       :     桩端固件安装
* Author            :     Hall
* modified Date     :     2022-10-18
* notice            :     
***********************************************************************************************/
bool chgFwUpdateFileInstall(const char *pPath)
{
    if(!pPath)
    {
        return false;
    }
    
    EN_SLOGI(TAG, "桩端固件安装:检查文件类型");
    if(chgFwUpdateIsChargerFile(pPath))
    {
        EN_SLOGI(TAG, "桩端固件安装:%s 开始...", pPath);
        if(!chgFwUpdateCharger(pPath))
        {
            EN_SLOGE(TAG, "桩端固件安装:%s 出错!", pPath);
            return false;
        }
        EN_SLOGI(TAG, "桩端固件安装:%s 结束...", pPath);
    }
    else
    {
        EN_SLOGE(TAG, "桩端固件安装:固件文件:%s 不支持!", pPath);
        return false;
    }
    
    return true;
}









