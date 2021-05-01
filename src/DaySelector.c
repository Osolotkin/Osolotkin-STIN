/*
	============================================================================================

		Basicly contains stuff to build struct DaySelector, which allows to add days and 
		browrse them, assumes that had fixed first day and no gaps beetween first and last

	============================================================================================
*/

#pragma once

#include <wchar.h>
#include <time.h>

#include "Config.h"
#include "DaySelector.h"
#include "Colors.h"
#include "Utils.h"
#include "render.h"

double marginRate = 0.1;
int margin = 0;

int inBoudns(DayButton* button, POINT* coords);
void selectButton(DaySelector* selector, DayButton* button);
void selectDayButton(DaySelector* selector, DayButton* button);

void adjustTabsVisibility(DaySelector* selector, int modNumber);
void scroll(DaySelector* selector, int dir);

void setByDay(DaySelector* selector, int dayNumber);
int selectDaySelector(DaySelector* daySelector, POINT* coords);
int hoverDaySelector(DaySelector* daySelector, POINT* coords);

/*
*	kind of constructor
*/
DaySelector* newDaySelector(int x, int y, int width, int height, int tabCount, time_t startTime) {

	DaySelector* daySelector = (DaySelector*) malloc(sizeof(DaySelector));
	if (daySelector == NULL) {
		return NULL;
	}

	DayButton* buttons = (DayButton*) malloc((tabCount + 2) * sizeof(DayButton));
	if (buttons == NULL) {
		return NULL;
	}

	margin = (int) (height * marginRate);



	/* firstly creating control buttons, square size at the begining and end of the bar */

	buttons[0].y = buttons[tabCount + 1].y = y + margin;
	buttons[0].height = buttons[tabCount + 1].height = height - 2 * margin;
	buttons[0].width = buttons[tabCount + 1].width = buttons[0].height;
	buttons[0].visible = buttons[tabCount + 1].visible = 1;
	buttons[0].color = buttons[tabCount + 1].color = BLACK;
	buttons[0].backColor = buttons[tabCount + 1].backColor = WHITE;
	buttons[0].label = buttons[tabCount + 1].label = "";
	buttons[0].labelLength = buttons[tabCount + 1].labelLength = 1;
	
	buttons[0].x = x + margin;
	buttons[tabCount + 1].x = x + width - margin - buttons[0].height;

	buttons[0].inBounds = buttons[tabCount + 1].inBounds = &inBoudns;
	buttons[0].select = buttons[tabCount + 1].select = &selectButton;
	
	buttons[0].id = 0;
	buttons[tabCount + 1].id = tabCount + 1;



	/* creating day buttons */

	int offsetX = 2 * margin + buttons[0].width;
	int tabWith = (width - 2 * buttons[0].width - ((tabCount + 1) + 2) * margin) / tabCount;
	for (int i = 1; i <= tabCount; i++) {

		buttons[i].id = i;
		buttons[i].visible = 1;
		buttons[i].color = COLOR_MAIN_BACK;
		buttons[i].backColor = COLOR_MAIN_FRONT;
		buttons[i].width = tabWith;
		buttons[i].height = height - 2 * margin;
		buttons[i].y = y + margin;
		buttons[i].x = offsetX + (i - 1) * (tabWith + margin);
		buttons[i].labelLength = 11;
		buttons[i].label = (LPCWSTR*) malloc(buttons[i].labelLength * sizeof(LPCWSTR));
		wcscpy_s(buttons[i].label, buttons[i].labelLength, L"YYYY-MM-DD");

		buttons[i].inBounds = &inBoudns;
		buttons[i].select = &selectDayButton;
	}



	/* day selector itself */

	daySelector->x = x;
	daySelector->y = y;
	daySelector->width = width;
	daySelector->height = height;
	daySelector->page = 1;
	daySelector->lastPage = 1;
	daySelector->days = 0;
	daySelector->color = BLACK;
	daySelector->backColor = WHITE;
	daySelector->selectedColor = COLOR_SELECTION_BACK;
	daySelector->selectedBackColor = COLOR_SELECTION_FRONT;
	daySelector->selected = -1;
	daySelector->itemsPerPage = tabCount;
	daySelector->buttons = buttons;
	daySelector->visible = 1;

	daySelector->setByDay = &setByDay;
	daySelector->select = &selectDaySelector;
	daySelector->hover = &hoverDaySelector;

	daySelector->startTime = startTime;

	return daySelector;

}

