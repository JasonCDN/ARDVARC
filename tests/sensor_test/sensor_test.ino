#include <SensorControl.h>
#include <Array.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is Front 1,2,3; Rear 1; Line Tracker
	sensors.setSensorPins(8, 9, 10, 11, 12);
}

void loop() {
  Array<int> comps = Array<int>(3);
  sensors.getDistanceComponents(comps);
	for (int i = 0; i < 3; ++i) {
		Serial.println(comps[i]);
	}
	delay(250); // Very bad practice, but works for a test
}
