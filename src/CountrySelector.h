#pragma once

#include "Search.h"
#include "Button.h"

typedef struct ToggleButton {

	Button* actvRect;

	int color;
	int backColor;

} ToggleButton;

typedef struct SelectButton {

	Button* actvRect;

	int selected;

	int color;

} SelectButton;

typedef struct CountrySelector {

	int x;
	int y;
	int width;
	int height;

	int visible;

	ToggleButton* toggleButton;

	wchar_t** countries;
	int* countriesLengths;
	int countriesCount;

	SelectButton** selectButtons;
	int buttonsCount;

	int selected;

	SearchHandler* searchHandler;

	int (*select) (struct CountrySelector* selector, struct POINT* coords);
	int (*hover) (struct CountrySelector* selector, struct POINT* coords);

} CountrySelector;

CountrySelector* newCountrySelector(int* colors, int countriesToSelect, const char* countriesFile, int x, int y, int width, int height);

void drawCountriesSelector(CountrySelector* selector);
