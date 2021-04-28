#pragma once

#include <time.h>

#include "Config.h"
#include "Search.h"
#include "render.h"
#include "Colors.h"

#define CURSOR_HIDDEN L' '
#define CURSOR_VISIBLE L'_'

int selectSearchHandler(SearchHandler* search, POINT* mouseCoords);
int hoverSearchHandler(SearchHandler* search, POINT* mouseCoords);

void addChar(SearchHandler* searchHandler, wchar_t ch);
void removeChar(SearchHandler* searchHandler);
void search(SearchHandler* searchHandler);

void toLowerCase(wchar_t* ch);

clock_t cursorLastTime = 0;
int cursorAnimLen = 500; /* ms */
wchar_t cursor = CURSOR_VISIBLE;

SearchHandler* newSearchHandler(wchar_t** data, int* dataLength, int dataCount, int x, int y, int width, int height, int results) {

	int lineHg = height / (results + 1);

	/* input */
	SearchInput* input = (SearchInput*)malloc(sizeof(SearchInput));
	if (input == NULL) {
		return NULL;
	}

	Button* inpRect = newButton(x, y, width, lineHg);
	if (inpRect == NULL) {
		return NULL;
	}

	input->inpRect = inpRect;
	input->maxLength = MAX_INPUT_LENGTH;
	input->length = 0;
	input->hasFocus = 0;

	input->addChar = &addChar;
	input->removeChar = &removeChar;

	/* results box */
	SearchResultsBox* resultsBox = (SearchResultsBox*) malloc(sizeof(SearchResultsBox));
	if (resultsBox == NULL) {
		return NULL;
	}

	resultsBox->x = x;
	resultsBox->y = y + input->inpRect->height;
	resultsBox->width = width;
	resultsBox->height = lineHg * results;
	resultsBox->size = results;
	resultsBox->resultsOffset = 0;
	resultsBox->resultsCount = results;

	SearchResult** dspResults = (SearchResult**) malloc(sizeof(SearchResult*) * results);
	if (dspResults == NULL) {
		return NULL;
	}

	int bY = resultsBox->y;
	int bX = resultsBox->x;
	int bHg = input->inpRect->height;
	int bWd = resultsBox->width;
	for (int i = 0; i < results;  i++) {

		dspResults[i] = (SearchResult*) malloc(sizeof(SearchResult));
		if (dspResults[i] == NULL) {
			return NULL;
		}

		dspResults[i]->actvRect = newButton(bX, bY, bWd, bHg);
		if (dspResults[i]->actvRect == NULL) {
			return NULL;
		}

		dspResults[i]->color = COLOR_MAIN_FRONT;
		dspResults[i]->backColor = COLOR_MAIN_BACK;

		bY += bHg;

	}

	resultsBox->results = dspResults;
	resultsBox->selected = 0;

	/* search handler */
	SearchHandler* handler = (SearchHandler*) malloc(sizeof(SearchHandler));
	if (handler == NULL) {
		return NULL;
	}
	
	handler->x = x;
	handler->y = y;
	handler->width = width;
	handler->height = height;//input->inpRect->height + resultsBox->height;
	handler->input = input;
	handler->resultsBox = resultsBox;
	handler->data = data;
	handler->dataCount = dataCount;
	handler->dataLengths = dataLength; // make a copy

	handler->select = &selectSearchHandler;
	handler->hover = &hoverSearchHandler;

	return handler;

}

