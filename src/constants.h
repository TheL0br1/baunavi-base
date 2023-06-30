#pragma once

#include"structures.h"







// Define variables to store incoming readings
float incomingCharge;
int incomingReadingsId;

structMessage myData;
structMessage incomingReadings;
structMessagePairing pairingData;


#define EEPROM_ALLOC sizeof(connectionData)+1
#define VREF 3.3      // точное напряжение на пине 5V (в данном случае зависит от стабилизатора на плате Arduino)
#define DIV_R1 900  // точное значение 10 кОм резистора
#define DIV_R2 100
