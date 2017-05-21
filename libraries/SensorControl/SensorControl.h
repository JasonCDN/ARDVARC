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


#define MAG_ADDR 0x1E		 // Address of the HMC5883L
#define BACKGROUND_FIELD 300 // milligauss - used to determine if magnetic field is of target
#define MAG_THRESHOLD 90     // Number of milligauss deviation before considered a real signal.

#define R_CORRECTION 90     // Angle added to the bearing to correct for negatives
#define PING_COUNT 2		// Number of pings to average out for our final value
#define PING_INTERVAL 20    // Minimum amount of time to wait (ms) in-between pings.
#define MAX_SONAR_DIST 3000 // Maximum distance for sensing (in mm).

// Blipping constants
#define BLIP_CAP 2000 // distances are capped at this to stop noise from being interpreted as a blip
#define BLIP_HIST 5   // Number of readings to keep in history
#define BLIP_THRESHOLD 50        // Difference in reading that counts as a falling edge on the blip
#define BLIP_RETURN_THRESHOLD 50 // Difference from expected reading (from falling edge) that counts as a rising edge


class SensorControl
{
public:

	// NOTE: Initializing all subclasses with a default value that won't work, because I want 
	// the API to set pins *after* the class is constructed. That's just the pattern.
	SensorControl() : front_sonar(0,0), right_sonar(0,0), rear_sonar(0,0), left_sonar(0,0), floor1(0) {};

	void setSensorPins(int front, int right, int rear, int left, int line_tracker); // Sonars F,Ri,Re,L; Rear 1; Line Tracker

	// Ultrasonics
	void fillDistArray(Array<int> array); // Mods a 4-element array of distance measurements (starting at front, clockwise).
	int getFrontDistance(); // Returns the distance to the closest Front obstacle (in line of sight of sensor).
	int getRightDistance(); // As above, for Right
	int getRearDistance(); // As above, for Rear
	int getLeftDistance(); // As above, for Left
	int getBehindDistance() { return getRearDistance(); }; // Alias for getRearDistance

	// Ultrasonic blipping
	void SensorControl::getLeftBlipped(Array<int> out); // Returns the number of milliseconds since last blip on left sonar
	void SensorControl::getRightBlipped(Array<int> out); // Same as above, but for the right sonar

	// TCRT5000
	bool isFloorStart(); // Returns true if the floor is dark
	bool isFloorMain(); // Returns true if the floor is light
	short getFloorType(); // Returns 1 if the floor is dark, 2 if the floor is light.
	bool hasFloorChanged(int interval = 100); // Returns true if the floor has changed in the given interval
	int getTimeFloorLastChanged(); // Returns how many milliseconds ago the floor changed

	 // Magnetic Sensor
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

	struct PingCapture {
		unsigned long s_time = 0;
		unsigned short dist = 0;
	};

	PingCapture _l_blip_hist[BLIP_HIST]; // Keeps a record of left pings
	PingCapture _r_blip_hist[BLIP_HIST]; // Keeps a record of right pings
	int _last_right_blip;
	int _last_left_blip;
	void pushToBlipStore(int dist, PingCapture blip_store[]);
	void SensorControl::getBlippedFromStore(PingCapture blip_store[], Array<int> out); // Fills an array with time and dist of blip

	int getDistance(NewPing sonar); // Returns the distance ping in mm (rather than cm)
	int getPingDelay(); // Returns a delay (in ms) that should work to wait for next ping
};


#endif