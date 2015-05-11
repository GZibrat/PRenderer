#pragma once

#include "PlanetRenderer.h"

#define IS_PRESSED_REG(key) keys[256 + key]
#define IS_PRESSED_SPC(key) keys[key]

extern byte keys[512];

void initKeyboard();