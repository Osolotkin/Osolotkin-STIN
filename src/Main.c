/* 
    ============================================================================================
        
        Main file where all 'general' parts of the program are assembled together.
        Supposed to be consisted only of functions and variables that lead to solve
        particular problem, main problem, that this program suppose to solve.
    
    ============================================================================================
*/

#pragma once

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "zlib.h"

#include "Render.h"
#include "DaySelector.h"
#include "Graph.h"
#include "RequestHandler.h"
#include "Utils.h"
#include "CountrySelector.h"
#include "CSVHandler.h"
#include "CallbackButton.h"
#include "Config.h"

#define ICON_PATH L"icon.ico"
#define ICON_SIZE 256

/* max value that can present 4 bytes long signed int */
#define INT_MAX 2147483647

/* 
*   number of days to be added to data sotred array each time the limit 
*   will be reached
*/
#define DAYS_TO_ADD_AT_EXPANSION 365

/*
*   number of countries that has to participate in vaccination comparison,
*   except Czechia
*/
#define COUNTRIES_TO_SELECT 4

/* in ms */
#define REQUEST_RESEND_DELAY (5 * 60 * 1000)

/* 0 to 23 */
#define REQUEST_UPDATE_HOUR 14

#define SECONDS_IN_A_DAY (24 * 60 * 60)
#define SECONDS_IN_AN_HOUR (60 * 60)

/* margin in pixels from windows borders and data containers borders */
#define BORDER_MARGIN 8

/*
*   comparasion data file format:
* 
*   file starts with date of the first day in following format:
*   int64 - seconds from 1970 i guess
*
*   afterwards comming comparasion data in folowing format, each one represented as int32:
*   czech increase, world increase, czech total, world total, czech time diff, world time diff
*/
#define COMPARASION_DATA_SAVE_FILE "saves/comparasion_data.save"

/*
*   vaccination data will be stored as csv files with same format and data as they have upon
*   request
*   
*   name of the file will represent day the file supposed to cover in following format: yyyy-mm-dd
*/
#define VACCINATION_DATA_FOLDER_PATH "saves/vaccination/"

/*
*   file where list of countries is stored, starts with number of countries it consists of 
*   and after come countries names each on separate line
*/
#define COUNTRIES_FILE "Countries.txt"

#define CURSOR_ANIM_TIMER 14
#define FAILED_REQUEST_TIMER 88

HCURSOR defaultCursor;
HCURSOR actionCursor;
HCURSOR inputCursor;

typedef struct CompDataBuffer {

    int increase;
    int total;
    int timeDiff;
    int available;

} CompDataBuffer;

typedef struct tm LocalDateTime;

const LPCWSTR CLASS_NAME = L"Class_Name";
const LPCWSTR WINDOW_TITLE = L"Program";

/* thread to collect data and process them while not disturbing gui thread */
HANDLE dataCollectingThread;
DWORD WINAPI dataCollecting(void* arg);

DaySelector* daySelector;
Graph SourceComparisonGraphs[3];
SearchHandler searchHandler = {0};

/* has to be day rounded */
time_t firstDayTime;

/* 0 to 364 */
int lastDay = 0;

/* has to be poitive multiple of DAYS_TO_ADD_AT_EXPANSION */
int daysAllocated = 0;

/* 
*   stored data format:
*   czInc wdInc czTotal wdTotal czTime wdTime
*   -1 value means, that value wasn't in the record or was unable to load
*   if first value of the set is -2, there are no data for entire set
*   
*   as it will store all data after the first start of the app, it has to
*   be allocated on heap, to realloc it latter, so initialization is in main
*/
int* comparisonData;

CompDataBuffer sCompDataBufferCz = { 0 };
CompDataBuffer sCompDataBufferWd = { 0 };

Color countriesColors[COUNTRIES_TO_SELECT + 1] = {
        HEX_6B9CE2,
        HEX_29A88D,
        HEX_B64A63,
        HEX_F2BA44,
        HEX_494B8A
};

/*
*   0 -> allways Czechia
*   1 - COUNTRIES_TO_SELECT -> selected ones
*
*   data represent ratio, but, to have higher precission, stored values are 
*   100 times larger, so range of values supose to be <0, 1000>
*   -1 value means, that value wasn't in the record or was unable to load
*   if first value of is -2, there are no data for entire set
*/ 
int vaccinationData[COUNTRIES_TO_SELECT + 1] = { -2 };

CountrySelector* countrySelector;

CallbackButton* requestButton;

const int reqButtDfStrLen = 12;
const wchar_t reqButtDfStr[] = L"request data";

enum ErrorMessage {

    EM_NO_INTERNET_CONNECTION,
    EM_REQUEST_FAILED,
    EM_NO_DATA_FOR_THE_MOMENT

};

int errorMessageLen;
wchar_t* errorMessage;

enum ErrorMessage lastErrorMessage;

#define DATA_TEXT_VIEW 1
#define DATA_HISTO_VIEW 0

int comparisonDataViewType;
int vaccinationDataViewType;

HANDLE ghMutex;

int windowWidth;
int windowHeight;

enum ExitErrors {

    EE_MALLOC_ERROR,
    EE_WRONG_FILE_FORMAT,
    EE_CRITICAL_ERROR,
    EE_WIN_32_ERROR

};

const wchar_t* EXIT_ERRORS[] = {

    L"MALLOC_ERROR",
    L"WRONG_FILE_FORMAT",
    L"CRITICAL_ERROR",
    L"WIN_32_ERROR"

};

int loadCzechData();
int loadWorldData();

int tryLoadComparisonData();

void loadVaccineData();

int initSaveComparasionDataFile();
int saveComparasionData(int* pData);
int saveComparasionDataFull(int* pData, int days);

void drawDay(int day);
void drawComparisonData(int day, int viewType);
void drawVaccineComparasion(int day, int viewType);

void drawNoDataError(int x, int y, int height, int width);
void drawErrorMessage(int x, int y, int height, int width);

void addNewDay();
void fillNewDay(CompDataBuffer* dataBufferCz, CompDataBuffer* dataBufferWd);

int requestButtonCallback();
VOID CALLBACK drawRequestError(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD millis);

void scaleMouseCoords(POINT* coords);

void switchMainError(enum errorMessage error);

int exitWithError(const wchar_t* msg);
int showError(const wchar_t* msg);



/*
*   transform mouse coords, that are assumed as coords of client area, to the actual
*   coords of active part of the window
*/
void scaleMouseCoords(POINT* coords) {

    if (SCREEN_HEIGHT * windowWidth > SCREEN_WIDTH * windowHeight) {
        
        int wd = (SCREEN_WIDTH * windowHeight) / SCREEN_HEIGHT;
        int x = (windowWidth - wd) / 2;
        coords->x = ((SCREEN_WIDTH * (coords->x - x)) / wd);
        coords->y = (coords->y * SCREEN_HEIGHT) / windowHeight;

    } else {
    
        int hg = (SCREEN_HEIGHT * windowWidth) / SCREEN_WIDTH;
        int y = (windowHeight - hg) / 2;
        coords->y = y + ((SCREEN_HEIGHT * (coords->y - y)) / hg);
        coords->x = (coords->x * SCREEN_WIDTH) / windowWidth;

    }

}

