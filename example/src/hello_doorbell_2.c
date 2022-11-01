/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Agora Lab, Inc (http://www.agora.io/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <termio.h>
#include <sys/time.h>

#include "agora_iot_api.h"
#include "agora_iot_call.h"
#include "agora_iot_dp.h"
#include "agora_iot_device_manager.h"

#include "cJSON.h"

#include "app_config.h"
#include "hello_doorbell_comm.h"
#include "device_state.h"

#include "warning_jpeg.h"

/* Alarm type start */
#define AGORA_ALARM_TYPE_VAD       0   // Voice detection
#define AGORA_ALARM_TYPE_MOD       1   // Motion detection
#define AGORA_ALARM_TYPE_PIR_PASS  2   // PIR pass
#define AGORA_ALARM_TYPE_BUTTON    3   // Button pushed
/* Alarm type end */

/* Data Point ID start */
/* AGORA_DP_ID_<TYPE>_<NAME> */
#define AGORA_DP_ID_BOOL_OSD_WATERMARK_SWITCH       100
#define AGORA_DP_ID_ENUM_INFRARED_NIGHT_VISION      101
#define AGORA_DP_ID_BOOL_MOVING_WARNING             102
#define AGORA_DP_ID_ENUM_PIR_SWITCH                 103
#define AGORA_DP_ID_INT_VOLUME_CONTROL              104
#define AGORA_DP_ID_BOOL_FORCE_RELEASING_WARNING    105
#define AGORA_DP_ID_INT_BATTERY_CAPACITY            106
#define AGORA_DP_ID_ENUM_VIDEO_ART                  107
#define AGORA_DP_ID_BOOL_WORK_LED                   108
#define AGORA_DP_ID_STR_FIRMWARE_VER                109
#define AGORA_DP_ID_ENUM_TF_STATE                   110
#define AGORA_DP_ID_INT_TF_STORAGE_AVAILABLE        111
#define AGORA_DP_ID_ENUM_TF_FORMAT_CTRL             112
#define AGORA_DP_ID_INT_PREVIEW_TIME                113
#define AGORA_DP_ID_BOOL_ALRAM_SONG                 114
#define AGORA_DP_ID_BOOL_VOICE_SENSE                115

#define AGORA_DP_ID_STR_WIFI_SSID                   501
#define AGORA_DP_ID_STR_DEVICE_IP                   502
#define AGORA_DP_ID_STR_DEVICE_MAC                  503
#define AGORA_DP_ID_STR_TIME_ZONE                   504

#define AGORA_DP_ID_ENUM_STANDBY_STATE              1000
/* Data Point ID end */

#define STR_LEN 32
static char g_str_firmware_ver[STR_LEN] = "ver.0.0.1";
static char g_str_wifi_ssid[STR_LEN] = "AG_IOT_WIFI";
static char g_str_device_ip[STR_LEN] = "192.168.100.100";
static char g_str_device_mac[STR_LEN] = "01:23:45:AB:CD:EF";
static char g_str_time_zone[STR_LEN] = "GTM+8";

typedef enum {
  SYS_UP_MODE_NONE = 0,
  SYS_UP_MODE_RESTORE,
  SYS_UP_MODE_POWERON,
  SYS_UP_MODE_WAKEUP,
  SYS_UP_MODE_MAX
} sys_up_mode_e;

sys_up_mode_e up_mode = SYS_UP_MODE_POWERON;
device_handle_t g_dev_state = NULL;
app_t g_app = {
  .iot_handle = NULL,
  .b_login_success = false,
  .b_push_thread_run = false,
  .b_exit = false,
  .video_thread_id = 0,
  .audio_thread_id = 0,
};

/* The mode of the data point is decided by the Application */
typedef enum doorbell_dp_info_mode {
  MODE_R, // read only
  MODE_RW // read & write
} doorbell_dp_info_mode_e;
typedef struct doorbell_dp_info {
  doorbell_dp_info_mode_e mode;
  agora_dp_info_t info;
} doorbell_dp_info_t;

// Just for test
extern char g_rtm_peer_uid[64];

static doorbell_dp_info_t g_mock_dp_state[] = {
  /* DP ID: 100 ~ 1** */
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_OSD_WATERMARK_SWITCH, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_ENUM_INFRARED_NIGHT_VISION, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_enum = 0, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_MOVING_WARNING, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_ENUM_PIR_SWITCH, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_enum = 0, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_INT_VOLUME_CONTROL, .info.dp_type = AGORA_DP_TYPE_INT, .info.dp_value.dp_int = 50, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_FORCE_RELEASING_WARNING, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_INT_BATTERY_CAPACITY, .info.dp_type = AGORA_DP_TYPE_INT, .info.dp_value.dp_int = 50, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_ENUM_VIDEO_ART, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_enum = 1, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_WORK_LED, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_STR_FIRMWARE_VER, .info.dp_type = AGORA_DP_TYPE_STR, .info.dp_value.dp_str = g_str_firmware_ver},
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_ENUM_TF_STATE, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_enum = 1},
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_INT_TF_STORAGE_AVAILABLE, .info.dp_type = AGORA_DP_TYPE_INT, .info.dp_value.dp_int = 1024},
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_ENUM_TF_FORMAT_CTRL, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_enum = 1},
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_INT_PREVIEW_TIME, .info.dp_type = AGORA_DP_TYPE_INT, .info.dp_value.dp_int = 60},
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_ALRAM_SONG, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_BOOL_VOICE_SENSE, .info.dp_type = AGORA_DP_TYPE_BOOL, .info.dp_value.dp_bool = false, },
  /* DP ID: 500 ~  */
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_STR_WIFI_SSID, .info.dp_type = AGORA_DP_TYPE_STR, .info.dp_value.dp_str = g_str_wifi_ssid},
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_STR_DEVICE_IP, .info.dp_type = AGORA_DP_TYPE_STR, .info.dp_value.dp_str = g_str_device_ip},
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_STR_DEVICE_MAC, .info.dp_type = AGORA_DP_TYPE_STR, .info.dp_value.dp_str = g_str_device_mac},
  { .mode = MODE_R, .info.dp_id = AGORA_DP_ID_STR_TIME_ZONE, .info.dp_type = AGORA_DP_TYPE_STR, .info.dp_value.dp_str = g_str_time_zone},
  { .mode = MODE_RW, .info.dp_id = AGORA_DP_ID_ENUM_STANDBY_STATE, .info.dp_type = AGORA_DP_TYPE_ENUM, .info.dp_value.dp_int = 2, },
};
static int g_mock_dp_state_total = sizeof(g_mock_dp_state) / sizeof(doorbell_dp_info_t);

