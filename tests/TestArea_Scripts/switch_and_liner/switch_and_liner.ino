#include <SensorControl.h>
#include <Array.h>
#include <ARDVARC_UTIL.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is FRiReL; Line Tracker
	sensors.setSensorPins(10, 11, 8, 9, 12);
}

void loop() {
	// Test the baseline sensors
    // Test Mode display
  Serial.print("Switch: " + String(isTestMode()));
  Serial.println(", Under: " + String(sensors.isFloorStart()));
}
