#pragma once

//#define CONFIG_FILE_PLAYER_TEST_ENABLED
//#define CONFIG_MONKEY_TEST_ENABLED

#define CONFIG_AGORA_APP_ID "4b31f*****************953037" 	// Please replace with your own APP ID
#define CONFIG_CUSTOMER_KEY "8620f*****************307363"	// Please replace with your custome key from agora Console website
#define CONFIG_CUSTOMER_SECRET "492c*****************7e802"	// Please replace with your custome secret from agora Console website
#define CONFIG_LICENSE_PID "00F8*****************822646"	// Please replace with your license PID from salesman

#define CONFIG_USER_ID "687******440" // Please replace with your own user ID，unused if input QRcode string
#define CONFIG_DEVICE_ID "mydoorbell" // Please replace with your own device ID, unused if run demo commend with Device ID parameter

// Agora Master Server URL
#define CONFIG_MASTER_SERVER_URL "https://app.agoralink-iot-cn.sd-rtn.com"

// Agora Slave Server URL
#define CONFIG_SLAVE_SERVER_URL "https://api.sd-rtn.com/agoralink/cn/api"

// Found product key form device manager platform
#define CONFIG_PRODUCT_KEY "EJ*************I4"

// Device cert file size max
#define CERT_BUF_SIZE_MAX (1024 * 2)

// AWS service root ca, it will never be changed
#define CONFIG_AWS_ROOT_CA                                                                                                    \
  "-----BEGIN CERTIFICATE-----\r\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\r\n\
-----END CERTIFICATE-----\r\n"

/**
 * @brief The data type of the video, refer to the definition of ago_av_data_type_e
 *
 */
#define SEND_VIDEO_DATA_TYPE (1) // 1 for H264 and 2 for JPEG, 3 for H265

#if (SEND_VIDEO_DATA_TYPE == 1)
#include "h264_test_data_352x288.h"
#define CONFIG_SEND_H264_FRAMES
#define CONFIG_SEND_FRAME_RATE (25)
#elif (SEND_VIDEO_DATA_TYPE == 2)
#include "jpeg_test_data_640x480.h"
#define CONFIG_SEND_JPEG_FRAMES
#define CONFIG_SEND_FRAME_RATE (5)
#elif (SEND_VIDEO_DATA_TYPE == 3)
#include "h265_test_data_360x640.h"
#define CONFIG_SEND_H265_FRAMES
#define CONFIG_SEND_FRAME_RATE (25)
#else
// to be added
#endif

/**
 * Note: if the data type of the audio to send is not PCM, such as G711U,
 * and the shoule be set INTERNAL_AUDIO_ENC_TYPE to be 0 (Disable).
 */
#define SEND_AUDIO_DATA_TYPE (10) // 10 for PCM, 13 for G711U

/**
 * @brief The internal audio encoder codec type
 *
 * The SDK supports encode the PCM data before sending.
 * The codec type of encoder refer to the definition of ago_audio_codec_type_e
 * 0 -> Disable encoder
 * 1 -> OPUS
 * 2 -> G722
 * 3 -> G711A
 * 4 -> G711U
 *
 * Note: The Agora OSS doesn't support OPUS yet.
 */
#define INTERNAL_AUDIO_ENC_TYPE (2)

#if (INTERNAL_AUDIO_ENC_TYPE == 0)
#define CONFIG_PCM_SAMPLE_RATE (16000)
#define CONFIG_PCM_CHANNEL_NUM (1)
#elif (INTERNAL_AUDIO_ENC_TYPE == 1)
#define CONFIG_PCM_SAMPLE_RATE (16000)
#define CONFIG_PCM_CHANNEL_NUM (1)
#elif (INTERNAL_AUDIO_ENC_TYPE == 2)
#define CONFIG_PCM_SAMPLE_RATE (16000)
#define CONFIG_PCM_CHANNEL_NUM (1)
#elif (INTERNAL_AUDIO_ENC_TYPE == 3)
#define CONFIG_PCM_SAMPLE_RATE (8000)
#define CONFIG_PCM_CHANNEL_NUM (1)
#elif (INTERNAL_AUDIO_ENC_TYPE == 4)
#define CONFIG_PCM_SAMPLE_RATE (8000)
#define CONFIG_PCM_CHANNEL_NUM (1)
#endif

// TODO: If you changed the audio test data, you should also update the sample rate, the channel number matched your own data.
#if (SEND_AUDIO_DATA_TYPE == 10)
#if (CONFIG_PCM_SAMPLE_RATE == 16000)
#include "pcm_test_data_16K_16bit_1ch_5s.h"
#else
#include "pcm_test_data_8k_16bit_1ch.h"
#endif
#define AUDIO_DATA pcm_test_data
#define AUDIO_FRAME_DURATION_MS (20)
#define AUDIO_FRAME_LEN \
        (AUDIO_FRAME_DURATION_MS * sizeof(int16_t) * CONFIG_PCM_CHANNEL_NUM * (CONFIG_PCM_SAMPLE_RATE / 1000))
#elif (SEND_AUDIO_DATA_TYPE == 13)
#include "g711u_test_data.h"
#define AUDIO_DATA g711u_test_data
#define AUDIO_FRAME_DURATION_MS (20)
#define AUDIO_FRAME_LEN (160)
#endif

// Device Firmware version
#define CONFIG_FM_WIFI_VER "1.0.0"
#define CONFIG_FM_MCU_VER "1.0.0"

// Define this while needing dump the received audio/video data
#undef DUMP_RECEIVED