/* window Procedure */
LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
) {

    switch (msg) {

        case WM_LBUTTONDBLCLK: {

            DWORD posMouse = GetMessagePos();
            POINT mouseCoords = {
                (posMouse & 0x000FFF),
                (posMouse >> 16)
            };

            if (ScreenToClient(hwnd, &mouseCoords) <= 0) break;
            scaleMouseCoords(&mouseCoords);

            if (mouseCoords.y > ((SCREEN_HEIGHT - daySelector->height) / 2)) {
                
                if (vaccinationData[0] > -2) {
                    int hmsg = countrySelector->hover(countrySelector, &mouseCoords);
                    if (hmsg > 0) return 0;
                }
                vaccinationDataViewType = !vaccinationDataViewType;
            
            } else {

                comparisonDataViewType = !comparisonDataViewType;
            
            }

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

            break;

        }

        case WM_LBUTTONDOWN: {

            DWORD posMouse = GetMessagePos();
            POINT mouseCoords = {
                (posMouse & 0x000FFF),
                (posMouse >> 16)
            };

            if (ScreenToClient(hwnd, &mouseCoords) <= 0) break;
            scaleMouseCoords(&mouseCoords);

            if (daySelector->select(daySelector, &mouseCoords)) {
                loadVaccineData();
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                break;
            }

            if (vaccinationData[0] > -2) {

                int ans = countrySelector->select(countrySelector, &mouseCoords);
                if (ans == 1) {

                    loadVaccineData();
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;

                } else if (ans == -1) {

                    KillTimer(NULL, CURSOR_ANIM_TIMER);
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;
                
                } else if (ans == -2) {

                    SetTimer(
                        hwnd,
                        CURSOR_ANIM_TIMER,
                        500,
                        NULL
                    );

                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;
                
                } else if (ans == 2) {
                
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;

                }

            }

            int bMsg = requestButton->select(requestButton, &mouseCoords);
            if (bMsg >= 1) {
                if (bMsg != 2) {
                    SetTimer(
                        hwnd,
                        FAILED_REQUEST_TIMER,
                        2000,
                        drawRequestError
                    );
                }

                switchMainError((enum ErrorMessage) EM_REQUEST_FAILED);
                
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                break;
            }


            break;
        
        }

        case WM_MOUSEMOVE: {

            DWORD posMouse = GetMessagePos();
            POINT mouseCoords = {
                (posMouse & 0x000FFF),
                (posMouse >> 16)
            };

            if (ScreenToClient(hwnd, &mouseCoords) <= 0) break;
            scaleMouseCoords(&mouseCoords);

            int dmsg = daySelector->hover(daySelector, &mouseCoords);
            if (dmsg == 1 || dmsg == 2) {
                
                SetCursor(actionCursor);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                
                break;
            
            }

            if (vaccinationData[0] > -2) {
                int hmsg = countrySelector->hover(countrySelector, &mouseCoords);
                if (hmsg == 10 || hmsg == 1) {
                    
                    SetCursor(actionCursor);
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    
                    break;
                
                } else if (hmsg == 2) {

                    SetCursor(inputCursor);
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    
                    break;
                
                } else if (hmsg == 11) {
                    
                    SetCursor(actionCursor);
                    
                    break;
                    
                }
            }

            int bmsg = requestButton->hover(requestButton, &mouseCoords);
            if (bmsg == 1) {
                SetCursor(actionCursor);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                break;
            } else if (bmsg == 2) {
                SetCursor(defaultCursor);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                break;
            }

            SetCursor(defaultCursor);

            break;
        
        }

        case WM_SETCURSOR: {

            break;
        
        }

        case WM_TIMER: {

            switch (wParam) {

                case CURSOR_ANIM_TIMER: {
                    
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;
                
                }

                case FAILED_REQUEST_TIMER: {
                
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    break;

                }

                default: {

                    break;
                
                }
            
            }
            
        }

        case WM_PAINT: {

            if (IsIconic(hwnd)) break;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (hdc == NULL) break;
            if (pixels == NULL) break;
        
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            
            int realDayIdx = getRealSelectedDay(daySelector) - 1;

            renderClear();

            drawDaySelector(daySelector);
            drawDay(realDayIdx);

            if (vaccinationData[0] > -2) {
                drawCountriesSelector(countrySelector);
            }

            render(hdc, windowWidth, windowHeight);

            EndPaint(hwnd, &ps);
        
            break;

        }

        case WM_SIZE: {

            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            windowWidth = width;
            windowHeight = height;

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

            break;

        }

        case WM_ERASEBKGND: {

            break;

        }

        case WM_KEYDOWN: {
        
            if (GetKeyState(VK_BACK) & 0x8000) {
                SearchHandler* srchHndlr = countrySelector->searchHandler;
                srchHndlr->input->removeChar(srchHndlr);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            
            break;

        }

        case WM_CHAR: {

            if (GetKeyState(VK_BACK) & 0x8000) return;

            wchar_t ch = (wchar_t) wParam;
            SearchHandler* srchHndlr = countrySelector->searchHandler;
            srchHndlr->input->addChar(srchHndlr, ch);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

            break;

        }

        case WM_CLOSE: {

            CloseHandle(dataCollectingThread);
            DestroyWindow(hwnd);
            break;

        }

        case WM_DESTROY: {

            /* have to free memory here, just in case */

            PostQuitMessage(0);
            break;

        }

        default: {

            return DefWindowProc(hwnd, msg, wParam, lParam);

        }

    }

    return 0;

}

int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR cmdLine,
    int cmdShow
) {

    /* first of try to load config data from file */
    loadConfig();

    /* registering the Window Class */
    WNDCLASSEX winClass;

    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_DBLCLKS;
    winClass.lpfnWndProc = WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = instance;
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = CLASS_NAME;
    winClass.hIconSm = LoadImage(NULL, ICON_PATH, IMAGE_ICON, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
    winClass.hIcon = LoadImage(NULL, ICON_PATH, IMAGE_ICON, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);

    int xxxx = GetLastError();

    if (!RegisterClassEx(&winClass)) {

        exitWithError(L"Window Creation Failed!", EE_WIN_32_ERROR);
        return 0;

    }

    /* creating the Window */
    HWND hwnd;

    DWORD styleFlags = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX;

    RECT winRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    if (AdjustWindowRectEx(&winRect, styleFlags, 0, WS_EX_CLIENTEDGE) == 0) {

        exitWithError(L"Window Creation Failed!", EE_WIN_32_ERROR);
        return 2;

    }
    int winHeight = winRect.bottom - winRect.top;
    int winWidth = winRect.right - winRect.left;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        CLASS_NAME,
        WINDOW_TITLE,
        styleFlags,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        winWidth,
        winHeight,
        NULL,
        NULL,
        instance,
        NULL
    );

    if (hwnd == NULL) {

        exitWithError(L"Window Creation Failed!", EE_WIN_32_ERROR);
        return 0;

    }

    windowWidth = SCREEN_WIDTH;
    windowHeight = SCREEN_HEIGHT;

    defaultCursor = LoadCursor(NULL, IDC_ARROW);
    actionCursor = LoadCursor(NULL, IDC_HAND);
    inputCursor = LoadCursor(NULL, IDC_IBEAM);

    ShowWindow(hwnd, cmdShow);
    UpdateWindow(hwnd);



    /* basic variables */

    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = SCREEN_WIDTH; // rect.right - rect.left;
    int height = SCREEN_HEIGHT; // rect.bottom - rect.top;

    /* preparing all the stuff around */

    /* try to load data from previous session */
    int successLoad = 0;
    int missedDaysCount = loadPreviousSession();
    if (missedDaysCount < 0) {
        /* no valid data from previous session, loading default values */

        daysAllocated = DAYS_TO_ADD_AT_EXPANSION;
        comparisonData = (int*)malloc(sizeof(int) * daysAllocated);
        if (comparisonData == NULL) {
            exitWithError(L"Could not allocate memory while preparing data!", EE_MALLOC_ERROR);
        }

        lastDay = 0;

        comparisonData[0] = -2;
        comparisonData[1] = -1;

        comparisonData[2] = -1;
        comparisonData[3] = -1;

        comparisonData[4] = -1;
        comparisonData[5] = -1;

        time_t timeNow = time(NULL);
        roundTimeToDay(&timeNow);
        firstDayTime = timeNow;

        if (initSaveComparasionDataFile()) {
            showError(L"[fopen error]\nCould not initialize save file, suggesting restart the app and make sure all necessary files/paths have write permission!");
            goto loadingEnd;
        }

    } else if (missedDaysCount > 1) {
        /* there are some days without data, have to resave internal file with all current data to make it trully valid */
        
        saveComparasionDataFull(comparisonData + (lastDay - missedDaysCount) * 6, missedDaysCount - 1);
    
    } else {

        successLoad = (missedDaysCount) ? 0 : 1;
    
    }

loadingEnd:;


    /* day selector */

    int dsWidth = width;
    int dsHeight = 60;

    int dsX = 0;
    int dsY = height - dsHeight;

    int dsTabCount = 7;

    daySelector = newDaySelector(dsX, dsY, dsWidth, dsHeight, dsTabCount, firstDayTime);
    if (daySelector == NULL) {
        exitWithError(L"Could not allocate enough memory to build daySelector!", EE_CRITICAL_ERROR);
    }

    daySelector->setByDay(daySelector, lastDay + 1);
	

	
    /* country selector */

    int csWidth = (width - 2 * BORDER_MARGIN) / 3;
    int csHeight = (height - dsHeight) / 2 - BORDER_MARGIN * 1.5;

    int csX = 2 * csWidth + BORDER_MARGIN;
    int csY = csHeight + 2 * BORDER_MARGIN;

    int results = 4;

    countrySelector = newCountrySelector(
        GRAPH_COLORS + 1,
        COUNTRIES_TO_SELECT,
        COUNTRIES_FILE,
        csX,
        csY,
        csWidth,
        csHeight
    );

    if (countrySelector == NULL) {
        exitWithError(L"Could not allocate enough memory to build countrySelector!\nOR\nFile format of 'Countries.txt' is invalid!", EE_MALLOC_ERROR);
    }

    int btWidth = width / 4;
    int btHeight = csHeight / 6;
    
    int btX = (width - btWidth) / 2;
    int btY = (height - dsHeight - btHeight) / 2 + btHeight + BORDER_MARGIN / 2;

    wchar_t btText[] = L"request data";
    int btTextLen = 12;//wcslen(btText);

    requestButton = newCallbackButton(
        btX,
        btY,
        btWidth,
        btHeight,
        btText,
        btTextLen,
        &requestButtonCallback
    );
    
    if (requestButton == NULL) {
        exitWithError(L"Could not allocate enough memory to build requestButton!", EE_MALLOC_ERROR);
    }

    switchMainError((enum errorMessage) EM_NO_DATA_FOR_THE_MOMENT);

    comparisonDataViewType = DATA_HISTO_VIEW;
    vaccinationDataViewType = DATA_HISTO_VIEW;

    /* render */
    renderAlloc(width, height);

    
    
    ghMutex = CreateMutex(
        NULL,
        FALSE,
        NULL
    );

    if (ghMutex == NULL) {
        exitWithError(L"Mutex Creation Failed!", EE_CRITICAL_ERROR);
        return 0;
    }

    /* starting separate thread for data collecting and processing */
    int thArgs[] = { successLoad, hwnd };
    dataCollectingThread = CreateThread(NULL, 0, dataCollecting, &thArgs, 0, NULL);



    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

    /* message loop */
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) > 0) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    CloseHandle(ghMutex);

    return msg.wParam;

}

