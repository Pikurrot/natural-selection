#include "entity.h"

Entity::Entity(int x, int y, float dir)
	: x(x), y(y), dir(dir) {}

int Entity::getX() { return x;}
int Entity::getY() { return y; }
int Entity::getDir() { return dir; }