void drawDaySelector(DaySelector* daySelector) {

	if (!daySelector->visible) return;

	int x = daySelector->x;
	int y = daySelector->y;
	int width = daySelector->width;
	int height = daySelector->height;

	renderColor = daySelector->backColor;
	fillRect(x, y, width, height);
	
	DayButton* button;
	for (int i = 0; i < daySelector->itemsPerPage + 2; i++) {

		button = &(daySelector->buttons[i]);

		if (button->visible == 0) continue;

		renderColor = button->color;
		fillRect(button->x, button->y, button->width, button->height);

		const int strSz = 8; // in px
	
		StringWriter stringWriter = StringWriterCenter();
		renderRect strRect = {
			button->x,
			button->y,
			button->width,
			button->height
		};

		renderColor = button->backColor;
		stringWriter.write(
			button->label, 
			button->labelLength - 1, 
			button->color, 
			strSz, 
			&strRect
		);

	}

	button = &(daySelector->buttons[0]);
	if (button->visible) {

		renderColor = button->backColor;
		fillTriangle(
			button->x + margin, button->y + (button->height / 2),
			button->x + button->width - margin, button->y + margin,
			button->x + button->width - margin, button->y + button->height - margin
		);

	}

	button = &(daySelector->buttons[daySelector->itemsPerPage + 1]);
	if (button->visible) {

		renderColor = button->backColor;
		fillTriangle(
			button->x + button->width - margin, button->y + (button->height / 2),
			button->x + margin, button->y + margin,
			button->x + margin, button->y + button->height - margin
		);

	}

}

/*
*	returns 0 if nothing useful happened
*	returns 1 if any day was selected
*/
int selectDaySelector(DaySelector* daySelector, POINT* coords) {

	if (daySelector == NULL) return 0;

	for (int i = 0; i <= daySelector->itemsPerPage + 1; i++) {

		if (daySelector->buttons[i].inBounds(daySelector->buttons + i, coords)) {

			daySelector->buttons[i].select(daySelector, &(daySelector->buttons[i]));
			return 1;

		}

	}

	return 0;

}

/*
*	returns 0 if nothing useful happened
*	returns 1 if mouse was over any button
*	returns 2 if mouse wasn't over any scroll buttons
*/
int hoverDaySelector(DaySelector* daySelector, POINT* coords) {

	if (daySelector == NULL) return 0;

	int lastIdx = daySelector->itemsPerPage;

	for (int i = 1; i <= lastIdx; i++) {

		if (daySelector->buttons[i].inBounds(&(daySelector->buttons[i]), coords)) {
			return 1;
		}

	}

	int retVal = 0;

	if (daySelector->buttons[0].color == daySelector->selectedColor) {
		
		daySelector->buttons[0].color = daySelector->color;
		daySelector->buttons[0].backColor = daySelector->backColor;
		retVal = 2;
	
	} else if (daySelector->buttons[lastIdx + 1].color == daySelector->selectedColor) {

		daySelector->buttons[lastIdx + 1].color = daySelector->color;
		daySelector->buttons[lastIdx + 1].backColor = daySelector->backColor;

		retVal = 2;

	}

	if (daySelector->buttons[0].inBounds(&(daySelector->buttons[0]), coords)) {
		
		daySelector->buttons[0].color = daySelector->selectedColor;
		daySelector->buttons[0].backColor = daySelector->selectedBackColor;
		return 1;

	}

	if (daySelector->buttons[lastIdx + 1].inBounds(&(daySelector->buttons[lastIdx + 1]), coords)) {

		daySelector->buttons[lastIdx + 1].color = daySelector->selectedColor;
		daySelector->buttons[lastIdx + 1].backColor = daySelector->selectedBackColor;
		return 1;

	}

	return retVal;

}

/* CTRL + C | CTRL + V */
int inBoudns(DayButton* button, POINT* coords) {

	return 
		button->visible && 
		(coords->x > button->x) && (coords->x < button->x + button->width) &&
		(coords->y > button->y) && (coords->y < button->y + button->height);

}

