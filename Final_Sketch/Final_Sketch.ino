/*
 * To the poor unfortunate soul who has accidentally wandered into this tragic cesspit of broken dreams: Greetings.
 * What you have before you can only be described as a wasteland of certain uncertainty, consistent inconsistencies and ultimately, the
 * beginning of the last chapter in your life. The lucky ones last a mere two minutes in this heartbreaking maze of confusion. 
 * We're still searching for the unlucky ones.
 * Get out while you can.
 * One final warning: If you touch anything in here, you are doomed. You may try to run, change your name, move to Sweden. But ultimately,
 * you will end up exactly like poor Jimmy Cooper. Ever heard of him?
 * Exactly.
 */


/*
 * 1. collect _/
 * 2. forward_scan
 * 3. magnet_scan (need two?)
 * 4. set path_follow _/
 * 5. Upper and lower level
 * 6. Comments and constants
 * 7. Test for 10 targets _/
 * 8. Test for change in initial US, adjust so straight
 * 9. Going around non magnetic objects
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
bool lower_level = false; //should be true at start

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
    while(sensors.getFrontDistance() > follow_path){
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
      driver.stopAll();
      break;
    }
   }
   driver.stopAll();
}
}

int collect(int orientation, int distance){
  driver.turnAngle(orientation);
  int turn_back = -orientation;
  driver.forward(distance);
  control();
  if(sensors.isMagInRange() && sensors.isMagValid()){
    //arm.collectTarget();
    targets += targets;
    //arm.restPosition();
  }
  driver.backward(distance); //Back you go
  driver.turnAngle(turn_back);
  control();
  check_return();
  if(miracle != true){//U: Perhaps move to loop() - > using break statements now
    Serial.print("Done collecting");
    finished();
  }
}

void finished(){
  while(true){
  }
}

void movement(){ 
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
//    follow_path = sensors.getRightDistance() + 20;
//    on_path = true;
//    main();
//  }
  update_sides();
  Serial.print("Initial right: ");
  Serial.print(dist_to_right);
  Serial.print(" Initial left: ");
  Serial.print(dist_to_left);
}

void loop() {
  // Only consider first level
  while(sensors.getRightDistance() == 0 || sensors.getLeftDistance() == 0 || sensors.getFrontDistance() == 0 || sensors.getRearDistance() == 0){
    driver.forward(1);
    control();
  }
  
  update_sides();
  comparison = millis();
  driver.forward(100);
  driver.run();

  while(true){
    //While statement so comparison doesn't reset, also to help control so loop reset
    if(millis() - comparison >= 10{
      //Spaces out ultrasonic sampling and comparisons
      
      if(right_dist - sensors.getRightDistance() >= 20 && sensors.getRightDistance() != 0){//U: test for unstable structure
      //Compares recent sample with current sample, useful if  right ultrasonic isn't spazzing and returning 0       
        driver.stopAll();
        int distance = sensors.getRightDistance() - 140; //U: 140 must be tested
        //Distance between right ultrasonic and target, with space for collecting
        int orientation = -42; //U: will be 90
        collect(orientation, dist);
      }
      
      else if(left_dist - sensors.getleftDistance() >= 20 && sensors.getleftDistance() != 0){
        //Same as for right distance      
        driver.stopAll();
        int distance = sensors.getLeftDistance() - 140; //U: 140 must be tested
        int orientation = 97; //U: will be 90
        driver.backward(20);//U: to align magnetic sensor with target after turn
        collect(orientation, dist);
      }
      
      else if(sensors.getFrontDistance() <= 100 && sensors.getFrontDistance() != 0){
        driver.stopAll();
        //forward scan
        //count angle
        //if magnetic, collect
        //if not, go around
      }
      
      else if(sensors.isMagInRange() && sensors.isMagValid()){
        driver.stopAll();
        //magnet scan
      }
      
      else if(sensors.getRearDistance() >= 1950 - len && lower_level){
        //Vehicle is at base of ramp
        driver.stopAll();
        driver.forward(558)//U: reach top
        driver.turnAngle(97);//Will be -90
        lower_level = false;
      }
      
      else if(millis() > 230000){
        //Vehicle may need to return to start area
      }
      break;
  }
}
