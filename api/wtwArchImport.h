/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

#define WTW_ARCH_IMPORT_REGISTER	L"WTW/ArchImportR"
#define WTW_ARCH_IMPORT_UNREGISTER	L"WTW/ArchImportU"

struct wtwImportMech 
{
	__wtwStdStructDefs(wtwImportMech);

	int				structSize;

	const wchar_t * id;
	
	WTWFUNCTION		callback;		// this will be called from non-main thread, so rememer about locking
	void *			cbData;

	const wchar_t * mechName;
};

struct wtwImportMechEvent 
{
	__wtwStdStructDefs(wtwImportMechEvent);

	int				structSize;

	const wchar_t * id;

	HWND			hStatus;
	HWND			hProgess;
	HWND			hMainDlg;

};