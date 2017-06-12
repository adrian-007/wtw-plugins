/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#ifdef _EXP_DEF_SCK_
#include "libWTW.h"
#else
#include "Core.h"
#endif

namespace wtw 
{

	class LIBWTW_API CConv
	{
	public:
		CConv();

	public:
		static void release(char *p);
		static void release(wchar_t *p);

		// active code page
		static wchar_t * mtow(const char *p);
		static char * wtom(const wchar_t *p);

		// utf-8
		static char * wtou(const wchar_t *p);
		static char * wtou(const wchar_t *p, int size);
		static wchar_t * utow(const char *p);
		static wchar_t * utow(const char *p, int size);
	
		//iso-8559-2
		static char * wtoi85592(const wchar_t *p);
		static wchar_t * i85592tow(const char *p);

		static char * CConv::wtow1252(const wchar_t *in);	// win1252
		static wchar_t * CConv::w1252tow(const char *in);	// win1252

	protected:
		static wchar_t * _tow(const char *in, int cp, int size = -1); // to wide-char
		static char * _tom(const wchar_t *p, int cp, int size = -1); // to multi-byte
	};

};