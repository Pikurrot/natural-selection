import numpy as np
import math
from math import sin,cos
import pygame

# Constants
PI = math.pi
MAP_W = 1000
MAP_H = 1000

class Entity:
	max_spd = 3
	max_w = PI/6 # radians
	max_energy = 10
	max_split = 10

	def __init__(self,x,y,dir,weights,bias,hidden_neurons):
		# kinematic state
		self._x = x
		self._y = y
		self._dir = dir
		# dynamic state
		self._spd = 0
		self._w = 0
		# entity variables
		self._energy = Entity.max_energy
		self._split = 0
		# brain
		self._weights = weights
		self._bias = bias
		self._hidden_neurons = hidden_neurons
	
	# getters
	@property
	def x(self):
		return self._x
	@property
	def y(self):
		return self._y
	@property
	def dir(self):
		return self._dir
	# setters
	@x.setter
	def x(self,x):
		self._x = x
	@y.setter
	def y(self,y):
		self._y = y
	@dir.setter
	def dir(self,dir):
		self._dir = dir

	# methods
	def move(self):
		spd_x = self._spd*cos(self.dir)
		spd_y = self._spd*sin(self.dir)
		self.x += spd_x
		self.y += spd_y
		self.dir += self._w
