#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "constants.h"
#include "functions.h"
#include "structures.h"
#include "espWrapper.hpp"



void setup() {

    // Init Serial Monitor

    //pinMode(LED_BUILTIN, OUTPUT);
    // Init DHT sensor

    // Set device as a Wi-Fi Station


    // Init ESP-NOW

    // Set ESP-NOW Role
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
    esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
    if (server.macAddr[0] != 0xFF && server.macAddr[0] != 0) {
        pairingData.channel = server.channel;
        Serial.println("PAIRED TO:");
        printMAC(server.macAddr);
        if(addPeer(server.macAddr, server.channel))
        {
            esp_now_send(server.macAddr, (uint8_t *)&pairingData, sizeof(pairingData));

            delay(1000);
        }  // add the peerInfo to the peer list

    }
    pairingData.id = 2;
    while (pairingStatus != PAIR_PAIRED) {
        autoPairing();
        delay(100);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("notInit");
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);

}



void loop() {
    static unsigned long lastEventTime = millis();
    static const unsigned long EVENT_INTERVAL_MS = 5000;
    WiFi.forceSleepBegin();
    WiFi.softAP("rssid_test4");

    delay(10000);
    WiFi.forceSleepWake();
    delay(1);
    WiFi.softAP("rssid_test4");
    delay(10);
    //Set values to send
    myData.msgType = DATA;
    myData.charge = getCharge();

    // Send message via ESP-NOW to all peers
    int res = esp_now_send(server.macAddr, (uint8_t *)&myData, sizeof(myData));
    printMAC(server.macAddr);
    Serial.print(res);

}