static void _query_callback(agora_dp_info_t *info, void *args)
{
  printf("------query DP[%d], type[%d]", info->dp_id, info->dp_type);

  for (int i = 0; i < g_mock_dp_state_total; i++) {
    if (info->dp_id == g_mock_dp_state[i].info.dp_id) {
      if (info->dp_type == g_mock_dp_state[i].info.dp_type) {
        info->dp_value = g_mock_dp_state[i].info.dp_value;

        switch (g_mock_dp_state[i].info.dp_type) {
        case AGORA_DP_TYPE_INT:
          printf(", value[%d]\n", info->dp_value.dp_int);
          return;
        case AGORA_DP_TYPE_BOOL:
          printf(", value[%s]\n", info->dp_value.dp_bool ? "true" : "false");
          return;
        case AGORA_DP_TYPE_ENUM:
          printf(", value[%d]\n", info->dp_value.dp_enum);
          return;
        case AGORA_DP_TYPE_STR:
          printf(", value[%s]\n", info->dp_value.dp_str ? info->dp_value.dp_str : "null");
          return;
        default:
          /* This case should not be ran */
          printf("\n");
          return;
        }
      } else {
        printf("\n------and the type does't match to the type[%d]\n", g_mock_dp_state[i].info.dp_type);
        return;
      }
    }
  }

  printf("\n------and cann't find the DP");
}

