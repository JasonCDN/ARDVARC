# Arm Control

This document will describe how the ArmControl API works, like a tutorial. The
aim is to take you through how to use all the features of the ArmControl API
to make full use of the arm. Note also that the ArmControl API has the
functions required to handle the dump bin. This is a choice made because the
dump bin is effectively part of the collection mechanism, and it keeps the
same library having control of all the servos.

There are different levels of features. High-level features like
`collectTarget()` will coordinate all the servos to put the collected target
in the stow bin. Low level features, such as `setBase(theta)` will move the
lower part of the arm to the desired `theta`.

Each method in the API is attached to the **ArmControl** class. Think of this
class sort of like a person, who can be given tasks (like "collect target") and
will carry them out for you - without you needing to think about the details.
This person's job is to control the mechanical arm.

# WARNING

## Synchronous Library

Nothing else can be happening while the arm is in operation - no sensors, no
driving - nothing. This is because the nature of the arm library is
synchronous, so any code that you try to run concurrently will not run - it
will be forced to wait until the arm is finished moving.

You can, of course, move the arm in steps, do some code, then move the arm
again. Just remember that **you cannot do other things at the same time as
moving the arm**.

## Signal Noise

If you don't tell the servos to go somewhere, and then you run DC motors, then the servos will max themselves out of their own accord. This is because they are at a floating signal state and they don't care where they sit. All you need to do is make sure the servos initially know where they're supposed to be before turning any other motors on.

By default (i.e. based on the `SET_INITIAL` flag in the library header file) the `setServoPins(...)` function will automatically tell the servos to stick to their default configuration (i.e. fully closed).


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
### void setServoPins(int base, int grip, int dump)

Set the pin numbers for the base servo and the grip servo. Also pass in a pin
for the dump-bin servo. Use the `A<x>` constants for the analog pins. (e.g.
`A0` for Analog 0);

**Important Note:** This function actually does two things. It attaches the servos to the specified pins, and will then proceed to put them in their default configuration positions *at maximum speed*. This will usually take about a second or so.

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

Reads the grip servo's last-written angle and returns it. See warnings for `getAngle()`.

### void setDump(int angle)  

Set dumping servo's angle from 0 (fully down) to 90 (fully up).

### int getDump()   

Reads the dump servo's last-written angle and returns it. See warnings for `getAngle()`.

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

### void dumpTargets() 

Moves the dump servo in such a manner as to dump out the targets. Note that this function will also make sure that the gripper arm is out of the way before trying to dump the targets, to avoid a self collision.