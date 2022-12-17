#include <iostream>
#include <vector>
#include "functions.h"
#include "neural_network.h"

using namespace std;

int main()
{
	int i;
	NeuralNetwork nn(3);

	for (i = 0; i < 15; i++)
	{
		nn.mutate();
	}

	vector<float> in = {1,2,3};
	vector<float> out = nn.outputs(in);

	nn.show();
	for (i = 0; i < out.size(); i++)
	{
		cout << out[i] << endl;
	}

	cout << "Done!" << endl;
}