DWORD WINAPI dataCollecting(void* arg) {
    
    int successLoad = ((int*)arg)[0];
    HWND hwnd = ((HWND*)arg)[1];

    time_t tm = time(NULL);
    LocalDateTime today = *localtime(&tm);
    today.tm_year += 1900;
    today.tm_mon += 1;

    if (successLoad) goto afterSuccessLoad;

    long long requestHourInSec = REQUEST_UPDATE_HOUR * SECONDS_IN_AN_HOUR;
    long long lastDayInSec = firstDayTime + (long long) (lastDay) * SECONDS_IN_A_DAY;
    if (lastDayInSec + requestHourInSec - time(NULL) > 0)
        goto afterNewDay;
	
	while (1) {

        DWORD dwWaitResult = WaitForSingleObject(
            ghMutex,
            INFINITE
        );

        if (dwWaitResult != WAIT_OBJECT_0) {
            Sleep(100); 
            continue;
        }

        requestButton->visible = 1;
        
        if (comparisonData[lastDay * 6] > -2) {
            /* already loaded manualy */
            goto afterSuccessLoad;
        }

        if (!tryLoadComparisonData()) {

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            
            if (!isNewDay(&today)) {

                ReleaseMutex(ghMutex);
                /* sleep some time and try again */
                Sleep(REQUEST_RESEND_DELAY);
                continue;
            
            }

            saveComparasionData(comparisonData + lastDay * 6);
            ReleaseMutex(ghMutex);

        } else {

            fillNewDay(&sCompDataBufferCz, &sCompDataBufferWd);
            saveComparasionData(comparisonData + lastDay * 6);

        afterSuccessLoad:;

            loadVaccineData();
            ReleaseMutex(ghMutex);

            requestButton->visible = 0;
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

            /* sleep till new day */
            int x = (lastDay + 1) * SECONDS_IN_A_DAY;
            time_t nextDay = firstDayTime + (((long long) lastDay) + 1) * SECONDS_IN_A_DAY;
            time_t nowTime = time(NULL);
            time_t timeDiff = nextDay - nowTime;

            if (timeDiff < 0) {
                /* it seems new day already started while processing some calculations */
                break;
            }

            Sleep(timeDiff * 1000);
        
        }

        time_t tm = time(NULL);
        today = *localtime(&tm);
        today.tm_year += 1900;
        today.tm_mon += 1;

        addNewDay();

    afterNewDay:;

        requestButton->visible = 1;
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

		/* sleep till update time */
        long long requestHourInSec = REQUEST_UPDATE_HOUR * SECONDS_IN_AN_HOUR;
        long long lastDayInSec = firstDayTime + (long long)(lastDay) * SECONDS_IN_A_DAY;

        time_t updateTime = lastDayInSec + requestHourInSec;
        time_t nowTime = time(NULL);
        time_t timeDiff = updateTime - nowTime;

        if (timeDiff < 0) {
            /* it's already time to update */
            continue;
        }

        Sleep(timeDiff * 1000);
		
	}
	
	return 0;
	
}

