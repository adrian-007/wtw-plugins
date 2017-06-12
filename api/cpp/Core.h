/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef LIBWTW_API
#ifdef LIBWTW_EXPORTS
#define LIBWTW_API __declspec(dllexport)
#else
#define LIBWTW_API __declspec(dllimport)
#endif
#endif

#include "..\wtwPlugin.h"
#include "..\wtwLog.h"
#include <deque>
#include <wchar.h>
#include "CriticalSection.h"

namespace wtw 
{

	class LIBWTW_API CCore : public WTWFUNCTIONS
	{
	public:
		CCore(WTWFUNCTIONS *pF);
		virtual ~CCore();

	public:
		WTW_PTR showAbout(
				const wchar_t *	dialogTitle,
				const wchar_t *	headerLine1,
				const wchar_t *	headerLine2,
				const wchar_t *	headerLine3,
				const wchar_t *	mainText,
				HICON			hIcon,
				HBITMAP			hBitmap);

		void log(const wchar_t *mdl, 
				const wchar_t *msg, 
				int lvl = WTW_LOG_LEVEL_NORMAL
				);
		void logCritical(const wchar_t *msg, 
						const wchar_t * graphId = NULL
					);
		void logFormatted(int lvl, 
					  const wchar_t* mdl, 
					  const wchar_t* format, ...
					  );

		HWND getMainHWND();

		const wchar_t *getProfileDir();
		const wchar_t *getProfileName();

		wchar_t * getDirectory(int dirId);

		HANDLE installProtocolFunction(
						const wchar_t * netClass,
						int				netId,
						const wchar_t * funcName,
						WTWFUNCTION		funcAddr,
						void *			cbData
						);
	};
};