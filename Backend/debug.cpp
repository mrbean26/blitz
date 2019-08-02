#include "debug.h"
#include "frontend.h"
#include "monsters.h"
#include "structures.h"

#define DEBUG_FONT_SIZE 25
#define DEBUG_UPDATE_TIME_MS 500


int debugText;
int debugTextPlaceholder;
float updateTimer = DEBUG_UPDATE_TIME_MS / 1000.0f;

void debugBegin(){
    debugText = createText();
    debugTextPlaceholder = debugText;
    allTexts[debugText].fontPath = "assets/fonts/zekton.ttf";
    allTexts[debugText].fontSize = 25;
    allTexts[debugText].position = vec2(display_x / 100, 
        -display_y / 27.5f + display_y);
    allTexts[debugText].colour = vec3(1.0f, 0.0f, 0.0f);
    allTexts[debugText].active = false;
}

void debugMainloop(){
    if(checkKeyDown(GLFW_KEY_F3)){
        allTexts[debugText].active = !allTexts[debugText].active;
    }
    string displayedText = "FPS: " + to_string(glm::round(frames));
    displayedText = removeAfterCharacter(displayedText, ".") + ", ";

    displayedText += "XYZ: [";
    string xString = removeAfterCharacter(to_string(glm::round(mainPlayer.position.x)), ".") + ", ";
    string yString = removeAfterCharacter(to_string(glm::round(mainPlayer.position.y)), ".") + ", ";
    string zString = removeAfterCharacter(to_string(glm::round(mainPlayer.position.z)), ".");
    displayedText += xString + yString + zString + "], ";

    displayedText += "Planet: " + WorldGeneration.currentAreaPrefix + ", ";
    displayedText += "Monsters: " + to_string(allMonsters.size()) + ", ";
    displayedText += "Structures: " + to_string(allColourBuildings.size() - 1) + ", ";
    
    string timeString = removeAfterCharacter(to_string(glm::round(currentTime)), ".");
    displayedText += "Time: " + timeString + ", ";
    displayedText += "Terrain Verts: " + to_string(WorldGeneration.terrainVerts);

    if(updateTimer < 0.0f){
        allTexts[debugText].displayedText = displayedText;
        updateTimer = DEBUG_UPDATE_TIME_MS / 1000.0f;
    }
    updateTimer -= (float) deltaTime;
}