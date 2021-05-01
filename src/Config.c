#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Config.h"

#define CFG_FILE "config.cfg"

int CZECHIA_INDEX = 36;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 640;

const Color GRAPH_COLORS[] = {
       HEX_6B9CE2,
       HEX_29A88D,
       HEX_B64A63,
       HEX_F2BA44,
       HEX_494B8A
};

#define PARAMS_COUNT 3

typedef const enum Params {
    E_CZECHIA_INDEX,
    E_SCREEN_WIDTH,
    E_SCREEN_HEIGHT
} Params;

void loadParam(Params param, char* value);

/* returns 1 if cfg was successfuly loaded from file, otherwise 0 */
int loadConfig() {

    FILE* file = fopen(CFG_FILE, "r");
    if (file == NULL) return 0;

    const char* paramNames[] = {
        "czechia_index",
        "screen_width",
        "screen_height"
    };

    int paramsBool[] = {
        0,
        0,
        0
    };

    const EOL = '\n';

    char buffer[20];
    int bytesLeft = 0;
    int bytesRead;
    while ((bytesRead = fread(buffer + bytesLeft, sizeof(char), sizeof(buffer) - bytesLeft, file)) > 0) {

        char* pBuffer = buffer;
        int bytesProcessed = 0;

        while (1) {

            int eolIdx = -1;
            int i;
            for (i = 0; i < bytesRead + bytesLeft - bytesProcessed; i++) {
                if (pBuffer[i] == EOL) {
                    eolIdx = i;
                    break;
                }
            }

            if (eolIdx < 0) {
                if (bytesRead + bytesLeft == sizeof(buffer)) {
                    memmove(buffer, pBuffer, i);
                    bytesLeft = i;
                    break;
                }
                eolIdx = i;
            }

            pBuffer[eolIdx] = '\0';
            char* line = pBuffer;

            char* paramName;
            char* paramValue;

            int spcIdx = -1;
            for (i = 0; i < eolIdx; i++) {
                if (line[i] == ' ') {
                    spcIdx = i;
                    break;
                }
            }

            if (spcIdx < 0) {
                memmove(buffer, pBuffer, bytesRead + bytesLeft - eolIdx - 1);
                bytesLeft = eolIdx + 1;
                break;
            }

            line[spcIdx] = '\0';
            paramName = line;
            paramValue = line + spcIdx + 1;

            for (int i = 0; i < PARAMS_COUNT; i++) {

                if (paramsBool[i]) {
                    continue;
                }

                if (strcmp(paramNames[i], paramName) == 0) {
                    paramsBool[i] = 1;
                    loadParam(i, paramValue);
                    break;
                }

            }

            pBuffer += eolIdx + 1;
            bytesProcessed += eolIdx + 1;

            if (bytesProcessed >= bytesRead + bytesLeft) {
                bytesLeft = 0;
                break;
            }
        
        }

    }

    fclose(file);
    return 1;

}

void loadParam(Params param, char* value) {

    switch (param) {

        case E_CZECHIA_INDEX: {

            CZECHIA_INDEX = strtol(value, NULL, 10);
            break;

        }

        case E_SCREEN_WIDTH: {

            SCREEN_WIDTH = strtol(value, NULL, 10);
            break;

        }

        case E_SCREEN_HEIGHT: {

            SCREEN_HEIGHT = strtol(value, NULL, 10);
            break;

        }

    }

}
