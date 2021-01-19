#include "Device.h"
 
Device::Device(){}


bool Device::begin(){
    if (!EEPROM.begin(EEPROM_SIZE)){ return false; }
    return true;
}

void Device::clear(){
    for (int i = 0 ; i < EEPROM_SIZE ; i++) {
        EEPROM.write(i, 0);
    }
}

void Device::write(char* data, int address){
    EEPROM.writeString(address, data);
    EEPROM.commit();
}

String Device::read(int address){
    String dataString = EEPROM.readString(address);
    return dataString;
}

