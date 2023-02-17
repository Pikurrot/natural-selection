#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "entity.h"

class Prey : public Entity
{
public:
	Prey(float x, float y, float dir, NeuralNetwork *brain = nullptr, int generation = 0, float temp_spd = 0, float temp_dir = 0);
	~Prey() override;

	// getters
	int getType() override;

	// methods
	static int getCount();
	static int getInputs();

protected:
	static int count;

	// getters
	int getFovRays() override;
	int getFovRange() override;
	float getFovWidth() override;

private:
	static constexpr int	fov_rays			= 21; // must be odd
	static constexpr int	fov_range			= 250; // pixels
	static constexpr float	split_recharge		= 0.75;
	static constexpr float	fov_width			= 300 * M_PI / 180; // radians
	static constexpr float	minimum_rest		= 200; // energy units
	static constexpr float	energy_recovery		= 1; // energy units

	// entity variables
	bool rest;
	int type;

	// methods
	void split() override;
	void updateProperties() override;
};