#include <iostream>
#include <algorithm>
#include "neural_network.h"
#include "functions.h"

using namespace std;

int NeuralNetwork::count = 0;
vector<NeuralNetwork *> NeuralNetwork::history = {};

NeuralNetwork::NeuralNetwork(int n_inputs, NeuralNetwork *parent)
{
	int l;
	this->n_inputs = n_inputs;
	type = 0;

	if (parent == nullptr)
	{
		// initialize parameters to 0
		weights[0] = zeros(n_inputs, N_NEURONS[0]); // input weights
		weights[weights.size() - 1] = zeros(N_NEURONS[N_LAYERS - 1], 2); // output weights
		biases[biases.size() - 1] = zeros(2); // output biases

		for (l = 1; l < N_LAYERS; l++)
		{
			weights[l] = zeros(N_NEURONS[l - 1], N_NEURONS[l]); // hidden layer weigths
		}

		for (l = 0; l < N_LAYERS; l++)
		{
			biases[l] = zeros(N_NEURONS[l]); // hidden layer biases
		}
	}
	else
	{
		// inherit parameters from the given brain
		type = parent->type;
		weights = parent->weights;
		biases = parent->biases;
	}

	history.push_back(this);
	count++;
}

vector<float> NeuralNetwork::outputs(vector<float> &inputs)
{
	// return the outputs using forward-propagation
	vector<float> outputs;
	outputs = vecSum(matMul(inputs, weights[0]), biases[0]);
	activationFunction1(outputs);

	for (int l = 1; l < N_LAYERS + 1; l++)
	{
		outputs = vecSum(matMul(outputs, weights[l]), biases[l]);
		if (l != N_LAYERS)
		{
			activationFunction1(outputs); // apply activationFunction1 until last layer (not included)
		}
	}

	activationFunction2(outputs); // last layer apply activationFunction2
	return outputs;
}

int NeuralNetwork::getType()
{
	return type;
}

vector<unsigned __int32> NeuralNetwork::getEncodedInfo()
{
	int layer, neuron, n_neurons, j, i, w, b;
	unsigned __int32 val;
	vector<unsigned __int32> encoded_info, encoded_w, encoded_b;

	for (layer = 0; layer < N_LAYERS + 1; layer++)
	{
		val = 0;
		j = 0;
		i = 0;

		if (layer == 0) n_neurons = n_inputs;
		else n_neurons = N_NEURONS[layer - 1];

		for (neuron = 0; neuron < n_neurons; neuron++)
		{
			for (w = 0; w < weights[layer][neuron].size(); w++)
			{
				if (j == 32)
				{
					encoded_w.push_back(val);
					val = 0;
					j = 0;
					i++;
				}

				if (weights[layer][neuron][w] != 0)
				{
					setBit(val, j);
				}

				j++;
			}
		}

		while (i < 4)
		{
			encoded_w.push_back(val);
			val = 0;
			i++;
		}

		val = 0;
		j = 0;

		if (layer == N_LAYERS) n_neurons = 2;
		else n_neurons = N_NEURONS[layer];

		for (b = 0; b < biases[layer].size(); b++)
		{
			if (biases[layer][b] != 0)
			{
				setBit(val, j);
			}

			j++;
		}

		encoded_b.push_back(val);

	}

	encoded_info.reserve(encoded_w.size() + encoded_b.size());
	encoded_info.insert(encoded_info.end(), encoded_w.begin(), encoded_w.end());
	encoded_info.insert(encoded_info.end(), encoded_b.begin(), encoded_b.end());

	return encoded_info;
}

void NeuralNetwork::mutate()
{
	// randomize some random parameters
	int i, r, layer, row, col;
	vector<float> p = {.6,.2,.2};
	vector<float> range = {0,1,2};
	vector<float> p_layers;
	vector<float> range_layers;

	for (i = 0; i < N_LAYERS + 1; i++)
	{
		range_layers.push_back(i);
	}

	r = randomChoice(range, p);

	switch (r)
	{
	case 0: // randomize a weight
		p_layers = {.7,.15,.15};
		layer = randomChoice(range_layers, p_layers);
		row = randint(0, weights[layer].size() - 1);
		col = randint(0, weights[layer][0].size() - 1);
		randomizeWeight(layer, row, col);
		break;
	case 1: // randomize a bias
		p_layers = {.4,.3,.3};
		layer = randomChoice(range_layers, p_layers);
		row = randint(0, biases[layer].size() - 1);
		randomizeBias(layer, row);
		break;
	case 2: // randomize a path of weights (from input to output)
		row = randint(0, weights[0].size() - 1);
		for (layer = 0; layer < N_LAYERS + 1; layer++)
		{
			col = randint(0, weights[layer][0].size() - 1);
			randomizeWeight(layer, row, col);
			row = col;
		}
		break;
	default:
		break;
	}
}

void NeuralNetwork::setType(int new_type)
{
	type = new_type;
}

void NeuralNetwork::randomizeWeight(int layer, int row, int col)
{
	weights[layer][row][col] = random(-1, 1);
}

void NeuralNetwork::randomizeBias(int layer, int row)
{
	biases[layer][row] = random(-0.5, 0.5);
}
