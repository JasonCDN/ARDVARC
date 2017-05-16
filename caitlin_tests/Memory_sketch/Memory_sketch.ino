
#include <BitArray.h>

BitArray myBits[6880];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int i = 0; i <8;i++)
  {
    Serial.print(myBits[i].value);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
