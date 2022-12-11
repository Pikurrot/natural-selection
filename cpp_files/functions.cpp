#define _USE_MATH_DEFINES
#include <math.h>
#include "functions.h"

inline double activationFunction1(double x)
{
	return 1/(1+pow(M_E,x));
}

inline double activationFunction2(double x)
{
	return (pow(M_E,x)-pow(M_E,-x))/(pow(M_E,x)+pow(M_E,-x));
}
