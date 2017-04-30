#include <DriveControl.h>
#include <SensorControl.h>

DriveControl driver;
SensorControl sensors;


void setup() {
	Serial.begin(9600);
	// Front 1,2,3, Rear Sonar, Line Tracker
	sensors.setSensorPins(11,10,9,8,12);
}

void loop() {
	// Sensor Demo
	Array<float> comps = Array<float>(3);
	// sensors.getDistanceComponents(comps);
	sensors.getMagComponents(comps);
	for (int i = 0; i < 3; ++i) {
    Serial.print(comps[i]); 
		Serial.print(", ");
	}
	Serial.print(sensors.getMagBearing());
	Serial.println();
}
