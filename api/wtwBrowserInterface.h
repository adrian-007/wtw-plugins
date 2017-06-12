/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

/*******************************************************************************
**
** Function: 
**		WTW_BROWSER_INT_ALLOCATE_EX
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwBrowserInfo*
**		lP - NULL
**
*/

#define WTW_BROWSER_INT_ALLOCATE_EX	L"WTW/ReqBrIntPtrEx"

struct wtwBrowserInfo
{
	__wtwStdStructDefs(wtwBrowserInfo);

	int				structSize;

	DWORD			flags;
	int				location;
	void *			brPtr;	// return value, wtw::CBrowserInterface*
};

#define WTW_BROWSER_FLAG_FORCE_HTMLAYOUT	0x0001L
#define WTW_BROWSER_FLAG_FORCE_IEXPLORE		0x0002L

/*******************************************************************************
**
** Function: 
**		EXT_BROWSER_INT_ALLOCATE
**
** Return value: 
**		wtw::CBrowerInterface*
**
** Parameters:
**		wP - WTW_BROWSER_LOCATION_*
**		lP - NULL
**
** Remarks:
**		Register this function if You want to override default layout engine
**
*/

#define EXT_BROWSER_INT_ALLOCATE	L"Browser/Allocate"


#define WTW_BROWSER_LOCATION_UNKNOWN		0x0000L
#define WTW_BROWSER_LOCATION_CHATWND		0x0001L
#define WTW_BROWSER_LOCATION_CONFERENCE_TAB 0x0002L
#define WTW_BROWSER_LOCATION_CONFERENCE_LOG 0x0003L
#define WTW_BROWSER_LOCATION_PROTO_CONSOLE	0x0004L
#define WTW_BROWSER_LOCATION_ARCH_WND		0x0005L
#define WTW_BROWSER_LOCATION_SYS_MSG_WND	0x0006L
#define WTW_BROWSER_LOCATION_PLUGIN_WND		0x0007L
#define WTW_BROWSER_LOCATION_OPTIONS_PLUG	0x0008L
#define WTW_BROWSER_LOCATION_CONFERENCE_CHT 0x0009L

#define WTW_GET_NEW_IE_CONTROL_SITE		L"WTW/GetIEControlSite"

struct wtwIeControlSite
{
	__wtwStdStructDefs(wtwIeControlSite);

	int structSize;

	void *pCtrlCont;
	void *pBrowser;

	void *pCustomSite;
};