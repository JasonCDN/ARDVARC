#include <SensorControl.h>
#include <Array.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is Front 1,2,3; Rear 1; Line Tracker
	sensors.setSensorPins(8, 9, 10, 11, 12);
}

void loop() {
/*
	// Test the 3 front ultrasonics:

	Array<int> comps = Array<int>(3); // Make a zeroed 3-element array
	sensors.getDistanceComponents(comps); // Give to the function to fill array

	// Not best practice, but shows results in serial plotter
	Serial.print(comps[0]); // Front 1
	Serial.print(",");
	Serial.print(comps[1]); // Front 2
	Serial.print(",");
	Serial.print(comps[2]); // Front 3
	Serial.print(",");
	Serial.println(sensors.getRearDistance());
	// Test the other sensor methods:
	Serial.println(sensors.getWallDistance());
*/

/*
	// Test the TCRT5000 sensor
*/
	Serial.println(sensors.isFloorMain());
  delay(100);

}
