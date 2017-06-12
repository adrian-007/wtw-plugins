#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"

namespace wtw
{
	class LIBWTW_API CBase64
	{
	public:
		CBase64();
		~CBase64();

	public:
		static int encode(const BYTE *in, size_t inLen, char **out);
		static int encode(const BYTE *in, size_t inLen, wchar_t **out);

		static int decode(const char *in, size_t inLen, BYTE **out);
		static int decode(const wchar_t *in, size_t inLen, BYTE **out);

		static void release(wchar_t *);
		static void release(char *);
		static void release(BYTE *);
	};
}