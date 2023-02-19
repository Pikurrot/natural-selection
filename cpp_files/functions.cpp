#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <random>
#include <iostream>
#include <string>
#include <chrono>
#include "constants.h"
#include "functions.h"

using namespace std;

chrono::high_resolution_clock::time_point start_time;

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

float randomChoice(vector<float> &items, vector<float> &weights)
{
	float choice;

	mt19937 rng(random_device{}());
	discrete_distribution<> dist(weights.begin(), weights.end());

	choice = items[dist(rng)];

	return choice;
}

vector<float> randomChoices(vector<float> &items, vector<float> &weights, int k)
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

float dis(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrtf(dx * dx + dy * dy);
}

float ang(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return atan2f(dy, dx);
}

void activationFunction1(float &x)
{
	float a = 2.5;
	x = (pow(a, x) - pow(a, -x)) / (pow(a, x) + pow(a, -x));
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
	float a = 2;
	x = (pow(a,x)-pow(a,-x))/(pow(a,x)+pow(a,-x));
}

void activationFunction2(vector<float> &arr)
{
	for (int i = 0; i < arr.size(); i++)
	{
		activationFunction2(arr[i]);
	}
}

void setBit(unsigned __int32 &val, int pos)
{
	__int32 mask = 1 << pos;
	val |= mask;
}

void setCurrentTime()
{
	start_time = chrono::high_resolution_clock::now();
}

long long duration()
{
	auto end_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
	start_time = end_time;
	return duration.count();
}