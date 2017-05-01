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
  driver.setRevsPerDC(60);
  driver.setSpeed(0.3); // MUST RUN AT 30% WITH THESE MOTORS!!
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
 // WARNING: One of them only runs well backwards
 /*driver.forward(100);
 driver.backward(100);
 driver.turnRight(90);
 driver.turnLeft(90);
 while (true) {
  driver.run();
  Serial.println("R");
  delay(500);
 };
 */
}
