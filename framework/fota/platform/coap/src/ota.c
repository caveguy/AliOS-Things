/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "iot_import_ota.h"
#include "iot_export_ota.h"
#include "ota_internal.h"


#include "ota_lib.c"

//#if defined (OTA_CH_SIGNAL_MQTT)
//#include "ota_mqtt.c"
#if defined (OTA_CH_SIGNAL_COAP)
#include "ota_coap.c"
#else
#error "NOT support yet!"
#endif

#if (OTA_CH_FETCH_HTTP) != 0
#include "ota_fetch.c"
#else
#error "NOT support yet!"
#endif

typedef struct  {
    const char *product_key;    //point to product key
    const char *device_name;    //point to device name

    uint32_t id;                //message id
    IOT_OTA_State_t state;          //OTA state
    uint32_t size_last_fetched; //size of last downloaded
    uint32_t size_fetched;      //size of already downloaded
    uint32_t size_file;         //size of file
    char *purl;                 //point to URL
    char *version;              //point to string
    char md5sum[33];            //MD5 string

    void *md5;                  //MD5 handle
    void *ch_signal;            //channel handle of signal exchanged with OTA server
    void *ch_fetch;             //channel handle of download

    IOT_OTA_Err_t err;              //last error code

} OTA_Struct_t, *OTA_Struct_pt;

#define OTA_BUF_LEN        (500)

static int fetch_ota(void *h_ota)
{
    int rc = 1;
    FILE *fp;
    uint32_t last_percent = 0, percent = 0;
    char version[128], md5sum[33];
    int32_t len, size_downloaded, size_file;
    char buf_ota[OTA_BUF_LEN];
    int32_t firmware_valid;

    if (NULL == (fp = fopen("ota.bin", "wb+"))) {
        OTA_LOG_ERROR("open file failed");
        return -1;
    }

    do {
        len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
        if (len > 0) {
            if (1 != fwrite(buf_ota, len, 1, fp)) {
                OTA_LOG_ERROR("write data to file failed");
                rc = -1;
                break;
            }
        } else if (len < 0) {
            OTA_LOG_ERROR("fetch data failed");
            rc = -1;
            break;
        }

        /* get OTA information */
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);

        last_percent = percent;
        percent = (size_downloaded * 100) / size_file;
        OTA_LOG_ERROR("fetch percent = %d",percent);
        if (percent - last_percent > 0) {
            IOT_OTA_ReportProgress(h_ota, percent, NULL);
            IOT_OTA_ReportProgress(h_ota, percent, "hello");
        }

    }while(!IOT_OTA_IsFetchFinish(h_ota));

    while (1 == rc) {
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
        if (0 == firmware_valid) {
            OTA_LOG_ERROR("The firmware is invalid");
            rc = -1;
            break;
        } else {
            OTA_LOG_INFO("The firmware is valid");
            // rc = -1;
            break;
        }
    }

    if (NULL != fp) {
        fclose(fp);
    }

    return rc;
}


//-1, fetch failed
//0, no any ota firmware
//1, fetch successfully
int try_fetch_ota(void *h_ota)
{
    if (IOT_OTA_IsFetching(h_ota)) {
        return fetch_ota(h_ota);
    }

    return 0;
}

//check whether the progress state is valid or not
//return: true, valid progress state; false, invalid progress state.
static bool ota_check_progress(IOT_OTA_Progress_t progress)
{
    return ((progress >= IOT_OTAP_BURN_FAILED)
            && (progress <= IOT_OTAP_FETCH_PERCENTAGE_MAX));
}

static void ota_callback(void *pcontext, const char *msg, uint32_t msg_len)
{
    const char *pvalue;
    uint32_t val_len;

    OTA_Struct_pt h_ota = (OTA_Struct_pt) pcontext;

    if (h_ota->state >= IOT_OTAS_FETCHING) {
        OTA_LOG_INFO("In downloading or downloaded state");
        return;
    }

    pvalue = otalib_JsonValueOf(msg, msg_len, "message", &val_len);
    if (NULL == pvalue) {
        OTA_LOG_ERROR("invalid json doc of OTA ");
        return;
    }

    //check whether is positive message
    if (!((strlen("success") == val_len) && (0 == strncmp(pvalue, "success", val_len)))) {
        OTA_LOG_ERROR("fail state of json doc of OTA");
        return ;
    }

    //get value of 'data' key
    pvalue = otalib_JsonValueOf(msg, msg_len, "data", &val_len);
    if (NULL == pvalue) {
        OTA_LOG_ERROR("Not 'data' key in json doc of OTA");
        return;
    }

    if (0 != otalib_GetParams(pvalue, val_len, &h_ota->purl, &h_ota->version, h_ota->md5sum, &h_ota->size_file)) {
        OTA_LOG_ERROR("Get firmware parameter failed");
        return;
    }

    if (NULL == (h_ota->ch_fetch = ofc_Init(h_ota->purl))) {
        OTA_LOG_ERROR("Initialize fetch module failed");
        return ;
    }

    h_ota->state = IOT_OTAS_FETCHING;
}


