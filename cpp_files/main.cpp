#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include "constants.h"
#include "functions.h"
#include "file_writer.h"
#include "neural_network.h"
#include "entity.h"
#include "prey.h"
#include "predator.h"

using namespace std;

int main()
{
	int i, N, percent;
	string file_num;
	vector<__int16> first_parameters;
	ofstream sim_file, brains_file;

	N = 30000;

	file_num = "1";
	sim_file.open("simulations/simulation" + file_num + ".bin", ios::binary);
	brains_file.open("simulations/neural_nets" + file_num + ".bin", ios::binary);

	if (!brains_file.is_open())
	{
		cout << "Warning! file neural_nets" << file_num << ".bin not found. Neural networks will not be saved" << endl;
	}

	// create initial entities
	i = 0;
	while (i < STARTING_PREYS)
	{
		new Prey(randint(0, MAP_W), randint(0, MAP_H), random(0, M_PI * 2));
		i++;
	}
	i = 0;
	while (i < STARTING_PREDATORS)
	{
		new Predator(randint(0, MAP_W), randint(0, MAP_H), random(0, M_PI * 2));
		i++;
	}
	cout << "(0%) 0: " << Prey::getCount() << " - " << Predator::getCount() << endl;

	// write the first parameters
	first_parameters = {MAP_W, MAP_H}; // map width, map heigth

	for (__int16 p : Entity::getInfo()) 
	{
		first_parameters.push_back(p); // entity size, max speed
	}

	first_parameters.push_back(NeuralNetwork::N_LAYERS); // nº hidden layers
	first_parameters.push_back(Prey::getInputs()); // nº prey inputs
	first_parameters.push_back(Predator::getInputs()); // nº predator inputs
	first_parameters.push_back(2); // nº outputs

	for (__int16 p : NeuralNetwork::N_NEURONS)
	{
		first_parameters.push_back(p); // neurons / hidden layer
	}

	sim_file.write(reinterpret_cast<char *>(first_parameters.data()), first_parameters.size() * sizeof(__int16));

	// write the first data
	writeSimData(sim_file, brains_file);

	// write ticks
	if (sim_file.is_open())
	{
		setCurrentTime();
		for (i = 1; i <= N; i++)
		{
			Entity::entitiesTick();
			writeSimData(sim_file, brains_file); // write the tick data

			if (i % 100 == 0 || i == N)
			{
				percent = i * 100 / N;
				cout << "(" << percent << "%) " << i << ": " << Prey::getCount() << " - " << Predator::getCount() << endl;
			}
			
			if (Prey::getCount() == 0)
			{
				cout << "Predators win!" << endl;
				break;
			}
			else if (Predator::getCount() == 0)
			{
				cout << "Preys win!" << endl;
				break;
			}
		}
		sim_file.close();
		cout << "Duration: " << duration() / 1000000.0 << "sec" << endl;
	}
	else
	{
		cout << "Unable to open file simulation" << file_num << ".bin" << endl;
	}
	if (brains_file.is_open())
	{
		brains_file.close();
	}

	Entity::clearEntities();
	
	cout << "Done" << endl;
}