void drawSearch(SearchHandler* search) {

	/* general things */

	renderColor = COLOR_MAIN_FRONT;

	SearchInput* input = search->input;
	Button* inpRect = input->inpRect;

	renderColor = COLOR_MAIN_BACK;
	fillRect(search->x, search->y, search->width, search->height);

	SearchResultsBox* resultsBox = search->resultsBox;

	StringWriter stringWriter = StringWriterLeft();

	renderColor = COLOR_MAIN_FRONT;

	const int charsPerLine = 15;

	/* line margins */
	double marginRate = 0.05;
	int marginX = marginRate * inpRect->width;
	int marginY = marginRate * inpRect->height;

	int lineHg = inpRect->height - marginY * 2;
	int fullLineHg = inpRect->height;



	/* input */

	renderRect strRect = {
		inpRect->x + marginX,
		inpRect->y + marginY,
		inpRect->width - marginX * 2,
		lineHg
	};

	int strSz = strRect.width / charsPerLine;

	int strOffset = 0;
	int strLen = input->length + 1;
	if (strLen > charsPerLine) {
		
		strOffset = strLen - charsPerLine;
		strLen = charsPerLine;
		
	}

	if (input->hasFocus) {

		clock_t now = clock();
		if (now > cursorLastTime + cursorAnimLen) {
			cursor = (cursor == CURSOR_VISIBLE) ? CURSOR_HIDDEN : CURSOR_VISIBLE;
			cursorLastTime = now;
		}
	
	} else {

		cursor = CURSOR_HIDDEN;
	
	}

	input->buffer[input->length] = cursor;

	renderColor = WHITE;
	stringWriter.write(
		input->buffer + strOffset,
		strLen,
		0,
		strSz,
		&strRect
	);



	/* results box */

	int rbLen = (resultsBox->size < resultsBox->resultsCount) ? resultsBox->size : resultsBox->resultsCount;

	int rbX = resultsBox->x + marginX;
	int rbY = resultsBox->y + (search->height - fullLineHg * (resultsBox->size + 1));
	int rbWidth = resultsBox->width - marginX * 2;
	int rbHeight = lineHg;

	SearchResult** results = resultsBox->results;
	for (int i = 0; i < rbLen; i++) {

		renderColor = results[i]->backColor;
		results[i]->actvRect->draw(results[i]->actvRect);

		renderColor = results[i]->color;

		wchar_t* str = search->data[resultsBox->resultsOffset + i];
		strLen = search->dataLengths[resultsBox->resultsOffset + i];

		if (strLen > charsPerLine) {

			strLen = charsPerLine;

		}

		renderRect strRect = {
			rbX,
			rbY,
			rbWidth,
			rbHeight
		};

		strSz = strRect.width / charsPerLine;

		//renderColor = WHITE;
		stringWriter.write(
			str,
			strLen,
			results[i]->backColor,
			strSz,
			&strRect
		);

		rbY += fullLineHg;

	}

	renderColor = COLOR_MAIN_FRONT;
	drawRect(inpRect->x, inpRect->y, inpRect->width, inpRect->height);
	drawRect(search->x, search->y, search->width, search->height);

}

int selectSearchHandler(SearchHandler* search, POINT* mouseCoords) {

	search->input->hasFocus = search->input->inpRect->inBounds(search->input->inpRect, mouseCoords);

	int rSize = search->resultsBox->size;
	int rResults = search->resultsBox->resultsCount;
	int len = (rResults < rSize) ? rResults : rSize;

	for (int i = 0; i < len; i++) {

		SearchResult* result = search->resultsBox->results[i];
		if (result->actvRect->inBounds(result->actvRect, mouseCoords)) {

			search->resultsBox->selected = i;
			/*
			int resIdx = search->resultsBox->resultsOffset + i;
			memcpy(search->input->buffer, search->data[resIdx], sizeof(wchar_t) * search->dataLengths[resIdx]);
			search->input->length = search->dataLengths[resIdx];
			*/
			return i;

		}

	}

	return -1;

}

void setInput(SearchHandler* search, int idx) {
	
	int resIdx = search->resultsBox->resultsOffset + idx;
	memcpy(search->input->buffer, search->data[resIdx], sizeof(wchar_t) * search->dataLengths[resIdx]);
	search->input->length = search->dataLengths[resIdx];

}

