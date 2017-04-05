/* 

Library to control the dual-independent motors at the hardware level. This is
a low level library that simply turns the motors forwards or backwards and
controls their speed.

Author: Jason Storey
License: GPLv3

*/

#ifndef l293ddriver_h
#define l293ddriver_h

// Pull in the Arduino standard libraries
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

/*

This class contains all the low-level code to get the voltages and currents
where you need them to get the motors going. The user shouldn't need to use
this class for anything - the class "L293D" uses "Motor" instances to
control the vehicle.

Because this class should not be needed by the user, it is not as heavily
documented as those that are. Additionally, all documentation is inline,
rather than in the README.

*/
class Motor
{
public:
	Motor() {}; // Cannot pass pins in constructor. Breaks convention.
	void setPins(int enable, int input1, int input2); // Set up the pin numbers
	void drive(float speed); // A "speed" between -255 and 255 (-ve = backwards)
private:
	int _en = false; // Set to false to facilitate reassignment blocking (see setPins code)
	int _in1;
	int _in2;
	float _speed; // Holds the current speed, in case it is needed.
};

/*

The L293D chip is a standard H-bridge to control two motors. As such, the
class for it has two methods (left and right) that control the speed and
direction of the left and right motors.

Speed and direction is controlled with a single integer between -255 and 255.
Numbers outside the range are clipped. Negative speed values will run the
motor backwards. A "0" value lets the motor coast.

*/

class L293D
{
public:
	void setLeft(int enable1, int input1, int input2);
	void setRight(int enable2, int input3, int input4);
	void left(float speed);
	void right(float speed);
private:
	Motor _left_motor;
	Motor _right_motor;
};


#endif