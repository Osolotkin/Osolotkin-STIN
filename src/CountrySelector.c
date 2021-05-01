/*
    ============================================================================================

        Basicly contains stuff to build struct CountrySelector, that allows to search across
        countries list, given as file, file format is specified in separate document 
        README.txt, also creates struct SearchHandler, that is used to search list

    ============================================================================================
*/

#pragma once

#include <stdlib.h>
#include "Config.h"
#include "Search.h"
#include "Render.h"
#include "CountrySelector.h"

int selectCountry(CountrySelector* selector, POINT* coords); 
int hoverCountrySelector(CountrySelector* selector, POINT* coords);

/*
*   kind of constructor
*/
CountrySelector* newCountrySelector(int* colors, int countriesToSelect, const char* countriesFile, int x, int y, int width, int height) {

    CountrySelector* selector = (CountrySelector*) malloc(sizeof(CountrySelector));
    if (selector == NULL) {
        return NULL;
    }

    selector->x = x;
    selector->y = y;
    selector->width = width;
    selector->height = height;
    selector->selected = 1;
    selector->visible = 0;

    selector->select = &selectCountry;
    selector->hover = &hoverCountrySelector;

    selector->toggleButton = (ToggleButton*) malloc(sizeof(ToggleButton) * countriesToSelect);
    if (selector->toggleButton == NULL) {
        return NULL;
    }
    selector->toggleButton->color = WHITE;
    selector->toggleButton->backColor = BLACK;

    selector->selectButtons = (SelectButton**) malloc(sizeof(SelectButton*) * countriesToSelect);
    if (selector->selectButtons == NULL) {
        return NULL;
    }
    selector->buttonsCount = countriesToSelect;

    double divider = 0.1;

    int slX = x;
    int slY = y;
    int slWidth = width;
    int slHeight = height * divider;

    int tgbSz = 0.05 * width;
    int tgbX = x + width - (slHeight - tgbSz) / 2 - tgbSz;
    int tgbY = y + (slHeight - tgbSz) / 2;
    
    Button* button = newButton(tgbX, tgbY, tgbSz, tgbSz);
    if (button == NULL) {
        return NULL;
    }
    selector->toggleButton->actvRect = button;

    int buttSz = slHeight / 2;
    int buttMargin = buttSz;

    int buttX = slX + (slWidth - countriesToSelect * buttSz - (countriesToSelect - 1) * buttMargin) / 2;
    int buttY = slY + (slHeight - buttSz) / 2;

    for (int i = 0; i < countriesToSelect; i++) {

        selector->selectButtons[i] = (SelectButton*) malloc(sizeof(SelectButton));
        if (selector->selectButtons[i] == NULL) {
            return NULL;
        }

        Button* button = newButton(buttX, buttY, buttSz, buttSz);
        if (button == NULL) {
            return NULL;
        }

        selector->selectButtons[i]->actvRect = button;
        /* the same states can ocure, better generate permutation, but will do it later, maybe */
        selector->selectButtons[i]->selected = i;
        selector->selectButtons[i]->color = colors[i];

        buttX += buttSz + buttMargin;

    }

    if (loadCountries(selector, countriesFile)) {
        return NULL;
    }

    int srX = x;
    int srY = slY + slHeight;

    int srWidth = width;
    int srHeight = height - slHeight;

    int results = countriesToSelect;
    
    SearchHandler* searchHandler = newSearchHandler(
        selector->countries,
        selector->countriesLengths,
        selector->countriesCount,
        srX,
        srY,
        srWidth,
        srHeight,
        results
    );
    if (searchHandler == NULL) {
        return NULL;
    }

    selector->searchHandler = searchHandler;
    
	return selector;

};

/*
*   just draw CountrySelector to the global pixel array
*/
void drawCountriesSelector(CountrySelector* selector) {

    ToggleButton* tgButt = selector->toggleButton;
    renderColor = tgButt->color;

    StringWriter stringWriter = StringWriterLeft();
    renderRect strRect = {
        tgButt->actvRect->x,
        tgButt->actvRect->y,
        tgButt->actvRect->width,
        tgButt->actvRect->height
    };

    wchar_t* tgCh = (selector->visible) ? L"-" : L"+";

    stringWriter.write(
        tgCh,
        1,
        tgButt->backColor,
        tgButt->actvRect->height,
        &strRect
    );

    if (selector->visible != 1) return;

    renderColor = COLOR_MAIN_FRONT;
    drawRect(selector->x, selector->y, selector->width, selector->height);

    for (int i = 0; i < selector->buttonsCount; i++) {
        
        SelectButton* button = selector->selectButtons[i];
        renderColor = button->color;
        button->actvRect->draw(button->actvRect);

    }

    SelectButton* selButton = selector->selectButtons[(selector->selected - 1)];
    renderColor = COLOR_MAIN_FRONT;
    drawRect(selButton->actvRect->x, selButton->actvRect->y, selButton->actvRect->width, selButton->actvRect->height);

    renderColor = COLOR_MAIN_FRONT;
    drawSearch(selector->searchHandler);

}

