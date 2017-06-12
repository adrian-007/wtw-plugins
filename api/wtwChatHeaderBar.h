/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"
#include "wtwChatCommandBar.h"

#define WTW_CHB_FUNCT_ADD		L"CHB/Add" // wP - wtwCommandHeaderEntry*
#define WTW_CHB_FUNCT_MOD		L"CHB/Mod" // wP - wtwCommandHeaderEntry*
#define WTW_CHB_FUNCT_DEL		L"CHB/Del" // wP - wtwCommandHeaderEntry*
#define WTW_CHB_FUNCT_CLEAR		WTW_CCB_FUNCT_CLEAR

struct wtwCommandHeaderEntry 
{
	__wtwStdStructDefs(wtwCommandHeaderEntry);

	int				structSize;

	wtwContactDef * pContactData;// do identyfikacji okna uzywamy albo tego, albo tego ponzej
								// to ponizej jest szybsze, ale pWnd
								// jest "valid" tylko w eventach okien rozmowy,
								// z drugiej strony, contactInfo jest "invalid" w _CREATE

	void *			pWnd;		// 
	HANDLE			hInstance;	// do automatycznego usuniêcia przy wyladowaniu wtyczki

	const wchar_t*	itemId;

	WTWFUNCTION		callback;
	void *			cbData;

	void *			reserved;
	const wchar_t*  toolTip;
	const wchar_t*	graphId;

	DWORD			itemFlags;
	void		*	itemData;
};

#define CHB_FLAG_CHANGEICON		0x00000002
#define CHB_FLAG_CHANGETIP		0x00000008
#define CHB_FLAG_CHANGEFLAGS	0x00000010

#define CHB_FLAG_CHECKED		0x00010000

#define CHB_FLAG_MASK			0xFFFF0000

struct wtwCommandHeaderCallback 
{
	__wtwStdStructDefs(wtwCommandHeaderCallback);

	int				structSize;

	const wchar_t * itemId;
	wtwContactDef * contactInfo;
	int				contactCount;
	int				selectedContact;
	POINT			pt; // gdzie pokazac menu na przyklad, screen coordinates

	int				action;
	void		*	doNotTouchPtr;
	void		*	itemData;

	HWND			hWnd;
	const void *	pWnd;
};

#define CHB_ACTION_LCLICK	0x01
#define CHB_ACTION_RCLICK	0x02