#pragma once

#include <windows.h>

#include "Colors.h"
#include "StringAlignment.h"

UINT32* pixels;

Color renderColor;

typedef struct renderRect {

    int x;
    int y;
    int width;
    int height;

} renderRect;

void render(HDC hdc, int width, int height);

int renderAlloc(int width, int height);

void renderClear();

void drawLine(int x1, int y1, int x2, int y2);
void fillRect(int x1, int y1, int wd, int hg);
void drawRect(int x1, int y1, int wd, int hg);
void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void drawString(wchar_t* string, int length, int size, int backColor, int x, int y);



/* 
*   trying to implement interface?
*   doesn't have much sense here, may use enum, but just testing function pointers
*/

/* kind of interface, that defines params */
typedef struct StringWriter {

    void (*write) (wchar_t* string, int length, int backColor, int size, renderRect* rect);

} StringWriter;

StringWriter StringWriterCenter(); 
StringWriter StringWriterLeft();
StringWriter StringWriterRight();
