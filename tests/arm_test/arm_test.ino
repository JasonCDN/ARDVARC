#include <ArmControl.h>

ArmControl arm;

void setup() {
  arm.setServoPins(A1, A0);
}

void loop() {
  arm.collectTarget();
  delay(2000);
}

