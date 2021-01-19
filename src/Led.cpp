#include "Led.h"

Led::Led(Device* dev, int tPin, int tLedCount) {
    device = dev;
    dataInPin = tPin;
    ledCount = tLedCount;
}

void Led::begin(){
    strip = new Adafruit_NeoPixel(ledCount, dataInPin, NEO_GRBW + NEO_KHZ800);
    strip->begin();
    strip->fill(strip->Color(0, 0, 0, 0));
    strip->show();
    strip->setBrightness(200);
}

void Led::update(){
    if(device->status == 1){
        int colors[4];
        hsi2rgbw(mapfloat(device->hue, 0, 65535, 0, 360), mapfloat(device->saturation, 0, 255, 0, 1), mapfloat(device->value, 0, 255, 0, 1), colors);
        strip->fill(strip->Color(colors[0], colors[1], colors[2], colors[3]));
        strip->show();
    }else{
        strip->fill(strip->Color(0,0,0,0));
        strip->show();
    }
}

void Led::show(uint32_t color){
    strip->fill(color);
    strip->show();
}


// PRIVATE

float Led::mapfloat(long x, long in_min, long in_max, long out_min, long out_max){
    return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

// https://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
void Led::hsi2rgbw(float H, float S, float I, int* rgbw) {
    int r, g, b, w;
    float cos_h, cos_1047_h;
    H = fmod(H,360); // cycle H around to 0-360 degrees
    H = 3.14159*H/(float)180; // Convert to radians.
    S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
    I = I>0?(I<1?I:1):0;
    
    if(H < 2.09439) {
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        r = S*255*I/3*(1+cos_h/cos_1047_h);
        g = S*255*I/3*(1+(1-cos_h/cos_1047_h));
        b = 0;
        w = 255*(1-S)*I;
    } else if(H < 4.188787) {
        H = H - 2.09439;
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        g = S*255*I/3*(1+cos_h/cos_1047_h);
        b = S*255*I/3*(1+(1-cos_h/cos_1047_h));
        r = 0;
        w = 255*(1-S)*I;
    } else {
        H = H - 4.188787;
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        b = S*255*I/3*(1+cos_h/cos_1047_h);
        r = S*255*I/3*(1+(1-cos_h/cos_1047_h));
        g = 0;
        w = 255*(1-S)*I;
    }
  
    rgbw[0]=r;
    rgbw[1]=g;
    rgbw[2]=b;
    rgbw[3]=w;
}