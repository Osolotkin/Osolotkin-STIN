#pragma once

#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#pragma comment(lib, "wininet.lib")

#define HTTPS_PORT 443
#define HTTP_PORT 80

const wchar_t appName[] = L"SUPER_NAME";
const wchar_t HTTP_VER[] = L"HTTP/1.1";

int httpGetGeneric(wchar_t* rootUrl, wchar_t* pathUrl, char** response, int ssl);




int httpGet(wchar_t* rootUrl, wchar_t* pathUrl, char** response) {

	return httpGetGeneric(rootUrl, pathUrl, response, 1, 0);

}

int httpsGet(wchar_t* rootUrl, wchar_t* pathUrl, char** response) {

	return httpGetGeneric(rootUrl, pathUrl, response, 1, 1);

}




// int ssl param should be either 0 or 1, 1 uses ssl, at least hope so, to connect to the server 
int httpGetGeneric(wchar_t* rootUrl, wchar_t* pathUrl, char** response, int ssl) {

	int PORT;
	int FLAG;

	if (ssl) {
		PORT = HTTPS_PORT;
		FLAG = INTERNET_FLAG_SECURE;
	}
	else {
		PORT = HTTP_PORT;
		FLAG = 0;
	}

	*response = NULL;

	int error = 0;

	HINTERNET iOpenHandle = NULL;
	HINTERNET iConnectHandle = NULL;
	HINTERNET getReqHandle = NULL;

	iOpenHandle = InternetOpenW(
		appName,
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		0
	);

	if (iOpenHandle == NULL) {
		error = -1;
		goto exitAndCleanup;
	}

	iConnectHandle = InternetConnectW(
		iOpenHandle,
		rootUrl,
		PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0
	);

	if (iConnectHandle == NULL) {
		error = -2;
		goto exitAndCleanup;
	}

	getReqHandle = HttpOpenRequestW(
		iConnectHandle,
		NULL,
		pathUrl,
		HTTP_VER,
		NULL,
		NULL,
		FLAG,
		0
	);

	if (getReqHandle == NULL) {
		error = -3;
		goto exitAndCleanup;
	}

	if (!HttpSendRequestW(getReqHandle, NULL, 0, NULL, 0)) {
		error = -4;
		goto exitAndCleanup;
	}

	int queryBuffLen = 64;
	char queryBuff[64];
	int qAns = HttpQueryInfoA(
		getReqHandle,
		HTTP_QUERY_CONTENT_LENGTH,
		queryBuff,
		&queryBuffLen,
		0
	);

	if (qAns != 1) {
		/* as we getting just length, we can not to care about wrong length and just return if length will be higher than 64 digits */
		goto exitAndCleanup;
	}

	int bodyLen = strtol(queryBuff, NULL, 10);

	unsigned long responseSize = 0;
	if (!InternetQueryDataAvailable(getReqHandle, &responseSize, 0, 0)) {
		error = -5;
		goto exitAndCleanup;
	}

	if (responseSize <= 0) {
		error = 0;
		goto exitAndCleanup;
	}

	*response = (char*)malloc((bodyLen + 1) * sizeof(char));
	if (*response == NULL) {
		error = -6;
		goto exitAndCleanup;
	}

	char* pResponse = *response;
	unsigned long bytesRead = 0;
	error = bodyLen;
	int rdCmplt = 0;
	int remaining = bodyLen;
	do {

		rdCmplt = InternetReadFile(getReqHandle, pResponse, remaining, &bytesRead);
		if (rdCmplt == 0) {
			
			free(*response);
			*response = NULL;

			error = -7;
			goto exitAndCleanup;
		
		}

		remaining -= bytesRead;
		if (rdCmplt && remaining == 0) {
			break;
		}
		pResponse += bytesRead;

	} while (1);

	(*response)[bodyLen] = '\0';


exitAndCleanup:

	if (getReqHandle != NULL) InternetCloseHandle(getReqHandle);
	if (iConnectHandle != NULL) InternetCloseHandle(iConnectHandle);
	if (iOpenHandle != NULL) InternetCloseHandle(iOpenHandle);

	return error;

}
