/*
	============================================================================================

		Just a button, or may I say rect...

	============================================================================================
*/

#pragma once

#include "Button.h"
#include "Render.h"

int inBoundsButton(Button* butt, POINT* coords);
void drawButton(Button* butt);

Button* newButton(int x, int y, int width, int height) {

	Button* button = (Button*) (malloc(sizeof(Button)));
	if (button == NULL) {
		return NULL;
	}

	button->x = x;
	button->y = y;
	button->width = width;
	button->height = height;

	button->inBounds = &inBoundsButton;
	button->draw = &drawButton;

	return button;

}

int inBoundsButton(Button* butt, POINT* coords) {

	return (coords->x > butt->x) && (coords->x < butt->x + butt->width) &&
		(coords->y > butt->y) && (coords->y < butt->y + butt->height);

}

void drawButton(Button* butt) {

	fillRect(butt->x, butt->y, butt->width, butt->height);

}
