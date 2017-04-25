#include <SensorControl.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is Front 1,2,3; Rear 1; Line Tracker
	sensors.setSensorPins(8, 9, 10, 11, 12);
}

void loop() {
	for (int i = 0; i < 3; ++i) {
		Serial.print(sensors.getDistanceComponents());
	}
	Serial.print('\n');
	delay(250); // Very bad practice, but works for a test
}
