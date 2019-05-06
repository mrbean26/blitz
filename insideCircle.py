# basics of creating a mountain

# the circle represents the mountain from birds eye view
# the colours represent how high up the mountain is from the birds eye view
# other shapes represent the mountain from side view

# each point represents a triangle in the main project

import math

from tkinter import *
window = Tk()
c = Canvas(window, width=640, height=360)
c.pack()


def drawPoint(x, y, color):
    c.create_rectangle(x-1, y-1, x, y, outline=color)

yMultiply = 0.0

def drawCircle(posX, posY, radiusX, pointCount, color): # divide pointCount by a number and round it to make different resolution mountains
    k = 2.0 * math.pi / pointCount
    for i in range(0, pointCount):
        phi = i * k
        drawPosX = radiusX * math.cos(phi)
        drawPosY = radiusX * math.sin(phi)
        drawPosX += posX
        drawPosY += posY
        
        drawPoint(drawPosX, drawPosY, color)

def inCircle(circleX, circleY, radius, x, y):
    drawPoint(x, y, 'black')
    if ((x-circleX)*(x-circleX)+(y-circleY)*(y-circleY) <= radius*radius):
        return True
    else:
        return False

drawCircle(320, 180, 100, round(100/4)*2, 'black')


if inCircle(320, -180, 100, 310, -220):
    print("inside")
