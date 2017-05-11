#include "ArmControl.h"

/*
	Setting Variables
*/
void ArmControl::setServoPins(int base, int grip) {
	if (base == grip) {
		if (F_DEBUG) Serial.println("WARNING: Base and grip servos on same pin");
		return;
	}
	s_base.attach(base);
	s_grip.attach(grip);
}

void ArmControl::setServoSpeed(float speed){

}

/*
	Individual Servos
*/

// Main function (universal)
void ArmControl::moveServo(Servo servo, int angle, int min_map, int max_map, int min_true, int max_true) {

}

void ArmControl::setAngle(){

} 
int ArmControl::getAngle(){

}  
void ArmControl::setGrip(){

}  
int ArmControl::getGrip(){

}   


/*
	Predefined actions
*/
void ArmControl::collectTarget(){

} 
void ArmControl::restPosition(){

} 
void ArmControl::readyPosition(){

}