//Initialize OTA module
void *IOT_OTA_Init(const char *product_key, const char *device_name, void *ch_signal)
{
    OTA_Struct_pt h_ota = NULL;

    if ((NULL == product_key) || (NULL == device_name) || (NULL == ch_signal)) {
        OTA_LOG_ERROR("one or more parameters is invalid");
        return NULL;
    }

    if (NULL == (h_ota = OTA_MALLOC(sizeof(OTA_Struct_t)))) {
        OTA_LOG_ERROR("allocate failed");
        return NULL;
    }
    memset(h_ota, 0, sizeof(OTA_Struct_t));
    h_ota->state = IOT_OTAS_UNINITED;

    h_ota->ch_signal = osc_Init(product_key, device_name, ch_signal, ota_callback, h_ota);
    if (NULL == h_ota->ch_signal) {
        OTA_LOG_ERROR("initialize signal channel failed");
        goto do_exit;
    }

    h_ota->md5 = otalib_MD5Init();
    if (NULL == h_ota->md5) {
        OTA_LOG_ERROR("initialize md5 failed");
        goto do_exit;
    }

    h_ota->product_key = product_key;
    h_ota->device_name = device_name;
    h_ota->state = IOT_OTAS_INITED;
    return h_ota;

do_exit:

    if (NULL != h_ota->ch_signal) {
        osc_Deinit(h_ota->ch_signal);
    }
    
    if (NULL != h_ota->md5) {
        otalib_MD5Deinit(h_ota->md5);
    }

    if (NULL != h_ota) {
        OTA_FREE(h_ota);
    }

    return NULL;

#undef AOM_INFO_MSG_LEN
}


//deinitialize OTA module
int IOT_OTA_Deinit(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == h_ota) {
        OTA_LOG_ERROR("handle is NULL");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    osc_Deinit(h_ota->ch_signal);
    ofc_Deinit(h_ota->ch_fetch);
    otalib_MD5Deinit(h_ota->md5);

    if (NULL != h_ota->purl) {
        OTA_FREE(h_ota->purl);
    }

    if (NULL != h_ota->version) {
        OTA_FREE(h_ota->version);
    }

    OTA_FREE(h_ota);
    return 0;
}


#define OTA_VERSION_STR_LEN_MIN     (1)
#define OTA_VERSION_STR_LEN_MAX     (32)

