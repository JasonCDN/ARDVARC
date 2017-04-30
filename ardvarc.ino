#include <DriveControl.h>
#include <SensorControl.h>

DriveControl driver;
SensorControl sensors;


void setup() {
  // Front 1,2,3, Rear Sonar, Line Tracker
	sensors.setSensorPins(11,10,9,8,12);
	Serial.begin(9600);
}

void loop() {
	Array<int> comps = Array<int>(3);
	sensors.getDistanceComponents(comps);
	for (int i = 0; i < 3; ++i) {
		Serial.print(comps[i]);
		Serial.print(", ");
	}
  Serial.print(sensors.getRearDistance());
	Serial.println();
}
