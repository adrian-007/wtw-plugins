/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"
#include "wtwContact.h"

struct wtwMetacontact
{
	__wtwStdStructDefs(wtwMetacontact);

	int				structSize;

	const wchar_t*	guid;
	const wchar_t*	name;
	const wchar_t*	group;
	DWORD			flags;

	wtwContactDef * pContacts;
	int				iContacts;
};

/*******************************************************************************
**
** Function: 
**		WTW_METACONTACT_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwMetaContact*
**		lP - NULL
**
*/

#define	WTW_METACONTACT_ADD		L"WTW/MetacontactAdd"

/*******************************************************************************
**
** Function: 
**		WTW_METACONTACT_CHECK
**
** Return value: 
**		S_OK (is in meta), S_FALSE (is not) or E_*
**
** Parameters:
**		wP - wtwContactDef*
**		lP - const wchar_t** (or NULL if we dont care about meta UUID)
**
*/

#define	WTW_METACONTACT_CHECK		L"WTW/MetacontactCheck"

/*******************************************************************************
**
** Function: 
**		WTW_METACONTACT_GET
**
** Return value: 
**		filled pContact data count (or needed pContact count
**		if pContacts nad iContacts are null) or E_*
**		wtwMetaContact.uuid has to be always filled;
**
** Parameters:
**		wP - wtwMetaContact*
**		lP - NULL
**
*/

#define	WTW_METACONTACT_GET		L"WTW/MetacontactGet"