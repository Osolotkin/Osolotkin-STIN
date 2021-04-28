#pragma once

#include "windows.h"

typedef struct Button {

	int x;
	int y;
	int width;
	int height;

	int (*inBounds) (struct Button* input, POINT* coords);
	int (*draw) (struct Button* input);

} Button;

Button* newButton(int x, int y, int width, int height);