static void _cmd_callback(const agora_dp_info_t *info, void *args)
{
  printf("------control DP[%d], type[%d]", info->dp_id, info->dp_type);

  for (int i = 0; i < g_mock_dp_state_total; i++) {
    if (info->dp_id == g_mock_dp_state[i].info.dp_id) {
      if (MODE_RW != g_mock_dp_state[i].mode) {
        printf("\n------the data point is read only\n");
        return;
      }

      if (info->dp_type == g_mock_dp_state[i].info.dp_type) {
        switch (g_mock_dp_state[i].info.dp_type) {
        case AGORA_DP_TYPE_INT:
          g_mock_dp_state[i].info.dp_value.dp_int = info->dp_value.dp_int;
          printf(", value[%d]\n", g_mock_dp_state[i].info.dp_value.dp_int);
          return;
        case AGORA_DP_TYPE_BOOL:
          g_mock_dp_state[i].info.dp_value.dp_bool = info->dp_value.dp_bool;
          printf(", value[%s]\n", g_mock_dp_state[i].info.dp_value.dp_bool ? "true" : "false");
          return;
        case AGORA_DP_TYPE_ENUM:
          g_mock_dp_state[i].info.dp_value.dp_enum = info->dp_value.dp_enum;
          printf(", value[%d]\n", g_mock_dp_state[i].info.dp_value.dp_enum);
          return;
        case AGORA_DP_TYPE_STR:
          memset(g_mock_dp_state[i].info.dp_value.dp_str, 0, STR_LEN);
          snprintf(g_mock_dp_state[i].info.dp_value.dp_str, STR_LEN, "%s", info->dp_value.dp_str);
          printf(", value[%s]\n", g_mock_dp_state[i].info.dp_value.dp_str ?
                  g_mock_dp_state[i].info.dp_value.dp_str : "null");
          return;
        default:
          /* This case should not be ran */
          printf("\n");
          return;
        }
      } else {
        printf("\n------and the type does't match to the type[%d]\n", g_mock_dp_state[i].info.dp_type);
        return;
      }
    }
  }

  printf("\n------and cann't find the DP");
}

static char g_device_id[32] = { 0 };
static const char *mock_get_device_id(void)
{
  // TODO: should get device from system at here
  if (g_device_id[0] == 0) {
    strncpy(g_device_id, CONFIG_DEVICE_ID, sizeof(g_device_id));
  }
  return g_device_id;
}

#define STATE_FILE_NAME "device_state.cfg"
#define STATE_FILE_SIZE_MAX (1024 * 16)
static device_handle_t mock_load_devic_state(void)
{
  device_handle_t dev_state = NULL;
  FILE *state_file = NULL;

  // malloc buffer to read file
  char *file_buf = (char *)malloc(STATE_FILE_SIZE_MAX);
  if (NULL == file_buf) {
    printf("malloc failed !\n");
    goto load_state_err;
  }
  memset(file_buf, 0, STATE_FILE_SIZE_MAX);

  // read state config from config file, you can save it to falsh on your board
  state_file = fopen(STATE_FILE_NAME, "rt+");
  if (state_file) {
    // device was activated, read state item from file
    fread(file_buf, 1, STATE_FILE_SIZE_MAX, state_file);
    dev_state = device_load_state(file_buf);
    if (NULL == dev_state) {
      printf("device_state.cfg is invalid !\n");
      goto load_state_err;
    }
  }

load_state_err:
  if (state_file) {
    fclose(state_file);
  }
  if (file_buf) {
    free(file_buf);
  }
  return dev_state;
}

static int mock_save_device_state(device_handle_t dev_state)
{
  FILE *state_file = fopen(STATE_FILE_NAME, "wt+");
  if (NULL == state_file) {
    printf("Cannot create device_state.cfg !\n");
    return -1;
  }
  const char *state_contect = device_build_state_content(dev_state);
  if (NULL == state_contect) {
    printf("device_build_state_content failed !\n");
    fclose(state_file);
    return -1;
  }
  fwrite(state_contect, 1, strlen(state_contect), state_file);
  fclose(state_file);
  free((void *)state_contect);
  return 0;
}

