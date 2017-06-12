/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#include "resource.h"

struct protocolInfo
{
	std::wstring name;
	std::wstring netClass;
	int netId;
};

class ProtoDialog : public CDialogImpl<ProtoDialog>
{
public:
	enum { IDD = IDD_PROTO_DIALOG };

	ProtoDialog();
	~ProtoDialog();

	BEGIN_MSG_MAP(ProtoDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_CLOSE, onClose)
		COMMAND_HANDLER(IDC_CLEAR, BN_CLICKED, OnBnClickedClear)
		COMMAND_HANDLER(IDC_PROTOCOLS, CBN_SELENDOK, OnCbnSelendokProtocols)
	END_MSG_MAP()

private:
	static WTW_PTR onProtoEvent(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwProtocolEvent* evn = (wtwProtocolEvent*)wParam;

		if(evn != nullptr && (evn->event == WTW_PEV_RAW_DATA_RECV || evn->event == WTW_PEV_RAW_DATA_SEND) && evn->type == WTW_PEV_TYPE_AFTER)
		{
			switch(evn->event)
			{
			case WTW_PEV_RAW_DATA_RECV:
				{
					reinterpret_cast<ProtoDialog*>(ptr)->onRawData(evn->netClass, evn->netId, true, reinterpret_cast<wtwRawDataDef*>(lParam));
					break;
				}
			case WTW_PEV_RAW_DATA_SEND:
				{
					reinterpret_cast<ProtoDialog*>(ptr)->onRawData(evn->netClass, evn->netId, false, reinterpret_cast<wtwRawDataDef*>(lParam));
					break;
				}
			}
		}

		return 0;
	}

	void onRawData(const wchar_t* netClass, int netId, bool incoming, wtwRawDataDef* data);
	void install();
	void uninstall();
	protocolInfo* getCurrentProto() const;

	void append(std::wstring text, COLORREF color, bool bold = false);
	std::wstring fromBinary(const char* buf, int len) const;

	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokProtocols(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CComboBox ctrlProtocols;
	CButton ctrlBinary;
	CRichEditCtrl ctrlData;
	
	CFont fontData;
	HFONT font;

	COLORREF clrSend;
	COLORREF clrRecv;
	COLORREF clrBack;

	HANDLE protoHook;
};
