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
    uint8_t macAddr[6];
    uint8_t channel;
    connectionData server;
    uint32_t serialId;
    esp_now_peer_info_t peerInfo;
    std::string wifiName = NULL;
    bool initWifi = false;
    unsigned long previousMillis;
    uint8_t broadcastAddressX[6]={ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    double charge;
    long messageSended;
    PairingStatus pairingStatus = PAIR_REQUEST;
    uint16_t eepromIterator = 1;
    static espWrapper *espWrapper_;
    void printMAC();
    double getCharge();
    bool addPear(const int *peer_addr, int chan);
    void printMAC(const int* mac_addr);
    PairingStatus autoPairing();
    static espWrapper* getInstance();
    void initEEPromData();
    unsigned long currentMillis;


private:
    espWrapper();



};


#endif //UNTITLED2_ESPWRAPPER_HPP
