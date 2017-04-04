#include <Arduino.h>
#include "DriveControl.h"

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