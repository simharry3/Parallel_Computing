#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from mpl_toolkits.mplot3d import Axes3D
# from mayavi import mlab
import os
import sys
import pylab as lab
import subprocess
import math as ma
import timeit


input = sys.argv[1]
g = open(input, 'r')
x = []
y = []
z = []
cond = []

conditions = {"Collided", "Active", "Aggregator"}
colors = {"Collided":"b", "Active":"0.5", "Aggregator":"r"}

for line in g:
 ln = line.split(' ')
 x.append(int(ln[0]))
 y.append(int(ln[1]))
 z.append(int(ln[2]))
 cond.append(int(ln[3]))

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

for i in range(0, len(x)):
    xV = x[i]
    yV = y[i]
    zV = z[i]
    colorTag = colors["Active"]
    if(cond[i] == 10):
        colorTag = colors["Collided"]

    if(cond[i] == 9):
        colorTag = colors["Aggregator"]

    ax.scatter(xV,yV, zV, c=colorTag, marker='o', s=380, depthshade=True)

patch = []
for key in colors:
    patch.append(mpatches.Patch(label=key + " Particles", color=colors[key]))

plt.legend(handles=patch, bbox_to_anchor=[1.1, 0.1])

plt.title("Accumulation Structure of Full Diffusion Limited Aggregation Simulation")

ax.set_xlim3d([min(x) - 1,max(x) + 1])
ax.set_ylim3d([min(y) - 1,max(y) + 1])
ax.set_zlim3d([min(z) - 1,max(z) + 1])
ax.view_init(elev=20, azim=225)
plt.show()