#pragma once

#include <time.h>
#include <direct.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
*	finds max value in given array
*/
int maxIntArray(int* array, int length) {

	int max = 0;
	for (int i = 0; i < length; i++) {
		if (array[i] > max) max = array[i];
	}
	return max;

}

/* 
*	https://stackoverflow.com/questions/1306727/way-to-get-number-of-digits-in-an-int 
*/
int getNumberOfDigits(int number) {

    if (number < 100000) {
        // 5 or less
        if (number < 100) {
            // 1 or 2
            return number < 10 ? 1 : 2;
        }
        else {
            // 3 or 4 or 5
            if (number < 1000)
                return 3;
            else {
                // 4 or 5
                return number < 10000 ? 4 : 5;
            }
        }
    }
    else {
        // 6 or more
        if (number < 10000000) {
            // 6 or 7
            return number < 1000000 ? 6 : 7;
        }
        else {
            // 8 to 10
            if (number < 100000000)
                return 8;
            else {
                // 9 or 10
                return number < 1000000000 ? 9 : 10;
            }
        }
    }

}

/* 
*	https://stackoverflow.com/questions/46983772/fastest-way-to-obtain-a-power-of-10
*/
const int POWERS_OF_10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
int powerOfTen(int pow) {
    return POWERS_OF_10[pow];
}

/*
*	is designed to use just for two chars, no tests included
*	assuming that char size is 16 bits
*	returns index of the first occurence, -1 if nothing founc
*/
int bitwiseFind(char* array, int arrLen, char* del, int offset) {

    const int pattern = (del[0] << 16) | del[1];

    for (int i = offset; i < arrLen - 1; i++) {

        if (((array[i] << 16) | array[i + 1]) == pattern) {
            return i;
        }

    }

    return -1;

}

/*
*	wide char function
*	is designed to use just for two chars, no tests included
*	assuming that char size is 16 bits
*	returns index of the first occurence, -1 if nothing founc
*/
int bitwiseFindW(wchar_t* array, int arrLen, wchar_t* del, int offset) {

    const int pattern = (del[0] << 16) | del[1];

    for (int i = offset; i < arrLen - 1; i++) {

        if (((array[i] << 16) | array[i + 1]) == pattern) {
            return i;
        }

    }

    return -1;

}

/*
*	compares strings from behind
*	returns 1 if text, with applied offset was ends with 
*	otherwise 0
*/
int backwardCompare(char* text, int tLen, char* pattern, int pLen, int offset) {

    if (pLen > offset || tLen < pLen) return 0;

    offset = offset - pLen + 1;
    for (int i = pLen - 1; i >= 0; i--) {
        if (pattern[i] != text[i + offset]) return 0;
    }

    return 1;

}

/*
*	wide char function 
*	compares strings from behind
*	returns 1 if text, with applied offset was ends with 
*	otherwise 0
*/
int backwardCompareW(wchar_t* text, int tLen, wchar_t* pattern, int pLen, int offset) {

    if (pLen > offset || tLen < pLen) return 0;

    offset = offset - pLen + 1;
    for (int i = pLen - 1; i >= 0; i--) {
        if (pattern[i] != text[i + offset]) return 0;
    }

    return 1;

}

/*
*	parse given datatime as null-terminated string in format YYYY-MM-DD-HH-MM-SS
*	returns parsed date time into given buffer of struct tm
*/
void parseDateTime(char* zTime, int len, struct tm* dateTime) {

    if (len < 20) {
        return;
    };

    char* tmp = zTime;

    zTime[4] = '\0';
    dateTime->tm_year = strtol(tmp, NULL, 10);

    tmp += 5;
    zTime[7] = '\0';
    dateTime->tm_mon = strtol(tmp, NULL, 10);

    tmp += 3;
    zTime[10] = '\0';
    dateTime->tm_mday = strtol(tmp, NULL, 10);

    tmp += 3;
    zTime[13] = '\0';
    dateTime->tm_hour = strtol(tmp, NULL, 10);

    tmp += 3;
    zTime[16] = '\0';
    dateTime->tm_min = strtol(tmp, NULL, 10);

    tmp += 3;
    zTime[19] = '\0';
    dateTime->tm_sec = strtol(tmp, NULL, 10);
	
}

/*
*	wide char function
*	parse given datatime as null-terminated string in format YYYY-MM-DDTHH:MM:SS
*	returns parsed date time into given buffer of struct tm
*/
void parseDateTimeW(wchar_t* zTime, int len, struct tm* dateTime) {

    if (len < 20) {
        return;
    };

    struct tm dateTime;

    wchar_t* tmp = zTime;

    zTime[4] = '\0';
    dateTime.tm_year = wcstol(tmp, NULL, 10);

    tmp += 5;
    zTime[7] = '\0';
    dateTime.tm_mon = wcstol(tmp, NULL, 10);

    tmp += 3;
    zTime[10] = '\0';
    dateTime.tm_mday = wcstol(tmp, NULL, 10);

    tmp += 3;
    zTime[13] = '\0';
    dateTime.tm_hour = wcstol(tmp, NULL, 10);

    tmp += 3;
    zTime[16] = '\0';
    dateTime.tm_min = wcstol(tmp, NULL, 10);

    tmp += 3;
    zTime[19] = '\0';
    dateTime.tm_sec = wcstol(tmp, NULL, 10);
	
}

