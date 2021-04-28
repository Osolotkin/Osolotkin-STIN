#pragma once
#include "Button.h"

struct CallbackButton {

	Button* actvRect;

	wchar_t* text;
	int textLen;

	int visible;
	int mouseOver;

	int color;
	int backColor;

	int selectColor;
	int selectBackColor;

	int borderColor;

	int (*callback) (void);

	int (*select) (struct CallbackButton* button, POINT* coords);
	int (*hover) (struct CallbackButton* button, POINT* coords);


} typedef CallbackButton;

CallbackButton* newCallbackButton(int x, int y, int width, int height, wchar_t* text, int textLen, int (*callback) (void));

void drawCallbackButton(CallbackButton* button);
