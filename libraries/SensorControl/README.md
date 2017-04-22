# Sensor Control
> For ARDVARC.
> Author: Jason Storey

This document will describe how the SensorControl API works, like a tutorial.
The aim is to take you through how to use all the features of the SensorControl
API to make full use of the arm.

The API aims to take as much of the hassle out of using the sensors as
possible. As such, it provides many high-level features to quickly get access
to the data you need as fast as possible. Because this API is written
expressly for the ARDVARC hardware platform, you will come across API methods
(like `setSensorPins(...)`) with design-specific parameters. Keep this in mind
if porting the code to other projects.

Each method in the API is attached to the **SensorControl** class. See below
for a function reference.

## Setting up the sensors

There is a few peices of information that SensorControl needs before you can
start using sensors. The sensor array consists of four ultrasonic sensors
(three at the front, one at the rear), one darkness sensor and one 3-axis
magnetic sensor. The pins for all of these are set up in one
`setSensorPins(...)` function. The pins of the 3-axis magnetic sensor are
already known and therefore don't need to be set up (because the sensor uses
the I2C bus).

The only other required information is the spacing between the front facing
ultrasonic sensors. Ideally, the spacing will be the same between all of
them, but things *can* go wrong, so you can pass in two spacing values if
needed.

Below is an example of how one might set up the sensors:

```cpp

SensorControl sensors;

void setup() {
	// Ultrasonics 1,2,3 at front, 4 at back, 5 for the line tracker.
	sensors.setSensorPins(7, 8, 9, 10, 11);

	// Note that we don't need to set pins for the magnetic sensor, because it runs from I2C


	// 20mm between both closest pairs of front ultrasonic sensor boards (between centers!)
	sensors.setSonarSpacing(20);
	// If spacing was off, you could do something like --> sensors.setSonarSpacing(19, 23)
}

```

## Getting some data out

There's a lot to explain, and many different ways to get data from the sensors.
Remember that the SensorControl class is the one-stop-shop for all the sensor
data that the vehicle has available.

I'll briefly go through each section of data sources, but I would really
recommend the function reference to give you a better idea of what's possible --
not everything is included in the tutorials below.

### Distance data

The ultrasonic sensors are what provide distance data. For the singular sensor
on the back, there's virtually only one way you can interface with it:
`getDistanceRear()`. This function returns a noise-filtered distance reading (in
mm) to whatever surface is in line-of-sight at the back of the vehicle. 

Where things actually get exiting is the three sensor array on the front of the
vehicle. There are a few functions you can call, but I'll go over the two that
are most useful: `getWallDistance()` and `getWallAngle()`.

The first (`getWallDistance()`) returns the shortest measured distance to the
wall. This way, you have a better estimate of how far forward you can actually
drive. The `getWallAngle()` function gives you an offset angle to the normal of
the wall (see diagram below). With the combination of these two datapoints, it
becomes a fair bit easier to self-locate in an unknown environment. No where
near as good as a 360 camera, but much better than just a distance reading.

![Depiction of offset angle](url)

To get an idea of how one would implement these data readings, here's an example
snippet which positions the vehicle to face a wall and stay 100 mm from it - no
matter where it starts in relation to the wall it chooses. Here we assume that
the vehicle is in an irregular box with flat walls, such that the sensors always
return valid readings:

```cpp

SensorControl sensors;

void setup() { 
	// Blah blah blah (pin setups etc.)

	// I want the robot within 2 degrees of facing the wall dead on.
	int angle_threshold = 2;

	// I want the robot to be 100 mm from the wall, give or take 3mm.
	int desired_dist = 100;
	int dist_threshold = 3;
}

void loop() {
	while (abs(sensors.getWallAngle()) > angle_threshold) {
		// turn the car by the value returned by getWallAngle()
		// (whatever code you need to do that)
	}

	while (abs(sensors.getWallDistance() - desired_dist) > dist_threshold) {
		// Move the car forward/backward by the difference in position
		// (whatever code you need to do that)
	}
}
```

While this library doesn't know *how* to move the vehicle for you (use
DriveControl for that), it can provide the necessary data to make informed
descions about *where* to move the vehicle.

## Floor-type data

That last section was a bit intense, so lets take a break with potentially the
most boring and dead-easy-to-understand sensor on the vehicle. The "line
tracker" sensor.

It's only job is to tell you if the floor is light, or dark. A better sensor
might be able to tell you *how* light, or *how* dark, but this cheapo sensor is
based on a transistor that's either on or off based on a threshold that you set
with a variable resistor. 

All that is to say, you can change what the sensor thinks is light and dark, but
it will still only give two outputs.

Although it only has two outputs, it still somehow manages to have 5 functions:

1. `isFloorStart()`: Returns true if we're in the starting area, false otherwise
2. `isFloorMain()`: Returns true if we're not in the starting area, false otherwise
3. `getFloorType()`: Returns `1` if we're in the start zone, and `2` if we're in the main area
4. `hasFloorChanged(interval)`: Returns true if the floor type has changed in the last `interval` milliseconds, false otherwise
4. `getTimeFloorLastChanged()`: Returns the number of milliseconds since the floor last changed

The first three are just different ways of getting the same data. Depending on
how you write your code, which one you use may change how easy it is to read.
The last two in the list just utilizes the internal sensor timer to tell you if
or how long ago the floor changed. You can see these functions in the function
reference - but you can figure out what they do just from their names.

Anyway, with that done, lets get on to the hardcore serious stuff.

## Magnetic field data

