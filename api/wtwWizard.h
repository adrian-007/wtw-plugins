/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

struct wtwWizard
{
	__wtwStdStructDefs(wtwWizard);

	int			structSize;

	const wchar_t	* id;			// must be always valid
	const wchar_t	* wizzardTitle;

	DWORD			flags;

	WTWFUNCTION		callback;		// wP - wtwWizardEvent*, lP - wtwWizardPage* (for current page) or NULL
	void *			cbData;

	HWND			hParent;		// for DO_MODAL only
	const wchar_t * iconId;

	POINT			minSize;		// 
	POINT			maxSize;		// 
	POINT			defSize;		// 
};

#define WTW_WIZARD_FLAG_ENABLE_FINISH	0x00000001
#define WTW_WIZARD_FLAG_ENABLE_CANCEL	0x00000002
#define WTW_WIZARD_FLAG_NO_AUTO_DESTROY	0x00000004
#define WTW_WIZARD_FLAG_SHOW_WINDOW		0x00010000
#define WTW_WIZARD_FLAG_VALID_TITLE		0x00020000
#define WTW_WIZARD_FLAG_SIZEABLE		0x00040000
#define WTW_WIZARD_FLAG_SIZE_MIN		0x00080000
#define WTW_WIZARD_FLAG_SIZE_MAX		0x00100000
#define WTW_WIZARD_FLAG_SIZE_DEFAULT	0x00200000

struct wtwWizardPage
{
	__wtwStdStructDefs(wtwWizardPage);

	int			structSize;

	const wchar_t	* id;			// pageId
	const wchar_t	* wizardId;		// wizardId, both must be valid
	
	WTWFUNCTION		callback;		// wP - wtwWizardEvent*, lP - wtwWizardPage*
	void *			cbData;

	const wchar_t *	title;
	const wchar_t *	summary;
	const wchar_t * insertBeforeId;
	void *			ownerData;

	DWORD			flags;
};

struct wtwWizardEvent
{
	__wtwStdStructDefs(wtwWizardEvent);

	int			structSize;

	const wchar_t	* id;			// wizzard id

	DWORD			event;			// WTW_WIZARD_EVENT_

	const wchar_t *	nextPage;		// for FORWARD and BACKWARD movement, NULL if there is no next page
	const wchar_t *	currPage;

	RECT			targetRect;		// for SHOW and MOVE only
	HWND			hWizardWnd;
	DWORD			bgColor;		// window bgColor
};

#define WTW_WIZARD_EVENT_PAGE_SHOW 1	// we send these to pages only
#define WTW_WIZARD_EVENT_PAGE_HIDE 2	//	
#define WTW_WIZARD_EVENT_PAGE_MOVE 3	//

#define WTW_WIZARD_EVENT_FORWARD	4	// these are send to current page, and if no error is reported to main wizzard procedure
#define WTW_WIZARD_EVENT_BACKWARD	5	// 
#define WTW_WIZARD_EVENT_FINISH	6	// no more pages, user pressed "next" or "finish" was enabled and user peressed it 
#define WTW_WIZARD_EVENT_CANCEL	7	// user called wizzard
#define WTW_WIZARD_EVENT_DESTROY	8	// wizzard is being destroyd (upon succesful completion or cancellation)

#define WTW_WIZARD_EVENT_CREATED		20	// only for main wizzard procedure
#define WTW_WIZARD_EVENT_BUTTON_NEXT	21	// only for main wizzard procedure
#define WTW_WIZARD_EVENT_BUTTON_PREV	22	//
#define WTW_WIZARD_EVENT_BUTTON_CANCEL 23	//
#define WTW_WIZARD_EVENT_BUTTON_FINISH 24	//

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_PAGE_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizardPage*
**		lP - NULL
**
*/

#define WTW_WIZARD_PAGE_ADD	L"WTW/Wizard/PageAdd"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_PAGE_GET
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizardPage*
**		lP - NULL
**
*/

#define WTW_WIZARD_PAGE_GET	L"WTW/Wizard/PageGet"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_PAGE_REMOVE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizardPage*
**		lP - NULL
**
*/

#define WTW_WIZARD_PAGE_REMOVE	L"WTW/Wizard/PageRemove"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_CREATE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizard*
**		lP - NULL
**
** Remarks:
**		Unless specified otherwise, wizzard will be auto-destroyed on
**		completion. 
**
*/

#define WTW_WIZARD_CREATE	L"WTW/Wizard/Create"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_DO_MODAL
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizard*
**		lP - NULL
**
** Remarks:
**		There is no need for calling CREATE/DESTROY functions for modal wizzards
**
*/

#define WTW_WIZARD_DO_MODAL	L"WTW/Wizard/DoModal"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_DESTROY
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizard*
**		lP - NULL
**
*/

#define WTW_WIZARD_DESTROY	L"WTW/Wizard/Destroy"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_SHOW
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizard*
**		lP - NULL
**
*/

#define WTW_WIZARD_SHOW	L"WTW/Wizard/Show"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_GOTO_PAGE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizard*
**		lP - const wchar_t * pageId
**
*/

#define WTW_WIZARD_GOTO_PAGE	L"WTW/Wizard/GoToPage"

/*******************************************************************************
**
** Function: 
**		WTW_WIZARD_ENABLE_BUTTON
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwWizardButton*
**		lP - NULL
**
** Remarks:
**		Next and Prev buttons are managed by framework anyway, usually You
**		don't have to touch these
**
*/

#define WTW_WIZARD_ENABLE_BUTTON	L"WTW/Wizard/EnableButton"

struct wtwWizardButton
{
	__wtwStdStructDefs(wtwWizardButton);

	int			structSize;

	const wchar_t	* id;			// must be always valid

	DWORD			buttonMask;
	DWORD			buttonState;
};

#define WTW_WIZARD_BUTTON_CANCEL	0x0001
#define WTW_WIZARD_BUTTON_PREV		0x0002
#define WTW_WIZARD_BUTTON_NEXT		0x0004
#define WTW_WIZARD_BUTTON_FINISH	0x0008

/*******************************************************************************
**
** Event: 
**		WTW_EVENT_BEFORE_WIZARD_SHOW
**
** Return value: 
**		S_OK
**
** Parameters:
**		wP - wtwWizard* (with valid id)
**		lP - NULL
**
*/

#define WTW_EVENT_BEFORE_WIZARD_SHOW	L"WTW/BeforeWizShow"

/*******************************************************************************
**
** Event: 
**		WTW_EVENT_CONFIG_WIZARD_PAGE_REQUEST
**
** Return value: 
**		S_OK
**
** Parameters:
**		wP - NULL
**		lP - NULL
**
*/

#define WTW_EVENT_CONFIG_WIZARD_PAGE_REQUEST	L"WTW/ConfigWizardPageRequest"

/*******************************************************************************
**
** Constant: 
**		WTW_CONFIG_WIZARD_NAME
**
** Remarks:
**		Name of WTWs configuration wizard. You can add Your own pages to it
**		in WTW_EVENT_CONFIG_WIZARD_PAGE_REQUEST event;
**
*/

#define WTW_CONFIG_WIZARD_NAME L"WTW/ConfigWizard"
