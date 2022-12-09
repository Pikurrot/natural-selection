import numpy as np
import math
from math import sin,cos,radians
import pygame
import random
import copy
import NN_visualizer

# Constants
PI = math.pi
E = math.e

MAP_W = 500
MAP_H = 350
MAX_FPS = 60
MAX_PREYS = 100
MAX_PREDATORS = 50
STARTING_PREYS = 40
STARTING_PREDATORS = 10

class Entity:
	max_spd = 100
	max_ang_spd = radians(30) # radians
	max_energy = 100
	max_split_charge = 100
	size = 10
	move_impulse = True
	max_tmp_spd = max_spd
	temp_spd_dropping = max_spd
	r_energy_spd = 0.1 # energy dropped per spd unit
	split_impulse = max_tmp_spd*2
	tick = 0 # sec

	def __init__(self,x,y,dir,brain,temp_spd=0,temp_dir=0):
		# kinematic state
		self._x = x # pixels
		self._y = y # pixels
		self._dir = dir # rad (between PI and -PI)
		self.temp_dir = temp_dir # rad (between PI and -PI)
		# dynamic state
		self._spd = 0 # pixels/sec
		self._ang_spd = 0 # rad/sec
		self.temp_spd = temp_spd # pixels/sec
		# entity variables
		self._energy = Entity.max_energy
		self._split_charge = 0
		# brain
		self._brain = brain
	
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
	@property
	def temp_dir(self):
		return self._temp_dir
	@property
	def spd(self):
		return self._spd
	@property
	def ang_spd(self):
		return self._ang_spd
	@property
	def temp_spd(self):
		return self._temp_spd
	@property
	def energy(self):
		return self._energy
	@property
	def split_charge(self):
		return self._split_charge

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
		if dir > PI: self._dir = dir - 2*PI
		elif dir < -PI: self._dir = dir + 2*PI
		else: self._dir = dir
	@temp_dir.setter
	def temp_dir(self,temp_dir):
		if temp_dir > PI: self._temp_dir = temp_dir - 2*PI
		elif temp_dir < -PI: self._temp_dir = temp_dir + 2*PI
		else: self._temp_dir = temp_dir
	@spd.setter
	def spd(self,spd):
		if spd < -Entity.max_spd: self._spd = -Entity.max_spd
		elif spd > Entity.max_spd: self._spd = Entity.max_spd
		else: self._spd = spd
	@ang_spd.setter
	def ang_spd(self,ang_spd):
		if ang_spd < -Entity.max_ang_spd: self._ang_spd = -Entity.max_ang_spd
		elif ang_spd > Entity.max_ang_spd: self._ang_spd = Entity.max_ang_spd
		else: self._ang_spd = ang_spd
	@temp_spd.setter
	def temp_spd(self,temp_spd):
		if temp_spd < -Entity.max_tmp_spd: self._temp_spd = -Entity.max_tmp_spd
		elif temp_spd > Entity.max_tmp_spd: self._temp_spd = Entity.max_tmp_spd
		else: self._temp_spd = temp_spd
	@energy.setter
	def energy(self,energy):
		if energy < 0: self._energy = 0
		elif energy > Entity.max_energy: self._energy = Entity.max_energy
		else: self._energy = energy
	@split_charge.setter
	def split_charge(self,split_charge):
		if split_charge < 0: self._split_charge = 0
		elif split_charge > Entity.max_split_charge: self._split_charge = Entity.max_split_charge
		else: self._split_charge = split_charge

	# methods
	def move(self):
		# move entity x, y and dir
		self.dir += self._ang_spd*Entity.tick
		if Entity.move_impulse:
			if self.temp_spd == 0:
				self.temp_spd = self.spd
				self.temp_dir = self.dir
			elif self.temp_spd > 0:
				self.temp_spd -= min(Entity.temp_spd_dropping * Entity.tick, self.temp_spd)
			else:
				self.temp_spd += min(Entity.temp_spd_dropping * Entity.tick, -self.temp_spd)
		else:
			self.temp_spd = self.spd
		spd_x = self.temp_spd*cos(self.temp_dir)
		spd_y = self.temp_spd*sin(self.temp_dir)
		self.x += spd_x*Entity.tick
		self.y += spd_y*Entity.tick

	def show(self):
		# show entity in screen
		pygame.draw.circle(screen,type(self).color,(self.x,self.y),Entity.size)
		pygame.draw.line(screen,type(self).color,(self.x,self.y),(self.x+cos(self.dir)*Entity.size*1.5,self.y+sin(self.dir)*Entity.size*1.5))
		# pygame.draw.circle(screen,(150,100,0),(self.x-10,self.y-10),Entity.size*(self.spd/Entity.max_spd))
		# pygame.draw.circle(screen,(0,0,100),(self.x+10,self.y-10),Entity.size*abs(self.ang_spd/Entity.max_ang_spd))

	def FOV_entities(self,other_class,fov_range=None,fov_width=None):
		# returns array of entities inside FOV
		if fov_range == None: fov_range = type(self).FOV_range
		if fov_width == None: fov_width = type(self).FOV_width
		return np.array([entity for entity in entities if 
			(type(entity) == other_class and e_dis(self,entity) < fov_range and (abs(e_ang(self,entity)-self.dir) < fov_width/2))])

	def FOV(self,other_class):
		# returns array with activation values (0.0-1.0) of every ray in the Field Of View
		half_rays = type(self).FOV_rays//2
		rays_sep = type(self).FOV_width / type(self).FOV_rays
		ray_angs = [ray * rays_sep + self.dir for ray in range(-half_rays,half_rays+1)]
		ray_angs = np.array([ray-2*PI if ray > PI else (ray+2*PI if ray < -PI else ray) for ray in ray_angs])
		ray_strenghts = np.zeros(ray_angs.shape)
		for entity in self.FOV_entities(other_class):
			dis = e_dis(self,entity)
			e_ang_ = e_ang(self,entity)
			e_ray_activations = np.array([(abs(dis*sin(e_ang_-ang)) <= Entity.size) and (abs(e_ang_-ang)<rays_sep) for ang in ray_angs])
			e_ray_strenghts = e_ray_activations*(1-dis/type(self).FOV_range)
			ray_strenghts = np.maximum(ray_strenghts,e_ray_strenghts)
		# for ang,strenght in zip(ray_angs,ray_strenghts):
		# 	ray_x = self.x + type(self).FOV_range * cos(ang)
		# 	ray_y = self.y + type(self).FOV_range * sin(ang)
		# 	pygame.draw.line(screen, (0,0,255), (self.x,self.y), (ray_x,ray_y), max(0,int(strenght*30)))
		return ray_strenghts

	def split(self):
		# create a clone entity and both impulse to opposite directions
		if (type(self) == Prey and Prey.count == MAX_PREYS) or (type(self) == Predator and Predator.count == MAX_PREDATORS): return
		split_direction = radians(random.randint(-180,179))
		new_brain = copy.deepcopy(self._brain)
		new_brain.mutate()
		entities.append(type(self)(self.x,self.y,self.dir,new_brain,Entity.split_impulse,split_direction))
		self.temp_spd = Entity.split_impulse
		self.temp_dir = split_direction - PI

	def update_properties(self):
		o1,o2 = self._brain.outputs(self.FOV())
		self.spd = round(o1*Entity.max_spd,3)
		self.ang_spd = round(o2*Entity.max_ang_spd,3)
		self.energy -= self.spd*Entity.r_energy_spd*Entity.tick
		if self.split_charge == Entity.max_split_charge:
			self.split()
			self.split_charge = 0

	def entity_tick(self):
		self.update_properties()
		self.move()
		self.show()


