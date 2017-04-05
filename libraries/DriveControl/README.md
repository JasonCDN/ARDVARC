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
