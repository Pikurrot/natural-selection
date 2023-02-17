import numpy as np
import pygame
from math import pi, sin, cos, sqrt, atan2
from abc import ABC, abstractmethod

def sign(x):
	if x == 0: return 0
	return int(x / abs(x))

def get_bits(byte):
	byte = byte[2:]
	while len(byte) < 8:
		byte = "0" + byte
	return byte

class Actuator(ABC):
	def __init__(self, left, top, w, h, command, visible):
		self.original_x = left
		self.original_y = top
		self.original_w = w
		self.original_h = h
		self.visible = visible
		self.val = 0
		self.command = command
		self.area = pygame.Rect(left, top, w, h)

	def mouse_collision(self,mouse_pos):
		# checks if mouse is colliding with the actuator
		return self.area.collidepoint(mouse_pos)
	
	def move(self, new_left, new_top):
		# move rectangle to new position
		self.area.move_ip(new_left - self.area.x, new_top - self.area.y)

	def update_to_ratio(self, rw, rh):
		# update x, y, w and heigth given ratio
		self.move(self.original_x * rw, self.original_y * rh)
		self.area.inflate_ip(max(6, self.original_w * rw) - self.area.w, max(6, self.original_h * rh) - self.area.h)

	@abstractmethod
	def actuate(self, mouse_pos):
		return NotImplementedError
	
	@abstractmethod
	def show(self, screen):
		return NotImplementedError


class Button(Actuator):
	def __init__(self, left, top, w, h, command, visible = True, sprites = ()):
		super().__init__(left, top, w, h, command, visible)
		self.original_sprites = tuple([pygame.transform.scale(sprite, (int(w), int(h))) for sprite in sprites])
		self.sprites = list(sprites)

	def update_to_ratio(self, rw, rh):
		super().update_to_ratio(rw, rh)
		for i in range(len(self.sprites)):
			original_size = self.original_sprites[i].get_size()
			self.sprites[i] = pygame.transform.scale(self.original_sprites[i], (int(original_size[0] * rw), int(original_size[1] * rh)))

	def actuate(self, mouse_pos = None):
		pass

	def show(self, screen):
		if self.visible:
			if len(self.sprites) == 0:
				pass
			elif len(self.sprites) == 1:
				screen.blit(self.sprites[0], (self.area.x, self.area.y))
			else:
				screen.blit(self.sprites[self.val], (self.area.x, self.area.y))

class Slider(Actuator):
	def __init__(self, left, top, w, h, slider_w, command, min_val, max_val, visible = True):
		super().__init__(left, top, w, h, command, visible)
		self.original_slider_w = slider_w
		self.min_val = min_val
		self.max_val = max_val
		self.slider = pygame.Rect(self.area.x - slider_w / 2, self.area.y, slider_w, self.area.h)
	
	def move(self, new_left, new_top):
		# moves the slider together with the rectangle
		self.slider.move_ip(new_left - self.area.x, new_top - self.area.y)
		super().move(new_left, new_top)

	def update_to_ratio(self, rw, rh):
		super().update_to_ratio(rw, rh)
		self.slider.inflate_ip(max(4, self.original_slider_w * rw) - self.slider.w, max(4, self.area.h) - self.slider.h)

	def actuate(self, mouse_pos):
		# returns the new val of the slider [0 - 1]
		self.val = min(max(0, (mouse_pos[0] - self.area.x) / self.area.w), 1) * (self.max_val - self.min_val) + self.min_val
		self.update_pos()

	def update_pos(self):
		# updates slider pos according to val
		slider_x = self.area.x - self.slider.w / 2 + self.area.w / (self.max_val - self.min_val) * (self.val - self.min_val)
		self.slider.move_ip(slider_x - self.slider.x, 0)
	
	def show(self, screen):
		# draws the slider in the screen if visible
		if self.visible:
			line_y = self.area.y + self.area.h / 2
			pygame.draw.line(screen, (0, 0, 0), (self.area.x, line_y), (self.area.x + self.area.w, line_y), int(self.area.h/6))
			pygame.draw.rect(screen, (0, 0, 0), self.slider)


def draw_map(tick):
	# draws entities to the map
	rm = min(rw, rh)
	screen.fill((255, 255, 255))
	preys_data = data[tick]["preys"]
	predators_data = data[tick]["predators"]
	for x,y in preys_data:
		pygame.draw.circle(screen, (0, 255, 0), (x * rw, y * rh), max(1, entity_size * rm))
	for x,y in predators_data:
		pygame.draw.circle(screen, (255, 0, 0), (x * rw, y * rh), max(1, entity_size * rm))

