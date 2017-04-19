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

}

void SensorControl::setSonarSpacing(int spacing1, int spacing2 = -1) {
	if (spacing1 < 0) { // Spacing1 is negative - useless
		return;
	} else { // Then, spacing 2 must be 0 or positive, so...
		if (spacing2 < 0) { // If spacing2 is negative, set both spacings to be spacing1
			_spacing1 = spacing1;
			_spacing2 = spacing1;
		} else { // Otherwise both are positive or 0, so set them to their respective values
			_spacing1 = spacing1;
			_spacing2 = spacing2;
		}
	}
}