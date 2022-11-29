import numpy as np
import math
from math import sin,cos
import pygame

# Constants
PI = math.pi
MAP_W = 1000
MAP_H = 1000
MAX_FPS = 120

class Entity:
	max_spd = 100
	max_w = PI/6 # radians
	max_energy = 100
	max_split = 100
	size = 10
	tick = 0 # sec

	def __init__(self,x,y,dir,weights,bias,hidden_neurons):
		# kinematic state
		self._x = x # pixels
		self._y = y # pixels
		self._dir = dir # rad (between PI and -PI)
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
		if x > MAP_W: self._x = Entity.size
		elif x < 0: self._x = MAP_W - Entity.size
		else: self._x = x
	@y.setter
	def y(self,y):
		if y > MAP_H: self._y = Entity.size
		elif y < 0: self._y = MAP_H - Entity.size
		else: self._y = y
	@dir.setter
	def dir(self,dir):
		self._dir = dir

	def move(self):
		# move entity x, y and dir
		spd_x = self._spd*cos(self.dir)
		spd_y = self._spd*sin(self.dir)
		self.x += spd_x*Entity.tick
		self.y += spd_y*Entity.tick
		self.dir += self._ang_spd*Entity.tick
		if self.dir > PI: self.dir -= 2*PI
		elif self.dir < -PI: self.dir += 2*PI

	def show(self):
		# show entity in screen
		color = (0,0,200)
		pygame.draw.circle(screen,color,(self.x,self.y),Entity.size)

	def FOV_entities(self,entity_class):
		# returns array of entities inside FOV
		return np.array([entity for entity in entities if 
			(entity != self and type(entity) == entity_class and e_dis(self,entity) < entity_class.FOV_range)])

	def FOV(self,entity_class):
		# returns array with activation values (0.0-1.0) of every ray in the Field Of View
		half_rays = entity_class.FOV_rays//2
		rays_sep = entity_class.FOV_width / entity_class.FOV_rays
		ray_angs = [ray * rays_sep + self.dir for ray in range(-half_rays,half_rays+1)]
		ray_angs = np.array([ray-2*PI if ray > PI else (ray+2*PI if ray < -PI else ray) for ray in ray_angs])
		ray_strenghts = np.zeros(ray_angs.shape)
		for entity in self.FOV_entities(entity_class):
			dis = e_dis(self,entity)
			e_ang_ = e_ang(self,entity)
			e_ray_activations = np.array([(abs(dis*sin(e_ang_-ang)) <= Entity.size) and (abs(e_ang_-ang)<rays_sep) for ang in ray_angs])
			e_ray_strenghts = e_ray_activations*(1-dis/entity_class.FOV_range)
			ray_strenghts = np.maximum(ray_strenghts,e_ray_strenghts)
		for ang,strenght in zip(ray_angs,ray_strenghts):
			ray_x = self.x + entity_class.FOV_range * cos(ang)
			ray_y = self.y + entity_class.FOV_range * sin(ang)
			pygame.draw.line(screen, (255,0,0), (self.x,self.y), (ray_x,ray_y), max(1,int(strenght*30)))
		return ray_strenghts

	def entity_tick(self):
		self.move()
		self.FOV()
		self.show()


class Prey(Entity):
	FOV_rays = 21 # must be odd
	FOV_range = 300 # pixels
	FOV_width = math.radians(300) # radians
	Color = (0,200,0)

	def __init__(self,x,y,dir,weights,bias,hidden_neurons):
		super().__init__(self,x,y,dir,weights,bias,hidden_neurons)

	def FOV(self):
		return super().FOV(Prey)


class Predator(Entity):
	FOV_rays = 21 # must be odd
	FOV_range = 500 # pixels
	FOV_width = math.radians(50) # radians
	Color = (200,0,0)

	def __init__(self,x,y,dir,weights,bias,hidden_neurons):
		super().__init__(self,x,y,dir,weights,bias,hidden_neurons)

	def FOV(self):
		return super().FOV(Predator)
		

# functions
def e_dis(entity1,entity2):
	# return distance between 2 entities
	return np.sqrt(np.sum(np.square(np.array((entity1.x,entity1.y)) - np.array((entity2.x,entity2.y)))))

def e_ang(entity1,entity2):
	# return angle between entity1 and entity2 position
	dx = entity2.x-entity1.x
	dy = entity2.y-entity1.y
	return math.atan2(dy,dx)#+entity1.dir

def frame():
	screen.fill((255,255,255))
	Entity.tick = clock.tick(MAX_FPS)/1000
	for entity in entities:
		entity.entity_tick()
	pygame.display.update()



pygame.init()
screen = pygame.display.set_mode((MAP_W,MAP_H))
pygame.display.set_caption('Simulation')
clock = pygame.time.Clock()

entities = np.array([])
prey0 = Prey(MAP_W/2,MAP_H/2,0,0,0,0)
predator0 = Predator(MAP_W/2+100,MAP_H/2+100,PI,0,0,0)
entities = np.append(entities,prey0)
entities = np.append(entities,predator0)

frame()
running = True
while running:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
	frame()