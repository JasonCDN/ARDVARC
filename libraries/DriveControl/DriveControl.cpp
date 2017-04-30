#include "DriveControl.h"
#include <L293dDriver.h>
#include <QueueList.h>
#include <Coordinates.h>

#define PI 3.141592 // Needed for rotational calculations

/*

Parameter Setting

*/

void DriveControl::setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4)
{
	_motors.setLeft(en1, in1, in2);
	_motors.setRight(en2, in3, in4);
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
	_wheel_dia = max(wheel, 1E-4);
}

void DriveControl::setTrackWidth(float track)
{
	_track = max(track, 1E-4);
}

/*

Translational Motion

*/

void DriveControl::forward(float dist, float speed_scalar = 1)
{
	addInstruction(dist, dist, speed_scalar);
}

void DriveControl::backward(float dist, float speed_scalar = 1)
{
	addInstruction(-dist, -dist, speed_scalar);
}


/*

Rotational Motion

*/

// This is the fundamental turning function that transforms an angle into two
// distances (arc-lengths). Speed will be the max given by the speed_scalar
void DriveControl::turnAngle(float theta, float speed_scalar = 1)
{
	// Check that we have an angle
	if (theta < 0.25) {
		return;
	}


	// Calculate circumference of turning circle
	float turn_circ = 2 * PI * (_track/2);

	// Calculate the arc length given the subtending angle
	float arc_len = turn_circ * theta/360;

	// Determine direction of rotation
	byte director = 1; // This will be toggled to 1 or -1, depending on direction
	if (theta > 0) { // We are turning RIGHT, so left wheel forward
		director = 1;
	} else { // Then we are turning LEFT, so right wheel forward
		director = -1;
	}

	// Add the instruction to the queue. Right always opposes left at same speed.
	addInstruction(arc_len * director, -1 * arc_len * director, speed_scalar);

}

// Equivalent: a function alias
void DriveControl::turnRight(float theta, float speed_scalar = 1)
{
	turnAngle(theta, speed_scalar);
}

// Opposite: a function antialias?
void DriveControl::turnLeft(float theta, float speed_scalar = 1)
{
	turnAngle(-1 * theta, speed_scalar);
}

// Does what it says.
void DriveControl::turnAround(float speed_scalar = 1)
{
	turnAngle(180, speed_scalar);
}

// turnAngle, but with theta between -180 and 180 degrees
void DriveControl::turnAngleClamped(float theta, float speed_scalar = 1)
{
	float new_theta = constrain(-180, 180, theta);
	turnAngle(new_theta, speed_scalar);
}

/*

Compound (complex) motion

*/

void DriveControl::goToPoint(float x, float y, float speed_scalar = 1)
{
	goToPointSticky(x, y, speed_scalar);

	// Find angle to rotate back by
	Coordinates coords(x, y);
	if (abs(coords.getAngle()) > 0) {
		turnAngle(coords.getAngle(), speed_scalar);
	}
}

void DriveControl::goToPointSticky(float x, float y, float speed_scalar = 1)
{
	// Perform polar convserion
	Coordinates coords(x, y);

	// With polar attributes, now execute minimal instruction set:
	if (abs(coords.getAngle()) > 0) {
		turnAngle(coords.getAngle(), speed_scalar);
	}

	forward(coords.getR(), speed_scalar);
}

// Uses two arcs to move horizontally, and then corrects the vertical.
// This function uses a very specific algorithm, meant for SHORT movements.
// Longer paths won't work with this algorithm.
void DriveControl::nudge(float x, float y, float speed_scalar = 0.5)
{
	// Ensure the displacement is within doable boundaries
	if (abs(x) > _track || abs(y) > _track) {
		// Displacement is too large!
		return;
	}

	// Find the angle needed for proper displacement
	double angle = acos(1 - abs(x)/_track); // Angle needs to be in rads
	// Find arc length
	float wheel_dist = angle * (_track / 2); 
	// Find how much to correct vertically after our two arcs
	double vert_correct = abs(y) - abs(_track * sin(angle));


	// Determine drive directions vertically
	if (y > 0) { // Use forward motion
		// (things are already positive)
	} else if (y < 0) { // Use backwards motion
		wheel_dist = -1 * wheel_dist;
		vert_correct = -1 * vert_correct;
	} // If 0, then things are fine already

	// Perform double-arc horizontal "crawl" motion.
	// Determine the motion based on the sign of x
	if (x > 0) { // Left wheel first
		addInstruction(wheel_dist, 0, speed_scalar);
		addInstruction(0, wheel_dist, speed_scalar);
	} else if (x < 0) { // Right wheel first
		addInstruction(0, wheel_dist, speed_scalar);
		addInstruction(wheel_dist, 0, speed_scalar);
	} // If x == 0, do nothing.

	// Do vertical correction
	forward(vert_correct, speed_scalar);
}
 

