#include <DriveControl.h>
#include <SensorControl.h>
#include <ArmControl.h>
#include <ARDVARC_UTIL.h>

DriveControl driver;
SensorControl sensors;


void setup() {
  Serial.begin(9600);
  // Arm
  arm.setServoPins(A1, A2, A3);
  // Sensors
  sensors.setSensorPins(10, 11, 8, 9, 12);
  // Driver (motors)
  driver.setSpeed(1);
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(11);
  driver.setWheelScales(2, 1);
  driver.setBackScaling(0.1);
}

void loop() {
	Serial.println(isTestMode());
}