/*
*   returns 0 if nothing useful happened
*   returns 1 if selected country was changed
*   returns 2 if other country was selected
*   returns -1 if selector becomed hidden
*   returns -2 if selector becomed visible
*/
int selectCountry(CountrySelector* selector, POINT* coords) {

    ToggleButton* tgButt = selector->toggleButton;
    if (tgButt->actvRect->inBounds(tgButt->actvRect, coords)) {

        selector->visible = !selector->visible;

        if (selector->visible == 1) {
            wcscpy_s(
                selector->searchHandler->input->buffer, 
                selector->searchHandler->input->maxLength, 
                selector->countries[selector->selectButtons[selector->selected - 1]->selected]
            );
            selector->searchHandler->input->length = selector->countriesLengths[selector->selectButtons[selector->selected - 1]->selected];
            
            return -2;
        }
    
    }

    if (selector->visible != 1) return -1;

    SearchHandler* srch = selector->searchHandler;
    int idx = srch->select(srch, coords);
    int offset = srch->resultsBox->resultsOffset;

    if (idx >= 0) {

        int isSelected = 0;
        for (int i = 0; i < selector->buttonsCount; i++) {
            if (selector->selectButtons[i]->selected == offset + idx) {
                isSelected = 1;
                break;
            }
        }

        if (isSelected) return 0;
        setInput(srch, idx);

        selector->selectButtons[selector->selected - 1]->selected = offset + idx;
        return 1;
    
    }

    for (int i = 0; i < selector->buttonsCount; i++) {
        
        if (selector->selectButtons[i]->actvRect->inBounds(selector->selectButtons[i]->actvRect, coords)) {
            
            selector->selected = i + 1;
            setInput(srch, selector->selectButtons[i]->selected - offset);
            return 2;

        }
    
    }

    return 0;

}

/*
*   returns 0 if nothing useful happened
*   returns 1 - 9 are returns from SearchHandler, take a look at it documentation
*   returns 10 if mouse was over toggle button
*   returns 11 if mouse was over any country selection button
*/
int hoverCountrySelector(CountrySelector* selector, POINT* coords) {

    if (selector->visible == 1) {
        int tmp = selector->searchHandler->hover(selector->searchHandler, coords);
        if (tmp > 0) {
            return tmp;
        };
    }

    ToggleButton* tgButt = selector->toggleButton;
    if (tgButt->actvRect->inBounds(tgButt->actvRect, coords)) {
        return 10;
    }

    for (int i = 0; i < selector->buttonsCount; i++) {

        if (selector->selectButtons[i]->actvRect->inBounds(selector->selectButtons[i]->actvRect, coords)) {

            return 11;

        }

    }

    return 0;

}

/*
*   returns 1 if success, otherwise 0
*/
int loadCountries(CountrySelector* selector, char* fileName) {

    FILE* file;

    file = fopen(fileName, "r");
    if (file == NULL) {
        return 1;
    }

    wchar_t* buffer;
    int length;

    length = readFullFile(&buffer, file);
    if (length <= 0) return;

    selector->countriesCount = 0;

    int i = 0;
    for (i; i < length; i++) {
        if (buffer[i] == L'\n') {

            buffer[i] = L'\0';
            selector->countriesCount = wcstol((wchar_t*) buffer, NULL, 10);

            break;

        }
    }

    int start = i + 1;
    int cnt = 0;
    for (i; i < length; i++) {
        
        if (buffer[i] == L'\n') {

            buffer[i] = L'\0';
            selector->selectButtons[cnt]->selected = wcstol((wchar_t*) buffer + start, NULL, 10) - 1;
            if (selector->selectButtons[cnt]->selected < 0) return 1;
            
            start = i + 1;
            if (cnt >= 3) break;
            cnt++;

        }

    }

    selector->countries = (wchar_t**) malloc(selector->countriesCount * sizeof(wchar_t*));
    if (selector->countries == NULL) {
        free(buffer);
        return 1;
    }

    selector->countriesLengths = (int*) malloc(selector->countriesCount * sizeof(int));
    if (selector->countriesLengths == NULL) {
        free(buffer);
        return 1;
    }

    int lineIdx = 0;
    int lineStart = i + 1;
    for (i; i < length; i++) {
        if (buffer[i] == '\n') {

            int len = i - lineStart + 1;
            buffer[i] = '\0';

            selector->countries[lineIdx] = (wchar_t*) malloc(len * sizeof(wchar_t));
            if (selector->countries[lineIdx] == NULL) {

                for (int i = 0; i < lineIdx; i++) {
                    free(selector->countries[i]);
                }
                free(selector->countries);

                free(buffer);

                return 1;

            }

            memcpy(selector->countries[lineIdx], buffer + lineStart, sizeof(wchar_t) * len);

            selector->countriesLengths[lineIdx] = len - 1;

            lineStart = i + 1;
            lineIdx++;

        }
    }

    free(buffer);

    return 0;

}