def draw_interface(tick):
	# draws interface
	rm = min(rw, rh)
	screen_w = START_SCREEN_W * rw
	screen_h = START_SCREEN_H * rh
	menu_w = START_MENU_W * rw
	menu_h = START_MENU_H * rh
	menu_color = (40, 150, 190)
	font = pygame.font.SysFont("consolas", int(max(1, 30 * rm)))
	pygame.draw.rect(screen, menu_color, pygame.Rect(0, screen_h - menu_h, screen_w - menu_w, menu_h))
	pygame.draw.rect(screen, menu_color, pygame.Rect(screen_w - menu_w, 0, menu_w, screen_h))

	text = font.render("tick : " + str(tick), True, (0, 0, 0))
	screen.blit(text, (screen_w - menu_w - text.get_width() - 50 * rw, screen_h - 75 * rh))

	# actuators
	for actuator in actuators:
		actuator.update_to_ratio(rw, rh)
		actuator.show(screen)

	# entity info
	if len(selected_entity) != 0:
		# bounding box & direction
		if not selected_entity["dead"]:
			offset = 3
			frame_x = (selected_entity["x"] - entity_size - offset) * rw
			frame_y = (selected_entity["y"] - entity_size - offset) * rh
			pygame.draw.rect(screen, menu_color, pygame.Rect(frame_x, frame_y, (entity_size + offset) * 2 * rw, (entity_size + offset) * 2 * rh), int(max(1, 3 * rm)))

		# neural network
		rect_w = (START_MENU_W - 100) * rw
		rect_h = (START_MENU_W - 100) * rh
		rect_x = screen_w - menu_w + 50 * rw
		rect_y = 50 * rh
		pygame.draw.rect(screen, (0, 0, 0), pygame.Rect(rect_x, rect_y, rect_w, rect_h), int(max(1, 5 * min(rw, rh))))

		weights = brains_data[selected_entity["id"]][0]
		biases = brains_data[selected_entity["id"]][1]
		n_layers = len(weights) + 1
		n_inputs = np.array(weights[0]).shape[0]
		n_outputs = np.array(weights[-1]).shape[1]
		n_neurons = [n_inputs] + [len(l) for l in biases] + [n_outputs]

		x_offset = rect_w / 16
		y_offset = rect_h / 16
		neuron_size = int(100 / max(n_neurons) * rm)
		max_w_size = neuron_size // 2
		x_sep = (rect_w - x_offset * 2) / max(n_layers - 1, 1)
		y_sep = (rect_h - y_offset * 2) / max(max(n_neurons) - 1, 1)

		x_pos = [[rect_x + x_offset + x_sep * l for _ in range(n_neurons[l])] for l in range(n_layers)]
		y_pos = [[rect_y + y_offset + y_sep * n + (y_sep / 2) * (max(n_neurons) - n_neurons[l]) for n in range(n_neurons[l])] for l in range(n_layers)]
		
			# draw weigths
		for l in range(n_layers - 1):
			for n1 in range(n_neurons[l]):
				for n2 in range(n_neurons[l + 1]):
					pos1 = (x_pos[l][n1], y_pos[l][n1])
					pos2 = (x_pos[l+1][n2], y_pos[l + 1][n2])
					w = weights[l][n1][n2]
					if w != 0:
						pygame.draw.line(screen, (250, 180, 0), pos1, pos2, max(min((int(round(w * max_w_size))), max_w_size), 1))

			# draw input neurons
		for n in range(n_neurons[0]):
			pygame.draw.circle(screen, (0, 0, 0), (x_pos[0][n], y_pos[0][n]), neuron_size)

			# draw rest of neurons
		for l in range(1, n_layers):
			for n in range(n_neurons[l]):
				x, y = x_pos[l][n], y_pos[l][n]
				b = biases[l-1][n]
				pygame.draw.circle(screen, (0, 0, 200), (x, y), neuron_size, int((b == 0) * neuron_size * 0.4))
		
		# info
		for i, (string, value) in enumerate(selected_entity.items()):
			if i < 1 or value == None: continue
			if type(value) == float:
				value = round(value, 2)
			text = font.render(string + " : " + str(value), True, (0, 0, 0))
			screen.blit(text, (rect_x, rect_y + rect_h + 40 * rh * i))

	# population graphs
	rect_w = (START_MENU_W - 100) * rw
	rect_h = 130 * rh
	rect_x = screen_w - menu_w + 50 * rw
	rect_y_preys = screen_h - rect_h * 2 - 100 * rh
	rect_y_predators = screen_h - rect_h - 75 * rh
	edge_w = int(max(1, 5 * min(rw, rh)))
	
	populations = np.array([[len(tick["preys"]), len(tick["predators"])] for tick in data[max(0, tick - show_ticks) : tick + 1]])
	try:
		scaled_populations = populations / np.max(populations) * 0.8
	except ZeroDivisionError:
		scaled_populations = np.zeros(populations.shape)
	preys_population, predators_population = scaled_populations[:,0], scaled_populations[:,1]
	preys_points = [(rect_x + rect_w * (i + 1) / len(preys_population), rect_y_preys + rect_h * (1 - val)) for i, val in enumerate(preys_population)] \
		+ [(rect_x + rect_w, rect_y_preys + rect_h), (rect_x, rect_y_preys + rect_h), (rect_x, rect_y_preys + rect_h * (1 - preys_population[0]))]
	predators_points = [(rect_x + rect_w * (i + 1) / len(predators_population), rect_y_predators + rect_h * (1 - val)) for i, val in enumerate(predators_population)] \
		+ [(rect_x + rect_w, rect_y_predators + rect_h), (rect_x, rect_y_predators + rect_h), (rect_x, rect_y_predators + rect_h * (1 - predators_population[0]))]

	if np.max(preys_population) != 0: pygame.draw.polygon(screen, (0, 255, 0), preys_points)
	if np.max(predators_population) != 0: pygame.draw.polygon(screen, (255, 0, 0), predators_points)
	pygame.draw.rect(screen, (0, 0, 0), pygame.Rect(rect_x - edge_w, rect_y_preys - edge_w, rect_w + edge_w * 2, rect_h + edge_w * 2), edge_w)
	pygame.draw.rect(screen, (0, 0, 0), pygame.Rect(rect_x - edge_w, rect_y_predators - edge_w, rect_w + edge_w * 2, rect_h + edge_w * 2), edge_w)

	text = font.render(str(show_ticks), True, (0, 0, 0))
	screen.blit(text, (screen_w - menu_w + 270 * rw, rect_y_predators + rect_h + 17 * rh))