int hoverSearchHandler(SearchHandler* search, POINT* mouseCoords) {

	int len = search->resultsBox->size;

	if (search->resultsBox->selected >= 0 && search->resultsBox->selected < len) {
		search->resultsBox->results[search->resultsBox->selected]->color = COLOR_MAIN_FRONT;
		search->resultsBox->results[search->resultsBox->selected]->backColor = COLOR_MAIN_BACK;
	}

	if (search->input->inpRect->inBounds(search->input->inpRect, mouseCoords)) {
		return 2;
	}

	for (int i = 0; i < len; i++) {

		SearchResult* result = search->resultsBox->results[i];
		if (result->actvRect != NULL && result->actvRect->inBounds(result->actvRect, mouseCoords)) {

			result->color = COLOR_SELECTION_FRONT;
			result->backColor = COLOR_SELECTION_BACK;

			search->resultsBox->selected = i;
			return 1;

		}

	}

	return 0;

}

void addChar(SearchHandler* searchHandler, wchar_t ch) {

	SearchInput* input = searchHandler->input;

	if (input->hasFocus != 1 || input->length >= input->maxLength) {
		return;
	}
	
	input->buffer[input->length] = ch;
	input->length++;

	search(searchHandler);

}

void removeChar(SearchHandler* searchHandler) {

	SearchInput* input = searchHandler->input;

	if (input->hasFocus != 1 || input->length <= 0) {
		return;
	}

	input->length--;
	input->buffer[input->length] = '\0';

	search(searchHandler);

}

void search(SearchHandler* searchHandler) {

	SearchInput* input = searchHandler->input;

	wchar_t* pattern = input->buffer;
	int patternLen = input->length;

	if (patternLen <= 0) {

		searchHandler->resultsBox->resultsOffset = -1;
		searchHandler->resultsBox->resultsCount = 0;
		
		return;
	
	}

	wchar_t** data = searchHandler->data;
	int dataCount = searchHandler->dataCount;

	if (dataCount <= 0) {

		searchHandler->resultsBox->resultsOffset = -1;
		searchHandler->resultsBox->resultsCount = 0;

		return;
	
	}

	int* dataLengths = searchHandler->dataLengths;

	/* assuming ASCII only for now */
	/* may expand later */

	int j = 0;
	int offset = 0;

	wchar_t** pData = data;
	int* pDataLengths = dataLengths;

	int upperBound = dataCount - 1;
	int bottomBound = 0;
	int idx = dataCount / 2;
	while (1) {

		wchar_t ptCh = pattern[j];
		toLowerCase(&ptCh);

		wchar_t ch = (j < pDataLengths[idx]) ? pData[idx][j] : L'\0';
		toLowerCase(&ch);

		if (ch == ptCh) {
			/* found, have to find boundaries, and move to the next letter */

			int firstIdx = 0;
			for (int i = idx - 1; i >= 0; i--) {
				if (pDataLengths[i] <= j || (pData[i][j] != pData[idx][j])) {
					firstIdx = i + 1;
					break;
				}
			}

			int lastIdx = idx;
			for (int i = idx + 1; i < dataCount; i++) {
				if (pData[i][j] != pData[idx][j]) {
					lastIdx = i - 1;
					break;
				}
				lastIdx = i;
			}

			dataCount = lastIdx - firstIdx + 1;

			j++;
			if (patternLen <= j) {

				searchHandler->resultsBox->resultsOffset = offset + firstIdx;
				searchHandler->resultsBox->resultsCount = dataCount;
				
				return;
			
			}

			offset += firstIdx;
			pData += firstIdx;
			pDataLengths += firstIdx;
			idx = dataCount / 2;
			
			upperBound = dataCount - 1;
			bottomBound = 0;
			
			continue;

		}

		int intLen = upperBound - bottomBound + 1;

		if (ch > ptCh) {
			
			upperBound = idx - 1;
			idx -= (intLen / 2);
			if (idx < bottomBound) idx = bottomBound;

		} else {

			bottomBound = idx + 1;
			idx += (intLen / 2);
			if (idx > upperBound) idx = upperBound;
		
		}

		if (bottomBound > upperBound) {
			/* not found */

			searchHandler->resultsBox->resultsOffset = -1;
			searchHandler->resultsBox->resultsCount = 0;

			return;

		}

	}

}

void toLowerCase(wchar_t* ch) {

	if (*ch >= L'A' && *ch <= L'Z') {
		*ch = *ch + (L'a' - L'A');
	}

}
