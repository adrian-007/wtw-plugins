/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"
#include "BaseObject.h"

namespace wtw
{
	class LIBWTW_API CBuffer : public CBaseObject
	{
	public:
		CBuffer(void);
		virtual ~CBuffer(void);

	private:
		BYTE *	m_pBuffer;

		DWORD	m_iAllocSize;
		DWORD	m_iDataSize;

	public:
		int		getLength() const { return m_iDataSize; };
		int		replaceBuffer(BYTE *pData, DWORD dataSize);
		BYTE *	getBuffer(DWORD &dataSize) const;
		BYTE *	getBuffer() const;
	
		int		push(BYTE *pData, DWORD dataSize);
		int		popHead(DWORD size);

		void	disposeBuffers();
		void	resize(DWORD size);

		BYTE *	detach(int & datalen, int * realLength);

	public:
		static BYTE * alignedRealloc(void *p, DWORD & allocated, DWORD size, DWORD used);
		WTW_PTR dump(const wchar_t *file) /*override*/;
	};

};