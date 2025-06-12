pub const FUNCTION: &str = r#"/**
 * @breif 读取：[comment]
 */
[type] [fun_name](u8 u8Addr, u8 u8Device)
{
    stSingleData_t* data = ModDrvV2HGingleData();
    return data->[name];
}
"#;

pub const FUNCTION_PROTOTYPE: &str = r#"[type] [fun_name](u8 u8Addr, u8 u8Device);"#;