/*
*   just creates file and sets its first 8 bytes as first day time
*/
int initSaveComparasionDataFile() {

    char filename[] = COMPARASION_DATA_SAVE_FILE;
    return writeFile(&firstDayTime, 8, filename);

}

/*
*   appends new data to the end of the file
*   input pData has to point to the at least 6 * 4 bytes of allocated memory
*/
int saveComparasionData(int* pData) {
    
    FILE* cmpFile;

    cmpFile = fopen(COMPARASION_DATA_SAVE_FILE, "ab+");
    if (cmpFile == NULL) {
        return 1;
    }

    fwrite(pData, 4, 6, cmpFile);

    fclose(cmpFile);

    return 0;

}

int saveComparasionDataFull(int* pData, int days) {

    FILE* cmpFile;

    cmpFile = fopen(COMPARASION_DATA_SAVE_FILE, "wb");
    if (cmpFile == NULL) {
        return 1;
    }

    fwrite(&firstDayTime, 8, 1, cmpFile);

    fwrite(pData, 4, days * 6, cmpFile);

    fclose(cmpFile);

    return 0;

}

/* 
*   loads days from previous session, if there are no data, or they are corupted
*   treats as its first start, if interval beetween last saved and currendt days
*   is bigger than one day, imort blank days beetween
*/
int loadPreviousSession() {

    /* reading comparasion data from saved file if exists */

    firstDayTime = 0;

    FILE* cmpFile;

    cmpFile = fopen(COMPARASION_DATA_SAVE_FILE, "rb");
    if (cmpFile == NULL) {
        return -1;
    }

    int buffer[6];
    
    if (fread(buffer, 8, 1, cmpFile) < 1) {
        fclose(cmpFile);
        return -1;
    }

    firstDayTime = (time_t) buffer[0];
    if (firstDayTime < 0) {
        return -1;
    }

    time_t timeNow = time(NULL);
    time_t timeDiff = timeNow - firstDayTime;

    if (timeDiff < 0) {
        return -1;
    }

    int days = (timeDiff / SECONDS_IN_A_DAY) + 1;
    int cmpDataDays = DAYS_TO_ADD_AT_EXPANSION * (days / DAYS_TO_ADD_AT_EXPANSION + 1);

    lastDay = days - 1;

    comparisonData = (int*) malloc(6 * cmpDataDays * sizeof(int));
    if (comparisonData == NULL) {
        return -1;
    }

    daysAllocated = cmpDataDays;

    const int step = 6;
    int i = 0;
    while(fread(buffer, 4, 6, cmpFile) == 6 && i < days) {
        

        int offset = i * step;
        
        comparisonData[offset] = buffer[0];
        comparisonData[offset + 1] = buffer[1];
        comparisonData[offset + 2] = buffer[2];
        
        comparisonData[offset + 3] = buffer[3];
        comparisonData[offset + 4] = buffer[4];
        comparisonData[offset + 5] = buffer[5];

        i++;
    
    }

    fclose(cmpFile);

    if (i >= days) {
        return 0;
    }

    int missedDaysCount = days - i;

    for (i; i < days; i++) {

        int offset = i * step;
    
        comparisonData[offset] = -2;
        comparisonData[offset + 1] = -1;
        comparisonData[offset + 2] = -1;

        comparisonData[offset + 3] = -1;
        comparisonData[offset + 4] = -1;
        comparisonData[offset + 5] = -1;
    
    }

    return missedDaysCount;

}

/* returns 0 if data was sucesfully loaded */
int loadCzechData(CompDataBuffer* dataBuffer) {

    wchar_t* rootUrl = L"onemocneni-aktualne.mzcr.cz";
    wchar_t* pathUrl = L"/api/v2/covid-19/zakladni-prehled.json";
    char* response = NULL;

    int responseLen = httpsGet(rootUrl, pathUrl, &response);
    if (responseLen <= 0) {
        return (responseLen < 0) ? responseLen : 1;
    }

    const char date[] = "modified";
    const int dateLen = 8;

    const char dailyInc[] = "potvrzene_pripady_vcerejsi_den";
    const int dailyIncLen = 30;

    const char sickCount[] = "potvrzene_pripady_celkem";
    const int sickCountLen = 24;

    const char delimiter[] = "\":";
    const int delLen = 2;

    /* b as bool */
    int bDate = 1;
    int bDailyIncrease = 1;
    int bSickCount = 1;

    int increase = 0;
    int total = 0;

    int offset = 0;

    dataBuffer->available = 0;

    int idx;
    while ((idx = bitwiseFind(response, responseLen, delimiter, offset) - 1) >= 0) {

        if (bDate && backwardCompare(response, responseLen, date, dateLen, idx)) {

            int lastIdx = findArrayIdx(response, responseLen, ',', idx + 3);
            if (lastIdx == -1) {
                return 2;
            }

            char* dt = response + idx + 5;
            int dtLen = lastIdx - idx - 6 + 1;
            dt[dtLen - 1] = '\0';

            idx += dtLen;

            int zHrs = 0;
            int zMin = 0;
            getUTCOffset(dt, dtLen, &zHrs, &zMin);

            int lHrs = 0;
            int lMin = 0;
            getLocalUTCOffset(&lHrs, &lMin);

            struct tm dateTime;
            parseDateTime(dt, dtLen, &dateTime);

            /* time from the noon */
            int hrsDiff = dateTime.tm_hour - zHrs + lHrs;
            int minDiff = dateTime.tm_min - zMin + lHrs;

            if (isNewDay(&dateTime)) {
                return 3;
            }
            
            if (hrsDiff < 0 || (hrsDiff == 0 && minDiff < 0)) {
                /* not fresh data */
                return 3;
            }

            dataBuffer->timeDiff = 60 * hrsDiff + minDiff;

            bDate = 0;

        }

        if (bDailyIncrease && backwardCompare(response, responseLen, dailyInc, dailyIncLen, idx)) {

            int lastIdx = findArrayIdx(response, responseLen, ',', idx + 3);
            if (lastIdx == -1) {
                return 2;
            }

            char* inc = response + idx + 3;
            int incLen = lastIdx - idx - 2;

            int lfIdx = trimIdxLeft(inc, incLen);
            int rtIdx = trimIdxRight(inc, incLen);

            if (lfIdx > rtIdx) {
                return 2;
            }

            inc += lfIdx;
            incLen = rtIdx - lfIdx;

            inc[incLen] = '\0';

            increase = strtol(inc, NULL, 10);
            if (increase < 0) increase = 0;

            dataBuffer->increase = increase;

            bDailyIncrease = 0;

        }

        if (bSickCount && backwardCompare(response, responseLen, sickCount, sickCountLen, idx)) {

            int lastIdx = findArrayIdx(response, responseLen, ',', idx + 3);
            if (lastIdx == -1) {
                return 2;
            }

            char* ttl = response + idx + 3;
            int ttlLen = lastIdx - idx - 2;

            int lfIdx = trimIdxLeft(ttl, ttlLen);
            int rtIdx = trimIdxRight(ttl, ttlLen);

            if (lfIdx > rtIdx) {
                return 2;
            }

            ttl += lfIdx;
            ttlLen = rtIdx - lfIdx;

            ttl[ttlLen] = '\0';

            total = strtol(ttl, NULL, 10);
            if (total < 0) total = 0;

            dataBuffer->total = total;

            bSickCount = 0;

        }

        offset = idx + 3;

    }

    dataBuffer->available = 1;

    return 0;

}

