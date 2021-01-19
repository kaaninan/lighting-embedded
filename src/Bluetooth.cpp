#include "Bluetooth.h"

Bluetooth::Bluetooth(Device* dev){
    device = dev;
}

// Server Callback
class MyServerCallbacks: public BLEServerCallbacks {
    public:
        bool* connected;
        MyServerCallbacks(bool* x){ connected = x; }

    void onConnect(BLEServer* pServer) { *connected = true; Serial.println("Bluetooth Connected"); };
    void onDisconnect(BLEServer* pServer) { *connected = false; Serial.println("Bluetooth Disconnected"); }
};


// Callback
class callbackLedValue: public BLECharacteristicCallbacks {

    public:
        Device* device;
        callbackLedValue(Device* dev){device = dev;};

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, value);
        JsonObject obj = doc.as<JsonObject>();

        String cmd = obj["command"].as<String>();

        // Serial.print("INCOMING COMMAND - callbackLedValue: ");
        // Serial.println(cmd);

        long led_hue = obj["hue"].as<String>().toInt();
        long led_saturation = obj["sat"].as<String>().toInt();
        long led_value = obj["val"].as<String>().toInt();
        long led_brightness = obj["bright"].as<String>().toInt();

        // hue => [0, 65535]
        // saturation => [0,255]
        // value => [0,255]
        // brightness => [0,255]

        device->hue = led_hue;
        device->saturation = led_saturation;
        device->value = led_value;
        device->brightness = led_brightness;
        
        StaticJsonDocument<2000> returnJson;
        JsonObject root = returnJson.to<JsonObject>();
        root["success"] = true;
        String output;
        serializeJson(returnJson, output);
        char sendData[200];
        sprintf(sendData, "%s", output.c_str());
        pCharacteristic->setValue(sendData);
        pCharacteristic->notify();
    }
};

class callbackLedStatus: public BLECharacteristicCallbacks {

    public:
        Device* device;
        callbackLedStatus(Device* dev){device = dev;};

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, value);
        JsonObject obj = doc.as<JsonObject>();

        String cmd = obj["command"].as<String>();

        // Serial.print("INCOMING COMMAND - callbackLedStatus: ");
        // Serial.println(cmd);

        long led_status = obj["status"].as<String>().toInt();

        device->status = led_status;
        
        StaticJsonDocument<2000> returnJson;
        JsonObject root = returnJson.to<JsonObject>();
        root["success"] = true;
        String output;
        serializeJson(returnJson, output);
        char sendData[200];
        sprintf(sendData, "%s", output.c_str());
        pCharacteristic->setValue(sendData);
        pCharacteristic->notify();
    }
};

class callbackLedMode: public BLECharacteristicCallbacks {

    public:
        Device* device;
        callbackLedMode(Device* dev){device = dev;};

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, value);
        JsonObject obj = doc.as<JsonObject>();

        String cmd = obj["command"].as<String>();

        // Serial.print("INCOMING COMMAND - callbackLedMode: ");
        // Serial.println(cmd);

        long led_mode = obj["mode"].as<String>().toInt();

        device->mode = led_mode;

        if(led_mode == 1){
            // Default Set White
            device->hue = 0;
            device->saturation = 0;
            device->value = 255;
        }
        
        StaticJsonDocument<2000> returnJson;
        JsonObject root = returnJson.to<JsonObject>();
        root["success"] = true;
        String output;
        serializeJson(returnJson, output);
        char sendData[200];
        sprintf(sendData, "%s", output.c_str());
        pCharacteristic->setValue(sendData);
        pCharacteristic->notify();

    }
};


