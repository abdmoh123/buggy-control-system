#include <iostream>
#include <fstream> //required for file writing
#include <cstdlib>
//libraries for waiting (sleep)
#include <ctime>
#include <unistd.h>
#include <cmath> //required for abs() and tanh()
#include <math.h> //allows pi constant to be used

#ifndef PROP_HPP
#include "proportional.hpp"
#endif
#ifndef INTG_HPP
#include "integral.hpp"
#endif
#ifndef DIFF_HPP
#include "differential.hpp"
#endif
#ifndef FUNC
#include "functions.hpp"
#endif

using namespace std;

//the ofstreams are global variables to allow other files in the program (not main.cpp) to write data
ofstream errorResponseGraph("error response.csv", ios::out); //creates or opens a csv file for error response
ofstream steerResponseGraph("steer response.csv", ios::out); //creates or opens a csv file for steering response
ofstream speedResponseGraph("speed response.csv", ios::out); //creates or opens a csv file for speed response

int main() {
	const float V_MAX = 2.6; //max voltage output from sensor subsystem
	const float V_MIN = 1.44; //minimum voltage output from sensor subsystem
	const float MAX_ERROR = V_MAX - V_MIN;
	const float TIME_INCREMENT = 0.1; //time between every measurement

	const float S_MAX = 115; //maximum value for steering motor (moves right)
	const float S_REF = 90; //reference value for steering motor
	const float S_MIN = 65; //minimum value for steering motor (moves left)
	const float D_MAX = 85; //maximum value for driving motors (very fast)
	const float D_REF = 25; //reference and minimum value for driving motors (just before starting to move)

	const float MAX_TOTAL_ERROR = calcProp(MAX_ERROR) + calcIntg(0, MAX_ERROR, TIME_INCREMENT) - calcDiff(MAX_ERROR, 0, TIME_INCREMENT); //maximum error produced by PID algorithm
	const float MAX_ABS_ERROR = abs(calcProp(MAX_ERROR)) + abs(calcIntg(0, MAX_ERROR, TIME_INCREMENT)) + abs(calcDiff(MAX_ERROR, 0, TIME_INCREMENT)); //maximum absolute error produced by PID algorithm
	const float S_SCALE = 25 / MAX_TOTAL_ERROR; //scales the influenced error when calculating steering
	const float D_SCALE = -1.39 / MAX_ABS_ERROR; //scales the abs error when calculating the motor speed

	float errorOffset = 0; //variable is used to account for asymmetric sensor voltages
	float errorDistance = -2; //distance between middle of buggy and track (negative if to the left and vice versa)
	float randDistance = 0; //used to simulate non-straight unpredictable track
	float sensorSeparation = 5; //distance between the 2 sensors

	//initialises the sensor distances in advance
	float distanceL = sensorSeparation / 2;
	float distanceR = sensorSeparation / 2;
	//initialises the sensor variables in advance
	float sensorL = 0.0;
	float sensorR = 0.0;
	//float sensorR = 0.05; //for testing offset calibration function

	//calibrates sensors if they are not synchronised
	calibrateSensors(sensorL, sensorR);

	//initialises the output data so it doesn't have to be declared again
	float steer = S_REF;
	float speed = D_REF;
	float totalTime = 0; //time starts at zero

	float error = 0 + errorOffset; //default starting value for error
	float oldError = error; //default starting value for the old error (previous)

	if (!errorResponseGraph.good() || !steerResponseGraph.good() || !speedResponseGraph.good()) {
		//outputs error message and ends program if any file could not be created
		cerr << "Error: Files failed to open" << endl;
		exit(EXIT_FAILURE);
	} else {
		//makes random number more unpredictable (needs to be outside loop)
		srand(time(NULL));

		while (true) {
			randDistance = (((float) rand() / RAND_MAX) * 1.0) - 0.5; //random number between -0.5 and 0.5

			//calculates sensor distances using distance between track and buggy midpoint
			distanceL = (sensorSeparation / 2) - errorDistance;
			distanceR = (sensorSeparation / 2) + errorDistance;
			//calculates sensor voltage outputs based on distance from the track while keeping them within voltage limits
			sensorL = limitFloat(V_MAX * exp(-0.12 * distanceL), V_MIN, V_MAX); //constant ensures min value is ~V_MIN
			sensorR = limitFloat(V_MAX * exp(-0.12 * distanceR), V_MIN, V_MAX); //constant ensures min value is ~V_MIN
			//sensorR = limitFloat(V_MAX * exp(-0.12 * distanceR), V_MIN, V_MAX) + 0.05; //for testing offset
			
			//error is calculated by finding difference between sensor voltages
			error = (sensorL - sensorR) + errorOffset; //offset allows sensors to be calibrated
			//proportional response error value is calculated
			float propError = calcProp(error);
			//integral response error value is calculated by multiplying dt with the previous error
			float intgError = calcIntg(oldError, error, TIME_INCREMENT);
			//differential response is calculated by finding the gradient between the previous error and the current one
			float diffError = calcDiff(oldError, error, TIME_INCREMENT);
			//combines all error values together
			float errorTotal = propError + intgError + diffError;
			float absError = abs(propError) + abs(intgError) + abs(diffError);

			//calculates steering motor output based on response as well as keeping it within limits
			steer = limitFloat(S_REF + (errorTotal * S_SCALE), S_MIN, S_MAX);
			//calculates driving motors outputs based on response as well as keeping it within limits
			speed = limitFloat(D_REF + (D_MAX - D_REF) * exp(D_SCALE * absError), D_REF, D_MAX);

			//finds new error distance using output variables calculated
			//steer is converted into radians due to cos()
			errorDistance = speed * (TIME_INCREMENT / 3) * cos(steer * (M_PI / 180));// + randDistance; //random variable for unpredictability

			//outputs error values for debugging and testing
			cout << "Input error: " << error << endl << "Output error: " << errorTotal << endl;
			cout << "Steering: " << steer << endl << "Speed: " << speed << endl;
			totalTime += TIME_INCREMENT; //increases time every time it loops (for graph)
			writeData(error, steer, speed, totalTime); //writes all csv files

			//if statements mainly used for testing the program
			if (errorTotal > 0) {
				//turns steering motor left
				cout << "Moving left" << endl;
			} else if (errorTotal < 0) {
				//turns steering motor right
				cout << "Moving right" << endl;
			} else {
				//steering motor is not turned
				cout << "Equilibrium" << endl;
				break; //stops the program
			}

			//waits before refreshing the outputs
			usleep(1000000 * TIME_INCREMENT); //0.5 seconds
			cout << "\033[2J\033[1;1H";
		}
	}
}
