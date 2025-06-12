/******************************************************************************
 
 Copyright © Shenzhen EN Plus Technologies Co., Ltd. 2015-2099. All rights reserved.
 
 ******************************************************************************
 * @File Name             :     hpav_security.c
 * @Version               :     V1.0
 * @Author                :     LiHongzhao <hongzhao.li@en-plus.com.cn>
 * @Date Created          :     2024-01-02
 * @Attention             :     从联芯通提供的<hpav_security.c>中移植并用mbedtls库替换openssl库
 * @Brief                 :     HomePlug AV Security
 * 
 * @History:
 * 
 * 1.@Date: 2024-1-2
 *   @Author: LiHongzhao <hongzhao.li@en-plus.com.cn>
 *   @Modify: 
 * 
******************************************************************************/
#include "mbedtls/sha256.h"

#include "hpav_security.h"
#include <string.h>










int hpav_pbkdf1_sha256(const char *password, unsigned int password_size, const unsigned char *salt, unsigned int salt_size, unsigned int iteration_count, unsigned char *result, unsigned int result_size);



int hpav_generate_key(const char *password, unsigned char salt[HPAV_PBKDF1_SALT_SIZE], unsigned char result[HPAV_AES_KEY_SIZE]);
int hpav_generate_nmk(const char *password, unsigned char result[HPAV_AES_KEY_SIZE]);
int hpav_generate_dak(const char *password, unsigned char result[HPAV_AES_KEY_SIZE]);
int hpav_generate_nid(const unsigned char nmk[HPAV_AES_KEY_SIZE], unsigned char security_level, unsigned char *result);








// Implementation of PBKDF1 with SHA256 as underlying hash function
// Password doesn't need to be a null terminated string at this level of the API
int hpav_pbkdf1_sha256(const char *password,            unsigned int password_size,
                       const unsigned char *salt,       unsigned int salt_size,
                       unsigned int iteration_count,    unsigned char *result,
                       unsigned int result_size)
{
    // Iterate (minimum of 1)
    unsigned char digest[SHA256_DIGEST_LENGTH];
    unsigned int iteration_index;
    
    mbedtls_sha256_context sha_ctx;
    mbedtls_sha256_init(&sha_ctx);
    mbedtls_sha256_starts(&sha_ctx, 0);
    mbedtls_sha256_update(&sha_ctx, (unsigned char *)password, password_size);
    mbedtls_sha256_update(&sha_ctx, salt, salt_size);
    mbedtls_sha256_finish(&sha_ctx, digest);
    
    for (iteration_index = 1; iteration_index < iteration_count; ++iteration_index) 
    {
        mbedtls_sha256_init(&sha_ctx);
        mbedtls_sha256_starts(&sha_ctx, 0);
        mbedtls_sha256_update(&sha_ctx, digest, SHA256_DIGEST_LENGTH);
        mbedtls_sha256_finish(&sha_ctx, digest);
    }
    
    if (result_size > SHA256_DIGEST_LENGTH)
    {
        // Not very interesting case for the caller
        memcpy(result, digest, SHA256_DIGEST_LENGTH);
    }
    else
    {
        memcpy(result, digest, result_size);
    }
    
    mbedtls_sha256_free(&sha_ctx);
    
    return 0;
}








int hpav_generate_key(const char *password, unsigned char salt[HPAV_PBKDF1_SALT_SIZE], unsigned char result[HPAV_AES_KEY_SIZE])
{
    // Iteration count for HPAV PBKDF1 is 1000
    unsigned int iteration_count = HPAV_PBKDF1_ITERATION_COUNT;
    
    return hpav_pbkdf1_sha256(password,           strlen(password),
                              salt,               HPAV_PBKDF1_SALT_SIZE,
                              iteration_count,    result,
                              HPAV_AES_KEY_SIZE);
}







// There are two functions as the salt value in the PBKDF1 algo is not the same
// for NMK and DAK
// Generate a NMK from a NPW
int hpav_generate_nmk(const char *password, unsigned char result[HPAV_AES_KEY_SIZE])
{
    // Set salt value for NMK (?.10.7.1 in HPAV 1.1 spec)
    unsigned char salt[HPAV_PBKDF1_SALT_SIZE] = {0x08, 0x85, 0x6D, 0xAF, 0x7C, 0xF5, 0x81, 0x86};
    
    return hpav_generate_key(password, salt, result);
}









// Generate a DAK from DPW
int hpav_generate_dak(const char *password, unsigned char result[HPAV_AES_KEY_SIZE])
{
    // Set salt value for DAK (?.10.7.1 in HPAV 1.1 spec)
    unsigned char salt[HPAV_PBKDF1_SALT_SIZE] = {0x08, 0x85, 0x6D, 0xAF, 0x7C, 0xF5, 0x81, 0x85};
    
    return hpav_generate_key(password, salt, result);
}








// Generate a NID from a NMK. Used to map NID and NMK.
int hpav_generate_nid(const unsigned char nmk[HPAV_AES_KEY_SIZE], unsigned char security_level, unsigned char *result)
{
    // No salt and iteration count 5 per HPAV spec
    int generation_result = hpav_pbkdf1_sha256((const char *)nmk,   HPAV_AES_KEY_SIZE,
                                                NULL,               0,
                                                5,                  result,
                                                HPAV_NID_SIZE);
    
    if (generation_result == 0)
    {
        // Put security level (see ?.4.3.1 in the HPAV spec) :
        // shift the four MSbits to the right by four and put the security level
        // in bits 4 and 5, and zero bits 6 and 7
        result[HPAV_NID_SIZE - 1] = (result[HPAV_NID_SIZE - 1] >> 4) | ((security_level & 0x3) << 4);
    }
    
    return generation_result;
}








