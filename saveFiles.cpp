#include "saveFiles.h"

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

vector<string> readLines(const char * fileName){
	vector<string> allLines;
	ifstream newFile(fileName);
	string currentLine;
	if (!newFile) {
		cout << "File could not be opened: " << fileName << endl;
	}
	while (getline(newFile, currentLine)) {
		allLines.push_back(currentLine);
	}
	return allLines;
}

void writeLines(const char * fileName, vector<string> fileLines){
	ofstream currentFile;
	currentFile.open(fileName);
	if (!currentFile) {
		cout << "File could not be opened: " << fileName << endl;
		return;
	}
	int vectorSize = fileLines.size();
	for (int i = 0; i < vectorSize; i++) {
		currentFile << fileLines[i] << endl;
	}
	currentFile.close();
}

bool contains(string mainString, string substring) {
	if (mainString.find(substring) != string::npos) {
		return true;
	}
	return false;
}

string removeString(string currentString, string toRemove){
	string toReturn = currentString;
	string::size_type pos = currentString.find(toRemove);
	if (pos != string::npos) {
		toReturn.erase(pos, toRemove.length());
	}
	return toReturn;
}

string removePosition(string currentString, int start, int size){
	string toReturn = currentString.erase(start, size);
	return toReturn;
}

vec2 getVec2File(const char * fileName, string vec2Name, int pos) {
	vec2Name += " "; //clear ready for float values

	vector<string> fileLines = readLines(fileName);
	int lineCount = fileLines.size();
	float floatOne = -1.0f, floatTwo = -1.0f;
	if (pos == -1) {
		for (int i = 0; i < lineCount; i++) {
			string currentLine = fileLines[i];
			if (currentLine.find(vec2Name) != string::npos) {
				string valueOnly = removeString(currentLine, vec2Name);
				int valueLength = valueOnly.length();
				int midValuePosition;
				// get values from string
				for (int c = 0; c < valueLength; c++) {
					char currentChar = valueOnly[c];
					if (currentChar == ' ') {
						midValuePosition = c;
					}
				}
				string valueOne;
				for (int c1 = 0; c1 < midValuePosition; c1++) {
					valueOne += valueOnly[c1];
				}
				string valueTwo;
				for (int c2 = midValuePosition + 1; c2 < valueLength; c2++) {
					valueTwo += valueOnly[c2];
				}
				floatOne = stof(valueOne);
				floatTwo = stof(valueTwo);
			}
		}
	}
	if (pos > -1) {
		if (contains(fileLines[pos], vec2Name)) {
			string valueOnly = removeString(fileLines[pos], vec2Name);
			int valueLength = valueOnly.length();
			int midValuePosition;
			// get values from string
			for (int c = 0; c < valueLength; c++) {
				char currentChar = valueOnly[c];
				if (currentChar == ' ') {
					midValuePosition = c;
				}
			}
			string valueOne;
			for (int c1 = 0; c1 < midValuePosition; c1++) {
				valueOne += valueOnly[c1];
			}
			string valueTwo;
			for (int c2 = midValuePosition + 1; c2 < valueLength; c2++) {
				valueTwo += valueOnly[c2];
			}
			floatOne = stof(valueOne);
			floatTwo = stof(valueTwo);
		}
	}
	if (floatOne == -1.0f || floatTwo == -1.0f) {
		return vec2(-1.0f, -1.0f);
	}
	return vec2(floatOne, floatTwo);
}

float getFloatFile(const char * fileName, string floatName, int pos){
	floatName += " ";
	vector<string> allLines = readLines(fileName);
	if (pos == -1) {
		int lineCount = allLines.size();
		for (int i = 0; i < lineCount; i++) {
			string currentString = allLines[i];
			if (contains(currentString, floatName)) {
				string justValue = removeString(currentString, floatName);
				return stof(justValue);
			}
		}
	}
	if (pos != -1) {
		if (contains(allLines[pos], floatName)) {
			string justValue = removeString(allLines[pos], floatName);
			return stof(justValue);
		}
	}
	return -1.0f;
}