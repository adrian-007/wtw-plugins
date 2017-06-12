/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"
#include "wtwContact.h"

/*******************************************************************************
**
** Function: 
**		WTW_CONTACT_FAVOURITE_CHECK
**
** Return value: 
**		S_OK (is faved), S_FALSE (is not) or E_*
**
** Parameters:
**		wP - wtwContactDef*
**		lP - NULL
**
*/

#define WTW_CONTACT_FAVOURITE_CHECK		L"WTW/FavCheck"

/*******************************************************************************
**
** Function: 
**		WTW_CONTACT_FAVOURITE_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactDef*
**		lP - NULL
**
*/

#define WTW_CONTACT_FAVOURITE_ADD		L"WTW/FavAdd"

/*******************************************************************************
**
** Function: 
**		WTW_CONTACT_FAVOURITE_REMOVE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactDef*
**		lP - NULL
**
*/

#define WTW_CONTACT_FAVOURITE_REMOVE	L"WTW/FavRemove"