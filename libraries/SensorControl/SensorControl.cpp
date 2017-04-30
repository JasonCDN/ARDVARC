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

void SensorControl::setSensorPins(int f1, int f2, int f3, int r1, int lt) {
	// MAX_SONAR_DIST is in mm, so divide by 10 to get cm for the NewPing functions
	front1 = NewPing(f1, f1, MAX_SONAR_DIST/10); // Trigger and echo are the same
	front2 = NewPing(f2, f2, MAX_SONAR_DIST/10);
	front3 = NewPing(f3, f3, MAX_SONAR_DIST/10); 
	rear1  = NewPing(r1, r1, MAX_SONAR_DIST/10); 
	floor1 = TCRT5000(lt); // We only have a receiving pin

	// Activate the Magnetic Sensor
	Wire.begin();
	Wire.beginTransmission(MAG_ADDR); //open communication with HMC5883
	Wire.write(0x02); //select mode register
	Wire.write(0x00); //continuous measurement mode
	if (Serial) Serial.println("Activating Magnetic Sensor...");
	Wire.endTransmission();
	if (Serial) Serial.println("Activated.");
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

float calculateAngle(int dist1, int dist2, int spacing); // Needed for sub-functionalty (definition later)

// Returns the distance ping in mm (rather than cm)
// Takes a sensor object and returns its median ping times 10 (convert to mm).
int SensorControl::getDistance(NewPing sonar) {
	return NewPing::convert_cm(sonar.ping_median(3)) * 10;
} 

// Calculates the angle to the wall from the normal (+ve to the right, -ve to the left)
int SensorControl::getWallAngle() {
	// Get the 3 distances (as components)
	Array<int> c = Array<int>(FRONT_SENS_NUM);
	getDistanceComponents(c);

/*

	This is done in two pairs, labelled (1) and (2). Calculations are compared
	to check for likely data. and filtered down to be more accurate.

*/

	float n_theta1 = calculateAngle(c[2], c[1], _spacing1);
	float n_theta2 = calculateAngle(c[1], c[0], _spacing2);

	// Decide if we need to choose a most-accurate, or average
	if (abs(n_theta1 - n_theta2) > ANGLE_THRESHOLD) {
		// Return the smallest angle (because that's likely from a more consistent surface)
		return floor(min(n_theta2, n_theta2));
	} else {
		return floor((n_theta1 + n_theta2)/2);
	}
}

float calculateAngle(int dist1, int dist2, int spacing) { // Right-side sonar first!
	// If this is negative, the right-side sonar in the pair is
	// reading a smaller distance (i.e. we are angled left of the wall normal)
	int dist_delta = dist1 - dist2;
	double theta = 90 - sin(dist_delta/spacing) * 180/PI;
	return theta;
}

// Returns the closest distance measured from the front
int SensorControl::getWallDistance() {
	Array<int> components = Array<int>(FRONT_SENS_NUM);
	getDistanceComponents(components);

	return components.getMin();
}

// Modifies a 3-element array of distance measurements (from left to right).
void SensorControl::getDistanceComponents(Array<int> array) {
	array[0] = getDistance(front1);
	delay(PING_INTERVAL);
	array[1] = getDistance(front2);
	delay(PING_INTERVAL);
	array[2] = getDistance(front3);
}

// Returns the distance to the closest rear obstacle (in line of sight of sensor).
int SensorControl::getRearDistance() {
	delay(PING_INTERVAL/2); // Stop crosstalk
	return getDistance(rear1);
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
	int x,y,z; //triple axis data

	//Tell the HMC5883 where to begin reading data
	Wire.beginTransmission(MAG_ADDR);
	Wire.write(0x03); //select register 3, X MSB register
	Wire.endTransmission();


	//Read data from each axis, 2 registers per axis
	Wire.requestFrom(MAG_ADDR, 6);
	if(6<=Wire.available()){
		x = Wire.read()<<8; //X msb
		x |= Wire.read(); //X lsb
		z = Wire.read()<<8; //Z msb
		z |= Wire.read(); //Z lsb
		y = Wire.read()<<8; //Y msb
		y |= Wire.read(); //Y lsb
	}

	array[0] = x * MSCALE;
	array[1] = y * MSCALE;
	array[2] = z * MSCALE;

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

} 

// Returns angle of tile from horizon (negative if towards the ground)
int SensorControl::getMagElevation() {

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
		if (abs(comps[i]) > (4000 * MSCALE)) {
			return false;
		}
	}
	return true;
} 

// True if the magnitude of the signal is far enough from Earth's magnetic field
bool SensorControl::isMagInRange() {
	return abs(getMagStrength() - EARTH_FIELD) > MAG_THRESHOLD;
} 
