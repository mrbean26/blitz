#pragma once

#ifndef INVENTORY_H
#define INVENTORY_H

#include <glew.h>
#include <glfw3.h>
#include "texture.h"
#include "monsters.h"
#include "interface.h"

#include <vector>
using namespace std;

#include <glm.hpp>
using namespace glm;

class inventorySlot{
public:
    int itemType = -1; // empty
    bool hotbar = false;
    int buttonIndex = 0;
    vec3 buttonPos = vec3(0.0f);
    int buttonIconIndex = 0;
    int itemQuantity = 0;
};

extern texture defaultTexture;
class item{
public:
    item(vector<float> modelVertices = {}, texture buttonTexture = defaultTexture, vec2 itemButtonScale = vec2(1.0f), vec3 itemModelScale = vec3(1.0f));
    void render();
    int createButtonIcon(int inventorySlotIndex);
    void removeButton(int buttonIndex);

    texture iconTexture;
    vec2 buttonScale = vec2(0.25f);

    vec3 modelScale = vec3(1.0f);
    vec3 modelPosition = vec3(5.0f, 5.0f, -5.0f);
    vec3 modelRotation = vec3(0.0f);

    GLuint VAO, VBO;
    GLuint size;

    int quantity = 1;
    int itemType = 0;

    bool dropping = false;
    vec3 droppingEnd = vec3(0.0f);
    vec3 droppingStart = vec3(0.0f);
    float droppingAngle = 0.0f;
    void droppingInteraction();
};

extern vector<inventorySlot> allSlots;
extern vector<item> allItems;
extern vector<item> allEntities;
extern vector<int> allSlotTexts;
extern int currentSwappingIndex;

vector<string> entitySaveLines();
vector<string> inventorySaveLine();
void getEntities();
void swapItems();
void givePlayerEntity();
vec3 entityColliders(vec3 start);
void collectItems();
void entityInteractions();
void monsterItemDrops(monster &usedMonster);
void startItems();
void inventoryUIMainloop();
void startInventoryUI();
void getInventory();
void inventoryBegin();
void inventoryMainloop();

#endif