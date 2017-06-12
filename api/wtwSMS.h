/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

#define SMST_CAPS_LOGIN		0x00000002
#define SMST_CAPS_OWN_NUM	0x00000004
#define SMST_CAPS_PAY		0x00000008
#define SMST_CAPS_DELAY		0x00000010
#define SMST_CAPS_FLASH		0x00000020
#define SMST_CAPS_MMS		0x00000040


#define SMST_OP_TMOBILE		0x00000001
#define SMST_OP_HEYAH		0x00000002
#define SMST_OP_PLUS		0x00000004
#define SMST_OP_SSWOI		0x00000008
#define SMST_OP_MBANK		0x00000010
#define SMST_OP_PLAY		0x00000020
#define SMST_OP_WPMOBILE	0x00000040
#define SMST_OP_ORANGE		0x00000080

#define SMST_OP_FOREGIN		0x10000000
#define SMST_OP_UNKNOWN		0x00000000

#define SMST_OP_ALL			0xFF00FFFF
#define SMST_OP_POLISH		0x0000FFFF

struct	smsTransportProfile
{
	__wtwStdStructDefs(smsTransportProfile);

	int				structSize;

	const wchar_t *	profileName;
	const wchar_t *	gateGUID;
	const wchar_t *	configString;
	
	DWORD			flags;

	const wchar_t *	profileGUID;
};

#define WTW_SMS_PROFILE_FLAG_NO_DELETE	0x0001
#define WTW_SMS_PROFILE_FLAG_TEMPORARY	0x0002

#define WTW_SMSP_ADD	L"WTW/SMS/ProfileAdd"	// wP - smsTransportProfile*
#define WTW_SMSP_DELETE	L"WTW/SMS/ProfileDel"	// wP - smsTransportProfile*

#define ST_S_OK			0x0000	// wszystko ok
#define ST_S_ERROR		0x0001	// b³¹d, ale taki po ktorym mozna ponowic
#define ST_S_FATAL		0x0002	// taki blad po ktorym nie mozna ponowiæ
#define ST_S_ABORT		0x0003	// uzytkownik anulowal

struct smsTransportRegister 
{
	__wtwStdStructDefs(smsTransportRegister);

	int				structSize;
	
	const wchar_t * gateUID;
	const wchar_t * gateName;
	const wchar_t * regURL;

	DWORD			capsFlags;

	int				maxSMSCount;	//	maximum multipart messages allowed
	int				maxSMSCharCount;//	high 16bits contain "first message size", 
									//	low 16bits contain "single message size if multipart"
									//	for normal SMS message, these values are: 160 and 153
	int				maxPhoneCount;	//	maximum receipients count

	WTWFUNCTION		funcSend;		// to bedzie wywo³ane z osobnego w¹tku, pamietaæ o tym, wP - smstsendmessage*
	WTWFUNCTION		funcConf;		// tu powinien byc modalny dialog umieszczony, wP - smstconfigure * (g³owny w¹tek)
	WTWFUNCTION		funcAbort;		// chcemy przerwaæ wysy³kê (g³owny w¹tek)
	WTWFUNCTION		funcCheckNum;	// chcemy sprawdzuc czy ten numer jest dozwolony na tej brace;
};

struct wtwSMSTConfigure
{
	__wtwStdStructDefs(wtwSMSTConfigure);

	int				structSize;
	
	char		  * configString;		// bufor z aktualnym configstringiem, null terminated
	DWORD			configSizeMax;		// maksymalna ilosc danych jaka mzona skopiowac do bufora;
	DWORD			configSizeReal;		// co faktycznie tam umieœliliœmy;

	const wchar_t * gateGUID;
};

struct wtwSMSTSendMessage 
{
	__wtwStdStructDefs(wtwSMSTSendMessage);

	int				structSize;

	const char	  * configString;			// ustawienia profilu
	int				configStringLen;

	const wchar_t * gateUID;				// UID podany przy rejestracji transportu
	
	const wchar_t ** phoneNum;				// tablica wskaznikow do numerow telefonow
	int			  * phoneNumStatus;
	int				phoneNumCount;			// ilosc wskaznikow w powyzszej

	const wchar_t * message;				// tresc wiadomosci

	HWND			hLogWindow;

	DWORD			flags;
	void *			nextHeader;
	int				nextHeaderType;
};

struct wtwSMSTProfileInfo 
{
	__wtwStdStructDefs(wtwSMSTProfileInfo);

	int				structSize;

	const wchar_t * configStringBase64;
	const wchar_t * gateUID;
	const wchar_t * profileName;
};

#define WTW_SMST_REGISTER	L"SMST/Register"
#define WTW_SMST_UNREGISTER	L"SMST/Unregister"	// wystarczy podaæ UID jako wP
#define WTW_SMST_ENUMERATE	L"SMST/Enumerate"

struct wtwSmsShowWindow
{
	__wtwStdStructDefs(wtwSmsShowWindow);

	int structSize;

	const wchar_t * to;
	const wchar_t * text;
	const wchar_t * preferredGateUID;

	DWORD			flags;
};

#define WTW_SMST_SHOWWINDOW L"SMST/ShowWindow"