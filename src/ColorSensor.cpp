#include "ColorSensor.h"

ColorSensor::ColorSensor(Device* dev) {
    device = dev;
}

void ColorSensor::begin(){
    tcs = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
    isBegin = tcs->begin();
    // isBegin = false;
}

void ColorSensor::read(){
    tcs->getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs->calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs->calculateLux(r, g, b);

    if(false){
        Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
        Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
        Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
        Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
        Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
        Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
        Serial.println(" ");
    }
}