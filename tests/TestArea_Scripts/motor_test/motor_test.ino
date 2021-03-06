#include <ARDVARC_UTIL.h>
#include <DriveControl.h>
#include <ArmControl.h>

DriveControl driver;
ArmControl arm;

void setup() {
  Serial.begin(9600);
  arm.setServoPins(A0, A1, A2);
  driver.setSpeed(0.9); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(22.5);
  driver.setBackScaling(1);
}

void loop() {
  driver.forward(100);
  driver.turnRight(90);
  driver.turnLeft(90);
  driver.backward(100);

  while (true) {
    driver.run();
    if (!isTestMode()) {
      driver.stopAll();
    }
  }
}

