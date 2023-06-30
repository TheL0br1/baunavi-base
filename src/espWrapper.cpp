//
// Created by User on 6/30/2023.
//

#include <espnow.h>
#include <HardwareSerial.h>
#include "espWrapper.hpp"
#include "structures.h"
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
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void espWrapper::OnDataRecv(int *mac, int *incomingData, int len) {

}
void espWrapper::OnDataSent(int *mac_addr, int sendStatus) {

}
