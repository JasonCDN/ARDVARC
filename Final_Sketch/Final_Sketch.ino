/*
 * 1. collect 
 * 2. forward_scan
 * 3. magnet_scan (need two?)
 * 4. set path_follow
 * 5. Upper and lower level
 * 6. Comments and constants
 * 7. Test for 10 targets
 */

#include <SensorControl.h>
#include <ArmControl.h>
#include <DriveControl.h>
#include <ARDVARC_UTIL.h>

ArmControl arm;
SensorControl sensors;
DriveControl driver;

//Used to store millis(), to see time elapsed
unsigned long comparison;

//Determines how the vehicle will react, based on position
bool lower_level = true;

//Number of targets currently holding
int targets = 0; // Hopefully not for long :0

//True if 10 targets collected and returned
bool miracle = false;

//Distance between ultrasonics through vehicle centre lines
const int width = 100;
const int len = 160;

//Duh...
const float pi = 3.14159;

//Distances to the walls, valid when update_distances() is called
int dist_to_right;
int dist_to_left;
int dist_to_rear;

//Same as dist_to_right, but constant once it's initialised. Used to return to start.
int follow_path;

void forward_scan(){
  
}

void magnet_scan(){
  
}

int collect(int orientation, int distance){
  driver.turnAngle(orientation);
  int turn_back = -orientation;
  driver.forward(distance);
  if(sensors.isMaginRange() && sensors.isMagValid()){
    //arm.collectTarget();
    targets += targets;
    //arm.restPosition();
  }
  driver.backward(distance); //Back you go
  driver.turnAngle(turn_back)
  check_return();
  if(miracle != true){
    driver.forward(2000);
    driver.run();
  }
}

void check_return(){
  if(lower_level && (millis() >= 270000 || targets == 10)){
    //Checks if vehicle needs to return
    //U: allows 30 seconds to return
    driver.stopAll();
    driver.turnAngle(180);
    homeward_bound();
   }
   else if(lower_level != true && (millis() >= 240000 || targets == 10)){
    driver.stopAll();
    driver.turnAngle(180);
    while(sensors.getFrontDistance() < follow_path){
      driver.forward(20);
      control();
    }
    driver.turnAngle(-90);//U: when vehicle turns correctly, will change to 90
    homeward_bound();
   }
}

void pre_test(){
  /*
   * Demonstrates functionality for the pretest.
   */
  driver.forward(50);
  driver.backward(20);
  driver.turnAngle(20);
  driver.turnAngle(-20);
  control(); //execute and clear queue
  int comparison = millis();//Used to end while statement below
  while(millis() - comparison < 10000){
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
  Serial.print("End test");
  //arm.collectTarget();
  //arm.restPosition();  
}

void straighten(){
  /*
   * Takes sensor input from left and right walls and uses it to align itself parallel with the walls.
   */
  int distance = sensors.getRightDistance() + sensors.getLeftDistance() + width;
  float rad_turn = acos(1200.0/distance);
  int deg_turn = radians_to_degrees(rad_turn);
  Serial.println(deg_turn);
  //Actually turns right :)
  driver.turnLeft(deg_turn); //Should now be parallel to right and left walls
  control();
}

float radians_to_degrees(float radians1){
  float degrees1 = radians1 * (180 / pi);
  return degrees1;
}

void update_sides(){//U: account for all levels and orientations
  /*
   * Updates distances. Obviously.
   * Parameters (int check): checks if vehicle has just initialised. If yes, 1. If not, 0.
   */
  dist_to_right = sensors.getRightDistance();
  dist_to_left = sensors.getLeftDistance();
}

void homeward_bound(){
   while(sensors.isFloorStart() != true){
      driver.forward(100);
      control();
    }
    driver.forward(100);
    control();
    miracle = true;// Made it back!!!
    //Yipee!!!
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

void setup() {
  //General setup
  Serial.begin(9600); 
  sensors.setSensorPins(10, 11, 8, 9, 12);
  //arm.setServoPins(A0, A1);
  driver.setSpeed(0.9); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(14);
  driver.setBackScaling(1);

  //Reset arm position and initialise LED
  //arm.restPosition();
  pinMode(13, OUTPUT);

//  if(isTestMode()){
//    pre_test();
//  }
//  else{
//    straighten(); //T: use only if necessary.
//    //Stores the left, right and rear distances at the point before initial movement
//    update_sides();
//    driver.forward(2000);
//    driver.run();
//  }
}

void loop() {
  // Only consider first level

  update_sides();
  
  while(driver.isDriving()){
    if(sensors.getRightDistance() - dist_to_right >= 20){
      driver.stopAll();
      int orientation = -90; //U: when vehicle turns correctly, will change to 90
      int distance = sensors.getRightDistance() - 60;
      collecting = true;
      collect(orientation, distance);
    }
    else if(sensors.getLeftDistance() - dist_to_left >= 20){
      driver.stopAll();
      int distance = sensors.getLeftDistance() - 60;
      driver.backward(50);
      control();
      int orientation = 90;//U: when vehicle turns correctly, will change to -90
      collecting = true;
      collect(orientation, distance);
    }
    else if(sensors.getFrontDistance() <= 150 || (sensors.isMagInRange() && sensors.isMagValid())){
      driver.stopAll();
      forward_scan();
    }
   else if(sensors.isMagInRange() && sensors.isMagValid()){
    driver.stopAll();
    magnet_scan();
   }
   else if(sensors.getRearDistance() >= 1950 - len){
    //U: more reliable way to check rear distance
    //Should be approximately 5cm from bottom of ramp
    driver.stopAll();
    driver.forward(558);//U: what about when there is a target right at the top of the ramp?
    driver.turnAngle(90);
    driver.forward(2000);
    driver.run();
   }
   else if(millis() > 230000){
    check_return();
   }
   else{
    update_sides();
    check_return();
   }
  }   
}
