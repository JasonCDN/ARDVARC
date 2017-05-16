//#include <SensorControl.h>
//
//#include <DriveControl.h>


/*
 * xCoordinate = x coordinate of vehicle's position (mm)
 * yCoordinate = y coordinate of vehicle's position (mm) 
 * orientation = direction of front of vehicle relative to starting position, in radians (0 = forward, 90 = right, 180 = back, 270 = left) 
 * !! use mod for left turns and multiple right turns !!
 */
 float x_position = 1003;
 float y_position = 3462;
 float orientation = 37.4;

/*
 * len = length from front (or back) to centre of vehicle, to be specified later
 * width = width from left (or right) to centre of vehicle, to be specified later
 * !!Alter through testing!!
 */

 const int len = 115;
 const int width = 45;

 const float pi = 3.14159;


/* 
 *  distances[0] = dist from centre of vehicle to upper either bottom of ramp (lower_level), top of ramp (ramp_level) or upper horizontal wall (upper_level)
 *  distances[1] = dist from centre of vehicle to right wall
 *  distances[2] = dist from centre of vehicle to either lower horizontal wall (lower_level), bottom of ramp (ramp_level) or top of ramp (upper_level)
 *  distances[3] = dist from centre of vehicle to left wall
 */
int distances[5];

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

float x_rotation(float dist, float overall_angle){
  //Calculates the new x coordinate of the vehicle's centre after rotation
  //Parameters : 
  float change_x;
  if(overall_angle <= pi){
    change_x = dist * sin(overall_angle);
  }
  else{
    change_x = -dist * sin(overall_angle);
  }
  float new_x = x_position + change_x;
  return new_x;
}

float y_rotation(float dist, float overall_angle){
  //Calculates the new x coordinate of the vehicle's centre after rotation
  //!!Need to try 'or'
  float change_y;
  if(overall_angle <= pi/2){
    change_y = dist * cos(overall_angle);
  }
  else if(3*pi/2 < overall_angle <= 2*pi){
    change_y = dist * cos(overall_angle);
  }
  else{
    change_y = -dist * cos(overall_angle);
  }
  float new_y = y_position + change_y;
  return new_y;
}

float overall_angle(int front_angle){
  //Calculates the overall change in angle of the centre of the vehicle, used for x_rotation and y_rotation
  //Parameters : front_angle -> the angle by which the vehicle has turned, in degrees (-360 <= front_angle <= 360)
  //!!Try to combine with distance function!!
  float restricted_front_angle = front_angle % 360; // 0 <= restricted_front_angle < 360
  float radian_turn = degrees_to_radians(restricted_front_angle);
  float radian_orientation = degrees_to_radians(orientation);
  float relative_angle;
  if(radian_turn <= pi){
    relative_angle = (3*pi + radian_turn)/2;
  }
  else{
    relative_angle = (-pi + radian_turn)/2;
  }
  float angle = radian_orientation + relative_angle;
  return angle;
}

float distance(int front_angle){
  float restricted_front_angle = front_angle % 360; // 0 <= restricted_front_angle < 360
  float radian_turn = degrees_to_radians(restricted_front_angle);
  float distance = sqrt(2*65*65*(1 - cos(radian_turn)));
  return distance;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  driver.setMotorPins(3, 4, 5, 6, 7, 8);
//  driver.setWheelDiameter(65);
//  driver.setTrackWidth(125);
//  driver.setRevsPerDC(31);
//  sensors.setSonarSpacing(20);
  //Set initial positioning by right and rear wall bearings (vehicle should be placed in the right corner)  
  //x_position = 2400 - sensors.getWallDistance() * sin(getWallAngle());
  //y_position = sensors.getDistanceRear() + len;
  //sqrt(2*65*65*(1 - cos(radian_turn)))
  float angle = overall_angle(-45);
  float dist = distance(-45);
  int y = y_rotation(dist, angle);
  int x = x_rotation(dist, angle);
  Serial.print("x is: ");
  Serial.print(x);
  Serial.print("  y is: ");
  Serial.print(y);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
