#include <DriveControl.h>
#include <SensorControl.h>

DriveControl driver;
SensorControl sensors;


void setup() {
	Serial.begin(9600);
	// Front 1,2,3, Rear Sonar, Line Tracker
	sensors.setSensorPins(11,10,9,8,12);
  // en12, in1, in2, en34, in3, in4
  driver.setMotorPins(3,2,4,5,6,7);
  driver.setWheelDiameter(42);
  driver.setTrackWidth(90);
  driver.setRevsPerDC(1);
  driver.setSpeed(0.1); // MUST RUN AT 10% WITH THESE MOTORS!!
}

void loop() {
	// Sensor Demo
	/*Array<float> comps = Array<float>(3);
	// sensors.getDistanceComponents(comps);
	sensors.getMagComponents(comps);
	for (int i = 0; i < 3; ++i) {
    Serial.print(comps[i]); 
		Serial.print(", ");
	}
	Serial.print(sensors.getMagBearing());
	Serial.println();
 */

 // Motor Demo
 driver.forward(10);
 while (true) {
  Serial.println("Running...");
  driver.run();
  delay(1000);
 }
}
