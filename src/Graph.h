#pragma once

#include <wchar.h>

typedef struct Graph {

	int x;
	int y;

	int width;
	int height;

	void* data;

	void (*draw) (struct Graph* graph);

} Graph;

typedef struct HistData {

	wchar_t* title;
	int titleLength;

	int* data;
	int* colors;

	int length;

} HistData;

typedef struct PlotData {

	wchar_t* title;
	int titleLength;

	int* x;
	int* y;

	int* colors;

	int length;

} PlotData;

Graph newPlot(int x, int y, int width, int height, PlotData* data);
Graph newHist(int x, int y, int width, int height, HistData* data);
Graph newHistRow(int x, int y, int width, int height, HistData* data);

void drawLegend(
	wchar_t** titles,
	int* titlesLengths,
	int titlesCount,
	int strSz,
	int* colors,
	int x,
	int y,
	int width,
	int height
);
