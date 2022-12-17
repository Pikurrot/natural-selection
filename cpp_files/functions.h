#pragma once
#include <vector>

using namespace std;

float random(float a, float b);

int randint(int a, int b);

vector<float> random_choices(vector<float> &items, vector<float> &weights, int k = 1);

vector<vector<float>> zeros(int rows, int cols);
vector<float> zeros(int rows);

vector<float> matMul(vector<float> &a, vector<vector<float>> &b);

vector<float> vecSum(vector<float> &a, vector<float> &b);

void activationFunction1(float &x);
void activationFunction1(vector<float> &arr);

void activationFunction2(float &x);
void activationFunction2(vector<float> &arr);