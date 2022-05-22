#ifndef PROP_HPP
#include "proportional.hpp"

float calcProp(float error) {
	//proportional gain is a constant that affects how quickly the system performs
	const float K_P = 0.2 * 154; //value calculated through Ziegler-Nichols

	//output is produced by multiplying the error by the gain
	return error * K_P;
}

#endif