#include "DriveControl.h"
#include <L293dDriver.h>

void DriveControl::setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4)
{
	_motors.setLeft(en1, in1, in2);
	_motors.setRight(en2, in3, in4);
};

void DriveControl::setRevsPerDC(float rpdc)
{

}

void DriveControl::move(float dist, float speed_scalar = 1)
{
	speed_scalar = constrain(0, speed_scalar, 1);
	_motors.left(speed_scalar * 255);
	_motors.right(speed_scalar * 255);
}