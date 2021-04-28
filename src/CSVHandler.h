#pragma once

#include "CSVDelimiters.h"
#include "wchar.h"

struct CSV {

    wchar_t* data;
    int length;

    char dlm;

};

/* all strings has to be null terminated */
/* function will allocate return buffer */
struct CSVSearchHandler {

    /* name of the attribute to search by */
    wchar_t* searchBy;

    /* names of fileds to find under searchBy attribute */
    wchar_t** searchFor;
    int inputs;

    /* names of attributes to return from found records*/
    wchar_t** returnWhat;
    int results;

    wchar_t*** returnBuff;

};

int searchCSV(struct CSV* csv, struct CSVSearchHandler* srchHndlr);