class Prey(Entity):
	count = 0
	FOV_rays = 21 # must be odd
	FOV_range = 250 # pixels
	FOV_width = radians(300) # radians
	color = (0,200,0)
	minimum_rest = Entity.max_energy/8
	energy_recovery = Entity.max_energy/20
	split_recharge = Entity.max_split_charge/10

	def __init__(self,x,y,dir,brain,temp_spd=0,temp_dir=0):
		super().__init__(x,y,dir,brain,temp_spd,temp_dir)
		self.rest = False
		Prey.count += 1

	# methods
	def show(self):
		return super().show()

	def FOV(self):
		return super().FOV(Predator)
	
	def update_properties(self):
		super().update_properties()
		if self.energy == 0: 
			self.spd = 0
			self.rest = True
		elif self.energy >= Prey.minimum_rest: self.rest = False
		if self.spd == 0: self.energy += self.energy_recovery * Entity.tick
		self.split_charge += Prey.split_recharge * Entity.tick

	def __del__(self):
		Prey.count -= 1


class Predator(Entity):
	count = 0
	FOV_rays = 21 # must be odd
	FOV_range = 500 # pixels
	FOV_width = radians(50) # radians
	color = (200,0,0)
	max_digest_charge = 10
	digest_dropping = max_digest_charge/1
	energy_recovery = Entity.max_energy/3
	energy_dropping = Entity.max_energy/40
	split_recharge = Entity.max_split_charge/2
	split_dropping = Entity.max_split_charge/30
	max_eat_dis = Entity.size*1.5
	max_eat_ang = radians(120)

	def __init__(self,x,y,dir,brain,temp_spd=0,temp_dir=0):
		super().__init__(x,y,dir,brain,temp_spd,temp_dir)
		self._digest_charge = 0
		Predator.count += 1

	# getters
	@property
	def digest_charge(self):
		return self._digest_charge
	# setters
	@digest_charge.setter
	def digest_charge(self,digest_charge):
		if digest_charge < 0: self._digest_charge = 0
		elif digest_charge > Predator.max_digest_charge: self._digest_charge = Predator.max_digest_charge
		else: self._digest_charge = digest_charge

	# methods
	def show(self):
		return super().show()

	def FOV(self):
		return super().FOV(Prey)

	def eat(self,prey):
		# eats the given prey
		entities.remove(prey)
		if self.digest_charge == 0:
			self.energy += Predator.energy_recovery
			self.split_charge += self.split_recharge
		self.digest_charge = self.max_digest_charge

	def eat_nearby_prey(self):
		# eats the most nearby prey in its direction
		if self.spd == 0: return
		fov_preys = self.FOV_entities(Prey,Predator.max_eat_dis,Predator.max_eat_ang)
		if len(fov_preys) == 0: return
		#print(fov_preys)
		distances = np.array([e_dis(self,prey) for prey in fov_preys])
		self.eat(fov_preys[np.argmin(distances)])


	def update_properties(self):
		super().update_properties()
		self.energy -= Predator.energy_dropping * Entity.tick
		if self.energy == 0: entities.remove(self)
		self.digest_charge -= Predator.digest_dropping * Entity.tick
		self.split_charge -= Predator.split_dropping * Entity.tick

	def entity_tick(self):
		super().entity_tick()
		self.eat_nearby_prey()

	def __del__(self):
		Predator.count -= 1


