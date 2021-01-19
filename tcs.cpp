#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "Device.h"
#include "Led.h"
Device* device = new Device();

Led* led = new Led(device, 18, 1);
// Led* ledBig = new Led(device, PinIO::led, 100);

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup(void) {
  Serial.begin(115200);

  led->begin();

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Now we're ready to get readings!
}

void loop(void) {
  uint16_t r, g, b, c, colorTemp, lux;

  for (size_t i = 0; i < 3; i++)
  {

	  	tcs.enable();
		switch (i)
		{
		case 0:
			Serial.print("RED  -> \t");
			led->strip->fill(led->strip->Color(255,0,0, 0));
			led->strip->show();
			break;
		case 1:
			Serial.print("GREEN -> \t");
			led->strip->fill(led->strip->Color(0,255,0, 0));
			led->strip->show();
			break;
		case 2:
			Serial.print("BLUE -> \t");
			led->strip->fill(led->strip->Color(0,0,255, 0));
			led->strip->show();
			break;
		
		default:
			break;
		}
		delay(1000);
		
		tcs.getRawData(&r, &g, &b, &c);
		colorTemp = tcs.calculateColorTemperature(r, g, b);
		// colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
		lux = tcs.calculateLux(r, g, b);

		// Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
		Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
		Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
		Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
		Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
		// Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
		Serial.println(" ");

		tcs.disable();
		led->show(led->strip->gamma32(led->strip->Color(0,0,0,0)));
		delay(1000/2);
  }

  Serial.println();
  Serial.println();
  

}