//
// Created by User on 6/30/2023.
//

#include <espnow.h>
#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <core_esp8266_waveform.h>
#include <EEPROM.h>
#include "espWrapper.hpp"
#include "structures.h"
#include "functions.h"
#include "callBacks.hpp"

espWrapper espWrapper::*espWrapper_ = nullptr;

espWrapper* espWrapper::getInstance() {
    if(espWrapper_== nullptr){
        espWrapper_ = new espWrapper();
    }
    return espWrapper_;

}

espWrapper::espWrapper(){
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        Serial.println(esp_now_init());
        return;
    }
    Serial.println("ESP-NOW initializated succes");
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    this->initEEPromData();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(wifiName.c_str());
    Serial.println(WiFi.macAddress());
    if (server.macAddr[0] != 0xFF && server.macAddr[0] != 0) {
        pairingData.channel = server.channel;
        Serial.println("PAIRED TO:");
        printMAC(server.macAddr);
        if(addPear(server.macAddr, server.channel))
        {
            esp_now_send(const_cast<int *>(server.macAddr), reinterpret_cast<int *>((uint8_t *) &pairingData), sizeof(pairingData));

            delay(1000);
        }  // add the peerInfo to the peer list

    }
}


bool espWrapper::addPear(const int *peer_addr, int chan) {
    memset(&peerInfo, 0, sizeof(peerInfo));
    esp_now_peer_info_t *peer = &peerInfo;
    memcpy(peerInfo.peer_addr, peer_addr, sizeof(peerInfo.peer_addr));

    peerInfo.channel = chan;  // pick a channel
    peerInfo.encrypt = 0;     // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(peerInfo.peer_addr);
    if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
        return true;
    } else {
        esp_err_t addStatus = esp_err_t(esp_now_add_peer(peer->peer_addr, ESP_NOW_ROLE_COMBO, peer->channel, NULL, 0));  // add the peerInfo to the peer list
        Serial.print("Pairing to ");
        printMAC(reinterpret_cast<const int *>(peer->peer_addr));
        Serial.println();
        Serial.print("Channel: ");
        Serial.println(peer->channel);
        if (addStatus == ESP_OK) {
            // Pair success
            Serial.println("Pair success");
            return true;
        } else {
            Serial.println("Pair failed");
            return false;
        }
    }
}
void espWrapper::printMAC(const int* mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.println();
}
void espWrapper::printMAC() {
    Serial.println(WiFi.macAddress());
}
PairingStatus espWrapper::autoPairing() {

        switch (pairingStatus) {
            case PAIR_REQUEST:
                Serial.print("Pairing request on channel ");
                Serial.println(channel);

                // clean esp now
                esp_now_deinit();
                WiFi.mode(WIFI_STA);
                // set WiFi channel
                wifi_promiscuous_enable(1);
                wifi_set_channel(channel);
                wifi_promiscuous_enable(0);
                /// WiFi.printDiag(Serial);
                WiFi.disconnect();

                // Init ESP-NOW
                if (esp_now_init() != 0) {
                    Serial.println("Error initializing ESP-NOW");
                }
                esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
                // set callback routines
                esp_now_register_send_cb(OnDataSent);
                esp_now_register_recv_cb(OnDataRecv);

                // set pairing data to send to the peerInfo
                pairingData.channel = channel;
                pairingData.serialID = ESP.getChipId();
                pairingData.initWifi = initWifi;
                previousMillis = millis();

                // add peer and send request
                Serial.print("Send request to pair: ");
                Serial.println(esp_now_send(broadcastAddressX, (uint8_t *)&pairingData, sizeof(pairingData)));
                pairingStatus = PAIR_REQUESTED;
                break;

            case PAIR_REQUESTED:
                // time out to allow receiving response from peerInfo
                currentMillis = millis();
                if (currentMillis - previousMillis > 100) {
                    previousMillis = currentMillis;
                    // time out expired,  try next channel
                    channel++;
                    if (channel > 13) {
                        channel = 0;
                    }
                    pairingStatus = PAIR_REQUEST;
                }
                break;

            case PAIR_PAIRED:
                Serial.println("Paired!");
                break;
        }
        return pairingStatus;
    }

void espWrapper::initEEPromData() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_ALLOC);
    Serial.print("EEPROM INIT: ");
    Serial.print(EEPROM_ALLOC);
    Serial.println(" bytes"); // Initialize EEPROM with 512 bytes of space
    if (EEPROM.read(0) != 0xFF) {
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0xFF);
        }
    }
    EEPROM.get(eepromIterator, server);
    eepromIterator+=sizeof(server);
    EEPROM.get(eepromIterator, initWifi);
    eepromIterator++;
    if(initWifi){
        EEPROM.get(eepromIterator, wifiName);
        Serial.println("Wifi name:");
    }
}


