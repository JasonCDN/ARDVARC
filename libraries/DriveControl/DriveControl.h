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
	* Path following (control-vertex spline tracking)
	* Turning on the spot
	* Fine position adjustment
	* Moving forwards and backwards (user specifies)

See the README for examples and a detailed reference.

Author: Jason Storey
License: GPLv3

*/

#ifndef drivecontrol_h
#define drivecontrol_h

/*

This is "Sir DriveControl". His job is to make the motors turn in such a precise
manner that the car ends up where you want it to. All you have to do is say
where and how fast!

See the README for specific details and examples.

*/
class DriveControl
{
public:
	DriveControl();
	~DriveControl();
	
};


/*

This class contains all the low-level code to get the voltages and currents
where you need them to get the motors going. The user shouldn't need to use
this class for anything - the class "DriveControl" uses "Motor" instances to
control the vehicle.

Because this class should not be needed by the user, it is not as heavily
documented as those that are. Additionally, all documentation is inline,
rather than in the README.

*/
class Motor
{
public:
	Motor();
}


#endif