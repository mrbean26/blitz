#pragma once

#ifndef SAVEFILES_H
#define SAVEFILES_H

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

using namespace std;
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

vector<string> readLines(const char * fileName); // read lines from a file into a vector

void writeLines(const char * fileName, vector<string> fileLines); // write all lines from a vector to a file

bool contains(string mainString, string substring);

string removeString(string currentString,
	string toRemove); // remove substring from string

string removePosition(string currentString,
	int start, int size); //remove string in a positon

vec2 getVec2File(const char * fileName, string vec2Name, int pos = -1);

float getFloatFile(const char * fileName, string floatName, int pos = -1);

#endif