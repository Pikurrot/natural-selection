#include <cmath>
#include "entity.h"
#include "constants.h"

Entity::Entity(int x, int y, float dir, NeuralNetwork brain, int generation, int temp_spd, float temp_dir)
{
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->brain = brain;
	this->generation = generation;
	this->temp_spd = temp_spd;
	this->temp_dir = temp_dir;
	spd = 0;
	ang_spd = 0;
	energy = max_energy;
	split_charge = 0;
}

int Entity::getX() { return x; }
void Entity::setX(int x)
{
	if (x > MAP_W) this->x = size;
	else if (x < 0) this->x = MAP_W - size;
	else this->x = x;
}

int Entity::getY() { return y; }
void Entity::setY(int y)
{
	if (y > MAP_H) this->y = size;
	else if (y < 0) this->y = MAP_H - size;
	else this->y = y;
}

float Entity::getDir() { return dir; }
void Entity::setDir(int dir)
{
	if (dir > M_PI) this->dir = dir - 2 * M_PI;
	else if (dir < -M_PI) this->dir = dir + 2 * M_PI;
	else this->dir = dir;
}

void Entity::setTempDir(float temp_dir)
{
	if (temp_dir > M_PI) this->temp_dir = temp_dir - 2 * M_PI;
	else if (temp_dir < -M_PI) this->temp_dir = temp_dir + 2 * M_PI;
	else this->temp_dir = temp_dir;
}

void Entity::setSpd(int spd)
{
	if (spd < -max_spd) this->spd = -max_spd;
	else if (spd > max_spd) this->spd = max_spd;
	else this->spd = spd;
}

void Entity::setAngSpd(float ang_spd)
{
	if (ang_spd < -max_ang_spd) this->ang_spd = -max_ang_spd;
	else if (ang_spd < -max_ang_spd) this->ang_spd = -max_ang_spd;
	else this->ang_spd = ang_spd;
}

void Entity::setTempSpd(int temp_spd)
{
	if (temp_spd < -max_temp_spd) this->temp_spd = -max_temp_spd;
	else if (temp_spd > max_temp_spd) this->temp_spd = max_temp_spd;
	else this->temp_spd = temp_spd;
}

void Entity::setEnergy(int energy)
{
	if (energy < 0) this->energy = 0;
	else if (energy > max_energy) this->energy = max_energy;
	else this->energy = energy;
}

void Entity::setSplitCharge(int split_charge)
{
	if (split_charge < 0) this->split_charge = 0;
	else if (split_charge > max_split_charge) this->split_charge = max_split_charge;
	else this->split_charge = split_charge;
}