int IOT_OTA_ReportVersion(void *handle, const char *version)
{
#define MSG_INFORM_LEN  (128)

    int ret, len;
    char *msg_informed;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == h_ota) || (NULL == version)) {
        OTA_LOG_ERROR("one or more invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        OTA_LOG_ERROR("version string is invalid: must be [1, 32] chars");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (NULL == (msg_informed = OTA_MALLOC(MSG_INFORM_LEN))) {
        OTA_LOG_ERROR("malloc failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, h_ota->id, version);
    if (ret != 0) {
        OTA_LOG_ERROR("generate inform message failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }

    ret = osc_ReportVersion(h_ota->ch_signal, msg_informed);
    if (0 != ret) {
        OTA_LOG_ERROR("Report version failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }
    ret = 0;

do_exit:
    if (NULL != msg_informed) {
        OTA_FREE(msg_informed);
    }
    return ret;

#undef MSG_INFORM_LEN
}


int IOT_OTA_ReportProgress(void *handle, IOT_OTA_Progress_t progress, const char *msg)
{
#define MSG_REPORT_LEN  (256)

    int ret = -1;
    char *msg_reported;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (!ota_check_progress(progress)){
        OTA_LOG_ERROR("progress is a invalid parameter");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    if (NULL == (msg_reported = OTA_MALLOC(MSG_REPORT_LEN))) {
        OTA_LOG_ERROR("malloc failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    ret = otalib_GenReportMsg(msg_reported, MSG_REPORT_LEN, h_ota->id, progress, msg);
    if (0 != ret) {
        OTA_LOG_ERROR("generate reported message failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = osc_ReportProgress(h_ota->ch_signal, msg_reported);
    if (0 != ret) {
        OTA_LOG_ERROR("Report progress failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = 0;

do_exit:
    if (NULL != msg_reported) {
        OTA_FREE(msg_reported);
    }
    return ret;

#undef MSG_REPORT_LEN
}


//check whether is downloading
bool IOT_OTA_IsFetching(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt)handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return 0;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHING == h_ota->state);
}


//check whether fetch over
bool IOT_OTA_IsFetchFinish(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return 0; 
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHED == h_ota->state);
}


int IOT_OTA_FetchYield(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_ms)
{
    int ret;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == handle) || (NULL == buf) || (0 == buf_len)) {
        OTA_LOG_ERROR("invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_FETCHING != h_ota->state) {
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return IOT_OTAE_INVALID_STATE;
    }

    ret = ofc_Fetch(h_ota->ch_fetch, buf, buf_len, timeout_ms);
    if (ret < 0) {
        OTA_LOG_ERROR("Fetch firmware failed");
        h_ota->state = IOT_OTAS_FETCHED;
        h_ota->err = IOT_OTAE_FETCH_FAILED;
        return -1;
    }

    h_ota->size_last_fetched = ret;
    h_ota->size_fetched += ret;

    if (h_ota->size_fetched >= h_ota->size_file) {
        h_ota->state = IOT_OTAS_FETCHED;
    }

    otalib_MD5Update(h_ota->md5, buf, ret);

    return ret;
}


int IOT_OTA_Ioctl(void *handle, IOT_OTA_CmdType_t type, void *buf, size_t buf_len)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == handle) || (NULL == buf) || (0 == buf_len)) {
        OTA_LOG_ERROR("invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (h_ota->state < IOT_OTAS_FETCHING) {
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return IOT_OTAE_INVALID_STATE;
    }

    switch( type ) {
    case IOT_OTAG_FETCHED_SIZE:
        if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
            OTA_LOG_ERROR("Invalid parameter");
            h_ota->err = IOT_OTAE_INVALID_PARAM;
            return -1;
        } else {
            *((uint32_t *)buf) = h_ota->size_fetched;
            return 0;
        }
        break;

    case IOT_OTAG_FILE_SIZE:
        if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
            OTA_LOG_ERROR("Invalid parameter");
            h_ota->err = IOT_OTAE_INVALID_PARAM;
            return -1;
        } else {
            *((uint32_t *)buf) = h_ota->size_file;
            return 0;
        };
        break;

    case IOT_OTAG_VERSION:
        strncpy(buf, h_ota->version, buf_len);
        ((char *)buf)[buf_len-1] = '\0';
        break;

    case IOT_OTAG_MD5SUM:
        strncpy(buf, h_ota->md5sum, buf_len);
        ((char *)buf)[buf_len-1] = '\0';
        break;

    case IOT_OTAG_CHECK_FIRMWARE:
        if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
            OTA_LOG_ERROR("Invalid parameter");
            h_ota->err = IOT_OTAE_INVALID_PARAM;
            return -1;
        } else if (h_ota->state != IOT_OTAS_FETCHED) {
            h_ota->err = IOT_OTAE_INVALID_STATE;
            OTA_LOG_ERROR("Firmware can be checked in IOT_OTAS_FETCHED state only");
            return -1;
        } else {
            char md5_str[33];
            otalib_MD5Finalize(h_ota->md5, md5_str);
            OTA_LOG_DEBUG("origin=%s, now=%s", h_ota->md5sum, md5_str);
            if (0 == strcmp(h_ota->md5sum, md5_str)) {
                *((uint32_t *)buf) = 1;
            } else {
                *((uint32_t *)buf) = 0;
            }
            return 0;
        }

    default:
        OTA_LOG_ERROR("invalid cmd type");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    return 0;
}


//Get last error code
IOT_OTA_Err_t IOT_OTA_GetLastError(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return  IOT_OTAE_INVALID_PARAM;
    }

    return h_ota->err;
}

