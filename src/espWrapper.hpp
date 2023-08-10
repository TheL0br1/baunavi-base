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
    uint8_t macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t channel=1;
    long long start;
    connectionData server;
    uint32_t serialId = 0;
    messagePairing pairingData = messagePairing(0, SWITCH);
    esp_now_peer_info_t peerInfo{};
    bool initWifi = false;
    unsigned long previousMillis{};
    uint8_t broadcastAddressX[6]={ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    int messageSended=0;
    PairingStatus pairingStatus = PAIR_REQUEST;
    uint16_t eepromIterator = 1;
    static espWrapper *espWrapper_;
    static void printMAC();
    double getCharge();
    bool addPear();
    static void printMAC(const uint8_t *mac_addr);
    PairingStatus autoPairing();
    static espWrapper* getInstance();
    void initEEPromData();
    unsigned long currentMillis{};
    myData prepareDataToSend();

    void setPairingStatus(PairingStatus pairingStatus);

private:
    espWrapper();



};


#endif //UNTITLED2_ESPWRAPPER_HPP
