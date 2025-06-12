/**
 * @file IsoCheck.c
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief 绝缘检测
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2024. All rights reserved 
 * 
 */
#include "iso.h"


extern stAdcDrvCache_t stAdcDrvCache;




//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "IsoCheck";



stIsoCache_t stIsoCache;





//初始化
bool sIsoCheckInit(bool (*pCbOpen )(ePileGunIndex_t eGunIndex), 
                   bool (*pCbClose)(ePileGunIndex_t eGunIndex), 
                   bool (*pCbSet  )(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit));


//状态机函数
bool sIsoCheckStart(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode);
bool sIsoCheckModule(ePileGunIndex_t eGunIndex);
bool sIsoCheckRelayAction1(ePileGunIndex_t eGunIndex);
bool sIsoCheckRelayAction2(ePileGunIndex_t eGunIndex);
bool sIsoCheckCalcResult(ePileGunIndex_t eGunIndex);
bool sIsoCheckEnd(u8 u8Indexbool, eIsoCheckMode_t eIsoCheckMode);


//状态机内部的操作函数
bool sIsoCheckSelf(ePileGunIndex_t eGunIndex);
bool sIsoCheckReadVolt1(ePileGunIndex_t eGunIndex);
bool sIsoCheckReadVolt2(ePileGunIndex_t eGunIndex);
bool sIsoEvcsCs(ePileGunIndex_t eGunIndex);
bool sIsoGetStableAdVolt(ePileGunIndex_t eGunIndex, f32* pUiso, eIsoTypeU_t eType);


//api函数供外部获取绝缘电阻
bool sIsoGetOhm(ePileGunIndex_t eGunIndex, f32* Ohm);




/**
 * @brief 绝缘检测初始化函数，重映射控制信号索引
 * 
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-15
 */
bool sIsoCheckInit(bool (*pCbOpen )(ePileGunIndex_t eGunIndex), 
                   bool (*pCbClose)(ePileGunIndex_t eGunIndex), 
                   bool (*pCbSet  )(ePileGunIndex_t eGunIndex, f32 f32Udc, f32 f32IdcLimit))
{
    i32 i;
    
    memset(&stIsoCache, 0, sizeof(stIsoCache));
    stIsoCache.pCbOpen  = pCbOpen;
    stIsoCache.pCbClose = pCbClose;
    stIsoCache.pCbSet   = pCbSet;
    for(i = ePileGunIndex0; i < ePileGunIndexNum; i++)
    {
        stIsoCache.eIsoMechinaStatus[i].eIsoRst = eIsoIf;
        stIsoCache.eIsoMechinaStatus[i].eIsoCheckStatus = eIsoCheckStart;
        
    }
    
    return true;
}



/**
 * @brief 绝缘检测状态机函数
 * 
 * @param eGunIndex 枪号索引
 * @param eIsoCheckMode_t 绝缘检测类型
 * @return eIsoMechinaStatus_t* 状态机状态与绝缘检测结果，若还未产生结果，则eIsoMechinaStatus.eIsoRst == eIsoCheckIng
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
eIsoMechinaStatus_t* sIosCheck(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode)
{
    
    switch(stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus)
    {
        //绝缘检测开始
        case eIsoCheckStart:
            sIsoCheckStart(eGunIndex, eIsoCheckMode);
            break;
        //模块输出电压检测
        case eIsoCheckModule:
            sIsoCheckModule(eGunIndex);
            break;
        //闭合闭合绝缘检测继电器、绝缘检测正母排继电器，断开绝缘检测负母排继电器
        case eIsoCheckRelayAction1:
            sIsoCheckRelayAction1(eGunIndex);
            break;
        //闭合绝缘检测负母排继电器，断开绝缘检测正母排继电器
        case eIsoCheckRelayAction2:
            sIsoCheckRelayAction2(eGunIndex);
            break;
        //计算绝缘数据
        case eIsoCheckCalcResult:
            sIsoCheckCalcResult(eGunIndex);
            break;
        //结束处理
        case eIsoCheckEnd:
            sIsoCheckEnd(eGunIndex, eIsoCheckMode);
            break;
        default:
            break;
    }
    return &stIsoCache.eIsoMechinaStatus[eGunIndex];
}





/**
 * @brief 将状态机的状态从空闲设置为开始
 * 
 * @param eGunIndex 枪号
 * @param f32IsoCheckVolt 绝缘检测使用的电压
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-03
 */
