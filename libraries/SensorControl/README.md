# Sensor Control
> For ARDVARC.
> Author: Jason Storey

**Note: Ultrasonic API has 100 ms blocking functions, to improve of ease of use. If a timer subroutine style is needed (i.e. like DriveControl's `run()` function) then contact API author.**

This document describes how the SensorControl API works, like a tutorial.
The aim is to take you through how to use all the features of the SensorControl
API to make full use of the ARDVARC sensor system.

The API aims to take as much of the hassle out of using the sensors as
possible. As such, it provides many high-level features to quickly get access
to the data you need. Because this API is written expressly for the ARDVARC
hardware platform, you will come across API methods (like
`setSensorPins(...)`) with design-specific parameters. Keep this in mind if
porting the code to other projects.

Each method in the API is attached to the **SensorControl** class. See below
for a function reference.

## Setting up the sensors

There is a few pieces of information that SensorControl needs before you can
start using sensors. The sensor array consists of four ultrasonic sensors
(one on each side of the car), one darkness sensor and one 3-axis
magnetic sensor. The pins for all of these are set up in one
`setSensorPins(...)` function. The pins of the 3-axis magnetic sensor are
already known and therefore don't need to be set up (because the sensor uses
the I2C bus).

Below is an example of how one might set up the sensors:

```cpp

SensorControl sensors; // Define the sensors handle (class instance)

void setup() {
	// Ultrasonics 1,2,3 at front, 4 at back, 5 for the line tracker.
	sensors.setSensorPins(7, 8, 9, 10, 11);

	// Note that we don't need to set pins for the magnetic sensor, because it runs from I2C
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

The ultrasonic sensors are what provide distance data. They require parallel
line of sight to whatever object is having its distance measured. Every
reading is actually multiple readings that are taken and averaged. This ensures
we have cleaner data, but the sample time is longer. (Contact library author
or modify yourself if the ping time is too long).

As there are four sensors, there are four data points. You can get the
distance to the nearest line-of-sight plane on a side by calling
`get<side>Dist()`, where `<side>` is one of *Front*, *Back*, *Left* or
*Right*. You can also use the `fillDistArray(array)` to fill up a four-element
array (which you pass into the function) with distance data. Note that this
function takes a while (~200 ms) to call.

To get an idea of how one would implement these data readings, here's an
example snippet which positions to stay 100 mm from a wall - no matter where
it starts in relation to the wall it chooses. Here we assume that the vehicle
is placed head on to the wall, such that the algorithms involved aren't too complex:

```cpp

SensorControl sensors;

void setup() { 
	// Blah blah blah (pin setups etc.)

	// I want the robot to be 100 mm from the wall, give or take 3mm.
	int desired_dist = 100;
	int dist_threshold = 3;
}

void loop() {
	int diff_pos;
	while (abs(sensors.getFrontDist() - desired_dist) > dist_threshold) {
		// Move the car forward/backward by the difference in position.
		// diff_pos will be negative if too close, indicating we need to move backwards
		diff_pos = sensors.getFrontDist() - desired_dist;

		// (whatever code you need to move the car)
	}
}
```

A quick thing to note is that sensor data isn't always accurate and may even
be useless in some cases. See the function reference for more detailed
information.

While this library doesn't know *how* to move the vehicle for you (use
DriveControl for that), it can provide the necessary data to make informed
descions about *where* (or *why*)to move the vehicle.

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

<a id="arraysandapi"></a>
# Using Arrays with the API

In Arduino-C, arrays are contiguous blocks of memory that are allocated (i.e.
reserved) and then written to. It is important to know the size of the array
before hand, so the appropriate amount of memory can be allocated. If dynamic
arrays are needed, usually special libraries are written to enable this.

With that introduction, here's the key point: you should not expect a function
to return an array. This is to save memory - every time you create a new
array, a whole new chunk of memory is reserved (and may not be released until
after you need it most). 

Instead, especially in embedded systems like the Arduino, functions expect to
be *passed* an array, which they will then modify. Usually the array is of an
expected size, because when it comes to raw arrays (i.e. memory blocks),
there's no real way to check the boundaries of the array.

That's where the "Array" library comes in. It's simply a class-template
wrapper around raw arrays that add boundary checking features and some useful
methods, directly to the array variable. 

You create an Array like this:

```cpp