def update_screen(tick):
	# draws map and interface
	draw_map(tick)
	draw_interface(tick)
	pygame.display.update()

file_num = "2"
# read raw data
with open("simulations/simulation" + file_num + ".bin", "rb") as sim_file:
	binary_data = sim_file.read()

with open("simulations/neural_nets" + file_num + ".bin", "rb") as brains_file:
	brains_binary_data = brains_file.read()

# preprocess raw data
flat_data = np.frombuffer(binary_data, dtype=np.int16).astype(np.int32)

# format data
map_w, map_h, entity_size, entity_max_spd, n_hidden, n_prey_inputs, n_predator_inputs, n_outputs = flat_data[:8]
n_hidden_neurons = list(flat_data[8 : 8 + n_hidden])
n_inputs = {"preys" : n_prey_inputs, "predators" : n_predator_inputs}

data = []
demography = []
deaths = []
n_preys = 0
n_predators = 0
n = 2 # nº of parameters per entity
i = 8 + n_hidden
while i < len(flat_data):
	prey_births, prey_deaths, predator_births, predator_deaths = flat_data[i:i+4]
	n_preys = n_preys + prey_births - prey_deaths
	n_predators = n_predators + predator_births - predator_deaths
	i += 4

	dead_preys = flat_data[i : i + prey_deaths]
	i += prey_deaths
	dead_predators = flat_data[i : i + predator_deaths]
	i += predator_deaths

	preys_data = flat_data[i : i + n_preys * n]
	preys_data = [(preys_data[i], preys_data[i+1]) for i in range(0, len(preys_data) - 1, 2)]
	i += n_preys * n

	predators_data = flat_data[i : i + n_predators * n]
	predators_data = [(predators_data[i], predators_data[i + 1]) for i in range(0, len(predators_data) - 1, 2)]
	i += n_predators * n

	data.append({"preys": preys_data, "predators": predators_data})
	demography.append({"preys" : (prey_births, prey_deaths), "predators" : (predator_births, predator_deaths)})
	deaths.append({"preys" : dead_preys, "predators" : dead_predators})

