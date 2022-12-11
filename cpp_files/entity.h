# pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "functions.h"
#include "neural_network.h"

class Entity
{
public:
	static constexpr int    size				= 10;
	static int              tick; // sec

	Entity(int x, int y, float dir, NeuralNetwork brain, int generation = 0, int temp_spd = 0, float temp_dir = 0);

	// getters
	int getX();
	int getY();
	float getDir();


protected:
	static constexpr float	max_energy			= 100;
	static constexpr float	max_split_charge	= 100;
	
	// kinematic state
	int x; // pixels
	int y; // pixels
	float dir; // rad (between pi and -pi)
	float temp_dir; // rad (between pi and -pi)

	// dinamic state
	int spd; // pixels/sec
	float ang_spd; // rad/sec
	int temp_spd; // pixels/sec

	// entity variables
	int energy;
	int split_charge;
	int generation;

	// brain
	NeuralNetwork brain;

	// setters
	void setX(int x);
	void setY(int y);
	void setDir(int dir);
	void setTempDir(float temp_dir);
	void setSpd(int spd);
	void setAngSpd(float ang_spd);
	void setTempSpd(int temp_spd);
	void setEnergy(int energy);
	void setSplitCharge(int split_charge);

private:
	static constexpr int	max_spd				= 100;
	static constexpr float	max_ang_spd			= 30 * M_PI * 180; // radians
	static constexpr bool	move_impulse		= false;
	static constexpr int	max_temp_spd			= max_spd;
	static constexpr int	temp_spd_dropping	= max_temp_spd;
	static constexpr float	r_energy_spd		= 0.05; // energy dropped per spd unit
	static constexpr int	split_impulse		= max_temp_spd * 2;
};
