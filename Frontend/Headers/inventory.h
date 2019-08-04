#pragma once

#ifndef INVENTORY_H
#define INVENTORY_H

#include <glew.h>
#include <glfw3.h>
#include "texture.h"

#include <vector>
using namespace std;

#include <glm.hpp>
using namespace glm;

class inventorySlot{
public:
    int itemType = 0; // empty
    bool hotbar = false;
    int buttonIndex = 0;
    vec3 buttonPos = vec3(0.0f);
};

class item{
public:
    item(vector<float> modelVertices, texture buttonTexture, vec2 buttonScale, vec3 modelScale);
    void render(mat4 model);
    int createButtonIcon(int inventorySlotIndex);
    void removeButton(int buttonIndex);

    texture iconTexture;
    vec2 buttonScale = vec2(0.25f);

    vec3 modelScale = vec3(1.0f);
    GLuint VAO, VBO;
    GLuint size;
};

extern vector<inventorySlot> allSlots;
extern vector<item> allItems;

void startItems();
void inventoryUIMainloop();
void startInventoryUI();
void getInventory();
void inventoryBegin();
void inventoryMainloop();

#endif