#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import sys

moves = []
for line in sys.stdin:
	moves.append(tuple(np.reshape(np.array(line.split()).astype(int), (-1,3)).T))

ax = plt.figure().add_subplot(projection='3d')
for i in range(len(moves)):
	plt.cla()

	# A bit unnecessary but split up old and new voxels into separate calls so that the new ones are visible when occluded by old
	old = np.zeros((4,4,4,4), dtype=np.float32)
	for move in moves[:i]:
		old[move] = (0,0,0,0.5)
	ax.voxels(np.any(old, axis=3), facecolors=old, edgecolor='k')

	new = np.zeros((4,4,4,4), dtype=np.float32)
	new[moves[i]] = (1,0,0,0.5)
	ax.voxels(np.any(new, axis=3), facecolors=new, edgecolor='k')

	ax.set_axis_off()
	plt.draw()
	plt.pause(0.25)

plt.show()
