#include "model.h"

#include <string>
#include <algorithm>
#include <vector>

#include "structures.h"
#include "inventory.h"
#include "worldGeneration.h"

/*
To setup an OBJ enable only triangulate faces & enable write normals & enable apply modifiers
	& enable objects as OBJ objects

Use readyModel to make obj file ok to use in game
*/

model::model(float colourMultiplier, const char* filePath) { // load model (dont use this for main game, only for getting data)
	// load model
	vector<string> allLines = readLines(filePath);
	int lCount = allLines.size();

	vector<vec4> points;
	vector<vec4> faceIndexes;
	vector<string> names;
	int shapeCount = 0;

	for (int i = 0; i < lCount; i++) {
		string line = allLines[i];
		if (contains(line, "v")) {
			vec3 point = getVec3File(filePath, "v", i);
			vec4 newPoint = vec4(point, i);
			points[newVectorPos(&points)] = newPoint;
		}
		if (contains(line, "f")) {
			vec3 index = getVec3File(filePath, "f", i);
			vec4 newIndex = vec4(index, i);
			faceIndexes[newVectorPos(&faceIndexes)] = newIndex;
		}//
		if(contains(line, "o")){
			names[newVectorPos(&names)] = to_string(i) + line;
			shapeCount++;
		}
	}
	// add colours
	int fCount = faceIndexes.size();
	int lastIndex = 0;
	int currentShape = -1;
	vec3 currentColour = vec3(1.0f);
	for (int f = 0; f < fCount; f++) {
		if(faceIndexes[f].w > lastIndex + 1){
			currentShape++;

			cout << "Enter colour X for " << names[currentShape] << ": ";
			cin >> currentColour.x;

			cout << "Enter colour Y for " << names[currentShape] << ": ";
			cin >> currentColour.y;

			cout << "Enter colour Z for " << names[currentShape] << ": ";
			cin >> currentColour.z;

			cout << currentColour.x << " " << currentColour.y << " " << currentColour.z << endl;
		}
		int xIndex = (int) faceIndexes[f].x;
		int yIndex = (int) faceIndexes[f].y;
		int zIndex = (int) faceIndexes[f].z;

		int indexes[] = { xIndex, yIndex, zIndex };
		vec3 usedColour = currentColour + colourDifference(colourMultiplier);
		for (int v = 0; v < 3; v++) {
			int currentIndex = indexes[v] - 1;
			vertices[newVectorPos(&vertices)] = points[currentIndex].x;
			vertices[newVectorPos(&vertices)] = points[currentIndex].y;
			vertices[newVectorPos(&vertices)] = points[currentIndex].z;

			vertices[newVectorPos(&vertices)] = usedColour.x;
			vertices[newVectorPos(&vertices)] = usedColour.y;
			vertices[newVectorPos(&vertices)] = usedColour.z;
		}
		lastIndex = (int) faceIndexes[f].w;
	}
	startIrregularColorBuilding(vertices, VAO, VBO, size);
}

void model::render(){
	glUseProgram(playerShader);
	setMat4(playerShader, "model", modelMat);
	setMat4(playerShader, "projection", projectionMatrix());
	setMat4(playerShader, "view", viewMatrix());
	setShaderInt(playerShader, "useLight", false);
	setShaderFloat(playerShader, "alpha", 1.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, size);
}

void model::outputVertices() { // used for getting verts used for main game
	cout << "{ ";

	int vCount = vertices.size();
	for (int v = 0; v < vCount; v++) {
		string output = to_string(vertices[v]) + "f" + ", ";
		if(!contains(output, ".")){
			output = to_string(vertices[v]) + ".0" + "f" + ", ";
		}
		cout << output;
	}

	cout << "}" << endl;
}

void readyModel(const char* filePath) { // change file format to readable
	vector<string> lines = readLines(filePath);
	vector<string> newLines;

	int count = lines.size();
	for (int i = 0; i < count; i++) {
		if (contains(lines[i], "v ")) {
			newLines[newVectorPos(&newLines)] = lines[i];
		}
		if(contains(lines[i], "o ")){
			newLines[newVectorPos(&newLines)] = lines[i];
		}
		if (contains(lines[i], "f ")) {
			string line = lines[i];
			int lineLength = line.length();

			int slashPosOne = 0, slashPosTwo = 0, slashPosThree = 0;
			for (int c = 0; c < lineLength; c++) {
				if (line[c] == '/' && slashPosOne == 0) {
					slashPosOne = c;
					c = c + 1;
					continue;
				}
				if (line[c] == '/' && slashPosTwo == 0) {
					slashPosTwo = c;
					c = c + 1;
					continue;
				}
				if (line[c] == '/' && slashPosThree == 0) {
					slashPosThree = c;
					c = c + 1;
					continue;
				}
			}
			string one;
			for (int c1 = slashPosOne; c1 < 1000; c1++) {
				if (line[c1] == ' ') {
					break;
				}
				one += line[c1];
			}
			string two;
			for (int c2 = slashPosTwo; c2 < 1000; c2++) {
				if (line[c2] == ' ') {
					break;
				}
				two += line[c2];
			}
			string three;
			for (int c3 = slashPosThree; c3 < 1000; c3++) {
				if (c3 > lineLength - 1) {
					break;
				}
					
				three += line[c3];
			}
			line = removeString(line, one);
			line = removeString(line, two);
			line = removeString(line, three);
			newLines[newVectorPos(&newLines)] = line;
		}
	}

	writeLines(filePath, newLines);
}

/*
setting up texture models:
export with only:
-triangulate faces
-apply modifiers
-include UVs
-objects as obj objects
*/

