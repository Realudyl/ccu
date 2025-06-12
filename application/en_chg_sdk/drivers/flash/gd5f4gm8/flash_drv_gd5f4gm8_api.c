/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     flash_drv_gd5f4gm8_api.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-9-13
 * @Attention             :     
 * @Brief                 :     GD5F4GM8 SPI NAND FLASH芯片驱动API
 * 
 * @History:
 * 
 * 1.@Date: 2024-9-13
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "flash_drv_gd5f4gm8_api.h"






extern stFlashDrvDevice_t stFlashDrvDev;





bool sFlashDrvApiGetInitState(void);




/*******************************************************************************
 * @FunctionName   :      sFlashDrvApiGetInitState
 * @Author         :      LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @DateTime       :      2024年9月13日  15:46:51
 * @Description    :      获取FLASH芯片初始化状态 API函数
 * @Input          :      void        
 * @Return         :      
*******************************************************************************/
bool sFlashDrvApiGetInitState(void)
{
    return(stFlashDrvDev.eInitState);
}











