
//structures for pairing
//for esp8266-32 compatibility
#include <cstdint>
#include<list>
typedef u8 uint8_t;
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
enum MessageType { PAIRING, DATA,
    SET_INIT,
};
enum PairingStatus { PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};
enum EspRole { MAIN,
    SWITCH,
    BASE,
};

struct messagePairing {
    MessageType msgType;
    uint8_t macAddr[6];
    uint8_t channel;
    EspRole role;
    uint32_t serialId;
    messagePairing(uint32_t serialId, EspRole role) :
            msgType(PAIRING),serialId(serialId), role(role){
        Serial.println("messagePairing constructor start" );
        WiFi.macAddress(macAddr);
        channel = WiFi.channel();

        Serial.println("messagePairing constructor end" );
    }
    messagePairing(messagePairing &message){
        Serial.println("messagePairing copy constructor start" );
        msgType = message.msgType;
        memcpy_P(macAddr, message.macAddr, sizeof(uint8_t)*6);
        channel = message.channel;
        role = message.role;
        serialId = message.serialId;
        Serial.println("messagePairing copy constructor end" );
    }
};





struct connectionData{
    EspRole role;
    uint8_t *macAddr;
    uint32_t serialId;
    uint8_t channel;
    connectionData() :channel(1), role(MAIN) {
        macAddr = new uint8_t[6]; // Выделение памяти для массива macAddr
        for (int i = 0; i < 6; ++i) {
            macAddr[i] = 0xFF;
        }
    }
    connectionData(uint8_t channel, uint8_t* macAddr, EspRole role, uint32_t serialId) : serialId(serialId),
                                                                                         channel(channel), role(role){
        this->macAddr = new uint8_t[6]; // Выделение памяти для массива macAddr
        memcpy_P(this->macAddr, macAddr, sizeof(uint8_t)*6);
    }
    connectionData(messagePairing data) {
        this->macAddr = new uint8_t[6]; // Выделение памяти для массива macAddr
        memcpy_P(this->macAddr, data.macAddr, sizeof(uint8_t)*6);
        this->channel = data.channel;
        this->role = data.role;
        this->serialId = data.serialId;
    }

    void print() {
        Serial.print("Role: ");
        Serial.println(this->role);
        Serial.print("Serial id ");
        Serial.println(this->serialId);
        Serial.print("Channel: ");
        Serial.println(this->channel);
        Serial.print("Mac address: ");
        for (int i = 0; i < 6; ++i) {
            Serial.print(this->macAddr[i], HEX);
            Serial.print(":");
        }
        Serial.println();
    }
};

struct myData{
    MessageType type;
    EspRole role;
    uint32_t serialId;
    float charge;
    myData(uint32_t serialId, double charge, EspRole role): type(DATA), serialId(serialId), role(role), charge(charge){
        Serial.println("myData constructor");
    }
    void print(){
        Serial.print("MessageType: ");
        Serial.println(this->type);
        Serial.print("Role: ");
        Serial.println(this->role);
        Serial.print("Serial id ");
        Serial.println(this->serialId);
        Serial.print("Charge: ");
        Serial.println(this->charge);
        Serial.print("Wifi name: ");
    }
} __attribute__((packed));
