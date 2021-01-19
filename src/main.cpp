#include <Arduino.h>
#include "Device.h"
#include "Bluetooth.h"
#include "ColorSensor.h"
#include "Led.h"

Device* device = new Device();
Bluetooth* bluetooth = new Bluetooth(device);
ColorSensor* colorSensor = new ColorSensor(device);
Led* led = new Led(device, 18, 1);
Led* ledCalibration = new Led(device, 19, 1);


void config(){
	device->clear();

	char serial[20] = "T1A2345678";
	char hardware[10] = "REV1";
	char firmware[10] = "0.0.1";
	char manufacturer[40] = "BAUMIND";

	device->write(serial, Device::address::serial);
	device->write(hardware, Device::address::hardware);
	device->write(firmware, Device::address::firmware);
	device->write(manufacturer, Device::address::manufacturer);

	char name[20];
	sprintf(name, "coLighting-%c%c%c%c", serial[0], serial[1], serial[2], serial[3]);
	device->write(name, Device::address::bleName);
}

QueueHandle_t xQueueBluetooth;

struct BluetoothQueueMsg
{
    uint8_t type;
} xMesageBluetooth;



// Bluetooth

void sendLedValues (void* arg){
	struct BluetoothQueueMsg xTxValue;
	while (1) {
		xTxValue.type = 0;
		xQueueSend( xQueueBluetooth, (void*) &xTxValue, ( TickType_t ) 0 );
		xTxValue.type = 1;
		xQueueSend( xQueueBluetooth, (void*) &xTxValue, ( TickType_t ) 0 );
		xTxValue.type = 2;
		xQueueSend( xQueueBluetooth, (void*) &xTxValue, ( TickType_t ) 0 );
		
		vTaskDelay(100 / portTICK_RATE_MS);

		if(uxQueueSpacesAvailable(xQueueBluetooth) < 5){
			Serial.print("Available: ");
			Serial.print(uxQueueSpacesAvailable(xQueueBluetooth));
		}
	}
}

void sendBattery (void* arg){
	struct BluetoothQueueMsg xTxValue;
	while (1) {
		xTxValue.type = 3;
		xQueueSend( xQueueBluetooth, (void*) &xTxValue, ( TickType_t ) 0 );
		xTxValue.type = 4;
		xQueueSend( xQueueBluetooth, (void*) &xTxValue, ( TickType_t ) 0 );
		
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

void bluetoothSendQueue (void* arg){
	while(1){

		struct BluetoothQueueMsg xRxValue;

		if( xQueueReceive( xQueueBluetooth, &( xRxValue ), ( TickType_t ) 0 ) == pdPASS && device->bluetoothConnected ){			
			if(xRxValue.type == 0){
				bluetooth->publishLedValue();
			}else if(xRxValue.type == 1){
				bluetooth->publishLedMode();
			}else if(xRxValue.type == 2){
				bluetooth->publishLedStatus();
			}else if(xRxValue.type == 3){
				bluetooth->publishBatteryLevel();
			}else if(xRxValue.type == 4){
				bluetooth->publishBatteryStatus();
			}
			vTaskDelay(10 / portTICK_RATE_MS);
		}else{
			vTaskDelay(1 / portTICK_RATE_MS);
		}
	}
}


// Led, Sensor

void ledTask (void* arg){
	while(1){
		led->update();
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}

void sensorTask (void* arg){
	while(1){
		// MANUAL MODE
		if(colorSensor->isBegin && device->mode == 0){
			// colorSensor->read(); // Maybe send values over ble
		}
		
		// AUTOMATIC MODE (Calibration)
		else if(colorSensor->isBegin && device->mode == 1){
			for (size_t i = 0; i < 3; i++){
				switch (i){
				case 0:
					// SET LIGHT
					ledCalibration->show(ledCalibration->strip->Color(255,0,0,0));
					vTaskDelay(500 / portTICK_RATE_MS);
					// READ SENSOR DATA
					colorSensor->read();

					// DO STAFF
					Serial.print("\nR:\t");
					Serial.println(colorSensor->r);
					break;

				case 1:
					// SET LIGHT
					ledCalibration->show(ledCalibration->strip->Color(0,255,0,0));
					vTaskDelay(500 / portTICK_RATE_MS);
					// READ SENSOR DATA
					colorSensor->read();

					// DO STAFF
					Serial.print("G:\t");
					Serial.println(colorSensor->g);
					break;

				case 2:
					// SET LIGHT
					ledCalibration->show(ledCalibration->strip->Color(0,0,255,0));
					vTaskDelay(500 / portTICK_RATE_MS);
					// READ SENSOR DATA
					colorSensor->read();

					// DO STAFF
					Serial.print("B:\t");
					Serial.println(colorSensor->b);
					break;
				
				default:
					break;
				}

				// Led OFF & Wait
				ledCalibration->show(ledCalibration->strip->Color(0,0,0,0));
				vTaskDelay(250 / portTICK_RATE_MS);
			}

			// Set Calibration
			// Eger hala otomatik modda ise
			if(device->mode == 1){
				device->hue = random(0, 65535);
				device->saturation = random(0, 255);
				device->value = random(0, 255);
			}
			
			
		}else if(colorSensor->isBegin == false && device-> mode == 1){
			// Error Sensor: Red Led Blink
			device->hue = 0;
			device->saturation = 255;
			device->value = 255;
			vTaskDelay(250 / portTICK_RATE_MS);
			device->hue = 0;
			device->saturation = 255;
			device->value = 0;
			vTaskDelay(250 / portTICK_RATE_MS);
		}
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}
void setup() {
	Serial.begin(115200);

	device->begin();
	// config();

	bluetooth->setup();
	led->begin();
	ledCalibration->begin();
	colorSensor->begin();

	// for testing
	// if(colorSensor->isBegin == false){
	// 	Serial.println("Error Color Sensor");
	// 	while(1){
	// 		led->show(led->strip->Color(255,0,0,0));
	// 		delay(250);
	// 		led->show(led->strip->Color(0,0,0,0));
	// 		delay(250);
	// 	}
	// }

	// Publish Initial Values
	bluetooth->publishDeviceInfo();
	bluetooth->publishBatteryLevel();
	bluetooth->publishBatteryStatus();

	xQueueBluetooth = xQueueCreate(50, sizeof( xMesageBluetooth ));
	if(xQueueBluetooth == NULL){ Serial.println("ERROR QUEUE"); }
	
	// Bluetooth
	xTaskCreatePinnedToCore(bluetoothSendQueue, "bluetoothSendQueue", 10000/2, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(sendLedValues, "sendLedValues", 10000/2, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(sendBattery, "sendBattery", 10000/2, NULL, 1, NULL, 0);
	
	// Led
	xTaskCreatePinnedToCore(ledTask, "ledTask", 10000/2, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(sensorTask, "sensorTask", 10000/2, NULL, 1, NULL, 1);
}

void loop() {
}