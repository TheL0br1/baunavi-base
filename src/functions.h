#pragma once

#include <cstdint>
#include <HardwareSerial.h>
#include <cstdio>
#include <user_interface.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "constants.h"

void OnDataSent(u8 *mac_addr, u8 sendStatus);
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.println();
}

void printIncomingReadings() {
    // Display Readings in Serial Monitor
    Serial.println("INCOMING READINGS");
    Serial.print("Charge: ");
    Serial.print(incomingCharge);
    Serial.println(" %");
    Serial.print("Led: ");
    Serial.print(incomingReadingsId);
}

PairingStatus autoPairing() {
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
            pairingData.id = BOARD_ID;
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
bool addPeer(const uint8_t *peer_addr, int chan) {  // add pairing
    memset(&slave, 0, sizeof(slave));
    esp_now_peer_info_t *peer = &slave;
    memcpy(slave.peer_addr, peer_addr, sizeof(slave.peer_addr));

    slave.channel = chan;  // pick a channel
    slave.encrypt = 0;     // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
        return true;
    } else {
        esp_err_t addStatus = esp_err_t(esp_now_add_peer(peer->peer_addr, ESP_NOW_ROLE_COMBO, peer->channel, NULL, 0));  // add the peerInfo to the peer list
        Serial.print("Pairing to ");
        printMAC(peer->peer_addr);
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
void initESP_NOW() {
    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        Serial.println(esp_now_init());
        return;
    }
    Serial.println("ESP-NOW initializated succes");
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}