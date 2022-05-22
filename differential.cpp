#ifndef DIFF_HPP
#include "differential.hpp"

#include <iostream>

float calcDiff(float newError, float oldError, float TIME_INCREMENT) {
	//the differential gain will affect how quick the rate of steering will slow down
	const float K_D = 0.2 * (TIME_INCREMENT * 2); //value calculated through Ziegler-Nichols. TIME_INCREMENT is doubled because the period is twice the measuring frequency

	//calculates the gradient of the error response graph
	float differential = (newError - oldError) / TIME_INCREMENT;

	//the new output depends on how fast the response is; the faster it is, the slower its rate of change will be
	return differential * K_D;
}

#endif