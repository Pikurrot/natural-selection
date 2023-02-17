# pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <memory>
#include "neural_network.h"

using namespace std;

class Entity
{
public:
	static vector<Entity*> entities;
	static vector<NeuralNetwork> new_brains;
	bool alive;
	float relative_dis;
	float relative_ang;

	Entity(float x, float y, float dir, NeuralNetwork *brain = nullptr, int generation = 0, float temp_spd = 0, float temp_dir = 0);
	virtual ~Entity();

	// getters
	virtual int getType() = 0;
	float getX();
	float getY();
	float getDir();

	// methods
	static void entitiesTick();
	static void clearEntities();
	static vector<__int16> getInfo();
	static vector<__int16> getDeadPreys();
	static vector<__int16> getDeadPredators();

protected:
	static constexpr bool	move_impulse		= false;
	static constexpr int    size				= 8;
	static constexpr float	max_split_charge	= 300;
	static constexpr float	max_spd				= 12;
	static constexpr float	max_temp_spd		= max_spd;
	static constexpr float	temp_spd_dropping	= 0.15;
	static constexpr float	split_impulse		= 2.5;
	static constexpr float	max_ang_spd			= 18 * M_PI / 180; // radians
	static constexpr float	max_energy			= 1000; // energy units
	static constexpr float	r_energy_spd		= 0.3; // energy units dropped per spd unit
	
	// kinematic state
	float x; // pixels
	float y; // pixels
	float dir; // rad (between pi and -pi)
	float temp_dir; // rad (between pi and -pi)

	// dinamic state
	float spd; // pixels/sec
	float ang_spd; // rad/sec
	float temp_spd; // pixels/sec

	// entity variables
	int generation;
	float energy; // energy units
	float split_charge;

	// brain
	NeuralNetwork *brain = nullptr;

	// getters
	virtual int getFovRays() = 0;
	virtual int getFovRange() = 0;
	virtual float getFovWidth() = 0;

	// setters
	virtual void setSpd(float spd);
	virtual void setTempSpd(float temp_spd);
	void setX(float x);
	void setY(float y);
	void setDir(float dir);
	void setTempDir(float temp_dir);
	void setAngSpd(float ang_spd);
	void setEnergy(float energy);
	void setSplitCharge(float split_charge);

	// methods
	virtual void split() = 0;
	virtual void updateProperties();
	virtual void entityTick();
	vector<Entity *> fovEntities(int other_type, int fov_range = -1, float fov_width = -1.);
	vector<float> fov(int other_type);
	void move();

private:
	static vector<__int16> dead_preys;
	static vector<__int16> dead_predators;
};
