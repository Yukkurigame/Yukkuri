#!/usr/bin/python
import sys
import matplotlib.pyplot as plt

infile = sys.argv[1]

with open(infile) as inf:
    points = []
    lines = []
    for line in inf:
        line = line.rstrip('\n')
        if line.find('line') == 0:
           plt.plot(*zip(*[x.split(':') for x in line.split()[1:]]), color="black")
        else:
            points.append(line.split())
    if len(points):
        points = zip(*points)
        plt.plot(points[0], points[1], 'ro')
    plt.show()