void sIsoStart(ePileGunIndex_t eGunIndex, f32 f32IsoCheckVolt)
{
    stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckStart;
    stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot = f32IsoCheckVolt;
}





/**
 * @brief 充电中进行绝缘检测结束后调用此函数，关闭绝缘检测的所有继电器
 *  充电中绝缘检测时：绝缘检测本身无法知道合适停止充电，因此无法主动关闭所有绝缘检测继电器
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoStop(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode)
{
    bool bRst = true;
    
    //清除超时计数标志
    stIsoCache.stIsoData[eGunIndex].u32ModuleCount = 0;
    stIsoCache.stIsoData[eGunIndex].u32Count = 0;
    stIsoCache.stIsoData[eGunIndex].u32EndCount = 0;
    
    //设置结果与状态
    stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoInvalid;
    stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckIdel;
    
    //硬件复位
    if(eIsoCheckMode == eIsoCheckBeforeCharge)
    {
        bRst = bRst & sIoSetDrK(eGunIndex, false);
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv1(eGunIndex), RESET);
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv2(eGunIndex), RESET);
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv3(eGunIndex), RESET);
        stIsoCache.pCbClose(eGunIndex);
    }
    else
    {
        //绝缘检测继电器以及正负母排绝绝缘检测继电器断开
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv1(eGunIndex), RESET);
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv2(eGunIndex), RESET);
        bRst = bRst & sIoDrvSetOutput(sIoGetRelayIsoDrv3(eGunIndex), RESET);
    }
    
    
    return bRst;
}





/**
 * @brief 绝缘检测初始化
 * 
 * @param eGunIndex 枪号
 * @param eIsoCheckMode_t 检测模式
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckStart(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode)
{
    if(stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus == eIsoCheckStart)
    {
        //初始化状态
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoCheckIng;
        if(eIsoCheckMode == eIsoCheckBeforeCharge)
        {
            //判断车端接触器是否粘连、输出接触器是否粘连
            sIsoCheckSelf(eGunIndex);
        }
        else
        {
            //充电中绝缘检测
            stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckRelayAction1;
        }
    }
    return true;
}





/**
 * @brief 闭合正母排绝缘检测继电器，断开负母排绝缘检测继电器
 * 
 * @param eGunIndex 枪号索引
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckRelayAction1(ePileGunIndex_t eGunIndex)
{
    //闭合绝缘检测继电器、闭合正母排绝缘检测继电器，断开负母排绝缘检测继电器
    if(!sIoDrvSetOutput(sIoGetRelayIsoDrv3(eGunIndex), SET))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoRelayf;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "绝缘检测继电器异常");
    }
    else if(!sIoDrvSetOutput(sIoGetRelayIsoDrv1(eGunIndex), SET))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoRelayDrv1f;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "正母排继电器异常");
    }
    else if(!sIoDrvSetOutput(sIoGetRelayIsoDrv2(eGunIndex), RESET))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoRelayDrv2f;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "负母排继电器异常");
    }
    else
    {
        //读取绝缘采样电压值1
        sIsoCheckReadVolt1(eGunIndex);
        return true;
    }
    
    return false;
}






/**
 * @brief 闭合正母排绝缘检测继电器，断开负母排绝缘检测继电器
 * 
 * @param eGunIndex 枪号索引
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckRelayAction2(ePileGunIndex_t eGunIndex)
{
    if(!sIoDrvSetOutput(sIoGetRelayIsoDrv1(eGunIndex), RESET))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoRelayDrv1f;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "正母排继电器异常");
    }
    else if(!sIoDrvSetOutput(sIoGetRelayIsoDrv2(eGunIndex), SET))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoRelayDrv2f;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "负母排继电器异常");
    }
    else
    {
        //读取绝缘采样电压值2
        sIsoCheckReadVolt2(eGunIndex);
        return true;
    }
    return false;
}




/**
 * @brief 计算绝缘检测结果
 * 
 * @param eGunIndex 枪号索引
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckCalcResult(ePileGunIndex_t eGunIndex)
{
    EN_SLOGD(TAG, "U1 = %f", stIsoCache.stIsoData[eGunIndex].f32U1);
    EN_SLOGD(TAG, "U2 = %f", stIsoCache.stIsoData[eGunIndex].f32U2);
    EN_SLOGD(TAG, "Ubus = %f", stIsoCache.stIsoData[eGunIndex].f32Ubus);
    
    //计算绝缘电阻   
    if(stIsoCache.stIsoData[eGunIndex].f32U1 == 0)
    {
        //大于cIsoOhmPerStd1就行。因为f32U1 == 0，绝缘电阻趋向无穷大，直接赋f32OhmPerV2一个很大的值。
        stIsoCache.stIsoData[eGunIndex].f32OhmPerV2 = 500000;
    }
    else
    {
        //取绝对值是为了防止采样误差导致算出来的电阻是个负数，取绝对值造成的误差可以接受
        stIsoCache.stIsoData[eGunIndex].f32Rx = ((750 * stIsoCache.stIsoData[eGunIndex].f32Ubus) / (fabs(stIsoCache.stIsoData[eGunIndex].f32U1)) - 153750);
        stIsoCache.stIsoData[eGunIndex].f32OhmPerV1 = stIsoCache.stIsoData[eGunIndex].f32Rx / stIsoCache.stIsoData[eGunIndex].f32Ubus;
    }
    
    if(stIsoCache.stIsoData[eGunIndex].f32U2 == 0)
    {
        //大于cIsoOhmPerStd1就行。因为f32U2 == 0，绝缘电阻趋向无穷大，直接赋f32OhmPerV1一个很大的值。
        stIsoCache.stIsoData[eGunIndex].f32OhmPerV1 = 500000;
    }
    else
    {
        //取绝对值是为了防止采样误差导致算出来的电阻是个负数，取绝对值造成的误差可以接受
        stIsoCache.stIsoData[eGunIndex].f32Ry = ((750 * stIsoCache.stIsoData[eGunIndex].f32Ubus) / (fabs(stIsoCache.stIsoData[eGunIndex].f32U2)) - 153750);
        stIsoCache.stIsoData[eGunIndex].f32OhmPerV2 = stIsoCache.stIsoData[eGunIndex].f32Ry / stIsoCache.stIsoData[eGunIndex].f32Ubus;
    }
    
    EN_SLOGD(TAG, "Rx = %f", stIsoCache.stIsoData[eGunIndex].f32Rx);
    EN_SLOGD(TAG, "Ry = %f", stIsoCache.stIsoData[eGunIndex].f32Ry);
    
    //判断绝缘是否正常
    stIsoCache.stIsoData[eGunIndex].f32OhmPerV = (stIsoCache.stIsoData[eGunIndex].f32OhmPerV1 > stIsoCache.stIsoData[eGunIndex].f32OhmPerV2) ? stIsoCache.stIsoData[eGunIndex].f32OhmPerV2 : stIsoCache.stIsoData[eGunIndex].f32OhmPerV1;
    if(stIsoCache.stIsoData[eGunIndex].f32OhmPerV >= cIsoOhmPerStd1)
    {
        //绝缘正常
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoOk;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        stIsoCache.stIsoData[eGunIndex].bRst = eIsoOk;
        EN_SLOGD(TAG, "绝缘检测正常");
        return true;
    }
    else if(stIsoCache.stIsoData[eGunIndex].f32OhmPerV >= cIsoOhmPerStd2)
    {
        //绝缘异常
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoIa;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        stIsoCache.stIsoData[eGunIndex].bRst = eIsoIa;
        EN_SLOGD(TAG, "绝缘检测异常");
        return true;
    }
    else
    {
        //绝缘故障
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoIf;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        stIsoCache.stIsoData[eGunIndex].bRst = eIsoIf;
        EN_SLOGD(TAG, "绝缘检测故障");
        return false;
    }
}





/**
 * @brief 绝缘检测结束处理，充电中绝缘检测不进入此函数
 * 
 * @param eIsoCheckMode_t 
 * @param eGunIndex 枪号索引
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckEnd(ePileGunIndex_t eGunIndex, eIsoCheckMode_t eIsoCheckMode)
{
    f32 f32Volt = 0.0f;
    
    //绝缘检测结束后的处理：
    //如果是充电前绝缘检测，需要把输出接触器与绝缘检测继电器以及正负母排绝绝缘检测继电器断开
    //如果是充电中绝缘检测，则切换到eIsoCheckStart

    /*充电前绝缘检测最后阶段，断开直流接触器后，需要判断母排电压是否小于60V，小于60v才能返回绝缘检测结束
    每隔cIsoTime3查一次，最多查找cIsoCount3次。若连续cIsoCount3次母排电压仍然大于cIsoVolt4
    则认为绝缘检测结束阶段母排电压异常*/
    if(eIsoCheckMode == eIsoCheckBeforeCharge)
    {
        if(stIsoCache.stIsoData[eGunIndex].u32EndCount == 0)
        {
            //充电前绝缘检测
            sIoDrvSetOutput(sIoGetRelayIsoDrv3(eGunIndex), RESET);
            sIoDrvSetOutput(sIoGetRelayIsoDrv1(eGunIndex), SET);
            sIoDrvSetOutput(sIoGetRelayIsoDrv2(eGunIndex), SET);
            stIsoCache.pCbClose(eGunIndex);
            stIsoCache.stIsoData[eGunIndex].u32EndTimeStamp = xTaskGetTickCount(); 
            stIsoCache.stIsoData[eGunIndex].u32EndCount = 1; 
        }
        
        //绝缘检测失败
        if((stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst != eIsoOk) && (stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst != eIsoIa))
        {
            stIsoCache.stIsoData[eGunIndex].u32EndCount = 0;
            stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckIdel;
            return false;
        }
        
        if((xTaskGetTickCount() - stIsoCache.stIsoData[eGunIndex].u32EndTimeStamp) > cIsoTime3)
        {
            stIsoCache.stIsoData[eGunIndex].u32EndCount ++;
            stIsoCache.stIsoData[eGunIndex].u32EndTimeStamp = xTaskGetTickCount();
            sMeterDrvOptGetUdcIdcPdc((u8)eGunIndex,&f32Volt,NULL,NULL);
            if(f32Volt < cIsoVolt4)
            {
                //电压下降到60v后持500ms才继续下一步,同步让mod_avg组件判断电压小于60v降标志给清掉(mod任务周期200ms)
                vTaskDelay(500 / portTICK_RATE_MS);
                sIoSetDrK(eGunIndex, false);
                stIsoCache.stIsoData[eGunIndex].u32EndCount = 0;
                stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckIdel;
            }
            if(stIsoCache.stIsoData[eGunIndex].u32EndCount >= cIsoCount3)
            {
                stIsoCache.stIsoData[eGunIndex].u32EndCount = 0;
                stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckIdel;
                stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoBusVolt;
                EN_SLOGD(TAG, "关闭模块超时, 绝缘检测结束时母排电压(%0.2fV)在超时时间内未小于%0.2fV", f32Volt, cIsoVolt4);
                
                return false;
            } 
        }
    }
    else
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckStart;
    }
    
    return true;
}




