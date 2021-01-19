#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Arduino.h>
#include "Device.h"

#include <ArduinoJson.h>
// #include "FS.h"
// #include "SPIFFS.h"


// DEVICE INFORMATION
#define DEVICE_SERVICE_UUID                             "180A" // Device Information
#define DEVICE_CHARACTERISTIC_UUID_MODEL_NUMBER         "2A24" // Model Number
#define DEVICE_CHARACTERISTIC_UUID_SERIAL_NUMBER        "2A25" // Serial Number
#define DEVICE_CHARACTERISTIC_UUID_FIRMWARE_REVISION    "2A26" // Firmware Revision
#define DEVICE_CHARACTERISTIC_UUID_HARDWARE_REVISION    "2A27" // Hardware Revision
#define DEVICE_CHARACTERISTIC_UUID_MANUFACTURER         "2A29" // Manufacturer

// BATTERY
#define BATTERY_SERVICE_UUID                            "180F"
#define BATTERY_CHARACTERISTIC_UUID_LEVEL               "2A19"
#define BATTERY_CHARACTERISTIC_UUID_STATE               "2A1A"

// LED
#define LED_SERVICE_UUID                    "44D02C61-3543-4A30-8E94-ACA5853D55C1"
#define LED_VALUE_CHARACTERISTIC_UUID       "44D02C62-3543-4A30-8E94-ACA5853D55C1"
#define LED_MODE_CHARACTERISTIC_UUID        "44D02C63-3543-4A30-8E94-ACA5853D55C1"
#define LED_STATUS_CHARACTERISTIC_UUID      "44D02C64-3543-4A30-8E94-ACA5853D55C1"

// LED CONTROL
#define LED_CONTROL_SERVICE_UUID                    "45D02C61-3543-4A30-8E94-ACA5853D55C1"
#define LED_CONTROL_VALUE_CHARACTERISTIC_UUID       "45D02C62-3543-4A30-8E94-ACA5853D55C1"
#define LED_CONTROL_MODE_CHARACTERISTIC_UUID        "45D02C63-3543-4A30-8E94-ACA5853D55C1"
#define LED_CONTROL_STATUS_CHARACTERISTIC_UUID      "45D02C64-3543-4A30-8E94-ACA5853D55C1"



class Bluetooth
{
private:

    Device* device;

    BLEServer* pServer = NULL;

    // DEVICE
    BLECharacteristic* pCharacteristicDeviceModelNumber = NULL;
    BLECharacteristic* pCharacteristicDeviceSerialNumber = NULL;
    BLECharacteristic* pCharacteristicDeviceFirmwareRevision = NULL;
    BLECharacteristic* pCharacteristicDeviceHardwareRevision = NULL;
    BLECharacteristic* pCharacteristicDeviceManufacturer = NULL;

    // BATTERY
    BLECharacteristic* pCharacteristicBatteryLevel = NULL;
    BLECharacteristic* pCharacteristicBatteryState = NULL;

    // LED
    BLECharacteristic* pCharacteristicLedValue = NULL;
    BLECharacteristic* pCharacteristicLedMode = NULL;
    BLECharacteristic* pCharacteristicLedStatus = NULL;

    // LED CONTROL
    BLECharacteristic* pCharacteristicLedControlValue = NULL;
    BLECharacteristic* pCharacteristicLedControlMode = NULL;
    BLECharacteristic* pCharacteristicLedControlStatus = NULL;




public:

    // bool deviceConnected = false;

    Bluetooth(Device*);

    void setup();
    void close();

    void publishDeviceInfo();

    void publishBatteryStatus();
    void publishBatteryLevel();
    
    void publishLedValue();
    void publishLedMode();
    void publishLedStatus();


    // void publishSensorData(Sensor*);
    // void publishSyncData(String, String = "", String = "");
    
    // void publishWiFiData(String);
};
 
#endif

