#include <ArmControl.h>

// ArmControl arm;

Servo arm1;
Servo arm2;
 
void setup() {
  Serial.begin(9600);
  //arm.setServoPins(A1, A2, A3);
  //arm.setServoSpeed(0.8);
  //arm1.attach(A1);
  arm2.attach(A2);
}

void loop() {
  //arm.setAngle(0);
  arm2.write(20);
  delay(5000);
}

