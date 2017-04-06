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

void DriveControl::wheel(Motor motor, float dist, float time)
{

};

void DriveControl::run()
{
	// Check instruction register.
	// If instruction in register, then
		// Check if duration expired:
			// If expired, shift instruction off the queue and check for other instructions as before
			// If not expired (or equal to 0), store in register and do nothing
				// break if not expired --- >

	// Pop first instruction from queue
	// Check duration. 
		// If > 0, set start time to millis()
		// If not > 0, remove from queue and discard. Loop.
	// Set motor speeds and directions
};

// Just loop through every item in the queue and remove it.
void DriveControl::clearQueue()
{
	for (int i = 0; i < queue.count(); ++i)
	{
		queue.pop();
	}
};

bool DriveControl::isDriving()
{
	return _driving;
};