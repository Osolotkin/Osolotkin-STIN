#pragma once

#include "wchar.h"
#include "Button.h"

#define MAX_INPUT_LENGTH 64

typedef struct SearchInput {

	Button* inpRect;

	int maxLength;
	int length;
	wchar_t buffer[MAX_INPUT_LENGTH + 1]; /* last char is for cursor */

	int hasFocus;

	void (*addChar) (struct SearchHandler* searchHandler, wchar_t ch);
	void (*removeChar) (struct SearchHandler* searchHandler);

} SearchInput;

typedef struct SearchResult {

	Button* actvRect;

	int color;
	int backColor;

} SearchResult;

typedef struct SearchResultsBox {

	int x;
	int y;
	int width;
	int height;

	SearchResult** results; /* points to results that are displayed */
	int size; /* max results that can be displayed at once */
	
	int selected;

	int resultsOffset; /* offset of the first result */
	int resultsCount; /* total number of results */

} SearchResultsBox;

typedef struct SearchHandler {

	int x;
	int y;
	int width;
	int height;

	SearchInput* input;
	SearchResultsBox* resultsBox;

	wchar_t** data;
	int dataCount;
	int* dataLengths;

	int(*select) (struct SearchHandler* input, POINT* mouseCoords);
	int(*hover) (struct SearchHandler* input, POINT* mouseCoords);


} SearchHandler;

SearchHandler* newSearchHandler(wchar_t** data, int* dataLength, int dataCount, int x, int y, int width, int height, int results);

void drawSearch(SearchHandler* search);

void setInput(SearchHandler* search, int idx);
