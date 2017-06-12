/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"
#include "wtwContact.h"

/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_REGISTER_FEED
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwGeolocFeedMech*
**		lP - NULL
**
*/

#define WTW_GEOLOC_REGISTER_FEED L"WTW/GeoLoc/RegisterFeed"

struct wtwGeolocFeedMech
{
	__wtwStdStructDefs(wtwGeolocFeedMech);

	int structSize;

	const wchar_t * mechId;
	const wchar_t * mechName;
};

/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_UNREGISTER_FEED
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - const wchar_t* id
**		lP - NULL
**
*/

#define WTW_GEOLOC_UNREGISTER_FEED L"WTW/GeoLoc/UnregisterFeed"

/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_FEED_DATA
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwGeolocData* 
**		lP - NULL
**
*/

#define WTW_GEOLOC_FEED_DATA L"WTW/GeoLoc/FeedData"

struct wtwGeolocData
{
	__wtwStdStructDefs(wtwGeolocData);

	int structSize;

	const wchar_t *mechId;

	double	lon;	// deg
	double	lat;	// deg
	double	alt;	// m.n.p.m
	double	spd;	// km/h
	double  accuracy; // m
	
	double  reserved1;
	double  reserved2;

	int		sats;			// sattelites used to obtain fix

	QWORD	time;			// GPS time
	QWORD	timeOfTheFix;	// required, time when fix was aqquired, UTC

	DWORD	source;
	DWORD	flags;
	DWORD	status;
};

#define WTW_GEOLOC_DATA_FLAG_VALID_LONLAT	0x00000001	// longitude & lattitude fields are valid
#define WTW_GEOLOC_DATA_FLAG_VALID_ACCURACY	0x00000002
#define WTW_GEOLOC_DATA_FLAG_VALID_ALT		0x00000004
#define WTW_GEOLOC_DATA_FLAG_VALID_SPD		0x00000008
#define WTW_GEOLOC_DATA_FLAG_VALID_SAT		0x00000010
#define WTW_GEOLOC_DATA_FLAG_VALID_TIME		0x00000020

#define WTW_GEOLOC_DATA_STATUS_FIX_OK	1
#define WTW_GEOLOC_DATA_STATUS_NO_FIX	2

#define WTW_GEOLOC_DATA_SOURCE_UNKNOWN	0
#define WTW_GEOLOC_DATA_SOURCE_GPS		1
#define WTW_GEOLOC_DATA_SOURCE_NETWORK	2
#define WTW_GEOLOC_DATA_SOURCE_MANUAL	3

/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_GET_LAST_LOCATION
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwGeolocData* 
**		lP - NULL
**
** Description:
**		If mechId is NULL, function will return data from last valid location update
**		If mechId is specified, function will query specified mechanism and return data regardles of fix validity, 
**			if mechanism is not found - error is returned
**
*/

#define WTW_GEOLOC_GET_LAST_LOCATION L"WTW/GeoLoc/GetLastLocation"

/*******************************************************************************
**
** Event: 
**		WTW_EVENT_GEOLOC_LOCATION
**
** Return value: 
**		S_OK
**
** Parameters:
**		wP - wtwGeolocData* 
**		lP - NULL
**
** Description:
**		Event fired when location is updated by selected mechanism
**
*/

#define WTW_EVENT_GEOLOC_LOCATION L"WTW/Event/GeoLoc/Update"








/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_GET_CONTACT_LOCATION
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwGeolocData* 
**		lP - wtwContactDef*
**
*/

#define WTW_GEOLOC_GET_CONTACT_LOCATION L"WTW/GeoLoc/GetLastCtLocation"

/*******************************************************************************
**
** Function: 
**		WTW_GEOLOC_SET_CONTACT_LOCATION
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwGeolocData* or NULL if you want to remove location data
**		lP - wtwContactDef*
**
*/

#define WTW_GEOLOC_SET_CONTACT_LOCATION L"WTW/GeoLoc/SetLastCtLocation"

/*******************************************************************************
**
** Event: 
**		WTW_EVENT_GEOLOC_CONTACT_LOCATION
**
** Return value: 
**		S_OK
**
** Parameters:
**		wP - wtwGeolocData* - may be NULL
**		lP - wtwContactDef*
**
** Description:
**		Event fired when contact location is updated
**
*/

#define WTW_EVENT_GEOLOC_CONTACT_LOCATION L"WTW/Event/GeoLoc/CtUpdate"
