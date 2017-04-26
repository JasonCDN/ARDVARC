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
	// MAX_SONAR_DIST is in mm, so divide by 10 to get cm for the NewPing functions
	front1 = NewPing(f1, f1, MAX_SONAR_DIST/10); // Trigger and echo are the same
	front2 = NewPing(f2, f2, MAX_SONAR_DIST/10);
	front3 = NewPing(f3, f3, MAX_SONAR_DIST/10); 
	rear1  = NewPing(r1, r1, MAX_SONAR_DIST/10); 
	floor  = TCRT5000(lt); // We only have a receiving pin
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



/*

Ultrasonic Sensors

*/

// Returns the distance ping in mm (rather than cm)
// Takes a sensor object and returns its ping times 10.
int SensorControl::getDistance(NewPing sonar) {
	return sonar.ping_cm(MAX_SONAR_DIST/10) * 10;
} 

// Calculates the angle to the wall from the normal (+ve to the right, -ve to the left)
int SensorControl::getWallAngle() {

}

// Returns the closest distance measured from the front
int SensorControl::getWallDistance() {

}

// Modifies a 3-element array of distance measurements (from left to right).
void SensorControl::getDistanceComponents(Array<int> array) {
	// int components[3] = {getDistance(front1), getDistance(front2), getDistance(front3)};
	array[0] = 0;
	array[1] = 2;
	array[2] = 67;
}

// Returns the distance to the closest rear obstacle (in line of sight of sensor).
int SensorControl::getRearDistance() {
	return getDistance(rear1);
}



/*

Line Tracking Sensor

*/

// This is a wrapper around the .isClose() function with timing logic.
// Everything is based on this.
bool SensorControl::isFloorMain() {
	front1.ping_cm();
	bool floor_state = floor.isClose();
	if (_last_floor_state != floor_state) {
		_last_floor_time = millis();
		_last_floor_state = floor_state;
	}
	return floor_state;
}

bool SensorControl::isFloorStart() {
	return !isFloorMain();
}


short SensorControl::getFloorType() {
	if (isFloorMain()) {
		return 2;
	} else {
		return 1;
	}
}

// Returns true if the difference between the current time and "the last
// time that we checked and the floor had changed" is within the interval
bool SensorControl::hasFloorChanged(int interval = 100) {
	isFloorMain(); // Update estimates
	return getTimeFloorLastChanged() < interval;
}

int SensorControl::getTimeFloorLastChanged() {
	return (millis() - _last_floor_time);
}



/*

3-Axis Magnetic Sensor

*/

// Returns an x,y,z array of ints with field components
int* SensorControl::getMagComponents() {

} 

// Returns xy plane angle of displacement
float SensorControl::getMagBearing() {

} 

// Returns angle of tile from horizon (negative if towards the ground)
float SensorControl::getMagElevation() {

} 

// Returns the strength of the magnetic field
float SensorControl::getMagStrength() {

} 

// Returns a value between 0 and 1 based on how much the reading has changed in recent times
float SensorControl::deltaMagScore(int interval = 100) {

} 

// True if none of the axial components are maxed out
bool SensorControl::isMagValid() {

} 

// True if the magnitude of the signal is far enough from Earth's magnetic field
bool SensorControl::isMagInRange() {

} 
