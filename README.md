# Welcome to ARDVARC

ARDVARC (Autonomous Radioactive Debris Verification Acquisition and Retrieval
Coordinator) is an autonomous vehicle designed to collect radioactive waste.

This directory contains the code behind ARDVARC.

## Important!

If you want to build this code, then you will need to **change the Arduino IDE sketchbook path to the repository root** on your local hard drive. For example, if I clone the repository into `C:\Some\Path\ARDVARC`, then I will need to go into the Arduino IDE and change the sketchbook directory to match. You can do this by going to `File > Preferences` and changing `Sketchbook Location`. 

The reason for this, is that Arduino can only handle importing libraries (with `#include`) that are located in the `<Sketchbook>\libraries` folder. The lack of support for local libraries is a limitation of the Arduino compiler itself, and makes code sharing a bit more complex. Luckily, it can be easily worked around by following the above steps.

The ARDVARC code was developed on Arduino IDE 1.8.1