/* returns 0 if data was sucesfully loaded */
int loadWorldData(CompDataBuffer* dataBuffer) {

    const char delimiter[] = "\":";

    char* response = NULL;
    int responseLen = 0;

    /* separate request for date time */
    wchar_t* rootUrlTime = L"covid19.who.int";
    wchar_t* pathUrlTime = L"/page-data/sq/d/361700019.json";

    if ((responseLen = httpsGet(rootUrlTime, pathUrlTime, &response)) <= 0) {
        return (responseLen < 0) ? responseLen : 1;
    }

    char date[] = "\"date";
    int dateLen = 5;

    int idx;
    int offset = 0;
    while ((idx = bitwiseFind(response, responseLen, delimiter, offset) - 1) >= 0) {

        if (backwardCompare(response, responseLen, date, dateLen, idx)) {

            int lastIdx = findArrayIdx(response, responseLen, ',', idx + 3);
            if (lastIdx == -1) {
                return 2;
            }

            char* dt = response + idx + 4;
            int dtLen = lastIdx - idx - 10;
            dt[dtLen - 1] = '\0';

            idx += dtLen;

            /* assuming that time is GMT */
            int zHrs = 0;
            int zMin = 0;

            int lHrs = 0;
            int lMin = 0;
            getLocalUTCOffset(&lHrs, &lMin);

            struct tm dateTime;
            parseDateTime(dt, dtLen, &dateTime);

            int hrsDiff = dateTime.tm_hour - zHrs + lHrs;
            int minDiff = dateTime.tm_min - zMin + lHrs;

            if (isNewDay(&dateTime)) {
                return 3;
            }

            if (hrsDiff < 0 || (hrsDiff == 0 && minDiff < 0)) {
                /* not fresh data */
                return 3;
            }

            dataBuffer->timeDiff = 60 * hrsDiff + minDiff;

            break;

        }

        offset = idx + 3;

    }
    free(response);

    /* 'normal' data request */
    wchar_t* rootUrl = L"covid19.who.int";
    wchar_t* pathUrl = L"/WHO-COVID-19-global-table-data.csv";
    response = NULL;

    responseLen = 0;
    if ((responseLen = httpsGet(rootUrl, pathUrl, &response)) <= 0) {
        return 1;
    }

    int pResponseLen = 0;
    char* pResponse = response;

    int lIdx = trimIdxLeft(pResponse, responseLen);
    
    pResponseLen = responseLen - lIdx + 1;
    pResponse += lIdx;

    wchar_t* wResponse = (wchar_t*) malloc(sizeof(wchar_t) * pResponseLen);
    if (wResponse == NULL) {
        return 1;
    }

    mbstowcs(
        wResponse,
        pResponse,
        pResponseLen
    );
    free(response);

    struct CSV csv = {
        wResponse,
        responseLen,
        COMMA
    };

    wchar_t* searchFor[] = { L"Czechia" };
    wchar_t* returnWhat[] = { {L"Cases - cumulative total"}, {L"Cases - newly reported in last 24 hours"} };
    struct CSVSearchHandler srchHndl = {
        L"Name",
        searchFor,
        1,
        returnWhat,
        2,
        NULL
    };

    int srchAns = searchCSV(&csv, &srchHndl);
    free(wResponse);

    dataBuffer->available = 1;
    dataBuffer->total = (srchAns != 0 || srchHndl.returnBuff[0][0] == NULL) ? -1 : wcstol(srchHndl.returnBuff[0][0], NULL, 10);
    dataBuffer->increase = (srchAns != 0 || srchHndl.returnBuff[0][1] == NULL) ? -1 : wcstol(srchHndl.returnBuff[0][1], NULL, 10);

    return 0;

}

/* returns 1 if fresh data loaded, otherwise 0  */
int tryLoadComparisonData() {
    
    int czLoaded = 1;
    int wdLoaded = 1;

    czLoaded = loadCzechData(&sCompDataBufferCz);
    if (czLoaded != 0) {

        if (czLoaded < 0 && czLoaded >= -4) {
            switchMainError((enum ErrorMessage) EM_NO_INTERNET_CONNECTION);
        }
        else {
            switchMainError((enum ErrorMessage) EM_NO_DATA_FOR_THE_MOMENT);
        }

    } else {

        wdLoaded = loadWorldData(&sCompDataBufferWd);
        if (wdLoaded < 0 && wdLoaded >= -4) {
            switchMainError((enum ErrorMessage) EM_NO_INTERNET_CONNECTION);
        } else if (wdLoaded != 0) {
            switchMainError((enum ErrorMessage) EM_NO_DATA_FOR_THE_MOMENT);
        }

    }

    return (czLoaded == 0) && (wdLoaded == 0);

}

