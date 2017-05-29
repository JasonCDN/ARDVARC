#include <ARDVARC_UTIL.h>
#include <DriveControl.h>
#include <ArmControl.h>

DriveControl driver;
ArmControl arm;

void setup() {
  Serial.begin(9600);
  arm.setServoPins(A1, A2, A3);
  driver.setSpeed(1); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(11);
  driver.setWheelScales(2, 1);
  driver.setBackScaling(0.1);
}

void loop() {
  arm.restPosition(); // Required, to keep servos steady
  driver.turnRight(90);
  driver.turnLeft(90);

  while (true) {
    driver.run();
    if (isTestMode()) {
      driver.stopAll();
    }
  }
}