static int parse_qrcode_content(device_handle_t dev_state, const char *content)
{
  int ret = -1;

  cJSON *root = cJSON_Parse(content);
  if (NULL == root) {
    printf("#### cannot parse QRcode: %s\n", content);
    goto qrcode_parse_err;
  }
  // get ssid
  cJSON *item = cJSON_GetObjectItemCaseSensitive(root, "s");
  if (cJSON_GetStringValue(item)) {
    device_set_item_string(dev_state, "ssid", cJSON_GetStringValue(item));
  } else {
    printf("#### cannot found ssid in QRcode !\n");
  }
  item = NULL;

  // get pws
  item = cJSON_GetObjectItemCaseSensitive(root, "p");
  if (cJSON_GetStringValue(item)) {
    device_set_item_string(dev_state, "password", cJSON_GetStringValue(item));
  } else {
    printf("#### cannot found password in QRcode !\n");
  }
  item = NULL;

  // get product key
  item = cJSON_GetObjectItemCaseSensitive(root, "k");
  if (cJSON_GetStringValue(item)) {
    device_set_item_string(dev_state, "product_key", cJSON_GetStringValue(item));
  } else {
    printf("#### cannot found  product key in QRcode !\n");
  }
  item = NULL;

  // get user id
  item = cJSON_GetObjectItemCaseSensitive(root, "u");
  if (cJSON_GetStringValue(item)) {
    device_set_item_string(dev_state, "user_id", cJSON_GetStringValue(item));
  } else {
    printf("#### cannot found user id in QRcode !\n");
  }
  item = NULL;

  // get device name
  item = cJSON_GetObjectItemCaseSensitive(root, "n");
  if (cJSON_GetStringValue(item)) {
    device_set_item_string(dev_state, "device_name", cJSON_GetStringValue(item));
  } else {
    printf("#### cannot found device_name in QRcode, use the default: %s !\n", mock_get_device_id());
    device_set_item_string(dev_state, "device_name", mock_get_device_id());
  }
  item = NULL;

  ret = 0;

qrcode_parse_err:
  if (root) {
    cJSON_Delete(root);
  }
  return ret;
}

static int mock_network_config(device_handle_t dev_state)
{
  // input QRcode content, need to got it by scan QRcode on your board
  char qrcode_content[512 + 1] = { 0 };
  printf("\n\n------------------ Please input QRcode string with JSON type ----------------------\n");
  scanf("%512s", qrcode_content);
  printf("-------------------- Got string and parse it now ------------------------------------\n");

  // parse content
  return parse_qrcode_content(dev_state, qrcode_content);
}

static int mock_connect_network(char *ssid, char *psw)
{
  // get ssid and pws from config file directly, you have to do it on your board

  // get NTP time and fet to system, you have to do it on your board

  return 0;
}

static int activate_device(device_handle_t dev_state)
{
  int ret = -1;
  char *user_id = NULL;
  char *device_name = NULL;
  char *crt_buf = NULL;
  char *key_buf = NULL;
  const char *device_id = mock_get_device_id();

  // 1. activate license
  char *cert = NULL;
  if (0 != agora_iot_license_activate(CONFIG_AGORA_APP_ID, CONFIG_CUSTOMER_KEY, CONFIG_CUSTOMER_SECRET,
                                      CONFIG_PRODUCT_KEY, device_id, CONFIG_LICENSE_PID, &cert)) {
    printf("cannot activate agora license !\n");
    goto activate_err;
  }
  device_set_item_string(dev_state, "license", cert);

  // 2. register DP service
  if (0 != device_get_item_string(dev_state, "user_id", &user_id)) {
    printf("cannot found user_id from device state.\n");
    goto activate_err;
  }
  if (0 != device_get_item_string(dev_state, "device_name", &device_name)) {
    printf("cannot found user_id from device state.\n");
    goto activate_err;
  }
  crt_buf = (char *)malloc(CERT_BUF_SIZE_MAX);
  key_buf = (char *)malloc(CERT_BUF_SIZE_MAX);
  if (!crt_buf || !key_buf) {
    printf("cannot malloc buffer for register.\n");
    goto activate_err;
  }
  memset(crt_buf, 0, CERT_BUF_SIZE_MAX);
  memset(key_buf, 0, CERT_BUF_SIZE_MAX);
  agora_iot_device_info_t device_info = { 0 };
  if (0 != agora_iot_register_and_bind(CONFIG_MASTER_SERVER_URL, CONFIG_PRODUCT_KEY, device_id,
                                      user_id, device_name, &device_info)) {
    printf("register device to aws failure\n");
    goto activate_err;
  }
  device_set_item_string(dev_state, "domain", device_info.domain);
  device_set_item_string(dev_state, "dev_crt", device_info.certificate);
  device_set_item_string(dev_state, "dev_key", device_info.private_key);
  device_set_item_string(dev_state, "client_id", device_info.client_id);

  // 3. get bind user info
  char user_account[AGORA_IOT_CLIENT_ID_MAX_LEN] = { 0 };
  int rval = agora_iot_query_user(CONFIG_MASTER_SERVER_URL, CONFIG_PRODUCT_KEY, device_id, user_account, AGORA_IOT_CLIENT_ID_MAX_LEN);
  if (0 != rval) {
    printf("query device manager user failed\n");
    goto activate_err;
  }
  device_set_item_string(dev_state, "bind_user", user_account);

  ret = 0;

activate_err:
  if (cert) {
    free(cert);
  }
  if (user_id) {
    free(user_id);
  }
  if (device_name) {
    free(device_name);
  }
  if (crt_buf) {
    free(crt_buf);
  }
  if (key_buf) {
    free(key_buf);
  }
  return ret;
}

