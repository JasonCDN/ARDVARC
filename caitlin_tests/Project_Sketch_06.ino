#include <SensorControl.h>
#include <ArmControl.h>
#include <DriveControl.h>

ArmControl arm;
SensorControl sensors;
DriveControl driver;

 const int len = 115; //Need to update
 const int width = 45; //Half distance between L and R ultrasonics

 const float pi = 3.14159;
 int x_position = 0;
 int y_position = 0;

bool target_detected = false;

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

void scan(int turn_towards, int dist_threshold){
  driver.stopAll();
  driver.turnAngle(turn_towards);
  driver.run();
  if(sensors.getFrontDistance() <= dist_threshold){
    driver.stopAll();
    //Target in front function (must change bool)
  }
}

void right_wall(){
   //Initialise orientation, vehicle should be in right corner facing left of parallel to right wall
  int distance = sensors.getRightDistance() + sensors.getLeftDistance() + 2*width;
  int rad_turn = acos(distance/1200);
  int deg_turn = radians_to_degrees(rad_turn);
  driver.turnRight(90 + deg_turn); //Should now be parallel to right and left walls, include testing algorithm 
  while(driver.isDriving()){
    driver.run();
  }  
  //Initialise position
  y_position = sensors.getRearDistance() + len;
  x_position = 2400 - sensors.getLeftDistance() - width; // Must be <= 2255 (>= 145mm from wall)

  //Cover right wall 
  driver.forward(900 - sensors.getRearDistance() - len);
  driver.run();
  if(sensors.getFrontDistance() <= 200){//Might have to change 200 based on Friday tests
    scan(-90, 225);
    if(driver.isDriving() != true && target_detected == false){
      scan(180, 225);
    } //Currently assuming target detected by scans
  }
  else if(sensors.getRightDistance() <= 2400 - x_position - width - 10){//Only reliable if vehicle is actually going parallel. TEST!
    int dist = sensors.getRightDistance() - 10;
    check_sides(75, dist);
    }
  else if(sensors.getLeftDistance() <= x_position - 2100 - width - 10){
    int dist = x_position - 2100 - width - 10;
    check_sides(-75, dist);
  }
target_detected = false;
}

void check_sides(int angle, int dist){
  driver.stopAll();
  driver.backward(40);//Assumed distance between ulatrasonic and centre of rotation, plus extra 0.5cm for reaction time. CHECK! 
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

  right_wall();
}      

void loop() {
  // put your main code here, to run repeatedly:

}