# compute entities id
i = 0
entities_id = []
for tick in range(len(data)):
	try:
		entities_id.append({})
		for entity_type in ("preys", "predators"):
			if tick == 0: 
				entities_id[tick][entity_type] = []
			else: 
				entities_id[tick][entity_type] = entities_id[tick - 1][entity_type][:]
			births = demography[tick][entity_type][0]
			dead_entities = deaths[tick][entity_type]
			for j in range(births):
				entities_id[tick][entity_type].append(i)
				i += 1	
			for j in sorted(dead_entities, reverse = True):
				del entities_id[tick][entity_type][j]
				
	except:
		MAX_TICKS = tick - 1
		print("Error reading entities data at tick ", tick)
		break
	else:
		MAX_TICKS = len(data) - 1


# format brains data
i = 0
j = 0
brains_data = {}
for tick in range(len(data)):
	try:
		for entity_type in ("preys", "predators"):
			layers_neurons = [n_inputs[entity_type]] + n_hidden_neurons + [n_outputs]
			births = demography[tick][entity_type][0]
			for birth in range(births):
				# extract one neural network
				weights = []
				biases = []
				for layer in range(1, len(layers_neurons)):
					# extract layer weigths
					weights.append([])
					layer_w = ""
					for _ in range(4 * 4):
						layer_w += get_bits(bin(brains_binary_data[j]))[::-1]
						j += 1
					n = layers_neurons[layer]
					for _ in range(layers_neurons[layer - 1]):
						weights[-1].append([int(bit) for bit in layer_w[:n]])
						layer_w = layer_w[n:]
				
				for layer in range(1, len(layers_neurons)):
					# extract layer biases
					layer_b = ""
					for _ in range(4):
						layer_b += get_bits(bin(brains_binary_data[j]))[::-1]
						j += 1
					n = layers_neurons[layer]
					biases.append([int(bit) for bit in layer_b[:n]])
				brains_data[i] = (weights, biases)
				i += 1
	except:
		MAX_TICKS = tick - 1
		print("Error reading brains data at tick ", tick)
		break

# simulation parameters
MAX_SPEED = 200 # includes negative val
MAX_FPS = 60 # PC max FPS
FPS = 20 # default FPS
sim_speed = 1
pause = False

# interface
show_ticks = 500
rw, rh = 0.5, 0.5
START_MENU_W = 400
START_MENU_H = 200
START_SCREEN_W = map_w + START_MENU_W
START_SCREEN_H = map_h + START_MENU_H

actuators = []
actuators.append(Slider(50,										START_SCREEN_H - START_MENU_H + 50,		START_SCREEN_W - START_MENU_W - 100,		30, 	10,		0, 		0,		MAX_TICKS))
actuators.append(Slider(300,									START_SCREEN_H - START_MENU_H + 120, 	200, 										30, 	10, 	1,		1,		MAX_SPEED))
actuators.append(Button(50, 									START_SCREEN_H - START_MENU_H + 110,	50, 										50,				2, 		sprites = (pygame.image.load("python_files/images/pause.png"),pygame.image.load("python_files/images/play.png"))))
actuators.append(Button(110, 									START_SCREEN_H - START_MENU_H + 110,	50, 										50,				3, 		sprites = (pygame.image.load("python_files/images/next.png"),pygame.image.load("python_files/images/previous.png"))))
actuators.append(Button(170, 									START_SCREEN_H - START_MENU_H + 110, 	50, 										50, 			4, 		sprites = (pygame.image.load("python_files/images/forward.png"), pygame.image.load("python_files/images/backward.png"))))
actuators.append(Slider(START_SCREEN_W - START_MENU_W + 50,		START_SCREEN_H - START_MENU_H + 140, 	200, 										30, 	10, 	5,		10,		5000))

selected_entity = {}

# initiate simulation
pygame.init()
screen = pygame.display.set_mode((int(START_SCREEN_W * rw), int(START_SCREEN_H * rh)), pygame.RESIZABLE)
pygame.display.set_caption('Simulation')
clock = pygame.time.Clock()

