#include <DriveControl.h>
#include <SensorControl.h>
#include <ArmControl.h>
#include <ARDVARC_UTIL.h>

DriveControl driver;
SensorControl sensors;


void setup() {
	Serial.begin(9600);
}

void loop() {
	Serial.println(isTestMode());
}