texture blankTexture;

void readyTextureModelfile(const char * filePath){
	vector<string> lines = readLines(filePath);
	vector<string> newLines;

	int count = lines.size();
	for (int i = 0; i < count; i++) {
		if (contains(lines[i], "v ")) {
			newLines[newVectorPos(&newLines)] = lines[i];
		}
		if(contains(lines[i], "vt")){
			newLines[newVectorPos(&newLines)] = lines[i];
		}
		if (contains(lines[i], "f ")) {
			string line = lines[i];
			
			std::replace(line.begin(), line.end(), '/', ',');
			std::replace(line.begin(), line.end(), ' ', ',');
			line[1] = ' ';
			line += ',';

			newLines[newVectorPos(&newLines)] = line;
		}
	}

	writeLines(filePath, newLines);
}

void textureModel::outputVertices(){
	cout << "{ ";

	int vCount = vertices.size();
	for (int v = 0; v < vCount; v++) {
		string output = to_string(vertices[v]) + "f" + ", ";
		if(!contains(output, ".")){
			output = to_string(vertices[v]) + ".0" + "f" + ", ";
		}
		cout << output;
	}

	cout << "}" << endl;
}

textureModel::textureModel(const char * filePath, texture usedTexture){
	// load model
	vector<string> allLines = readLines(filePath);
	int lCount = allLines.size();

	vector<vec4> points;
	vector<vector<float>> indexes;
	vector<vec2> uvCoords;

	for (int i = 0; i < lCount; i++) {
		string line = allLines[i];
		if (contains(line, "v ")) {
			vec3 point = getVec3File(filePath, "v", i);
			vec4 newPoint = vec4(point, i);
			points[newVectorPos(&points)] = newPoint;
		}
		if (contains(line, "f")) {
			vector<float> faceIndexes = getVectorFile(filePath, "f", i);
			indexes[newVectorPos(&indexes)] = faceIndexes;
		}//
		if(contains(line, "vt")){
			vec2 uv = getVec2File(filePath, "vt", i);
			uvCoords[newVectorPos(&uvCoords)] = uv;
		}
	}
	modelTexture = usedTexture;


	// coord then uv
	int fCount = indexes.size();
	for(int f = 0; f < fCount; f++){
		int xIndex = (int) indexes[f][0];
		int xUVIndex = (int) indexes[f][1];
		int yIndex = (int) indexes[f][2];
		int yUVIndex = (int) indexes[f][3];
		int zIndex = (int) indexes[f][4];
		int zUVIndex = (int) indexes[f][5];
		
		int indexes[] = { xIndex, xUVIndex, yIndex, yUVIndex, zIndex, zUVIndex };
		for (int v = 0; v < 3; v++) {
			int currentIndex = indexes[v * 2] - 1;
			vertices[newVectorPos(&vertices)] = points[currentIndex].x;
			vertices[newVectorPos(&vertices)] = points[currentIndex].y;
			vertices[newVectorPos(&vertices)] = points[currentIndex].z;

			// uv
			int currentUVIndex = indexes[v * 2 + 1] - 1;
			vertices[newVectorPos(&vertices)] = uvCoords[currentUVIndex].x;
			vertices[newVectorPos(&vertices)] = uvCoords[currentUVIndex].y;
		}
	}

	// begin vao & vbo
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
	size = vertices.size() / 5;
}

int modelTextureShader;
void startModelTextureShader(){
	int vertex = createShader("assets/shaders/modelTextureVert.txt", GL_VERTEX_SHADER);
	int fragment = createShader("assets/shaders/modelTextureFrag.txt", GL_FRAGMENT_SHADER);
	modelTextureShader = createProgram({vertex, fragment});
}

void textureModel::render(mat4 model){
	glUseProgram(modelTextureShader);
	setMat4(modelTextureShader, "view", viewMatrix());
	setMat4(modelTextureShader, "projection", projectionMatrix());
	setMat4(modelTextureShader, "model", model);
	
	glActiveTexture(GL_TEXTURE31);
	enableTexture(modelTexture);
	setShaderInt(modelTextureShader, "texture0", 31);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, size);
}

readyTextureModel::readyTextureModel(std::vector<float> usedVertices, texture usedTexture, bool initiate){
	if (!initiate) {
		return;
	}
	modelTexture = usedTexture;
	// begin vao & vbo
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, usedVertices.size() * sizeof(float), usedVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
	size = usedVertices.size() / 5;
}

void readyTextureModel::render(mat4 model, bool useLight, vec3 multiply, bool inWater, vec3 waterColour){
	glUseProgram(modelTextureShader);

	setShaderInt(modelTextureShader, "cameraInWater", inWater);
	setShaderVecThree(modelTextureShader, "waterMultiplyColour", waterColour);

	setShaderVecThree(modelTextureShader, "multiplyColour", multiply);
	setShaderVecThree(modelTextureShader, "lightPos", lightPos);
	setShaderFloat(modelTextureShader, "lightIntensity", lightIntensity);
	setShaderFloat(modelTextureShader, "lightRadius", lightRadius);
	setShaderInt(modelTextureShader, "useLight", useLight);
	setShaderFloat(modelTextureShader, "lowestLight", lowestLight);

	setMat4(modelTextureShader, "view", viewMatrix());
	setMat4(modelTextureShader, "projection", projectionMatrix());
	setMat4(modelTextureShader, "model", model);
	
	glActiveTexture(GL_TEXTURE31);
	enableTexture(modelTexture);
	setShaderInt(modelTextureShader, "texture0", 31);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, size);
}
