#pragma once

#include <wchar.h>

int httpGet(wchar_t* rootUrl, wchar_t* pathUrl, char** response);
int httpsGet(wchar_t* rootUrl, wchar_t* pathUrl, char** response);
