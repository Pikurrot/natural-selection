#include <iostream>
#include "entity.h"

using namespace std;

int main()
{
	int a;
	NeuralNetwork *nn = new NeuralNetwork();
	Entity entity0(4, 5, 3.14, *nn);
	cout << entity0.getX() << endl;
}