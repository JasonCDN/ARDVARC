#include <ArmControl.h>

ArmControl arm;

void setup() {
  Serial.begin(9600);
  arm.setServoPins(A0, A1, A2);
  arm.setServoSpeed(0.5);
}

void loop() {
  arm.collectTarget();
  delay(5000);
}

