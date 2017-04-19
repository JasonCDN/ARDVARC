/* 

Library to control the sensor array built into the ARDVARC platform. There's a
lot of functionality packed into this class, based on individual sensor
libraries. See the function reference in the README for detailed documentation.

Author: Jason Storey
License: GPLv3

*/

#ifndef sensorcontrol_h
#define sensorcontrol_h

// Pull in the Arduino standard libraries
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include <NewPing.h>
#include <HMC5883L.h>
#include <tcrt5k.h>

#define EARTH_FIELD 8.9E-3  // Tesla (NOT ACCURATE)
#define PING_INTERVAL 33    // Minimum amount of time to wait in-between pings.
#define FRONT_SENS_NUM 3    // Number of sensors on the front
#define MAX_SONAR_DIST 5000 // Maximum distance for sensing (in mm)


class SensorControl
{
public:
	void setSensorPins(int f1, int f2, int f3, int r1, int lt); // Front 1,2,3; Rear 1; Line Tracker
	void setSonarSpacing(int spacing1, int spacing2 = -1); // Pased in values must be positive and are in mm. spacing2 = spacing1 if negative.

	float getWallAngle(); // Calculates the angle to the wall from the normal (+ve to the right, -ve to the left)
	float getWallDistance(); // Returns the closest distance measured from the front
	float* getDistanceComponents(); // Returns a 3-element array of distance measurements (from left to right).
	float getRearDistance(); // Returns the distance to the closest rear obstacle (in line of sight of sensor).

	bool isFloorStart(); // Returns true if the floor is dark
	bool isFloorMain(); // Returns true if the floor is light
	short getFloorType(); // Returns 1 if the floor is dark, 2 if the floor is light.
	bool hasFloorChanged(int interval = 100); // Returns true if the floor has changed in the given interval
	int getTimeFloorLastChanged(); // Returns how many milliseconds ago the floor changed

	int* getMagComponents(); // Returns an x,y,z array of ints with field components
	float getMagBearing(); // Returns xy plane angle of displacement
	float getMagElevation(); // Returns angle of tile from horizon (negative if towards the ground)
	float getMagStrength(); // Returns the strength of the magnetic field
	float deltaMagScore(int interval = 100); // Returns a value between 0 and 1 based on how much the reading has changed in recent times
	bool isMagValid(); // True if none of the axial components are maxed out
	bool isMagInRange(); // True if the magnitude of the signal is far enough from Earth's magnetic field
private:
	/*
		Sensor Objects (not constructed until pins are set)
	*/
		// NewPing front1;
		// NewPing front2;
		// NewPing front3;
		// NewPing rear1;
		// TCRT5000 floor;

	// Magnetic sensor runs from I2C, so no pins declared
	HMC5883L mag;

	// State variables
	int _last_floor_time; // Time value in ms since last floor type change
	short _mag_history[3]; // Keeps the magnitude score of the last three readings

	float getDistance(NewPing sonar); // Returns the distance ping in mm (rather than cm)
};


#endif