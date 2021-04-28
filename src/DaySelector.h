#pragma once

#include <time.h>
#include <windows.h>

typedef struct DayButton {

	int id;

	int x;
	int y;

	int width;
	int height;

	int color;
	int backColor;

	int visible;

	wchar_t* label;
	int labelLength;

	int (*inBounds) (struct DayButton* button, POINT* coords);
	void (*select) (struct DaySelector* selector, struct DayButton* button);

} DayButton;

typedef struct DaySelector {

	int x;
	int y;

	int width;
	int height;

	time_t startTime;

	int page;
	int lastPage;
	int days;

	int color;
	int backColor;

	int selectedColor;
	int selectedBackColor;

	int selected;

	int itemsPerPage;
	DayButton* buttons;

	int visible;

	void (*setByDay) (struct DaySelector* selector, int dayNumber);
	int (*select) (struct DaySelector* selector, POINT* coords);
	int (*hover) (struct DaySelector* daySelector, POINT* coords);

} DaySelector;

DaySelector* newDaySelector(int x, int y, int width, int height, int tabCount, time_t startTime);

void drawDaySelector(DaySelector* daySelector);

void addDayTabDaySelector(DaySelector* selector);

wchar_t* getCurrentDayLabel(DaySelector* daySelector, int* labelLen);
int getRealSelectedDay(DaySelector* selector);
