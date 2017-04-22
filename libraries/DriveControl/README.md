# Drive Control API
> For ARDVARC.
> Author: Jason Storey

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

#### Setting the Wheel spacing (track width)

Another required parameter (for turning and arcs) is the so-called *track
width*. This is the standard term for the the distance (in mm) that the wheels
are apart, center to center. You can set it with the `setTrackWidth(...)` method.

```cpp

DriveControl driver;

void setup() {
	// The distance is in mm
	driver.setTrackWidth(125);

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

Ideally, this value is a constant. (Hopefully that assumption won't be a
problem later - we might need to find some more data and this may become more
complex in the future).

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
cause the car to move forward 100 mm, then backward 50 mm, then turn 90 degrees
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

The DriveControl API has a function called `isDriving()`. This function will
return `true` if there is currently executing instructions on the queue, and
`false` if the queue is empty (and thus, we aren't moving anywhere). This
function is useful for quite a few things, like checking for motion, waiting
for code to complete or stopping a motion-sensitive sensor from functioning
until we stop.

Here's an example of how to "pause" the code until all the instructions on the
queue are finished:

```cpp
// ...
void loop() {
	driver.forward(100); // Instruction to move forward by 10 cm

	// When .isDriving() returns false, we have stopped and the loop will exit
	while(driver.isDriving()) {
		// Actually run and manage the instruction
		driver.run();
	}

	// Print a message to the serial monitor
	Serial.println("We have moved this country forward!");
}

