/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Core.h"
#include "String.h"

#define WTW_CURRENT_BROWSER_INTERFACE_VERSION 8

namespace wtw 
{
	class LIBWTW_API CBrowserObject
	{
	public:
		enum _type
		{
			T_UNKNOWN	= 0x0000L,
			T_TEXT		= 0x0001L,
			T_LINK		= 0x0002L,
			T_SELECTION = 0x0003L,
			T_IMAGE		= 0x0004L,
		};

	public:
		CBrowserObject();
		virtual ~CBrowserObject();

	public:
		const wchar_t*	m_objectData;
		DWORD			m_objectType;
	};

	class LIBWTW_API CBrowserEventSink
	{
	public:
		CBrowserEventSink();
		virtual ~CBrowserEventSink();

	public:

		virtual WTW_PTR onRButtonUp(
			const POINT &pt,
			CBrowserObject *pObjects,
			DWORD objectCount
			);

		virtual WTW_PTR onLinkNavigate(
			const POINT &ptClick,
			const wchar_t *pLinkTarget,
			CBrowserObject *pObjects,
			DWORD objectCount
			);

		virtual WTW_PTR onLButtonUp(
			const POINT &pt,
			CBrowserObject *pObjects,
			DWORD objectCount
			);

		virtual WTW_PTR onMButtonUp(
			const POINT &pt,
			CBrowserObject *pObjects,
			DWORD objectCount
			);

		virtual WTW_PTR onLDblClick(
			const POINT &pt,
			CBrowserObject *pObjects,
			DWORD objectCount
			);

		virtual WTW_PTR onDocumentCompleted();

		virtual WTW_PTR onBeforeNavigate(
			const wchar_t *pLinkTarget,
			const wchar_t *pTarget,
			DWORD flags
			);
	};

	class LIBWTW_API CBrowserHtmlElement
	{
	public:
		// v4
		virtual WTW_PTR getAttribute(const wchar_t * name, wtw::CString & value) = 0;
		virtual WTW_PTR setAttribute(const wchar_t * name, const wchar_t * value) = 0;

		virtual WTW_PTR setInnerHtml(const wchar_t * html) = 0;
		virtual WTW_PTR getInnerHtml(wtw::CString & html) = 0;

		virtual WTW_PTR getParent(CBrowserHtmlElement **pel) = 0;
		virtual WTW_PTR getSibling(CBrowserHtmlElement **pel) = 0;

		virtual WTW_PTR getTagName(wtw::CString & name) = 0;
		virtual WTW_PTR remove() = 0;

		virtual void release() = 0; // delete this;

		//v6
		virtual WTW_PTR enable(int bEnable) = 0;
		virtual WTW_PTR isEnabled(int & bEnabled) = 0;
		virtual WTW_PTR getState(DWORD stateMask, DWORD & state) = 0;
		virtual WTW_PTR setState(DWORD stateMask, DWORD state) = 0;

		//v7
		virtual WTW_PTR getRect(RECT & rect, DWORD flags) = 0;
		virtual WTW_PTR update(DWORD flags) = 0;

	public:
		enum _state
		{
			S_UNKNOWN = 0x0000L,
			S_VISIBLE = 0x0001L,
			S_ENABLED = 0x0002L,
		};

	};

	class LIBWTW_API CBrowserInterface
	{
	public:
		enum
		{
			WTW_BIF_ADD_AT_END		=	0x0001,	// add at begining of the tag
			WTW_BIF_ADD_AT_BEGINIG	=	0x0002,	// add before end of the tag
			WTW_BIF_REPLACE_ONLY	=	0x0004,	// don't add element if it's not exists
			WTW_BIF_REPLACE_OR_ADD	=	0x0008, // add or replace
			WTW_BIF_OUTER			=	0x0010, // replace whole tag (normally function replaces inner html) 
		};

	public:
		CBrowserInterface();
		virtual ~CBrowserInterface();

	protected:
		CBrowserEventSink *m_pSink;

	public:
		virtual WTW_PTR getInterfaceVersion() = 0; // return WTW_CURRENT_BROWSER_INTERFACE_VERSION

		virtual WTW_PTR init(
			HWND hParent, 
			int x, int y, int cx, int cy, 
			CBrowserEventSink *pSink) = 0;

		virtual WTW_PTR terminate() = 0; // returns HRESULT
		virtual WTW_PTR getBrowserHWND(HWND &hWnd) = 0; // returns HRESULT

		virtual WTW_PTR clear(const wchar_t *htmlEnvelope, const wchar_t *pBasePath) = 0; // returns HRESULT
		virtual WTW_PTR addChatText(const wchar_t *html) = 0; // returns HRESULT
		virtual WTW_PTR setText(const wchar_t *html) = 0; // returns HRESULT
		virtual WTW_PTR navigate(const wchar_t *pUri, int bForceSameWindow = 0) = 0; // returns HRESULT
		virtual WTW_PTR getSelectedText(wchar_t **text, DWORD flags) = 0; // returns HRESULT
		virtual WTW_PTR freePtr(wchar_t *ptr) = 0; // returns HRESULT
		virtual WTW_PTR selectAll() = 0; // returns HRESULT

		virtual WTW_PTR scrollDown() = 0; // returns HRESULT
		virtual WTW_PTR scrollTop() = 0; // returns HRESULT
		virtual WTW_PTR scrollToObject(const wchar_t *objId, DWORD flags) = 0; // returns HRESULT
		virtual WTW_PTR isScrollDown() = 0; // returns 1 (true) or 0 (false)
		
		virtual WTW_PTR showWindow(int bShow) = 0; // returns HRESULT
		virtual WTW_PTR moveWindow(int x, int y, int cx, int cy) = 0; // returns HRESULT
		virtual WTW_PTR getWindowRect(long & x, long & y, long & cx, long & cy) = 0; // returns HRESULT

		virtual void release() = 0; //delete this;

		// version 3
		virtual WTW_PTR addOrReplaceHtml(const wchar_t *html, const wchar_t *elementId, DWORD flags) = 0; // returns HRESULT, flags = WTW_BIF_*
		
		// version 4
		virtual WTW_PTR findElement(const POINT & pt, wtw::CBrowserHtmlElement ** pel) = 0;
		virtual WTW_PTR findElement(const wchar_t *id, wtw::CBrowserHtmlElement ** pel) = 0;
		virtual WTW_PTR getRootElement(wtw::CBrowserHtmlElement **pel) = 0;

		// version 8
		virtual WTW_PTR setUA(const wchar_t *ua) = 0;
		virtual WTW_PTR getInterfacePtr(void **ptr) = 0;
	};
};