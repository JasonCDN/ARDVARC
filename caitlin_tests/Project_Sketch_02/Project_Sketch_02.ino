//#include <SensorControl.h>

#include <DriveControl.h>


/*
 * xCoordinate = x coordinate of vehicle's position (mm), when 0 < y < 2400, 1200 < x < 2400.
 * yCoordinate = y coordinate of vehicle's position (mm), 
 * orientation = direction of frot of vehicle relative to starting position (0 = forward, 90 = right, 180 = back, 270 = left) !! use mod for left turns and multiple right turns !!
 */
 int x_position;
 int y_position = 7;
 int orientation = 0;

/*
 * len = length from front (or back) to centre of vehicle, to be specified later
 * width = width from left (or right) to centre of vehicle, to be specified later
 */

 const int len = 0;
 const int width = 0;

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
  if (y <= (2400 - len)){//vehicle is on lower level or ramp
    float left = x - 1200;
    return left;
  }
  else if ((2400 - len) <= y <= (3600 - len)){//vehicle is on upper level
    float left = x;
    return left;
  }
}

float lower_wall_distance(float x, float y){//returns the distance to the lower boundary
  if (x < (1200 + width)){//vehicle is on left side of board (must be on upper level)
    float lower = y - 2400;
    return lower;
  }
  else if ((1200 + width) <= x <= (2400 - width)){//vehicle is on right side (either level)
    float lower = y;
    return lower;
  }
}

float upper_wall_distance(float y){//returns the distance to the upper boundary
  float upper = 3600 - y;
  return upper;
}

void distance_test(float upper, float lower, float right, float left){
  Serial.print("x coordinate = 927.27   ");
  Serial.print("y coordinate = 2938.32        ");
  Serial.print("Upper ");
  Serial.print(upper);
  Serial.print(" Lower ");
  Serial.print(lower);
  Serial.print(" Right ");
  Serial.print(right);
  Serial.print(" Left ");
  Serial.print(left);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  /* Ultrasonics 1,2,3 at front, 4 at back, 5 for the line tracker.
  sensors.setSensorPins(7, 8, 9, 10, 11);
  sensors.setSonarSpacing(20);
   Set initial positioning by right and rear wall bearings (vehicle should be placed in the right corner)  
  initital dist from centre to right = sensors.getWallDistance() * sin(getWallAngle());
  initial dist from centre to wall behind = sensors.getDistanceRear() + len; */
  float x = 927.27;
  float y = 2938.32;
  float a = upper_wall_distance(y);
  float b = lower_wall_distance(x, y);
  float c = right_wall_distance(x);
  float d = left_wall_distance(x, y);
  distance_test(a, b, c, d);
  
}

//int x = 1400;
//int y = 2500;
//distance_test(upper_wall_distance(y), lower_wall_distances(x, y), right_wall_distances(x), left_wall_distances(x, y));

void loop() {
  // put your main code here, to run repeatedly:

}
