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

#include "app_config.h"

#include <pthread.h>

#include "agora_iot_api.h"
#include "agora_iot_call.h"

#define DEFAULT_MAX_BITRATE (2500000)

typedef struct {
  agora_iot_handle_t iot_handle;
  pthread_t video_thread_id;
  pthread_t audio_thread_id;
  bool b_login_success;
  bool b_push_thread_run;
  bool b_exit;
} app_t;


void install_signal_handler(void);
void iot_cb_call_request(const char *peer_name, const char *attach_msg);
void iot_cb_start_push_frame(uint8_t push_type);
void iot_cb_stop_push_frame(uint8_t push_type);
void iot_cb_call_answered(const char *peer_name);
void iot_cb_call_hung_up(const char *peer_name);
void iot_cb_call_busy(const char *peer_name);
void iot_cb_call_timeout(const char *peer_name);
void iot_cb_call_custom_msg(const char *peer_name, const char *msg, int len);

void iot_cb_receive_audio_frame(ago_audio_frame_t *frame);
void iot_cb_receive_video_frame(ago_video_frame_t *frame);
void iot_cb_key_frame_requested(void);
void iot_cb_target_bitrate_changed(uint32_t target_bps);

void iot_cb_receive_rtm(const char *peer_uid, const void *msg, size_t msg_len);
void iot_cb_send_rtm_result(uint32_t msg_id, agora_rtm_err_e error_code);

void iot_cb_connect_status(agora_iot_status_e status);