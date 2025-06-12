use regex::Regex;
use std::fmt;
use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
mod file_string;
use file_string::*;
struct Stdata {
    pub name: String,
    pub data_type: String,
    pub comment: String,
}

impl fmt::Display for Stdata {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        writeln!(
            f,
            "name: {}, data_type: {}, comment: {}",
            self.name, self.data_type, self.comment
        )
    }
}

struct Stdatavec {
    pub data: Vec<Stdata>,
}

impl Stdatavec {
    pub fn new() -> Stdatavec {
        Stdatavec { data: Vec::new() }
    }
    fn push(&mut self, name: String, data_type: String, comment: String) {
        self.data.push(Stdata {
            name,
            data_type,
            comment,
        });
    }
}

impl fmt::Display for Stdatavec {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for data in &self.data {
            writeln!(
                f,
                "name: {}, data_type: {}, comment: {}",
                data.name, data.data_type, data.comment
            )?;
        }
        Ok(())
    }
}

fn extract_struct_fields(c_code: &str) -> Stdatavec {
    // 正则表达式匹配结构体中的字段定义
    let struct_re =
        Regex::new(r"typedef\s+struct\s+stSingleData_t\s*\{([\s\S]+?)\}\s+stSingleData_t\s*;")
            .unwrap();
    let data = struct_re.captures(c_code).unwrap().get(1).unwrap().as_str();
    let re = Regex::new(r"\s*(\w+)\s+(\w+)\s*;\s*//\s*(.*)").unwrap();

    let mut field_vec = Stdatavec::new();
    // 使用正则表达式查找所有匹配项
    for caps in re.captures_iter(data) {
        let data_type = caps[1].to_string(); // 数据类型
        let field_name = caps[2].to_string(); // 数据名
        let comment = caps[3].to_string(); // 注释内容

        field_vec.push(field_name, data_type, comment);
    }

    field_vec
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut file = File::open("mod_drv_env2h_data_def.md")?;

    let mut contents = String::new();

    file.read_to_string(&mut contents)?;

    let _c_code = r#"
/**
 * @file mod_drv_env2h_data_def.h
 * @author anlada (bo.hou@en-plus.com.cn)
 * @brief en+ 双向模块协议数据定义
 * @version 0.1
 * @date 2025-02-11
 * 
 * @copyright Shenzhen EN Plus Technologies Co., Ltd. 2015-2025. All rights reserved 
 * 
 */

#pragma once

#include "en_common.h"

/**
 * @defgroup ENpV2H 双向模块协议数据定义
 * @brief 此模块定义了en+双向模块协议的数据结构和相关功能。
 * @{
 */

/**
 * @brief 数据编号枚举
 */
typedef enum {
    eDCVoltage,
    eDCCurrent,
    eModuleStatus2,
    eModuleStatus1,
    eModuleStatus0,
    eInverterStatus2,
    eInverterStatus1,
    eInverterStatus0,
    ePhaseAVoltage,
    ePhaseBVoltage,
    ePhaseCVoltage,
    ePhaseACurrent,
    ePhaseBCurrent,
    ePhaseCCurrent,
    eFrequency,
    eActivePowerA,
    eActivePowerB,
    eActivePowerC,
    eReactivePowerA,
    eReactivePowerB,
    eReactivePowerC,
    eSetDCVoltage,
    eSetDCCurrent,
    ePowerOnOff,
    eOffGridVoltageSetting,
    eOffGridFrequencySetting,
    eACReactivePowerSetting,
    eOperationMode,
    eACActivePowerSetting,
} eDataIndex_t;

/**
 * @brief 双向模块协议数据结构
 */
typedef struct stModDrvV2H_t
{
    eDataIndex_t                        eIndex;                                  //数据编号
    u8                                  u8Cmd;                                  //命令号
    u8                                  u8GroupNum;                             //数据组编号
    u8                                  u8DataNum;                              //数据编号
    u8                                  u8DataStart;                            //数据起始字节偏移
    u8                                  u8DataL;                                //数据
    void*                               pSingleParemeter;                       //单个模块参数指针
    void*                               pGroupParemeter;                        //组模块参数指针(预留)
}stModDrvV2H_t;

extern stModDrvV2H_t stV2HMap[];

#define V2HMapMaxSize (sizeof(stV2HMap)/sizeof(stModDrvV2H_t))

#define V2H_DeviceSingle                0x0C                                    /*单播设备号*/
#define V2H_DeviceGroup                 0x0D                                    /*组播设备号*/

typedef struct stSingleData_t
{
    f32                                  f32DCVoltage;                           // 直流电压 mV
    f32                                  f32DCCurrent;                           // 直流电流 mA
    u8                                   u8ModuleStatus2;                        // 模块状态2
    u8                                   u8ModuleStatus1;                        // 模块状态1
    u8                                   u8ModuleStatus0;                        // 模块状态0
    u8                                   u8InverterStatus2;                      // 逆变状态2
    u8                                   u8InverterStatus1;                      // 逆变状态1
    u8                                   u8InverterStatus0;                      // 逆变状态0
    f32                                  f32PhaseAVoltage;                       // 交流A相相电压 mV
    f32                                  f32PhaseBVoltage;                       // 交流B相相电压 mV
    f32                                  f32PhaseCVoltage;                       // 交流C相相电压 mV
    f32                                  f32PhaseACurrent;                       // 交流A相相电流 mA
    f32                                  f32PhaseBCurrent;                       // 交流B相相电流 mA
    f32                                  f32PhaseCCurrent;                       // 交流C相相电流 mA
    f32                                  f32Frequency;                           // 交流频率 mHz
    f32                                  f32ActivePowerA;                        // A相有功功率 mW
    f32                                  f32ActivePowerB;                        // B相有功功率 mW
    f32                                  f32ActivePowerC;                        // C相有功功率 mW
    f32                                  f32ReactivePowerA;                      // A相无功功率 mVar
    f32                                  f32ReactivePowerB;                      // B相无功功率 mVar
    f32                                  f32ReactivePowerC;                      // C相无功功率 mVar
    f32                                  f32SetDCVoltage;                        // 设置直流电压 mV
    f32                                  f32SetDCCurrent;                        // 设置直流电流 mA
    u16                                  u16SetPowerOnOff;                          // 开关机状态
    f32                                  f32SetOffGridVoltageSetting;               // 离网电压设置 V
    f32                                  f32SetOffGridFrequencySetting;             // 离网频率设置 Hz
    f32                                  f32SetACReactivePowerSetting;              // AC侧无功功率设置 mVar
    u16                                  u16SetOperationMode;                       // 工作模式
    f32                                  f32SetActivePowerSetting;                // AC侧有功功率设置 mW
} stSingleData_t;


/**
 * @brief 组模块数据结构,预留
 */

stModDrvV2H_t* ModDrvV2HGetDataMap();
void ModDrvV2HUpdateSingleParameter(u8 u8Device, u8 u8Cmd, u8* pData);
u8 ModDrvV2HMetaDataGet(eDataIndex_t index, u8* pData);

/**
 * @}
 */
"#;

    let fields = extract_struct_fields(&contents);
    let mut file_c = OpenOptions::new()
        .create(true)
        .append(true)
        .open("c.md")?;

    let mut file_h = OpenOptions::new()
        .append(true)
        .create(true)
        .open("h.md")?;
        
    for field in fields.data.iter() {
        let mut function_name = field.name.clone();

        if function_name.contains("Set") {
            continue;
        }
        //删除名字中的类型
        function_name = function_name.replace(&field.data_type, "");
        //在名字前加上前缀ModDrvV2H
        function_name = format!("ModDrvV2H{}", function_name);

        let function_prototype = FUNCTION_PROTOTYPE
            .replace("[fun_name]", &function_name)
            .replace("[type]", &field.data_type);
        writeln!(file_h, "{}", function_prototype)?;

        let function = FUNCTION
            .replace("[fun_name]", &function_name)
            .replace("[name]", &field.name)
            .replace("[type]", &field.data_type)
            .replace("[comment]", &field.comment);
        writeln!(file_c, "{}",function)?;
    }

    Ok(())
}
