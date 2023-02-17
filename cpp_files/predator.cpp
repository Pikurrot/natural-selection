#include <iostream>
#include <vector>
#include "predator.h"
#include "constants.h"
#include "functions.h"

using namespace std;

const float epsilon = 1e-9;

int Predator::count = 0;

Predator::Predator(float x, float y, float dir, NeuralNetwork *brain, int generation, float temp_spd, float temp_dir) : Entity(x, y, dir, brain, generation, temp_spd, temp_dir)
{
	type = 1;
	this->brain = new NeuralNetwork(getFovRays(), brain); // make a copy of the given brain
	if (brain == nullptr) this->brain->setType(getType()); // in case of no parent, set the brain type
	this->brain->mutate(); // mutate the brain
	new_brains.push_back(*(this->brain)); // add brain to the list

	digest_charge = 0;
	count++;
}

Predator::~Predator()
{
	count--;
}

int Predator::getType()
{
	return type;
}

int Predator::getCount()
{
	return count;
}

int Predator::getInputs()
{
	return fov_rays;
}

int Predator::getFovRays()
{
	return fov_rays;
}

int Predator::getFovRange()
{
	return fov_range;
}

float Predator::getFovWidth()
{
	return fov_width;
}

void Predator::setSpd(float spd)
{
	// predator spd is only forward
	if (spd < 0) this->spd = -spd;
	else if (spd > max_spd) this->spd = max_spd;
	else this->spd = spd;

	if (fabs(this->spd - 0) <= epsilon) this->spd = 0;
}

void Predator::setTempSpd(float temp_spd)
{
	// predator temp_spd is only forward
	if (temp_spd < 0) this->temp_spd = -temp_spd;
	else if (temp_spd > max_spd) this->temp_spd = max_temp_spd;
	else this->temp_spd = temp_spd;

	if (fabs(this->temp_spd - 0) <= epsilon) this->temp_spd = 0;
}

void Predator::setDigestCharge(int digest_charge)
{
	if (digest_charge < 0) this->digest_charge = 0;
	else if (digest_charge > max_digest_charge) this->digest_charge = max_digest_charge;
	else this->digest_charge = digest_charge;

	if (fabs(this->digest_charge - 0) <= epsilon) this->digest_charge = 0;
}

void Predator::split()
{
	// create a copy of this object and its brain, and mutate the brain
	if (count == MAX_PREDATORS) return;
	float split_direction = radians(randint(-180, 179)); // impulse to a random direction
	new Predator(x, y, dir, brain, generation + 1, split_impulse, split_direction); // here the brain is mutated automatically
	setTempSpd(split_impulse);
	setTempDir(split_direction - M_PI);
}

void Predator::updateProperties()
{
	vector<float> out = brain->outputs(fov(0)); // compute NN outputs based on Preys
	setSpd(out[0] * max_spd); // change spd based on first NN output
	setAngSpd(out[1] * max_ang_spd); // change ang_spd based on second NN output
	Entity::updateProperties();
	setEnergy(energy - energy_dropping); // continuously drop energy
	if (energy == 0)
	{
		// die if no energy
		alive = false;
		return;
	}
	setDigestCharge(digest_charge - digest_dropping); // continuously drop digest_charge
	if (generation < 0)
	{
		setSplitCharge(split_charge + split_dropping); // increase split_charge the first X generations
	}
	else
	{
		setSplitCharge(split_charge - split_dropping); // drop split_charge the rest of generations
	}
}

void Predator::entityTick()
{
	Entity::entityTick();
	eatNearbyPrey();
}

void Predator::eat(Entity *prey)
{
	// kill the giving prey
	prey->alive = false;
	if (digest_charge == 0) // if not digesting
	{
		setEnergy(energy + energy_recovery); // recover energy
		setSplitCharge(split_charge + split_recharge); // increase split_charge
	}
	setDigestCharge(max_digest_charge); // start digesting
}

void Predator::eatNearbyPrey()
{
	// search for nearby preys to eat
	int i, argmin = -1;
	float min_dis = -1, distance;
	if (spd == 0) return;
	vector<Entity *> fov_preys = fovEntities(0, max_eat_dis, max_eat_ang);
	if (fov_preys.size() == 0) return;
	for (i = 0; i < fov_preys.size(); i++) // for all preys in the FOV
	{
		// search the prey at minimum distance
		distance = fov_preys[i]->relative_dis;
		if (distance < min_dis || min_dis == -1)
		{
			min_dis = distance;
			argmin = i;
		}
	}
	if (argmin != -1)
	{
		eat(fov_preys[argmin]); // eat that prey
	}
}