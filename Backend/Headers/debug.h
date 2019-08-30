#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "interface.h"

extern int debugText;
extern int debugTextPlaceholder;
void debugBegin();
void debugMainloop();

extern int alertBackground;
extern int alertBackgroundPlaceholder;
extern texture alertBackgroundTexture;

extern int alertText;
extern int alertTextPlaceholder;

extern bool inShowing;
extern bool preShowing;
extern float alertTimer;

extern float upMultiplier;
extern float downMultiplier;

void alertBegin();
void alertMainloop();
void alert(string text = "Alert Error!", float time = 1.0f);

#endif
