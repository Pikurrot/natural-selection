#include <iostream>
#include <vector>
#include <math.h>
#include "prey.h"
#include "constants.h"
#include "functions.h"

using namespace std;

int Prey::count = 0;

Prey::Prey(float x, float y, float dir, NeuralNetwork *brain, int generation, float temp_spd, float temp_dir) : Entity(x, y, dir, brain, generation, temp_spd, temp_dir)
{
	type = 0;
	this->brain = new NeuralNetwork(getFovRays(), brain); // make a copy of the given brain
	if (brain == nullptr) this->brain->setType(getType()); // in case of no parent, set the brain type
	this->brain->mutate(); // mutate the brain
	new_brains.push_back(*(this->brain)); // add brain to the list

	rest = false;
	count++;
}

Prey::~Prey()
{
	count--;
}

int Prey::getType()
{
	return type;
}

int Prey::getCount()
{
	return count;
}

int Prey::getInputs()
{
	return fov_rays;
}

int Prey::getFovRays()
{
	return fov_rays;
}

int Prey::getFovRange()
{
	return fov_range;
}

float Prey::getFovWidth()
{
	return fov_width;
}

void Prey::split()
{
	// create a copy of this object and its brain, and mutate the brain
	if (count == MAX_PREYS) return;
	float split_direction = radians(randint(-180, 179)); // impulse to a random direction
	new Prey(x, y, dir, brain, generation + 1, split_impulse, split_direction); // here the brain is mutated automatically
	setTempSpd(split_impulse);
	setTempDir(split_direction - M_PI);
}

void Prey::updateProperties()
{
	vector<float> out = brain->outputs(fov(2)); // compute NN outputs based on Predators
	setSpd(out[0] * max_spd); // change spd based on first NN output
	setAngSpd(out[1] * max_ang_spd); // change ang_spd based on second NN output
	Entity::updateProperties();
	if (energy == 0 || rest)
	{
		setSpd(0); // stop if no energy or in rest mode
		rest = true; // rest mode to ON
	}
	if (energy >= minimum_rest)
	{
		rest = false; // rest mode to OFF if achieved minimum rest 
	}
	if (spd == 0)
	{
		setEnergy(energy + energy_recovery); // recover energy if in rest mode
	}
	setSplitCharge(split_charge + split_recharge); // continuously recover split charge
}