/**
 * @brief 车端接触器粘连/车未熄火检测
 * 
 * @param eGunIndex 枪索引
 * @return true 正常粘连/未熄火
 * @return false 粘连/未熄火
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckSelf(ePileGunIndex_t eGunIndex)
{
    f32 f32EvVolt = 0;
    
    //判断车端接触器是否粘连。车端接触器粘连与车未熄火是一样的。通过检测枪线电压，大于cIsoVolt1，则认为车未熄火。
    sAdcAppSampleGetGunDcOut(eGunIndex, &f32EvVolt);
    
    if(fabs(f32EvVolt) > cIsoVolt1)
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoEvCs;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "车端接触器粘连");
    }
    else if(!sIsoEvcsCs(eGunIndex))
    {
        //判断输出接触器是否粘连。
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoEvseCs;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "输出接触器粘连"); 
    }
    else if((sModAppOptSwitchContactorY() == false) || (sIoSetDrK(eGunIndex, true) == false))
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoEvseCf;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "充电桩输出接触器异常");
    }
    else
    {
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckModule;
        return true;
    }
    return false;
}




/**
 * @brief 绝缘检测时，设置绝缘检测使用的电压
 * 
 * @param eGunIndex 枪号索引
 * @param f32Volt 电压，单位1V
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-27
 */
