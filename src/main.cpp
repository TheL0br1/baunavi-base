#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "constants.h"
#include "functions.h"
#include "structures.h"

// Callback when data is sent
void OnDataSent(int *mac_addr, int sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if(sendStatus == ESP_OK){
        Serial.println("Delivery Success to ");
        printMAC(mac_addr);

    }else{
        Serial.println("Delivery Failed to ");
        printMAC(mac_addr);
        Serial.print("Status:");
        Serial.println(sendStatus);
    }
    Serial.println();
}

// Callback when data is received
void OnDataRecv(int *mac, int *incomingData, int len) {
    Serial.print("Size of message : ");
    Serial.print(len);
    Serial.print(" from ");
    printMAC(mac);
    Serial.println();
    uint8_t type = incomingData[0];
    switch (type) {
        case DATA:
            memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
            Serial.print(len);
            Serial.print(" Data bytes received from: ");
            printMAC(mac);
            Serial.println();
            incomingCharge=incomingReadings.charge;
            printIncomingReadings();

            if (incomingReadings.readingId % 2 == 1) {
                digitalWrite(LED_BUILTIN, LOW);
            } else {
                digitalWrite(LED_BUILTIN, HIGH);
            }
            break;

        case PAIRING:
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            if (pairingData.id == 0) {  // the message comes from peerInfo

                Serial.print("Pairing done for ");
                printMAC(mac);
                memcpy(&pairingData.macAddr, mac, sizeof(&pairingData.macAddr));
                Serial.print(" on channel ");
                Serial.print(pairingData.channel);  // channel used by the peerInfo
                Serial.print(" in ");
                Serial.print(millis() - start);
                Serial.println("ms");
                //esp_now_del_peer(pairingData.macAddr);
                //esp_now_del_peer(mac);
                memcpy(&server.macAddr, mac, sizeof(server.macAddr));
                server.channel = pairingData.channel;
                EEPROM.put(1, server);
                EEPROM.commit();
                addPeer(mac, server.channel);
                // add the peerInfo to the peer list
                pairingStatus = PAIR_PAIRED;
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP("rssid_test4");                                                              // set the pairing status
            }
            break;
        case SET_INIT:
            Serial.println("GET {SET_INIT} DATA");


    }
}

void setup() {
    // Init Serial Monitor
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
    EEPROM.get(1, server);
    //pinMode(LED_BUILTIN, OUTPUT);
    // Init DHT sensor

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("rssid_test4");
    Serial.println(WiFi.macAddress());

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    // Set ESP-NOW Role
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
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