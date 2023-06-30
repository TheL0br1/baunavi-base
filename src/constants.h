#pragma once

#include <core_esp8266_features.h>
#include"structures.h"
uint8_t channel = 1;
int readingId = 0;
int id = 2;


unsigned long currentMillis = millis();

uint8_t broadcastAddressX[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

enum PairingStatus { PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};
PairingStatus pairingStatus = PAIR_REQUEST;
MessageType messageType;


// Define variables to store incoming readings
float incomingCharge;
int incomingReadingsId;
bool initWifi = false;
// Updates DHT readings every 10 seconds
//const long interval = 10000;
unsigned long previousMillis = 0;  // will store last time DHT was updated

//Structure example to send data
//Must match the receiver structure

// Create a struct_message called myData
struct_message myData;
struct_message incomingReadings;
struct_pairing pairingData;

connectionData server;

esp_now_peer_info_t slave;
#define EEPROM_ALLOC sizeof(connectionData)+1
#define VREF 3.3      // точное напряжение на пине 5V (в данном случае зависит от стабилизатора на плате Arduino)
#define DIV_R1 900  // точное значение 10 кОм резистора
#define DIV_R2 100
unsigned long start;