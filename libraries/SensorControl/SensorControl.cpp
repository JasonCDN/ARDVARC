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
	mag.setRange(HMC5883L_RANGE_8_1GA);
	mag.setDataRate(HMC5883L_DATARATE_75HZ);
	if (Serial) Serial.println("Activated.");
}


/*

Ultrasonic Sensors

*/


// Modifies a 4-element array of distance measurements (clockwise from front).
void SensorControl::fillDistArray(Array<int> array) {
	array[0] = getFrontDistance();
	array[1] = getRightDistance();
	array[2] = getRearDistance();	
	array[3] = getLeftDistance();
}

// INDIVIDUAL SONARS:
int SensorControl::getFrontDistance() {
	return getDistance(front_sonar);
}

int SensorControl::getRightDistance() {
	int dist = getDistance(right_sonar);
	pushToBlipStore(dist, _r_blip_hist);
	return dist;
}

int SensorControl::getRearDistance() {
	return getDistance(rear_sonar);
}

int SensorControl::getLeftDistance() {
	int dist = getDistance(left_sonar);
	pushToBlipStore(dist, _l_blip_hist);
	return dist;
}


// Returns the distance ping in mm (rather than cm)
// Takes a sensor object and returns its median ping times 10 (convert to mm).
int SensorControl::getDistance(NewPing sonar) {
	delay(getPingDelay()); // Stop crosstalk
	int dist = NewPing::convert_cm(sonar.ping_median(PING_COUNT)) * 10;
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


// Blipping

int SensorControl::getLeftBlipped() {
	return getBlippedFromStore(_l_blip_hist);
}

int SensorControl::getRightBlipped() {
	return getBlippedFromStore(_r_blip_hist);
}

// Searches the blip_store for a blip and returns "now - timestamp". Otherwise, -1.
int SensorControl::getBlippedFromStore(PingCapture blip_store[]) {
	int expected = 0;
	int rising_edge = BLIP_HIST; // Start here so the next loop won't run if there's no edge
	
	// We start i at 1 so we have something to compare with in the algorithm
	for (int i = 1; i < BLIP_HIST; ++i)	{
		// Must be a negative difference, otherwise not a rising edge
		if (blip_store[i].dist - blip_store[i - 1].dist < -1 * BLIP_THRESHOLD) {
			// Store what we expect, so we can compare the falling edge in the next loop
			rising_edge = i;
			expected = blip_store[i - 1].dist;
			break;
		}
	}

	// If we have a rising edge, this will run and return something if there's a falling edge
	for (int i = rising_edge; i < BLIP_HIST; ++i) {
		// If the value in the store is close enough to the value before the falling edge...
		if (abs(blip_store[i].dist - expected) < BLIP_RETURN_THRESHOLD) {
			return millis() - blip_store[rising_edge].s_time; // Then return the time of the rising edge.
		}
	}

	// We didn't find a falling edge to the right value, or no edges at all
	return -1;
}

void SensorControl::pushToBlipStore(int dist, PingCapture blip_store[]) {
	// Iterate backwards for a shift-then-overwrite routine
	for (int i = BLIP_HIST - 1; i >= 0; --i) {	
		if (i > 0) { // If we're in the thick of it:
			// Replace each member with the next most recent
			blip_store[i] = blip_store[i - 1];
			// (Effectively a right shift)
		} else if (i == 0) { // Otherwise, we're at the top of the history
			// So put our new value in the right spot
			blip_store[i].dist = dist; // Store distance
			blip_store[i].s_time = millis(); // Then store the timestamp
		}
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
	return sqrt(square(abs(a)) + square(abs(b)) + square(abs(c)));
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

	// Find angle in the Q1 section
	// NOTE: We're using ZAxis because the sensor is mounted vertically
	int angle = atan2(vec.ZAxis, vec.XAxis) * 180.0/PI + R_CORRECTION;

	return angle;
} 

// Returns angle of tile from horizon (negative if towards the ground)
int SensorControl::getMagElevation() {
	Vector vec = mag.readNormalize();

	return atan2(vec.YAxis, vec.XAxis) * 180/PI;
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
		if (abs(comps[i]) > (2000)) {
			return false;
		}
	}
	return true;
} 

// True if the magnitude of the signal is far enough from background magnetic field
bool SensorControl::isMagInRange() {
	return abs(getMagStrength() - BACKGROUND_FIELD) > MAG_THRESHOLD;
} 
