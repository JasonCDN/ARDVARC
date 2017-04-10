#include "DriveControl.h"
#include <L293dDriver.h>
#include <QueueList.h>

void DriveControl::setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4)
{
	_motors.setLeft(en1, in1, in2);
	_motors.setRight(en2, in3, in4);
}

void DriveControl::move(float dist, float speed_scalar = 1)
{
	speed_scalar = constrain(0, speed_scalar, 1);
	_motors.left(speed_scalar * 255);
	_motors.right(speed_scalar * 255);
}

void DriveControl::left(float dist, float time)
{

}

void DriveControl::right(float dist, float time)
{
	
}

/*

Instruction Logic:

* Creating instructions: newInstruction()
* Adding instructions to the queue: addInstruction()
* Keeping track of instructions in queue (and expiring them): run()
* Executing instructions: executeInstruction()

*/

// TODO: Put these in a utilities library
// Return true if positive or 0, false if negative
bool DriveControl::boolsgn(float num)
{
	return num >= 0;
}

// Return 1 if true, or -1 if false
short DriveControl::sgnbool(bool boolsgn)
{
	return -1^(1 + boolsgn);
}

// Takes required wheel distances, creates an instruction and pushes it onto the end of the queue.
void DriveControl::addInstruction(float left_dist, float right_dist, float speed_scalar = 1)
{
	queue.push(newInstruction(left_dist, right_dist, speed_scalar));
}

void DriveControl::stopAll()
{
	clearQueue();
	executeInstruction(empty_instruction);
}

// Will make an instruction to move the wheels a given distance in a scaled time frame. The speed of each wheel
// is automatically calculated here. Duration is then calculated from the max speed we can handle.
DriveControl::drive_instruction DriveControl::newInstruction(float left_dist, float right_dist, float speed_scalar = 1)
{
	// The fastest speed that a wheel can possibly travel in this system.
	// Note that this should never equal 0, or we will have a problem.
	float max_velocity = _rpdc * 60 * _wheel_dia * 3.14159; // Convert rmp to rps, then to a distance with dia*pi.

	if (max_velocity <= 0) {
		// Do nothing. If we get to here, something went wrong with setting parameters.
		return empty_instruction;
	}

	// Work out relative scales of the speeds. Be aware of division by 0.
	float left_speed = (left_dist > 0) ? left_dist / abs(left_dist) : 0;
	float right_speed = (right_dist > 0) ? right_dist / abs(left_dist) : 0;

	// Scalar between "1" magnitude, and max_velocity (based on larger value)
	float scalar = max(left_speed, right_speed) / max_velocity;
	speed_scalar = constrain(speed_scalar, 0, 1) * _global_speed_scalar; // Must be within range for this to work.

	// Normalize results to have a max at the max_speed, then map to -255 -> 255
	int left_analog = int(map(speed_scalar * scalar * left_speed, -max_velocity, max_velocity, -255, 255));
	int right_analog = int(map(speed_scalar * scalar * right_speed, -max_velocity, max_velocity, -255, 255));

	// Time needed to travel full distance of either wheel (remember that t is const)
	float time_needed;
	if (left_speed > 0)
	{
		time_needed = left_dist / left_speed; // In seconds
	} 
	else if (right_speed > 0) 
	{
		time_needed = right_dist / right_speed; // In seconds
	}
	else
	{
		time_needed = 0; // If both are 0, we are stopping. No time needed.
	}

	// Build instruction from previous calculations
	drive_instruction inst;
	inst.duration = time_needed * 1E3; // In milliseconds
	inst.left_speed = abs(left_analog);
	inst.left_direction = boolsgn(left_analog);
	inst.right_speed = abs(right_analog);
	inst.right_direction = boolsgn(left_analog);

	return inst;
}

void DriveControl::executeInstruction(drive_instruction inst) const
{
	_motors.left(sgnbool(inst.left_direction) * inst.left_speed);
	_motors.right(sgnbool(inst.right_direction) * inst.right_speed);
}

void DriveControl::run()
{
	// Loop through items, only moving on to the next if the current one has expired
	while (queue.count() > 0)
	{
		// Get a pointer to the current instruction, so we can read/change it
		drive_instruction *active_instruction = &queue.peek();

		// Start the instruction (if necessary)
		if (active_instruction->start_time <= 0)
		{
			// Set start time to "right now"
			active_instruction->start_time = millis();
			// Execute the instruction (and set a flag for external use)
			executeInstruction(*active_instruction); // Make sure to de-reference pointer
			_driving = true;
		}

		// Check to see if current instruction has expired
		float time_passed = millis() - active_instruction->start_time;
		if (active_instruction->duration == 0 or time_passed > active_instruction->duration)
		{
			// If so, remove it from the queue and unset the _driving flag
			queue.pop();
			_driving = false;
			// That may have been the only instruction. If it was, stop the car
			if (queue.count() <= 0)
			{
				stopAll();
			}
			continue;
		} else {
			// If instruction not expired, then yield from loop. 
			// This function will be called again, and we can check then.
			break;
		}
	}
}

// Just loop through every item in the queue and remove it.
void DriveControl::clearQueue()
{
	while(!queue.isEmpty())
	{
		queue.pop();
	}
}

bool DriveControl::isDriving() const
{
	 // This is modified only by the run() method when adding/expiring instructions from the front of the queue.
	return _driving;
}

// Set the internal speed scalar to a value between 0 and 1.
void DriveControl::setSpeed(float speed)
{
	_global_speed_scalar = constrain(speed, 0, 1);
}

// Set the internal rpm (for 100% duty cycle) to a value. 
// This cannot be negative, but it can be zero (but then the whole car is useless).
void DriveControl::setRevsPerDC(float rpdc)
{
	_rpdc = max(rpdc, 0);
}

void DriveControl::setWheelDiameter(float wheel)
{
	_wheel_dia = max(wheel, 0);
}