#include <Array.h> // Include Array library -- make sure it's installed!

const int size = 4; // Always know your sizes
int raw[4] = {1,2,3,4}; // First create a raw array, with initialized values

// Here's the library magic:
Array<int> array = Array<int>(raw, size);

```

Whenever you see `Array<some_type>`, it means "This variable / expression is
an 'Array' class filled with elements of type 'some_type'.". It's a bit funky
looking, but it lets you declare what type of elements are members of the
array (to preserve operations).

Now, you can use:

```cpp

// Using array.size(), because it's awesome.
for (int i = 0; i < array.size(); ++i) {
	Serial.print(array[i]);
}

// Just because.
Serial.println(array.getAverage());

```

There are a few more methods you can call on the Array object. Have a look at
the `Array.h` file in the Array library folder for more detail in the code.

# Function reference

Because the SensorControl class is essentially the amalgamation of three
different sensor types, the functions provided by the SensorControl are
categorized by the sensors they use. Functions with "Wall" or "Distance" in the
name are for the ultrasonic sensors, functions with "Floor" are for the line-
tracker, and functions with "Mag" are for the magnetic sensor.

### Table of Contents



#### <a href="#linetrackingsensor">Line tracking sensor (*Floor*)</a>

* <a href="#isfloorstart">isFloorStart()</a>: Returns true if the floor is dark
* <a href="#isfloormain">isFloorMain()</a>: Returns true if the floor is light
* <a href="#getfloortype">getFloorType()</a>: Returns 1 if the floor is dark, 2 if the floor is light.
* <a href="#hasfloorchanged">hasFloorChanged(interval = 100)</a>: Returns true if the floor has changed in the given interval
* <a href="#gettimefloorlastchanged">getTimeFloorLastChanged()</a>: Returns how many milliseconds ago the floor changed

#### <a href="#ultrasonicsonars">Ultrasonic sonars (*Wall* or *Distance*)</a>

* <a href="#getwallangle">getWallAngle()</a> : Calculates the angle to the wall from the normal
* <a href="#getwalldistance">getWallDistance()</a> : Returns the closest distance measured from the front
* <a href="#getdistancecomponents">getDistanceComponents(Array<int> array)</a> : Returns a 3-element array of distance measurements (from left to right).
* <a href="#getreardistance">getRearDistance()</a> : Returns the distance to the closest rear obstacle (in line of sight of sensor).

#### <a href="#magneticsensor">Magnetic sensor (*Mag*)</a>

* <a href="#getmagcomponents">getMagComponents(Array<int> array);</a> :  Fills an x,y,z array of ints with magnetic field components
* <a href="#getmagbearing">getMagBearing();</a> : Returns xy plane (horizon plane) angle of displacement from pure forward
* <a href="#getmagelevation">getMagElevation();</a> : Returns angle of tile from horizon (negative if towards the ground)
* <a href="#getmagstrength">getMagStrength();</a> : Returns the strength of the magnetic field
* <a href="#deltamagscore">deltaMagScore(int interval = 100);</a> : Returns a value between 0 and 1 based on how much the reading has changed in recent times
* <a href="#ismagvalid">isMagValid();</a> : True if none of the axial components are maxed out
* <a href="#ismaginrange">isMagInRange();</a> : True if the magnitude of the signal is far enough from Earth's magnetic field to be considered a real signal


------------------------------------------------------------------------------


<a id="ultrasonicsonars"></a>
## Ultrasonic sensors (*Wall* or *Distance*)

<a id="getwallangle"></a>
### int getWallAngle()

Returns the argument angle to the wall's normal. The angle will be negative if
the car is angled to the right, and positive if angled to the left. All angles
are relative to the normal vector.

Note that the angle range isn't strictly `-90 -> 90` degrees. Rather, the
angle range depends on the spacing and the accuracy of the ultrasonic sensors.

Also take care with how often you call this function. It takes roughly 100 ms
to get get samples from the ultrasonics on the front of the car (speed of
sound and all that), so try not to use it while running timer sensitive tasks
(like driving, for instance).

<a id="getwalldistance"></a>
### int getWallDistance()

Returns the shortest distance to the wall - nuff said. (Contact API author if
average or middle distance needed.)

Like with previous functions, take care with how often you call *this* function.
It takes roughly 100 ms to get get samples from the ultrasonics on the front of
the car (speed of sound and all that), so try not to use it while running
timer sensitive tasks.


<a id="getdistancecomponents"></a>
### void getDistanceComponents(Array<int> array)

Internally fills a 3-element array with distances from each of the three
ultrasonic sensors on the front. See the ["Using Arrays with the API"](#arraysandapi) section.

Note that it takes roughly 100 ms to ping all three sensors, so try to keep
pinging to a minimum. (Contact author if the timing is giving you trouble,
then will check if can avoid pinging all three sensors.)

<a id="getreardistance"></a>
### int getRearDistance()

Returns the distance measured by the rear ultrasonic sensor.


------------------------------------------------------------------------------


<a id="linetrackingsensor"></a>
## Line tracking sensor (*Floor*)


<a id="isfloorstart"></a>
### bool isFloorStart() 

Returns true if the floor is dark, false if the floor is, well, not dark. Good
for testing if we're in the starting area.

<a id="isfloormain"></a>
### bool isFloorMain()

Returns true if the floor is *light*, false if the floor is dark. Useful for
testing if we're in the "main" area.

<a id="getfloortype"></a>
### short getFloorType()

Returns 1 if the floor is dark, 2 if the floor is light. Good for if you need
two logical conditions. Other than that, a bit redundant.

<a id="hasfloorchanged"></a>
### bool hasFloorChanged(int interval = 100)

This *should* tell you if the floor has changed type in the given interval (in
milliseconds). However, it only updates its guess on whether or not the floor
has changed when you actually test the floor. If you want to use this
function, you need to be checking constantly (with *any* of the floor checking
functions). Even then, it won't be perfectly accurate.

<a id="gettimefloorlastchanged"></a>
### int getTimeFloorLastChanged()

This returns how may milliseconds ago the floor changed type. The same
warnings apply as for `hasFloorChanged(...)` -- make sure you have been
checking what the floor type is.


------------------------------------------------------------------------------


<a id="magneticsensor"></a>
## 3-Axis Magnetic sensor (*Mag*)


<a id="getmagcomponents"></a>
### void getMagComponents(Array<float> array);

This is the coal-face of the magnetic sensor. Pass in a three-element array
and it will fill it with x, y, and z component data for the magnetic field
vector. These numbers are "floating point" (float) numbers, so they have a
decimal place. The components are the measured magnetic field strength in
milligauss. For your reference, here's a table of relatively common magnetic field
strengths (in milligauss):

| Magnetic Field Source^  | Strength (in mG)    |
|-------------------------|---------------------|
| Earth's magnetic field  | 500                 |
| Limit of the HMC5883L   | 8100                |
| Fridge Magnet           | 100000              |
| Strong permanent magnet | 12500000            |
| MRI Scanner             | 150000000-300000000 |

> ^ Note that these measurements are **at the surface** of the object. Field
> strength drops off with the inverse cube of the distance, so the sensor should
> still work at reasonable range (i.e. beyond ~3 cm of separation).

<a id="getmagbearing"></a>
### int getMagBearing();

Converts the x,y,z component data into a bearing. That is, it will return a
value (in degrees) between -180 and 180 that describes the bearing relative to
the sensor's horizontal plane.

Note that this *should* be accurate to within 2 degrees.

<a id="getmagelevation"></a>
### int getMagElevation();

Converts x,y,z component data into an elevation angle. This is relative to the
sensor's vertical front-facing plane.

<a id="getmagstrength"></a>
### float getMagStrength();

This is a simple function that returns the strength (magnitude) of the
magnetic field in milligauss.

<a id="deltamagscore"></a>
### float deltaMagScore(int interval = 100);

Gives you a rough value between 0 and 1 based on how much the reading has
changed in recent times

<a id="ismagvalid"></a>
### bool isMagValid();

Returns true if none of the axial magnetic components are maxed out

<a id="ismaginrange"></a>
### bool isMagInRange();

Returns true if the magnitude of the signal is far enough from Earth's
magnetic field to be considered a real signal from a local magnet.
