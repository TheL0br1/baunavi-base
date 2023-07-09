//
// Created by User on 7/9/2023.
//

#ifndef UNTITLED2_CALLBACKS_HPP
#define UNTITLED2_CALLBACKS_HPP

#endif //UNTITLED2_CALLBACKS_HPP
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
                espWrapper.pairingStatus = PAIR_PAIRED;
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP("rssid_test4");                                                              // set the pairing status
            }
            break;
        case SET_INIT:
            Serial.println("GET {SET_INIT} DATA");


    }
}
espWrapper espInstance = *espWrapper::getInstance();