/* function searches for the saved file, if no file present, does get request to get file */
void loadVaccineData() {

    wchar_t* csvData;
    int csvDataLen;

    FILE* cmpFile;

    char fileName[sizeof(VACCINATION_DATA_FOLDER_PATH) + 14];
    
    strcpy(
        fileName,
        VACCINATION_DATA_FOLDER_PATH
    );

    int labelLen;
    wchar_t* wlabel = getCurrentDayLabel(daySelector, &labelLen);
    
    char label[11];
    wcstombs(label, wlabel, labelLen);

    strcat(
        fileName,
        label
    );

    strcat(
        fileName,
        ".csv"
    );

    cmpFile = fopen(fileName, "r");
    if (cmpFile == NULL) {
        /* 
        *   file doesn't exist or we have no acces to read it
        *   if file has to have information about today, we ask
        *   the source server through request about data, otherwise
        *   just assume data as invalid and return
        */
        
        if (getRealSelectedDay(daySelector) == lastDay + 1 && comparisonData[lastDay * 6] > -2) {
            goto loadDataAsRequest;
        } else {
            vaccinationData[0] = -2;
        }

        return;
        
    }

    csvDataLen = readFullFile(&csvData, cmpFile);
    if (csvDataLen <= 0) {
        vaccinationData[0] = -2;
        return;
    }

    goto processData;

loadDataAsRequest: ;

    char* response;
    int responseLen;

    wchar_t* rootUrl = L"covid19.who.int";
    wchar_t* pathUrl = L"/who-data/vaccination-data.csv";

    response = NULL;

    responseLen = 0;
    if ((responseLen = httpsGet(rootUrl, pathUrl, &response)) <= 0) {

        vaccinationData[0] = -2;
        free(response);
        return 1;
    
    }

    /*  
    *   for now using lib, it works somehow
    */
    int ret;
    int unzippedResponseLen = 50000;
    /* allocated on heap as it seems like big buffer for stack, or at least vs studio told me so */
    unsigned char* unzippedResponse = (char*) malloc(sizeof(char) * unzippedResponseLen);
    if (unzippedResponse == NULL) {
        goto zlibCleanup;
    }

    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    infstream.avail_in = (uInt)(responseLen + 1);     /* input size in bytes*/
    infstream.next_in = (Bytef*)response;             /* input */
    infstream.avail_out = (uInt)unzippedResponseLen;  /* output size in bytes */
    infstream.next_out = (Bytef*)unzippedResponse;    /* output */

    ret = inflateInit2(&infstream, 16 + MAX_WBITS);
    if (ret != 0) {
        goto zlibCleanup;
    }

    ret = inflate(&infstream, Z_FINISH);
    if (ret != 1) {
        goto zlibCleanup;
    }

    ret = inflateEnd(&infstream);
    if (ret != 0) {
        goto zlibCleanup;
    }

    csvDataLen = infstream.total_out;
    unzippedResponse[csvDataLen] = '\0';

    free(response);

    csvData = (wchar_t*) malloc(sizeof(wchar_t) * csvDataLen);
    if (csvData == NULL) {
        return 0;
    }

    mbstowcs(
        csvData,
        unzippedResponse,
        csvDataLen
    );

    writeFile(unzippedResponse, csvDataLen, fileName);
    free(unzippedResponse);

processData: ;

    struct CSV csv = {
        csvData,
        csvDataLen,
        COMMA
    };

    wchar_t** countries = countrySelector->countries;
    int* countriesLengths = countrySelector->countriesLengths;
    SelectButton** butt = countrySelector->selectButtons;

    wchar_t* searchFor[] = {
        { countries[CZECHIA_INDEX] },
        { countries[butt[0]->selected] },
        { countries[butt[1]->selected] },
        { countries[butt[2]->selected] },
        { countries[butt[3]->selected] }
    };

    wchar_t* returnWhat[] = { {L"PERSONS_VACCINATED_1PLUS_DOSE_PER100"} };
    struct CSVSearchHandler srchHndl = {
        L"COUNTRY",
        searchFor,
        countrySelector->buttonsCount + 1,
        returnWhat,
        1,
        NULL
    };

    searchCSV(&csv, &srchHndl);
    free(csvData);

    if (srchHndl.returnBuff == NULL) {
        vaccinationData[0] = -2;
        return;
    }

    for (int i = 0; i < countrySelector->buttonsCount + 1; i++) {
        
        if (srchHndl.returnBuff[i] != NULL && srchHndl.returnBuff[i][0] != NULL) {
            int value = (int) (wcstof(srchHndl.returnBuff[i][0], NULL) * 100);
            vaccinationData[i] = value;
        } else {
            vaccinationData[i] = -1;
        }
    
    }

    return 0;

zlibCleanup:

    vaccinationData[0] = -2;
    free(response);
    return 1;

}

/* is kinda overkill, but let it be */
int isNewDay(LocalDateTime *today) {

    time_t tm = time(NULL);
    LocalDateTime now = *localtime(&tm);
    now.tm_year += 1900;
    now.tm_mon += 1;

    if (
        now.tm_year == today->tm_year &&
        now.tm_mon == today->tm_mon &&
        now.tm_mday == today->tm_mday
    ) {
        return 0;
    }

    return 1;

}

/*
*   adds new blank day, if the limit of buffer is not reach
*   appends data, otherwise shiffts data in buffer to the start by one field
*   and wrutte data at the end of the buffer
*/
void addNewDay() {

    lastDay++;

    if (lastDay >= daysAllocated) {

        int count = daysAllocated + DAYS_TO_ADD_AT_EXPANSION;
        
        int* tmp = (int*) realloc(comparisonData, count * sizeof(int));
        if (tmp == NULL) {

            /* cannot alloc more space, just return */
            return;
        
        } else if (comparisonData != tmp) {

            comparisonData = tmp;
        
        }

        tmp = NULL;

        daysAllocated += DAYS_TO_ADD_AT_EXPANSION;
        firstDayTime += SECONDS_IN_A_DAY;

    }

    int idx = lastDay * 6;

    comparisonData[idx + 0] = -2;
    comparisonData[idx + 1] = -1;

    comparisonData[idx + 2] = -1;
    comparisonData[idx + 3] = -1;
    
    comparisonData[idx + 4] = -1;
    comparisonData[idx + 5] = -1;

    addDayTabDaySelector(daySelector);

}

/*
*   adds new data to the global buffer at the last day position
*/
void fillNewDay(CompDataBuffer* dataBufferCz, CompDataBuffer* dataBufferWd) {

    int idx = lastDay * 6;

    comparisonData[idx + 0] = dataBufferCz->increase;
    comparisonData[idx + 1] = dataBufferWd->increase;

    comparisonData[idx + 2] = dataBufferCz->total;
    comparisonData[idx + 3] = dataBufferWd->total;

    comparisonData[idx + 4] = dataBufferCz->timeDiff;
    comparisonData[idx + 5] = dataBufferWd->timeDiff;

}

/*
*   assynchronusly traies to download data
*   hope these mutex magic works somehow
*/
int requestButtonCallback() {
    
    DWORD dwWaitResult = WaitForSingleObject(
        ghMutex,
        INFINITE
    );

    if (dwWaitResult != WAIT_OBJECT_0) {
        return 0;
    }

    if (comparisonData[lastDay * 6] > -2) {
        return 0;
    }

    int rMsg = 0;
    if (tryLoadComparisonData()) {

        fillNewDay(&sCompDataBufferCz, &sCompDataBufferWd);
        saveComparasionData(comparisonData + lastDay * 6);
        loadVaccineData();

        requestButton->visible = 0;

        rMsg = 1;

    }

    ReleaseMutex(ghMutex);

    return rMsg;

}

