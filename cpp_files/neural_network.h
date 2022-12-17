#pragma once
#include <vector>
#include <array>

using namespace std;

class NeuralNetwork
{
public:
	NeuralNetwork(int n_inputs);
	vector<float> outputs(vector<float> &inputs);
	void mutate();
	void show();

private:
	static constexpr int n_neurons[] = {3,2};
	static constexpr int n_layers = size(n_neurons);
	const int n_inputs;

	array<vector<vector<float>>, n_layers + 1> weights;
	array<vector<float>, n_layers + 1> biases;

	void randomizeWeight(int layer, int row, int col);
	void randomizeBias(int layer, int row);
};