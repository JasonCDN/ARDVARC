#include <ARDVARC_UTIL.h>
#include <DriveControl.h>
#include <ArmControl.h>

DriveControl driver;
ArmControl arm;

void setup() {
  Serial.begin(9600);
  arm.setServoPins(A0, A1);
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(102);
  driver.setRevsPerDC(22.5);
}

void loop() {
  arm.restPosition();
  driver.turnRight(90);
  driver.turnLeft(90);

  while (true) {
    driver.run();
  }
}

