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

}
