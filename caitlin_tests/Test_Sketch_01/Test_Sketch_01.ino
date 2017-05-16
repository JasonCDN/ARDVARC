#include <SensorControl.h>
#include <DriveControl.h>
#include <ArmControl.h>

//Define library handles
ArmControl arm;
SensorControl sensors;
DriveControl driver;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.setSensorPins(10, 11, 8, 9, 12);
  arm.setServoPins(A0, A1);
  driver.setSpeed(0.9); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(22.5);
  driver.setBackScaling(1);

  driver.forward(2450);
  driver.turnLeft(90);
  driver.forward(1100);
  driver.turnRight(90);
  driver.forward(900);
  driver.turnRight(90);
  driver.forward(1200);
  driver.turnRight(90);
  driver.forward(500);
  driver.turnLeft(90);
  driver.forward(400);
  driver.turnRight(90);
  driver.forward(1200);
}

void loop() {
  // put your main code here, to run repeatedly:
  driver.run();

}
