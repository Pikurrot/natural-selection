#include <algorithm>
#include <iostream>
#include "neural_network.h"
#include "functions.h"

using namespace std;

NeuralNetwork::NeuralNetwork(int n_inputs) : n_inputs(n_inputs)
{
	weights[0] = zeros(n_inputs, n_neurons[0]);
	weights[weights.size() - 1] = zeros(n_neurons[n_layers - 1], 2);
	biases[biases.size() - 1] = zeros(2);

	for (int l = 0; l < n_layers-1; l++)
	{
		weights[l+1] = zeros(n_neurons[l], n_neurons[l+1]);
	}
	for (int l = 0; l < n_layers; l++)
	{
		biases[l] = zeros(n_neurons[l]);
	}

}

vector<float> NeuralNetwork::outputs(vector<float> &inputs)
{
	vector<float> outputs;
	outputs = vecSum(matMul(inputs, weights[0]), biases[0]);
	activationFunction1(outputs);

	for (int l = 1; l < n_layers + 1; l++)
	{
		outputs = vecSum(matMul(outputs, weights[l]), biases[l]);
		if (l != n_layers)
		{
			activationFunction1(outputs);
		}
	}

	activationFunction2(outputs);
	return outputs;
}

void NeuralNetwork::mutate()
{
	int r, layer, row, col;
	vector<float> p = {.6,.2,.2};
	vector<float> range = {0,1,2};
	r = random_choices(range, p)[0];
	cout << r;

	switch (r)
	{
	case 0:
		layer = randint(0, n_layers);
		row = randint(0, weights[layer].size() - 1);
		col = randint(0, weights[layer][0].size() - 1);

		cout << " Randomizing weight " << layer << " " << row << " " << col << endl;
		randomizeWeight(layer, row, col);
		cout << " Weight is: " << weights[layer][row][col] << endl;

		break;
	case 1:
		layer = randint(0, n_layers);
		row = randint(0, weights[layer].size() - 1);

		cout << " Randomizing bias " << layer << " " << row << endl;
		randomizeBias(layer, row);
		cout << " Bias is: " << biases[layer][row] << endl;

		break;
	case 2:
		row = randint(0, weights[0].size() - 1);
		for (layer = 0; layer < n_layers + 1; layer++)
		{
			col = randint(0, weights[layer][0].size() - 1);

			cout << " Randomizing weight " << layer << " " << row << " " << col << endl;
			randomizeWeight(layer, row, col);
			cout << " Weight is: " << weights[layer][row][col] << endl;
			row = col;
		}

		break;
	default:
		break;
	}
}

void NeuralNetwork::show()
{
	for (int i = 0; i < weights.size(); i++)
	{
		for (int j = 0; j < weights[i].size(); j++)
		{
			for (int k = 0; k < weights[i][j].size(); k++)
			{
				cout << i << " " << j << " " << k << " " << weights[i][j][k] << endl;
			}
		}
	}
	for (int i = 0; i < biases.size(); i++)
	{
		cout << i << endl;
		for (int j = 0; j < biases[i].size(); j++)
		{
			cout << i << " " << j << " " << biases[i][j] << endl;
		}
	}
}

void NeuralNetwork::randomizeWeight(int layer, int row, int col)
{
	weights[layer][row][col] = random(-1, 1);
}

void NeuralNetwork::randomizeBias(int layer, int row)
{
	biases[layer][row] = random(-1, 1);
}
