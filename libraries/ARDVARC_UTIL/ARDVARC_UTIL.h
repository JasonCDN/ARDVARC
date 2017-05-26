/* 

Library for utility functions relating to the ARDVARC platform.

Author: Jason Storey
License: GPLv3

*/

#ifndef ardvarc_util_h
#define ardvarc_util_h

// Pull in the Arduino standard libraries
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#define TEST_SWITCH_PIN A0
#define F_DEBUG true // A debug flag that logs Serial messages (if available) when true.

/*
	DO NOT CALL THIS
	This will initialize things for ardvarc, 
	but will automatically only be called once whenever 
	an ardvarc function is called.
*/

static bool __ardvarc_initialized = false;
inline void _ardvarc_init() {

	if (!__ardvarc_initialized) {
		Serial.println("ARDVARC Initialized.");
		__ardvarc_initialized = true;
		pinMode(TEST_SWITCH_PIN, INPUT);
	}
}

// Returns true or false based on whether or not the 
// test mode switch is on or off.
inline bool isTestMode() {

	_ardvarc_init();
	return digitalRead(TEST_SWITCH_PIN) == LOW;
}

// Need our own map function that can handle decimals
inline float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif