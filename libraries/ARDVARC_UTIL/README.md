# ARDVARC Util
> For ARDVARC.
> Author: Jason Storey

This document describes inbuilt ARDVARC functionality that doesn't belong to
any particular control library set. It is useful for platform-specific features
that play into multiple (or extremely high-level) roles.

# Function reference

* <a href="#istestmode">isTestMode()</a> : Returns whether or not we are in test mode

<a id="istestmode"></a>
### bool isTestMode()

Simply call the function as you would any other: `isTestMode()`. It will
return `true` or `false`, depending on whether or not the "test mode" switch
is on.

Under the hood, the Arduino reads the voltage of the digital pin defined by
the `TEST_SWITCH_PIN` constant in the library header (default: D13). Then, the
function will return a boolean value (1 or 0) depending on if the pin reads
HIGH or LOW. By default, the pin should read LOW for test mode (to avoid
unwanted autonomous operation that could drain battery).
