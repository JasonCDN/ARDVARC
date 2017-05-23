#include <SensorControl.h>
#include <ArmControl.h>
#include <DriveControl.h>

ArmControl arm;
SensorControl sensors;
DriveControl driver;

const int len = 230; //Distance between F and B ultrasonics - > Update
const int width = 100; //Distance between L and R ultrasonics -> Update

const float pi = 3.14159; 

int distance; // Used as a parameter for movement -> Distance to be travelled

bool target_detected = false;
int targets = 0;

/*
 * Assume total length = 220 mm
 * Assume total width = 90 mm
 * Assime turning point is 65 mm from front
 * Vehicle must be at least 145 mm from wall if travelling parallel, in order to turn 90 degrees away from wall
 */

float degrees_to_radians(float degrees1){
  float radians1 = degrees1 * (pi / 180);
  return radians1;
}  
float radians_to_degrees(float radians1){
  float degrees1 = radians1 * (180 / pi);
  return degrees1;
}

void control_movement(){
  while(driver.isDriving){
    driver.run();
  }
}

void straighten(){
  int distance = sensors.getRightDistance() + sensors.getLeftDistance() + width;
  int rad_turn = acos(300/distance);
  int deg_turn = radians_to_degrees(rad_turn);
  driver.turnRight(deg_turn); //Should now be parallel to right and left walls, include testing algorithm 
  driver.run();
  control_movement();
}

void scan(int turn_towards, int dist_threshold){
  driver.stopAll();
  driver.turnAngle(turn_towards);
  driver.run();
  if(sensors.getFrontDistance() <= dist_threshold){
    driver.stopAll();
    //Target in front function (must change bool)
  }
}

void side_detected(int turn_towards, approx_dist){
  driver.stopAll();
  driver.backward(65);//Test and edit, will change based on speed
  driver.turnAngle(turn_towards);
  driver.run();
  control_movement();
  driver.forward(approx_dist);
  /*
   * scan()? Test for magnetism?  
   * Adjust distance, test for magnetism, activate arm
   */ 
}

int movement(distance){
  /*Checks area to left and right for targets, and short distance area in front. If target detected, scan and stop when 
   * Paramters: distance(int) - distance to travel (exclusing distance covered to break off path and collect targets
   * Steps to write:
   *  1. Check 'scan' and 'check_sides' functions
   *  2. Update distance_covered -> must call fuction from within itself with (distance - distance_covered)
   *  3. Use 'blip' function instead of getDistance()
   *  4. Include use of magnetic sensor
   */
  int distance_covered = 0;
  driver.forward(distance);
  driver.run();
  
  if(sensors.getFrontDistance() <= 200){//Alter '200' and include magnetic sensor
    scan(-90, 225); //Alter '225'
    if(driver.isDriving() != true && target_detected == false){
      scan(180, 225);
    } //Currently assuming target detected by scans
  }
  else if(sensors.getRightBlipped() == 1){//Edit to only include time
    Array<int> out = Array<int>(2);
    int approx_dist = sensors.getRightBlipped(out)
    side_detected(90, approx_dist);
    }
  else if(sensors.getLeftBlipped() == 1){//Edit to only include time
    int approx_dist = sensors.getLeftBlipped(Array<int> out)[0])//Check if this is correct use
    side_detected(-90, approx_dist);
  }
}

void check_sides(int angle, int dist){//EDIT
  driver.stopAll();
  driver.backward(40);//Assumed distance between ulatrasonic and centre of rotation, plus extra 0.5cm for reaction time. CHECK, maybe increase so < angle < 90! 
  driver.turnAngle(angle);
    while(driver.isDriving()){
      driver.run();
    }
  if(angle < 0){
    scan(-180-angle, dist);
  }
  else{
    scan(180-angle, dist);
  }
}

void setup() {
  // Setup
  Serial.begin(9600); 
  sensors.setSensorPins(10, 11, 8, 9, 12);
  arm.setServoPins(A0, A1);
  driver.setSpeed(0.9); // Preserve motors
  driver.setMotorPins(3, 4, 2, 5, 6, 7);
  driver.setWheelDiameter(55);
  driver.setTrackWidth(105);
  driver.setRevsPerDC(22.5);
  driver.setBackScaling(1);

  delay(5000); //Allow time to remove cord
  driver.turnAngle(100);
  distance = (2000 - sensors.getRearDistance() - (len/2))/2; //Halfway through lower level
  movement(distance);

  //Reset position and loop back to right_wall(), until facing ramp and driver.getRearDistance() == (2000 - len - initial rear distance) (approximately)
}      

void loop() {
  // put your main code here, to run repeatedly:
  //If millis() >= 180 000 (3 mins) and target_detected == false, return to start. TEST!
  //If targets == 10, return to start.

}
