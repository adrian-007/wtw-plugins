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

#include "stdinc.hpp"
#include "ProtoDialog.hpp"

#include "../utils/windows.hpp"
#include "../utils/text.hpp"
#include <boost/format.hpp>

ProtoDialog::ProtoDialog() : protoHook(nullptr), clrSend(RGB(230, 230, 230)), clrRecv(RGB(170, 170, 170)), clrBack(RGB(70, 70, 70))
{
}

ProtoDialog::~ProtoDialog()
{
}

void ProtoDialog::onRawData(const wchar_t* netClass, int netId, bool incoming, wtwRawDataDef* data)
{
	protocolInfo* p = getCurrentProto();

	if(p != nullptr)
	{
		if(wcscmp(p->netClass.c_str(), netClass) == 0 && p->netId == netId)
		{
			// okay
			bool bin = ctrlBinary.GetCheck() == BST_CHECKED || ((data->flags & WTW_RAW_FLAG_BIN) != 0);
			COLORREF clr = incoming ? clrRecv : clrSend;

			if(bin == true)
			{
				append(fromBinary(data->pData, data->pDataLen), clr);
			}
			else
			{
				if(data->flags & WTW_RAW_FLAG_UNI)
				{
					append(std::wstring(reinterpret_cast<const wchar_t*>(data->pData), data->pDataLen / sizeof(wchar_t)) + L'\n', clr);
				}
				else if(data->flags & WTW_RAW_FLAG_UTF)
				{
					append(utils::text::toWide(std::string(data->pData, data->pDataLen), CP_UTF8) + L'\n', clr);
				}
				else if(data->flags & WTW_RAW_FLAG_TEXT)
				{
					append(utils::text::toWide(std::string(data->pData, data->pDataLen), CP_ACP) + L'\n', clr);
				}
			}
		}
	}
}

void ProtoDialog::install()
{
	uninstall();
	protoHook = wtw->evHook(WTW_ON_PROTOCOL_EVENT, ProtoDialog::onProtoEvent, (void*)this);
}

void ProtoDialog::uninstall()
{
	if(protoHook != nullptr)
	{
		wtw->evUnhook(protoHook);
		protoHook = nullptr;
	}
}

protocolInfo* ProtoDialog::getCurrentProto() const
{
	int pos = ctrlProtocols.GetCurSel();

	if(pos > 0)
	{
		return reinterpret_cast<protocolInfo*>(ctrlProtocols.GetItemData(pos));
	}

	return nullptr;
}

void ProtoDialog::append(std::wstring text, COLORREF color, bool bold /*= false*/)
{
	ctrlData.SetRedraw(FALSE);

	SCROLLINFO si = { 0 };
	POINT pt = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	ctrlData.GetScrollInfo(SB_VERT, &si);
	ctrlData.GetScrollPos(&pt);

	{
		std::wstring::size_type i = 0;
		while((i = text.find(L'\r', i)) != std::wstring::npos)
		{
			text.erase(i, 1);
		}
	}

	CHARFORMAT2W cf;

	cf.cbSize			= sizeof(cf);
	cf.dwMask			= CFM_COLOR;
	cf.dwEffects		= ~CFE_AUTOCOLOR;

	if(bold == true)
	{
		cf.dwMask		|= CFM_BOLD;
		cf.dwEffects	|= CFE_BOLD;
	}

	cf.crTextColor = color;

	int start = ctrlData.GetTextLengthEx(GTL_NUMCHARS);
	ctrlData.InsertText(start, text.c_str());

	ctrlData.SetSel(start, start + text.length());
	ctrlData.SetSelectionCharFormat(cf);
	ctrlData.SetSelNone();

	ctrlData.SetScrollPos(&pt);

	ctrlData.SetRedraw(TRUE);
	ctrlData.InvalidateRect(nullptr);
}

