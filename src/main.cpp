#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "structures.h"
#include "espWrapper.hpp"

espWrapper *pEspWrapper;
void OnDataSent(int *macAddr, int sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == ESP_OK) {
        Serial.println("Delivery Success to ");
        // espWrapper::printMAC(reinterpret_cast<const int *>(macAddr));
        pEspWrapper->messageSended++;

    } else {
        Serial.println("Delivery Failed to ");
        espWrapper::printMAC(macAddr);
        Serial.print("Status:");
        Serial.println(sendStatus);
    }
    Serial.println();
    Serial.println("ended send callback");
}

// Callback when data is received
void OnDataRecv(int *mac, int *incomingData, int len) {
    Serial.print("Size of message : ");
    Serial.print(len);
    Serial.print(" from ");
    espWrapper::printMAC(mac);
    Serial.println();
    auto type = static_cast<MessageType>(incomingData[0]);
    switch (type) {
        case SET_INIT: {
            structMessage message = reinterpret_cast<structMessage &&>(incomingData);
            espWrapper::espWrapper_->wifiName = message.WiFiName;
            Serial.print("Wifi name seted:");
            Serial.println(message.WiFiName);
        }
        case PAIRING: {

            memcpy(&espWrapper::espWrapper_->server, incomingData, sizeof(espWrapper::espWrapper_->server));
            if (espWrapper::espWrapper_->server.role == MAIN ||
                espWrapper::espWrapper_->server.role == SWITCH) {  // the message comes from peerInfo
                Serial.print("Pairing done for ");
                espWrapper::printMAC(mac);
                memcpy(espWrapper::espWrapper_->server.macAddr, mac,
                       sizeof(&espWrapper::espWrapper_->server.macAddr));
                Serial.print(" on channel ");
                Serial.print(espWrapper::espWrapper_->server.channel);  // channel used by the peerInfo
                Serial.print(" in ");
                Serial.print(millis() - espWrapper::espWrapper_->start);
                Serial.println("ms");
                //esp_now_del_peer(pairingData.macAddr);
                //esp_now_del_peer(mac);
                EEPROM.put(1, espWrapper::espWrapper_->server);
                EEPROM.commit();
                espWrapper::espWrapper_->addPear();
                // add the peerInfo to the peer list
                espWrapper::espWrapper_->pairingStatus = PAIR_PAIRED;
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP(
                        "rssid_test4");                                                              // set the pairing status
            }

        }
    }
}
void setup() {
    Serial.begin(115200);
    Serial.println("esp started");
    esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
    esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
    pEspWrapper = espWrapper::getInstance();


}



void loop() {
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