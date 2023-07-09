//
// Created by User on 7/9/2023.
//

#ifndef UNTITLED2_CALLBACKS_HPP
#define UNTITLED2_CALLBACKS_HPP

#endif //UNTITLED2_CALLBACKS_HPP

// Callback when data is sent
void OnDataSent(int *macAddr, int sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == ESP_OK) {
        Serial.println("Delivery Success to ");
        espWrapper::espWrapper_->printMAC(macAddr);
        espWrapper::espWrapper_->messageSended++;

    } else {
        Serial.println("Delivery Failed to ");
        espWrapper::espWrapper_->printMAC(macAddr);
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
    espWrapper::espWrapper_->printMAC(mac);
    Serial.println();
    MessageType type = static_cast<MessageType>(incomingData[0]);
    switch (type) {
        case SET_INIT: {
            structMessage message = reinterpret_cast<structMessage &&>(incomingData);
            espWrapper::espWrapper_->wifiName = message.WiFiName;
            Serial.print("Wifi name seted:");
            Serial.println(message.WiFiName);
        }
            break;
        case PAIRING: {

            memcpy(&espWrapper::espWrapper_->server, incomingData, sizeof(&espWrapper::espWrapper_->server));
            if (espWrapper::espWrapper_->server.role == MAIN ||
                espWrapper::espWrapper_->server.role == SWITCH) {  // the message comes from peerInfo
                Serial.print("Pairing done for ");
                espWrapper::espWrapper_->printMAC(mac);
                memcpy(espWrapper::espWrapper_->server.macAddr, mac, sizeof(&espWrapper::espWrapper_->server.macAddr));
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
            break;
        }
    }
}
