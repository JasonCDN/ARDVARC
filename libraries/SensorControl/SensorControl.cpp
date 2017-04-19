/* 

Library to control the sensor array built into the ARDVARC platform. There's a
lot of functionality packed into this class, based on individual sensor
libraries. See the function reference in the README for detailed documentation.

Author: Jason Storey
License: GPLv3

*/

#include <SensorControl.h>

/*

Setting parameters

*/

void SensorControl::setSensorPins(int f1, int f2, int f3, int r1, int lt) {
	NewPing front1(f1, f1, MAX_SONAR_DIST); // Trigger and echo are the same

	this->front1 = front1;
}