static int update_device_work_state(agora_iot_handle_t handle, sys_up_mode_e mode)
{
  /* Register the query callback for every data point, and only register the command callback for some data points */
  for (int i = 0; i < g_mock_dp_state_total; i++) {
    agora_iot_dp_register_dp_query_handler(handle, g_mock_dp_state[i].info.dp_id, g_mock_dp_state[i].info.dp_type,
                                            _query_callback, (void *)handle);
    if (MODE_RW == g_mock_dp_state[i].mode) {
      agora_iot_dp_register_dp_cmd_handler(handle, g_mock_dp_state[i].info.dp_id, g_mock_dp_state[i].info.dp_type,
                                            _cmd_callback, (void *)handle);
    }
  }

  /* have to update at last once to create shadow, do it in SDK */

  if (SYS_UP_MODE_WAKEUP != mode) {
    agora_iot_dp_publish_all(handle);
  } else {
    agora_dp_info_t dp_info = { 0 };
    dp_info.dp_id = AGORA_DP_ID_ENUM_STANDBY_STATE;
    dp_info.dp_type = AGORA_DP_TYPE_ENUM;
    dp_info.dp_value.dp_int = 2;
    agora_iot_dp_publish(handle, &dp_info);
  }

  return 0;
}

static void update_device_low_power(agora_iot_handle_t handle)
{
  agora_dp_info_t dp_info = { 0 };
  dp_info.dp_id = AGORA_DP_ID_ENUM_STANDBY_STATE;
  dp_info.dp_type = AGORA_DP_TYPE_ENUM;
  dp_info.dp_value.dp_int = 1;
  agora_iot_dp_publish(handle, &dp_info);
}

static agora_iot_handle_t mock_device_bringup(sys_up_mode_e mode)
{
  char *ssid = NULL;
  char *psw = NULL;
  char *license = NULL;

  // 1. load device state config
  g_dev_state = mock_load_devic_state();

  // 2. do network config process
  if (NULL == g_dev_state) {
    if (SYS_UP_MODE_WAKEUP == mode) {
      printf("device state config must be here is system was wakeup from low-poer mode");
      goto dev_bringup_err;
    }
    g_dev_state = device_create_state();
    if (NULL == g_dev_state) {
      printf("cannot create device state items !\n");
      goto dev_bringup_err;
    }
    if (0 != mock_network_config(g_dev_state)) {
      printf("config network failed!\n");
      goto dev_bringup_err;
    }
  }

  // 3. connect network (TODO: maybe need not do it on some system)
  if (0 != device_get_item_string(g_dev_state, "ssid", &ssid) ||
      0 != device_get_item_string(g_dev_state, "password", &psw)) {
    printf("cannot found ssid and password !\n");
    goto dev_bringup_err;
  }
  if (0 != mock_connect_network(ssid, psw)) {
    printf("cannot connect network !\n");
    goto dev_bringup_err;
  }

  // 4. activate the device
  char user_account[AGORA_IOT_CLIENT_ID_MAX_LEN] = { 0 };
  if (SYS_UP_MODE_WAKEUP != mode) {
    // maybe nee to avtiate device if was not wakeup frome low-power mode
    if (SYS_UP_MODE_RESTORE == mode
        || 0 != agora_iot_query_user(CONFIG_MASTER_SERVER_URL, CONFIG_PRODUCT_KEY, mock_get_device_id(), user_account, AGORA_IOT_CLIENT_ID_MAX_LEN)
        || 0 == strlen(user_account)
        || 0 != device_get_item_string(g_dev_state, "license", &license)) {
      // active device if cannot found license info or cannot found bind user info
      if (0 != activate_device(g_dev_state)) {
        printf("cannot activate device !\n");
        goto dev_bringup_err;
      }
      // save state items to file or flash
      mock_save_device_state(g_dev_state);
    }
  }
  if (ssid) {
    free(ssid);
  }
  if (psw) {
    free(psw);
  }
  if (license) {
    free(license);
  }
  return g_dev_state;

dev_bringup_err:
  if (g_dev_state) {
    device_destroy_state(g_dev_state);
    g_dev_state = NULL;
  }
  if (ssid) {
    free(ssid);
  }
  if (psw) {
    free(psw);
  }
  if (license) {
    free(license);
  }
  return NULL;
}

