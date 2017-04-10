# Drive Control

This document will describe how the DriveControl API works, like a tutorial.
The aim is to take you through how to use all the features of the DriveControl
API to make full use of ARDVARC's dual-independent-motor drive system.

There are different levels of features. High-level features like
`turnAround()` will coordinate the motors to rotate the car 180 degrees about
its Z axis, without changing its displacement. Low level features, such as
`moveForward(distance, [speed])` will make the car drive `distance` (in
centimeters) at a given percentage of its maximum speed. 

> Note that unless you specify a global speed and/or specify an additional
once-off speed, the DriveControl API will do things as fast as the vehicle
physically allows it. Normally, that shouldn't be an issue.

Each method in the API is attached to the **DriveControl** class. Think of
this class sort of like a person, who can be given tasks (like "turn around")
and they will carry them out for you - without you needing to think about the
details. In this case, this person's job is to control how the vehicle moves.

## Setting up the parameters

Before using the DriveControl API, you need to hook a few things up in the
code.

#### Setting motor pins

So that DriveControl knows where the motors are, you need to tell it which
pins to use. You can do this with the function `setMotorPins(...)` as such:

```cpp

DriveControl driver;

void setup() {
	// Order: Enable1, Input1, Input2, Enable2, Input3, Input4
	// See reference below for more information
	driver.setMotorPins(3, 4, 5, 6, 7, 8);

	// ... More code
}

```

#### Setting the Wheel diameter

To allow DriveControl to calculate (very roughly) how far it has traveled, one
of the required parameters is the wheel diameter. To tell DriveControl how big
your wheels are, use the `setWheelDiameter(...)` function.

```cpp

DriveControl driver;

void setup() {
	// The diameter is in mm
	driver.setWheelDiameter(65);

	// ... More code (set pins, etc.)
}

```

#### Setting the RPM for full power (RPDC)

The second parameter for DriveControl to calculate roughly how far the vehicle
has gone is the "speed-per-voltage". This is called "RPM-per-duty-cycle"
(ARDVARC specific term) that tells DriveControl how far it can expect the
motors to take the car at full power for one minute. 

To be specific, this value is the revs-per-minute of the motors while the car
is running at full power. This value can be found experimentally, by running
the car at full speed for an amount of time and seeing how far it goes.

You can set the RPDC with the `setRevsPerDC(...)` function, as such:

```cpp

DriveControl driver;

void setup() {
	// Revs per minute at full power (full duty-cycle)
	driver.setRevsPerDC(31);

	// ... More code (set pins and wheel diameter)
}

```

## How to get things moving

#### Intro to the API

Because actions done by DriveControl take considerable amounts of time to
complete when compared to average processing speed, the API is based on a
queue system. Don't worry - you don't really need to know about how the queue
works, just what it means.

This means that when you call a function like `forward(100)`, all this does is
add an instruction to the "driving queue". Then, the rest of the code will
execute as normal, without waiting for the physical driving to happen. 

> You *can* make the code wait if you want - more on that later.

This instruction won't actually do anything. It will just sit there in the
queue until you call the most important method in DriveControl: `run()`. This
method will take instructions from the queue, start them running, monitor
their progress and terminate the instruction when its time is up. To let
DriveControl monitor what's going on, you should call `run()` often.

If you want to add multiple actions to the queue, to be executed in sequence,
then just call them one after the other. For example, the following code will
cause the car to move forward 100mm, then backward 50mm, then turn 90 degrees
to the right. After it's done all that, it will sit there and do nothing
forever until you terminate the program or add more instructions to the queue.


```cpp
DriveControl driver();

void setup() {
	driver.setMotorPins(/* ... whatever */);
	driver.setWheelDiameter(/* ... */);
	driver.setRevsPerDC(/* ... */);

	// Add actions to the queue
	driver.forward(100);
	driver.backward(50);
	driver.turnRight();
}


void loop() {
	driver.run() // IMPORTANT!
}
```

If you add an instruction to the queue, but you change your mind later, you
can call `clearQueue()`. This will remove all driving instructions from the
list and the car will just finish up whatever it's currently running. At this
time, adding individual instructions or nuking the queue are the only ways to
edit the sequence of instructions.

#### A Word on Memory

The basis of an instruction is that all vehicle motions can be reduced to
single steps of running each motor at a specific speed for a period of time.
Whenever the speed of a motor needs to change, a new instruction is needed.
This means that while some methods like `forward(<dist>)` only add one
instruction to the queue, more advanced methods like `goToPoint(<x>, <y>)`
might need to add 3 or more instructions to the queue.

All this is to say that you should keep track of what you're doing. Try to
restrict the number of instructions on the queue to a reasonable number. A
recommended maximum is 32 - this is due to restrictions of the Arduino
platform. You could theoretically have more, because the queue is dynamically
sized, but be warned that you may run out of memory.

