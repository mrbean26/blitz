using namespace std;

#include <vector>
#include <fstream>
#include <string>

vector<string> readLines(const char * fileName) { //all lines into vector
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

void writeLines(const char * fileName, vector<string> fileLines) { //all lines from vector into file
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