static void iot_cb_ota_update(const agora_iot_device_fota_info_t *info)
{
  printf("-- %s --\n", __FUNCTION__);
  printf("\ttype: %d, size: %u, versin: %s, url: %s\n",
        info->type, info->file_size, info->file_ver, info->file_url);

  int ret = -1;
  agora_iot_device_fw_info_t fw_info = {
    .fw_mcu_ver = "",
    .fw_wifi_ver = ""
  };
  if (AGO_FW_WIFI == info->type) {
    ret = device_set_item_string(g_dev_state, "fw_wifi_ver", info->file_ver);
    if (0 == ret) {
      printf("WIFI FW update successfully\n");
      snprintf(fw_info.fw_wifi_ver, sizeof(fw_info.fw_wifi_ver), "%s", info->file_ver);
      agora_iot_fw_info_update(g_app.iot_handle, &fw_info);
    } else {
      printf("WIFI FW update failure, ret=%d\n", ret);
    }
  } else if (AGO_FW_MCU == info->type) {
    ret = device_set_item_string(g_dev_state, "fw_mcu_ver", info->file_ver);
    if (0 == ret) {
      printf("MCU FW update successfully\n");
      snprintf(fw_info.fw_mcu_ver, sizeof(fw_info.fw_mcu_ver), "%s", info->file_ver);
      agora_iot_fw_info_update(g_app.iot_handle, &fw_info);
    } else {
      printf("MCU FW update failure, ret=%d\n", ret);
    }
  }
}