# run simulation
running = True
tick = 0
current_actuator = None
while running:
	# update parameters
	mouse_pos = pygame.mouse.get_pos()
	m = FPS * sim_speed
	rm = min(rw, rh)

	# check pygame events
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
		elif event.type == pygame.VIDEORESIZE:
			screen = pygame.display.set_mode(event.size, pygame.RESIZABLE)
			rw = event.size[0] / START_SCREEN_W
			rh = event.size[1] / START_SCREEN_H		
		elif event.type == pygame.MOUSEBUTTONDOWN:
			for actuator in actuators:
				if actuator.mouse_collision(mouse_pos):
					current_actuator = actuator
					break
			found = False
			if mouse_pos[0] <= map_w * rw and mouse_pos[1] <= map_h * rh:
				for entity_type, positions in data[int(tick)].items():
					for i, (x, y) in enumerate(positions):
						if (abs(x - mouse_pos[0] / rw) <= entity_size) and (abs(y - mouse_pos[1] / rh) <= entity_size):
							selected_entity["id"] = entities_id[int(tick)][entity_type][i]
							selected_entity["dead"] = False
							selected_entity["type"] = entity_type
							selected_entity["x"] = x
							selected_entity["y"] = y
							selected_entity["speed"] = None
							found = True
							break
					if found: break
				if not found: selected_entity = {}

		elif event.type == pygame.MOUSEBUTTONUP:
			current_actuator = None

	# selected entity tracking
	if len(selected_entity) != 0:
		try:
			i = entities_id[int(tick)][selected_entity["type"]].index(selected_entity["id"])
			selected_entity["dead"] = False

			try:
				reference_tick = int(tick) + 1 # always should compare with next tick
				reference_i = entities_id[reference_tick][selected_entity["type"]].index(selected_entity["id"])
			except:
				reference_tick = int(tick) - 1 # except when it exist no next tick
				reference_i = entities_id[reference_tick][selected_entity["type"]].index(selected_entity["id"])
			
			reference_x, reference_y = data[reference_tick][selected_entity["type"]][reference_i]
			x_dis = reference_x - selected_entity["x"]
			y_dis = reference_y - selected_entity["y"]
			entity_speed = sqrt(abs(x_dis) ** 2 + abs(y_dis) ** 2)
			entity_speed_x_wall = sqrt((abs(x_dis) - map_w) ** 2 + abs(y_dis) ** 2)
			entity_speed_y_wall = sqrt(abs(x_dis) ** 2 + (abs(y_dis) - map_h) ** 2)
			entity_speed = min(entity_speed, entity_speed_x_wall, entity_speed_y_wall)
			
			selected_entity["speed"] = entity_speed
			selected_entity["x"], selected_entity["y"] = data[int(tick)][selected_entity["type"]][i]

		except ValueError:
			selected_entity["dead"] = True

	# update screen and user interaction
	update_screen(int(tick))
	if current_actuator != None:
		current_actuator.actuate(mouse_pos)
		if current_actuator.command == 0: # tick slider
			tick = current_actuator.val
		elif current_actuator.command == 1: # sim speed slider
			sim_speed = current_actuator.val * sign(sim_speed)
		elif current_actuator.command == 2: # pause / play
			pause = not pause
			current_actuator = None
		elif current_actuator.command == 3: # next / previous
			tick = min(max(0, tick + sign(sim_speed)), MAX_TICKS)
			current_actuator = None
		elif current_actuator.command == 4: # forward / backward
			sim_speed = -sim_speed
			current_actuator = None
		elif current_actuator.command == 5: # graphs show ticks
			show_ticks = int(current_actuator.val)
	
	# set next tick
	if not pause and (current_actuator == None or current_actuator.command != 0):
		if abs(m) > MAX_FPS:
			tick += m / MAX_FPS
			clock.tick(MAX_FPS)
		else:
			tick += (m != 0) * sign(m)
			clock.tick(abs(m))
		tick = min(max(0, tick), MAX_TICKS)
	
	# set actuators val	(in case the external value changes but not theirs)
	for actuator in actuators:
		if actuator.command == 0: 
			actuator.val = tick
			actuator.update_pos()
		elif actuator.command == 1: 
			actuator.val = abs(sim_speed)
			actuator.update_pos()
		elif actuator.command == 2: 
			actuator.val = pause
		elif actuator.command == 3:
			actuator.val = (sim_speed < 0)
		elif actuator.command == 4:
			actuator.val = (sim_speed < 0)
		elif actuator.command == 5: 
			actuator.val = int(show_ticks)
			actuator.update_pos()

# 24h of simulation = 5h running the c++ program = 1,700,000 ticks = 1.2 GB