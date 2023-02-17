#include <iostream>
#include <vector>
#include <string>
#include "entity.h"
#include "functions.h"
#include "constants.h"

using namespace std;

const float epsilon = 1e-9;

vector<Entity*> Entity::entities;
vector<NeuralNetwork> Entity::new_brains;
vector<__int16> Entity::dead_preys;
vector<__int16> Entity::dead_predators;

Entity::Entity(float x, float y, float dir, NeuralNetwork *brain, int generation, float temp_spd, float temp_dir)
{
	setX(x);
	setY(y);
	setDir(dir);
	this->generation = generation;
	setTempSpd(temp_spd);
	setTempDir(temp_dir);
	setSpd(0);
	setAngSpd(0);
	setEnergy(max_energy);
	setSplitCharge(0);
	entities.push_back(this); // add entity to the list
}

Entity::~Entity()
{
	delete brain;
}

// Properties
float Entity::getX() { return x; }
void Entity::setX(float x)
{
	if (x > MAP_W) this->x = 0;
	else if (x < 0) this->x = MAP_W;
	else this->x = x;
}

float Entity::getY() { return y; }
void Entity::setY(float y)
{
	if (y > MAP_H) this->y = 0;
	else if (y < 0) this->y = MAP_H;
	else this->y = y;
}

float Entity::getDir() { return dir; }
void Entity::setDir(float dir)
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

void Entity::setSpd(float spd)
{
	if (spd < -max_spd) this->spd = -max_spd;
	else if (spd > max_spd) this->spd = max_spd;
	else this->spd = spd;

	if (fabs(this->spd - 0) <= epsilon) this->spd = 0;
}

void Entity::setTempSpd(float temp_spd)
{
	if (temp_spd < -max_temp_spd) this->temp_spd = -max_temp_spd;
	else if (temp_spd > max_temp_spd) this->temp_spd = max_temp_spd;
	else this->temp_spd = temp_spd;

	if (fabs(this->temp_spd - 0) <= epsilon) this->temp_spd = 0;
}

void Entity::setAngSpd(float ang_spd)
{
	if (ang_spd < -max_ang_spd) this->ang_spd = -max_ang_spd;
	else if (ang_spd < -max_ang_spd) this->ang_spd = -max_ang_spd;
	else this->ang_spd = ang_spd;

	if (fabs(this->ang_spd - 0) <= epsilon) this->ang_spd = 0;
}

void Entity::setEnergy(float energy)
{
	if (energy < 0) this->energy = 0;
	else if (energy > max_energy) this->energy = max_energy;
	else this->energy = energy;

	if (fabs(this->energy - 0) <= epsilon) this->energy = 0;
}

void Entity::setSplitCharge(float split_charge)
{
	if (split_charge < 0) this->split_charge = 0;
	else if (split_charge > max_split_charge) this->split_charge = max_split_charge;
	else this->split_charge = split_charge;

	if (fabs(this->split_charge - 0) <= epsilon) this->split_charge = 0;
}

// Methods
void Entity::move()
{
	// move entity x, y and dir
	float spd_x, spd_y;
	setDir(dir + ang_spd);

	if (temp_spd > 0)
		setTempSpd(temp_spd - min(temp_spd_dropping, temp_spd));
	else if (temp_spd < 0)
		setTempSpd(temp_spd + min(temp_spd_dropping, -temp_spd));

	if (move_impulse)
	{
		if (temp_spd == 0)
		{
			setTempSpd(spd);
			setTempDir(dir);
		}
		spd_x = temp_spd * cos(temp_dir);
		spd_y = temp_spd * sin(temp_dir);
	}
	else
	{
		if (temp_spd == 0)
		{
			spd_x = spd * cos(dir);
			spd_y = spd * sin(dir);
		}
		else // in case of split
		{
			spd_x = temp_spd * cos(temp_dir);
			spd_y = temp_spd * sin(temp_dir);
		}
	}

	setX(x + spd_x);
	setY(y + spd_y);
}

void Entity::entityTick()
{
	updateProperties();
	if (alive) // the entity can die in the previous line
	{
		move();
	}
}

void Entity::updateProperties()
{
	setEnergy(energy - spd * r_energy_spd); // decrease energy when moving
	if (split_charge == max_split_charge)
	{
		split(); // split when split_charge is at max
		setSplitCharge(0);
	}
}