static agora_iot_handle_t connect_agora_iot_service(device_handle_t dev_state)
{
  agora_iot_handle_t handle = NULL;
  char *domain = NULL;
  char *dev_crt = NULL;
  char *dev_key = NULL;
  char *license = NULL;
  char *client_id = NULL;
  char *user_id = NULL;
  if (0 != device_get_item_string(dev_state, "dev_crt", &dev_crt)
      || 0 != device_get_item_string(dev_state, "dev_key", &dev_key)
      || 0 != device_get_item_string(dev_state, "domain", &domain)
      || 0 != device_get_item_string(dev_state, "client_id", &client_id)
      || 0 != device_get_item_string(dev_state, "bind_user", &user_id)) {
    printf("cannot found dev_crt or dev_key or domain or client_id in device state items\n");
    goto agora_iot_err;
  }

  if (0 != device_get_item_string(dev_state, "license", &license)) {
    printf("cannot found license in device state items\n");
    goto agora_iot_err;
  }

  agora_iot_config_t cfg = {
    .app_id             = CONFIG_AGORA_APP_ID,
    .product_key        = CONFIG_PRODUCT_KEY,

    .client_id          = client_id,
    .domain             = domain,
    .root_ca            = CONFIG_AWS_ROOT_CA,
    .client_crt         = dev_crt,
    .client_key         = dev_key,

    .user_id            = user_id,

    .enable_rtc         = true,
    .certificate        = license,
    .enable_recv_audio  = true,
    .enable_recv_video  = false,
    .area_code          = AGO_AREA_CODE_GLOB,
    .rtc_cb = {
      .cb_start_push_frame        = iot_cb_start_push_frame,
      .cb_stop_push_frame         = iot_cb_stop_push_frame,
      .cb_receive_audio_frame     = iot_cb_receive_audio_frame,
      .cb_receive_video_frame     = iot_cb_receive_video_frame,
      .cb_target_bitrate_changed  = iot_cb_target_bitrate_changed,
      .cb_key_frame_requested     = iot_cb_key_frame_requested
    },

    .disable_rtc_log      = false,
    .log_level            = AGORA_LOG_WARNING,
    .max_possible_bitrate = DEFAULT_MAX_BITRATE,
    .enable_audio_config  = true,
    .audio_config = {
        .audio_codec_type = INTERNAL_AUDIO_ENC_TYPE,
        .pcm_sample_rate  = CONFIG_PCM_SAMPLE_RATE,
        .pcm_channel_num  = CONFIG_PCM_CHANNEL_NUM
    },

    .slave_server_url = CONFIG_SLAVE_SERVER_URL,
    .call_mode        = CALL_MODE_MUTLI,
    .call_cb = {
      .cb_call_request        = iot_cb_call_request,
      .cb_call_answered       = iot_cb_call_answered,
      .cb_call_hung_up        = iot_cb_call_hung_up,
      .cb_call_local_timeout  = iot_cb_call_timeout,
      .cb_call_peer_timeout   = iot_cb_call_timeout,
    },
    .ota_cb = {
      .fw_updated             = iot_cb_ota_update
    },
    .rtm_cb = {
      .on_receive_rtm         = iot_cb_receive_rtm,
      .on_send_rtm_result     = iot_cb_send_rtm_result
    },
    .connect_cb = {
      .on_connect_status      = iot_cb_connect_status
    }
  };

  handle = agora_iot_init(&cfg);
  if (NULL == handle) {
    printf("agora_iot_init failed\n");
    goto agora_iot_err;
  }

  agora_iot_device_fw_info_t fw_info;
  char *wifi_version = NULL;
  char *mcu_version = NULL;
  if (0 == device_get_item_string(dev_state, "fw_wifi_ver", &wifi_version)) {
    snprintf(fw_info.fw_wifi_ver, sizeof(fw_info.fw_wifi_ver), "%s", wifi_version);
  } else {
    snprintf(fw_info.fw_wifi_ver, sizeof(fw_info.fw_wifi_ver), "%s", CONFIG_FM_WIFI_VER);
  }
  if (0 == device_get_item_string(dev_state, "fw_mcu_ver", &mcu_version)) {
    snprintf(fw_info.fw_mcu_ver, sizeof(fw_info.fw_mcu_ver), "%s", mcu_version);
  } else {
    snprintf(fw_info.fw_mcu_ver, sizeof(fw_info.fw_mcu_ver), "%s", CONFIG_FM_MCU_VER);
  }
  agora_iot_fw_info_update(handle, &fw_info);

agora_iot_err:
  if (wifi_version) {
    free(wifi_version);
  }
  if (mcu_version) {
    free(mcu_version);
  }
  if (client_id) {
    free(client_id);
  }
  if (domain) {
    free(domain);
  }
  if (dev_crt) {
    free(dev_crt);
  }
  if (dev_key) {
    free(dev_key);
  }
  if (license) {
    free(license);
  }
  if (user_id) {
    free(user_id);
  }
  return handle;
}

static char mock_wait_key_press(void)
{
  struct termios tms_old, tms_new;

  tcgetattr(0, &tms_old);

  tms_new = tms_old;
  tms_new.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &tms_new);
  char ch = getchar();
  tcsetattr(0, TCSANOW, &tms_old);

  return ch;
}

