//
// Created by User on 6/30/2023.
//

#include <espnow.h>
#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <core_esp8266_waveform.h>
#include "espWrapper.hpp"
#include "structures.h"
#include "functions.h"

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
}

bool espWrapper::addPear(const uint8_t *peer_addr, int chan) {
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

