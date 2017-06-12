/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Buffer.h"

namespace wtw
{
	class LIBWTW_API CString : protected CBuffer
	{
	public:
		CString(void);
		CString(const CString &);
		CString(const wchar_t *);
		virtual ~CString(void);

	public:
		operator const wchar_t*() const;
		operator WTW_PARAM () const;

		CString & operator += (wchar_t*);
		CString & operator += (const wchar_t*);

		CString & operator += (const wchar_t);

		bool operator == (const CString &r);
		bool operator == (const wchar_t *r);

		CString & operator = (const wchar_t*); 
		CString & operator = (const CString &);

		int		append(wchar_t *pData, DWORD sizeInChars);
		int		append(wchar_t *pData);

		bool	compare(const wchar_t *str);
		bool	compareNoCase(const wchar_t *str);

		void	clear();

		const wchar_t * c_str() const;

		void makeLower();
		void makeUpper();

		DWORD getLength() const;
		int format(const wchar_t *fmt, ...);

		wchar_t * detach();
	};


};