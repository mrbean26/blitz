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
	string toReturn = currentString.erase(start, start + size);
	return toReturn;
}

string removeAfterCharacter(string currentString, string character){
	return currentString.substr(0, currentString.find(character));
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
				int midValuePosition = 0;
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
			int midValuePosition = 0;
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

vec4 getVec4File(const char* fileName, string vec4Name, int pos){
	float floatOne = -1.0f, floatTwo = -1.0f, floatThree = -1.0f, floatFour = -1.0f;
	vector<string> lines = readLines(fileName);
	int lCount = lines.size();
	if (pos == -1) {
		for (int l = 0; l < lCount; l++) {
			if (!contains(lines[l], vec4Name)) {
				continue;
			}
			
			int firstValuePos = 0, secondValuePos = 0, thirdValuePos = 0;
			string valueOnly = removeString(lines[l], vec4Name + " ");
			int valLength = valueOnly.length();
			for (int c = 0; c < valLength; c++) {
				char current = valueOnly[c];
				if (current == ' ') {
					if (firstValuePos == 0) {
						firstValuePos = c;
						continue;
					}
					if (secondValuePos == 0) {
						secondValuePos = c;
						continue;
					}
					if (thirdValuePos == 0) {
						thirdValuePos = c;
					}
				}
			}

			string valOne, valTwo, valThree, valFour;
			for (int v1 = 0; v1 < firstValuePos; v1++) {
				valOne += valueOnly[v1];
			}
			for (int v2 = firstValuePos + 1; v2 < secondValuePos; v2++) {
				valTwo += valueOnly[v2];
			}
			for (int v3 = secondValuePos + 1; v3 < thirdValuePos; v3++) {
				valThree += valueOnly[v3];
			}
			for (int v4 = thirdValuePos + 1; v4 < valLength; v4++) {
				valFour += valueOnly[v4];
			}

			floatOne = stof(valOne);
			floatTwo = stof(valTwo);
			floatThree = stof(valThree);
			floatFour = stof(valFour);
		}
	}
	if (pos != -1) {
		if (!contains(lines[pos], vec4Name)) {
			return vec4(-1.0f);
		}

		int firstValuePos = 0, secondValuePos = 0, thirdValuePos = 0;
		string valueOnly = removeString(lines[pos], vec4Name + " ");
		int valLength = valueOnly.length();
		for (int c = 0; c < valLength; c++) {
			char current = valueOnly[c];
			if (current == ' ') {
				if (firstValuePos == 0) {
					firstValuePos = c;
					continue;
				}
				if (secondValuePos == 0) {
					secondValuePos = c;
					continue;
				}
				if (thirdValuePos == 0) {
					thirdValuePos = c;
				}
			}
		}

		string valOne, valTwo, valThree, valFour;
		for (int v1 = 0; v1 < firstValuePos; v1++) {
			valOne += valueOnly[v1];
		}
		for (int v2 = firstValuePos + 1; v2 < secondValuePos; v2++) {
			valTwo += valueOnly[v2];
		}
		for (int v3 = secondValuePos + 1; v3 < thirdValuePos; v3++) {
			valThree += valueOnly[v3];
		}
		for (int v4 = thirdValuePos + 1; v4 < valLength; v4++) {
			valFour += valueOnly[v4];
		}

		floatOne = stof(valOne);
		floatTwo = stof(valTwo);
		floatThree = stof(valThree);
		floatFour = stof(valFour);
	}
	return vec4(floatOne, floatTwo, floatThree, floatFour);
}

vec3 getVec3File(const char* fileName, string vec3Name, int pos) {
	float floatOne = -1.0f, floatTwo = -1.0f, floatThree = -1.0f, floatFour = -1.0f;
	vector<string> lines = readLines(fileName);
	int lCount = lines.size();
	if (pos == -1) {
		for (int l = 0; l < lCount; l++) {
			if (!contains(lines[l], vec3Name)) {
				continue;
			}

			int firstValuePos = 0, secondValuePos = 0, thirdValuePos = 0;
			string valueOnly = removeString(lines[l], vec3Name + " ");
			int valLength = valueOnly.length();
			for (int c = 0; c < valLength; c++) {
				char current = valueOnly[c];
				if (current == ' ') {
					if (firstValuePos == 0) {
						firstValuePos = c;
						continue;
					}
					if (secondValuePos == 0) {
						secondValuePos = c;
						continue;
					}
				}
			}

			string valOne, valTwo, valThree;
			for (int v1 = 0; v1 < firstValuePos; v1++) {
				valOne += valueOnly[v1];
			}
			for (int v2 = firstValuePos + 1; v2 < secondValuePos; v2++) {
				valTwo += valueOnly[v2];
			}
			for (int v3 = secondValuePos + 1; v3 < valLength; v3++) {
				valThree += valueOnly[v3];
			}

			floatOne = stof(valOne);
			floatTwo = stof(valTwo);
			floatThree = stof(valThree);
		}
	}
	if (pos != -1) {
		if (!contains(lines[pos], vec3Name)) {
			return vec3(-1.0f);
		}

		int firstValuePos = 0, secondValuePos = 0;
		string valueOnly = removeString(lines[pos], vec3Name + " ");
		int valLength = valueOnly.length();
		for (int c = 0; c < valLength; c++) {
			char current = valueOnly[c];
			if (current == ' ') {
				if (firstValuePos == 0) {
					firstValuePos = c;
					continue;
				}
				if (secondValuePos == 0) {
					secondValuePos = c;
					continue;
				}
			}
		}

		string valOne, valTwo, valThree;
		for (int v1 = 0; v1 < firstValuePos; v1++) {
			valOne += valueOnly[v1];
		}
		for (int v2 = firstValuePos + 1; v2 < secondValuePos; v2++) {
			valTwo += valueOnly[v2];
		}
		for (int v3 = secondValuePos + 1; v3 < valLength; v3++) {
			valThree += valueOnly[v3];
		}

		floatOne = stof(valOne);
		floatTwo = stof(valTwo);
		floatThree = stof(valThree);
	}
	return vec3(floatOne, floatTwo, floatThree);
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

int getIntFile(const char* fileName, string intName, int pos){
	intName += " ";
	vector<string> allLines = readLines(fileName);
	if (pos == -1) {
		int lineCount = allLines.size();
		for (int i = 0; i < lineCount; i++) {
			string currentString = allLines[i];
			if (contains(currentString, intName)) {
				string justValue = removeString(currentString, intName);
				return stoi(justValue);
			}
		}
	}
	if (pos != -1) {
		if (contains(allLines[pos], intName)) {
			string justValue = removeString(allLines[pos], intName);
			return stoi(justValue);
		}
	}
	return -1;
	return 0;
}

vector<string> rewriteLine(vector<string> existingLines, string varName, string newValue, int pos) {
	int lCount = existingLines.size();
	if (pos == -1) {
		for (int l = 0; l < lCount; l++) {
			if (contains(existingLines[l], varName)) {
				existingLines[l] = varName + " " + newValue;
				break;
			}
		}
	}
	if (pos != -1) {
		if (pos < lCount) {
			if (contains(existingLines[pos], varName)) {
				existingLines[pos] = varName + " " + newValue;
			}
		}
	}
	return existingLines;
}