class NeuralNetwork:
	n_neurons = np.array([3]) # neurons x layer
	n_layers = n_neurons.shape[0]

	def __init__(self,n_inputs,weights=None,biases=None):
		self._n_inputs = n_inputs
		if weights == None:
			self._weights = list((0,)*(NeuralNetwork.n_layers+1))
			self._weights[0] = np.zeros((self._n_inputs,NeuralNetwork.n_neurons[0]))
			self._weights[-1] = np.zeros((NeuralNetwork.n_neurons[-1],2))
			for l in range(NeuralNetwork.n_layers-1):
				self._weights[l+1] = np.zeros((NeuralNetwork.n_neurons[l],NeuralNetwork.n_neurons[l+1]))
		else: self._weights = weights
		if biases == None:
			self._biases = list((0,)*(NeuralNetwork.n_layers+1))
			self._biases[-1] = np.zeros((2,))
			for l in range(NeuralNetwork.n_layers):
				self._biases[l] = np.zeros((NeuralNetwork.n_neurons[l],))
		else: self._biases = biases

	def outputs(self,inputs):
		# forward propagation. Returns a (2,) matrix with outputs ranging (0.0 - 1.0)
		outputs = activation_function1(np.dot(inputs,self._weights[0])+self._biases[0])
		for l in range(1,NeuralNetwork.n_layers+1):
			outputs = np.dot(outputs,self._weights[l])+self._biases[l]
			if l != NeuralNetwork.n_layers: outputs = activation_function1(outputs)
		outputs = activation_function2(outputs)
		return np.around(outputs,3)

	def randomize_weigth(self,layer,row,col):
		# make random the weight at a given position
		self._weights[layer][row][col] = np.around(random.uniform(-1,1),3)

	def randomize_bias(self,layer,row):
		# make random the bias at a given position
		self._biases[layer][row] = np.around(random.uniform(-1,1),3)

	def mutate(self):
		# mutate a random weight or the bias
		p = (.6,.2,.2)
		r = random.choices(range(len(p)),p)[0]
		if r == 0: 
			layer = random.randint(0,NeuralNetwork.n_layers)
			row = random.randint(0,self._weights[layer].shape[0]-1)
			col = random.randint(0,self._weights[layer].shape[1]-1)
			self.randomize_weigth(layer,row,col)
		elif r == 1:
			layer = random.randint(0,NeuralNetwork.n_layers)
			row = random.randint(0,self._biases[layer].shape[0]-1)
			self.randomize_bias(layer,row)
		else:
			n = random.randint(0,self._weights[0].shape[0]-1)
			for layer in range(NeuralNetwork.n_layers+1):
				col = random.randint(0,self._weights[layer].shape[1]-1)
				self.randomize_weigth(layer,n,col)
				n = col


# functions
def e_dis(entity1,entity2):
	# return distance between 2 entities
	return np.sqrt(np.sum(np.square(np.array((entity1.x,entity1.y)) - np.array((entity2.x,entity2.y)))))

def e_ang(entity1,entity2):
	# return angle between entity1 and entity2 position
	dx = entity2.x-entity1.x
	dy = entity2.y-entity1.y
	return math.atan2(dy,dx)

def activation_function1(x):
	return 1/(1+E**x)

def activation_function2(x):
	return (E**x-E**(-x))/(E**x+E**(-x))

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

entities = []
random_x = np.random.uniform(Entity.size, MAP_W-Entity.size, size=STARTING_PREYS+STARTING_PREDATORS)
random_y = np.random.uniform(Entity.size, MAP_H-Entity.size, size=STARTING_PREYS+STARTING_PREDATORS)

i = 0
while i < STARTING_PREYS:
	entities.append(Prey(random_x[i],random_y[i],radians(random.randint(-180,179)),NeuralNetwork(Prey.FOV_rays)))
	i += 1
while i < STARTING_PREYS+STARTING_PREDATORS:
	entities.append(Predator(random_x[i],random_y[i],radians(random.randint(-180,179)),NeuralNetwork(Predator.FOV_rays)))
	i += 1

frame()
running = True
while running:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
	frame()