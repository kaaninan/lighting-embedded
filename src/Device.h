#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <EEPROM.h>

class Device
{
private:

    // ### Partition Schema ###
    
    // - Device Info
    // --- Serial               - char[20]
    // --- Firmware Revision    - char[10]
    // --- Hardware Revision    - char[10]
    // --- Manufacturer         - char[40]
    
    // - Bluetooth Info
    // --- Name                 - char[20]

    size_t EEPROM_SIZE = 1024;

    
public:

    Device();

    // Memory Functions
    typedef enum {
        serial = 0,
        firmware = 21,
        hardware = 32,
        manufacturer = 43,
        bleName = 84,
    }address;

    bool bluetoothConnected = false;

    // 0 to 100
    int batteryLevel = 75;
    // 0 to 420, 420 means 4.2V
    int batteryVoltage = 0;

    // 0 -> Turned Off
    // 1 -> Running
    // 2 -> Ghost Charging
    int powerState = 0;

    int softSetup = 0;

    // 0 -> Nothing
    // 1 -> Charging
    // 2 -> StandBy
    int chargingState = 0;

    // LED
    unsigned long hue = 0; // [0, 65535]
    unsigned long saturation = 0; // [0,255]
    unsigned long value = 0; // [0,255]

    int brightness = 0;

    int mode = 0; // manual(0) - automatic(1)
    int status = 0; // off(0) - on(1)
    

    bool begin();

    void clear();
    void write(char*, int);
    String read(int);

};
 
#endif