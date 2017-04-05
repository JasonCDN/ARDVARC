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

#include "L293d/L293dDriver.h" // Need the type definition

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
	DriveControl();
	
	void setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4); // Pins for the motors
	void forward(float dist, float speed_scalar = 1); // Moves forwards a certain `dist` (in mm). Optional speed scalar.
	void backward(float dist, float speed_scalar = 1); // Moves backwards a certain `dist` (in mm).
	void setSpeed(float speed); // Modifies the overall speed of the car (every motion is scaled by `speed`)
	void nudge(float x, float y); // Uses fine adjustment movements to move a small distance
	void turnRight(); // Shortcut for turnAngle(90)
	void turnLeft(); // Shortcut for turnAngle(-90)
	void turnAngle(float theta); // Will turn the vehicle a certain angle relative to its current position.
	void goToPoint(float x, float y); // Pass in relative coordinates (in mm) to travel there.
	void setP2PMode(); // Sets point to point driving mode
	void setArcMode(); // Sets arc driving mode
	void setRevsPerVolt(float rpv); // Used to keep track of how far car has gone in a certain amount of time
	void setWheelDiameter(float wheel_dia); // This is in mm, and is used to keep track of travel distance
	bool isDriving(); // Returns the "_driving" flag
private:
	void move(float dist, float speed_scalar = 1);

	L293D _motors;
	bool _driving = false; // Flag for if driving or not. Could be used internally to perform an interrupt routine.
	float _speed_scalar = 1; // Between 0 and 1 - scales the speed down from the max.
	float _left_speed; // Speed of left motor
	float _right_speed; // Speed of right motor
	float _wheel_dia; // Wheel diameter - for distance tracking while travelling
	float _rpv; // Revs-per-volt. Note that this is actually RPM per Volt.
	float _x_disp; // To keep track of X axis displacement
	float _y_disp; // To keep track of Y axis displacement

};

#endif