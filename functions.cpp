#include "functions.h"
#include <cmath>

inline double deg2rad(double deg)
{
	return deg*180/M_PI;
}

inline double activationFunction1(double x)
{
	return 1/(1+pow(M_E,x));
}

inline double activationFunction2(double x)
{
	return (pow(M_E,x)-pow(M_E,-x))/(pow(M_E,x)+pow(M_E,-x));
}
