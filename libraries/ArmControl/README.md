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
