/*
    ============================================================================================

        Stuff to render various things on the window, basically it has global pixel array, 
        which supposed to be filled somehow used various functions, and than scaled, and as 
        bitmap drawn on the window.

    ============================================================================================
*/

#pragma once

#include <windows.h>
#include <math.h>

#include "Render.h"
#include "Colors.h"
#include "Font.h"
#include "StringAlignment.h"

/* 
*   dimensions of the actual render
*   determinate size of the pixels
*/
int RENDER_WIDTH;
int RENDER_HEIGHT;

/* 
*   contains basic info about render 
*   determine the way pixels will be perceive while draw
*/
BITMAPINFO BITMAP_INFO = {

	.bmiHeader = {
		.biSize = sizeof(BITMAP_INFO.bmiHeader),
		.biWidth = 0, 
		.biHeight = 0,
		.biPlanes = 1,
		.biBitCount = 32,
		.biCompression = BI_RGB
	}

};


/* array of the pixels which will be drawn on the screen */
UINT32* pixels;

/* define color the next pixel or set of pixels will be drawn to the pixels array */
Color renderColor;

/*
*   has to be called when you need to draw pixels array on the window
*   preserves aspect ratio of the render dimensions
*/
void render(HDC hdc, int width, int height) {

    int x = width;
    int y = height;

    if (RENDER_HEIGHT * width > RENDER_WIDTH * height) {
        
        width = (RENDER_WIDTH * height) / RENDER_HEIGHT;
        x = (x - width) / 2;
        y = 0;
        
        BitBlt(hdc, 0, 0, x, height, 0, 0, 0, BLACKNESS);
        BitBlt(hdc, x + width, 0, x, height, 0, 0, 0, BLACKNESS);

    } else {
        
        height = (RENDER_HEIGHT * width) / RENDER_WIDTH;
        y = (y - height) / 2;
        x = 0;

        BitBlt(hdc, 0, 0, width, y, 0, 0, 0, BLACKNESS);
        BitBlt(hdc, 0, y + height, width, y, 0, 0, 0, BLACKNESS);

    }

	StretchDIBits(
		hdc,
		x,
		y,
		width,
		height,
		0,
		0,
		RENDER_WIDTH,
		RENDER_HEIGHT,
		pixels,
		&BITMAP_INFO,
		DIB_RGB_COLORS,
		SRCCOPY
	);

}

