#include <SensorControl.h>
#include <ArmControl.h>
#include <DriveControl.h>
#include <ARDVARC_UTIL.h>

ArmControl arm;
SensorControl sensors;
DriveControl driver;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  sensors.setSensorPins(10, 11, 8, 9, 12);
  //arm.setServoPins(A0, A1);
  driver.setSpeed(0.9); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(14);
  driver.setBackScaling(1);

  //arm.restPosition();
  pinMode(13, OUTPUT);

  if(isTestMode()){
    Serial.println("True");
    pre_test();
  }
}

void pre_test(){
  /*
   * Demonstrates functionality for the pre_test.
   */
  driver.forward(100);
  driver.backward(1000);
  driver.turnAngle(90);
  driver.turnAngle(-90);
  control(); //execute and clear queue
  int comparison = millis();//Used to end while statement below
  while(millis() - comparison < 30000){
  //Demonstrate an LED turning on when target is within 100mm (10cm)
  //Ends after 30 seconds
    if(sensors.getFrontDistance() <= 100){
      digitalWrite(13, HIGH);
      Serial.println(sensors.getFrontDistance());
    }
    else{
      digitalWrite(13, LOW);
    }  
  }
  digitalWrite(13, LOW);
  delay(3000); //allow time to place target in front of vehicle
  //arm.collectTarget();
  //arm.restPosition();
  
}

void control(){
  /*
   * Executes the current driving queue, until empty. Then resets the queue.
   */
  while(true){
    driver.run();
    if(driver.isDriving() == false){
      break;
    }
  }
  driver.stopAll();
}

void loop() {
  // put your main code here, to run repeatedly

}
