#pragma once

#include "Core.h"
#include "String.h"
#include <wtwGraph.h>
#include <cpp/BaseObject.h>

namespace wtw
{
	class LIBWTW_API CGraphics : public CBaseObject
	{
	public:
		friend class CGraphicsFactory;
	protected:
		CGraphics(CCore *pF, const wchar_t * id);
		CGraphics(CCore *pF, const wtwGraphics & graph);
		CCore *m_pF;
		CString m_graphId;
		void *m_gPointer;
		int m_cx, m_cy;

	public:
		virtual ~CGraphics();
		int getWidth();
		int getHeight();

	};

	class LIBWTW_API CGraphicsFactory abstract
	{
	public:
		static HRESULT loadPng(const wchar_t * graphId, const wchar_t * file, int bLoadFromDefLoc = 1, CGraphics **pGraph = NULL);
		static HRESULT loadPng(const wchar_t * graphId, HINSTANCE hInst, int resourceId, CGraphics **pGraph = NULL);
	};
};