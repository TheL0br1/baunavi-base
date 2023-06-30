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
    std::string WifiName;
    double charge;
    long messageSended;

    uint16_t eepromIterator = 1;
    espWrapper();
    void printMAC();

};


#endif //UNTITLED2_ESPWRAPPER_HPP