std::wstring ProtoDialog::fromBinary(const char* buf, int len) const
{
	if(len < 0)
	{
		len = strlen(buf);
	}

	std::string hex;
	std::string chr;
	std::string ret;

	int off = 0;
	int i = 0;
	int line = 0;

	for(i = 0; i < len; ++i)
	{
		hex += boost::str(boost::format("%|02X| ") % (int)(unsigned char)buf[i]);

		if((unsigned char)buf[i] >= 0x20 && (unsigned char)buf[i] <= 0x7E)
		{
			chr += buf[i];
		}
		else
		{
			chr += '.';
		}

		if(++off == 16)
		{
			ret += boost::str(boost::format("%|04X| : %-48s: %-16s\n") % line % hex % chr);
			hex.clear();
			chr.clear();

			off = 0;
			line += 16;
		}
	}

	if(hex.empty() == false)
	{
		ret += boost::str(boost::format("%|04X| : %-48s: %-16s\n") % line % hex % chr);
	}

	ret += std::string(73, '-') + '\n';

	return utils::text::toWide(ret);
}

LRESULT ProtoDialog::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	font = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, font);

	ctrlProtocols.Attach(GetDlgItem(IDC_PROTOCOLS));
	ctrlBinary.Attach(GetDlgItem(IDC_BINARY));
	ctrlData.Attach(GetDlgItem(IDC_DATA));

	if(fontData.CreateFontW(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, MONO_FONT, L"Courier New") == nullptr)
	{
	}

	ctrlProtocols.AddString(L"--- Disabled ---");

	ctrlData.SetFont(fontData);
	ctrlData.SetBackgroundColor(clrBack);
	ctrlData.SetTargetDevice(nullptr, 0);

	int size = (int)wtw->fnCall(WTW_PROTO_FUNC_ENUM, 0, -1);

	if(size > 0)
	{
		wtwProtocolInfo* nfo = new wtwProtocolInfo[size];

		if(wtw->fnCall(WTW_PROTO_FUNC_ENUM, (WTW_PARAM)nfo, (WTW_PARAM)size) != 0)
		{
			for(int i = 0; i < size; ++i)
			{
				if(nfo[i].guid == 0 || wcslen(nfo[i].guid) < 10)
				{
					continue;
				}

				if(nfo[i].flags & WTW_PROTO_FLAG_PSEUDO)
				{
					continue;
				}

				protocolInfo* p = new protocolInfo;
				p->name = nfo[i].name;
				p->netClass = nfo[i].netClass;
				p->netId = nfo[i].netId;

				int item = ctrlProtocols.AddString(p->name.c_str());
				ctrlProtocols.SetItemData(item, (DWORD_PTR)p);
			}
		}

		delete[] nfo;
		nfo = 0;
	}

	ctrlProtocols.SetCurSel(0);

	//append(fromBinary("\x01ala \xffma kota asd adas asasd as fasfasfasdf asf as fasf asf asf /\\", -1), RGB(255, 255, 255));

	return 0;
}

LRESULT ProtoDialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	uninstall();

	for(int i = 1; i < ctrlProtocols.GetCount(); ++i)
	{
		protocolInfo* p = reinterpret_cast<protocolInfo*>(ctrlProtocols.GetItemData(i));
		
		if(p != nullptr)
		{
			delete p;
		}
	}

	ctrlProtocols.Detach();
	ctrlBinary.Detach();
	ctrlData.Detach();

	DeleteObject(font);
	fontData.DeleteObject();

	return 0;
}

LRESULT ProtoDialog::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_HIDE);
	return 0;
}

LRESULT ProtoDialog::OnBnClickedClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ctrlData.SetSelAll();
	ctrlData.ReplaceSel(L"");
	ctrlData.SetSelNone();
	return 0;
}

LRESULT ProtoDialog::OnCbnSelendokProtocols(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	protocolInfo* p = getCurrentProto();

	if(p != nullptr)
	{
		install();
		SetWindowText((L"Active: " + p->name).c_str());
	}
	else
	{
		uninstall();
		SetWindowText(L"Inactive");
	}

	return 0;
}
