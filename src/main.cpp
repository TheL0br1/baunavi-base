#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "constants.h"
#include "structures.h"
#include "espWrapper.hpp"


espWrapper *pEspWrapper = nullptr;
void setup() {
    pEspWrapper = espWrapper::getInstance();

}



void loop() {
    static unsigned long lastEventTime = millis();
    static const unsigned long EVENT_INTERVAL_MS = 5000;
    WiFi.forceSleepBegin();
    WiFi.softAP(pEspWrapper->wifiName.c_str());

    delay(10000);
    WiFi.forceSleepWake();
    delay(1);
    WiFi.softAP(pEspWrapper->wifiName.c_str());
    delay(10);
    //Set values to send


    // Send message via ESP-NOW to all peers
    myData dataToSend = pEspWrapper->prepareDataToSend();
    int res = esp_now_send(pEspWrapper->server.macAddr, reinterpret_cast<u8 *>(&dataToSend), sizeof(dataToSend));
    espWrapper::printMAC(reinterpret_cast<const int *>(pEspWrapper->server.macAddr));
    Serial.print(res);

}