/*
*   allocates pixels array and sets render dimensions
*   returns 1 if alloc failed
*   otherwise 0
*/
int renderAlloc(int width, int height) {

	pixels = VirtualAlloc(
		0, 
		width * height * sizeof(UINT32),
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (pixels == NULL) return 1;

	BITMAP_INFO.bmiHeader.biWidth = width;
	BITMAP_INFO.bmiHeader.biHeight = -height;

    RENDER_WIDTH = width;
    RENDER_HEIGHT = height;

	return 0;

}

/*
*   reallocates array of pixels to the desired size
*   changes render dimensions accordingly
*   returns 1 if alloc failed
*   otherwise 0
*   never used, never tested
*/
int renderResize(int width, int height) {

	UINT32* tmp = (UINT32*) realloc(pixels, width * height * sizeof(UINT32));
	if (tmp == NULL) {
		return 1;
	}
	pixels = tmp;

    BITMAP_INFO.bmiHeader.biWidth = width;
    BITMAP_INFO.bmiHeader.biHeight = -height;

    RENDER_WIDTH = width;
    RENDER_HEIGHT = height;
	
	return 0;

}

/*
*   fills a rectangle of desire width and height at desire coords 
*   tries to do something with out of bounds situations,
*   but I don't really think that works properly
*   never tested
*/
void fillRect(int x1, int y1, int wd, int hg) {

	int width = BITMAP_INFO.bmiHeader.biWidth;
	int height = -BITMAP_INFO.bmiHeader.biHeight;

	if (y1 < 0) y1 = 0;
	int yMax = y1 + hg;
	if (yMax > height) yMax = height;

	if (x1 < 0) x1 = 0;
	int xMax = x1 + wd;
	if (xMax > width) xMax = width;

	for (int y = y1; y < yMax; y++) {

		int offset = y * width;
		for (int x = x1; x < xMax; x++) {
			pixels[offset + x] = renderColor;
		}

	}

}

/*
*   draws a rectangle of desire width and height at desire coords
*   tries to do something with out of bounds situations,
*   but I don't really think that works properly
*   never tested
*/
void drawRect(int x1, int y1, int wd, int hg) {

    int width = BITMAP_INFO.bmiHeader.biWidth;
    int height = -BITMAP_INFO.bmiHeader.biHeight;

    if (y1 < 0) y1 = 0;
    int yMax = y1 + hg;
    if (yMax > height) yMax = height - 1;

    if (x1 < 0) x1 = 0;
    int xMax = x1 + wd;
    if (xMax > width) xMax = width - 1;

    for (int y = y1; y < yMax; y++) {
        pixels[width * y + x1] = renderColor;
        pixels[width * y + xMax - 1] = renderColor;
    }

    int offset = y1 * width;
    int offsetMax = yMax * width;
    for (int x = x1; x < xMax; x++) {
        pixels[offset + x] = renderColor;
        pixels[offsetMax + x] = renderColor;
    }

}

/* fills the entire pixels array with zeros (black) */
void renderClear() {

    int tmpColor = renderColor;
    renderColor = 0;

    fillRect(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

    renderColor = tmpColor;

}

typedef struct Line {

    int* x;
    int* y;
    int length;

} Line;

int allocLine(Line** line, int length) {

    (*line) = (Line*) malloc(sizeof(Line));
    if ((*line) == NULL) return 1;

    (*line)->x = (int*) malloc(length * sizeof(int));
    if ((*line)->x == NULL) return 1;

    (*line)->y = (int*) malloc(length * sizeof(int));
    if ((*line)->y == NULL) return 1;

    (*line)->length = length;

    return 0;

}

void freeLine(Line** line) {

    free((*line)->x);
    free((*line)->y);
    free((*line));

}

/*
*   draws a line from [x1, x2] to [x2, y2] points
*   somehow takes care about out of bounds, but you do better not to be sure about it
*   it's pretty general inside, like the main loop part, may be faster if
*   some magic copy paste would happen
*/
void drawLine(int x1, int y1, int x2, int y2) {

    const int WIDTH = RENDER_WIDTH;
    const int HEIGHT = RENDER_HEIGHT;

    if (x1 >= WIDTH && x2 >= WIDTH) return;
    if (y1 >= HEIGHT && y2 >= HEIGHT) return;
    if (y1 < 0 && y2 < 0) return;

    if (x1 >= WIDTH) x1 = WIDTH - 1;
    if (x2 >= WIDTH) x2 = WIDTH - 1;

    if (x1 < 0) x1 = 0;
    if (x2 < 0) x2 = 0;

    if (y1 >= HEIGHT) y1 = HEIGHT - 1;
    if (y2 >= HEIGHT) y2 = HEIGHT - 1;

    if (y1 < 0) y1 = 0;
    if (y2 < 0) y2 = 0;

    int yDir;
    int xDir;
    int width;
    int height;

    if (x1 > x2) {
        xDir = -1;
        width = x1 - x2 + 1;
    }
    else {
        xDir = 1;
        width = x2 - x1 + 1;
    }
    if (y1 > y2) {
        yDir = -1;
        height = y1 - y2 + 1;
    }
    else {
        yDir = 1;
        height = y2 - y1 + 1;
    }

    int stepWidth = WIDTH;
    int stepHeight = 1;
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;
    double step = width / (double)height;
    if (step < 1) {

        stepWidth = 1;
        stepHeight = WIDTH;
        screenWidth = HEIGHT;
        screenHeight = WIDTH;

        int tmp = x1;
        x1 = y1;
        y1 = tmp;
        y2 = x2;

        tmp = xDir;
        xDir = yDir;
        yDir = tmp;

        step = 1 / step;
    
    }

    int rStep = (int) round(step);
    int offsetX = x1;
    double overstep = 0;

    int y = y1;
    while (y != y2 + yDir) {

        int offsetY = y * stepWidth;
        int x = offsetX;
        
        while (x != offsetX + xDir * rStep) {
            pixels[offsetY + x * stepHeight] = renderColor;
            x = x + xDir;
        }
        
        offsetX = offsetX + xDir * rStep;
        overstep += rStep - step;
        
        rStep = (int) round(step - overstep);
        if (offsetX + rStep > screenWidth) return;

        y = y + yDir;
    
    }

}

/*
*   specific function for internal usage
*   returns drawn line and doesnt check for out of bounds
*/
Line* drawAndGetLine(int x1, int y1, int x2, int y2) {

    // cool naming KEKW
    int WIDTH = BITMAP_INFO.bmiHeader.biWidth;
    int HEIGHT = -BITMAP_INFO.bmiHeader.biHeight;

    int yDir;
    int xDir;
    int width;
    int height;

    if (x1 > x2) {
        xDir = -1;
        width = x1 - x2 + 1;
    }
    else {
        xDir = 1;
        width = x2 - x1 + 1;
    }
    if (y1 > y2) {
        yDir = -1;
        height = y1 - y2 + 1;
    }
    else {
        yDir = 1;
        height = y2 - y1 + 1;
    }

    int stepWidth = WIDTH;
    int stepHeight = 1;
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;
    int swaped = 0;
    double step = width / (double) height;
    if (step < 1) {
        stepWidth = 1;
        stepHeight = WIDTH;
        screenWidth = HEIGHT;
        screenHeight = WIDTH;

        int tmp = x1;
        x1 = y1;
        y1 = tmp;

        tmp = y2;
        y2 = x2;
        x2 = tmp;

        tmp = xDir;
        xDir = yDir;
        yDir = tmp;

        swaped = 1;
        step = 1 / step;
    }

    int rStep = (int) round(step);
    int offsetX = x1;
    double overstep = 0;

    Line* line;
    int lineLength = (!swaped) ? abs(y2 - y1) + 1 : abs(x2 - x1) + 1;
    if (allocLine(&line, lineLength)) return NULL;

    int lineIndex = 0;

    int y = y1;
    while (y != y2 + yDir) {

        int offsetY = y * stepWidth;
        int x = offsetX;

        while (x != offsetX + xDir * rStep) {

            if (swaped) {
                line->x[lineIndex] = y;
                line->y[lineIndex] = x;
                lineIndex++;
            }
            pixels[offsetY + x * stepHeight] = renderColor;
            x += xDir;
        
        }
        
        if (!swaped) {
            line->x[lineIndex] = x - xDir;
            line->y[lineIndex] = y;
            lineIndex++;
        }
        
        offsetX = offsetX + xDir * rStep;
        overstep += rStep - step;
        
        rStep = (int) round(step - overstep);
        
        y = y + yDir;

    }

    return line;
}

/*
*   drwas filled triangle
*   dont think it works how it suppose to do, but it does its job
*/
void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {

    int width = BITMAP_INFO.bmiHeader.biWidth;
    int height = -BITMAP_INFO.bmiHeader.biHeight;

    /* pick most higher point */
    int tx, ty;
    int sx1, sy1;
    int sx2, sy2;

    if (y1 < y2) {
        if (y1 < y3) {
            /* y1 */
            tx = x1;
            ty = y1;
            sx1 = x2;
            sy1 = y2;
            sx2 = x3;
            sy2 = y3;
        }
        else {
            /* y3 */
            tx = x3;
            ty = y3;
            sx1 = x1;
            sy1 = y1;
            sx2 = x2;
            sy2 = y2;
        }
    }
    else {
        if (y2 < y3) {
            /* y2 */
            tx = x2;
            ty = y2;
            sx1 = x1;
            sy1 = y1;
            sx2 = x3;
            sy2 = y3;
        }
        else {
            /* y3 */
            tx = x3;
            ty = y3;
            sx1 = x1;
            sy1 = y1;
            sx2 = x2;
            sy2 = y2;
        }
    }

    /* checking bounds */
    if (sx1 >= width) sx1 = width - 1;
    if (sx2 >= width) sx2 = width - 1;

    if (sx1 < 0) sx1 = 0;
    if (sx2 < 0) sx2 = 0;

    if (sy1 >= height) sy1 = height - 1;
    if (sy2 >= height) sy2 = height - 1;

    if (sy1 < 0) sy1 = 0;
    if (sy2 < 0) sy2 = 0;

    if (tx >= width) tx = width - 1;

    if (tx < 0) tx = 0;

    if (ty >= height) ty = height - 1;

    if (ty < 0) ty = 0;

    /* drawing and getting pixels of the smaller sides */
    Line* lineA = drawAndGetLine(sx1, sy1, tx, ty);
    Line* lineB = drawAndGetLine(sx2, sy2, tx, ty);

    /* searching for the top point to set dirs and start indexes */
    int dirA = 1;
    int dirB = 1;
    int indexA = 0;
    int indexB = 0;
    if (lineA->y[0] != lineB->y[0]) {
        if (lineA->y[0] != lineB->y[0]) {
            /* for lineA its last, for lineB its unknown yet */
            if (lineA->y[0] != lineB->y[0]) {
                /* for lineA its last, for lineB its last */
                dirA = -1;
                indexA = lineA->length - 1;
                dirB = -1;
                indexB = lineB->length - 1;
            }
            else {
                /* for lineA its last, for lineB its first */
                dirA = -1;
                indexA = lineA->length - 1;
            }
        }
        else {
            /* for lineA its first, for lineB its last */
            dirB = -1;
            indexB = lineB->length - 1;
        }
    }

    int minLen = min(lineA[0].length, lineB[0].length);
    for (int i = 0; i < minLen; i++) {

        int diff = lineB->x[indexB] - lineA->x[indexA];
        int start;
        int end;
        int y;
        if (diff < 0) {
            start = lineB->x[indexB];
            end = lineA->x[indexA];
            y = lineA->y[indexA];
        }
        else {
            start = lineA->x[indexA];
            end = lineB->x[indexB];
            y = lineA->y[indexA];
        }

        for (int x = start; x < end; x++) {
            pixels[width * y + x] = renderColor;
        }

        indexB += dirB;
        indexA += dirA;

    }

    /* draw and get remaining line */
    Line* lineC = drawAndGetLine(sx1, sy1, sx2, sy2);

    /* lineA will represent line that has extra points */
    if (lineA->length < lineB->length) {
        Line* tmp = lineA;
        lineA = lineB;
        freeLine(&tmp);

        dirA = dirB;
        indexA = indexB;
    }
    dirA = -dirA;
    indexA = (dirA > 0) ? 0 : 1;

    int dirC = 1;
    int indexC = 0;
    /* again searching for the middle point */
    if (lineA->y[0] != lineC->y[0]) {
        /* for C its last */
        dirC = -1;
        indexC = lineC->length - 1;
    }

    minLen = min(lineC->length, lineA->length);
    for (int i = 0; i < minLen; i++) {

        int diff = lineC->x[indexC] - lineA->x[indexA];
        int start;
        int end;
        int y;
        if (diff < 0) {
            start = lineC->x[indexC];
            end = lineA->x[indexA];
            y = lineA->y[indexA];
        }
        else {
            start = lineA->x[indexA];
            end = lineC->x[indexC];
            y = lineA->y[indexA];
        }

        for (int x = start; x < end; x++) {
            pixels[width * y + x] = renderColor;
        }

        indexC += dirC;
        indexA += dirA;

    }

    freeLine(&lineA);
    freeLine(&lineC);

}

/*
*   draws string at specific position with specific size of char and back color
*   char size of in pixels
*/
void drawString(wchar_t* string, int length, int backColor, int size, int x, int y) {

    int top = y;
    int left = x;

    for (int i = 0; i < length; i++) {
    
        int ch = string[i];
        
        x = 0;
        y = top;
        int offset = CHAR_LENGTH * (ch - CHAR_OFFSET);
        for (int j = offset; j < offset + CHAR_LENGTH; j++) {

            if (x >= CHAR_SIZE) {
                x = 0;
                y--;
            }

            int xx = (x * size) / CHAR_SIZE;
            int yy = (y * size) / CHAR_SIZE;
            pixels[RENDER_WIDTH * yy + left + xx] = (BMP_FONT[j]) ? renderColor : backColor;
            
            x++;
        
        }

        left += size;
    
    }

}

/*
*   here comes some experemental stuff
*   have discovered that function pointer exists
*/

/*
*   basic function that will draw any string and is used by others, 
*   basically have additional param StringAlignment
*/
void drawStringEx(wchar_t* string, int length, StringAlignment alignment, int backColor, int size, renderRect* rect) {

    int dotedEnd = 0;
    wchar_t tmpCh[2];
    
    if (size * length > rect->width) {

        length = rect->width / size;

        if (length > 1) {
            
            dotedEnd = 1;
            
            tmpCh[0] = string[length - 2];
            tmpCh[1] = string[length - 1];

            string[length - 2] = L'.';
            string[length - 1] = L'.';

        }
    
    }

    int width = BITMAP_INFO.bmiHeader.biWidth;
    int height = -BITMAP_INFO.bmiHeader.biHeight;

    int top;
    int left;

    switch (alignment) {

        case CENTER: {

            top = rect->y + (rect->height - size) / 2;
            left = rect->x + (rect->width - length * size) / 2;
            break;

        }

        case LEFT: {

            top = rect->y + (rect->height - size) / 2;
            left = rect->x;

            break;

        }

        case RIGHT: {

            top = rect->y + (rect->height - size) / 2;
            left = rect->width + rect->x - length * size;
            break;

        }

        default: {

            top = rect->y;
            left = rect->x;

        }

    }

    for (int i = 0; i < length; i++) {

        int ch = string[i];

        int x = 0;
        int y = top;
        int offset = CHAR_LENGTH * (ch - CHAR_OFFSET);
        for (int j = offset; j < offset + CHAR_LENGTH; j++) {

            if (x >= CHAR_SIZE) {
                x = 0;
                y--;
            }

            int xx = (x * size) / CHAR_SIZE;
            int yy = top + ((top - y) * size) / CHAR_SIZE;
            pixels[width * yy + left + xx] = (BMP_FONT[j]) ? renderColor : backColor;

            x++;

        }

        left += size;

    }

    if (dotedEnd) {
        string[length - 2] = tmpCh[0];
        string[length - 1] = tmpCh[1];
    }

}



void drawStringCenter(wchar_t* string, int length, int backColor, int size, renderRect* rect) {

    drawStringEx(string, length, CENTER, backColor, size, rect);

}

/* using interface to write center-align string */
StringWriter StringWriterCenter() {

    StringWriter strWrt = {
        &drawStringCenter
    };
    return strWrt;

}




void drawStringLeft(wchar_t* string, int length, int backColor, int size, renderRect* rect) {

    drawStringEx(string, length, LEFT, backColor, size, rect);

}

/* using interface to write left-align string */
StringWriter StringWriterLeft() {

    StringWriter strWrt = {
        &drawStringLeft
    };
    return strWrt;

}




void drawStringRight(wchar_t* string, int length, int backColor, int size, renderRect* rect) {

    drawStringEx(string, length, RIGHT, backColor, size, rect);

}

/* using interface to write right-align string */
StringWriter StringWriterRight() {

    StringWriter strWrt = {
        &drawStringRight
    };
    return strWrt;

}
