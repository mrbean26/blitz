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

def drawCircle(posX, posY, radiusX, pointCount, color):
    k = 2.0 * math.pi / pointCount
    for i in range(0, pointCount):
        phi = i * k
        drawPosX = radiusX * math.cos(phi)
        drawPosY = radiusX * math.sin(phi)
        drawPosX += posX
        drawPosY += posY
        if drawPosY == posY:
            posZ = yMultiply * 100 # draw a 2d mountain here
            drawPoint(drawPosX - 160, 360-(posZ+10), 'black')
        drawPoint(drawPosX, drawPosY, color)

circleRadiuses = [90, 80, 70, 60, 50, 40, 30, 20, 10]

gradient = 1

for radius in circleRadiuses:
    yPosition = (circleRadiuses[0]-radius+(10*gradient)) /circleRadiuses[0]
    yPosition = yPosition * gradient
    if(yPosition > 1.0):
        yPosition = 1.0
    if(yPosition < 0.2):
        color = 'black'
    if(yPosition >= 0.2 and yPosition < 0.4):
        color = 'red'
    if(yPosition >= 0.4 and yPosition < 0.6):
        color = 'blue'
    if(yPosition >= 0.6 and yPosition < 0.8):
        color = 'green'
    if(yPosition >= 0.8 and yPosition < 1.0):
        color = 'brown'
    if(yPosition == 1.0):
        color = 'yellow'
    drawPoint(320, 180, 'yellow')
    currentPointCount = (round(radius / 4)) * 2
    yMultiply = yPosition
    drawCircle(320, 180, radius, currentPointCount, color)
    # in blitz , to get Y position , multiply yPosition variable by mountain Z scale





