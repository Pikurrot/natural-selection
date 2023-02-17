import pygame
import numpy as np

def visualize(screen,weights,biases,inputs=np.array([]),outputs=np.array([])):
	pygame.font.init()
	gui_x, gui_y = screen.get_size()
	screen.fill((255, 255, 255))

	n_layers = len(weights) + 1
	n_inputs = np.array(weights[0]).shape[0]
	n_outputs = np.array(weights[-1]).shape[1]
	n_neurons = [n_inputs] + [len(l) for l in biases] + [n_outputs]

	x_offset = gui_x / 16
	y_offset = gui_y / 16
	neuron_size = 100 // max(n_neurons)
	max_w_size = neuron_size // 2
	x_sep = (gui_x - x_offset * 2) / max(n_layers - 1, 1)
	y_sep = (gui_y - y_offset * 2) / max(max(n_neurons) - 1, 1)

	x_pos = [[x_offset + x_sep * l for _ in range(n_neurons[l])] for l in range(n_layers)]
	y_pos = [[y_offset + y_sep * n + (y_sep / 2) * (max(n_neurons) - n_neurons[l]) for n in range(n_neurons[l])] for l in range(n_layers)]

	
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
			pygame.draw.circle(screen, (0, 0, 200), (x, y), neuron_size, int((b != 0) * neuron_size * 0.4)) # draw hidden neuron

	
neurons = [21,3,2,2]
weights = [[[0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 1, 0], [0, 0, 0], [0, 0, 0]], [[0, 0], [0, 1], [0, 0]], [[0, 0], [1, 0]]]
biases = [[0, 1, 0], [1, 1], [0, 1]]

inputs = np.array([.5,1,-.3,.2,0])
outputs = np.array([.5,-.3])

pygame.init()
screen = pygame.display.set_mode((400,400))

running = True
while running:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
	visualize(screen,weights,biases)
	pygame.display.update()