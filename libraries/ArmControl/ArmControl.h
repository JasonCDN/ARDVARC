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
#include <ARDVARC_UTIL.h>


#define SET_INITIAL true // Whether or not to set the initial position of the servos on setup


/*
	Maximum, minimum (and thus starting angle) for the servos.
	(Some *could* go more, but would then break the continuity of the system).
*/
#define BASE_MAX 20 
#define BASE_MIN 180
#define DUAL_OFFSET -10
#define GRIP_MIN 30
#define GRIP_MAX 90
#define GRIP_SCALE 0.5 // Scale input angle by this much before mapping

#define MAX_STEP 40 // How many ms is a max (i.e servo speed is 0) for servo speeds
#define MIN_STEP 2  // How many ms is a min (i.e servo speed is 1) for servo speeds

class ArmControl
{
public:
	ArmControl() {};
	void setServoPins(int base1, int base2, int grip);
	void setServoSpeed(float speed); // Takes a decimal (from 0 -> 1) and uses that to move a servo.
	void setAngle(int angle); // Set base servo (arm angle) from 0 (fully retracted) to 180 (fully extended)
	int getAngle();  // Reads the servo's angle (Servo.read) and returns it
	void setGrip(int angle);  // Set gripping servo's angle from 0 (fully closed) to 90 (fully open)
	int getGrip();   // Reads the servo's angle and returns it

	void collectTarget(); // Runs through a pre-defined motion set to pick up a target positioned in the jaws
	void restPosition(); // Moves the arm to be "at rest"
	void readyPosition(); // Moves the arm to be ready to pick up a target (jaws open)
private:
	Servo s_base1; // Servo controlling the arm base (angle)
	Servo s_base2; // Servo controlling the arm base (second)
	Servo s_grip; // Servo controlling the grip position
	float _servo_speed = 0.5; // Controlling variable for a servo's speed when moving

	// Moves a servo from where it is, to where it needs to be. 
	// This function serves two purposes - mapping an input angle to the actual servo's angle
	// and then using the global _servo_speed variable to interpolate positions (without going too fast)
	void moveServo(Servo servo, int angle, int min_map, int max_map, int min_true, int max_true);
	void moveDualArm(Servo servo1, Servo servo2, int angle, int min_map, int max_map, int min_true, int max_true);
};

#endif