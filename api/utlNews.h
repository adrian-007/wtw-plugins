/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

struct utlNewsProperty
{
	__wtwStdStructDefs(utlNewsProperty);

	int				structSize;

	const wchar_t * name;		// required
	const wchar_t * value;
	const wchar_t * id;			// locally unique id, required
};

struct utlNewsEntry
{
	__wtwStdStructDefs(utlNewsEntry);		// self-initialize if @C++

	int				structSize;

	const wchar_t * itemId;			// globally unique id, required
	const wchar_t * title;			// required
	const wchar_t * text;
	const wchar_t * hint;
	const wchar_t * sourceName;		// required
	const wchar_t * uri;			// url to show when "display" is 
									// selected or null if ya want callbackFunc to be called
	const wchar_t * iconId;
	
	const wchar_t * callbackFuncName;	// registered (in wtw func table) callback function name
										// coz we dun want troubles when the source plugin
										// vanish
	void		  * itemData;

	DWORD			flags;

	utlNewsProperty*props;				// properties, can be null
	int				propsCount;			// properties count

	QWORD			newsTime;		// 64bit time_t
};

struct utlNewsEntryEx
{
	__wtwStdStructDefs(utlNewsEntryEx);		// self-initialize if @C++

	int				structSize;

	const wchar_t * itemId;			// globally unique id, required
	const wchar_t * title;			// required
	const wchar_t * text;
	const wchar_t * hint;
	const wchar_t * sourceName;		// required
	const wchar_t * uri;			// url to show when "display" is 
									// selected or null if ya want callbackFunc to be called
	const wchar_t * iconId;
	
	const wchar_t * callbackFuncName;	// registered (in wtw func table) callback function name
										// coz we dun want troubles when the source plugin
										// vanish
	void		  * itemData;

	DWORD			flags;

	utlNewsProperty*props;				// properties, can be null
	int				propsCount;			// properties count

	QWORD			newsTime;		// 64bit time_t
	const wchar_t * panelId;
	
	void		*	reserved1;
	void		*	reserved2;
};

struct utlNewsEvent	// wp* in callback
{
	__wtwStdStructDefs(utlNewsEvent);

	int				structSize;
	DWORD			event;

	const wchar_t * itemId;
	void *			itemData;
};

#define UTL_NEWS_EVENT_DISPLAY		0x0001

#define UTL_NEWS_FLAG_HTML_BODY		0x0001

#define UTL_NEWS_ADD_ENTRY L"News/AddEntry" // wP = utlNewsEntry*
#define UTL_NEWS_DEL_ENTRY L"News/DelEntry" // wP = itemId

#define UTL_EVENTS_ADD_ENTRY L"utlEvents/AddEntry" // wP = utlNewsEntryEx*
#define UTL_EVENTS_DEL_ENTRY L"utlEvents/DelEntry" // wP = utlNewsEntryEx*

#define UTL_NEWS_PANEL_ID	L"WTW/NewsCenter"
#define UTL_PHONE_PANEL_ID	L"WTW/PhoneCenter"
#define UTL_SOCIAL_PANEL_ID	L"WTW/SocialCenter"