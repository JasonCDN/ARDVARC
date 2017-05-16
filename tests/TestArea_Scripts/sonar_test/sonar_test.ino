#include <SensorControl.h>
#include <Array.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is FRiReL; Line Tracker
	sensors.setSensorPins(10, 11, 8, 9, 12);
}

void loop() {
	// Test the ultrasonics

	Array<int> comps = Array<int>(4); // Make a zeroed 4-element array
	sensors.fillDistArray(comps); // Give to the function to fill array

	// Not best practice, but shows results in serial plotter
  Serial.print("Front: ");
	Serial.print(comps[0]); // Front
	Serial.print(", Right: ");
	Serial.print(comps[1]); // Right
	Serial.print(", Rear: ");
	Serial.print(comps[2]); // Rear
	Serial.print(", Left: ");
	Serial.println(comps[3]); // Left
}
