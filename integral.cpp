#ifndef INTG_HPP
#include "integral.hpp"

using namespace std;

float calcIntg(float old_error, float error, float TIME_INCREMENT) {
	//the integral gain will affect how small the steady state error will be
	const float K_I = (TIME_INCREMENT * 2) / 1; //time period of wave is 2 * TIME_INCREMENT

	float area;
	//checks if the errors have opposite signs (1 positive and 1 negative)
	if ((error < 0) != (old_error < 0)) {
		//if they have different signs, then the area is calculated as 2 separate triangles
		float gradient = (error - old_error) / TIME_INCREMENT; //finds gradient
		float zeroPoint = -old_error / gradient; //uses gradient to find point where graph meets zero
		area = (abs(old_error * zeroPoint) + abs(error * (TIME_INCREMENT - zeroPoint))) / 2; //finds area of 2 triangles
	} else {
		//if they are both positive or negative, then the area under the graph = area of trapezium
		area = (TIME_INCREMENT / 2) * (old_error + error);
	}

	return area * K_I; //area is multiplied by gain to produce the final output
}

#endif