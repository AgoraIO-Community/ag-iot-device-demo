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

#ifndef __AGORA_IOT_API_H__
#define __AGORA_IOT_API_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "agora_iot_base.h"
#include "agora_iot_call.h"
#include "agora_iot_dp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  /**
     * 0: YUV420
     */
  AGO_VIDEO_DATA_TYPE_YUV420 = 0,
  /**
     * 1: H264
     */
  AGO_VIDEO_DATA_TYPE_H264 = 1,
  /**
     * 2: JPEG
     */
  AGO_VIDEO_DATA_TYPE_JPEG = 2,
  /**
     * 10: PCM
     * support SampleRate: 16000HZ; Bits: 16; Channel: 1
     */
  AGO_AUDIO_DATA_TYPE_PCM = 10,
  /**
     * 11: OPUS
     * support SampleRate: 16000HZ; Bits: 16; Channel: 1
     */
  AGO_AUDIO_DATA_TYPE_OPUS = 11,
  /**
    * 12: PCMA
    * support SampleRate: 8000HZ; Bits: 16; Channel: 1
    */
  AGO_AUDIO_DATA_TYPE_G711A = 12,
  /**
    * 13: PCMU
    * support SampleRate: 8000HZ; Bits: 16; Channel: 1
    */
  AGO_AUDIO_DATA_TYPE_G711U = 13,
  /**
     * 14: G722
     * support SampleRate: 16000HZ; Bits: 16; Channel: 1
     */
  AGO_AUDIO_DATA_TYPE_G722 = 14,
  /**
     * 15: AACLC
     */
  AGO_AUDIO_DATA_TYPE_AACLC = 15,
  /**
     * 16: HEAAC
     */
  AGO_AUDIO_DATA_TYPE_HEAAC = 16,
} ago_av_data_type_e;

typedef struct {
  ago_av_data_type_e data_type;
  bool is_key_frame;
  uint8_t *video_buffer;
  uint32_t video_buffer_size;
} ago_video_frame_t;

typedef struct {
  ago_av_data_type_e data_type;
  uint8_t *audio_buffer;
  uint32_t audio_buffer_size;
} ago_audio_frame_t;

/**
 * Agora IOT SDK Event Callback
 */
typedef struct agora_iot_rtc_callback {
  /************ Event callback related to RTC(video/audio) ***************/
  /**
   * @brief Occurs when sdk is ready to send video/audio frame.
   *
   * This callback notifies the sender to send video/audio frame.
   */
  void (*cb_start_push_frame)(void);

  /**
    * Occurs when sdk stops sending video/audio frame.
    *
    * This callback notifies the sender to stop sending video/audio frame.
    */
  void (*cb_stop_push_frame)(void);

  /**
   * @brief Occurs when receiving audio frame from a peer user.
   *
   * @param[in] frame:        audio frame, refer to ago_audio_frame_t.
   */
  void (*cb_receive_audio_frame)(ago_audio_frame_t *frame);

  /**
   * @brief Occurs when receiving video frame from a peer user.
   *
   * @param[in] frame:        video frame, refer to ago_video_frame_t.
   */
  void (*cb_receive_video_frame)(ago_video_frame_t *frame);

  /**
   * @brief Occurs when peer requests a keyframe.
   *
   * This callback is optional, it is extensional.
   * This callback notifies the sender to generate a new keyframe.
   */
  void (*cb_key_frame_requested)(void);
  /**
   * Occurs when network bandwidth change is detected.
   * User is expected to adjust encoder bitrate to |target_bps|
   *
   * This callback is optional, it is extensional.
   *
   * @param[in] target_bps:   Target value (bps) by which the bitrate should update
   */
  void (*cb_target_bitrate_changed)(uint32_t target_bps);
  /***********************************************************************/
} agora_iot_rtc_callback_t;

/**
 * Audio codec type list.
 */
typedef enum {
  /**
	 * 0: Disable audio codec
	 */
  AGO_AUDIO_CODEC_DISABLED = 0,
  /**
	 * 1: OPUS
	 */
  AGO_AUDIO_CODEC_TYPE_OPUS = 1,
  /**
	 * 2: G722
	 */
  AGO_AUDIO_CODEC_TYPE_G722 = 2,
	/**
	 * 3: G711A
	 */
	AGO_AUDIO_CODEC_TYPE_G711A = 3,
	/**
	 * 4: G711U
	 */
	AGO_AUDIO_CODEC_TYPE_G711U = 4,
} ago_audio_codec_type_e;

typedef struct _agora_iot_audio_config {
  /**
	 * Configure sdk built-in audio codec
	 */
  ago_audio_codec_type_e audio_codec_type;
  /**
	 * Pcm sample rate. Ignored if audio coded is diabled
	 */
  int pcm_sample_rate;
  /**
	 * Pcm channel number. Ignored if audio coded is diabled
	 */
  int pcm_channel_num;
} agora_iot_audio_config_t;

typedef struct agora_iot_config {
  /* the product and device's informations */
  const char *app_id;
  const char *product_key;

  const char *client_id; // The unique ID for the device, after registering and will get it
  const char *domain; // domain host for dp
  const char *root_ca; // aws root ca buffer for dp
  const char *client_crt; // client certificate buffer for dp
  const char *client_key; //  client private key buffer for dp

  /* RTC Video and Audio */
  bool enable_rtc;
  const char *certificate; // if RTC SDK enable License
  bool enable_recv_audio;
  bool enable_recv_video;
  agora_iot_rtc_callback_t rtc_cb;
  /* Extensional RTC config */
  bool disable_rtc_log; // disable low level rtc log
  uint32_t max_possible_bitrate;
  bool enable_audio_config;
  agora_iot_audio_config_t audio_config;

  /* IoT Call Server */
  const char *slave_server_url;
  agora_iot_call_callback_t call_cb;
} agora_iot_config_t;

/**
 * @brief Initialize a Agora IoT object, then can use Agora IoT Server to call by the Agora UID.
 * You should call agora_iot_register() to register yourself account firstly,
 * and then init SDK with your account.
 *
 * @param[in] cfg:            Refer to agora_iot_config_t
 * @return
 * - NOT NULL: success
 * - NULL: failure
 */
agora_iot_handle_t agora_iot_init(const agora_iot_config_t *cfg);

/**
 * @brief Destory the Agora IoT object and free some resources.
 *
 * @param[in] handle:         The reference when initialized
 */
void agora_iot_deinit(agora_iot_handle_t handle);

/**
 * @brief Send video frame to peer.
 *
 * @param[in] handle:         The reference when initialized
 * @param[in] frame :         Video frame to send
 * @return
 * - = 0: success
 * - < 0: failure
 */
int agora_iot_push_video_frame(agora_iot_handle_t handle, ago_video_frame_t *frame);

/**
 * @brief Send audio frame to peer.
 *        Note: everytime only can write the audio data for 20ms
 *
 * @param[in] handle :        The reference when initialized
 * @param[in] frame :         Audio frame to send
 * @return
 * - = 0: success
 * - < 0: failure
 */
int agora_iot_push_audio_frame(agora_iot_handle_t handle, ago_audio_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif // __AGORA_IOT_API_H__