# pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "functions.h"

class Entity
{
public:
	static constexpr int    size				= 10;
	static int              tick; // sec

	Entity(int x, int y, float dir);

	int getX();
	int getY();
	int getDir();

protected:
	static constexpr float	max_energy			= 100;
	static constexpr float	max_split_charge	= 100;
	
	int x;
	int y;
	float dir;

private:
	static constexpr int	max_spd				= 100;
	static constexpr float	max_ang_spd			= 30 * M_PI * 180; // radians
	static constexpr bool	move_impulse		= false;
	static constexpr int	max_tmp_spd			= max_spd;
	static constexpr int	temp_spd_dropping	= max_tmp_spd;
	static constexpr float	r_energy_spd		= 0.05; // energy dropped per spd unit
	static constexpr int	split_impulse		= max_tmp_spd * 2;
};
