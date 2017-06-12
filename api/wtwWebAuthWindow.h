#pragma once

#include "wtwPlugin.h"

#define WTW_WEB_AUTH_EVENT_VERIFY_URL	0x0001
#define WTW_WEB_AUTH_EVENT_DESTROY		0x0002
#define WTW_WEB_AUTH_EVENT_DOC_COMPLETE	0x0003

struct wtwWebAuthUrl
{
	__wtwStdStructDefs(wtwWebAuthUrl);

	int			structSize;

	const wchar_t * pUrl;
	const void *	interafacePtr;
};

#define WTW_WEB_AUTH_FLAG_MORE_HEIGHT	0x0001

struct wtwWebAuthDocComplete
{
	__wtwStdStructDefs(wtwWebAuthDocComplete);

	int			structSize;

	const void * interafacePtr;
	const void * redirectUrl;
	const wchar_t *pUrl;
};


struct wtwWebAuthWindow
{
	__wtwStdStructDefs(wtwWebAuthWindow);

	int			structSize;

	HWND			hParent;
	WTWFUNCTION		callback;
	void *			cbData;

	const wchar_t * initialUrl;
	const wchar_t * sandboxUrl;

	const wchar_t * windowTitle;
	const wchar_t * windowHint;

	DWORD			flags;

	const wchar_t * userAgent;
};

#define WTW_WEB_AUTH_SHOW_WINDOW	L"WTW/WebAuthShowWnd"