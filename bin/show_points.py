#!/usr/bin/python
import sys
import matplotlib.pyplot as plt

infile = sys.argv[1]

with open(infile) as inf:
    points = zip(*[ line.rstrip('\n').split(' ') for line in inf])
    plt.plot(points[0], points[1], 'ro')
    plt.show()

