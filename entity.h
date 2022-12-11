# pragma once

class Entity
{
public:
	static constexpr int    size				= 10;
	static int              tick; // sec
protected:
	static constexpr double max_energy			= 100;
	static constexpr double max_split_charge	= 100;
private:
	static constexpr double max_spd				= 100;
	static constexpr double max_ang_spd			= radians(30); // radians
	static constexpr bool   move_impulse		= false;
	static constexpr double max_tmp_spd			= max_spd;
	static constexpr double temp_spd_dropping	= max_spd;
	static constexpr double r_energy_spd		= 0.05; // energy dropped per spd unit
	static constexpr double split_impulse		= max_tmp_spd * 2;
};
