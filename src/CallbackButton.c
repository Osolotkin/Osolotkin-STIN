/*
	============================================================================================

		Just button with callback function...

	============================================================================================
*/

#pragma once

#include "Config.h"
#include "CallbackButton.h"
#include "Render.h"

int selectCallbackButton(CallbackButton* button, POINT* coords);
int hoverCallbackButton(CallbackButton* button, POINT* coords);

CallbackButton* newCallbackButton(int x, int y, int width, int height, wchar_t* text, int textLen, int (*callback) (void)) {

	CallbackButton* button = (CallbackButton*) malloc(sizeof(CallbackButton));
	if (button == NULL) {
		return NULL;
	}

	Button* rect = newButton(x, y, width, height);
	if (rect == NULL) {
		return NULL;
	}

	rect->x = x;
	rect->y = y;
	rect->width = width;
	rect->height = height;

	button->actvRect = rect;

	button->text = text;
	button->textLen = textLen;

	button->color = COLOR_MAIN_FRONT;
	button->backColor = COLOR_MAIN_BACK;

	button->selectColor = COLOR_SELECTION_FRONT;
	button->selectBackColor = COLOR_SELECTION_BACK;
	
	button->borderColor = COLOR_MAIN_FRONT;

	button->visible = 0;
	button->mouseOver = 0;

	button->select = &selectCallbackButton;
	button->hover = &hoverCallbackButton;

	button->callback = callback;

	return button;

}

void drawCallbackButton(CallbackButton* button) {

	if (!button->visible) return;

	int frontColor;
	int backColor;
	
	if (button->mouseOver) {
		frontColor = button->selectColor;
		backColor = button->selectBackColor;
	} else {
		frontColor = button->color;
		backColor = button->backColor;
	}

	renderColor = backColor;
	Button* bRect = button->actvRect;

	bRect->draw(bRect);

	int chSz = bRect->width * 0.9 / button->textLen;

	StringWriter stringWriter = StringWriterCenter();
	renderRect strRect = { 
		bRect->x, 
		bRect->y, 
		bRect->width, 
		bRect->height 
	};

	renderColor = frontColor;
	stringWriter.write(
		button->text, 
		button->textLen,
		backColor,
		chSz, 
		&strRect
	);
	
	renderColor = button->borderColor;
	drawRect(bRect->x, bRect->y, bRect->width, bRect->height);

}

/*
*	returns 0 if nothing useful happend
*	returns 1 if callback function was called and returned 0
*	returns 2 if callback function was called and returned +
*/
int selectCallbackButton(CallbackButton* button, POINT* coords) {

	if (!button->visible) return 0;

	if (button->actvRect->inBounds(button->actvRect, coords)) {
	
		if (button->callback()) {
			return 2;
		};
		return 1;
	
	}

	return 0;

}

/*
*	returns 0 if nothing useful happend
*	returns 1 if mouse was over button
*	returns 2 if mouse wasn't over button
*/
int hoverCallbackButton(CallbackButton* button, POINT* coords) {

	if (!button->visible) return 0;

	if (button->actvRect->inBounds(button->actvRect, coords)) {

		button->mouseOver = 1;
		return 1;

	}

	int msg = (button->mouseOver) ? 2 : 0;

	button->mouseOver = 0;
	return msg;

}