bool sIsoCheckModule(ePileGunIndex_t eGunIndex)
{
    if(stIsoCache.stIsoData[eGunIndex].u32ModuleCount == 0)
    {
        //首次运行时设置电源模块电压以及时间戳
        stIsoCache.stIsoData[eGunIndex].u32ModuleCount = 1;
        stIsoCache.stIsoData[eGunIndex].u32ModuleTimeStamp = xTaskGetTickCount();
        stIsoCache.pCbOpen(eGunIndex);
        stIsoCache.pCbSet(eGunIndex, stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot, 0);
    }
    
    
    if((xTaskGetTickCount() - stIsoCache.stIsoData[eGunIndex].u32ModuleTimeStamp) >= cIsoTime1)
    {
        //表示判读过一次电压差
        stIsoCache.stIsoData[eGunIndex].u32ModuleCount ++; 
        
        //经过cIsoTime1后再判断电压
        stIsoCache.pCbSet(eGunIndex, stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot, 0);
        stIsoCache.stIsoData[eGunIndex].u32ModuleTimeStamp = xTaskGetTickCount();
        sMeterDrvOptGetUdcIdcPdc(eGunIndex,&stIsoCache.stIsoData[eGunIndex].f32ModuleVoltRead,NULL,NULL);
        
        
        //读取电压小于30v，判定为模块未收到开机指令，重发
        if(stIsoCache.stIsoData[eGunIndex].f32ModuleVoltRead < 30.0f)
        {
            stIsoCache.pCbOpen(eGunIndex);
        }
        
        //判断电压是否稳定
        if(fabsf(stIsoCache.stIsoData[eGunIndex].f32ModuleVoltRead - stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot) <= cIsoVolt2)
        {
            //电压稳定
            stIsoCache.stIsoData[eGunIndex].u32ModuleCount = 0;
            stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckRelayAction1;
            EN_SLOGD(TAG, "电压稳定，执行下一个阶段，当前电表电压：%0.2f, 绝缘检测电压：%0.2f", stIsoCache.stIsoData[eGunIndex].f32ModuleVoltRead, stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot);
            
            return true;
        }
    }
    
    if(stIsoCache.stIsoData[eGunIndex].u32ModuleCount >= cIsoCount1)
    {
        //模块输出未到达要求，且超时
        stIsoCache.stIsoData[eGunIndex].u32ModuleCount = 0;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoPmof;
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        EN_SLOGD(TAG, "电源模块输出异常: 枪内侧电压:%0.2f, 绝缘检测需求电压:%0.2f", stIsoCache.stIsoData[eGunIndex].f32ModuleVoltRead, stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot);
        
        return false;
    }
    
    //模块输出未到达要求，但未超时
    return true;
}





