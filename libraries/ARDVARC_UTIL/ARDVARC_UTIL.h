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

#include <L293dDriver.h> // Need the type definitions
#include <QueueList.h>

#define TEST_SWITCH 13

/*
	DO NOT CALL THIS
	This will initialize things for ardvarc, 
	but will automatically only be called once whenever 
	an ardvarc function is called.
*/
bool __ardvarc_initialized = false;
void _ardvarc_init() {
	if (!__ardvarc_initialized) {
		Serial.println("ARDVARC Initialized.");
		__ardvarc_initialized = true;
		pinMode(TEST_SWITCH, INPUT);
	}
}

// Returns true or false based on whether or not the 
// test mode switch is on or off.
bool isTestMode() {
	_ardvarc_init();
	return digitalRead(TEST_SWITCH) == LOW;
}

#endif