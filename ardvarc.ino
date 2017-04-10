#include <DriveControl.h>

DriveControl driver;

void setup() {
	driver.setMotorPins(2,3,4,5,6,7);
}

void loop() {
	driver.run();
}
