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

	/* button that hiddes / reveals country selector */
	ToggleButton* toggleButton;

	wchar_t** countries;
	int* countriesLengths;
	int countriesCount;

	/* buttons that represent countries allowed for selection */
	SelectButton** selectButtons;
	int buttonsCount;

	/* number of selected country, min val 1, max val buttonsCount */
	int selected;

	/* struct that takes care about searching */
	SearchHandler* searchHandler;

	int (*select) (struct CountrySelector* selector, struct POINT* coords);
	int (*hover) (struct CountrySelector* selector, struct POINT* coords);

} CountrySelector;

CountrySelector* newCountrySelector(int* colors, int countriesToSelect, const char* countriesFile, int x, int y, int width, int height);

void drawCountriesSelector(CountrySelector* selector);
