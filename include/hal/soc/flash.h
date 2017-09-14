/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_FLASH_H
#define AOS_FLASH_H

#define PAR_OPT_READ_POS      ( 0 )
#define PAR_OPT_WRITE_POS     ( 1 )

#define PAR_OPT_READ_MASK     ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_MASK    ( 0x1u << PAR_OPT_WRITE_POS )

#define PAR_OPT_READ_DIS      ( 0x0u << PAR_OPT_READ_POS )
#define PAR_OPT_READ_EN       ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_DIS     ( 0x0u << PAR_OPT_WRITE_POS )
#define PAR_OPT_WRITE_EN      ( 0x1u << PAR_OPT_WRITE_POS )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef enum {
    HAL_PARTITION_ERROR = -1,
    HAL_PARTITION_BOOTLOADER,
    HAL_PARTITION_APPLICATION,
    HAL_PARTITION_ATE,
    HAL_PARTITION_OTA_TEMP,
    HAL_PARTITION_RF_FIRMWARE,
    HAL_PARTITION_PARAMETER_1,
    HAL_PARTITION_PARAMETER_2,
    HAL_PARTITION_PARAMETER_3,
    HAL_PARTITION_PARAMETER_4,
    HAL_PARTITION_BT_FIRMWARE,
    HAL_PARTITION_MAX,
    HAL_PARTITION_NONE,
} hal_partition_t;

typedef enum {
    HAL_FLASH_EMBEDDED,
    HAL_FLASH_SPI,
    HAL_FLASH_QSPI,
    HAL_FLASH_MAX,
    HAL_FLASH_NONE,
} hal_flash_t;

typedef struct {
    hal_flash_t partition_owner;
    const char *partition_description;
    uint32_t    partition_start_addr;
    uint32_t    partition_length;
    uint32_t    partition_options;
} hal_logic_partition_t;


/**
 * Get the infomation of the specified flash area
 *
 * @param[in]  in_partition The target flash logical partition which should be erased
 *
 * @return     HAL_logi_partition struct
 */
hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition);


/**
 * Erase an area on a Flash logical partition
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return     0             On success.
 * @return     EIO           If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set,
                               uint32_t size);

/**
 * Write data to an area on a Flash logical partition
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to
 * @param[in]  inBuffer        Point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0                  On success.
 * @return  EIO                If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                               const void *in_buf , uint32_t in_buf_len);

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]   in_partition    The target flash logical partition which should be read
 * @param[in]   off_set         Point to the start address that the data is read
 * @param[out]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]   inBufferLength  The length of the buffer
 *
 * @return      0               On success.
 * @return      EIO             If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                              void *out_buf, uint32_t in_buf_len);

/**
 * Set security options on a logical partition
 *
 * @param[in]    partition  The target flash logical partition
 * @param[in]    offset     Point to the address
 * @param[in]    size       Size of enabled flash area
 *
 * @return       0          On success.
 * @return       EIO        If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set,
                                           uint32_t size);


/**
 * Disable security options on a logical partition
 *
 * @param[in]    partition  The target flash logical partition
 * @param[in]    offset     Point to the start address
 * @param[in]    size       Size of disabled flash area
 *
 * @return       0          On success.
 * @return       EIO        If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set,
                                      uint32_t size);

#endif

