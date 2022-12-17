#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include "functions.h"

using namespace std;

float random(float a, float b)
{
	float random = (float)rand() / RAND_MAX;

	return a + (b - a) * random;
}

int randint(int a, int b)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(a, b);

	return dis(gen);
}

vector<float> random_choices(vector<float> &items, vector<float> &weights, int k)
{
	vector<float> choices;

	mt19937 rng(random_device{}());
	discrete_distribution<> dist(weights.begin(), weights.end());

	for (int i = 0; i < k; ++i)
	{
		choices.push_back(items[dist(rng)]);
	}

	return choices;
}

vector<vector<float>> zeros(int rows, int cols)
{
	vector<vector<float>> matrix(rows, vector<float>(cols));

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			matrix[r][c] = 0.0;
		}
	}

	return matrix;
}

vector<float> zeros(int len)
{
	vector<float> matrix(len);

	for (int i = 0; i < len; i++)
	{
		matrix[i] = 0.0;
	}

	return matrix;
}

vector<float> matMul(vector<float> &a, vector<vector<float>> &b)
{
	int size = b[0].size();
	vector<float> result(size, 0);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < a.size(); j++)
		{
			result[i] += a[j] * b[j][i];
		}
	}

	return result;
}

vector<float> vecSum(vector<float> &a, vector<float> &b)
{
	vector<float> result;
	
	for (int i = 0; i < a.size(); i++)
	{
		result.push_back(a[i] + b[i]);
	}

	return result;
}

void activationFunction1(float &x)
{
	x = 1/(1+pow(M_E,-x));
}

void activationFunction1(vector<float> &arr)
{
	for (int i = 0; i < arr.size(); i++)
	{
		activationFunction1(arr[i]);
	}
}

void activationFunction2(float &x)
{
	x = (pow(M_E,x)-pow(M_E,-x))/(pow(M_E,x)+pow(M_E,-x));
}

void activationFunction2(vector<float> &arr)
{
	for (int i = 0; i < arr.size(); i++)
	{
		activationFunction2(arr[i]);
	}
}
