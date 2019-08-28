import math
from tkinter import *
window = Tk()
c = Canvas(window, width=640, height=360, bg='grey')
c.pack()

def drawPoint(x, y, color):
    c.create_rectangle(x-1, y-1, x, y, outline=color)

def drawTriangle(posX, posY, posX1, posY1, posX2, posY2, colour):
    c.create_polygon(posX, posY, posX1, posY1, posX2, posY2, fill=colour)

def drawCircle(posX, posY, radiusX, pointCount, color):
    pointCount2 = math.ceil(pointCount / 4)
    k = 2.0 * math.pi / pointCount2
    for i in range(0, pointCount2):
        phi = i * k
        drawPosX = radiusX * math.cos(phi)
        drawPosY = radiusX * math.sin(phi)
        drawPosX += posX
        drawPosY += posY
        drawPoint(drawPosX, drawPosY, color)

        nextphi = (i+1)*k
        nextx = radiusX*math.cos(nextphi)
        nexty = radiusX*math.sin(nextphi)
        nextx+=posX
        nexty+=posY

        c.create_line(drawPosX, drawPosY, nextx, nexty, fill='blue')

circleRadiuses = [90, 80, 70, 60, 50, 40, 30, 20, 10]

#drawTriangle(0, 0, 640, 0, 640, 360, 'red')

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

