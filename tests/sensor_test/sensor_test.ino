#include <SensorControl.h>
#include <Array.h>

SensorControl sensors;

void setup() {
	Serial.begin(9600);
	// Order is Front 1,2,3; Rear 1; Line Tracker
	sensors.setSensorPins(10, 11, 8, 9, 12);
  pinMode(13, OUTPUT);
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
 if (sensors.isMagInRange()) {
  digitalWrite(13, HIGH);
 } else {
  digitalWrite(13, LOW);
 }

 // test magnetic sensor axis outputs
 Array<float> mags = Array<float>(3);
 sensors.getMagComponents(mags);
 Serial.print(mags[0]); // X
 Serial.print(", ");
 Serial.print(mags[1]); // Y
 Serial.print(", ");
 Serial.print(mags[2]); // Z
 Serial.print(", ");
 Serial.println(sensors.getMagStrength());
  


/*
	// Test the TCRT5000 sensor

	Serial.println(sensors.isFloorMain());
	delay(100);*/

}
