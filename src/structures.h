//for esp8266-32 compatibility
#include <cstdint>

#ifndef ESP32
typedef enum {
    ESP_OK = 0,                 /*!< No error */
    ESP_ERR_NO_MEM,             /*!< Out of memory */
    ESP_ERR_INVALID_ARG,        /*!< Invalid argument */
    ESP_ERR_INVALID_STATE,      /*!< Invalid state */
    ESP_ERR_INVALID_SIZE,       /*!< Invalid size */
    ESP_ERR_NOT_FOUND,          /*!< Not found */
    ESP_ERR_NOT_SUPPORTED,      /*!< Not supported */
    ESP_ERR_TIMEOUT,            /*!< Timeout */
    ESP_ERR_INVALID_RESPONSE,   /*!< Invalid response */
    ESP_ERR_INVALID_CRC,        /*!< Invalid CRC */
    ESP_ERR_INVALID_VERSION,    /*!< Invalid version */
    ESP_ERR_INVALID_MAC,        /*!< Invalid MAC address */
    ESP_ERR_WIFI_BASE = 0x3000, /*!< Starting number of WiFi error codes */
    ESP_ERR_MESH_BASE = 0x4000, /*!< Starting number of Mesh error codes */
} esp_err_t;

typedef struct {
    uint8_t peer_addr[6]; /**< MAC address of the peer */
    uint8_t channel;      /**< Communication channel */
    uint8_t ifidx;        /**< Interface index */
    uint8_t encrypt;      /**< Peer encryption flag */
} esp_now_peer_info_t;
#endif
#pragma once

//structures for messaging
enum MessageType { PAIRING,
    DATA,
    SET_INIT,
};
enum PairingStatus { PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};
typedef struct structMessage {
    MessageType msgType;
    float charge;
    char WiFiName[];


};

typedef struct structMessagePairing {
    MessageType msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
    uint32_t serialID;
    bool initWifi;
};
typedef struct connectionData{
    uint8_t macAddr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    uint8_t channel;
};

typedef struct myData{
    uint8_t macAddr[6];
    uint8_t channel;
    uint32_t serialId;
};