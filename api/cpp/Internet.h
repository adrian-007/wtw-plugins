/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"
#include "File.h"
#include "String.h"
#include "BaseObject.h"

namespace wtw 
{
	class LIBWTW_API CInternet : public CBaseObject
	{
	public:
		CInternet();
		virtual ~CInternet();

		enum
		{
			NET_FLAG_RELOAD				= 0x0001,
			NET_FLAG_NO_AUTO_REDIRECT	= 0x0002,
		} flags;

	public:
		void* m_hNet;

	public:
		HRESULT open(DWORD flags = 0, const wchar_t *userAgent = WTW_DEFAULT_USER_AGENT);
		HRESULT open(const wchar_t * proxyHost, int proxyPort, DWORD flags = 0, const wchar_t *userAgent = WTW_DEFAULT_USER_AGENT);
		HRESULT close();
		HRESULT attach(void* hNet);
		void* detach();
		HRESULT isOpen();
	};

	class LIBWTW_API CInternetHttp : public CInternet
	{
	public:
		struct httpPairW
		{
			wtw::CString name;
			wtw::CString value;
		};

	protected:
		std::deque <httpPairW> m_cookies;
		std::deque <httpPairW> m_headers;

		void *	m_postDataPtr;
		DWORD	m_postDataLen;

	public:
		CInternetHttp();
		virtual ~CInternetHttp();

	protected:
		HRESULT downloadFile(const wchar_t *fullUri, void *ptr, int type, const wchar_t *verb);
		HRESULT downloadFile(const wchar_t *fullUri, void *ptr, int type, const wchar_t *verb, wtw::CBuffer & postData);

	public:
		HRESULT addRequestHeader(const wchar_t * name, const wchar_t * value);
		HRESULT removeRequestHeader(const wchar_t * name);
		HRESULT clearRequestHeaders();

		HRESULT getResponseHeader(const wchar_t * name, wtw::CString & value);
		HRESULT setPostDataPtr(void *ptr, DWORD len);

		HRESULT cookieAddOrEdit(const wchar_t * name, const wchar_t * value);
		const wchar_t * cookieGet(const wchar_t * name);

		HRESULT cookiesExport(wtw::CString & buffer);
		HRESULT cookiesImport(wchar_t *data, int bClear = 1);

		HRESULT cookieRemove(const wchar_t * name);
		HRESULT cookieClear();
		HRESULT downloadFile(const wchar_t *fullUri, const wchar_t *localFilePath, const wchar_t *verb = L"GET");
		HRESULT downloadFile(const wchar_t *fullUri, wtw::CBuffer & buffer, const wchar_t *verb = L"GET");
		HRESULT makeHttpRequest( 
			const wchar_t * verb, const wchar_t * url, wtw::CFile & openedFile,
			const wchar_t * referer = NULL, DWORD flags = wtw::CInternet::NET_FLAG_RELOAD, 
			const wchar_t * getHeaderName = NULL, wchar_t * pHeaderBuffer = NULL, DWORD iHeaderBuffer = 0, 
			int * pResponseCode = NULL);
		HRESULT makeHttpRequest( 
			const wchar_t * verb, const wchar_t * url, wtw::CBuffer & buffer,
			const wchar_t * referer = NULL, DWORD flags = wtw::CInternet::NET_FLAG_RELOAD, 
			const wchar_t * getHeaderName = NULL, wchar_t * pHeaderBuffer = NULL, DWORD iHeaderBuffer = 0, 
			int * pResponseCode = NULL);

	protected:
		HRESULT prepareHttpRequest(
			const wchar_t * verb, const wchar_t * remotePath, 
			const wchar_t * referer, DWORD flags, 
			const wchar_t * getHeaderName, wchar_t * pHeaderBuffer, DWORD iHeaderBuffer, 
			int * pResponseCode = NULL);
		HRESULT finishHttpRequest(DWORD flags, 
			int * pResponseCode = NULL, const wchar_t * getHeaderName = NULL, 
			wchar_t * pHeaderBuffer = NULL, DWORD iHeaderBuffer = 0);
		static size_t _writeDataToFile( char *b, size_t s, size_t nmemb, void *userdata);
		static size_t _writeDataToBuffer( char *b, size_t s, size_t nmemb, void *userdata);
		static size_t _writeHeader( char *b, size_t s, size_t nmemb, void *userdata);

	protected:
		std::deque <httpPairW> m_rqHeaders;
	};
}