/**
 * @brief 读取绝缘检测电压u1
 * 
 * @param eGunIndex 枪号索引
 * @return true
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckReadVolt1(ePileGunIndex_t eGunIndex)
{
    //读取绝缘检测电压
    return sIsoGetStableAdVolt(eGunIndex, &(stIsoCache.stIsoData[eGunIndex].f32U1), eIsoTypeU1);
    
}




/**
 * @brief 读取绝缘检测电压u2
 * 
 * @param eGunIndex 枪号索引
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-04-01
 */
bool sIsoCheckReadVolt2(ePileGunIndex_t eGunIndex)
{
    //读取绝缘检测电压
    return sIsoGetStableAdVolt(eGunIndex, & (stIsoCache.stIsoData[eGunIndex].f32U2), eIsoTypeU2);
}




/**
 * @brief 判断输出接触器是否粘连
 * 
 * @param eGunIndex 枪号索引
 * @return bool 返回true则粘连，返回false则未粘连
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-27
 */
bool sIsoEvcsCs(ePileGunIndex_t eGunIndex)
{
    if(sAdcAppSampleGetFbStateK(eGunIndex) == eAdcAppFbState00)
    {
        return true;
    }
    EN_SLOGD(TAG, "绝缘检测正母排或负母排继电器设置失败");
    
    return false;
}





