/* 

Library to control the ARDVARC's arm

Author: Jason Storey
License: GPLv3

*/

#ifndef armcontrol_h
#define armcontrol_h

// Pull in the Arduino standard libraries
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include <Servo.h> // Need the type definitions

// TODO Pull this into the ARDVARC library
#define F_DEBUG true // A debug flag that logs Serial messages (if available) when true.

/*
	Maximum, minimum (and thus starting angle) for the servos.
	(Some *could* go more, but would then break the continuity of the system).
*/
#define BASE_MAX 180 
#define BASE_MIN 0
#define GRIP_MAX 180
#define GRIP_MIN 0

class ArmControl
{
public:
	ArmControl() {};
	void setServoPins(int base, int grip);
	void setServoSpeed(float speed); // Takes a decimal (from 0 -> 1) and uses that to move a servo.
	void setAngle(); // Set base servo (arm angle) from 0 (fully retracted) to 180 (fully extended)
	int getAngle();  // Reads the servo's angle (Servo.read) and returns it
	void setGrip();  // Set gripping servo's angle from 0 (fully closed) to 90 (fully open)
	int getGrip();   // Reads the servo's angle and returns it

	void collectTarget(); // Runs through a pre-defined motion set to pick up a target positioned in the jaws
	void restPosition(); // Moves the arm to be "at rest"
	void readyPosition(); // Moves the arm to be ready to pick up a target (jaws open)
private:
	Servo s_base; // Servo controlling the arm base (angle)
	Servo s_grip; // Servo controlling the grip position
	float _servo_speed; // Controlling variable for a servo's speed when moving

	// Moves a servo from where it is, to where it needs to be. 
	// This function serves two purposes - mapping an input angle to the actual servo's angle
	// and then using the global _servo_speed variable to interpolate positions (without going too fast)
	void moveServo(Servo servo, int angle, int min_map, int max_map, int min_true, int max_true);
};

#endif