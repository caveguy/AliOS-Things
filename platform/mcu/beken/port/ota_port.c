#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

enum ota_parti_e
{
    OTA_PARTITION_KERNEL,
    OTA_PARTITION_APP,
    OTA_PARTITION_DEFAULT,
};

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} __attribute__((packed)) ota_hdl_t;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;

int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc )
{
    uint32_t offset;
    ota_hdl_t ota_hdl,ota_hdl_rb;
    hal_logic_partition_t* ota_partition;
    
    ota_partition = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );

    memset( &ota_hdl, 0, sizeof(ota_hdl_t) );    
    ota_hdl.dst_adr = parti == OTA_PARTITION_KERNEL ? 0x13200 : parti == OTA_PARTITION_APP ? 0x7C720 : 0x13200;
    ota_hdl.src_adr = ota_partition->partition_start_addr;
    ota_hdl.siz = ota_data_len;
    ota_hdl.crc = ota_data_crc;

    printf("OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n", 
    ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.siz, ota_hdl.crc);

    offset = 0x00;
    hal_flash_erase( HAL_PARTITION_PARAMETER_1, offset, sizeof(ota_hdl_t) );

    offset = 0x00;
    hal_flash_write( HAL_PARTITION_PARAMETER_1, &offset, (const void *)&ota_hdl, sizeof(ota_hdl_t));

    offset = 0x00;
    memset(&ota_hdl_rb, 0, sizeof(ota_hdl_t));
    hal_flash_read( HAL_PARTITION_PARAMETER_1, &offset, &ota_hdl_rb, sizeof(ota_hdl_t));

    if(memcmp(&ota_hdl, &ota_hdl_rb, sizeof(ota_hdl_t)) != 0)
    {
        printf("OTA header compare failed, OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n", 
        ota_hdl_rb.dst_adr, ota_hdl_rb.src_adr, ota_hdl_rb.siz, ota_hdl_rb.crc);
        return -1;
    }

    /* reboot */
    hal_reboot();

    return 0;
}

static  CRC16_Context contex;

unsigned int _off_set = 0;
static int moc108_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    printf("set ota init---------------\n");
    
    partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0 ,partition_info->partition_length);

    int _off_set = 0;
    CRC16_Init( &contex );
    return 0;
}


static int moc108_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    if(ota_info.ota_len == 0) {
        _off_set = 0;
        CRC16_Init( &contex );
        memset(&ota_info, 0 , sizeof ota_info);
    }
    CRC16_Update( &contex, in_buf, in_buf_len);
    int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
    ota_info.ota_len += in_buf_len;
    return ret;
}

static int moc108_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int moc108_ota_set_boot(hal_ota_module_t *m, void *something)
{
    uint8_t parti = *(uint8_t *)something;
    CRC16_Final( &contex, &ota_info.ota_crc );
    printf("set boot---------------\n");
    hal_ota_switch_to_new_fw(parti, ota_info.ota_len, ota_info.ota_crc);
    memset(&ota_info, 0 , sizeof ota_info);
    return 0;
}

struct hal_ota_module_s moc108_ota_module = {
    .init = moc108_ota_init,
    .ota_write = moc108_ota_write,
    .ota_read = moc108_ota_read,
    .ota_set_boot = moc108_ota_set_boot,
};
