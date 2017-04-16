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
#include <QueueList.h>

/*

This is "Sir DriveControl". His job is to make the motors turn in such a precise
manner that the car ends up where you want it to. All you have to do is say
where and how fast!

Don't forget to call the "run()" function!

See the README for specific details and examples.

*/
class DriveControl
{
public:
	DriveControl() {};
	
	void setSpeed(float speed); // Modifies the overall speed of the car (every motion is scaled by `speed`)
	void setRevsPerDC(float rpdc); // Used to keep track of how far car has gone in a certain amount of time
	void setWheelDiameter(float wheel); // Wheel (diameter) is in mm, and is used to keep track of travel distance
	void setTrackWidth(float track); // How far apart the drive wheels are (in mm)
	void setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4); // Pins for the motors

	void run(); // This class runs on a queue system. This function must be called to progress the queue. See README.
	void clearQueue(); // Remove all instructions from queue, finish up what we're doing.
	void stopAll(); // Clears the queue and executes an instruction to stop all motors.

	void forward(float dist, float speed_scalar = 1); // Moves forwards a certain `dist` (in mm). Optional speed scalar.
	void backward(float dist, float speed_scalar = 1); // Moves backwards a certain `dist` (in mm).

	void goToPoint(float x, float y, float speed_scalar = 1); // Pass in relative coordinates (in mm) to travel there.
	void goToPointSticky(float x, float y, float speed_scalar = 1); // As above, but don't undo last rotation
	void nudge(float x, float y, float speed_scalar = 0.5); // Uses fine adjustment techniques to move a small distance

	// All angles are in degrees (because people are used to it!)
	void turnRight(float theta, float speed_scalar = 1); // Shortcut for turnAngle(|theta|). Can be as large as needed, must be > 0.
	void turnLeft(float theta, float speed_scalar = 1); // Shortcut for turnAngle(-|theta|). Can be as large as needed, must be > 0.
	void turnAngle(float theta, float speed_scalar = 1); // Will turn the vehicle a certain angle relative to its current position.
	void turnAngleClamped(float theta, float speed_scalar = 1); // As above, but constrains to +-180 degrees.
	
	bool isDriving() const; // Returns the "_driving" flag, for external use. Will be true when items are in queue.
private:
	L293D _motors;
	bool _driving = false; // Flag for if driving or not. Could be used externally to perform an interrupt routine.
	float _global_speed_scalar = 1; // Between 0 and 1 - scales the speed down from the max.
	float _wheel_dia = 1; // Wheel diameter - for distance tracking while travelling
	float _track = 1; // Distance between wheel centers - used for rotational calculations
	float _rpdc = 1; // Revs-per-Duty-cycle. Note that this is actually RPM per Duty Cycle.

	struct drive_instruction {
		unsigned long start_time; // Set by run when the instruction is called. Used to track when it should stop
		unsigned int duration; // Decides when to halt the instruction. If 0, instruction runs until this is changed
		byte left_speed;
		bool left_direction; // 1 is forward, 0 is backward
		byte right_speed;
		bool right_direction;
	};

	QueueList<drive_instruction> queue; // Dynamic linked list to hold drive instructions
	drive_instruction empty_instruction = {0, 0, 0, 0, 0, 0}; // Used in value checking and to stop the car

	bool boolsgn(float num); // Return true if positive or 0, false if negative
	short sgnbool(bool boolsgn); // Return 1 if true, or -1 if false
	drive_instruction newInstruction(float left_dist, float right_dist, float speed_scalar = 1); // Create and return instruction
	void addInstruction(float left_dist, float right_dist, float speed_scalar = 1);
	void executeInstruction(drive_instruction instruction) const; // Actually run the instruction
};

#endif