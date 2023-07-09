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
    myData.msgType = DATA;
    myData.charge = getCharge();

    // Send message via ESP-NOW to all peers
    int res = esp_now_send(server.macAddr, (uint8_t *)&myData, sizeof(myData));
    printMAC(server.macAddr);
    Serial.print(res);

}