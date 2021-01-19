#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "PinIO.h"
#include "Device.h"

#include "math.h"
#define DEG_TO_RAD(X) (M_PI*(X)/180)


class Led
{
private:
    Device* device;
    
    int dataInPin;
    int ledCount;

    // uint32_t hue = 0;
    // uint32_t saturation = 0;
    // uint32_t value = 0;

    void hsi2rgbw(float, float, float, int*);
    float mapfloat(long x, long, long, long, long);
 
public:
    Adafruit_NeoPixel *strip;
    Led(Device*, int, int);

    void begin();
    void update();
    void show(uint32_t);

};
 
 #endif