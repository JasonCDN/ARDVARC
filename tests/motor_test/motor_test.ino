#include <Servo.h>
#include <Array.h>

Servo test1;
Servo test2;
Servo test3;

void setup() {
  test1.attach(8);
  test2.attach(9);
}

void loop() {
  float w[] = {2, 4, 7};
  for (int i = 0; i <= 180; ++i) {
    test1.write(i);
    test2.write(i);
    delay(10);
  }
  delay(1000);
}

