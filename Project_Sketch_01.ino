//#include <SensorControl.h>

//#include <DriveControl.h>


/*
 * xCoordinate = x coordinate of vehicle's position (mm)
 * yCoordinate = y coordinate of vehicle's position (mm)
 * orientation = direction of frot of vehicle relative to starting position (0 = forward, 90 = right, 180 = back, 270 = left) !! use mod for left turns and multiple right turns !!
 */
 int xCoordinate;
 int yCoordinate;
 int orientation = 0;

/* 
 *  arrayName[0] = dist from vehicle to upper left corner
 *  arrayName[1] = dist from vehicle to upper right corner
 *  arrayName[2] = dist from vehicle to lower left corner
 *  arrayName[3] = dist from vehicle to lower right corner
 */
int lowerLevel[] = {0, 0, 0, 0};
int upperLevel[] = {0, 0, 0, 0};
int rampLevel[] = {0, 0, 0, 0};

float distToWall(float left, float right, float len){
  /*
   * Calculates the distance between the vehicle and any wall, meausured either paralled or perpendicular to the wall.
   * Parameters:
   *    left = the dstance between the vehicle and the corner to its left when facing away from the wall
   *    right = the distance between the vehicle to the corner to its right when it is facing away from the wall
   *    len = the length of the wall
   */
  float distance = right * sin(acos((right * right + len * len - left * left) / (2 * right * len)));
  Serial.print(distance);
  return distance;
}

int rightWall(){
  //Returns the distance to the right wall, depending on what level the vehicle is on.
  if(yCoordinate < 2000){//vehicle on lower level
    int distance = distToWall(lowerLevel[3], lowerLevel[1], 2000);
    return distance;
}
  else if(2400 < yCoordinate < 3600){//vehicle on upper level
    int distance = distToWall(upperLevel[3], upperLevel[1], 1200);
    return distance;
}
}

/*
 * distVRight = distance (mm) to 'vertical' right wall (valid on either level, not ramp) - uses a function for different levels
 * distVLeft1 = distance (mm) to 'vertical' lower left wall (valid on lower level, not ramp)
 * distVLeft2 = distance (mm) to 'vertical' upper left wall (valid on upper level, not ramp)
 * distHLower = distance (mm) to 'horizontal' lower wall (valid on lower level, not ramp)
 * distHUpper = distance (mm) to 'horizontal' upper wall (valid on upper level, not ramp)
 */
int distVLeft1 = distToWall(lowerLevel[0], lowerLevel[2], 2000);
int distVLeft2 = distToWall(upperLevel[0], upperLevel[2], 1200);
int distHLower = distToWall(lowerLevel[2], lowerLevel[3], 1200);
int distHUpper = distToWall(upperLevel[1], upperLevel[0], 2400);
int distVRight = rightWall();

/*
if(yCoordinate < 2000){//vehicle on lower level
  //distVRight can be measured on the upper or lower level, requiring different measurements
  distVRight = distToWall(lowerlevel[3], lowerLevel[1], 2000);
}
  else if(2400 < yCoordinate < 3600){//vehicle on upper level
    distVRight = distToWall(upperLevel[3], upperLevel[1], 1200);
}
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  double i = 12.035;
  double j = 1191.102; 
  int h = 1200;
  double k = distToWall(i, j, h);

}

void loop() {
  // put your main code here, to run repeatedly:

}