/**
 * @brief 绝缘检测，获取稳定后的电压采样值
 * 
 * @param eGunIndex 枪号索引
 * @param pUiso 绝缘检测电压U1 或U2
 * @return true 
 * @return false 
 * @version 0.1
 * @author anlada (bo.hou@en-plus.com.cn)
 * @date 2024-03-27
 */
bool sIsoGetStableAdVolt(ePileGunIndex_t eGunIndex, f32* pUiso, eIsoTypeU_t eType)
{
    f32 f32Uiso = 0;                                                            //绝缘检测采样电压
    f32 f32UdcOut = 0;                                                          //枪外侧电压
    
    if(stIsoCache.stIsoData[eGunIndex].u32Count == 0)
    {
        //首次进入初始化时间戳
        stIsoCache.stIsoData[eGunIndex].u32Count = 1;
        stIsoCache.stIsoData[eGunIndex].u32UisoTimeStamp = xTaskGetTickCount();
    }
    
    if((xTaskGetTickCount() - stIsoCache.stIsoData[eGunIndex].u32UisoTimeStamp) > cIsoTime2)
    {
        stIsoCache.stIsoData[eGunIndex].u32Count ++;
        stIsoCache.stIsoData[eGunIndex].u32UisoTimeStamp = xTaskGetTickCount();
        
        //判断母排上电压是否稳定了
        sMeterDrvOptGetUdcIdcPdc(eGunIndex,&stIsoCache.stIsoData[eGunIndex].f32Voltbus,NULL,NULL);
        if(sAdcAppSampleGetGunDcOut(eGunIndex, &f32UdcOut) && (fabs(stIsoCache.stIsoData[eGunIndex].f32Voltbus - stIsoCache.stIsoData[eGunIndex].f32IsoCheckVlot) < cIsoVolt3))
        {
            
            //母排电压稳定
            sAdcAppSampleGetUiso(eGunIndex, &f32Uiso);
            *pUiso = f32Uiso;
            if(eType == eIsoTypeU1)
            {
                //电压u1时的母排电压
                stIsoCache.stIsoData[eGunIndex].f32Voltbus1 = f32UdcOut;
                stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckRelayAction2;
            }
            else if(eType == eIsoTypeU2)
            {
                //电压u2时的母排电压
                stIsoCache.stIsoData[eGunIndex].f32Voltbus2 = f32UdcOut;
                stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckCalcResult;
                //取均值作为绝缘电阻计算的母线电压
                stIsoCache.stIsoData[eGunIndex].f32Ubus = (stIsoCache.stIsoData[eGunIndex].f32Voltbus1 + stIsoCache.stIsoData[eGunIndex].f32Voltbus2) / 2;
            }
            
            //读取结束，恢复初值
            stIsoCache.stIsoData[eGunIndex].u32Count = 0;
            return true;
        }
    }
    
    if(stIsoCache.stIsoData[eGunIndex].u32Count > cIsoCount2)
    {
        //结束，恢复初值
        stIsoCache.stIsoData[eGunIndex].u32Count = 0;
        
        //检测到母线电压不稳定，结束
        stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoCheckStatus = eIsoCheckEnd;
        if(eType == eIsoTypeU1)
        {
            stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoU1UnSteady;
            EN_SLOGD(TAG, "U1电压不稳定");
        }
        else//eIsoTypeU2
        {
            stIsoCache.eIsoMechinaStatus[eGunIndex].eIsoRst = eIsoU2UnSteady;
            EN_SLOGD(TAG, "U2电压不稳定");
        }
        return false;
    }
    
    return false;
}





/***************************************************************************************************
* Description                           :   绝缘阻抗结果 获取api函数
* Author                                :   HH
* Creat Date                            :   2023-07-05
* notice                                :   
****************************************************************************************************/
bool sIsoGetOhm(ePileGunIndex_t eGunIndex, f32* Ohm)
{
    if(eGunIndex >= ePileGunIndexNum)
    {
        return false;
    }
    
    if(Ohm != NULL)
    {
        (*Ohm) = (stIsoCache.stIsoData[eGunIndex].f32Rx > stIsoCache.stIsoData[eGunIndex].f32Ry) ? stIsoCache.stIsoData[eGunIndex].f32Ry : stIsoCache.stIsoData[eGunIndex].f32Rx;
        return true;
    }
    return(false);
}




