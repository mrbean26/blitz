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
        
        #drawPoint(drawPosX, drawPosY, color)
    return [allPointsX, allPointsY]

circleRadiuses = [90, 80, 70, 60, 50, 40, 30, 20, 10]

gradient = 1

for radius in circleRadiuses:
    yPosition = (circleRadiuses[0]-radius+(10*gradient)) /circleRadiuses[0]
    yPosition = yPosition * gradient

    currentPointCount = round(radius / 4) * 2
    currentCircle = drawCircle(320, 180, radius, currentPointCount, 'black')

    nextRadius = radius-10
    if nextRadius <= 0:
        break
    nextPointCount = round(nextRadius/4)*2
    nextCircle = drawCircle(320, 180, nextRadius, nextPointCount, 'black')
    
    for i in range(0, len(currentCircle[0])):
        drawPoint(currentCircle[0][i], currentCircle[1][i], 'black') # represents every point created

        nextIndex = i+1
        if nextIndex > len(currentCircle[0]) - 1:
            nextIndex = 0
        
        x1 = currentCircle[0][i]
        y1 = currentCircle[1][i]

        x2 = currentCircle[0][nextIndex]
        y2 = currentCircle[1][nextIndex]

        c.create_line(x1, y1, x2, y2, fill='blue') # line connecting the circle points to each other (makes lots of rings)

        # line going up & to the right

        nextCircleIndex = math.floor(i / (currentPointCount / nextPointCount))

        x21 = nextCircle[0][nextCircleIndex]
        y21 = nextCircle[1][nextCircleIndex]
        
        c.create_line(x1, y1, x21, y21, fill='green')

        # line going up & to the left

        lastIndex = nextCircleIndex - 1
        if lastIndex < 0:
            lastIndex = len(nextCircle[0]) - 1

        x31 = nextCircle[0][lastIndex]
        y31 = nextCircle[1][lastIndex]
        c.create_line(x1, y1, x31, y31, fill='red')
