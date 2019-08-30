from tkinter import *
window = Tk()
c = Canvas(window, width = 600, height = 600, bg='blue')
c.pack()

pointCount = 10
radiusDifferent = 5
mountainScale = 25.0

from math import *

radiuses = []
for r in range(0, int(mountainScale / radiusDifferent)):
    radiuses.append(mountainScale - r * radiusDifferent)

pointEdgeOne = []
pointEdgeOneY = []
for radius in radiuses:
    newList = []
    newListY = []
    for i in range(0, pointCount):
        newList.append(cos(((360 / pointCount) * i) * 0.01745329251) * radius * 10)
        newListY.append(sin(((360 / pointCount) * i) * 0.01745329251) * radius * 10)
    pointEdgeOne.append(newList)
    pointEdgeOneY.append(newListY)

for r in range(0, len(radiuses)):
    for i in range(0, len(pointEdgeOne[r])):
        nextIndex = i + 1
        if nextIndex == len(pointEdgeOne[r]):
            nextIndex = 0
    
        c.create_line(pointEdgeOne[r][i] + 300, pointEdgeOneY[r][i] + 300, pointEdgeOne[r][nextIndex] + 300, pointEdgeOneY[r][nextIndex] + 300, fill='green')

        nextIndexR = r + 1
        if nextIndexR is not len(radiuses):
            c.create_line(pointEdgeOne[r][i] + 300, pointEdgeOneY[r][i] + 300, pointEdgeOne[nextIndexR][i] + 300, pointEdgeOneY[nextIndexR][i] + 300, fill='green')
            c.create_line(pointEdgeOne[r][i] + 300, pointEdgeOneY[r][i] + 300, pointEdgeOne[nextIndexR][nextIndex] + 300, pointEdgeOneY[nextIndexR][nextIndex] + 300, fill='green')

        if nextIndexR == len(radiuses):
            c.create_line(pointEdgeOne[r][i] + 300, pointEdgeOneY[r][i] + 300, 300, 300, fill='green')

    
