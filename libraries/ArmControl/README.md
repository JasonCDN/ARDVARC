# Arm Control

This document will describe how the ArmControl API works, like a tutorial. The
aim is to take you through how to use all the features of the ArmControl API to
make full use of the arm.

There are different levels of features. High-level features like `stowTarget()`
will coordinate all the servos to put the collected target in the stow bin. Low
level features, such as `setFirstArmAngle(theta, [speed])` will move the lower
part of the arm to the desired `theta`, at a given percentage of its total
possible `speed`.

Each method in the API is attached to the **ArmControl** class. Think of this
class sort of like a person, who can be given tasks (like "stow target") and
will carry them out for you - without you needing to think about the details.
This person's job is to control the mechanical arm.


# Intro

The ArmControl library is very simple - all it does is spin servos in a
specific way to make them do what you want. If provides easy-to-call functions
that are almost self explanatory.

There are also pre-defined motion sets that will move the arm such that it
performs a desired task.

### Setting up everything

Make sure, in your setup function, you call `arm.setServoPins(...)`. This is
the only set up required, but if you want you can change the default speed
from 0.5 to something else (between 0 and 1). See the function reference for
more detail.

### Making it go

The code is easy, I'll let it speak for itself:

```
#include <ArmControl.h>

ArmControl arm; // Define the ArmControl library handle

void setup() {
	arm.setServoPins(A0, A1); // Just some random default analog pins
}

void loop() {
	if (/* There's a target in front of us, ready to be picked up */) {
		arm.collectTarget(); // Boom. Done.
	}
}

```

There's also `restPosition()` and `readyPosition()`, which set the arm to a
pre-defined state. Normally, you would call `restPosition()` in setup, to make
sure everything is where it should be when the car starts driving around.

If you want more control, you can get and set the angle of each individual
servo. For the base of the arm (it's tilt controlling servo) you can set an
angle from 0 (fully retracted over ARDVARC) to 180 (fully extended to the
ground). Note that in real life it's not exactly 180 degrees, but it makes the
visualization easier.

Also, for the grip servo, you can set it from 0 (fully closed) to 90 (fully open).

# Function Reference
### void setServoPins(int base, int grip)

Set the pin numbers for the base servo and the grip servo. Use the `A<x>`
constants for the analog pins. (e.g. `A0` for Analog 0);

### void setServoSpeed(float speed) 

Takes a decimal (from 0 -> 1) and uses that to define the servo speed. When 0,
it will move as slowly as the definition allows (look at the MAX_STEP constant
in the library header file). When set to 1, it will move as quickly as the
definition allows.

### void setAngle(int angle) 

Set base servo (arm angle) from 0 (fully retracted) to 180 (fully extended).

### int getAngle()  

Reads the base servo's last-written angle and returns it. Note that this may
not reflect the actual angle of the servo - it's just remembering what angle
it was last told to go to (i.e. not actually reading the angle off the
hardware itself).

### void setGrip(int angle)  

Set gripping servo's angle from 0 (fully closed) to 90 (fully open).

### int getGrip()   

Reads the grip servo's last-written angle and returns it. Note that this may
not reflect the actual angle of the servo - it's just remembering what angle
it was last told to go to (i.e. not actually reading the angle off the
hardware itself).

### void collectTarget() 

Runs through a pre-defined motion set to pick up a target that is already
positioned in the jaws. 

Specifically, it will do this:

* Move it to ready position (if not already there)
* Close the jaws to a good angle to pick up most targets
  without stressing out the servos (too much - cause for concern here)
* Move the arm over the dump bin ready to drop the target
* Open the jaws
* Close the jaws

### void restPosition() 

Moves the arm to be "at rest". In other words, the gripper will be closed and
the arm will be completely above the vehicle (minimum footprint).

### void readyPosition()  

Moves the arm to be ready to pick up a target. This means the jaws are open
and down at ground level.