/*
*	parse datetime as null-terminated string in following format -> YYYY-MM-DDTHH:MM:SS+HH:MM
*	returns the UTC time offset
*/
void getUTCOffset(char* zTime, int len, int* hrs, int* min) {

    if (len < 26) {
        *hrs = 0;
        *min = 0;
        return;
    };

    char* tmp = zTime + 19;

    char sign = *tmp;
    char* hrsStr = tmp + 1; hrsStr[2] = '\0';
    char* minStr = tmp + 4;

    *hrs = strtol(hrsStr, NULL, 10);
    *min = strtol(minStr, NULL, 10);

}

/*
*	returns the UTC time offset of the local time
*/
void getLocalUTCOffset(int* hrs, int* min) {

    time_t now = time(NULL);

    struct tm hereDT = *localtime(&now);
    int hereHrs = hereDT.tm_hour;
    int hereMin = hereDT.tm_min;

    struct tm utcTime = *gmtime(&now);
    int utcHrs = utcTime.tm_hour;
    int utcMin = utcTime.tm_min;

    *hrs = (hereHrs - utcHrs);
    *min = (hereMin - utcMin);

    return;

}

/*
*	parse out from time_t value year, month and day
*/
void timeToDate(time_t* tm, int* y, int* m, int* d) {

    struct tm dateTime;
    localtime_s(&dateTime, tm);

    *y = dateTime.tm_year + 1900;
    *m = dateTime.tm_mon + 1;
    *d = dateTime.tm_mday;

}

/*
*	change the recieved time_t buffer to new value, that has 
*	the reprsent same year, same month and same day, but it's 00:00:00 time
*/
void roundTimeToDay(time_t* tm) {

    struct tm dateTime;
    localtime_s(&dateTime, tm);

    dateTime.tm_hour = 0;
    dateTime.tm_min = 0;
    dateTime.tm_sec = 0;

    *tm = mktime(&dateTime);

    return;

}

/*
*	returns 1 if given struct tm points to today date
*/
int compareDateWithToday(struct tm* dateTime) {

    time_t nowTm = time(NULL);
	struct tm nowDt;
    localtime_s(&nowDt, nowTm);

    return (
        nowDt.tm_year == dateTime->tm_year &&
        nowDt.tm_mon == dateTime->tm_mon &&
        nowDt.tm_mday == dateTime->tm_mday
    );

}

/*
*	finds index of given char in given string
*/
int findArrayIdx(char* array, int arrLen, char ch, int offset) {

    for (int i = offset; i < arrLen; i++) {

        if (ch == array[i]) {
            return i;
        }

    }

    return -1;

}

/*
*	wide char function
*	finds index of given char in given string
*/
int findArrayIdxW(wchar_t* array, int arrLen, wchar_t ch, int offset) {

    for (int i = offset; i < arrLen; i++) {

        if (ch == array[i]) {
            return i;
        }

    }

    return -1;

}

/*
*	returns index of the most left non whitespaced char in given string
*/
int trimIdxLeft(char* str, int length) {

    for (int i = 0; i < length; i++) {
        if (str[i] > 0 && !isspace(str[i])) {
            return i;
        }
    }

}

/*
*	returns index of the most right non whitespaced char in given string
*/
int trimIdxRight(char* str, int length) {

    for (int i = length - 1; i > -1; i--) {
        if (str[i] > 0 && !isspace(str[i])) {
            return i;
        }
    }

}

/*
*	reads full file into buffer it allocate and this buffer is returned via pointer back
*	closes the given file pointer
*/
int readFullFile(wchar_t** buffer, FILE* file) {

    fseek(file, 0L, SEEK_END);

    int fsize = ftell(file);
    if (fsize <= 0) return -1;

    rewind(file);

    *buffer = (wchar_t*)malloc(fsize * sizeof(wchar_t));
    if (*buffer == NULL) return -1;

    wchar_t ch = fgetwc(file);
    for (int i = 0; (feof(file) == 0); i++) {
        (*buffer)[i] = ch;
        ch = fgetc(file);
    }

    fclose(file);

    return fsize;

}

/*
*	writes bytes/char buffer to the file
*	meanwhile creates all necessary directories
*/
int writeFile(char* buffer, int bufferLen, char* fileName) {

    makeDir(fileName);

    FILE* file = fopen(fileName, "w");
    if (file == NULL) return 1;

    fwrite(buffer, sizeof(char), bufferLen, file);

    fclose(file);

    return 0;

}

/*
*	recursively creates all neccesery dirs for given file name
*/
int makeDir(char* fileName) {
    
    int i = 0;
    char ch;
    while ((ch = fileName[i]) != '\0') {

        if (ch == '/' || ch == '\\') {
            
            char tmp = fileName[i];
            fileName[i] = '\0';

            _mkdir(fileName);

            fileName[i] = tmp;

        }

        i++;
    
    }

}
