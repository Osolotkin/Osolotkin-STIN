/*
	============================================================================================

		Supposed to provide various stuff to draw several different graphs
		just histo style graphs are implemented...

	============================================================================================
*/

#pragma once

#include <stdio.h>
#include "Graph.h"
#include "Render.h"
#include "Utils.h"
#include "Config.h"

/*
*	supposed to draw plot graph
*	does nothing
*/
void plot(Graph* plot) {

}

/*
*	creates Graph struct with plot preset
*/
Graph newPlot(int x, int y, int width, int height, PlotData* data) {

	Graph graph = {
		x, 
		y, 
		width, 
		height,
		data,
		&plot
	};

	return graph;

}

/*
*	draws horizontal histo graph
*/
void histRow(Graph* hist) {

	HistData* data = (HistData*)hist->data;

	int barWidth = (hist->height) / data->length;

	int maxVal = maxIntArray(data->data, data->length);
	int digits = getNumberOfDigits(maxVal) - 1;
	int tmp = powerOfTen((digits - 1) >= 0 ? digits - 1 : 0);
	maxVal = (maxVal - maxVal % tmp) + tmp;

	int width = hist->width;
	int offset = hist->y;
	for (int i = 0; i < data->length; i++) {

		int barHeight = (int)(width * ((double)data->data[i] / maxVal));

		renderColor = data->colors[i];
		fillRect(hist->x + width - barHeight, offset, barHeight, barWidth);

		offset += barWidth;

	}

}

/*
*	creates Graph struct with histo preset, histo will be drawn as
*	horizontal one
*/
Graph newHistRow(int x, int y, int width, int height, HistData* data) {

	Graph graph = {
		x,
		y,
		width,
		height,
		data,
		&histRow
	};

	return graph;

}

/*
*	draws histo graph
*/
void hist(Graph* hist) {

	HistData* data = (HistData*)hist->data;

	const double marginRate = 0.1;
	const int margin = (int)(hist->width * marginRate);

	const double titleRate = 0.2;
	const int titleHeight = (int)(hist->height * titleRate);

	StringWriter stringWriter = StringWriterCenter();
	renderRect strRect = {
		hist->x,
		hist->y,
		hist->width,
		titleHeight
	};

	// have to think more, but for now ok
	int strSz = titleHeight - 0.6 * titleHeight;

	renderColor = COLOR_MAIN_FRONT;
	stringWriter.write(
		data->title,
		data->titleLength,
		COLOR_MAIN_BACK,
		strSz,
		&strRect
	);

	int barWidth = (hist->width - 2 * margin) / data->length;


	int maxVal = maxIntArray(data->data, data->length);
	int digits = getNumberOfDigits(maxVal) - 1;
	int tmp = powerOfTen((digits - 1) >= 0 ? digits - 1 : 0);
	maxVal = (maxVal - maxVal % tmp) + tmp;

	int height = hist->height;
	int offset = hist->x + margin;
	for (int i = 0; i < data->length; i++) {

		int barHeight = (int) ((height - titleHeight) * ((double) data->data[i] / maxVal));

		renderColor = data->colors[i];
		fillRect(offset, hist->y + height - barHeight, barWidth, barHeight);

		offset += barWidth;

	}

}

/*
*	creates Graph struct with histo preset
*/
Graph newHist(int x, int y, int width, int height, HistData* data) {

	Graph graph = {
		x,
		y,
		width,
		height,
		data,
		&hist
	};

	return graph;

}

/*
*	tries to draw legend at the specific coords with specific size
*/
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
) {
	
	int margin = width / 10;

	int sqrSz = strSz / 2;
	int lnSpc = sqrSz;

	x = x + margin;
	y = y + ((height - (strSz + lnSpc) * titlesCount + lnSpc) / 2);

	StringWriter stringWriter = StringWriterLeft();

	renderColor = COLOR_MAIN_FRONT;
	for (int i = 0; i < titlesCount; i++) {
		
		renderColor = colors[i];
		fillRect(x, y + (strSz - sqrSz) / 2, sqrSz, sqrSz);

		int strX = x + margin / 2 + sqrSz;
		renderRect strRect = {
			strX,
			y,
			width - (strX - x + margin),
			strSz
		};

		renderColor = COLOR_MAIN_FRONT;
		stringWriter.write(
			titles[i],
			titlesLengths[i],
			0,
			strSz,
			&strRect
		);

		y += strSz + lnSpc;
	
	}

}
