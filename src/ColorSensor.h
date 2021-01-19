#ifndef COLORSENSOR_H
#define COLORSENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "Device.h"

class ColorSensor
{
private:
    Adafruit_TCS34725* tcs;
    Device* device;
    
public:
    ColorSensor(Device*);

    uint16_t r, g, b, c, colorTemp, lux;
    bool isBegin = false;

    void begin();
    void read();
};
 
 #endif