/* 

Library to control the sensor array built into the ARDVARC platform. There's a
lot of functionality packed into this class, based on individual sensor
libraries. See the function reference in the README for detailed documentation.

Author: Jason Storey
License: GPLv3

*/

#include <SensorControl.h>
#include <Wire.h>

/*

Setting parameters

*/

void SensorControl::setSensorPins(int front, int right, int rear, int left, int line_tracker) {
	// MAX_SONAR_DIST is in mm, so divide by 10 to get cm for the NewPing functions
	front_sonar = NewPing(front, front, MAX_SONAR_DIST/10); // Trigger and echo are the same
	right_sonar = NewPing(right, right, MAX_SONAR_DIST/10);
	rear_sonar = NewPing(rear, rear, MAX_SONAR_DIST/10); 
	left_sonar  = NewPing(left, left, MAX_SONAR_DIST/10); 
	floor1 = TCRT5000(line_tracker); // We only have a receiving pin

	// Activate the Magnetic Sensor
	// Note that we have a serial output so we can detect if we have a freeze
	if (Serial) Serial.println("Activating Magnetic Sensor...");
	mag.begin();
	mag.setRange(HMC5883L_RANGE_0_88GA);
	if (Serial) Serial.println("Activated.");
}


/*

Ultrasonic Sensors

*/


// Modifies a 4-element array of distance measurements (clockwise from front).
void SensorControl::fillDistArray(Array<int> array) {
	array[0] = getDistance(front_sonar);
	delay(PING_INTERVAL);
	array[1] = getDistance(right_sonar);
	delay(PING_INTERVAL);
	array[2] = getDistance(rear_sonar);	
	delay(PING_INTERVAL);
	array[3] = getDistance(left_sonar);
}

// INDIVIDUAL SONARS:
int SensorControl::getFrontDistance() {
	return getDistance(front_sonar);
}

int SensorControl::getRightDistance() {
	return getDistance(right_sonar);
}

int SensorControl::getRearDistance() {
	return getDistance(rear_sonar);
}

int SensorControl::getLeftDistance() {
	return getDistance(left_sonar);
}


// Returns the distance ping in mm (rather than cm)
// Takes a sensor object and returns its median ping times 10 (convert to mm).
int SensorControl::getDistance(NewPing sonar) {
	delay(getPingDelay()); // Stop crosstalk
	int dist = NewPing::convert_cm(sonar.ping_median(3)) * 10;
	_last_ping_time = millis();
	return dist;
} 

// From current time and the last ping time, return a 
// suitable minimal delay (in ms). Based on PING_INTERVAL.
int SensorControl::getPingDelay() {
	int ping_diff = abs(millis() - _last_ping_time);
	if (ping_diff > PING_INTERVAL) {
		return 0;
	} else {
		return PING_INTERVAL - ping_diff;
	}
}



/*

Line Tracking Sensor

*/

// This is a wrapper around the .isClose() function with timing logic.
// Everything is based on this.
bool SensorControl::isFloorMain() {
	bool floor_state = floor1.isClose();
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

// Returns 3D vector magnitude
float magtd3(float a, float b, float c) {
	return sqrt(square(a) + square(b) + square(c));
}

// Modifies an x,y,z array of ints with field components
void SensorControl::getMagComponents(Array<float> array) {

	Vector vec = mag.readNormalize();

	array[0] = vec.XAxis;
	array[1] = vec.YAxis;
	array[2] = vec.ZAxis;

	// Make sure to add magnitude to history
	float magtd = magtd3(array[0], array[1], array[2]);
	// Backwards shifting for-loop (leave first element)
	for (int i = 3; i > 0 ; --i) {
		_mag_history[i] = _mag_history[i - 1];
	}
	_mag_history[0] = magtd;
} 

// Returns xy plane angle of displacement
int SensorControl::getMagBearing() {
	Vector vec = mag.readNormalize();

	return atan2(vec.YAxis, vec.XAxis) * 180/PI;
} 

// Returns angle of tile from horizon (negative if towards the ground)
int SensorControl::getMagElevation() {
	Vector vec = mag.readNormalize();

	return atan2(vec.ZAxis, vec.XAxis) * 180/PI;
}

// Returns the strength (magnitude) of the magnetic field
float SensorControl::getMagStrength() {
	Array<float> comps = Array<float>(3);
	getMagComponents(comps);
	return _mag_history[0];
} 

// Returns a value between 0 and 1 based on how much the reading has changed in recent times
float SensorControl::deltaMagScore(int interval = 100) {
	// Break variables into a mathable form (for readability);
	float a = _mag_history[0], b = _mag_history[1], c = _mag_history[2];
	float avg = (a + b + c) / 3;
	float std_dev = sqrt((square(a-avg) + square(b-avg) + square(c-avg))/3);
	return constrain(std_dev / 50, 0, 1); // "Normalized" standard deviation
} 

// True if none of the axial components are maxed out
bool SensorControl::isMagValid() {
	Array<float> comps = Array<float>(3);
	getMagComponents(comps);
	for (int i = 0; i < 3; ++i) {
		if (abs(comps[i]) > (290)) {
			return false;
		}
	}
	return true;
} 

// True if the magnitude of the signal is far enough from Earth's magnetic field
bool SensorControl::isMagInRange() {
	return abs(getMagStrength() - EARTH_FIELD) > MAG_THRESHOLD;
} 
