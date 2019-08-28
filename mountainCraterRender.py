# basics of creating a mountain

# the circle represents the mountain from birds eye view
# the colours represent how high up the mountain is from the birds eye view
# other shapes represent the mountain from side view

# each point represents a triangle in the main project

import math
import time

from tkinter import *
window = Tk()
c = Canvas(window, width=640, height=360)
c.pack()


def drawPoint(x, y, color):
    c.create_rectangle(x-1, y-1, x, y, outline=color)

def drawCircle(posX, posY, radiusX, pointCount, color): # divide pointCount by a number and round it to make different resolution mountains
    k = 2.0 * math.pi / pointCount
    allPointsX = []
    allPointsY = []
    for i in range(0, pointCount):
        phi = i * k
        drawPosX = radiusX * math.cos(phi)
        drawPosY = radiusX * math.sin(phi)
        drawPosX += posX
        drawPosY += posY
        
        allPointsX.append(drawPosX)
        allPointsY.append(drawPosY)
        
        drawPoint(drawPosX, drawPosY, color)
    return [allPointsX, allPointsY]



drawCircle(320, 180, 30, 360*400, 'black')

