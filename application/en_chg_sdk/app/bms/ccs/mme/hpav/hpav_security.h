/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     hpav_security.h
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2023-12-29
 * @Attention             :     从联芯通提供的<hpav_api.h>中移植并整理
 * @Brief                 :     HomePlug AV Security
 * 
 * @History:
 * 
 * 1.@Date: 2023-12-29
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#ifndef _hpav_security_H
#define _hpav_security_H









#define HPAV_PBKDF1_SALT_SIZE           8
#define HPAV_PBKDF1_ITERATION_COUNT     1000

#define SHA256_DIGEST_LENGTH            32

#define HPAV_NID_SIZE                   7

// Security functions
#define HPAV_AES_KEY_SIZE               16













extern int hpav_generate_key(const char *password, unsigned char salt[HPAV_PBKDF1_SALT_SIZE], unsigned char result[HPAV_AES_KEY_SIZE]);
extern int hpav_generate_nmk(const char *password, unsigned char result[HPAV_AES_KEY_SIZE]);
extern int hpav_generate_dak(const char *password, unsigned char result[HPAV_AES_KEY_SIZE]);
extern int hpav_generate_nid(const unsigned char nmk[HPAV_AES_KEY_SIZE], unsigned char security_level, unsigned char *result);




















#endif

