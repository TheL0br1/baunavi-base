#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "structures.h"
#include "espWrapper.hpp"

espWrapper *pEspWrapper;
void setup() {
    Serial.begin(115200);
    Serial.println("esp started");
    pEspWrapper = espWrapper::getInstance();
    while (pEspWrapper->pairingStatus!= PAIR_PAIRED) {
        pEspWrapper->start = millis();
        pEspWrapper->autoPairing();
        delay(500);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("notInit");
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);


}



void loop() {
    WiFi.forceSleepBegin();
    WiFi.softAP(pEspWrapper->wifiName);

    delay(10000);
    WiFi.forceSleepWake();
    delay(1);
    WiFi.softAP(pEspWrapper->wifiName);
    delay(10);
    //Set values to send


    // Send message via ESP-NOW to all peers
    myData dataToSend = pEspWrapper->prepareDataToSend();
    Serial.println("Serial id: " + String(ESP.getChipId()));
    dataToSend.print();
    int res = esp_now_send(pEspWrapper->server.macAddr, (uint8_t*)(&dataToSend), sizeof(dataToSend));
    espWrapper::printMAC((pEspWrapper->server.macAddr));
    Serial.print(res);

}