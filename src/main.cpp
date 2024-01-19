// PERLU DIINGAT DATANYA MASIH DUMMY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

#define BLE_SERVER_NAME "esp32-server"
// SERVEICE UUID
#define SERVEICE_UUID "f024821c-45fd-4c07-a27d-185f92699c4c"
// CHARACTERISTIC UUID
#define OKSIMETER_CHARACTERISTIC_UUID "0e1c52dc-6e6f-462d-89ed-9c30d3564ce3"
#define TERMOMETER_CHARACTERISTIC_UUID "7f7cdd10-883b-4611-aa6a-939078035f7d"
// Define variabels
BLECharacteristic *pOksimeterCharacteristic = NULL;
BLECharacteristic *pTermometerCharacteristic = NULL;
BLEServer *pServer = NULL;
// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
// connection
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallback : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
  {
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };
  void onDisconnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
  {
    deviceConnected = false;
  }
};

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // Serial.println(bleOksimeterCharacteristic.getUUID().toString().c_str());
  // Serial.println(bleOksimeterDescriptor.getUUID().toString().c_str());

  BLEDevice::init(BLE_SERVER_NAME);
  // create ble server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallback());
  // create ble serveice
  BLEService *service = pServer->createService(SERVEICE_UUID);
  // Create a BLE Characteristic
  pOksimeterCharacteristic = service->createCharacteristic(
      OKSIMETER_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pOksimeterCharacteristic->addDescriptor(new BLE2902());
  pTermometerCharacteristic = service->createCharacteristic(
      TERMOMETER_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pTermometerCharacteristic->addDescriptor(new BLE2902());
  // start service
  service->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVEICE_UUID);
  pAdvertising->setMinPreferred(0x0);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  BLEAddress myAddress = BLEDevice::getAddress();
  Serial.print("Mac address: ");
  Serial.println(myAddress.toString().c_str());
  Serial.println("");
}

void loop()
{
  if (deviceConnected)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    if ((millis() - lastTime) > timerDelay)
    {
      float temp = random(10, 40);
      float bloodOxygen = random(70, 120);
      float heartRate = random(70, 120);
      uint8_t tempString = temp;
      uint8_t dataOksimeter[2];
      dataOksimeter[0] = bloodOxygen;
      dataOksimeter[1] = heartRate;

      pOksimeterCharacteristic->setValue(dataOksimeter, sizeof(dataOksimeter));
      pOksimeterCharacteristic->notify();
      Serial.println("Oksigen Darah: " + String(bloodOxygen));
      Serial.println("Detak Jantung: " + String(heartRate));

      pTermometerCharacteristic->setValue(&tempString, 4);
      pTermometerCharacteristic->notify();
      Serial.println("Temperature: " + String(temp));
      Serial.println("----------------------------------------------------------------");
      lastTime = millis();
    }
  }
  else if (!deviceConnected && oldDeviceConnected)
  {
    delay(1000);
    // restar adversting
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    oldDeviceConnected = deviceConnected;
    digitalWrite(LED_BUILTIN, LOW);
  }
}