The data from the magnetic field sensor is a 3D vector. As such, there's quite a
bit of information we can deduce from it's current and past readings.


### Getting a heading

The sensor returns x, y and z values (positive or negative) depending on the
field strength. These values can be easily converted to a pitch and bearing
heading. In other words, you can find the x-y planar angle to the source of the
magnetic field, as well as the elevation. the magnitude of the field is also
easily calculated.

With these values, one can assumedly write an optimization algorithm to hone in
on the magnetic field:

```cpp

SensorControl sensors;

void setuo() {
	// pins, measurments, etc.
}

void loop() {
	while(abs(sensors.getMagBearing()) > 2) {
		// Then rotate the car to the angle returned.
		// Then, we'll (hopefully) be facing the magnet head-on
	}

	// Assume that -15 degrees from the xy plane is the elevation
	// required for the magnet to be directly in front of the vehicle
	while(sensors.getMagElevation() > -15) {
		// Move forward a bit (then the loop will run again)
	}
}

```

Note that there's something very important that's MISSING from the code above.
And that, is the `isMagValid()` function. This function will return true if none
of the axial data is maxed out. If you don't check that the data is relatively
ok with this function, then you may be operating on incorrect data because the
magnet is too strong. Then, you could get stuck in an infinite loop. An example
of using this function properly is:

```cpp

while(sensors.getMagElevation() > -15 && sensors.isMagValid()) {
	// ... do your code stuff
}

```

Still, you could get stuck in an infinite loop if the elevation condition never
breaks. For this reason, it's always a good idea to have a break condition
somewhere in the loop. You can code this break condtion with the
`deltaMagScore([interval])` function, which returns a float from `0` to `1`
based on "how much" the reading has changed in the past `interval` milliseconds.
`1` is where it has changed a lot, while `0` means it hasn't really changed at
all. Note that for memory reasons, there are restrictions on the effectiveness
of this function. See the reference for a full run down.

Note that the code above assumes that the magnetic sensor has infinite range
with the above code, when in actual fact, that code will only work if the
vehicle is relatively close to a magnet. Otherwise, the car will just forever
track the earth's magnetic field until it gets to one of the poles (or runs out
of battery).

See the function reference for more.

### Effective range

By default, the sensor returns x, y, and z values for the magnetic field based
on the field strength. If the field is too strong at a certain axis, it drops
the reading on that axis to 0 (due to limitations of the hardware). This means
that the sensor looses effectiveness if positioned too close to a magnet. Actual
testing reveals that as long as the sensor is more than 50 mm from a reasonably
strong magnet, it will return a reading. Use the `isMagValid()` function to
check that the data returned isn't maxed out on any of the axes.

One thing to note - the Earth's magnetic field *does* have an effect on the
readings. As such, the magnetic sensor can only be used to hone in on magnets
that are relatively close. There's a function called `isMagInRange()` which looks
at the magnitude of the field strength being read, and compares it to the known
strength of Earth's magnetic field. If the reading is too close to Earth's field
strength, then we assume that's what we're primarily getting and so the function
returns false. If the field is stronger in magnitude than the Earth's field, it
returns true and if it's weaker, then there's a good chance that the data is
unhelpful, so we return false.

Use the `isMagInRange()` and `isMagValid()` functions liberally to make sure
your descisions are informed by useable data.


# Function reference

Because the SensorControl class is essentially the amalgamation of three
different sensor types, the functions provided by the SensorControl are
categorized by the sensors they use. Functions with "Wall" or "Distance" in the
name are for the ultrasonic sensors, functions with "Floor" are for the line-
tracker, and functions with "Mag" are for the magnetic sensor.

### Table of Contents



#### <a href="linetrackingsensor">Line tracking sensor (*Floor*)</a>

* <a href="isfloorstart">isFloorStart()</a>: Returns true if the floor is dark
* <a href="isfloormain">isFloorMain()</a>: Returns true if the floor is light
* <a href="getfloortype">getFloorType()</a>: Returns 1 if the floor is dark, 2 if the floor is light.
* <a href="hasfloorchanged">hasFloorChanged(interval = 100)</a>: Returns true if the floor has changed in the given interval
* <a href="gettimefloorlastchanged">getTimeFloorLastChanged()</a>: Returns how many milliseconds ago the floor changed

------------------------------------------------------------------------------

## Ultrasonic sensors (*Wall* or *Distance*)




<a id="linetrackingsensor">
## Line tracking sensor (*Floor*)

<a id="isfloorstart">
### isFloorStart() 

Returns true if the floor is dark, false if the floor is, well, not dark. Good
for testing if we're in the starting area.

<a id="isfloormain">
### isFloorMain()

Returns true if the floor is *light*, false if the floor is dark. Useful for
testing if we're in the "main" area.

<a id="getfloortype">
### getFloorType()

Returns 1 if the floor is dark, 2 if the floor is light. Good for if you need
two logical conditions. Other than that, a bit redundant.

<a id="hasfloorchanged">
### hasFloorChanged(int interval = 100)

This *should* tell you if the floor has changed type in the given interval (in
milliseconds). However, it only updates its guess on whether or not the floor
has changed when you actually test the floor. If you want to use this
function, you need to be checking constantly (with *any* of the floor checking
functions). Even then, it won't be perfectly accurate.

<a id="gettimefloorlastchanged">
### getTimeFloorLastChanged()

This returns how may milliseconds ago the floor changed type. The same
warnings apply as for `hasFloorChanged(...)` -- make sure you have been
checking what the floor type is.



## 3-Axis Magnetic sensor (*Mag*)


