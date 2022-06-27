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

#include <pthread.h>

#include "agora_iot_api.h"
#include "agora_iot_call.h"

#include "app_config.h"

#if defined(CONFIG_SEND_H264_FRAMES)
#define VIDEO_DATA_TYPE AGO_VIDEO_DATA_TYPE_H264
#elif defined(CONFIG_SEND_JPEG_FRAMES)
#define VIDEO_DATA_TYPE AGO_VIDEO_DATA_TYPE_JPEG
#endif

#if defined(CONFIG_SEND_PCM_DATA)

#if defined(CONFIG_USE_OPUS_CODEC)
#define AUDIO_CODEC_TYPE AGO_AUDIO_CODEC_TYPE_OPUS
#define CONFIG_PCM_SAMPLE_RATE (16000)
#elif defined(CONFIG_USE_G722_CODEC)
#define AUDIO_CODEC_TYPE AGO_AUDIO_CODEC_TYPE_G722
#define CONFIG_PCM_SAMPLE_RATE (16000)
#elif defined(CONFIG_USE_G711U_CODEC)
#define AUDIO_CODEC_TYPE AGO_AUDIO_CODEC_TYPE_G711U
#define CONFIG_PCM_SAMPLE_RATE (8000)
#elif defined(CONFIG_USE_OPUSFB_CODEC)
#define AUDIO_CODEC_TYPE AGO_AUDIO_CODEC_TYPE_OPUS
#define CONFIG_PCM_SAMPLE_RATE (48000)
#endif

#define AUDIO_DATA_TYPE AGO_AUDIO_DATA_TYPE_PCM
#define AUDIO_FRAME_LEN CONFIG_PCM_FRAME_LEN
#define AUDIO_TEST_DATA pcm_test_data
#define AUDIO_TEST_DATA_LEN sizeof(AUDIO_TEST_DATA)
#define AUDIO_FRAME_DURATION_MS                                                                                        \
  (CONFIG_PCM_FRAME_LEN * 1000 / CONFIG_PCM_SAMPLE_RATE / CONFIG_PCM_CHANNEL_NUM / sizeof(int16_t))

#elif defined(CONFIG_SEND_G711U_DATA)

#define AUDIO_CODEC_TYPE AGO_AUDIO_CODEC_DISABLED
#define AUDIO_DATA_TYPE AGO_AUDIO_DATA_TYPE_G711U
#define AUDIO_FRAME_LEN CONFIG_G711U_FRAME_LEN
#define AUDIO_TEST_DATA g711u_test_data
#define AUDIO_TEST_DATA_LEN sizeof(AUDIO_TEST_DATA)
#define AUDIO_FRAME_DURATION_MS (20)
#endif

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
void iot_cb_start_push_frame(void);
void iot_cb_stop_push_frame(void);
void iot_cb_call_answered(const char *peer_name);
void iot_cb_call_hung_up(const char *peer_name);
void iot_cb_call_busy(const char *peer_name);
void iot_cb_call_timeout(const char *peer_name);
void iot_cb_call_custom_msg(const char *peer_name, const char *msg, int len);
void iot_cb_receive_audio_frame(ago_audio_frame_t *frame);
void iot_cb_receive_video_frame(ago_video_frame_t *frame);
void iot_cb_key_frame_requested(void);
void iot_cb_target_bitrate_changed(uint32_t target_bps);