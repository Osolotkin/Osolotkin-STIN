#pragma once

#include <stdlib.h>

#define SECONDS_PER_DAY 24 * 60 * 60


int maxIntArray(int* array, int length);
int getNumberOfDigits(int number);
int powerOfTen(int pow);

int bitwiseFind(char* array, int arrLen, char* del, int offset);
int bitwiseFindW(wchar_t* array, int arrLen, wchar_t* del, int offset);

int findArrayIdx(char* array, int arrLen, char ch, int offset);

int backwardCompare(char* text, int tLen, char* pattern, int pLen, int offset);
int backwardCompareW(wchar_t* text, int tLen, wchar_t* pattern, int pLen, int offset);

int trimIdxLeft(char* str, int length);
int trimIdxRight(char* str, int length);

void parseDateTime(char* zTime, int len, struct tm* dateTime); 
struct tm* parseDateTimeW(wchar_t* zTime, int len);
void roundTimeToDay(time_t* tm);

void getUTCOffset(char* zTime, int len, int* hrs, int* min);
void getLocalUTCOffset(int* hrs, int* min);

int readFullFile(wchar_t** buffer, FILE* file);
int writeFile(char* buffer, int bufferLen, char* fileName);
