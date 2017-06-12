/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "core.h"
#include "..\wtwContactList.h"
#include "BaseObject.h"

namespace wtw {

	class LIBWTW_API CContactDraw : public CBaseObject
	{
	public:
		CContactDraw(wtwContactListItemEvent *pF);

	protected:
		wtwContactListItemEvent *m_pEv;

	public:

		void drawBackground();
		void drawText(
			const wchar_t * text, int x, int y, int cx, int cy, int bMultiline = 0, int bMain = 1);
		void calcTextRect(
			const wchar_t * text, int & x, int & y, int & cx, int & cy, int bMultiline = 0, int bMain = 1);
	};

}