VOID CALLBACK drawRequestError(
    HWND hwnd,
    UINT msg,
    UINT_PTR timerId,
    DWORD millis
) {

    switchMainError((enum errorMessage) lastErrorMessage);
    
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
    KillTimer(hwnd, timerId);

}

void switchMainError(enum errorMessage error) {

    switch (error) {

        case EM_NO_INTERNET_CONNECTION: {
    
            errorMessageLen = 22;
            errorMessage = L"NO_INTERNET_CONNECTION";
            lastErrorMessage = EM_NO_INTERNET_CONNECTION;

            break;

        }

        case EM_REQUEST_FAILED: {

            errorMessageLen = 14;
            errorMessage = L"REQUEST_FAILED";

            break;

        }

        case EM_NO_DATA_FOR_THE_MOMENT: {

            errorMessageLen = 22;
            errorMessage = L"NO_DATA_FOR_THE_MOMENT";
            lastErrorMessage = EM_NO_DATA_FOR_THE_MOMENT;
            
            break;
        
        }
    
    }

}

void drawDay(int day) {

    if (
        (comparisonData[day * 6] <= -2) && 
        getRealSelectedDay(daySelector) == lastDay + 1
    ) {
    
        drawCallbackButton(requestButton);

        drawErrorMessage(
            BORDER_MARGIN,
            BORDER_MARGIN,
            SCREEN_WIDTH - 2 * BORDER_MARGIN,
            SCREEN_HEIGHT - 2 * BORDER_MARGIN - daySelector->height - 2 * BORDER_MARGIN
        );

        return;

    }

    drawComparisonData(day, comparisonDataViewType);
    drawVaccineComparasion(day, vaccinationDataViewType);

}

void drawComparisonData(int day, int type) {

    const int height = SCREEN_HEIGHT / 2 - daySelector->height / 2 - 1.5 * BORDER_MARGIN;

    renderColor = WHITE;
    drawRect(
        BORDER_MARGIN,
        BORDER_MARGIN,
        SCREEN_WIDTH - 2 * BORDER_MARGIN,
        height
    );

    if (day < 0 || day >= daysAllocated || comparisonData[day * 6] <= -2) {
        
        drawNoDataError(
            BORDER_MARGIN,
            BORDER_MARGIN,
            SCREEN_WIDTH - 2 * BORDER_MARGIN,
            height
        );

        return;
    
    }

    switch (type) {

        case 0: {

            const wchar_t* titles[] = {
                L"Increase",
                L"Total",
                L"Delay"
            };

            const int titlesLengths[] = {
                8,
                5,
                5
            };

            const int width = (SCREEN_WIDTH - 2 * BORDER_MARGIN) / 4;
            const int dataLength = 2;

            int x = width + BORDER_MARGIN;
            int y = BORDER_MARGIN;

            int i = 0;
            do {

                int* data = &comparisonData[day * 6 + 2 * i];

                HistData hData = {
                    titles[i],
                    titlesLengths[i],
                    data,
                    GRAPH_COLORS,
                    dataLength
                };

                Graph histIncrease = newHist(x, y, width, height, &hData);
                histIncrease.draw(&histIncrease);

                x += width;

                i++;

            } while (i < 3);

            int strSz = 16;
            int titlesCount = 2;

            const wchar_t* legendNames[] = {
                L"Czechia",
                L"World"
            };

            const int legendNamesLengths[] = {
                7,
                5
            };

            x = BORDER_MARGIN;
            y = BORDER_MARGIN;

            drawLegend(
                legendNames,
                legendNamesLengths,
                titlesCount,
                strSz,
                GRAPH_COLORS,
                x,
                y,
                width,
                height
            );

            break;

        }

        case 1: {

            const wchar_t CZECHIA[] = L"CZECHIA";
            const wchar_t WORLD[] = L"WORLD";
            const wchar_t DIFF[] = L"DIFFERENCE";

            const int CZECHIA_LEN = 7;
            const int WORLD_LEN = 5;
            const int DIFF_LEN = 10;

            wchar_t* titles[3] = {
                L"Increase [cnt]",
                L"Total    [cnt]",
                L"Delay    [min]"
            };

            const int titlesLengths[] = {
                14,
                14,
                14
            };

            const int width = (SCREEN_WIDTH - 2 * BORDER_MARGIN);

            const double marginRate = 0.05;
            const int marginX = width * marginRate;
            const int marginY = height * marginRate;

            const int lineHg = (height - marginY * 2) / 4;
            const int lineWd = (width - marginX * 2) / 4;

            const int dataLength = 2;

            const int strSz = lineHg * 0.2;

            int x = BORDER_MARGIN + marginX;
            int y = BORDER_MARGIN + marginY;

            /*
                    ~    | CZECHIA | WORLD | DIFF
                INCREASE |
                TOTAL    |
                DELAY    |
            */

            StringWriter lStringWriter = StringWriterLeft();
            StringWriter rStringWriter = StringWriterRight();
            renderRect strRect = { x + lineWd, y, lineWd, lineHg };

            rStringWriter.write(CZECHIA, CZECHIA_LEN, COLOR_MAIN_BACK, strSz, &strRect);

            strRect.x += lineWd;
            rStringWriter.write(WORLD, WORLD_LEN, COLOR_MAIN_BACK, strSz, &strRect);

            strRect.x += lineWd;
            rStringWriter.write(DIFF, DIFF_LEN, COLOR_MAIN_BACK, strSz, &strRect);

            y += lineHg;

            int i = 0;
            do {

                int czData = comparisonData[day * 6 + 2 * i];
                int wdData = comparisonData[day * 6 + 2 * i + 1];
                int diff = abs(czData - wdData);

                int czColor;
                int wdColor;

                if (czData < wdData) {
                    czColor = LIME;
                    wdColor = COLOR_ERROR;
                }
                else if (czData > wdData) {
                    czColor = COLOR_ERROR;
                    wdColor = LIME;
                }
                else {
                    czColor = COLOR_MAIN_FRONT;
                    wdColor = COLOR_MAIN_FRONT;
                }

                wchar_t czDataW[11];
                swprintf(czDataW, 11, L"%i", czData);

                wchar_t wdDataW[11];
                swprintf(wdDataW, 11, L"%i", wdData);

                wchar_t diffW[11];
                swprintf(diffW, 11, L"%i", diff);

                strRect.y = y;

                renderColor = COLOR_MAIN_FRONT;
                strRect.x = x;
                lStringWriter.write(titles[i], titlesLengths[i], COLOR_MAIN_BACK, strSz, &strRect);

                renderColor = czColor;
                strRect.x += lineWd;
                rStringWriter.write(czDataW, wcslen(czDataW), COLOR_MAIN_BACK, strSz, &strRect);

                renderColor = wdColor;
                strRect.x += lineWd;
                rStringWriter.write(wdDataW, wcslen(wdDataW), COLOR_MAIN_BACK, strSz, &strRect);

                renderColor = COLOR_MAIN_FRONT;
                strRect.x += lineWd;
                rStringWriter.write(diffW, wcslen(diffW), COLOR_MAIN_BACK, strSz, &strRect);

                y += lineHg;

                i++;

            } while (i < 3);

            break;
    
        }

    }

}

