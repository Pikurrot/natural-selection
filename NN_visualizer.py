import pygame
import numpy as np

def visualize(screen,weights,biases):
	pygame.font.init()
	gui_x,gui_y = screen.get_size()
	screen.fill((255,255,255))

	n_layers = len(weights)+1
	n_inputs = weights[0].shape[0]
	n_outputs = weights[-1].shape[1]
	n_neurons = [n_inputs]+[len(l) for l in biases]+[n_outputs]

	x_offset = gui_x/20
	y_offset = gui_y/20
	neuron_size = 100//max(n_neurons)
	max_w_size = neuron_size//2
	font = pygame.font.SysFont('Comic Sans MS', neuron_size)
	x_sep = (gui_x-x_offset*2)/max(n_layers-1,1)
	y_sep = (gui_y-y_offset*2)/max(max(n_neurons)-1,1)

	x_pos = [[x_offset+neuron_size+x_sep*l for _ in range(n_neurons[l])] for l in range(n_layers)]
	y_pos = [[y_offset+y_sep*n+(y_sep/2)*(max(n_neurons)-n_neurons[l]) for n in range(n_neurons[l])] for l in range(n_layers)]
	

	for l in range(n_layers-1):
		for n1 in range(n_neurons[l]):
			for n2 in range(n_neurons[l+1]):
				pos1 = (x_pos[l][n1],y_pos[l][n1])
				pos2 = (x_pos[l+1][n2],y_pos[l+1][n2])
				w = weights[l][n1][n2]
				if w == 0:
					pygame.draw.line(screen,(90,90,90),pos1,pos2,1)
				else:
					pygame.draw.line(screen,(225,100,25),pos1,pos2,min((int(w*max_w_size)+1),max_w_size))

	for n in range(n_neurons[0]):
		pygame.draw.circle(screen,(0,0,0),(x_pos[0][n],y_pos[0][n]),neuron_size)
	for n in range(n_neurons[-1]):
		pygame.draw.circle(screen,(0,0,0),(x_pos[-1][n],y_pos[-1][n]),neuron_size)
	for l in range(1,n_layers-1):
		for n in range(n_neurons[l]):
			x,y = x_pos[l][n],y_pos[l][n]
			pygame.draw.circle(screen,(0,0,255),(x,y),neuron_size)
			b = round(biases[l-1][n],2)
			text = font.render(str(b), False, (0,150,0))
			screen.blit(text, text.get_rect(center=(x,y+neuron_size+10)))

	

# neurons = [15,10,13,8,5,3,2,5]
# weights = [np.random.rand(neurons[i],neurons[i+1]) for i in range(len(neurons)-1)]
# biases = [np.random.rand(neurons[i]) for i in range(1,len(neurons)-1)]

# pygame.init()
# screen = pygame.display.set_mode((1000,1000))

# running = True
# while running:
# 	for event in pygame.event.get():
# 		if event.type == pygame.QUIT:
# 			running = False
# 	visualize(screen,weights,biases)
# 	pygame.display.update()