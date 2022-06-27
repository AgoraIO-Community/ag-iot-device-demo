#pragma once

#define CONFIG_LICENSE

#define CONFIG_AGORA_APP_ID "4b31f***********************3037" // Please replace with your own APP ID
#define CONFIG_CUSTOMER_KEY "8620f***********************7363"
#define CONFIG_CUSTOMER_SECRET "492c1***********************e802"

#define CONFIG_USER_ID "6875*********3440" // Please replace with your own user ID
#define CONFIG_DEVICE_ID "mydoorbell" // Please replace with your own device ID

// Agora Master Server URL
#define CONFIG_MASTER_SERVER_URL "https://app.agoralink-iot-cn.sd-rtn.com"

// Agora Slave Server URL
#define CONFIG_SLAVE_SERVER_URL "https://api.agora.io/agoralink/cn/api"

// Found product key form device manager platform
#define CONFIG_PRODUCT_KEY "EJIJ*******5lI4"

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

#define SEND_VIDEO_DATA_TYPE (0) // 0 for H264 and 1 for JPEG, 2 for H265
#define SEND_AUDIO_DATA_TYPE (0) // 0 for PCM, 1 for G711U
#define USE_AUDIO_CODEC_TYPE (1) // 0 for OPUS, 1 for G722, 2 for G711U, 3 for OPUSFB, ignored if audio data type is not PCM

#if (SEND_VIDEO_DATA_TYPE == 0)
#define CONFIG_SEND_H264_FRAMES
#define CONFIG_SEND_FRAME_RATE (25)
#elif (SEND_VIDEO_DATA_TYPE == 1)
#define CONFIG_SEND_JPEG_FRAMES
#define CONFIG_SEND_FRAME_RATE (5)
#else
// to be added
#endif

#if (SEND_AUDIO_DATA_TYPE == 0)
#define CONFIG_SEND_PCM_DATA
#define CONFIG_PCM_FRAME_LEN (640) // reconfigure the value to match with the real audio capture hardware, like 2048
#define CONFIG_PCM_CHANNEL_NUM (1)
#elif (SEND_AUDIO_DATA_TYPE == 1)
#define CONFIG_SEND_G711U_DATA
#define CONFIG_G711U_FRAME_LEN (160)
#endif

#if (USE_AUDIO_CODEC_TYPE == 0)
#define CONFIG_USE_OPUS_CODEC
#elif (USE_AUDIO_CODEC_TYPE == 1)
#define CONFIG_USE_G722_CODEC
#elif (USE_AUDIO_CODEC_TYPE == 2)
#define CONFIG_USE_G711U_CODEC
#elif (USE_AUDIO_CODEC_TYPE == 3)
#define CONFIG_USE_OPUSFB_CODEC
#endif