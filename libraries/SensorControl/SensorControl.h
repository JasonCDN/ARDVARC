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
#include <Array.h>
#include <Math.h>
#include <Wire.h>

#define MAG_ADDR 0x1E		// Address of the HMC5883L
#define EARTH_FIELD 630     // milligauss
#define PING_INTERVAL 40    // Minimum amount of time to wait in-between pings.
#define ANGLE_THRESHOLD 5   // Max angle measured difference before filtering applies.
#define MAG_THRESHOLD 70    // Number of milligauss deviation before considered a real signal.
#define FRONT_SENS_NUM 3    // Number of sensors on the front.
#define MAX_SONAR_DIST 5000 // Maximum distance for sensing (in mm).


class SensorControl
{
public:

	// NOTE: Initializing all subclasses with a default value that won't work, because I want 
	// the API to set pins *after* the class is constructed. That's just the pattern.
	SensorControl() : front_sonar(0,0), right_sonar(0,0), rear_sonar(0,0), left_sonar(0,0), floor1(0) {};

	void setSensorPins(int front, int right, int rear, int left, int line_tracker); // Sonars F,Ri,Re,L; Rear 1; Line Tracker

	void fillDistArray(Array<int> array); // Mods a 4-element array of distance measurements (starting at front, clockwise).
	int getFrontDistance(); // Returns the distance to the closest Front obstacle (in line of sight of sensor).
	int getRightDistance(); // As above, for Right
	int getRearDistance(); // As above, for Rear
	int getLeftDistance(); // As above, for Left
	int getBehindDistance() { return getRearDistance(); }; // Alias for getRearDistance

	bool isFloorStart(); // Returns true if the floor is dark
	bool isFloorMain(); // Returns true if the floor is light
	short getFloorType(); // Returns 1 if the floor is dark, 2 if the floor is light.
	bool hasFloorChanged(int interval = 100); // Returns true if the floor has changed in the given interval
	int getTimeFloorLastChanged(); // Returns how many milliseconds ago the floor changed

	void getMagComponents(Array<float> array); // Mods an x,y,z array of ints with field components
	int getMagBearing(); // Returns xy plane angle of displacement
	int getMagElevation(); // Returns angle of tile from horizon (negative if towards the ground)
	float getMagStrength(); // Returns the strength of the magnetic field
	float deltaMagScore(int interval = 100); // Returns a value between 0 and 1 based on how much the reading has changed in recent times
	bool isMagValid(); // True if none of the axial components are maxed out
	bool isMagInRange(); // True if the magnitude of the signal is far enough from Earth's magnetic field
private:
	/*
		Sensor Objects (constructed, then overwritten)
	*/
	NewPing front_sonar;
	NewPing right_sonar;
	NewPing rear_sonar;
	NewPing left_sonar;
	TCRT5000 floor1; // Note, we need the 1, so we don't override floor();

	// Magnetic sensor runs from I2C, so no pins declared
	HMC5883L mag;

	// State variables
	unsigned long _last_floor_time; // Time value in ms since last floor check (and it changed)
	unsigned long _last_ping_time = 0;  // Time value in ms since last ping (to avoid cross talk)
	bool _last_floor_state;
	float _mag_history[3]; // Keeps the magnitude score of the last three readings

	int getDistance(NewPing sonar); // Returns the distance ping in mm (rather than cm)
	int getPingDelay(); // Returns a delay (in ms) that should work to wait for next ping
};


#endif