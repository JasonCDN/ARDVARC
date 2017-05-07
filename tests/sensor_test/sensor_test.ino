#include <SensorControl.h>
#include <Array.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is Front 1,2,3; Rear 1; Line Tracker
	sensors.setSensorPins(10, 11, 8, 9, 12);
}

void loop() {
/*
	// Test the ultrasonics

	Array<int> comps = Array<int>(4); // Make a zeroed 4-element array
	sensors.fillDistArray(comps); // Give to the function to fill array

	// Not best practice, but shows results in serial plotter
	Serial.print(comps[0]); // Front
	Serial.print(",");
	Serial.print(comps[1]); // Right
	Serial.print(",");
	Serial.print(comps[2]); // Rear
	Serial.print(",");
	Serial.println(comps[3]); // Left
 */
 Serial.println(sensors.getMagBearing());


/*
	// Test the TCRT5000 sensor

	Serial.println(sensors.isFloorMain());
	delay(100);*/

}
