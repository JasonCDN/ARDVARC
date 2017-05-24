#include <ArmControl.h>

ArmControl arm;

void setup() {
  Serial.begin(9600);
  arm.setServoPins(A0, A1, A2);
  arm.setServoSpeed(0.8);
}

void loop() {
  arm.setGrip(0);
  delay(5000);
}