/*

Utility Functions

*/

// PUBLIC

void DriveControl::stopAll()
{
	// Remove any remaining instructions.
	while (queue.count() > 0) {
		queue.pop(); 
	}
	executeInstruction(empty_instruction);
}


bool DriveControl::isDriving() const
{
	 // This is modified only by the run() method when adding/expiring instructions from the front of the queue.
	return _driving;
}

// PRIVATE 

// Return true if positive or 0, false if negative
bool DriveControl::boolsgn(float num)
{
	return num >= 0;
}

// Return 1 if true, or -1 if false
short DriveControl::sgnbool(bool boolsgn)
{
	return pow(-1,(1 + boolsgn));
}

// Need our own map function that can handle decimals
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/*

Instruction and Queue Logic:

* Creating instructions: newInstruction()
* Adding instructions to the queue: addInstruction()
* Keeping track of instructions in queue (and expiring them): run()
* Executing instructions: executeInstruction()

*/


// Takes required wheel distances, creates an instruction and pushes it onto the end of the queue.
void DriveControl::addInstruction(float left_dist, float right_dist, float speed_scalar = 1)
{
	queue.push(newInstruction(left_dist, right_dist, speed_scalar));
}


// Will make an instruction to move the wheels a given distance in a scaled time frame. The speed of each wheel
// is automatically calculated here. Duration is then calculated from the max speed we can handle.
drive_instruction DriveControl::newInstruction(float left_dist, float right_dist, float speed_scalar = 1)
{
	// The fastest speed that a wheel can possibly travel in this system.
	// Note that this should never equal 0, or we will have a problem. (this is in mm/s)
	float max_velocity = (_rpdc / 60) * _wheel_dia * PI; // Convert rpm to rps, then to a distance with dia*pi.

	if (max_velocity <= 0) {
		// Do nothing. If we get to here, something went wrong with setting parameters.
		return empty_instruction;
	}

	// Work out relative scales of the speeds. Be aware of division by 0.
	float left_speed = (left_dist > 0) ? left_dist / abs(left_dist) : 0;
	float right_speed = (right_dist > 0) ? right_dist / abs(left_dist) : 0;


	speed_scalar = constrain(speed_scalar, 0, 1) * _global_speed_scalar; // Must be within range for this to work, also modified by global settings

	// Normalize results to have a max at the max_speed, then map to -255 -> 255
	int left_analog = int(map(speed_scalar * left_speed * max_velocity, -max_velocity, max_velocity, -255, 255));
	int right_analog = int(map(speed_scalar * right_speed * max_velocity, -max_velocity, max_velocity, -255, 255));

	// Time needed to travel full distance of either wheel (remember that t is const)
	float time_needed;
	if (left_speed > 0) {
		time_needed = left_dist / left_speed; // In seconds
	} else if (right_speed > 0) {
		time_needed = right_dist / right_speed; // In seconds
	} else {
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
		drive_instruction * active_instruction = queue.peek();

		// Start the instruction (if necessary)
		if (active_instruction->start_time <= 0) {
			// Set start time to "right now"
			active_instruction->start_time = millis();
			// Execute the instruction (and set a flag for external use)
			executeInstruction(*active_instruction); // Make sure to de-reference pointer
			_driving = true;
		}

		// Check to see if current instruction has expired
		float time_passed = millis() - active_instruction->start_time;
		if (active_instruction->duration == 0 or time_passed > active_instruction->duration) {
			// If so, remove it from the queue and unset the _driving flag
			queue.pop();
			_driving = false;
			// That may have been the only instruction. If it was, stop the car
			if (queue.count() <= 0)	{
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
	while(queue.count() > 1) { // Keep first instruction 
		queue.pop();
	}
}