#include <SensorControl.h>
#include <ArmControl.h>
#include <DriveControl.h>

ArmControl arm;
SensorControl sensors;
DriveControl driver;

/*
 * xCoordinate = x coordinate of vehicle's position (mm)
 * yCoordinate = y coordinate of vehicle's position (mm) 
 * orientation = direction of front of vehicle relative to starting position, in radians (0 = forward, 90 = right, 180 = back, 270 = left) 
 * !! use mod for left turns and multiple right turns !!
 */
 float x_position = 0;
 float y_position = 0;
 float orientation = 0;
 const float pi = 3.14159;
 float radian_orientation = orientation * (pi / 180);

/*
 * len = length from front (or back) to centre of vehicle, to be specified later
 * width = width from left (or right) to centre of vehicle, to be specified later
 * !!Alter through testing!!
 */

 const int len = 115;
 const int width = 45;


/* 
 *  distances[0] = dist from centre of vehicle to upper either bottom of ramp (lower_level), top of ramp (ramp_level) or upper horizontal wall (upper_level)
 *  distances[1] = dist from centre of vehicle to right wall
 *  distances[2] = dist from centre of vehicle to either lower horizontal wall (lower_level), bottom of ramp (ramp_level) or top of ramp (upper_level)
 *  distances[3] = dist from centre of vehicle to left wall
 */
int distances[5];

float overall_angle(int turning_angle){
  //Calculates the overall change in angle of the centre of the vehicle, used for x_rotation and y_rotation
  //Parameters : front_angle -> the angle by which the vehicle has turned, in degrees (-360 <= front_angle <= 360)
  float positive_angle = turning_angle % 360; // 0 <= positive_turning_angle < 360
  float radian_angle = degrees_to_radians(positive_angle); // 0 <= radian_angle <= 2pi
  float relative_angle;
  if(radian_angle == 0){
    relative_angle = 0;
  }
  else if(radian_angle <= pi){
    relative_angle = (3*pi + radian_angle)/2;
  }
  else{
    relative_angle = -(pi - radian_angle)/2;
  }
  float overall_angle = radian_orientation + relative_angle;
  orientation += turning_angle;
  Serial.println(relative_angle);
  return overall_angle; // given in radians
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
//  sensors.setSensorPins(10, 11, 8, 9, 12);
//  arm.setServoPins(A0, A1);
//  driver.setSpeed(0.9); // Preserve motors
//  driver.setMotorPins(3, 4, 2, 5, 6, 7);
//  driver.setWheelDiameter(55);
//  driver.setTrackWidth(105);
//  driver.setRevsPerDC(22.5);
//  driver.setBackScaling(1);
  
  float i = overall_angle(360);
}

void loop() {
  // put your main code here, to run repeatedly:

}

float right_wall_distance(float x){//gives the distance to the right wall (mm) (distance along gradient of 0)
  int right = 2400 - x;
  return right;
}

float left_wall_distance(float x, float y){//returns the distance to the left wall (mm) (distance along gradient of 0)
  if (y <= (2400 + width)){//vehicle is on lower level or ramp
    float left = x - 1200;
    return left;
  }
  else if ((2400 + width) <= y){//vehicle is on upper level
    float left = x;
    return left;
  }
}

float lower_wall_distance(float x, float y){//returns the distance to the lower boundary
  if (x < (1200 + width)){//vehicle is on left side of board (must be on upper level)
    float lower = y - 2400;
    return lower;
  }
  else if ((1200 + width) <= x){//vehicle is on right side (either level)
    float lower = y;
    return lower;
  }
}

float upper_wall_distance(float y){//returns the distance to the upper boundary
  float upper = 3600 - y;
  return upper;
}

float degrees_to_radians(float degrees1){
  float radians1 = degrees1 * (pi / 180);
  return radians1;
}

//float x_rotation(float dist, float overall_angle){
//  //Calculates the new x coordinate of the vehicle's centre after rotation
//  //Parameters : 
//  float change_x = dist * sin(overall_angle);
//  float new_x = x_position + change_x;
//  return new_x;
//}
//
//float y_rotation(float dist, float overall_angle){
//  //Calculates the new x coordinate of the vehicle's centre after rotation
//  //!!Need to try 'or'
//  float change_y = dist * cos(overall_angle);
//  float new_y = y_position + change_y;
//  return new_y;
//}

float distance(int turning_angle){//given in degrees
  float positive_angle = turning_angle % 360; // 0 <= restricted_front_angle < 360
  float radian_turn = degrees_to_radians(positive_angle); //0 <= radian_turn <= pi
  float distance = sqrt(2*65*65*(1 - cos(radian_turn)));
  return distance;
}

float update_rotation(int turning_angle){//Called after a rotation by turning_angle degrees
  float angle = overall_angle(turning_angle);
  float dist = distance(turning_angle);
  x_position -= dist * sin(angle);
  y_position -= dist * cos(angle);
}

float update_motion(int distance){//Called after straight-line motion to update coordinates
  radian_orientation = degrees_to_radians(orientation);
  x_position += distance * sin(radian_orientation);
  y_position += distance * cos(radian_orientation);
}
