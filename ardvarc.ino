#include <DriveControl.h>
#include <SensorControl.h>

DriveControl driver;
SensorControl sensors;

void setup() {
	sensors.setSensorPins(1,2,3,4,5);
}

void loop() {
	driver.run();
}
