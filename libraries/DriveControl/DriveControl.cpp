#include "DriveControl.h"
#include <L293dDriver.h>
#include <QueueList.h>

void DriveControl::setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4)
{
	_motors.setLeft(en1, in1, in2);
	_motors.setRight(en2, in3, in4);
};

void DriveControl::move(float dist, float speed_scalar = 1)
{
	speed_scalar = constrain(0, speed_scalar, 1);
	_motors.left(speed_scalar * 255);
	_motors.right(speed_scalar * 255);
};

void DriveControl::left(float dist, float time)
{

};

void DriveControl::right(float dist, float time)
{
	
};

// TODO: Put this in a utilities library
// Return true if positive or 0, false if negative
bool DriveControl::boolsgn(float num)
{
	return num >= 0;
}

// Will make an instruction to move the wheels a given distance. The speed of each wheel
// is automatically calculated here. Duration is calculated from the speed.
DriveControl::drive_instruction DriveControl::newInstruction(float left_dist, float right_dist, float speed_scalar = 1)
{
	// The fastest speed that a wheel can possibly travel in this system
	float max_velocity = _rpdc * 60 * _wheel_dia * 3.14159; // Convert rmp to rps, then to a distance with dia*pi.

	// Work out relative scales of the speeds
	float left_speed = left_dist / abs(left_dist);
	float right_speed = right_dist / abs(left_dist);

	// Scalar between "1" magnitude, and max_velocity (based on larger value)
	float scalar = max(left_speed, right_speed) / max_velocity;
	speed_scalar = constrain(speed_scalar, 0, 1); // Must be within range for this to work.

	// Normalize results to have a max at the max_speed, then map to -255 -> 255
	// TODO: Use lambdas here?
	int left_analog = int(map(speed_scalar * scalar * left_speed, -max_velocity, max_velocity, -255, 255));
	int right_analog = int(map(speed_scalar * scalar * right_speed, -max_velocity, max_velocity, -255, 255));

	// Time needed to travel full distance of either wheel (remember that t is const)
	float time_needed = left_dist / left_speed; // In seconds

	// Build instruction from previous calculations
	drive_instruction inst;
	inst.duration = time_needed * 1E3; // In milliseconds
	inst.left_speed = abs(left_analog);
	inst.left_direction = boolsgn(left_analog);
	inst.right_speed = abs(right_analog);
	inst.right_direction = boolsgn(left_analog);

	return inst;
};

void DriveControl::run()
{
	// Check first item on queue (if there is one)
	// If instruction, then
		// Check if duration expired:
			// If expired (or == 0, SE), shift instruction off the queue and check for other instructions as before
				// If no instructions remain
			// If not expired
				// break until expired --- >

	// Otherwise, Pop first instruction from queue (and set driving flag)
	// Check duration. 
		// If > 0, set start time to millis()
		// If not > 0, remove from queue and discard. Loop.
	// Set motor speeds and directions
};

// Just loop through every item in the queue and remove it.
void DriveControl::clearQueue()
{
	while(!queue.isEmpty())
	{
		queue.pop();
	}
};

bool DriveControl::isDriving()
{
	 // This is modified only by the run() method when adding/expiring instructions from the front of the queue.
	return _driving;
};