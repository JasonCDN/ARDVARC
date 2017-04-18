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

#define EARTH_FIELD 8.9E-3 // Tesla (NOT ACCURATE)


class SensorControl
{
public:
	float getWallAngle(); // Calculates the angle to the wall from the normal (+ve to the right, -ve to the left)
	float getWallDistance(); // Returns the closest distance measured from the front
	float* getDistanceComponents(); // Returns a 3-element array of distance measurements (from left to right).
	float getRearDistance(); // Returns the distance to the closest rear obstacle (in line of sight of sensor).

	bool isFloorStart(); // Returns true if the floor is dark
	bool isFloorMain(); // Returns true if the floor is light
	short getFloorType(); // Returns 1 if the floor is dark, 2 if the floor is light.
	bool hasFloorChanged(interval = 100); // Returns true if the floor has changed in the given interval
	int getTimeFloorLastChanged(); // Returns how many milliseconds ago the floor changed

	int* getMagComponents(); // Returns an x,y,z array of ints with field components
	float getMagBearing(); // Returns xy plane angle of displacement
	float getMagElevation(); // Returns angle of tile from horizon (negative if towards the ground)
	float getMagStrength(); // Returns the strength of the magnetic field
	float deltaMagScore(interval = 100); // Returns a value between 0 and 1 based on how much the reading has changed in recent times
	bool isMagValid(); // True if none of the axial components are maxed out
	bool isMagInRange(); // True if the magnitude of the signal is far enough from Earth's magnetic field
private:
	// Pins
	short _distance_pins[4];
	short _floor_pin;
	// Magnetic sensor runs from I2C

	// State variables
	int _last_floor_time; // Time value in ms since last floor type change
	short _mag_history[3]; // Keeps a magnitude score of the last three readings
}


#endif