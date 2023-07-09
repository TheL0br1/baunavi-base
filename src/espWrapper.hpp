//
// Created by User on 6/30/2023.
//

#ifndef UNTITLED2_ESPWRAPPER_HPP
#define UNTITLED2_ESPWRAPPER_HPP


#include <cstdint>
#include <string>
#include "structures.h"

class espWrapper {
public:
    uint8_t macAddr[6]{};
    uint8_t channel{};
    long long start;
    connectionData server = connectionData();
    uint32_t serialId = EspClass::getChipId();
    structMessagePairing pairingData = structMessagePairing(false, 0);
    esp_now_peer_info_t peerInfo{};
    std::string wifiName = nullptr;
    bool initWifi = false;
    unsigned long previousMillis{};
    uint8_t broadcastAddressX[6]={ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    double charge{};
    long messageSended{};
    PairingStatus pairingStatus = PAIR_REQUEST;
    uint16_t eepromIterator = 1;
    static espWrapper *espWrapper_;
    static void printMAC();
    double getCharge();
    bool addPear();
    static void printMAC(const int* mac_addr);
    PairingStatus autoPairing();
    static espWrapper* getInstance();
    void initEEPromData();
    unsigned long currentMillis{};
    bool setWifi(char* WifiName);
    myData prepareDataToSend();

private:
    espWrapper();



};


#endif //UNTITLED2_ESPWRAPPER_HPP