static unsigned long long start_alarm_record(agora_iot_handle_t handle)
{
  unsigned long long video_record_id = 0;
  int result = -1;

  // Step 1: Start recording
  struct timeval tv;
  if (gettimeofday (&tv, NULL) < 0) {
    printf("#### query system time failure\n");
    return 0;
  }
  // begin time is necessary, and end time can be estimated to avoid record stop failure
  unsigned long long begin_time = (unsigned long long)((uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
  result = agora_iot_cloud_record_start(handle, begin_time, begin_time + 60 * 1000,
                                        SEND_AUDIO_DATA_TYPE, SEND_VIDEO_DATA_TYPE, &video_record_id);
  if (0 != result) {
    printf("#### start recording failure: %d\n", result);
    return 0;
  }

  // Step 2: Push alarm image
  agora_iot_file_info_t file_info = {
    .name_suffix  = "jpeg",
    .buf          = WARNING_JPEG,
    .size         = sizeof(WARNING_JPEG)
  };
  char *image_id = NULL;
  result = agora_iot_push_alarm_image(handle, &file_info, &image_id);
  if (0 != result) {
    printf("#### push alarm image failure: %d\n", result);
    // image is not must be here, so need not to retur error
  }

  // Step 3: Publish alarm message, begin_time must be equal to or later than record start begin time
  result = agora_iot_push_alarm_message(handle, begin_time, "dev_nickname", AG_ALARM_TYPE_VAD,
                                        "This is a alarm test", image_id);
  if (image_id) {
    free(image_id);
    image_id = NULL;
  }
  if (0 != result) {
    printf("#### push alarm message failure: %d\n", result);
    agora_iot_cloud_record_stop(handle, video_record_id, begin_time);
    return 0;
  }
  return video_record_id;
}

static int stop_alarm_record(agora_iot_handle_t handle, unsigned long long video_record_id)
{
  if (video_record_id <= 0) {
    printf("#### improper video record ID\n");
    return -1;
  }
  struct timeval tv;
  if (gettimeofday (&tv, NULL) < 0) {
    printf("#### query system time failure\n");
    return -1;
  }
  unsigned long long end_time = (unsigned long long)((uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
  return agora_iot_cloud_record_stop(handle, video_record_id, end_time);
}

int main(int argc, char *argv[])
{
  agora_iot_handle_t handle = NULL;
  uint32_t rtm_msg_id = 0;
  unsigned long long video_record_id = 0;

  if (argc == 2) {
    strncpy(g_device_id, argv[1], sizeof(g_device_id));
  } else {
    printf("\n----------------------------------------------\n");
    printf("    Please input your device ID as this:\n");
    printf("    ./hello_doorbell_2 my_device_id\n");
    printf("----------------------------------------------\n\n");
    return -1;
  }

  install_signal_handler();

  // TODO: need to set real mode for it if system was
  up_mode = SYS_UP_MODE_POWERON;

  // Mock device bringup process, you must modify it on your board
  if (NULL == mock_device_bringup(up_mode)) {
    printf("mock_device_bringup failed.\n");
    goto EXIT;
  }

  // connect to agora iot service
  handle = connect_agora_iot_service(g_dev_state);
  if (NULL == handle) {
    printf("connect_agora_iot_service failed.\n");
    goto EXIT;
  }

  // update device state
  if (0 != update_device_work_state(handle, up_mode)) {
    printf("agora_iot_init failed\n");
    goto EXIT;
  }

  g_app.iot_handle = handle;

  agora_iot_logfile_config(handle, 1024 * 4, 5);

  // infinite loop
  char *user_account = NULL;
  while (!g_app.b_exit) {
    switch (mock_wait_key_press()) {
    case 'c': // call
      if (0 != device_get_item_string(g_dev_state, "bind_user", &user_account)) {
        printf("cannot found bind_user in device state items.\n");
        break;
      }
      if (0 != agora_iot_call(handle, user_account, "This is a call test")) {
        printf("------- call %s failed.\n", user_account);
      }
      if (user_account) {
        free(user_account);
      }
      // TODO: should start ring at here until recive answer/hangup/timeout callback
      break;
    case 'a': // answer
      agora_iot_answer(handle);
      break;
    case 'h': // hangup
      agora_iot_hang_up(handle);
      break;
    case 'w': // warning alarm
      video_record_id = start_alarm_record(handle);
      break;
    case 's': // clear alarm and stop cloud recording
      stop_alarm_record(handle, video_record_id);
      video_record_id = 0;  // set to invalid ID to prevent repeated calls
      break;
    case 'p': // publish all of the registered data points
      agora_iot_dp_publish_all(handle);
      break;
    case 'q': // quit
      g_app.b_exit = true;
      // mock to low power mode
      update_device_low_power(handle);
      break;
    case 'm':  // RTM send test
       if (g_rtm_peer_uid[0] != '\0') {
         agora_iot_send_rtm(handle, g_rtm_peer_uid, rtm_msg_id++, "This is a test message for RTM....", 20);
       }
       break;
    default:
      break;
    }
  }

EXIT:
  if (handle) {
    iot_cb_stop_push_frame(AGO_AV_PUSH_TYPE_MASK_RTC | AGO_AV_PUSH_TYPE_MASK_OSS);
    stop_alarm_record(handle, video_record_id);
    agora_iot_hang_up(handle);
    agora_iot_deinit(handle);
  }
  if (g_dev_state) {
    // save state items to file or flash
    mock_save_device_state(g_dev_state);
    device_destroy_state(g_dev_state);
  }

  return 0;
}