```

## Notes and warnings

A long driving instruction will be less accurate than a short one. This is
because the vehicle calculates how far it has traveled based on a provided
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

* <a href="#drivecontrol">DriveControl()</a> : The API constructor
* <a href="#setmotorpins">setMotorPins(en1, in1, in2, en2, in3, in4)</a> : Set up motor pin numbers
* <a href="#settrackwidth">setTrackWidth(track)</a> : Set how far apart the wheels are (in mm)
* <a href="#setwheeldiameter">setWheelDiameter(wheel_dia)</a> : Set wheel diameter (in mm)
* <a href="#setrevsperdc">setRevsPerDC(rpdc)</a> : Set a positive speed multiplier for the wheels at full power
* <a href="#setspeed">setSpeed(speed)</a> : Set a global (overlaid) speed multiplier.

* <a href="#run">run()</a> : Run and maintain the instruction queue
* <a href="#clearqueue">clearQueue()</a> : Remove all instructions from the queue
* <a href="#stopall">stopAll()</a> : Clear the queue and turn off the motors

* <a href="#forward">forward(dist, speed_scalar = 1)</a> : Move forward a given distance (in mm). Optional speed.
* <a href="#backward">backward(dist, speed_scalar = 1)</a> : Move backward a given distance (in mm). Optional speed.

* <a href="#gotopoint">goToPoint(x, y, speed_scalar = 1)</a> : Drive to a certain point relative to current location
* <a href="#gotopointsticky">goToPointSticky(x, y, speed_scalar = 1)</a> : Drive to a certain point relative to current location
* <a href="#nudge">nudge(x, y, speed_scalar = 0.5)</a> : Nudge to a certain point (ideally close by). Optional speed.

* <a href="#turnright">turnRight(theta, speed_scalar = 1)</a> : Turn right a given angle, without constraint
* <a href="#turnleft">turnLeft(theta, speed_scalar = 1)</a> : Turn left a given angle, without constraint
* <a href="#turnAround">turnAround(speed_scalar = 1)</a> : Turn 180 degress clockwise
* <a href="#turnangle">turnAngle(theta, speed_scalar = 1)</a> : Turn an angle "theta" degrees on the spot. Negative is to the left.
* <a href="#turnangleclamped">turnAngleClamped(theta, speed_scalar = 1);</a> : Turn an angle "theta" degrees on the spot. Automatically constrains to principal angles (from -180 degrees to 180 degrees).


<a id="drivecontrol"></a>
### DriveControl()

This is the class constructor for the API. Note that you don't actually call
this function - it is automatically called when you declare the API variable
name (as below). When you declare a variable with this as the "type" (class)
then you can use the variable to access the API functions.

Usage example:

```cpp
DriveControl driver;
```
	
## Setting up

<a id="setmotorpins"></a>
### setMotorPins(int en1, int in1, int in2, int en2, int in3, int in4);

This function is crucial to the API, as it tells it where to find the motors.
More specifically, it tells the API what Arduino pins correspond to pins on
the L293D driver chip. The pin names are:

* **Enable 1**: When this one is `HIGH`, output 1 and 2 can be activated by inputs 1 and 2. Should be a PWM pin.
* **Input 1**: The state of this pin (`HIGH` or `LOW`) matches the state of the corresponding output (output 1 in this case)
* **Input 2**: As for Input 1.
* **Enable 2** : As for Enable 1, but enables the second motor (output 3 and 4).
* **Input 3**: As for Input 1.
* **Input 4**: As for Input 1.

It is expected that the motor tied to **Enable 1** is the left-most motor,
while the motor tied to **Enable 2** is the one on the right. If you mix these
around, then all the motion will be mirrored!

Note that it's important that the Enable pins are a digital PWM pin. If they
were not, then the DriveControl API couldn't manage the speed of the motors -
they would be running at full tilt all the time!

Usage example:

```cpp
void setup() {
	driver.setMotorPins(7, 8, 9, 10, 11, 12);
}
```

<a id="setwheeldiameter"></a>
### setWheelDiameter(float wheel_dia); 

This function tells the DriveControl API the diameter of the drive wheels on
the vehicle. Note that the units are **millimeters**. Because this API is
written for two motors, we assume that all the drive wheels are the same size.

This value is very important for estimating the distance traveled in a certain
amount of time. By default, it is set to `1`, which is obviously too small. If
you're having trouble with your car going way to far, make sure the diameter
is set properly. Measure from the tires, not the wheel hub.

```cpp
void setup() {
	// code for driver.setMotorPins(...), etc.
	driver.setWheelDiameter(65);
}
```


<a id="settrackwidth"></a>
###	void setTrackWidth(float track); 

Tell DriveControl how far apart the drive wheels are (in mm). This value
should be measured from the center of each wheel. To do this, measure the
thickness of one wheel (assuming both wheels are the same thickness!) and
measure the distance between the outside faces of each wheel. Finally,
subtract 1/2 of the wheel width from each end. You end up with the distance
between centers, and hopefully more accurate than just eyeballing it.

*General interest:* the word "track" is a standard term used for the distance
between centers of wheels on the same rotational axis. The distance between
centers for the front and back wheels (on the same plane) is called the
"wheelbase".


<a id="setrevsperdc"></a>
### setRevsPerDC(float rpdc); 

This value is used to keep track of how far car has gone in a certain amount
of time. The value is the RPM of the wheels at full power. This number will
vary from vehicle to vehicle and should thus be determined experimentally.

Although the value is for wheels at full power, we assume that it scales
linearly down with speed. Clearly, this won't be true at ridiculously low
speeds, but it should hold for the vehicle's operating range.


```cpp
void setup() {
	// code for setting pins, wheel size etc.
	driver.setRevsPerDC(32.5); // Example value
}
```


<a id="setspeed"></a>
### setSpeed(float speed); 

Modifies the overall speed of the car, in everything it does. This is a global
speed scalar between 0 and 1, which can be used to dial back the power of the
motors. If you're worried about burning them out, or need to scale down the
effective voltage a little, then this function should help.


## Queue management

<a id="run"></a>
### run(); 

Whenever you add an instruction to the queue (see the introduction above if
you don't follow), then it doesn't actually do anything. You need to *execute*
the instruction somehow. That's what the `run()` function does.

Additionally, the `run()` function keeps track of how long an instruction has
run for and determines if it should be removed from the queue. 

Because the `run()` function has an important role in keeping time, it should
be called in the code quite often - every 100 ms or so. For this reason, please
try to avoid the `delay(...)` function, because it will increase the amount of
time between calls to `run()`, and could make the DriveControl API quite
inaccurate.

Usage example:

```cpp

void setup() {
	// Add two instructions to the queue
	driver.forward(100);
	driver.backward(100);
}

