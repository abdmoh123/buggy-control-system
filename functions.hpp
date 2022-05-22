#ifndef FUNC
#define FUNC

#include <fstream>

//responsible for calibrating sensors in case they output asymmetric voltages
float calibrateSensors(float, float);

//responsible for creating and writing csv files
void writeData(float, float, float, float);

//function below allows limiting of variables between 2 values
float limitFloat(float, float, float);

#endif