vector<Entity *> Entity::fovEntities(int other_type, int fov_range, float fov_width)
{
	// returns a vector of all entities in the field of view
	float distance, angle;
	vector<Entity *> fov_entities;

	// get fov parameters of subclass prey/predator (it's an abstract method)
	if (fov_range == -1) fov_range = getFovRange();
	if (fov_width == -1) fov_width = getFovWidth();

	for (int i = 0; i < entities.size(); i++) // check for all entities
	{
		if (entities[i] != this && // it's not this entity
			entities[i]->alive && // it's alive
			entities[i]->getType() == other_type) // it's the oposite class		 
		{
			distance = dis(getX(), getY(), entities[i]->getX(), entities[i]->getY());
			if (distance < fov_range) // it's near
			{
				angle = ang(getX(), getY(), entities[i]->getX(), entities[i]->getY());
				if (abs(angle - getDir()) < (fov_width / 2)) // it's in the right angle
				{
					entities[i]->relative_dis = distance;
					entities[i]->relative_ang = angle;
					fov_entities.push_back(entities[i]);
				}
			}
		}
	}
	return fov_entities;
}

vector<float> Entity::fov(int other_type)
{
	// returns a vector of strenghts, a value [0 - 1] for each ray in the FOV
	int i, j, ray;
	float ray_ang, e_dis, e_ang, ray_strenght;
	// get the FOV parameters of subclass
	int fov_rays = getFovRays();
	int fov_range = getFovRange();
	float fov_width = getFovWidth();

	int half_rays = fov_rays / 2;
	float rays_sep = fov_width / fov_rays;
	vector<float> ray_angs(fov_rays);
	vector<float> ray_strenghts(fov_rays, 0);
	vector<Entity *> fov_entities = fovEntities(other_type); // get entities in the FOV

	// create a vector of all angles in the FOV, values [-fov_width/2 - fov_width/2]
	for (i = 0; i < fov_rays; i++)
	{
		ray = i - half_rays;
		ray_ang = ray * rays_sep + dir;
		if (ray_ang > M_PI) ray_ang = ray_ang - 2 * M_PI;
		else if (ray_ang < -M_PI) ray_ang = ray_ang + 2 * M_PI;
		ray_angs[i] = ray_ang;
	}

	// set each ray strenght
	for (i = 0; i < fov_entities.size(); i++) // check for all entities
	{
		Entity *other = fov_entities[i];
		e_dis = other->relative_dis;
		e_ang = other->relative_ang;
		
		for (j = 0; j < fov_rays; j++)
		{
			ray_ang = ray_angs[j];

			if ((abs(e_ang - ray_ang) < rays_sep) && (abs(e_dis * sinf(e_ang - ray_ang)) <= size)) // entity collides with the ray
			{
				ray_strenght = 1 - (e_dis / fov_range);
				ray_strenghts[j] = max(ray_strenghts[j], ray_strenght);
				break;
			}
		}
	}
	return ray_strenghts;
}

void Entity::entitiesTick()
{
	bool dead;
	int i;
	__int16 j, k;
	dead_preys.clear();
	dead_predators.clear();
	new_brains.clear();
	// tick the entities that are alive
	for (i = 0; i < entities.size(); i++)
	{
		if (entities[i]->alive)
		{
			entities[i]->entityTick();
		}
	}
	// delete the dead entities
	j = 0;
	k = 0;
	for (vector<Entity *>::iterator it = entities.begin(); it != entities.end();)
	{
		dead = !(*it)->alive;
		if ((*it)->getType() == 0)
		{
			if (dead) dead_preys.push_back(j);
			j++;
		}
		else
		{
			if (dead) dead_predators.push_back(k);
			k++;
		}
		if (dead)
		{
			delete *it;
			it = entities.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Entity::clearEntities()
{
	// delete all entity objects
	for (int i = 0; i < entities.size(); i++)
	{
		delete entities[i]; // delete the pointer
	}
	entities.clear(); // clear the pointers from the vector
}

vector<__int16> Entity::getInfo()
{
	// return Entity info to save to file
	vector<__int16> info = {size};

	if (move_impulse)
	{
		info.push_back(max_temp_spd);
	}
	else
	{
		info.push_back(max_spd);
	}

	return info;
}

vector<__int16> Entity::getDeadPreys()
{
	return dead_preys;
}

vector<__int16> Entity::getDeadPredators()
{
	return dead_predators;
}