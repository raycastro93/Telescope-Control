#ifndef MyFunctions_h
#define MyFunctions_h
#include "arduino.h"

double ConstrainAngle180(double x);
double ConstrainAngle360(double x);
float DMS2DEC (float Deg, float Min, float Sec);

#endif