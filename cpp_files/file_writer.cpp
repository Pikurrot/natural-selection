#include <fstream>
#include <iostream>
#include "file_writer.h"
#include "entity.h"
#include "prey.h"
#include "predator.h"
#include "neural_network.h"

using namespace std;

__int16 previous_prey_count = 0;
__int16 previous_predator_count = 0;

void writeSimData(ofstream &sim_file, ofstream &brains_file)
{
	// write the data from the simulation to the file
	// writing format (each line = 1 tick):
	// nº preys, nº predators, x1, y1, x2, y2, ..., x1, y1, x2, y2, ...
	int i;
	__int16 prey_dif, predator_dif,
	prey_births = 0, prey_deaths = 0, predator_births = 0, predator_deaths = 0;
	vector<__int16> dead_preys = Entity::getDeadPreys();
	vector<__int16> dead_predators = Entity::getDeadPredators();
	vector<__int16> preys_pos, predators_pos;
	vector<unsigned __int32> encoded_brain;

	// write counts
	prey_dif = Prey::getCount() - previous_prey_count;
	predator_dif = Predator::getCount() - previous_predator_count;

	prey_deaths = dead_preys.size();
	predator_deaths = dead_predators.size();
	prey_births = prey_dif + prey_deaths;
	predator_births = predator_dif + predator_deaths;

	sim_file.write(reinterpret_cast<char *>(&prey_births), sizeof(prey_births));
	sim_file.write(reinterpret_cast<char *>(&prey_deaths), sizeof(prey_deaths));
	sim_file.write(reinterpret_cast<char *>(&predator_births), sizeof(predator_births));
	sim_file.write(reinterpret_cast<char *>(&predator_deaths), sizeof(predator_deaths));
	// write positions of dead entities
	if (dead_preys.size() > 0)
		sim_file.write(reinterpret_cast<char *>(dead_preys.data()), dead_preys.size() * sizeof(__int16));
	if (dead_predators.size() > 0)
		sim_file.write(reinterpret_cast<char *>(dead_predators.data()), dead_predators.size() * sizeof(__int16));
	previous_prey_count = Prey::getCount();
	previous_predator_count = Predator::getCount();

	// write entities positions
	for (i = 0; i < Entity::entities.size(); i++)
	{
		if (Entity::entities[i]->getType() == 0)
		{
			preys_pos.push_back(Entity::entities[i]->getX());
			preys_pos.push_back(Entity::entities[i]->getY());

		}
		else if (Entity::entities[i]->getType() == 1)
		{
			predators_pos.push_back(Entity::entities[i]->getX());
			predators_pos.push_back(Entity::entities[i]->getY());
		}
	}
	sim_file.write(reinterpret_cast<char *>(preys_pos.data()), preys_pos.size() * sizeof(__int16));
	sim_file.write(reinterpret_cast<char *>(predators_pos.data()), predators_pos.size() * sizeof(__int16));

	for (i = 0; i < Entity::new_brains.size(); i++)
	{
		NeuralNetwork brain = Entity::new_brains[i];
		if (brain.getType() == 0)
		{
			encoded_brain = brain.getEncodedInfo();
			brains_file.write(reinterpret_cast<char *>(encoded_brain.data()), encoded_brain.size() * sizeof(__int32));
		}
	}

	for (i = 0; i < Entity::new_brains.size(); i++)
	{
		NeuralNetwork brain = Entity::new_brains[i];
		if (brain.getType() == 1)
		{
			encoded_brain = brain.getEncodedInfo();
			brains_file.write(reinterpret_cast<char *>(encoded_brain.data()), encoded_brain.size() * sizeof(__int32));
		}
	}
}