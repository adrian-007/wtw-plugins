#pragma once

#include "wtwPlugin.h"

/*******************************************************************************
**
** Function: 
**		WTW_WEB_INFO_GET
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - netLibCreateDef*
**		lP - NULL
**
*/

#define WTW_WEB_INFO_GET	L"WTW/WebInfoGet"

/*******************************************************************************
**
** Function: 
**		WTW_WEB_INFO_CANCEL
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - const wchar_t* id
**		lP - NULL
**
*/

#define WTW_WEB_INFO_CANCEL	L"WTW/WebInfoCancel"

struct wtwWebInfo
{
	__wtwStdStructDefs(wtwWebInfo);

	int				structSize;

	const wchar_t * id;
	const wchar_t *	url;

	const wchar_t *	filePath;

	WTWFUNCTION		callback;	// wP - wtwWebThumbinalEvent*
	void		 *	cbData;

	void *			ownerData;

	DWORD			flags;

	DWORD			cx;	// max 1280
	DWORD			cy;	// max 1024

	DWORD			cropFactor;
};

#define WTW_WEB_INFO_FLAG_GET_THUMBINAL		0x000001
#define WTW_WEB_INFO_FLAG_GET_ALL			0x0000FF

#define WTW_WEB_INFO_FLAG_OVERRIDE_SETTINGS	0x100000	// if this is not specified, functions will behave according to user options
#define WTW_WEB_INFO_FLAG_IMG_SIZE			0x000100
#define WTW_WEB_INFO_FLAG_CROP				0x000200	// crop to 4:3 aspect ratio or to img size if WTW_WEB_INFO_FLAG_IMG_SIZE is specified
#define WTW_WEB_INFO_FLAG_CROP_CENTER		0x000400	// you have to set cropFactor
#define WTW_WEB_INFO_FLAG_CROP_VCENTER		0x000800	// you have to set cropFactor
#define WTW_WEB_INFO_FLAG_CROP_FACTOR		0x001000

#define WTW_WEB_INFO_FLAG_THUMB_TYPE_PNG	0x000000	// default
#define WTW_WEB_INFO_FLAG_THUMB_TYPE_JPG	0x010000
#define WTW_WEB_INFO_FLAG_THUMB_TYPE_BMP	0x020000

struct wtwWebInfoEvent
{
	__wtwStdStructDefs(wtwWebInfoEvent);

	int				structSize;

	const wchar_t * id;
	const wchar_t *	url;
	const wchar_t *	thumbFilePath;

	const wchar_t * siteTitle;
	const wchar_t * siteDescription;
	const wchar_t * mimeType;

	void *			ownerData;

	int				event;

	DWORD			flags;
};

#define WTW_WEB_INFO_EVENT_COMPLETE		1
#define WTW_WEB_INFO_EVENT_ERROR		2
#define WTW_WEB_INFO_EVENT_CANCELED		3
#define WTW_WEB_INFO_EVENT_DESTROY		4