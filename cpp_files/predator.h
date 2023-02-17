#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "entity.h"

class Predator : public Entity
{
public:
	Predator(float x, float y, float dir, NeuralNetwork *brain = nullptr, int generation = 0, float temp_spd = 0, float temp_dir = 0);
	~Predator() override;

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
	static constexpr int	fov_rays			= 11; // must be odd
	static constexpr int	fov_range			= 550; // pixels
	static constexpr int	max_digest_charge	= 10;
	static constexpr int	digest_dropping		= 1;
	static constexpr int	max_eat_dis			= 13;
	static constexpr float	split_recharge		= 140;
	static constexpr float	split_dropping		= 0.3;
	static constexpr float	fov_width			= 60 * M_PI / 180; // radians
	static constexpr float	energy_recovery		= 800;
	static constexpr float	energy_dropping		= 0.60;
	static constexpr float	max_eat_ang			= 160 * M_PI / 180; // radians

	// entity variables
	int type;
	int digest_charge;

	// setters
	void setSpd(float spd) override;
	void setTempSpd(float temp_spd) override;
	void setDigestCharge(int digest_charge);

	// methods
	void split() override;
	void updateProperties() override;
	void entityTick() override;
	void eat(Entity *prey);
	void eatNearbyPrey();
};