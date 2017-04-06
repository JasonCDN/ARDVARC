#include "DriveControl.h"
#include <L293dDriver.h>

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
	// Get first instruction from queue
	// Check if is_instruction
	// If isn't instruction, go to next instruction and repeat

		// If no other instructions, stop motors
		// If other instruction, shift it to the front of the queue and continue

	// Check start time. If > 0, that means has been started already:
		// Check if duration expired:
			// If expired, shift instruction off the queue and check for other instructions as before
			// If not expired (or equal to 0), return (do nothing)
				// break if not expired --- >

	// Check duration. 
		// If > 0, set start time to millis()
		// If not > 0, remove from queue.

	// Set motor speeds and directions


	// [1]: Need a function that will shift_to_next_instruction()
	// [2]: Need a function to remove_from_queue()
};

// Just sets everything to zero and executes the instruction
void DriveControl::clearQueue()
{
	drive_instruction halt = {0, 0, 0, 0, 0, 0};
	for (int i = 0; i < QUEUE_SIZE; ++i)
	{
		queue[i] = halt;
	}
	run();
};

bool DriveControl::isDriving()
{
	return _driving;
};