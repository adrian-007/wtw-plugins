#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"
#include <cpp/String.h>

namespace wtw
{
	class LIBWTW_API CUrl
	{
	private:
		CUrl();
		~CUrl();

	public:
		static HRESULT encode(const char *src, char **dst);
		static HRESULT encode(const wchar_t *src, wchar_t **dst); // wide versions dont handle unicode/utf charters

		static HRESULT decode(const char *src, char **dst);
		static HRESULT decode(const wchar_t *src, wchar_t **dst);

		static void release(const char*);
		static void release(const wchar_t*);

		static HRESULT getHostFromUrl(const wchar_t *uri, wtw::CString &host);
	};
}