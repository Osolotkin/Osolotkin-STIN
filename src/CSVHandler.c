*
    ============================================================================================

        Supposed to be filled with various functions that has to do something along with
        CSV file format to help working with it, but for now there is just one and only
        lonely strange functions, which does its job and dont complain

    ============================================================================================
*/

#pragma once

#include <windows.h>
#include <stdlib.h>

#include "CSVHandler.h"

/* 
*   csv data will be modified
*   is succeed return buffer first dim is surely allocated
*   basically finds various attributes for various keys, look at the CSVSearchHandler definition
*   not really tested
*/
int searchCSV(struct CSV* csv, struct CSVSearchHandler* srchHndlr) {

    int err = 0;
    const wchar_t EOL = L'\n';

    wchar_t* data = csv->data;
    int csvLen = csv->length;
    wchar_t dlm = (wchar_t) csv->dlm;

    int firstLineLen = 0;
    int cols = 0;
    wchar_t** attributes;

    int* resultIdxs = NULL;
    int* wasFound = NULL;

    /* get number of cols */

    int i = 0;
    while (i < csvLen) {

        if (data[i] == dlm) cols++;
        if (data[i] == EOL) {
            cols++;
            break;
        }

        i++;

    };
    firstLineLen = i + 1;

    if (cols == 0) return 2;

    /* parse 'header' */

    attributes = (wchar_t**)malloc(sizeof(wchar_t*) * cols);
    if (attributes == NULL) {
        err = 1;
        goto cleanup;
    }

    attributes[0] = data;
    int j = 1;
    i = 0;
    do {

        if (data[i] == dlm || data[i] == EOL) {

            data[i] = (wchar_t)'\0';
            if (data[i + 1] != dlm && data[i + 1] != EOL) {

                attributes[j] = data + i + 1;
                if (j >= cols - 1) break;

                i++;
                j++;

            }

        }

        i++;


    } while (i < csvLen);

    /* distribute indexes according to the attributes */

    /* pairs -> first means if was examined, second index of the column */
    resultIdxs = (int*) calloc(2 * srchHndlr->results, sizeof(int));
    if (resultIdxs == NULL) {
        err = 1;
        goto cleanup;
    }

    int rCnt = 0;
    int searchIdx = -1;
    for (i = 0; i < cols; i++) {

        for (int r = 0; r < srchHndlr->results; r++) {

            if (resultIdxs[2 * r + 1] == 1) continue;

            if (wcscmp(attributes[i], srchHndlr->returnWhat[r]) == 0) {
                resultIdxs[2 * r] = i;
                resultIdxs[2 * r + 1] = 1;
                rCnt++;
            }

        }

        if (wcscmp(attributes[i], srchHndlr->searchBy) == 0) {
            searchIdx = i;
        }

        if (rCnt >= srchHndlr->results && searchIdx > -1) {
            break;
        }

    }

    if (searchIdx == -1) {
        return 3;
    }

    for (int r = 0; r < srchHndlr->results; r++) {
        resultIdxs[2 * r + 1] = 0;
    }

    wasFound = (int*) calloc(srchHndlr->inputs, sizeof(int));
    if (wasFound == NULL) {
        err = 1;
        goto cleanup;
    }

    /* alloc return buffer */
    wchar_t*** returnBuff = (wchar_t***) malloc(sizeof(wchar_t**) * srchHndlr->inputs);
    if (returnBuff == NULL) {

        free(returnBuff);
        
        err = 1;
        goto cleanup;
    
    }

    for (int i = 0; i < srchHndlr->inputs; i++) {
        returnBuff[i] = (wchar_t**) malloc(sizeof(wchar_t*) * srchHndlr->results);
        if (returnBuff[i] == NULL) {
            
            for (int j = 0; j < i; j++) {
                free(returnBuff[j]);
            }
            free(returnBuff);
            
            err = 1;
            goto cleanup;
        
        }
    }

    for (int i = 0; i < srchHndlr->inputs; i++) {
        for (int j = 0; j < srchHndlr->results; j++) {
            returnBuff[i][j] = NULL;
        }
    }

    int foundCount = 0;
    int lineStart = firstLineLen;
    int col = -1;
    i = lineStart;
    while (1) {

        if (col < 0 || data[i] == dlm) {
            col++;

            if (col != searchIdx) continue;

            /* found right column */

            /* searching for the end of the col */
            int len = 1;
            while (i + len < csvLen && data[i + len] != dlm && data[i + len] != EOL) {
                len++;
            }

            wchar_t tmp = data[i + len];
            data[i + len] = L'\0';

            /* browersing all inputs */
            int lastLineIdx = i + len;
            for (int j = 0; j < srchHndlr->inputs; j++) {

                if (wasFound[j] == 1) {
                    continue;
                }

                if (wcscmp(data + i, srchHndlr->searchFor[j]) == 0) {
                    /* found */
                    foundCount++;
                    data[i + len] = tmp;
                    wasFound[j] = 1;

                    /* searching for results */
                    int resCount = 0;
                    int resCol = -1;

                    /*  
                    *   as every other col expect delimiter infront of it,
                    *   we have to adjust the first col to have compatible start index
                    */
                    int n = lineStart - 1;

                    while (n < csvLen) {

                        if (resCol < 0 || data[n] == dlm) {
                            resCol++;

                            for (int k = 0; k < srchHndlr->results; k++) {

                                if (resultIdxs[2 * k + 1] == 1) {
                                    continue;
                                }

                                if (resCol == resultIdxs[2 * k]) {
                                    /* found matching result col */
                                    resultIdxs[2 * k + 1] = 1;
                                    resCount++;

                                    int resLen = 1;
                                    while (data[n + resLen] != dlm && data[n + resLen] != EOL) {
                                        resLen++;
                                    }
                                    resLen--;

                                    returnBuff[j][k] = (wchar_t*) malloc(sizeof(wchar_t) * (resLen + 1));
                                    if (returnBuff[j][k] == NULL) {
                                        
                                        for (int i = 0; i < srchHndlr->results; i++) {
                                            for (int j = 0; j < srchHndlr->results; j++) {
                                                free(returnBuff[i][j]);
                                            }
                                            free(returnBuff[i]);
                                        }
                                        free(returnBuff);

                                        err = 1;
                                        goto cleanup;
                                    }

                                    memcpy(returnBuff[j][k], data + n + 1, sizeof(wchar_t) * (resLen));
                                    returnBuff[j][k][resLen] = L'\0';

                                    if (n + 1 + resLen > lastLineIdx) lastLineIdx = n + resLen + 1;
                                    break;
                                }

                            }

                            if (resCount >= srchHndlr->results) {
                                break;
                            }

                        }

                        n++;

                    }

                    break;

                }

            }

            if (foundCount >= srchHndlr->inputs) {
                break;
            }

            /* skip till new line */
            int j = lastLineIdx;
            while (j < csvLen && data[j] != EOL) {
                j++;
            }
            i = j + 1;
            col = -1;
            lineStart = i;

            if (j >= csvLen) break;

            for (int r = 0; r < srchHndlr->results; r++) {
                resultIdxs[2 * r + 1] = 0;
            }

            continue;
        }

        i++;

    }

    srchHndlr->returnBuff = returnBuff;

cleanup:

    free(attributes);
    free(resultIdxs);
    free(wasFound);

    return err;

}