#### Making the code wait

## Notes and warnings

A long driving instruction will be less accurate than a short one. This is
because the vehicle calculates how far it has travelled based on a provided
revs-per-duty-cycle value. Over time, the accuracy of this calculation will
decrease linearly.

Note that the maximum duration for an instruction is one minute. This can be
increased, but you shouldn't be running instructions for much longer than 10
seconds anyway (for the sake of accuracy).

## Getting things Going

That's all for the brief introduction to the API. If you want to learn more
about what the API can do (or you need some specific help with a function)
then the next step is the function reference below. Every single function that
the API makes available to you is documented there in detail.


# Function reference

* <a href="#drivecontrol">DriveControl(wheel, rpdc)</a> : The API constructor
* <a href="#setmotorpins">setMotorPins(en1, in1, in2, en2, in3, in4)</a> : Set up motor pin numbers
* <a href="#addinstruction">addInstruction(left_dist, right_dist, speed_scalar = 1)</a> : REMOVE THIS
* <a href="#run">run()</a> : Run and maintain the instruction queue
* <a href="#clearqueue">clearQueue()</a> : Remove all instructions from the queue
* <a href="#stopall">stopAll()</a> : Clear the queue and turn off the motors
* <a href="#forward">forward(dist, speed_scalar = 1)</a> : Move forward a given distance. Optional speed.
* <a href="#backward">backward(dist, speed_scalar = 1)</a> : Move backward a given distance. Optional speed.
* <a href="#nudge">nudge(x, y, speed_scalar = 0.5)</a> : Nudge to a certain point (ideally close by). Optional speed.
* <a href="#gotopoint">goToPoint(x, y)</a> : Drive to a certain point
* <a href="#setp2pmode">setP2PMode()</a> : REMOVE
* <a href="#setarcmode">setArcMode()</a> : REMOVE
* <a href="#turnright">turnRight()</a> : UPDATE 
* <a href="#turnleft">turnLeft()</a> : UPDATE
* <a href="#turnangle">turnAngle(theta)</a> : Turn an angle "theta" degrees on the spot. Negative is to the left.
* <a href="#setspeed">setSpeed(speed)</a> : Set a global (stacking) speed multiplier.
* <a href="#setrevsperdc">setRevsPerDC(rpdc)</a> : UPDATE
* <a href="#setwheeldiameter">setWheelDiameter(wheel_dia)</a> : UPDATE

<a id="drivecontrol"></a>
### DriveControl(float wheel, float rpdc = 1)
	
<a id="setmotorpins"></a>
### setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4); // Pins for the motors

<a id="addinstruction"></a>
### addInstruction(float left_dist, float right_dist, float speed_scalar = 1);

<a id="run"></a>
### run(); // This class runs on a queue system. This function must be called to progress the queue. See README.

<a id="clearqueue"></a>
### clearQueue(); // Remove all instructions from queue, finish up what we're doing.

<a id="stopall"></a>
### stopAll(); // Clears the queue and executes an instruction to stop all motors.

<a id="forward"></a>
### forward(float dist, float speed_scalar = 1); // Moves forwards a certain `dist` (in mm). Optional speed scalar.

<a id="backward"></a>
### backward(float dist, float speed_scalar = 1); // Moves backwards a certain `dist` (in mm).

<a id="nudge"></a>
### nudge(float x, float y, float speed_scalar = 0.5); // Uses fine adjustment movements to move a small distance

<a id="gotopoint"></a>
### goToPoint(float x, float y); // Pass in relative coordinates (in mm) to travel there.

<a id="setp2pmode"></a>
### setP2PMode(); // Sets point to point driving mode

<a id="setarcmode"></a>
### setArcMode(); // Sets arc driving mode

<a id="turnright"></a>
### turnRight(); // Shortcut for turnAngle(90)

<a id="turnleft"></a>
### turnLeft(); // Shortcut for turnAngle(-90)

<a id="turnangle"></a>
### turnAngle(float theta); // Will turn the vehicle a certain angle relative to its current position.

<a id="setspeed"></a>
### setSpeed(float speed) { _global_speed_scalar = speed; }; // Modifies the overall speed of the car (every motion is scaled by `speed`)

<a id="setrevsperdc"></a>
### setRevsPerDC(float rpdc) { _rpdc = rpdc; }; // Used to keep track of how far car has gone in a certain amount of time

<a id="setwheeldiameter"></a>
### setWheelDiameter(float wheel_dia); // This is in mm, and is used to keep track of travel distance

	bool isDriving() const; // Returns the "_driving" flag, for external use. Will be true when items are in queue.

