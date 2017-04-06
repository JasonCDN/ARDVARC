/* 

Library to control the dual-independent motors, ultimately controlling the
position of the vehicle. Basic functionality includes driving forward/backward
a specified distance and turning left/right a specified angle (on the spot).
The overall speed of the vehicle can also be controlled, if desired.

With a crude system of wheel rotation tracking (based on experimentally
determined rpm), the system can offer more advanced features designed to
position the vehicle where the user desires - relative to current position. It
should be noted that the accuracy of position tracking decreases with
distance, so short paths should be used (if used at all).

Other advanced features of the API include:

	* Driving and turning simultaneously
	* Turning on the spot
	* Moving to a specific point relative to vehicle's current position
	* Fine position adjustment (in all directions)
	* Moving forwards and backwards (user specifies)

See the README for examples and a detailed reference.

Important Note: This class does not keep track of where the car has been. Its
only job is to get it where you want it to go - the user will need to write
the code to record where the car has been (if desired). Additionally, if you
want the car to follow a path, then you will need to combine the "goToPoint()"
and "isDriving()" methods yourself. This is because the format of your path
may not be conducive to a pre-written method.

Author: Jason Storey
License: GPLv3

*/

#ifndef drivecontrol_h
#define drivecontrol_h

// Pull in the Arduino standard libraries
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include <L293dDriver.h> // Need the type definitions

/*

This is "Sir DriveControl". His job is to make the motors turn in such a precise
manner that the car ends up where you want it to. All you have to do is say
where and how fast!

See the README for specific details and examples.

TODO: Need an "update" function to control changes in speed (should be called often)

*/
class DriveControl
{
public:
	DriveControl(float wheel, float rpdc = 1) : _wheel_dia(wheel), _rpdc(rpdc) {};
	
	void setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4); // Pins for the motors

	void run(); // This class runs on a queue system. This function must be called to progress the queue. See README.
	void clearQueue(); // Stop everything and remove all instructions from queue.

	void forward(float dist, float speed_scalar = 1); // Moves forwards a certain `dist` (in mm). Optional speed scalar.
	void backward(float dist, float speed_scalar = 1); // Moves backwards a certain `dist` (in mm).
	void nudge(float x, float y, float speed_scalar = 0.5); // Uses fine adjustment movements to move a small distance

	void goToPoint(float x, float y); // Pass in relative coordinates (in mm) to travel there.
	void setP2PMode(); // Sets point to point driving mode
	void setArcMode(); // Sets arc driving mode

	void turnRight(); // Shortcut for turnAngle(90)
	void turnLeft(); // Shortcut for turnAngle(-90)
	void turnAngle(float theta); // Will turn the vehicle a certain angle relative to its current position.
	
	void setSpeed(float speed) { _global_speed_scalar = speed; }; // Modifies the overall speed of the car (every motion is scaled by `speed`)
	void setRevsPerDC(float rpdc) { _rpdc = rpdc; }; // Used to keep track of how far car has gone in a certain amount of time
	void setWheelDiameter(float wheel_dia); // This is in mm, and is used to keep track of travel distance
	bool isDriving(); // Returns the "_driving" flag, for external use. Will be true when items are in queue.
private:
	void move(float dist, float speed_scalar = 1); // Forward/backward translation
	void left(float dist, float time);   // These move the wheels with L293D, with built in global speed scalar.
	void right(float dist, float time);  // Time is relative to 1. A time of 2 means a speed of 0.5 (etc).
	void wheel(Motor motor, float dist, float time); // Left and right driving code

	L293D _motors;
	bool _driving = false; // Flag for if driving or not. Could be used externally to perform an interrupt routine.
	float _global_speed_scalar = 1; // Between 0 and 1 - scales the speed down from the max.
	const float _wheel_dia; // Wheel diameter - for distance tracking while travelling
	float _rpdc; // Revs-per-Duty-cycle. Note that this is actually RPM per Duty Cycle.
	static const byte QUEUE_SIZE = 32; // Maximum number of items in the queue allowed.

	struct drive_instruction {
		bool is_instruction = false; // Because static array, need to know if should actually run.
		unsigned long start_time; // Set by run when the instruction is called. Used to track when it should stop.
		unsigned int duration; // Decides when to halt the instruction. If 0, instruction runs until this is changed.
		byte left_speed;
		bool left_direction; // 1 is forward, 0 is backward
		byte right_speed;
		bool right_direction;
	};

	drive_instruction queue[QUEUE_SIZE]; // Static array to hold queue instructions.
};

#endif