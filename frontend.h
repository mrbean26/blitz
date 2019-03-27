#pragma once
//use of the engine to create the game

#ifndef FRONTEND_H
#define FRONTEND_H

#include "worldGeneration.h"
extern worldGeneration mainWorldGeneration;

#include "startScreen.h"
extern startScreen mainStart;

void frontendBegin(); // called before first frame

void frontendMainloop(); // called every frame

#endif