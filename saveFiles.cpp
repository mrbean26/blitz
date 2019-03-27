#include "saveFiles.h"

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
