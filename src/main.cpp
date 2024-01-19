#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Wire.h>

// SERVEICE UUID
#define SERVEICE_UUID "f024821c-45fd-4c07-a27d-185f92699c4c"

// CHARACTERISTIC UUID
BLECharacteristic bleOksimeterCharacteristic("0e1c52dc-6e6f-462d-89ed-9c30d3564ce3", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor bleOksimeterDescriptor(BLEUUID((uint16_t)0x2903));

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

bool deviceConnected = false;

void setup()
{
  Serial.println(bleOksimeterCharacteristic);
}

void loop()
{
}