void Bluetooth::setup(){

    // Get Bluetooth Name from EEPROM
    String red = device->read(Device::address::bleName);
    int len = red.length() + 1;
    char data[len];
    red.toCharArray(data, len);
    BLEDevice::init(data);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks(&device->bluetoothConnected));
    
    // Create the BLE Service
    BLEService *pServiceDevice = pServer->createService(DEVICE_SERVICE_UUID);
    BLEService *pServiceBattery = pServer->createService(BATTERY_SERVICE_UUID);
    BLEService *pServiceLed = pServer->createService(LED_SERVICE_UUID);
    BLEService *pServiceLedControl = pServer->createService(LED_CONTROL_SERVICE_UUID);


    // Add Characteristics

    // maksimum 4 characteristic olabilir
    
    // DEVICE INFORMATION
    pCharacteristicDeviceSerialNumber = pServiceDevice->createCharacteristic(
                        DEVICE_CHARACTERISTIC_UUID_SERIAL_NUMBER,
                        BLECharacteristic::PROPERTY_READ);

    pCharacteristicDeviceFirmwareRevision = pServiceDevice->createCharacteristic(
                        DEVICE_CHARACTERISTIC_UUID_FIRMWARE_REVISION,
                        BLECharacteristic::PROPERTY_READ);

    pCharacteristicDeviceHardwareRevision = pServiceDevice->createCharacteristic(
                        DEVICE_CHARACTERISTIC_UUID_HARDWARE_REVISION,
                        BLECharacteristic::PROPERTY_READ);

    pCharacteristicDeviceManufacturer = pServiceDevice->createCharacteristic(
                        DEVICE_CHARACTERISTIC_UUID_MANUFACTURER,
                        BLECharacteristic::PROPERTY_READ);

    

    // BATTERY
    pCharacteristicBatteryLevel = pServiceBattery->createCharacteristic(
                        BATTERY_CHARACTERISTIC_UUID_LEVEL,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_INDICATE );

    pCharacteristicBatteryState = pServiceBattery->createCharacteristic(
                        BATTERY_CHARACTERISTIC_UUID_STATE,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_INDICATE );


    // LED
    pCharacteristicLedValue = pServiceLed->createCharacteristic(
                        LED_VALUE_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicLedMode = pServiceLed->createCharacteristic(
                        LED_MODE_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY);


    pCharacteristicLedStatus = pServiceLed->createCharacteristic(
                        LED_STATUS_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY);


    // LED CONTROL
    pCharacteristicLedControlValue = pServiceLedControl->createCharacteristic(
                        LED_CONTROL_VALUE_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_WRITE_NR);

    pCharacteristicLedControlMode = pServiceLedControl->createCharacteristic(
                        LED_CONTROL_MODE_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_WRITE_NR);


    pCharacteristicLedControlStatus = pServiceLedControl->createCharacteristic(
                        LED_CONTROL_STATUS_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE | 
                        BLECharacteristic::PROPERTY_WRITE_NR );



    // BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t)0x2901));
    // BatteryLevelDescriptor.setValue("Percentage 0 - 100");
    // pCharacteristicBatteryLevel->addDescriptor(&BatteryLevelDescriptor);
    // pCharacteristicBatteryLevel->addDescriptor(new BLE2902());


    // Add Descriptor (React Native'de monitor (notfiy) yapmak icin eklemek gerekiyor)    
    pCharacteristicBatteryState->addDescriptor(new BLE2902());
    pCharacteristicBatteryLevel->addDescriptor(new BLE2902());

    pCharacteristicLedValue->addDescriptor(new BLE2902());
    pCharacteristicLedStatus->addDescriptor(new BLE2902());
    pCharacteristicLedMode->addDescriptor(new BLE2902());

    pCharacteristicLedControlValue->addDescriptor(new BLE2902());
    pCharacteristicLedControlStatus->addDescriptor(new BLE2902());
    pCharacteristicLedControlMode->addDescriptor(new BLE2902());

    // Start the service
    pServiceDevice->start();
    pServiceBattery->start();
    pServiceLed->start();
    pServiceLedControl->start();

    pCharacteristicLedControlValue->setCallbacks(new callbackLedValue(device));
    pCharacteristicLedControlStatus->setCallbacks(new callbackLedStatus(device));
    pCharacteristicLedControlMode->setCallbacks(new callbackLedMode(device));

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(DEVICE_SERVICE_UUID);
    pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
    pAdvertising->addServiceUUID(LED_SERVICE_UUID);
    pAdvertising->addServiceUUID(LED_CONTROL_SERVICE_UUID);
    
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
}

void Bluetooth::close(){
    device->bluetoothConnected = false;
    BLEDevice::deinit();
    // BLEDevice::deinit(true);
}

void Bluetooth::publishDeviceInfo(){
    String data;
    int len;

    data = device->read(Device::address::serial);
    len = data.length() + 1;
    char serial[len];
    data.toCharArray(serial, len);

    data = device->read(Device::address::firmware);
    len = data.length() + 1;
    char firmware[len];
    data.toCharArray(firmware, len);

    data = device->read(Device::address::hardware);
    len = data.length() + 1;
    char hardware[len];
    data.toCharArray(hardware, len);

    data = device->read(Device::address::manufacturer);
    len = data.length() + 1;
    char manufacturer[len];
    data.toCharArray(manufacturer, len);

    pCharacteristicDeviceSerialNumber->setValue(serial);
    pCharacteristicDeviceFirmwareRevision->setValue(firmware);
    pCharacteristicDeviceHardwareRevision->setValue(hardware);
    pCharacteristicDeviceManufacturer->setValue(manufacturer);
}

void Bluetooth::publishBatteryLevel(){
    uint8_t level = device->batteryLevel;
    pCharacteristicBatteryLevel->setValue(&level, 1);
    pCharacteristicBatteryLevel->indicate();
}

void Bluetooth::publishBatteryStatus(){
    uint8_t state = device->chargingState;
    pCharacteristicBatteryState->setValue(&state, 1);
    pCharacteristicBatteryState->indicate();
}


void Bluetooth::publishLedValue(){
    char sendData[20];
    sprintf(sendData, "%lu#%lu#%lu#%d", device->hue, device->saturation, device->value, device->brightness);
    pCharacteristicLedValue->setValue(sendData);
    pCharacteristicLedValue->notify();
}

void Bluetooth::publishLedMode(){
    char sendData[20];
    sprintf(sendData, "%d", device->mode);
    pCharacteristicLedMode->setValue(sendData);
    pCharacteristicLedMode->notify();
}

void Bluetooth::publishLedStatus(){
    char sendData[20];
    sprintf(sendData, "%d", device->status);
    pCharacteristicLedStatus->setValue(sendData);
    pCharacteristicLedStatus->notify();
}