/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"
#include "wtwProtocol.h"

struct wtwHttpGetFile
{
	__wtwStdStructDefs(wtwHttpGetFile);

	int				structSize;

	const wchar_t * fileId;	

	const wchar_t * uri;
	const wchar_t * method; // NULL = GET (unless HEADERS_ONLY is specified, if so NULL = "HEAD"), or GET, POST, etc
	const wchar_t * headers; // can be NULL
	const void *	methodData; // can be NULL if you want to pass method data for GET, 
								// then this must be wchar_t null terminated string
	DWORD			methodDataSize; // sizeof(*methodData)

	const wchar_t * file;
	DWORD			flags;

	WTWFUNCTION		callback;	// required
	void	 *		cbData;

	void	*		ownerData;
};

struct wtwHttpEvent
{
	__wtwStdStructDefs(wtwHttpEvent);

	int				structSize;

	int				event;
	const wchar_t * fileId;

	void *			ownerData;
};

struct wtwHttpStatus
{
	__wtwStdStructDefs(wtwHttpStatus);

	int				structSize;

	HRESULT			status; // S_OK/E_*
	int				responseCode;	// http response code

	const wchar_t * filePath;
};

struct wtwHttpProgress
{
	__wtwStdStructDefs(wtwHttpProgress);

	int				structSize;

	__int64			fileSize;
	__int64			downloaded;

	DWORD			flags;
};

#define WTW_HTTP_SERVICE_ASYNCHRONOUS	0x00000001
#define WTW_HTTP_SERVICE_SYNCHRONOUS	0x00000002	// not implemented, besides mutally exclusive with "ASYNC"
#define WTW_HTTP_SERVICE_WRITE_TO_FILE	0x00000004	// write fata to file, file path must be valid 
#define WTW_HTTP_SERVICE_WRITE_TO_CB	0x00000008	// generate "DATA" evetent(s)
#define WTW_HTTP_SERVICE_PROGRESS		0x00000100	// generate "PROGRESS" evetent(s)
#define WTW_HTTP_SERVICE_WRITE_TO_MEMB	0x00000200	// write to wtw::CBuffer* placed in 'file' (cannot be used with _TO_FILE)
#define WTW_HTTP_SERVICE_NO_URI_ESCAPE	0x00000010	// 
#define WTW_HTTP_SERVICE_HEADERS_ONLY	0x00000020	// get only headers, make "HEAD" request, write headers as file data
#define WTW_HTTP_SERVICE_USE_WRITE		0x00000080	// ignore method data and do upload in EVENT_WRITE, return S_OK
													// if you want to continue, or S_FALSE if upload is complete
#define WTW_HTTP_SERVICE_NO_VERIFY_CERT	0x00000100	// for https
#define WTW_HTTP_SERVICE_NO_FOLLOW		0x00000200	// do not follow http redirection
#define WTW_HTTP_SERVICE_TS_CB_DATA		0x00000400	// callback is thread-safe, it can be called from worker thred directly
#define WTW_HTTP_SERVICE_TS_CB_HEADER	0x00000800	// callback is thread-safe, it can be called from worker thred directly
#define WTW_HTTP_SERVICE_TS_CB_COMPLETE	0x00001000	// callback is thread-safe, it can be called from worker thred directly
#define WTW_HTTP_SERVICE_TS_CB_WRITE	0x00002000	// callback is thread-safe, it can be called from worker thred directly
#define WTW_HTTP_SERVICE_THREAD_SAFE_CB 0x00003C00	// 

#define WTW_HTTP_EVENT_DATA			0x0001 // lP - wtwRawDataDef*
#define WTW_HTTP_EVENT_COMPLETE		0x0002 // lP - wtwHttpStatus*
#define WTW_HTTP_EVENT_HEADER		0x0003 // lP - wtwRawDataDef* (data -> wchar_t*, called once per header)
#define WTW_HTTP_EVENT_WRITE		0x0004 // lP - wtwHttpBuffer*
#define WTW_HTTP_EVENT_PROGRESS		0x0005 // lP - wtwHttpProgress*

struct wtwHttpBuffer
{
	__wtwStdStructDefs(wtwHttpBuffer);

	int				structSize;

	void *			pBuffer;		// pointer to the buffer where You should store data
	DWORD			dwBufferSize;	// maximum size of data that buffer may contain, in bytes
	DWORD			dwDataSize;		// set this to actual length of the data, You placed there
};

#define	WTW_HTTP_GET_FILE			L"WTW/HttpGetFile" // wP - wtwHttpGetFile*