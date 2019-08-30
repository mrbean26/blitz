#include "debug.h"
#include "frontend.h"
#include "monsters.h"
#include "structures.h"

#define DEBUG_FONT_SIZE 25
#define DEBUG_UPDATE_TIME_MS 500

#define ALERT_UP_MULTIPLIER 5

int debugText;
int debugTextPlaceholder;
float updateTimer = DEBUG_UPDATE_TIME_MS / 1000.0f;

void debugBegin(){
    debugText = createText();
    debugTextPlaceholder = debugText;
    allTexts[debugText].fontPath = "assets/fonts/zekton.ttf";
    allTexts[debugText].fontSize = display_x / 50.0f;
    allTexts[debugText].position = vec2(display_x / 100, 
        -display_y / 27.5f + display_y);
    allTexts[debugText].colour = vec3(1.0f, 0.0f, 0.0f);
    allTexts[debugText].active = false;
    
    alertBegin();
}

void debugMainloop(){
    alertMainloop();
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
    updateTimer -= deltaTime;
}

texture alertBackgroundTexture;
float alertTimer = 0.0f;
bool inShowing = false;
bool preShowing = false;
int alertText;
int alertTextPlaceholder;

int alertBackground;
int alertBackgroundPlaceholder;

float upMultiplier = 0.0f;
float downMultiplier = 0.0f;

void alertBegin(){
    alertText = createText();
    alertTextPlaceholder = alertText;
    allTexts[alertText].fontPath = "assets/fonts/zekton.ttf";
    allTexts[alertText].fontSize = display_x / 17.5f;
    allTexts[alertText].alpha = 0.0f;
    allTexts[alertText].centered = true;
    
    alertBackground = createButton();
    alertBackgroundPlaceholder = alertBackground;
    allButtons[alertBackground].interactive = false;
    allButtons[alertBackground].scale = vec2(1.5f, 0.25f);
    allButtons[alertBackground].position = vec3(2.2f, 7.0f, 0.0f);
    allButtons[alertBackground].alpha = 0.0f;
    alertBackgroundTexture = loadTexture("assets/images/alertBackgroundImage.png");
    allButtons[alertBackground].texture = alertBackgroundTexture;
}

void alertMainloop(){
    button alert = allButtons[alertBackground];
    allTexts[alertText].position.x = (alert.minX + alert.maxX) / 2.0f;
    allTexts[alertText].position.y = display_y - ((alert.minY + alert.maxY) / 2.0f);
    
    if(preShowing){
        float currentAlpha = allButtons[alertBackground].alpha;
        currentAlpha += deltaTime * downMultiplier;
        
        if(currentAlpha > 1.0f){
            preShowing = false;
            inShowing = true;
            currentAlpha = 1.0f;
        }
        
        allButtons[alertBackground].alpha = currentAlpha;
        allTexts[alertText].alpha = currentAlpha;
    }
    if(inShowing){
        float currentAlpha = allButtons[alertBackground].alpha;
        currentAlpha -= deltaTime * upMultiplier;
        
        if(currentAlpha < 0.0f){
            inShowing = false;
            preShowing = false;
            currentAlpha = 0.0f;
        }
        
        allButtons[alertBackground].alpha = currentAlpha;
        allTexts[alertText].alpha = currentAlpha;
    }
}

void alert(string text, float time){
    allTexts[alertText].displayedText = text;
    inShowing = false;
    preShowing = true;
    downMultiplier = 1.0f / time;
    upMultiplier = 1.0f / (time * ALERT_UP_MULTIPLIER);
}