void loop() {
	driver.run() // Execute instructions and check for expiry
	delay(10); // Only 10 ms **in the main loop** -- small enough time that it should be fine. Trying not to overheat Arduino
}
```

<a id="clearqueue"></a>
### clearQueue();

If you have added many instructions to the queue, but then decide for some
reason that you don't want them anymore, just call this function to remove all
the instructions from the queue. Note that this **will not remove the first
instruction**, so the vehicle can finish up what it's doing. Then it will
stop.

<a id="stopall"></a>
### stopAll(); 

This function is like a supercharged `clearQueue()`. It will remove every
single instruction, *and* issue a command to stop the motors. The `stopAll()`
function will interrupt all driving actions and stop the car as quickly as it
can.

## Simple motion

<a id="forward"></a>
### forward(float dist, float speed_scalar = 1);

This is hopefully easy to understand. When you call this function, it will add
an instruction to the queue to move the car forward by `dist` (in mm). The
optional speed scalar can make the car do it slower, just this once.

<a id="backward"></a>
### backward(float dist, float speed_scalar = 1);

This function is exactly the same as `forward(...)`, but the car moves in
reverse instead.

## Relative motion

<a id="gotopoint"></a>
### goToPoint(float x, float y, float speed_scalar = 1);

If you need to go somewhere relative to the vehicle's current position, then
you need to "go to a point". That's what this function does. Pass in an `x`
and `y` (in mm) and the vehicle will try its best to move there, relative to
its current location. Try to keep the distances short, because we can only
estimate how far the car has traveled.

This function uses three instructions. 

For example, if you call

```
driver.goToPoint(100, -50);
```

then the car will calculate where that point is, relative to the car, then rotate towards it (in this case, about 120 degrees to the right), travel the distance (about 112 mm) and undo the rotation it made, so it's facing the same direction before and after the turn.

<a id="gotopointsticky"></a>
### goToPointSticky(float x, float y, float speed_scalar = 1);

This function is identical to the `goToPoint(...)` function, but it won't undo
it's initial rotation. As such, it only uses two instructions, but it means
that you can expect to be facing the direction that you were last traveling.
This may be useful, or not - it depends on your search algorithm.

<a id="nudge"></a>
### nudge(float x, float y, float speed_scalar = 0.5);

When you need to move very slightly one way or another, standard movement
algorithms don't really work - because the rest of the car is in the way. That
is the time you need the `nudge(...)` function.

By default, it will move at half of full speed, which should be slow enough to
keep things accurate. The provided `x` and `y` can be positive or negative,
and are relative to the vehicle's current position. Both `x` and `y` are in
millimeters.

Note that a nudge will use up to 3 instructions on the queue.

## Turning

<a id="turnright"></a>
### turnRight(float theta, float speed_scalar = 1);s

Given a positive angle (in degrees), rotate the car that much to the right
(clockwise, when viewed from the top) relative to its current position.

Note that the angle can be pretty large (obviously not arbitrarily -- we do
have memory limits). This means that you can make the car spin on the spot as
many times as you need by just passing in `x * 360`, where `x` is the number
of complete rotations you want.

Note that, because distance is an estimation, it probably won't get this
perfect every time.

<a id="turnleft"></a>
### turnLeft(float theta, float speed_scalar = 1);

As for `turnRight(...)`, but it goes the other way.

<a id="turnAround"></a>
### turnAround(float speed_scalar = 1);

Turns the vehicle 180 degrees clockwise. Use `turnAngle(-180)` if you wanted
anti-clockwise (no pleasing some people...).

<a id="turnangle"></a>
### turnAngle(float theta, float speed_scalar = 1);

This is like the `turnRight(...)` and `turnLeft(...)` functions, but it takes
a positive or negative angle. Positive mean "to the right" and negative means
"to the left". This is a useful function if you have some sort of internal
rotation mapping that needs negative angles.

<a id="turnangleclamped"></a>
### turnAngleClamped(float theta, float speed_scalar = 1);

This is functionally equivalent to `turnAngle(...)`, but it will constrain
`theta` to be between -180 and +180 degrees.


## Other

###	bool isDriving() const;

Returns `true` if there are currently instructions being executed from the
queue. Will return `false` otherwise.

