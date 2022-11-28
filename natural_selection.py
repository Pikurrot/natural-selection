import numpy as np
import math
from math import sin,cos
import pygame

# Constants
PI = math.pi
MAP_W = 1000
MAP_H = 1000
MAX_FPS = 2

class Entity:
	max_spd = 100
	max_w = PI/6 # radians
	max_energy = 100
	max_split = 100
	size = 10
	tick = 0 # sec

	def __init__(self,x,y,dir,weights,bias,hidden_neurons):
		# kinematic state
		self._x = x
		self._y = y
		self._dir = 0
		# dynamic state
		self._spd = 0 # pixels/sec
		self._ang_spd = 0 # rad/sec
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
		self.x += spd_x*Entity.tick
		self.y += spd_y*Entity.tick
		self.dir += self._ang_spd*Entity.tick
		if self.x > MAP_W: self.x = Entity.size
		elif self.x < 0: self.x = MAP_W - Entity.size
		if self.y > MAP_H: self.y = Entity.size
		elif self.y < 0: self.y = MAP_H - Entity.size

	def show(self):
		pygame.draw.circle(screen,(0,0,200),(self.x,self.y),Entity.size)


# functions
def frame():
	screen.fill((255,255,255))
	Entity.tick = clock.tick(MAX_FPS)/1000
	for entity in entities:
		entity.move()
		entity.show()
	pygame.display.update()
	print(Entity.tick)


pygame.init()
screen = pygame.display.set_mode((MAP_W,MAP_H))
pygame.display.set_caption('Simulation')
pygame.display.update()
clock = pygame.time.Clock()

entities = np.array([])
entity0 = Entity(MAP_W/2,MAP_H/2,0,0,0,0)
entities = np.append(entities,entity0)

running = True
while running:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
	frame()