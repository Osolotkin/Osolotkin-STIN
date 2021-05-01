#pragma once

#include "Colors.h"

extern int CZECHIA_INDEX;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

#define COLOR_MAIN_FRONT WHITE
#define COLOR_MAIN_BACK BLACK

#define COLOR_SELECTION_BACK HEX_6B9CE2
#define COLOR_SELECTION_FRONT BLACK

#define COLOR_ERROR 0xEC7063

#define COUNTRIES_TO_SELECT 4

extern const Color GRAPH_COLORS[];

int loadConfig();
