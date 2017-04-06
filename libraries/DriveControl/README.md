# Drive Control

This document will describe how the DriveControl API works, like a tutorial.
The aim is to take you through how to use all the features of the DriveControl
API to make full use of ARDVARC's dual-independent-motor drive system.

There are different levels of features. High-level features like
`turnAround()` will coordinate the motors to rotate the car 180 degrees about
its Z axis, without changing its displacement. Low level features, such as
`moveForward(distance, [speed])` will make the car drive `distance` (in
centimeters) at a given percentage of its total speed.

Each method in the API is attached to the **DriveControl** class. Think of
this class sort of like a person, who can be given tasks (like "turn around")
and they will carry them out for you - without you needing to think about the
details. In this case, this person's job is to control how the vehicle moves.

## Introduction to how it works

Because actions done by DriveControl take considerable amounts of time to
complete (because driving takes time), the class is based on a queue system.
This means that when you call a function like `forward(100)`, all this does is
add an instruction to the "driving queue". Then, the rest of the code will
execute as normal, without waiting for the driving action to complete
(although you can make the code wait if you want - more on that later). 

The basis of an instruction is that all vehicle motions can be reduced to
running each motor at a specific speed for a period of time. Whenever the
speed of a motor needs to change, a new instruction is needed. This means that
while some methods like `forward(<dist>)` only add one instruction to the
queue, more advanced methods like `goToPoint(<x>, <y>)` might need to add 2 or
more instructions to the queue.

This instruction won't actually do anything. It will just sit there in the
queue until you call the most important method in DriveControl: `run()`. This
method will take instructions from the queue, start them running, monitor
their progress and terminate the instruction when its time is up. To let
DriveControl monitor what's going on, you should call `run()` often.

If you want to add multiple actions to the queue, to be executed in sequence,
then just call them one after the other. For example, the following code will
cause the car to move forward 100mm, backward 50mm, then turn 90 degrees to
the right. Then it will sit there and do nothing forever until you terminate
the program.

```cpp
DriveControl driving_man(65); // Set wheel diameter to 65mm

driving_man.setMotorPins(/* ... whatever */);

// Add actions to the queue
driving_man.forward(100);
driving_man.backward(50);
driving_man.turnRight();

while (true)
{
	driving_man.run() // IMPORTANT!
}
```

If you add an action to the queue, but you change your mind later, you can
call `clearQueue()` to immediately stop the vehicle and remove all driving
instructions from the list. At this time, adding individual instructions or
nuking it are the only ways to edit the queue.

#### Important Note about the queue

You should only have a max of 32 instructions in the queue at once - this is
due to restrictions of the Arduino platform. You could theoretically have
more, because the queue is dynamically sized, but you may run out of memory.



# Function reference

**TODO**

# Notes and warnings

A long driving instruction will be less accurate than a short one. This is
because the vehicle calculates how far it has travelled based on a provided
revs-per-duty-cycle value. Over time, the accuracy of this calculation will
decrease linearly.

Note that the maximum duration for an instruction is one minute. This can be
increased, but you shouldn't be running instructions for much longer than 10
seconds anyway (for the sake of accuracy).