void drawVaccineComparasion(int day, int type) {

    const int height = (SCREEN_HEIGHT - daySelector->height) / 2 - 1.5 * BORDER_MARGIN;

    renderColor = WHITE;
    drawRect(
        BORDER_MARGIN, 
        countrySelector->y, 
        SCREEN_WIDTH - 2 * BORDER_MARGIN, 
        height
    );

    if (day < 0 || day >= daysAllocated || vaccinationData[0] <= -2) {

        drawNoDataError(
            BORDER_MARGIN,
            countrySelector->y,
            SCREEN_WIDTH - 2 * BORDER_MARGIN,
            height
        );

        return;

    }

    const int width = SCREEN_WIDTH - countrySelector->width - 2 * BORDER_MARGIN;
    const int dataLength = countrySelector->countriesCount;

    /*
        not fancy, and not even a general solution, but dont wanna to alloc something an
        free it latter, if it could be done another way, but has to be changed latter, if
        count of displayable countries will grow
    */

    wchar_t** countries = countrySelector->countries;
    int* countriesLengths = countrySelector->countriesLengths;
    SelectButton** butt = countrySelector->selectButtons;

    wchar_t* countriesNames[] = {
        countries[CZECHIA_INDEX],
        countries[butt[0]->selected],
        countries[butt[1]->selected],
        countries[butt[2]->selected],
        countries[butt[3]->selected]
    };

    int countriesNamesLengths[] = {
        countriesLengths[CZECHIA_INDEX],
        countriesLengths[butt[0]->selected],
        countriesLengths[butt[1]->selected],
        countriesLengths[butt[2]->selected],
        countriesLengths[butt[3]->selected]
    };

    switch (type) {

        case 0: {

            int x = BORDER_MARGIN;
            int y = countrySelector->y;

            const wchar_t title[] = L"Vaccination";
            const int titleLength = 11;

            HistData hData = {
                title,
                titleLength,
                vaccinationData,
                GRAPH_COLORS,
                countrySelector->buttonsCount + 1
            };

            Graph histIncrease = newHist(x, y, width, height, &hData);
            histIncrease.draw(&histIncrease);


            if (countrySelector->visible) return;


            int strSz = 16;
            int titlesCount = 5;

            drawLegend(
                countriesNames,
                countriesNamesLengths,
                titlesCount,
                strSz,
                GRAPH_COLORS,
                countrySelector->x,
                countrySelector->y,
                countrySelector->width,
                countrySelector->height
            );

            break;
        
        }

        case 1: {

            const double marginRate = 0.05;
            const int marginX = width * marginRate;
            const int marginY = height * marginRate;

            const int lineHg = (height - marginY * 2) / 6;
            const int chunkWd = (width - marginX * 2) / 3;
            const int leftWd = 2 * chunkWd;
            const int rightWd = chunkWd;

            const int strSz = lineHg * 0.4;

            int x = BORDER_MARGIN + marginX;
            int y = countrySelector->y + marginY;

            const wchar_t title[] = L"RATIO";
            const int titleLength = 5;

            StringWriter lStringWriter = StringWriterLeft();
            StringWriter rStringWriter = StringWriterRight();
            renderRect strRect = { x + leftWd, y, rightWd, lineHg };

            rStringWriter.write(title, titleLength, COLOR_MAIN_BACK, strSz, &strRect);

            y += lineHg;

            int minRatio = INT_MAX;
            int maxRatio = 0;
            for (int i = 0; i < COUNTRIES_TO_SELECT + 1; i++) {
                
                if (vaccinationData[i] > maxRatio) {
                    maxRatio = vaccinationData[i];
                } 
                
                if (vaccinationData[i] < minRatio) {
                    minRatio = vaccinationData[i];
                }
            
            }

            int i = 0;
            do {

                int data = vaccinationData[i];

                wchar_t dataW[11];
                swprintf(dataW, 11, L"%0.2f%%", (double) data / 100);

                strRect.y = y;

                renderColor = GRAPH_COLORS[i];
                strRect.x = x;
                strRect.width = leftWd;
                lStringWriter.write(countriesNames[i], countriesNamesLengths[i], COLOR_MAIN_BACK, strSz, &strRect);

                if (data <= minRatio) renderColor = COLOR_ERROR;
                else if (data >= maxRatio) renderColor = LIME;
                else renderColor = COLOR_MAIN_FRONT;

                strRect.x += strRect.width;
                strRect.width = rightWd;
                rStringWriter.write(dataW, wcslen(dataW), COLOR_MAIN_BACK, strSz, &strRect);

                y += lineHg;

                i++;

            } while (i < COUNTRIES_TO_SELECT + 1);

            const int histWd = countrySelector->width - marginX;
            const int histHg = countrySelector->height - 2 * marginY - lineHg;

            HistData hData = {
                L"TITLE",
                5,
                vaccinationData,
                GRAPH_COLORS,
                countrySelector->buttonsCount + 1
            };

            Graph histIncrease = newHistRow(
                x + leftWd + rightWd + marginX, 
                countrySelector->y + marginY + lineHg, 
                histWd, 
                histHg, 
                &hData
            );
            histIncrease.draw(&histIncrease);

            break;
        
        }
    
    }

}

/* CTRL + C | CTRL + V */
void drawNoDataError(int x, int y, int width, int height) {

    renderColor = COLOR_ERROR;

    const int strSize = 20;
    const int strLength = 17;
    wchar_t str[] = L"NO_DATA_AVALIABLE";

    StringWriter stringWriter = StringWriterCenter();
    renderRect strRect = { x, y, width, height };

    stringWriter.write(str, strLength, 0, strSize, &strRect);

}

void drawErrorMessage(int x, int y, int width, int height) {

    renderColor = COLOR_ERROR;

    const int strSize = 20;

    StringWriter stringWriter = StringWriterCenter();
    renderRect strRect = { x, y, width, height };

    stringWriter.write(errorMessage, errorMessageLen, 0, strSize, &strRect);

}

int exitWithError(const wchar_t* msg, enum ExitErrors err) {
    
    MessageBox(NULL, msg, EXIT_ERRORS[err], MB_ICONEXCLAMATION | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
    exit(1);
}

int showError(const wchar_t* msg) {
    MessageBox(NULL, msg, L"Error!", MB_ICONEXCLAMATION | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
}
