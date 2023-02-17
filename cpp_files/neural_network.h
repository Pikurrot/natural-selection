#pragma once
#include <vector>
#include <array>
#include <string>
#include "constants.h"

using namespace std;

class NeuralNetwork
{
public:
	static constexpr int					N_LAYERS	= 2; // hidden layers
	static constexpr array<int, N_LAYERS>	N_NEURONS	= {3,2}; // last layer should have same nº of neurons as nº of outputs
	// ALERT!  N_NEURONS[i] * N_NEURONS[i-1] can't be higher than 128 (include inputs and outputs as layers)

	NeuralNetwork(int n_inputs, NeuralNetwork *parent = nullptr);

	// getters
	int getType();
	vector<unsigned __int32> getEncodedInfo();

	// setters
	void setType(int new_type);

	vector<float> outputs(vector<float> &inputs);
	void mutate();

private:
	static int count;
	static vector<NeuralNetwork *> history; 

	int n_inputs;
	int type;

	array<vector<vector<float>>, N_LAYERS + 1> weights;
	array<vector<float>, N_LAYERS + 1> biases;

	void randomizeWeight(int layer, int row, int col);
	void randomizeBias(int layer, int row);
};