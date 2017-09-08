/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <ali_crypto.h>
#include <yos/yos.h>

#include "stdio.h"

#include "umesh_types.h"
#include "core/crypto.h"
#include "core/keys_mgr.h"

typedef void *umesh_aes_ctx_t;

uint8_t g_umesh_iv[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static ur_error_t umesh_aes_encrypt_decrypt(umesh_aes_ctx_t *aes,
                                            const void *src,
                                            uint16_t size,
                                            void *dst)
{
    ali_crypto_result result;
    uint32_t dlen = 1024;

    if (aes == NULL) {
        return UR_ERROR_FAIL;
    }

    result = ali_aes_finish(src, size, dst, &dlen, SYM_NOPAD, aes);

    if (result != ALI_CRYPTO_SUCCESS) {
        return UR_ERROR_FAIL;
    }

    return UR_ERROR_NONE;
}

ur_error_t umesh_aes_encrypt(const uint8_t *key, uint8_t key_size,
                             const void *src,
                             uint16_t size, void *dst)
{
    ur_error_t error;
    umesh_aes_ctx_t *aes;
    uint32_t aes_ctx_size;
    ali_crypto_result result;

    if (key == NULL || src == NULL || dst == NULL) {
        return UR_ERROR_FAIL;
    }

    result = ali_aes_get_ctx_size(AES_CTR, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        return UR_ERROR_FAIL;
    }

    aes = aos_malloc(aes_ctx_size);
    if (aes == NULL) {
        return UR_ERROR_FAIL;
    }

    result = ali_aes_init(AES_CTR, true,
                          key, NULL, key_size, g_umesh_iv, aes);
    if (result != ALI_CRYPTO_SUCCESS) {
        aos_free(aes);
        return UR_ERROR_FAIL;
    }

    error = umesh_aes_encrypt_decrypt(aes, src, size, dst);
    aos_free(aes);

    return error;
}

ur_error_t umesh_aes_decrypt(const uint8_t *key, uint8_t key_size,
                             const void *src,
                             uint16_t size, void *dst)
{
    ur_error_t error;
    umesh_aes_ctx_t *aes;
    uint32_t aes_ctx_size;
    ali_crypto_result result;

    if (key == NULL || src == NULL || dst == NULL) {
        return UR_ERROR_FAIL;
    }

    result = ali_aes_get_ctx_size(AES_CTR, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        return UR_ERROR_FAIL;
    }

    aes = aos_malloc(aes_ctx_size);
    if (aes == NULL) {
        return UR_ERROR_FAIL;
    }

    result = ali_aes_init(AES_CTR, false,
                          key, NULL, key_size, g_umesh_iv, aes);
    if (result != ALI_CRYPTO_SUCCESS) {
        aos_free(aes);
        return UR_ERROR_FAIL;
    }

    error = umesh_aes_encrypt_decrypt(aes, src, size, dst);
    aos_free(aes);

    return error;
}
