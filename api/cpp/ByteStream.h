/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Buffer.h"
#include <cpp/string.h>

namespace wtw
{
	class LIBWTW_API CByteStream : public CBuffer
	{
	public:
		CByteStream(void);
		CByteStream(void *b, size_t len);
		virtual ~CByteStream(void);

	protected:
		int m_position;

	public:
		void rewind();
		WTW_PTR seek(int offset);
		WTW_PTR eof();

	public:
		WTW_PTR getByte(BYTE & i);
		WTW_PTR getInt(int & i);
		WTW_PTR getDword(DWORD & i);
		WTW_PTR getLong(__int64 & i);
		WTW_PTR getVarStringUtf8(wtw::CString & str, int lenLen = 4);

	public:
		void putInt(const int val);
		void putByte(const char val);
		void putData(const void *data, int len);
		void putLong(const __int64 val);
		void putString(const char* val);
		void putString(const wchar_t* val);
		void putStringUtf8(const wchar_t* val);
		void putVarString(const wchar_t* val, int lenLen = 4);
		void putVarStringUtf8(const wchar_t* val, int lenLen = 4);
		void putVarString(const char* val, int lenLen = 4);
	};
};