void selectDayButton(DaySelector* selector, DayButton* button) {

	if (selector->selected == button->id) return;

	button->color = selector->selectedColor;
	button->backColor = selector->selectedBackColor;

	if (selector->selected > 0 && selector->selected <= selector->itemsPerPage) {
		
		selector->buttons[selector->selected].color = selector->color;
		selector->buttons[selector->selected].backColor = selector->backColor;

	}

	selector->selected = button->id;

}

void selectButton(DaySelector* selector, DayButton* button) {

	int dir = 1;
	if (button->id == 0) dir = -1;

	scroll(selector, dir);

}

/* makes visble only needed tabs */
void adjustTabsVisibility(DaySelector* selector, int modNumber) {

	const int itemsPerPage = selector->itemsPerPage;

	for (int i = 1; i <= modNumber; i++) {

		selector->buttons[i].visible = 1;

		/* computing date to display */
		time_t secOffset = ((selector->page - 1) * itemsPerPage + (i - 1)) * SECONDS_PER_DAY;
		time_t fullSecOffset = selector->startTime + secOffset;

		int y = 0;
		int m = 0;
		int d = 0;
		timeToDate(&fullSecOffset, &y, &m, &d);

		/* for sure preventing overflow */
		if (y > 9999 || y < 0) y = 0;
		if (m > 99 || m < 0) m = 0;
		if (d > 99 || d < 0) d = 0;

		swprintf(selector->buttons[i].label, 11, L"%.4i-%.2i-%.2i", y, m, d);

	}

	for (int i = modNumber + 1; i < itemsPerPage + 1; i++) {
		selector->buttons[i].visible = 0;
	}

}

void addDayTabDaySelector(DaySelector* selector) {

	selector->days++;

	const int itemsPerPage = selector->itemsPerPage;

	int modNumber = ((selector->days - 1) % itemsPerPage) + 1;

	selector->lastPage = selector->days / itemsPerPage;
	selector->lastPage += (modNumber != itemsPerPage) ? 1 : 0;
	selector->page = selector->lastPage;

	adjustTabsVisibility(selector, modNumber);

	selector->buttons[modNumber].id = modNumber;
	selector->buttons[modNumber].visible = 1;
	
	DayButton* button = selector->buttons + (modNumber);
	button->select(selector, button);

}

void scroll(DaySelector* selector, int dir) {

	const int itemsPerPage = selector->itemsPerPage;

	/* not allowing go out of bounds */
	selector->page += dir;
	if (selector->page < 1) {
		
		selector->page = 1;
		return;
	
	} else if (selector->page > selector->lastPage) {
		
		selector->page = selector->lastPage;
		return;
	
	}

	/* getting number of tabs on page that will be shown */
	int modNumber = itemsPerPage;
	if (selector->page == selector->lastPage) {
		modNumber = ((selector->days - 1) % itemsPerPage) + 1;
	}

	adjustTabsVisibility(selector, modNumber);

	DayButton* button = selector->buttons + ((dir < 0) ? modNumber : 1);
	button->select(selector, button);

}

int dayToButtonIndex(DaySelector* selector, int dayNumber) {

	return (dayNumber - 1) % selector->itemsPerPage + 1;

}

void setByDay(DaySelector* selector, int dayNumber) {

	selector->days = dayNumber;
	int tmp = (dayNumber / selector->itemsPerPage);
	selector->lastPage = (dayNumber % selector->itemsPerPage == 0) ? tmp : tmp + 1;
	selector->page = selector->lastPage;

	int idx = dayToButtonIndex(selector, dayNumber);

	adjustTabsVisibility(selector, idx);
	
	DayButton* button = selector->buttons + idx;
	button->select(selector, button);

	selector->selected = idx;

}

wchar_t* getCurrentDayLabel(DaySelector* daySelector, int* labelLen) {

	DayButton* button = daySelector->buttons + daySelector->selected;
	*labelLen = button->labelLength;

	return button->label;

}

int getRealSelectedDay(DaySelector* selector) {

	return selector->itemsPerPage * (selector->page - 1) + selector->selected;

}
