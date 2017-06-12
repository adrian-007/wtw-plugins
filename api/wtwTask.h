/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

/*******************************************************************************
**
** Function: 
**		WTW_TASK_CREATE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwTaskDef*
**		lP - NULL
**
*/

#define WTW_TASK_CREATE L"WTW/Task/Create"

struct wtwTaskDef
{
	__wtwStdStructDefs(wtwTaskDef);

	int			structSize;

	const wchar_t * id;

	WTWFUNCTION taskFunction;	// this will be called not-from-the-main-thread
	WTW_PARAM	wP;				// parameters for task function
	WTW_PARAM	lP;
	void	*	taskCbData;

	WTWFUNCTION	callback;		// upon completion (error or canclation), 
								// this will be called from main thread,
								// wP = wtwTaskEvent*
	void *		cbData;

	DWORD		flags;
};

struct wtwTaskEvent
{
	__wtwStdStructDefs(wtwTaskEvent);

	int				structSize;

	const wchar_t * id;

	WTW_PARAM		wP;				// task function params
	WTW_PARAM		lP;

	DWORD			flags;
};
