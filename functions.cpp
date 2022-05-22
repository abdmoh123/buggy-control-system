#ifndef FUNC
#include "functions.hpp"

using namespace std;

//required to be able to write all files in 1 function in this file
extern ofstream errorResponseGraph;
extern ofstream steerResponseGraph;
extern ofstream speedResponseGraph;

void writeData(float error, float steer, float speed, float t) {
	errorResponseGraph << error << "," << t << endl; //writes file with raw error values
	steerResponseGraph << steer << "," << t << endl; //writes file with steering motor PWM values
	speedResponseGraph << speed << "," << t << endl; //writes file with driving motors PWM values
}

//this function is calibrates the sensors assuming that the track is perfectly aligned with the middle of the buggy
float calibrateSensors(float sensorL, float sensorR) {
	//sensor offset will be used in the algorithm to compensate for the sensors not being symmetrical
	float sensorOffset = sensorL - sensorR;
	return sensorOffset;
}

float limitFloat(float var, float min, float max) {
	//limits steering value between maximum and minimum values
	if (var < min) {
		return min; //returns minimum value
	} else if (var > max) {
		return max; //returns maximum value
	}
	//value does not change if within limits
	return var;
}

#endif