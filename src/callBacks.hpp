//
// Created by User on 7/9/2023.
//

#ifndef UNTITLED2_CALLBACKS_HPP
#define UNTITLED2_CALLBACKS_HPP

// Callback when data is sent
void OnDataSent(uint8_t *macAddr, uint8_t sendStatus) {
    espWrapper *pEspWrapper = espWrapper::getInstance();

    Serial.print("Last Packet Send Status: ");
    if (sendStatus == ESP_OK) {
        Serial.println("Delivery Success to ");
        // espWrapper::printMAC(reinterpret_cast<const int *>(macAddr));

        pEspWrapper->messageSended++;

    } else {
        Serial.println("Delivery Failed to ");
        espWrapper::espWrapper_->printMAC(macAddr);
        Serial.print("Status:");
        Serial.println(sendStatus);
    }
    Serial.println();
    Serial.println("ended send callback");
}

// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, int len) {
    espWrapper *pEspWrapper = espWrapper::getInstance();
    Serial.print("Size of message : ");
    Serial.print(len);
    Serial.print(" from ");
    espWrapper::printMAC(mac);
    Serial.println();
    messagePairing recievedPairing = messagePairing(0, BASE);
    auto type = static_cast<MessageType>(incomingData[0]);
    switch (type) {
        case PAIRING: {

            memcpy(&recievedPairing, incomingData, sizeof(espWrapper::espWrapper_->server));
            if (recievedPairing.role == MAIN ||
                recievedPairing.role == SWITCH) {  // the message comes from peerInfo
                Serial.print("Pairing done for ");
                espWrapper::espWrapper_->server = connectionData(recievedPairing.channel, recievedPairing.macAddr,
                                                                 recievedPairing.role, recievedPairing.serialId);
                espWrapper::printMAC(recievedPairing.macAddr);

                Serial.print(" on channel ");
                Serial.print(recievedPairing.channel);  // channel used by the peerInfo
                Serial.print(" in ");
                Serial.print(millis() - espWrapper::espWrapper_->start);
                Serial.println("ms");
                //esp_now_del_peer(pairingData.macAddr);
                //esp_now_del_peer(mac);
                EEPROM.put(1, espWrapper::espWrapper_->server);
                EEPROM.commit();
                espWrapper::espWrapper_->addPear();
                // add the peerInfo to the peer list
                pEspWrapper->setPairingStatus(PAIR_PAIRED);
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP(
                        std::to_string(ESP.getChipId()).c_str());
                Serial.println("set wifi, end pairing");
            }

        }
    }
}

#endif //UNTITLED2_CALLBACKS_HPP

// Callback when data is sent
