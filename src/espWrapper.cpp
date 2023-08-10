// Created by User on 6/30/2023.
//

#include <espnow.h>
#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <core_esp8266_waveform.h>
#include <EEPROM.h>
#include <user_interface.h>
#include <ESP8266WiFi.h>
#include "espWrapper.hpp"
#include "structures.h"
#include "callBacks.hpp"
#include "constants.h"
espWrapper* espWrapper::espWrapper_ = nullptr;

espWrapper::espWrapper(){
    Serial.println("start constructor2");

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        Serial.println(esp_now_init());
        return;
    }
    Serial.println("ESP-NOW initializated succes");
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    this->initEEPromData();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(std::to_string(ESP.getChipId()).c_str());
    Serial.println(WiFi.macAddress());
    esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
    esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
    if (server.macAddr[0] != 0xFF && server.macAddr[0] != 0) {
        Serial.println("PAIRED TO:");
        printMAC((server.macAddr));
        if(addPear())
        {
            esp_now_send((server.macAddr),
                         reinterpret_cast<u8 *>(&pairingData), sizeof(pairingData));
            delay(1000);
        }  // add the peerInfo to the peer list
    }
    else{
        pairingData = messagePairing(serialId, SWITCH);
    }



}

 espWrapper* espWrapper::getInstance() {
    Serial.println("start constructor");
    if(espWrapper_== nullptr){
        Serial.println(sizeof(espWrapper));
        espWrapper_ = new espWrapper();


    }
    Serial.println("end constructor");
     return espWrapper_;


 }


bool espWrapper::addPear() {
    memset(&peerInfo, 0, sizeof(peerInfo));
    esp_now_peer_info_t *peer = &peerInfo;
    memcpy(peerInfo.peer_addr, server.macAddr, sizeof(peerInfo.peer_addr));

    peerInfo.channel = server.channel;  // pick a channel
    peerInfo.encrypt = 0;     // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(peerInfo.peer_addr);
    if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
        return true;
    } else {
        auto addStatus = esp_err_t(esp_now_add_peer(peer->peer_addr, ESP_NOW_ROLE_COMBO, peer->channel, nullptr, 0));  // add the peerInfo to the peer list
        Serial.print("Pairing to ");
        printMAC(peer->peer_addr);
        Serial.println();

        //Serial.println(peer->channel);
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
void espWrapper::printMAC(const uint8_t *mac_addr) {
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
    switch (this->pairingStatus) {
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
            esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
            esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
            Serial.println("trying to pair");
            // set pairing data to send to the peerInfo
            pairingData = messagePairing(serialId, SWITCH);
            previousMillis = millis();

            // add peer and send request
            Serial.print("Send request to pair: ");
            pairingStatus = PAIR_REQUESTED;
            Serial.println(esp_now_send(broadcastAddressX, (uint8_t *)&pairingData, sizeof(pairingData)));
            break;

        case PAIR_REQUESTED:
            // time out to allow receiving response from peerInfo
            currentMillis = millis();
            if (currentMillis - previousMillis > 1000) {
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
    }else if(EEPROM.read(eepromIterator) != 0xFF){
        Serial.println("eepromIterator: " + String(eepromIterator));
        EEPROM.get(eepromIterator, server);
        Serial.println("Server data:");
        server.print();
        eepromIterator += sizeof(decltype(server));

    }
}

myData espWrapper::prepareDataToSend() {
    return {ESP.getChipId(), getCharge(), BASE};
}

double espWrapper::getCharge(){
    return (analogRead(A0) / 1023.0) * VREF * ((DIV_R1 + DIV_R2) / DIV_R2);
}


void espWrapper::setPairingStatus(PairingStatus pairingStatus) {
    espWrapper::pairingStatus = pairingStatus;
    Serial.println("pairing status changed to: " + String(pairingStatus));
}

