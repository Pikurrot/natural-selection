#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <math.h>
#include <chrono>

using namespace std;

inline float radians(float x) { return x * M_PI / 180; };

float random(float a, float b);

int randint(int a, int b);

float randomChoice(vector<float> &items, vector<float> &weights);

vector<float> randomChoices(vector<float> &items, vector<float> &weights, int k = 1);

vector<vector<float>> zeros(int rows, int cols);
vector<float> zeros(int rows);

vector<float> matMul(vector<float> &a, vector<vector<float>> &b);

vector<float> vecSum(vector<float> &a, vector<float> &b);

float dis(float x1, float y1, float x2, float y2);

float ang(float x1, float y1, float x2, float y2);

void activationFunction1(float &x);
void activationFunction1(vector<float> &arr);

void activationFunction2(float &x);
void activationFunction2(vector<float> &arr);

void setBit(unsigned __int32 &val, int pos);

void setCurrentTime();

long long duration();